#!/bin/bash
cd ../../../ > /dev/null
    cmake -B build_std -S . -DBUILD_ALL="ON" -DUSE_NATIVEFMT="OFF" -DUSE_FMTLIB="ON"
cd scripts/dev/shell > /dev/null