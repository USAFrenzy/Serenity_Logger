cmake_minimum_required(VERSION 3.14)

set(PROJECT_NAME Serenity_Tests)

set(TEST_SOURCE_FILES
    main.cpp
    RotatingTests.cpp
    FormattingArgsTest.cpp
)

enable_testing()

add_executable(${PROJECT_NAME} ${TEST_SOURCE_FILES})

if (NOT USE_NATIVEFMT)
    cpmaddpackage(
        NAME
        argfmt
        GITHUB_REPOSITORY
        USAFrenzy/ArgFormatter
        VERSION
        ${ARG_FMT_VERSION}
        OPTIONS
        "BUILD_COMPILED_LIB ON"
        "BUILD_SANDBOX OFF"
        "BUILD_TESTS OFF"
        "BUILD_ALL OFF"
    )
    target_compile_definitions(${ARG_FMT} PUBLIC ${ARG_FMT_COMPILE_DEF})
endif ()

if (NOT USE_FMT_LIB)
    cpmaddpackage(
        NAME
        fmt
        GITHUB_REPOSITORY
        fmtlib/fmt
        GIT_TAG
        ${FMT_LIB_VERSION}
        OPTIONS
        "FMT_PEDANTIC OFF"
        "FMT_WERROR OFF"
        "FMT_DOC OFF"
        "FMT_INSTALL OFF"
        "FMT_TEST OFF"
        "FMT_FUZZ OFF"
        "FMT_CUDA_TEST OFF"
        "FMT_OS ON"
        "FMT_MODULE OFF"
        "FMT_SYSTEM_HEADERS OFF"
    )
endif ()

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

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD ${STANDARD})

if (USE_NATIVEFMT)
    target_compile_definitions(
        ${PROJECT_NAME}
        PUBLIC ${FORMATTER_BACKEND}
               ${SILENCE_NATIVE_WARNING}
               ${ARG_FMT_COMPILE_DEF}
    )
else ()
    target_compile_definitions(${PROJECT_NAME} PUBLIC ${FORMATTER_BACKEND} ${ARG_FMT_COMPILE_DEF})
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC ${ARGFMT_INCLUDE_DIR} ${FMTLIB_INCLUDE_DIR})

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC ${UTF_UTILS}
           ${ARG_FMT}
           ${FMT}
           Serenity
)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
set_tests_properties(${PROJECT_NAME} PROPERTIES RUN_SERIAL ON)
