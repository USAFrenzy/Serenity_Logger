
set (PROJECT_NAME Color_Tag_Demo)

set (TAG_DEMO_SOURCE_FILES "Color Tagging/ColorDemo.cpp")

add_executable (${PROJECT_NAME} ${TAG_DEMO_SOURCE_FILES})
set_target_properties (${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)
target_link_libraries (
    ${PROJECT_NAME}
    LINK_PUBLIC
    Serenity
)