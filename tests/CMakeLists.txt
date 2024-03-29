cmake_minimum_required(VERSION 3.14)

set(PROJECT_NAME Serenity_Tests)

set(TEST_SOURCE_FILES
main.cpp
RotatingTests.cpp
FormattingArgsTest.cpp
)

enable_testing()

add_executable(${PROJECT_NAME} ${TEST_SOURCE_FILES})

if (CMAKE_CXX_COMPILER_ID
    STREQUAL
    "MSVC"
)
if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 192930145)
set(STANDARD 20)
else()
set(STANDARD 23)
endif()
else()
set(STANDARD 20)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD ${STANDARD})

target_link_libraries(
    ${PROJECT_NAME}
    LINK_PUBLIC
    Serenity
)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
set_tests_properties(${PROJECT_NAME} PROPERTIES RUN_SERIAL ON)