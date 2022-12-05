set(PROJECT_NAME Serenity)

set(MESSAGE_DETAILS_SOURCE_FILES
    "src/MessageDetails/Message_Info.cpp"
    "src/MessageDetails/Message_Time.cpp"
    "src/MessageDetails/Message_Formatter.cpp"
    "src/MessageDetails/FlushPolicy.cpp"
)

set(TARGETS_SOURCE_FILES
    "src/Targets/Target.cpp"
    "src/Targets/ConsoleTarget.cpp"
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
    "include/serenity/MessageDetails/FlushPolicy.h"
)

set(TARGETS_HEADER_FILES
    "include/serenity/Targets/Target.h"
    "include/serenity/Targets/Target-impl.h"
    "include/serenity/Targets/ConsoleTarget.h"
    "include/serenity/Targets/FileTarget.h"
    "include/serenity/Targets/RotatingTarget.h"
)

set(UTILITIES_HEADER_FILES
    "include/serenity/Utilities/Utilities.h"
    "include/serenity/Utilities/FileHelper.h"
    "include/serenity/Utilities/TargetHelper.h"
    "include/serenity/Utilities/FormatBackend.h"
)

set(COLOR_HEADER_FILES "include/serenity/Color/Color.h")

set(MISC_HEADER_FILES
    "include/serenity/Version.h"
    "include/serenity/Common.h"
    "include/serenity/Defines.h"
)

# Once the custom formatting flags have been specialized, I may end up moving the FormatBackend.h
# logic into these separate files and just rename the 'CustomFormat' folder to 'FormatBackend'
# instead; this way, it's absolutely clear what's being included for each variation
if (USE_NATIVEFMT)
    set(CUSTOM_FMT_SPECIALIAZATIONS "include/serenity/CustomFormat/argfmt_backend.h")
elseif (USE_FMTLIB)
    set(CUSTOM_FMT_SPECIALIAZATIONS "include/serenity/CustomFormat/fmtlib_backend.h")
elseif (USE_STDFMT)
    set(CUSTOM_FMT_SPECIALIAZATIONS "include/serenity/CustomFormat/stdfmt_backend.h")
endif ()

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
    ${CUSTOM_FMT_SPECIALIAZATIONS}
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

# Compile Definitions
if (USE_NATIVEFMT)
    target_compile_definitions(
        ${PROJECT_NAME}
        PUBLIC ${FORMATTER_BACKEND}
               ${SILENCE_NATIVE_WARNING}
               ${ARG_FMT_COMPILE_DEF}
               ${UTF_UTILS_COMPILE_DEF}
    )
else ()
    target_compile_definitions(${PROJECT_NAME} PUBLIC ${FORMATTER_BACKEND} ${UTF_UTILS_COMPILE_DEF})
endif ()

# Include Directories
if (USE_NATIVEFMT)
    target_include_directories(
        ${PROJECT_NAME}
        PUBLIC ${SERENITY_INCLUDE_DIR}
               ${ARGFMT_INCLUDE_DIR}
               ${UTF_UTILS_INCLUDE_DIR}
    )
elseif (USE_FMTLIB)
    target_include_directories(
        ${PROJECT_NAME}
        PUBLIC ${SERENITY_INCLUDE_DIR}
               ${FMTLIB_INCLUDE_DIR}
               ${UTF_UTILS_INCLUDE_DIR}
    )
else ()
    target_include_directories(
        ${PROJECT_NAME} PUBLIC ${SERENITY_INCLUDE_DIR} ${UTF_UTILS_INCLUDE_DIR}
    )
endif ()

# Library Linkage
if (USE_NATIVEFMT)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${ARG_FMT} ${UTF_UTILS})
elseif (USE_FMTLIB)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${FMT} ${UTF_UTILS})
else ()
    target_link_libraries(${PROJECT_NAME} PUBLIC ${UTF_UTILS})
endif ()
