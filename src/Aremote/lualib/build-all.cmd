@echo off

rename Application.mk Application.mk.orig
rename Android.mk Android.mk.orig

CALL :BuildNDK android-21 5
CALL :BuildNDK android-27 8
CALL :BuildNDK android-28 9

move Application.mk.orig Application.mk
move Android.mk.orig Android.mk
rmdir /S /Q obj libs
exit

:BuildNDK
SETLOCAL
SET _platform=%1
SET _verapi=%2

mkdir lib\%_platform%\armeabi-v7a
mkdir lib\%_platform%\arm64-v8a
mkdir lib\%_platform%\x86
mkdir lib\%_platform%\x86_64

echo APP_ABI := all >Application.mk
echo APP_STL := c++_static >>Application.mk
echo APP_BUILD_SCRIPT := Android.mk >>Application.mk
echo APP_PLATFORM := %_platform% >>Application.mk
echo APP_BUILD_VERSION := %_verapi% >>Application.mk

copy Android-static.mk Android.mk
del /Q /S obj libs
@Cmd.exe /C E:\__BuildSource\__LIB__\64\android-ndk-r21\ndk-build.cmd NDK_APPLICATION_MK=./Application.mk NDK_PROJECT_PATH=./ APP_BUILD_VERSION=%_verapi% -j 4

move obj\local\armeabi-v7a\liblua535sa.a lib\%_platform%\armeabi-v7a
move obj\local\arm64-v8a\liblua535sa.a lib\%_platform%\arm64-v8a
move obj\local\x86\liblua535sa.a lib\%_platform%\x86
move obj\local\x86_64\liblua535sa.a lib\%_platform%\x86_64

copy Android-shared.mk Android.mk
del /Q /S obj libs
@Cmd.exe /C E:\__BuildSource\__LIB__\64\android-ndk-r21\ndk-build.cmd NDK_APPLICATION_MK=./Application.mk NDK_PROJECT_PATH=./ APP_BUILD_VERSION=%_verapi% -j 4

move libs\armeabi-v7a\liblua535a.so lib\%_platform%\armeabi-v7a
move libs\arm64-v8a\liblua535a.so lib\%_platform%\arm64-v8a
move libs\x86\liblua535a.so lib\%_platform%\x86
move libs\x86_64\liblua535a.so lib\%_platform%\x86_64

ENDLOCAL
EXIT /B 0

exit
