#!/bin/bash

pushd .. > /dev/null
cmake -B ../Serenity_Logger_Build -S . -G "Eclipse CDT4 - Unix Makefiles"
popd > /dev/null
