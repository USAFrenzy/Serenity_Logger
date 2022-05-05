cmake_minimum_required(VERSION 3.14)

project(Serenity_Sandbox)

include(cmake/PreventInSrcBuilds.cmake)

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
    ON
)

add_subdirectory(Serenity)

if (BUILD_DEMOS)
    add_subdirectory(Demos)
endif ()

if (BUILD_BENCHMARKS)
    add_subdirectory(bench)
endif ()

if (BUILD_SANDBOX)
    include(cmake/CPM.cmake)
    cpmaddpackage("gh:gabime/spdlog@1.10.0")
    add_subdirectory(Sandbox)
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Sandbox)
endif ()

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()

if (BUILD_ALL)
    add_subdirectory(Demos)

    add_subdirectory(bench)

    enable_testing()
    add_subdirectory(tests)

    include(cmake/CPM.cmake)
    cpmaddpackage("gh:gabime/spdlog@1.10.0")
    add_subdirectory(Sandbox)
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Sandbox)

endif ()
