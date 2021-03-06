
PLATFORM := armeabi-v7a
NDKROOT  := E:\__BuildSource\__LIB__\64\android-ndk-r21
PROJECT  := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
BUILDTAG := $(filter-out $@,$(MAKECMDGOALS))
BUILDOPT := 
BINBIT   :=

include Application.mk
include $(APP_BUILD_SCRIPT)

ifeq ($(OS),Windows_NT)
	CONFCBP  := $(shell dir /B *.cbp | findstr .cbp)
	ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
		BINBIT = 64
	endif
	ifeq ($(PROCESSOR_ARCHITECTURE),x86)
		BINBIT = 32
	endif
else
	CONFCBP  := $(shell find $("./") -name '*.cbp')
	BINBIT = 32
endif

ifneq ($(APP_ABI),all)
	PLATFORM = $(APP_ABI)
endif

ifneq ($(ANALIZE),)
	BUILDOPT += NDK_ANALIZE=1
endif

ifeq ($(BUILDTAG),Debug)
	BUILDOPT += V=1 NDK_DEBUG=1
else
	BUILDOPT += -j 4
endif

all: allndk
Debug: allndk adbsetup adbdebug buildscript rundebug
Release: allndk adbsetup
# Release: allndk adbsetup adbexec buildscript
Docs:
cleanDocs:
cleanDebug: clean
cleanRelease: clean
cleanall: clean

allndk:
	@echo '==== Build $(BUILDTAG) - $(APP_ABI) platform - active device: [ $(PLATFORM) ] ===='
ifeq ($(BINBIT),64)
	@cbp2ndk64.exe $(BUILDTAG) $(CONFCBP)
else
	@cbp2ndk32.exe $(BUILDTAG) $(CONFCBP)
endif
	@Cmd.exe /C $(NDKROOT)\ndk-build.cmd NDK_APPLICATION_MK=$(PROJECT)Application.mk NDK_PROJECT_PATH=$(PROJECT) $(BUILDOPT)

clean:
	@echo '==== Clean ===='
	@Cmd.exe /C $(NDKROOT)\ndk-build.cmd NDK_APPLICATION_MK=$(PROJECT)Application.mk NDK_PROJECT_PATH=$(PROJECT) clean
	@Cmd.exe /C adb.exe shell rm -f /data/local/tmp/$(LOCAL_MODULE)

adbsetup:
	@echo '==== ADB SETUP: [ $(PLATFORM) ] ===='
ifeq ($(shell expr $(APP_BUILD_VERSION) \< 8), 1)
	@echo '==== Version $(APP_BUILD_VERSION) needed clean binary header: [ $(PLATFORM)/$(LOCAL_MODULE) ] ===='
ifeq ($(BINBIT),64)
	@Cmd.exe /C android-elf-cleaner64.exe $(PROJECT)libs\$(PLATFORM)\$(LOCAL_MODULE)
else
	@Cmd.exe /C android-elf-cleaner32.exe $(PROJECT)libs\$(PLATFORM)\$(LOCAL_MODULE)
endif
endif
	@Cmd.exe /C adb.exe push $(PROJECT)libs\$(PLATFORM)\$(LOCAL_MODULE) /data/local/tmp/$(LOCAL_MODULE)
	@Cmd.exe /C adb.exe shell /system/bin/chmod 0777 /data/local/tmp/$(LOCAL_MODULE)

adbexec:
	@echo '==== ADB RUN: [ $(PLATFORM) ] ===='
	@readelf -d $(PROJECT)libs\$(PLATFORM)\$(LOCAL_MODULE)
	@Cmd.exe /C adb.exe shell /data/local/tmp/$(LOCAL_MODULE) $(CBP2NDK_CMDLINE)

adbdebug:
	@echo '==== GDB Debug: [ $(PLATFORM) ] ===='
	@Cmd.exe /C adb.exe push $(PROJECT)libs\$(PLATFORM)\gdb.setup /data/local/tmp/gdb.setup
	@Cmd.exe /C adb.exe push $(PROJECT)libs\$(PLATFORM)\gdbserver /data/local/tmp/gdbserver
	@Cmd.exe /C adb.exe shell /system/bin/chmod 0777 /data/local/tmp/gdbserver

rundebug:
	@Cmd.exe /C RunRemote.cmd

buildscript:
ifeq (,$(wildcard ./RunRemote.cmd))
ifeq ($(shell expr $(APP_BUILD_VERSION) \< 7), 1)
	@echo "adb.exe shell /data/local/tmp/$(LOCAL_MODULE) $(CBP2NDK_CMDLINE)" >RunRemote.cmd
else
	@echo "adb.exe exec-out /data/local/tmp/$(LOCAL_MODULE) $(CBP2NDK_CMDLINE)" >RunRemote.cmd
endif
endif
ifeq (,$(wildcard ./DebugRemote.cmd))
	@echo "adb.exe forward tcp:59999 tcp:59999" >DebugRemote.cmd
	@echo "start \"$(PLATFORM) GDB server\" /MIN adb.exe shell /data/local/tmp/gdbserver :59999 /data/local/tmp/$(LOCAL_MODULE) $(CBP2NDK_CMDLINE)" >>DebugRemote.cmd
	@echo "exit" >>DebugRemote.cmd
endif

.PHONY: clean all

