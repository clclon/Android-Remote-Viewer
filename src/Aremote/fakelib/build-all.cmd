@echo off

rename Application.mk Application.mk.orig

CALL :BuildNDK android-21 5
CALL :BuildNDK android-27 8
CALL :BuildNDK android-28 9

move Application.mk.orig Application.mk
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

del /Q /S obj libs
@Cmd.exe /C E:\__BuildSource\__LIB__\64\android-ndk-r21\ndk-build.cmd NDK_APPLICATION_MK=./Application.mk NDK_PROJECT_PATH=./ APP_BUILD_VERSION=%_verapi% -j 4

move libs\armeabi-v7a\*.so lib\%_platform%\armeabi-v7a
move libs\arm64-v8a\*.so lib\%_platform%\arm64-v8a
move libs\x86\*.so lib\%_platform%\x86
move libs\x86_64\*.so lib\%_platform%\x86_64

ENDLOCAL
EXIT /B 0

exit
