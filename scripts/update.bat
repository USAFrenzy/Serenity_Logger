@echo off
popd
  git pull
pushd
call clang_format
call cmake_format
call build

cd ../
