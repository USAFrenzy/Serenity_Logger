@echo off
    cd ../../../../
@echo on
cmake -B build_native_release_17 -S . -DUSE_NATIVEFMT="ON" -DUSE_FMTLIB="OFF" -DUSE_STDFMT="OFF" -DBUILD_ALL="OFF" -DBUILD_SANDBOX="OFF" -DBUILD_DEMOS="OFF" -DBUILD_TESTS="OFF" -DBUILD_BENCHMARKS="OFF" -DDISABLE_NATIVE_WARNING="ON" -G "Visual Studio 17 2022"
@echo off
    cd scripts/dev/bat/release_prep