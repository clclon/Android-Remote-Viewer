LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := liblua535a
LOCAL_INSTALLED_MODULE_STEM := liblua535a.so
LOCAL_CPP_EXTENSION := .cpp
LOCAL_SRC_FILES := src/lapi.cpp src/lcode.cpp src/lctype.cpp src/ldebug.cpp src/ldo.cpp src/ldump.cpp src/lfunc.cpp src/lgc.cpp src/llex.cpp src/lmem.cpp src/lobject.cpp src/lopcodes.cpp src/lparser.cpp src/lstate.cpp src/lstring.cpp src/ltable.cpp src/ltm.cpp src/lundump.cpp src/lvm.cpp src/lzio.cpp src/lauxlib.cpp src/lbaselib.cpp src/lbitlib.cpp src/lcorolib.cpp src/ldblib.cpp src/liolib.cpp src/lmathlib.cpp src/loslib.cpp src/lstrlib.cpp src/ltablib.cpp src/lutf8lib.cpp src/loadlib.cpp src/linit.cpp
LOCAL_CFLAGS := -O2 -Wall -Wextra -Wno-unknown-pragmas -DLUA_BUILD_AS_DLL -DLUA_COMPAT_5_2 -DLUA_LIB -D__ANDROID_API_PLATFORM__="$(TARGET_PLATFORM)"
LOCAL_CPPFLAGS := -std=c++17 -frtti -fexceptions -fdeclspec -fms-extensions
LOCAL_LDFLAGS := -s
LOCAL_LDLIBS :=
LOCAL_C_INCLUDES := ./
CBP2NDK_CMDLINE :=
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib/$(APP_PLATFORM)/$(TARGET_ARCH_ABI)
include $(BUILD_SHARED_LIBRARY)
