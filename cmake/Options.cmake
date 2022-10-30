
option(
    BUILD_SANDBOX
    "Build The Sandbox Project Alongside The Library"
    OFF
)

option(
    BUILD_BENCHMARKS
    "Build The Benchmarking Project Alongside The Library"
    OFF
)

option(
    BUILD_TESTS
    "Build Test Suite Alongside The Library"
    OFF
)

option(
    BUILD_DEMOS
    "Build The Demo Projects Alongside The Library"
    OFF
)

option(
    BUILD_ALL
    "Build Benchmarking, Testing, Demo, and Sandbox Projects Alongside The Library"
    OFF
)

option(
    USE_STDFMT
    "Build The Library With The C++20 <format> Header Instead Of The Native Formatter Provided By ArgFormatter"
    OFF
)

option(
    USE_FMTLIB
    "Build The Library With The fmtlib Library By Victor Zverovich Instead Of The Native Formatter Provided By ArgFormatter"
    OFF
)

option(
    USE_NATIVEFMT
    "Build The Library With The Built-In ArgFormatter Library"
    ON
)

option(
    DISABLE_NATIVE_WARNING
    "Disable The Info Message On Compilation Stating That The Built-In Formatter Is Currently Enabled"
    OFF
)