set(PROJECT_NAME Sandbox)

set(SANDBOX_HEADER_FILES)
set(SANDBOX_SOURCE_FILES "src/Sandbox.cpp")
set(SANDBOX_ALL_FILES ${SANDBOX_HEADER_FILES} ${SANDBOX_SOURCE_FILES})

add_executable(${PROJECT_NAME} ${SANDBOX_ALL_FILES})

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

# so that sandbox can use fmt::format
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

# so that sandbox can use ArgFormatter
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
    target_compile_definitions(${ARG_FMT} PUBLIC ${ARG_FMT_COMPILE_DEF} ${UTF_UTILS_COMPILE_DEF})
endif ()

# Since this is a sandbox, it should be allowed to access all the subprojects' include files
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC ${SERENITY_INCLUDE_DIR}
           ${ARGFMT_INCLUDE_DIR}
           ${FMTLIB_INCLUDE_DIR}
           ${UTF_UTILS_INCLUDE_DIR}
)

# Compile Time Definitions
if (USE_NATIVEFMT)
    target_compile_definitions(
        ${PROJECT_NAME}
        PUBLIC ${FORMATTER_BACKEND}
               ${SILENCE_NATIVE_WARNING}
               ${ARG_FMT_COMPILE_DEF}
               ${UTF_UTILS_COMPILE_DEF}
    )
else ()
    target_compile_definitions(
        ${PROJECT_NAME}
        PUBLIC ${FORMATTER_BACKEND}
               ${ARG_FMT_COMPILE_DEF}
               ${UTF_UTILS_COMPILE_DEF}
    )
endif ()

# Library Linkage
if (USE_FMTLIB)
    target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE ${SERENITY}
                ${SPDLOG}
                ${FMT}
                ${ARG_FMT}
                ${UTF_UTILS}
    )
elseif (USE_NATIVEFMT)
    target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE ${SERENITY}
                ${SPDLOG}
                ${ARG_FMT}
                ${UTF_UTILS}
    )
else ()
    target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE ${SERENITY}
                ${SPDLOG}
                ${UTF_UTILS}
    )
endif ()
