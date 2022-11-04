@echo off
    cd ../../../../
@echo on
cmake -B build_fmt_release_16 -S . -DUSE_NATIVEFMT="OFF" -DUSE_FMTLIB="ON" -DUSE_STDFMT="OFF" -DBUILD_ALL="OFF" -DBUILD_SANDBOX="OFF" -DBUILD_DEMOS="OFF" -DBUILD_TESTS="OFF" -DBUILD_BENCHMARKS="OFF" -DDISABLE_NATIVE_WARNING="ON" -G "Visual Studio 16 2019"
@echo off
    cd scripts/dev/bat/release_prep