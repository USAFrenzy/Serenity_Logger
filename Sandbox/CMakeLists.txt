set (PROJECT_NAME Sandbox)

set (SANDBOX_HEADER_FILES)
set (SANDBOX_SOURCE_FILES "src/demoTestMain.cpp")
set (SANDBOX_ALL_FILES ${SANDBOX_HEADER_FILES} ${SANDBOX_SOURCE_FILES})

add_executable (${PROJECT_NAME} ${SANDBOX_ALL_FILES})
set_target_properties (${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)
target_link_libraries (
    ${PROJECT_NAME}
    LINK_PUBLIC
    Serenity
)


set (PROJECT_NAME Experimental_Sandbox)

set (EXPERIMENTAL_SB_HEADER_FILES)
set (EXPERIMENTAL_SB_SOURCE_FILES "src/Experimental_SB/Experimental_Sandbox.cpp")
set (EXPERIMENTAL_SB_ALL_FILES ${EXPERIMENTAL_SB_HEADER_FILES} ${EXPERIMENTAL_SB_SOURCE_FILES})

add_executable (${PROJECT_NAME} ${EXPERIMENTAL_SB_ALL_FILES})
set_target_properties (${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)
target_link_libraries (
    ${PROJECT_NAME}
    LINK_PUBLIC
    Serenity
)