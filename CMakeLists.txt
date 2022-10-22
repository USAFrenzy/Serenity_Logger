cmake_minimum_required(VERSION 3.14)

project(Serenity_Sandbox)

include(cmake/PreventInSrcBuilds.cmake)
include(cmake/CPM.cmake)
include(cmake/Incremental_Build.cmake)

get_and_increment_version()
message("-- ${PROJECT_NAME} Building Version: ${SERENITY_FULL_VERSION}")

option(
    BUILD_SANDBOX
    "Build The Sandbox Project Alongside The Library"
    OFF
)

option(
    BUILD_BENCHMARKS
    "Build The Benchmarking Project Alongside The Library"
    OFF
)

option(
    BUILD_TESTS
    "Build Test Suite Alongside The Library"
    OFF
)

option(
    BUILD_DEMOS
    "Build The Demo Projects Alongside The Library"
    OFF
)

option(
    BUILD_ALL
    "Build Benchmarking, Testing, Demo, and Sandbox Projects Alongside The Library"
    OFF
)

option(
    USE_STDFMT
    "Build The Library With The C++20 <format> Header Instead Of The Native Formatter Provided By ArgFormatter"
    OFF
)

option(
    USE_FMTLIB
    "Build The Library With The fmtlib Library By Victor Zverovich Instead Of The Native Formatter Provided By ArgFormatter"
    OFF
)

option(
    USE_NATIVEFMT
    "Build The Library With The Built-In ArgFormatter Library"
    ON
)

option(
    DISABLE_NATIVE_WARNING
    "Disable The Info Message On Compilation Stating That The Built-In Formatter Is Currently Enabled"
    OFF
)

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
    # cpmaddpackage( NAME fmtlib GITHUB_REPOSITORY fmtlib/fmt VERSION 9.1.0 )
    set(FORMATTER_BACKEND USE_FMT_LIB)
elseif (
    USE_NATIVEFMT
    AND NOT USE_STDFMT
    AND NOT USEFMTLIB
)
    # cpmaddpackage( NAME argfmt GITHUB_REPOSITORY USAFrenzy/ArgFormatter VERSION 1.0.0 OPTIONS
    # "BUILD_COMPILED_LIB On" )
    set(VENDOR_LOCATION ${CMAKE_BINARY_DIR}/_deps/)

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
            1.0.0
            OPTIONS
            "BUILD_COMPILED_LIB OFF"
        )
        include_directories(${VENDOR_LOCATION}/argfmt-src/include)
    endif ()
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

if (NOT USE_NATIVEFMT)
    set(UTF_UTILS USE_INDEPENDANT_UTF_UTILS)
    if (NOT
        EXISTS
        ${VENDOR_FILE_LOCATION}/utfutils-src
    )
        cpmaddpackage(
            NAME
            utfutils
            GITHUB_REPOSITORY
            USAFrenzy/UTF-Utils
            VERSION
            1.0.0
        )
        include_directories(${VENDOR_FILE_LOCATION}/utfutils-src/include)
    endif ()
endif ()

if (DISABLE_NATIVE_WARNING)
    set(SILENCE_NATIVE_WARNING DISABLE_CFMT_WARN)
endif ()

add_subdirectory(Serenity)

if (BUILD_DEMOS)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Demos ${CMAKE_CURRENT_BINARY_DIR}/Demos)
endif ()

if (BUILD_BENCHMARKS
    OR BUILD_SANDBOX
    OR BUILD_ALL
)
    include(cmake/CPM.cmake)
    cpmaddpackage("gh:gabime/spdlog@1.10.0")
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
