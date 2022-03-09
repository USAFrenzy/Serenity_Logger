#!/bin/bash

echo "-- Formatting CMake Files"
	bash cmake_format.sh > /dev/null
echo "-- Formatting Project Files"
	bash clang_format.sh > /dev/null
pushd .. > /dev/null
	git add . > /dev/null
	git commit -m "auto commit update" > /dev/null
	git push > /dev/null
echo "-- Finished Formatting And Pushing To Git"

popd > /dev/null
