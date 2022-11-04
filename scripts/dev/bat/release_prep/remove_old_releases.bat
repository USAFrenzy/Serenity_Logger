@echo off

cd ../../../../

if exist build_native_release_16/ (
	RMDIR  /S /Q build_native_release_16
)

if exist build_native_release_17/ (
	RMDIR  /S /Q build_native_release_17
)

if exist build_fmt_release_16/ (
	RMDIR  /S /Q build_fmt_release_16
)

if exist build_fmt_release_17/ (
	RMDIR  /S /Q build_fmt_release_17
)

if exist build_std_release_16/ (
	RMDIR  /S /Q build_std_release_16
)

if exist build_std_release_17/ (
	RMDIR  /S /Q build_std_release_17
)

if exist Release_Stage/ (
	RMDIR  /S /Q Release_Stage
)

cd scripts/dev/bat/release_prep