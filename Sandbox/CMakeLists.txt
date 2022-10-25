set(PROJECT_NAME Sandbox)

set(SANDBOX_HEADER_FILES)
set(SANDBOX_SOURCE_FILES "src/Sandbox.cpp")
set(SANDBOX_ALL_FILES ${SANDBOX_HEADER_FILES} ${SANDBOX_SOURCE_FILES})

add_executable(${PROJECT_NAME} ${SANDBOX_ALL_FILES})

if (NOT USE_NATIVEFMT)
    cpmaddpackage(
        NAME
        argfmt
        GITHUB_REPOSITORY
        USAFrenzy/ArgFormatter
        VERSION
        1.0.0
        OPTIONS
        "BUILD_COMPILED_LIB OFF"
        "BUILD_SANDBOX OFF"
        "BUILD_TESTS OFF"
        "BUILD_ALL OFF"
    )
    target_include_directories(
        ${PROJECT_NAME} PUBLIC ${VENDOR_FILE_LOCATION}/argfmt-src/include ${GH_FMT_INCLUDE_DIR}
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

target_link_libraries(
    ${PROJECT_NAME}
    LINK_PUBLIC
    ${FMT}
    Serenity
)
