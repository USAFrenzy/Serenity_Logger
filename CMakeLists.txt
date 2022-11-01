cmake_minimum_required(VERSION 3.14)

project(Serenity_Sandbox)

include(cmake/PreventInSrcBuilds.cmake)
include(cmake/CPM.cmake)
include(cmake/Options.cmake)
include(cmake/Incremental_Build.cmake)

get_and_increment_version()
message("-- ${PROJECT_NAME} Building Version: ${SERENITY_FULL_VERSION}")

set(VENDOR_FILE_LOCATION ${CMAKE_BINARY_DIR}/_deps)
if (USE_STDFMT
    AND NOT USE_FMTLIB
    AND NOT USE_NATIVEFMT
)
    set(FORMATTER_BACKEND USE_STD_FMT)
elseif (
    USE_FMTLIB
    AND NOT USE_STDFMT
    AND NOT USE_NATIVEFMT
)
    cpmaddpackage(
        NAME
        fmt
        GITHUB_REPOSITORY
        fmtlib/fmt
        GIT_TAG
        9.1.0
        OPTIONS
        "FMT_PEDANTIC OFF"
        "FMT_WERROR OFF"
        "FMT_DOC OFF"
        "FMT_INSTALL OFF"
        "FMT_TEST OFF"
        "FMT_FUZZ OFF"
        "FMT_CUDA_TEST OFF"
        "FMT_OS ON"
        "FMT_MODULE OFF"
        "FMT_SYSTEM_HEADERS OFF"
    )
    set(GH_FMT_INCLUDE_DIR ${VENDOR_FILE_LOCATION}/fmt-src/include)
    set(FMT fmt::fmt)
    set(FORMATTER_BACKEND USE_FMT_LIB)
elseif (
    USE_NATIVEFMT
    AND NOT USE_STDFMT
    AND NOT USE_FMTLIB
)
    if (NOT
        EXISTS
        ${VENDOR_FILE_LOCATION}/argfmt-src
    )
        cpmaddpackage(
            NAME
            argfmt
            GITHUB_REPOSITORY
            USAFrenzy/ArgFormatter
            VERSION
            1.0.2
            OPTIONS
            "BUILD_COMPILED_LIB ON"
            "BUILD_SANDBOX OFF"
            "BUILD_TESTS OFF"
            "BUILD_ALL OFF"
        )
        # specific definition that needs to be applied for ArgFormatter to see the correct include
        # path
        add_compile_definitions(COMPILED_LIBRARY)
        set(ARG_FMT ArgFormatter_Lib)
    endif ()
    set(FORMATTER_BACKEND USE_BUILT_IN_FMT)
    set(ARGFMT_INCLUDE_DIRS ${VENDOR_FILE_LOCATION}/argfmt-src/include)
elseif (
    NOT USE_NATIVEFMT
    AND NOT USE_STDFMT
    AND NOT USEFMTLIB
)
    message(
        FATAL_ERROR
            "No Formatting Options Are Enabled - Please Enable One Formatting Option And Re-run The Build Command. CMAKE Will Now Exit The Build Process."
    )
else ()
    message(
        FATAL_ERROR
            "Multiple Formatting Options Are Enabled - Please Only Enable One Formatting Option And Re-run The Build Command. CMAKE Will Now Exit The Build Process."
    )
endif ()

if (DISABLE_NATIVE_WARNING)
    set(SILENCE_NATIVE_WARNING DISABLE_CFMT_WARN)
endif ()

if (USE_FMTLIB OR USE_STDFMT)
    if (NOT
        EXISTS
        ${VENDOR_FILE_LOCATION}/utfutils-src
    )
        if (NOT BUILD_ALL
            AND NOT BUILD_SANDBOX
            AND NOT BUILD_TESTS
            AND NOT BUILD_BENCHMARKS
        )
            set(INDEPENDANT_UTF_UTILS USE_INDEPENDANT_UTF_UTILS)
        endif ()
        cpmaddpackage(
            NAME
            utfutils
            GITHUB_REPOSITORY
            USAFrenzy/UTF-Utils
            VERSION
            1.0.1
            "BUILD_COMPILED_LIBRARY ON"
        )
    endif ()
endif ()

set(UFT_UTILS_INCLUDE_DIRS ${VENDOR_FILE_LOCATION}/utfutils-src/include)
set(UFT_UTILS UTF-Utils_Compiled)

add_subdirectory(Serenity)

if (BUILD_DEMOS)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Demos ${CMAKE_CURRENT_BINARY_DIR}/Demos)
endif ()

# setup for allowing Serenity and spdlog to be used in benchmark and sandbox with the same format
# backend
if (BUILD_BENCHMARKS
    OR BUILD_SANDBOX
    OR BUILD_ALL
)
    if (USE_FMTLIB)
        cpmaddpackage(
            NAME
            spdlog
            GITHUB_REPOSITORY
            gabime/spdlog
            GIT_TAG
            v1.10.0
            OPTIONS
            "SPDLOG_USE_STD_FORMAT OFF"
            "SPDLOG_FMT_EXTERNAL ON"
        )
        target_include_directories(spdlog PUBLIC ${GH_FMT_INCLUDE_DIR})
        target_link_libraries(spdlog PUBLIC ${FMT})
    else ()
        cpmaddpackage(
            NAME
            spdlog
            GITHUB_REPOSITORY
            gabime/spdlog
            GIT_TAG
            v1.10.0
            OPTIONS
            "SPDLOG_USE_STD_FORMAT OFF"
            "SPDLOG_FMT_EXTERNAL OFF"
        )
    endif ()
    include_directories(${VENDOR_FILE_LOCATION}/spdlog-src/include)
endif ()

if (BUILD_BENCHMARKS)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/bench ${CMAKE_CURRENT_BINARY_DIR}/bench)
endif ()

if (BUILD_SANDBOX)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Sandbox ${CMAKE_CURRENT_BINARY_DIR}/Sandbox)
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Sandbox)
endif ()

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests ${CMAKE_CURRENT_BINARY_DIR}/tests)
endif ()

if (BUILD_ALL)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Demos ${CMAKE_CURRENT_BINARY_DIR}/Demos)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/bench ${CMAKE_CURRENT_BINARY_DIR}/bench)
    enable_testing()
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests ${CMAKE_CURRENT_BINARY_DIR}/tests)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Sandbox ${CMAKE_CURRENT_BINARY_DIR}/Sandbox)
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Sandbox)
endif ()
