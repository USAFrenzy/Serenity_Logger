@echo off
    cd ../../../
@echo on
    cmake -B build_fmt -S . -DBUILD_ALL="ON" -DUSE_NATIVEFMT="OFF" -DUSE_FMTLIB="ON"
@echo off
    cd scripts/dev/bat