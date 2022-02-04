cmake_minimum_required (VERSION 3.20)

project (Serenity_Sandbox)

include (cmake/PreventInSrcBuilds.cmake)

include(cmake/Incremental_Build.cmake)
get_and_increment_version()
 message("-- ${PROJECT_NAME} Building Version: ${SERENITY_FULL_VERSION}")

include (cmake/CPM.cmake)
cpmaddpackage ("gh:gabime/spdlog@1.9.2")

option (BUILD_SANDBOX	"Build The Sandbox Project Alongside The Library" 	ON)
option (BUILD_DEMOS	"Build The Demo Projects Alongside The Library"		ON)

add_subdirectory (Serenity)

if (BUILD_SANDBOX)
    add_subdirectory (Sandbox)
endif ()

if (BUILD_DEMOS)
    add_subdirectory (Demos)
endif ()