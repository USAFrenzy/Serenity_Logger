#!/bin/bash

echo "-- Running cmake_format script..."
#####################################################################
#        Requires cmakelang to be installed and added to PATH       #
#####################################################################
cd ../../../ > /dev/null
cmake-format -i "CMakeLists.txt" -c "cmake-format.yaml"  > /dev/null
	pushd Serenity > /dev/null
		cmake-format -i "CMakeLists.txt" -c "../cmake-format.yaml" > /dev/null
	 popd > /dev/null
	pushd Sandbox > /dev/null
		cmake-format -i "CMakeLists.txt" -c "../cmake-format.yaml" > /dev/null
	popd > /dev/null
cd scripts/dev/shell > /dev/null
echo "-- Finished running cmake_format script"
