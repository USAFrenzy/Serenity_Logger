@echo off
    cd ../../../
@echo on
    cmake -B build_std -S . -DBUILD_ALL="ON" -DUSE_NATIVEFMT="OFF" -DUSE_STDFMT="ON"
@echo off
    cd scripts/dev/bat