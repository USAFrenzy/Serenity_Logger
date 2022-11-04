set(PROJECT_NAME Benchmark)

# Unused at the moment
set(BENCHMARK_HEADERS)
set(BENCHMARK_SOURCES "src/Benchmark.cpp")
set(BENCHMARK_ALL_FILES ${BENCHMARK_HEADERS} ${BENCHMARK_SOURCES})

add_executable(${PROJECT_NAME} ${BENCHMARK_ALL_FILES})

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

target_include_directories(${PROJECT_NAME} PUBLIC ${SERENITY_INCLUDE_DIR} ${SPDLOG_INCLUDE_DIR})

if (USE_NATIVEFMT)
    target_compile_definitions(${PROJECT_NAME} PUBLIC ${ARG_FMT_COMPILE_DEF} ${FORMATTER_BACKEND})
    target_link_libraries(
        ${PROJECT_NAME}
        PUBLIC ${SPDLOG}
               ${SERENITY}
               ${ARG_FMT}
    )
elseif (USE_FMTLIB)
    target_compile_definitions(${PROJECT_NAME} PUBLIC ${FORMATTER_BACKEND})
    target_link_libraries(
        ${PROJECT_NAME}
        PUBLIC ${SPDLOG}
               ${SERENITY}
               ${FMT}
    )
else ()
    target_compile_definitions(${PROJECT_NAME} PUBLIC ${FORMATTER_BACKEND})
    target_link_libraries(${PROJECT_NAME} PUBLIC ${SPDLOG} ${SERENITY})
endif ()
