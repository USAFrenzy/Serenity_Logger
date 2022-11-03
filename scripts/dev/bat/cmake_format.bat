@echo -- Running cmake_format script...
@echo off
REM #####################################################################
REM #        Requires cmakelang to be installed and added to PATH       #
REM #####################################################################
cd ../../../
cmake-format -i "CMakeLists.txt" -c "cmake-format.yaml"
	pushd tests
		cmake-format -i "CMakeLists.txt" -c "../cmake-format.yaml"
	 popd
	pushd bench
		cmake-format -i "CMakeLists.txt" -c "../cmake-format.yaml"
	 popd
	pushd Serenity
		cmake-format -i "CMakeLists.txt" -c "../cmake-format.yaml"
	 popd
	pushd Sandbox
		cmake-format -i "CMakeLists.txt" -c "../cmake-format.yaml"
	popd
cd scripts/dev/bat
@echo -- Finished running cmake_format script