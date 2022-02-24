set (PROJECT_NAME Serenity)

set (
    SERENITY_SOURCE_FILES
    "src/MessageDetails/Message_Info.cpp"
    "src/MessageDetails/Message_Time.cpp"
    "src/MessageDetails/Message_Formatter.cpp"
    "src/MessageDetails/FlushPolicy.cpp"
    "src/Targets/Target.cpp"
    "src/Targets/ColorConsoleTarget.cpp"
    "src/Targets/FileTarget.cpp"
    "src/Targets/RotatingTarget.cpp"
    "src/Common.cpp"
    "src/Color/Color.cpp"
    "src/Utilities/Utilities.cpp"
)

set (
    SERENITY_HEADER_FILES
    "include/serenity/Version.h"
    "include/serenity/Common.h"
    "include/serenity/MessageDetails/Message_Info.h"
    "include/serenity/MessageDetails/Message_Time.h"
    "include/serenity/MessageDetails/Message_Formatter.h"
    "include/serenity/MessageDetails/FlushPolicy.h"
    "include/serenity/Targets/Target.h"
    "include/serenity/Targets/Target-impl.h"
    "include/serenity/Targets/ColorConsoleTarget.h"
    "include/serenity/Targets/FileTarget.h"
    "include/serenity/Targets/RotatingTarget.h"
    "include/serenity/Color/Color.h"
    "include/serenity/Utilities/Utilities.h"
)

set (SERENITY_ALL_FILES ${SERENITY_SOURCE_FILES} ${SERENITY_HEADER_FILES})

add_library (${PROJECT_NAME} ${SERENITY_ALL_FILES})

target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries (${PROJECT_NAME} spdlog)

set_target_properties (
    ${PROJECT_NAME}
    PROPERTIES CXX_STANDARD 23
               CMAKE_ARCHIVE_OUTPUT_DIRECTORY_<CONFIG> ${CMAKE_SOURCE_DIR}/lib
               CMAKE_LIBRARY_OUTPUT_DIRECTORY_<CONFIG> ${CMAKE_SOURCE_DIR}/lib
               CMAKE_RUNTIME_OUTPUT_DIRECTORY_<CONFIG> ${CMAKE_SOURCE_DIR}/bin
               OUTPUT_NAME "serenity"
               DEBUG_POSTFIX "_d"
               COMPILE_FLAGS ${FLAG_STORAGE}
)
