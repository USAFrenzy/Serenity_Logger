@echo off
pushd ..
call devenv.exe "build/Serenity_Sandbox.sln"
popd
exit