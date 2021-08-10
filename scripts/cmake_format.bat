@echo off
REM #####################################################################
REM #        Requires cmakelang to be installed and added to PATH       #
REM #####################################################################
pushd ..
cmake-format -i "CMakeLists.txt" -c "cmake-format.yaml"
	pushd Serenity
		cmake-format -i "CMakeLists.txt" -c "../cmake-format.yaml"
	 popd
	pushd Sandbox
		cmake-format -i "CMakeLists.txt" -c "../cmake-format.yaml"
	popd
popd