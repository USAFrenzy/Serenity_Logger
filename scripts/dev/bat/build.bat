@echo off
    cd ../../../
@echo on
    cmake -B build -S . -DBUILD_ALL="ON"
@echo off
    cd scripts/dev/bat