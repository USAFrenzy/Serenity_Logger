cmake_minimum_required (VERSION 3.20)

include (cmake/PreventInSrcBuilds.cmake)
assureoutofsourcebuilds ()

include (cmake/CPM.cmake)
cpmaddpackage ("gh:gabime/spdlog@1.9.1")

project (Serenity_Sandbox)

option (
    BUILD_SANDBOX
    "Build The Sandbox Project Alongside The Library"
    ON
)

add_subdirectory (Serenity)
if (BUILD_SANDBOX)
    add_subdirectory (Sandbox)
endif ()
