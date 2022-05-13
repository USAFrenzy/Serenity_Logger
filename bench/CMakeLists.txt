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
    set(STANDARD 23)
else ()
    set(STANDARD 20)
endif ()

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD ${STANDARD})

target_link_libraries(
    ${PROJECT_NAME}
    LINK_PUBLIC
    spdlog
    Serenity
)