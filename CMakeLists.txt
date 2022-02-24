cmake_minimum_required(VERSION 3.20)

project(Serenity_Sandbox)

include(cmake/PreventInSrcBuilds.cmake)

include(cmake/Incremental_Build.cmake)
get_and_increment_version()
message("-- ${PROJECT_NAME} Building Version: ${SERENITY_FULL_VERSION}")

include(cmake/CPM.cmake)
cpmaddpackage("gh:gabime/spdlog@1.9.2")

set(FLAG_STORAGE "")

# Not sure if '/' or '-' really make a difference here could probably just simplify to '-utf-8'
if (CMAKE_CXX_COMPILER_ID
    STREQUAL
    "MSVC"
)
    set(UTF8_FLAG "/utf-8")
else ()
    set(UTF8_FLAG "-utf-8")
endif ()

option(
    BUILD_SANDBOX
    "Build The Sandbox Project Alongside The Library"
    ON
)
option(
    BUILD_DEMOS
    "Build The Demo Projects Alongside The Library"
    ON
)
option(
    ENABLE_UTF8_FLAG
    "Equivalent To /utf-8 For MSVC"
    ON
)

if (ENABLE_UTF8_FLAG)
    set(FLAG_STORAGE "${FLAG_STORAGE} ${UTF8_FLAG}")
endif ()

add_subdirectory(Serenity)

if (BUILD_SANDBOX)
    add_subdirectory(Sandbox)
endif ()

if (BUILD_DEMOS)
    add_subdirectory(Demos)
endif ()
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Sandbox)
