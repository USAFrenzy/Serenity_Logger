
set (PROJECT_NAME Color_Tag_Demo)

set (TAG_DEMO_SOURCE_FILES "Color Tagging/ColorDemo.cpp")

add_executable (${PROJECT_NAME} ${TAG_DEMO_SOURCE_FILES})
set_target_properties (${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)

if(USE_NATIVEFMT)
target_compile_definitions(${PROJECT_NAME} PUBLIC ${ARG_FMT_COMPILE_DEF})
endif()

if (USE_NATIVEFMT)
    target_compile_definitions(${PROJECT_NAME} PUBLIC ${ARG_FMT_COMPILE_DEF})
    target_link_libraries(
        ${PROJECT_NAME}
        PUBLIC
               Serenity
               ${ARG_FMT}
    )
elseif (USE_FMTLIB)
    target_link_libraries(
        ${PROJECT_NAME}
        PUBLIC
               Serenity
               ${FMT}
    )
else ()
    target_link_libraries(
        ${PROJECT_NAME}
        PUBLIC
               Serenity
    )
endif ()