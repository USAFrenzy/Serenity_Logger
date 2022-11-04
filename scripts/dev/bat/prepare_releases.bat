 @echo off

 :: This Will Now Remove Old Builds, Format Files, Re-build All Vesions, 
 :: Move Finished Build Configurations To The Release_Stage Folder And 
 :: Then Archive Them.
 :: ------------------------------- NOTE -------------------------------
 :: - For this script to be successful, 7zip cli needs to be downloaded
 ::   and the 7za.exe executable needs to be present on the environment
 ::   path variable 

 call gitprep
 
 pushd release_prep
 
 call remove_old_releases
 call build_native_release_VS16
 call build_native_release_VS17
 
 call build_fmt_release_VS16
 call build_fmt_release_VS17
 
 call build_std_release_VS16
 call build_std_release_VS17
 popd
 
 
 cd ../../../
 
 
 :: ---------------------------------------------------------------
 echo -- Building Native Versions
 cmake --build build_native_release_16 --config=Debug
 cmake --build build_native_release_16 --config=MinSizeRel
 cmake --build build_native_release_16 --config=Release
 cmake --build build_native_release_16 --config=RelWithDebInfo
 
 cmake --build build_native_release_17 --config=Debug
 cmake --build build_native_release_17 --config=MinSizeRel
 cmake --build build_native_release_17 --config=Release
 cmake --build build_native_release_17 --config=RelWithDebInfo
 echo -- Native Versions Has Been Built
 :: ---------------------------------------------------------------
 echo -- Building fmtlib Versions
 cmake --build build_fmt_release_16 --config=Debug
 cmake --build build_fmt_release_16 --config=MinSizeRel
 cmake --build build_fmt_release_16 --config=Release
 cmake --build build_fmt_release_16 --config=RelWithDebInfo
 
 cmake --build build_fmt_release_17 --config=Debug
 cmake --build build_fmt_release_17 --config=MinSizeRel
 cmake --build build_fmt_release_17 --config=Release
 cmake --build build_fmt_release_17 --config=RelWithDebInfo
 echo -- fmtlib Versions Has Been Built
 :: ---------------------------------------------------------------
 echo -- Building <format> Versions
 cmake --build build_std_release_16 --config=Debug
 cmake --build build_std_release_16 --config=MinSizeRel
 cmake --build build_std_release_16 --config=Release
 cmake --build build_std_release_16 --config=RelWithDebInfo
 
 cmake --build build_std_release_17 --config=Debug
 cmake --build build_std_release_17 --config=MinSizeRel
 cmake --build build_std_release_17 --config=Release
 cmake --build build_std_release_17 --config=RelWithDebInfo
 echo -- <format> Versions Has Been Built
 :: ---------------------------------------------------------------
 
 echo -- Checking For Folder Layout For File Moves
 if not exist Release_Stage/ (
   mkdir Release_Stage
 )
 pushd Release_Stage  
 :: -------------------- NATIVE VERSIONS --------------------
   if not exist Serenity_Windows_native_x86-64_VS16/ (
     mkdir Serenity_Windows_native_x86-64_VS16
   )
   pushd Serenity_Windows_native_x86-64_VS16
   if not exist lib/ (
     mkdir lib
   )
   pushd lib  
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
     xcopy /e /i /y "../../Serenity/include" "include"
   popd
   :: -----------------------------------------------------
   if not exist Serenity_Windows_native_x86-64_VS17/ (
     mkdir Serenity_Windows_native_x86-64_VS17
   )
   pushd Serenity_Windows_native_x86-64_VS17
   if not exist lib/ (
     mkdir lib
   )
   pushd lib  
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
     xcopy /e /i /y "../../Serenity/include" "include"
   popd
 
 :: -------------------- FMTLIB VERSIONS --------------------
   if not exist Serenity_Windows_fmtlib_x86-64_VS16/ (
     mkdir Serenity_Windows_fmtlib_x86-64_VS16
   )
   pushd Serenity_Windows_fmtlib_x86-64_VS16
     if not exist lib/ (
     mkdir lib
   )
   pushd lib
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
    xcopy /e /i /y "../../Serenity/include" "include"
   popd
   :: -----------------------------------------------------
 if not exist Serenity_Windows_fmtlib_x86-64_VS17/ (
     mkdir Serenity_Windows_fmtlib_x86-64_VS17
   )
   pushd Serenity_Windows_fmtlib_x86-64_VS17
     if not exist lib/ (
     mkdir lib
   )
   pushd lib
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
    xcopy /e /i /y "../../Serenity/include" "include"
   popd
 
 :: -------------------- STDFMT VERSIONS --------------------
   if not exist Serenity_Windows_stdfmt_x86-64_VS16/ (
     mkdir Serenity_Windows_stdfmt_x86-64_VS16
   )
   pushd Serenity_Windows_stdfmt_x86-64_VS16
       if not exist lib/ (
     mkdir lib
   )
   pushd lib
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
   xcopy /e /i /y "../../Serenity/include" "include"
 popd 
   :: -----------------------------------------------------
   if not exist Serenity_Windows_stdfmt_x86-64_VS17/ (
     mkdir Serenity_Windows_stdfmt_x86-64_VS17
   )
   pushd Serenity_Windows_stdfmt_x86-64_VS17
       if not exist lib/ (
     mkdir lib
   )
   pushd lib
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
     xcopy /e /i /y "../../Serenity/include" "include"
   popd 
 popd  
 
 echo -- Folder Layout Check Finished
