set(PROJECT_NAME Color_Tag_Demo)

set(TAG_DEMO_SOURCE_FILES "Color Tagging/ColorDemo.cpp")

add_executable(${PROJECT_NAME} ${TAG_DEMO_SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)

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
    target_link_libraries(${PROJECT_NAME} PUBLIC ${SERENITY} ${ARG_FMT})
elseif (USE_FMTLIB)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${SERENITY} ${FMT})
else ()
    target_link_libraries(${PROJECT_NAME} PUBLIC ${SERENITY})
endif ()
