@echo off
cd ..
	git pull
cd scripts/dev/bat
	call clang_format
	call cmake_format
echo -- Removing all build folders
	call clean
cd ../../
