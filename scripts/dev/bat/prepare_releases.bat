@echo off

echo -- This Will Now Remove Old Builds, Format Files, Re-build All Vesions, And Move Finished Build Configurations To The Release_Stage Folder
call clean
call gitprep
call build
call build_fmt
call build_std

cd ../../../

echo -- Building Native Version
cmake --build build --config=Debug
cmake --build build --config=MinSizeRel
cmake --build build --config=Release
cmake --build build --config=RelWithDebInfo
echo -- Native Version Has Been Built

echo -- Building fmtlib Version
cmake --build build_fmt --config=Debug
cmake --build build_fmt --config=MinSizeRel
cmake --build build_fmt --config=Release
cmake --build build_fmt --config=RelWithDebInfo
echo -- fmtlib Version Has Been Built

echo -- Building <format> Version
cmake --build build_std --config=Debug
cmake --build build_std --config=MinSizeRel
cmake --build build_std --config=Release
cmake --build build_std --config=RelWithDebInfo
echo -- <format> Version Has Been Built


echo -- Checking For Folder Layout For File Moves
cd ../../../
if not exist Release_Stage/ (
  mkdir Release_Stage
)
pushd Release_Stage  
  if not exist Serenity_Windows_native_x86-64/ (
    mkdir Serenity_Windows_native_x86-64
  )
  pushd Serenity_Windows_native_x86-64
  if not exist Debug/ (
    mkdir Debug
  )
  if not exist MinSizeRel/ (
    mkdir MinSizeRel
  )
  if not exist Release/ (
    mkdir Release
  )
  if not exist RelWithDebInfo/ (
    mkdir RelWithDebInfo
  )
  popd
  
  if not exist Serenity_Windows_fmtlib_x86-64/ (
    mkdir Serenity_Windows_fmtlib_x86-64
  )
  pushd Serenity_Windows_fmtlib_x86-64
  if not exist Debug/ (
    mkdir Debug
  )
  if not exist MinSizeRel/ (
    mkdir MinSizeRel
  )
  if not exist Release/ (
    mkdir Release
  )
  if not exist RelWithDebInfo/ (
    mkdir RelWithDebInfo
  )
  popd
  
  if not exist Serenity_Windows_stdfmt_x86-64/ (
    mkdir Serenity_Windows_stdfmt_x86-64
  )
  pushd Serenity_Windows_stdfmt_x86-64
  if not exist Debug/ (
    mkdir Debug
  )
  if not exist MinSizeRel/ (
    mkdir MinSizeRel
  )
  if not exist Release/ (
    mkdir Release
  )
  if not exist RelWithDebInfo/ (
    mkdir RelWithDebInfo
  )
popd
popd
echo -- Folder Layout Check Finished

echo -- Moving Native Version Configured Builds

cd build/Serenity
move Debug ../../Release_Stage/Serenity_Windows_native_x86-64/
move MinSizeRel ../../Release_Stage/Serenity_Windows_native_x86-64/
move Release ../../Release_Stage/Serenity_Windows_native_x86-64/
move RelWithDebInfo ../../Release_Stage/Serenity_Windows_native_x86-64/
cd ../../

echo -- Moving fmtlib Version Configured Builds

cd build_fmt/Serenity
move Debug ../../Release_Stage/Serenity_Windows_fmtlib_x86-64/
move MinSizeRel ../../Release_Stage/Serenity_Windows_fmtlib_x86-64/
move Release ../../Release_Stage/Serenity_Windows_fmtlib_x86-64/
move RelWithDebInfo ../../Release_Stage/Serenity_Windows_fmtlib_x86-64/
cd ../../

echo -- Moving <format> Version Configured Builds

cd build_std/Serenity
move Debug ../../Release_Stage/Serenity_Windows_stdfmt_x86-64/
move MinSizeRel ../../Release_Stage/Serenity_Windows_stdfmt_x86-64/
move Release ../../Release_Stage/Serenity_Windows_stdfmt_x86-64/
move RelWithDebInfo ../../Release_Stage/Serenity_Windows_stdfmt_x86-64/
cd ../../

echo -- All Configured Builds Have Been Moved To The Root Folder 'Release_Stage'



