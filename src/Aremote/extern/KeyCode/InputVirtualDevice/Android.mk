LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := inputvirtualdevice
LOCAL_CPP_EXTENSION := .cpp
LOCAL_SRC_FILES := main.cpp
LOCAL_CFLAGS := -Wall -Wno-unknown-pragmas -fexceptions -D__ANDROID_API_PLATFORM__="$(TARGET_PLATFORM)" -D_BUILD_SINGLE=1 -D__ANDROID_VER__=$(APP_BUILD_VERSION) -D__ANDROID_API_PLATFORM__="$(TARGET_PLATFORM)"
LOCAL_CPPFLAGS := -std=c++17 -frtti
LOCAL_LDFLAGS := -v -L../../../fakelib/libs/$(TARGET_ARCH_ABI)
LOCAL_LDLIBS := -lutils -lbinder -llog
LOCAL_C_INCLUDES := C:/__BuildSource/__LIB__/android-aosp-header/android-6.0.0_r1/system/core/include C:/__BuildSource/__LIB__/android-aosp-header/android-6.0.0_r1/frameworks/base/include C:/__BuildSource/__LIB__/android-aosp-header/android-6.0.0_r1/frameworks/native/include C:/__BuildSource/__LIB__/android-aosp-header/android-6.0.0_r1/hardware/libhardware/include C:/__BuildSource/__LIB__/ASIO/include C:/__BuildSource/__LIB__/ASIO/include/asio ../../sws ../../.. ./
CBP2NDK_CMDLINE :=
include $(BUILD_EXECUTABLE)
