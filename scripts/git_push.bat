@echo off

call cmake_format
call clang_format
pushd ..
 git add .
 git commit -m "auto commit update"
 git push
popd