cmake_minimum_required(VERSION 3.14)

project(Serenity_Sandbox)

include(cmake/PreventInSrcBuilds.cmake)
include(cmake/CPM.cmake)
include(cmake/Options.cmake)
include(cmake/Incremental_Build.cmake)

get_and_increment_version()
message("-- ${PROJECT_NAME} Building Version: ${SERENITY_FULL_VERSION}")

# Versions of sub projects to build
set(ARG_FMT_VERSION 1.0.4)
set(UTF_UTILS_VERSION 1.0.2)
set(FMT_LIB_VERSION 9.1.0)
set(SPDLOG_VERSION v1.10.0)

# include directory paths from the sub projects
set(VENDOR_FILE_LOCATION ${CMAKE_BINARY_DIR}/_deps)
set(FMTLIB_INCLUDE_DIR ${VENDOR_FILE_LOCATION}/fmt-src/include)
set(ARGFMT_INCLUDE_DIR ${VENDOR_FILE_LOCATION}/argfmt-src/include)
set(UTF_UTILS_INCLUDE_DIR ${VENDOR_FILE_LOCATION}/utfutils-src/include)
set(SPDLOG_INCLUDE_DIR ${VENDOR_FILE_LOCATION}/spdlog-src/include)
set(SERENITY_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Serenity/include)

# specific definition that needs to be applied for ArgFormatter to see the correct include path
set(ARG_FMT_COMPILE_DEF COMPILED_LIBRARY)
set(UTF_UTILS_COMPILE_DEF BUILD_COMPILED_LIB)

# The compiled object file names for the sub projects
set(ARG_FMT ArgFormatter_Lib)
set(UTF_UTILS UTF-Utils_Compiled)
set(FMT fmt::fmt)
set(SERENITY Serenity)
set(SPDLOG spdlog)

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
        ${FMT_LIB_VERSION}
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
    set(FORMATTER_BACKEND USE_FMT_LIB)
elseif (
    USE_NATIVEFMT
    AND NOT USE_STDFMT
    AND NOT USE_FMTLIB
)
    cpmaddpackage(
        NAME
        argfmt
        GITHUB_REPOSITORY
        USAFrenzy/ArgFormatter
        VERSION
        ${ARG_FMT_VERSION}
        OPTIONS
        "BUILD_COMPILED_LIB ON"
        "BUILD_SANDBOX OFF"
        "BUILD_TESTS OFF"
        "BUILD_ALL OFF"
    )
    target_compile_definitions(${ARG_FMT} PUBLIC ${ARG_FMT_COMPILE_DEF} ${UTF_UTILS_COMPILE_DEF})
    set(FORMATTER_BACKEND USE_BUILT_IN_FMT)
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

if (USE_STDFMT OR USE_FMTLIB)
    cpmaddpackage(
        NAME
        utfutils
        GITHUB_REPOSITORY
        USAFrenzy/UTF-Utils
        VERSION
        ${UTF_UTILS_VERSION}
        OPTIONS
        "BUILD_COMPILED_LIBRARY ON"
    )
endif ()

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
            ${SPDLOG_VERSION}
            OPTIONS
            "SPDLOG_USE_STD_FORMAT OFF"
            "SPDLOG_FMT_EXTERNAL ON"
        )
        target_include_directories(spdlog PUBLIC ${FMTLIB_INCLUDE_DIR})
        target_link_libraries(spdlog PUBLIC ${FMT})
    else ()
        cpmaddpackage(
            NAME
            spdlog
            GITHUB_REPOSITORY
            gabime/spdlog
            GIT_TAG
            ${SPDLOG_VERSION}
            OPTIONS
            "SPDLOG_USE_STD_FORMAT OFF"
            "SPDLOG_FMT_EXTERNAL OFF"
        )
    endif ()
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
