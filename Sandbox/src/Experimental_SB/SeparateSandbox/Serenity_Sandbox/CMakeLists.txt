cmake_minimum_required (VERSION 3.20)

project (Serenity_Sandbox)


set (SANDBOX_HEADER_FILES
"Utilities.h"
)

set (SANDBOX_SOURCE_FILES 
"Source.cpp"
"Utilities.cpp"
)

set (SANDBOX_ALL_FILES ${SANDBOX_HEADER_FILES} ${SANDBOX_SOURCE_FILES})
add_executable (${PROJECT_NAME} ${SANDBOX_ALL_FILES})
set_target_properties (${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)
