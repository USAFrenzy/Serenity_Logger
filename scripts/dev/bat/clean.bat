@echo off

cd ../../../

if exist build/ (
	RMDIR  /S /Q build
)

if exist build_fmt/ (
	RMDIR  /S /Q build_fmt
)

if exist build_std/ (
	RMDIR  /S /Q build_std
)

cd scripts/dev/bat