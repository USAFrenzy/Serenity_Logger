#!/bin/bash
cd ../../../ > /dev/null
    if [ -d "build" ] 
	    then
        rm -rf build
    )
    if [ -d "build_fmt" ] 
	    then
        rm -rf build_fmt
    )
    if [ -d "build_std" ] 
	    then
        rm -rf build_std
    )
cd scripts/dev/shell > /dev/null
