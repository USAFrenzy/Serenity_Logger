cmake_minimum_required(VERSION 3.20 REQUIRED)

project(Logging_Project)

set(Logger_Header_Files

)

set(Logger_Header_Files

)


add_library(serenity
STATIC
${Logger_Header_Files}
${Logger_Header_Files}
)
set_target_properties(
${serenity}
PROPERTIES
CMAKE_ARCHIVE_OUTPUT_DIRECTORY_<CONFIG> ${CMAKE_SOURCE_DIR}/lib
CMAKE_LIBRARY_OUTPUT_DIRECTORY_<CONFIG> ${CMAKE_SOURCE_DIR}/lib
CMAKE_RUNTIME_OUTPUT_DIRECTORY_<CONFIG> ${CMAKE_SOURCE_DIR}/bin
OUTPUT_NAME "serenity"
CMAKE_DEBUG_POSTFIX "_d.lib"
)
