#
#    MIT License
#
#    Code::Blocks + Android NDK project template
#    Copyright (c) 2016-2020 PS
#    HOWTO: https://clclon.github.io/Code-Blocks-Android-NDK/
#
#    - No root device!
#    - Build Debug (NDK toolchain)
#    - Build Release (NDK toolchain)
#    - Run from device
#    - Debug from device. 
#    - Auto start/stop gdbserverfrom device.
#    - No wrappers from Gradle/Java code required, works directly with the device.  
#    - Advanced project export to Android.mk file format, possible using utility cbp2ndk. 
#
#    Permission is hereby granted, free of charge, to any person obtaining a copy
#    of this software and associated documentation files (the "Software"), to deal
#    in the Software without restriction, including without limitation the rights
#    to use, copy, modify, merge, publish, distribute, sub license, and/or sell
#    copies of the Software, and to permit persons to whom the Software is
#    furnished to do so, subject to the following conditions:
#
#    The above copyright notice and this permission notice shall be included in all
#    copies or substantial portions of the Software.
#
#    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#    SOFTWARE.

# Variables PLATFORM* to Application.mk

PLATFORM_ABI := $(SC_NDK_ABI)
PLATFORM_API := $(SC_NDK_API)
PLATFORM_STL := c++_static
PLATFORM_SCRIPT := Android.mk
PLATFORM_OPTIM  :=

-include Application.mk
ifneq ($(APP_BUILD_SCRIPT),)
	PLATFORM_SCRIPT = $(APP_BUILD_SCRIPT)
endif
ifneq ($(APP_ABI),)
	PLATFORM_ABI = $(APP_ABI)
endif
ifneq ($(APP_PLATFORM),)
	PLATFORM_API = $(APP_PLATFORM)
endif
ifneq ($(APP_STL),)
	PLATFORM_STL = $(APP_STL)
endif
ifneq ($(APP_OPTIM),)
	PLATFORM_OPTIM = $(APP_OPTIM)
endif
-include $(PLATFORM_SCRIPT)

# Variables local

DIR_UTILS    := $(SC_BIN_PATH)
DIR_NDKROOT  := $(SC_NDK_DIR)
DIR_PROJECT  := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
STR_BUILDTAG := $(filter-out $@,$(MAKECMDGOALS))
STR_BUILDOPT :=

ifeq ($(OS),Windows_NT)
	DIR_CONFCBP  := $(shell dir *.cbp | findstr .cbp)
else
	DIR_CONFCBP  := $(shell find $("./") -name '*.cbp')
endif

ifneq ($(ANALIZE),)
	STR_BUILDOPT += NDK_ANALIZE=1
endif

ifeq ($(STR_BUILDTAG),Debug)
	STR_BUILDOPT += V=1 NDK_DEBUG=1
	PLATFORM_OPTIM = debug
else
	STR_BUILDOPT += -j 4
endif

all: setndk checkabi buildndk
Debug: setndk checkabi buildndk adbsetup adbdebug buildscript rundebug
Release: setndk checkabi buildndk adbsetup adbexec buildscript
cleanDebug: setndk checkabi clean
cleanRelease: setndk checkabi clean
cleanall: setndk checkabi clean

setndk:
	@echo '==== SET C::B NDK -> $(SC_NDK_API) -> $(SC_NDK_ABI) -> active device: [ $(PLATFORM_ABI) ] ===='
	@$(DIR_UTILS)/exec/cbp2ndk32.exe -t $(STR_BUILDTAG) -c $(DIR_CONFCBP) --api $(PLATFORM_API) --abi $(PLATFORM_ABI) --ndkopt $(PLATFORM_OPTIM) -include $(PLATFORM_SCRIPT)

buildndk:
	@echo '==== Build $(STR_BUILDTAG) -> $(PLATFORM_ABI) platform -> $(SC_NDK_API) API ===='
	@Cmd.exe /C $(DIR_NDKROOT)\ndk-build.cmd NDK_APPLICATION_MK=$(DIR_PROJECT)Application.mk NDK_PROJECT_PATH=$(DIR_PROJECT) $(STR_BUILDOPT)

