set(PROJECT_NAME Serenity)

set(MESSAGE_DETAILS_SOURCE_FILES
    "src/MessageDetails/Message_Info.cpp"
    "src/MessageDetails/Message_Time.cpp"
    "src/MessageDetails/Message_Formatter.cpp"
    "src/MessageDetails/FormatterArgs.cpp"
    "src/MessageDetails/FlushPolicy.cpp"
    "src/MessageDetails/ArgFormatter.cpp"
)

set(TARGETS_SOURCE_FILES
    "src/Targets/Target.cpp"
    "src/Targets/ColorConsoleTarget.cpp"
    "src/Targets/FileTarget.cpp"
    "src/Targets/RotatingTarget.cpp"
)

set(UTILITIES_SOURCE_FILES
    "src/Utilities/Utilities.cpp"
    "src/Utilities/FileHelper.cpp"
    "src/Utilities/TargetHelper.cpp"
)

set(COLOR_SOURCE_FILES "src/Color/Color.cpp")

set(MESSAGE_DETAILS_HEADER_FILES
    "include/serenity/MessageDetails/Message_Info.h"
    "include/serenity/MessageDetails/Message_Time.h"
    "include/serenity/MessageDetails/Message_Formatter.h"
    "include/serenity/MessageDetails/Message_Formatter_Impl.h"
    "include/serenity/MessageDetails/FormatterArgs.h"
    "include/serenity/MessageDetails/FlushPolicy.h"
    "include/serenity//MessageDetails/ArgFormatter.h"
    "include/serenity//MessageDetails/ArgFormatterImpl.h"
    "include/serenity//MessageDetails/ArgContainer.h"
    "include/serenity//MessageDetails/ArgContainerImpl.h"
)

set(TARGETS_HEADER_FILES
    "include/serenity/Targets/Target.h"
    "include/serenity/Targets/Target-impl.h"
    "include/serenity/Targets/ColorConsoleTarget.h"
    "include/serenity/Targets/FileTarget.h"
    "include/serenity/Targets/RotatingTarget.h"
)

set(UTILITIES_HEADER_FILES
    "include/serenity/Utilities/Utilities.h"
    "include/serenity/Utilities/FileHelper.h"
    "include/serenity/Utilities/TargetHelper.h"
)

set(COLOR_HEADER_FILES "include/serenity/Color/Color.h")

set(MISC_HEADER_FILES "include/serenity/Version.h" "include/serenity/Common.h")

set(SERENITY_SOURCE_FILES
    ${MESSAGE_DETAILS_SOURCE_FILES}
    ${TARGETS_SOURCE_FILES}
    ${UTILITIES_SOURCE_FILES}
    ${COLOR_SOURCE_FILES}
)

set(SERENITY_HEADER_FILES
    ${MESSAGE_DETAILS_HEADER_FILES}
    ${TARGETS_HEADER_FILES}
    ${UTILITIES_HEADER_FILES}
    ${COLOR_HEADER_FILES}
    ${MISC_HEADER_FILES}
)

source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PREFIX "Header Files"
    FILES ${SERENITY_HEADER_FILES}
)
source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PREFIX "Source Files"
    FILES ${SERENITY_SOURCE_FILES}
)

set(SERENITY_ALL_FILES ${SERENITY_SOURCE_FILES} ${SERENITY_HEADER_FILES})

add_library(${PROJECT_NAME} ${SERENITY_ALL_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (CMAKE_CXX_COMPILER_ID
    STREQUAL
    "MSVC"
)
    if (CMAKE_CXX_COMPILER_VERSION
        VERSION_GREATER_EQUAL
        192930145
    )
        set(STANDARD 20)
    else ()
        set(STANDARD 23)
    endif ()
else ()
    set(STANDARD 20)
endif ()

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES CXX_STANDARD ${STANDARD}
               CMAKE_ARCHIVE_OUTPUT_DIRECTORY_<CONFIG> ${CMAKE_SOURCE_DIR}/lib
               CMAKE_LIBRARY_OUTPUT_DIRECTORY_<CONFIG> ${CMAKE_SOURCE_DIR}/lib
               CMAKE_RUNTIME_OUTPUT_DIRECTORY_<CONFIG> ${CMAKE_SOURCE_DIR}/bin
               OUTPUT_NAME "serenity"
               DEBUG_POSTFIX "_d"
)
