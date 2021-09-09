set (PROJECT_NAME Serenity)

set (
    SERENITY_SOURCE_FILES 
    "src/Logger.cpp"
    "src/LogFileHelper.cpp"
    "src/Utilities.cpp"
    "src/Sinks.cpp"
)

set (
    SERENITY_HEADER_FILES
    "include/serenity/Logger.h"
    "include/serenity/Defines.h"
    "include/serenity/Common.h"
    "include/serenity/Helpers/LogFileHelper.h"
    "include/serenity/Interfaces/IObserver.h"
    "include/serenity/Utilities/Utilities.h"
    "include/serenity/Sinks/Sinks.h"
)

set (SERENITY_ALL_FILES ${SERENITY_SOURCE_FILES} ${SERENITY_HEADER_FILES})

add_library (${PROJECT_NAME} ${SERENITY_ALL_FILES})

target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries (${PROJECT_NAME} spdlog)

set_target_properties (
    ${PROJECT_NAME}
    PROPERTIES CXX_STANDARD 20
               CMAKE_ARCHIVE_OUTPUT_DIRECTORY_<CONFIG> ${CMAKE_SOURCE_DIR}/lib
               CMAKE_LIBRARY_OUTPUT_DIRECTORY_<CONFIG> ${CMAKE_SOURCE_DIR}/lib
               CMAKE_RUNTIME_OUTPUT_DIRECTORY_<CONFIG> ${CMAKE_SOURCE_DIR}/bin
               OUTPUT_NAME "serenity"
               DEBUG_POSTFIX "_d"
)