clean:
	@echo '==== Clean ===='
	@Cmd.exe /C $(DIR_NDKROOT)\ndk-build.cmd NDK_APPLICATION_MK=$(DIR_PROJECT)Application.mk NDK_PROJECT_PATH=$(DIR_PROJECT) clean
	@Cmd.exe /C adb.exe shell rm -f /data/local/tmp/$(LOCAL_MODULE)

adbsetup:
	@echo '==== ADB SETUP: [ $(PLATFORM_ABI) ] [ $(LOCAL_MODULE) ] ===='
ifeq ($(shell expr $(APP_BUILD_VERSION) \< 8), 1)
	@echo '==== Version $(APP_BUILD_VERSION) needed clean binary header: [ $(PLATFORM_ABI)/$(LOCAL_MODULE) ] ===='
	Cmd.exe /C $(DIR_UTILS)/exec/android-elf-cleaner32.exe $(DIR_PROJECT)libs\$(PLATFORM_ABI)\$(LOCAL_MODULE)
endif
	@Cmd.exe /C adb.exe push $(DIR_PROJECT)libs\$(PLATFORM_ABI)\$(LOCAL_MODULE) /data/local/tmp/$(LOCAL_MODULE)
	@Cmd.exe /C adb.exe shell /system/bin/chmod 0777 /data/local/tmp/$(LOCAL_MODULE)

adbexec:
	@echo '==== ADB RUN: [ $(PLATFORM_ABI) ] ===='
ifeq ($(shell expr $(APP_BUILD_VERSION) \< 7), 1)
	@Cmd.exe /C adb.exe shell /data/local/tmp/$(LOCAL_MODULE) $(CBP2NDK_CMDLINE)
else
	@Cmd.exe /C adb.exe exec-out /data/local/tmp/$(LOCAL_MODULE) $(CBP2NDK_CMDLINE)
endif

adbdebug:
	@echo '==== GDB Debug: [ $(PLATFORM_ABI) ] ===='
	@Cmd.exe /C adb.exe push $(DIR_PROJECT)libs\$(PLATFORM_ABI)\gdb.setup /data/local/tmp/gdb.setup
	@Cmd.exe /C adb.exe push $(DIR_PROJECT)libs\$(PLATFORM_ABI)\gdbserver /data/local/tmp/gdbserver
	@Cmd.exe /C adb.exe shell /system/bin/chmod 0777 /data/local/tmp/gdbserver

rundebug:
	@Cmd.exe /C RunRemote.cmd

buildscript:
ifeq (,$(wildcard ./RunRemote.cmd))
	@echo '==== CREATE RUN SCRIPT: [ $(PLATFORM_ABI) ] ===='
ifeq ($(shell expr $(APP_BUILD_VERSION) \< 7), 1)
	@echo "adb.exe shell /data/local/tmp/$(LOCAL_MODULE) $(CBP2NDK_CMDLINE)" >RunRemote.cmd
else
	@echo "adb.exe exec-out /data/local/tmp/$(LOCAL_MODULE) $(CBP2NDK_CMDLINE)" >RunRemote.cmd
endif
endif
ifeq (,$(wildcard ./DebugRemote.cmd))
	@echo '==== CREATE RUN REMOTE DEBUGGER SCRIPT: [ $(PLATFORM_ABI) ] ===='
	@echo "adb.exe forward tcp:59999 tcp:59999" >DebugRemote.cmd
	@echo "start \"$(PLATFORM_ABI) GDB server\" /MIN adb.exe shell /data/local/tmp/gdbserver :59999 /data/local/tmp/$(LOCAL_MODULE) $(CBP2NDK_CMDLINE)" >>DebugRemote.cmd
	@echo "exit" >>DebugRemote.cmd
endif

checkabi:
ifeq ($(PLATFORM_ABI),all)
	$(error Bad Android PLATFORM ABI, not run in 'all' value. Set 'APP_ABI' in file Application.mk)
endif


.PHONY: clean all
