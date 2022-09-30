#!/bin/bash


echo "-- Running cmake_format script..."
#####################################################################
#        Requires cmakelang to be installed and added to PATH       #
#####################################################################
pushd .. > /dev/null
cmake-format -i "CMakeLists.txt" -c "cmake-format.yaml"  > /dev/null
	pushd Serenity > /dev/null
		cmake-format -i "CMakeLists.txt" -c "../cmake-format.yaml" > /dev/null
	 popd > /dev/null
	pushd Sandbox > /dev/null
		cmake-format -i "CMakeLists.txt" -c "../cmake-format.yaml" > /dev/null
	popd > /dev/null
popd > /dev/null
echo "-- Finished running cmake_format script"