:: -----------------------------------------------------
:: -----------------------------------------------------
:: -----------------------------------------------------
echo -- Moving Native Version Configured Builds
:: -----------------------------------------------------

cd build_native_release_16/Serenity
xcopy /e /i /y "Debug" "../../Release_Stage/Serenity_Windows_native_x86-64_VS16/lib/Debug"
xcopy /e /i /y "MinSizeRel" "../../Release_Stage/Serenity_Windows_native_x86-64_VS16/lib/MinSizeRel"
xcopy /e /i /y "Release" "../../Release_Stage/Serenity_Windows_native_x86-64_VS16/lib/Release"
xcopy /e /i /y "RelWithDebInfo" "../../Release_Stage/Serenity_Windows_native_x86-64_VS16/lib/RelWithDebInfo"
cd ../../

cd build_native_release_17/Serenity
xcopy /e /i /y "Debug" "../../Release_Stage/Serenity_Windows_native_x86-64_VS17/lib/Debug"
xcopy /e /i /y "MinSizeRel" "../../Release_Stage/Serenity_Windows_native_x86-64_VS17/lib/MinSizeRel"
xcopy /e /i /y "Release" "../../Release_Stage/Serenity_Windows_native_x86-64_VS17/lib/Release"
xcopy /e /i /y "RelWithDebInfo" "../../Release_Stage/Serenity_Windows_native_x86-64_VS17/lib/RelWithDebInfo"
cd ../../
:: -----------------------------------------------------


echo -- Moving fmtlib Version Configured Builds
:: -----------------------------------------------------
cd build_fmt_release_16/Serenity
xcopy /e /i /y "Debug" "../../Release_Stage/Serenity_Windows_fmtlib_x86-64_VS16/lib/Debug"
xcopy /e /i /y "MinSizeRel" "../../Release_Stage/Serenity_Windows_fmtlib_x86-64_VS16/lib/MinSizeRel"
xcopy /e /i /y "Release" "../../Release_Stage/Serenity_Windows_fmtlib_x86-64_VS16/lib/Release"
xcopy /e /i /y "RelWithDebInfo" "../../Release_Stage/Serenity_Windows_fmtlib_x86-64_VS16/lib/RelWithDebInfo"
cd ../../

cd build_fmt_release_17/Serenity
xcopy /e /i /y "Debug" "../../Release_Stage/Serenity_Windows_fmtlib_x86-64_VS17/lib/Debug"
xcopy /e /i /y "MinSizeRel" "../../Release_Stage/Serenity_Windows_fmtlib_x86-64_VS17/lib/MinSizeRel"
xcopy /e /i /y "Release" "../../Release_Stage/Serenity_Windows_fmtlib_x86-64_VS17/lib/Release"
xcopy /e /i /y "RelWithDebInfo" "../../Release_Stage/Serenity_Windows_fmtlib_x86-64_VS17/lib/RelWithDebInfo"
cd ../../
:: -----------------------------------------------------

echo -- Moving <format> Version Configured Builds
:: -----------------------------------------------------
cd build_std_release_16/Serenity
xcopy /e /i /y "Debug" "../../Release_Stage/Serenity_Windows_stdfmt_x86-64_VS16/lib/Debug"
xcopy /e /i /y "MinSizeRel" "../../Release_Stage/Serenity_Windows_stdfmt_x86-64_VS16/lib/MinSizeRel"
xcopy /e /i /y "Release" "../../Release_Stage/Serenity_Windows_stdfmt_x86-64_VS16/lib/Release"
xcopy /e /i /y "RelWithDebInfo" "../../Release_Stage/Serenity_Windows_stdfmt_x86-64_VS16/lib/RelWithDebInfo"
cd ../../

cd build_std_release_17/Serenity
xcopy /e /i /y "Debug" "../../Release_Stage/Serenity_Windows_stdfmt_x86-64_VS17/lib/Debug"
xcopy /e /i /y "MinSizeRel" "../../Release_Stage/Serenity_Windows_stdfmt_x86-64_VS17/lib/MinSizeRel"
xcopy /e /i /y "Release" "../../Release_Stage/Serenity_Windows_stdfmt_x86-64_VS17/lib/Release"
xcopy /e /i /y "RelWithDebInfo" "../../Release_Stage/Serenity_Windows_stdfmt_x86-64_VS17/lib/RelWithDebInfo"
cd ../../
:: -----------------------------------------------------

echo -- All Configured Builds Have Been Moved To The Root Folder 'Release_Stage'

echo -- Now Archiving Configured Builds
pushd release_prep
call archive_releases
popd
echo -- All Configured Builds Have Been Archived

