LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE    := aremote
LOCAL_CPP_EXTENSION := .cpp
LOCAL_SRC_FILES := ARemote.cpp App.Cmdl.cpp App.Config.cpp App.Error.cpp App.IsRunning.cpp Event.Display.Image.cpp Event.Display.Stream.cpp Event.Display.StreamRawData.cpp Event.Display.cpp Event.Input.Key.cpp Event.Input.Macro.cpp Event.Input.Swipe.cpp Event.Input.Tap.cpp Event.Input.Text.cpp Event.Input.cpp Net.HTTPServer.cpp Net.IP.cpp extern/Helper/HelperIntrin.cpp extern/Helper/HelperIntrin/NDK/cpu-features.c extern/Helper/HelperLog.cpp extern/Helper/HelperXmlParse.cpp extern/ImageLite/ImageLite.Error.cpp extern/ImageLite/ImageLite.Formater.cpp extern/ImageLite/ImageLite.Formater.icompare.avx2.cpp extern/ImageLite/ImageLite.Formater.icompare.cpp extern/ImageLite/ImageLite.Formater.icompare.neon.cpp.neon extern/ImageLite/ImageLite.Formater.iconverter.cpp extern/ImageLite/ImageLite.Formater.iconverter.native.cpp extern/ImageLite/ImageLite.LuaSol3.cpp extern/ImageLite/ImageLite.RGBbuffer.cpp extern/Lua/Lua.Engine.cpp extern/Lua/Lua.Error.cpp extern/Lua/Lua.Method.cpp extern/lodepng.cpp extern/lz4/lz4.c extern/toojpeg.cpp extern/xml/tinyxml2.cpp
LOCAL_CFLAGS := -D_DEBUG=1 -funroll-loops -ffunction-sections -fopenmp -Wall -Wunused -Wunreachable-code -Wno-unknown-pragmas -Wl,--export-dynamic -Wl,--version-script,fakelib/vs-libsigchain.txt -D__ANDROID_VER__=$(APP_BUILD_VERSION) -D__ANDROID_API_PLATFORM__="$(TARGET_PLATFORM)"
LOCAL_CPPFLAGS := -std=c++17 -fexceptions -frtti
LOCAL_LDFLAGS := -static-openmp -L./fakelib/lib/$(APP_PLATFORM)/$(TARGET_ARCH_ABI) -L../../../../__LIB__/LUA/lib/$(APP_PLATFORM)/$(TARGET_ARCH_ABI)
LOCAL_LDLIBS := -lutils -lgui -lui -lbinder -lomp -llua535sa -llog
LOCAL_C_INCLUDES := fakelib/include fakelib/include/ndk ../../../../__LIB__/android-aosp-header/android-6.0.0_r1/system/core/include ../../../../__LIB__/android-aosp-header/android-6.0.0_r1/frameworks/base/include ../../../../__LIB__/android-aosp-header/android-6.0.0_r1/frameworks/native/include ../../../../__LIB__/android-aosp-header/android-6.0.0_r1/hardware/libhardware/include ../../../../__LIB__/ASIO/include ../../../../__LIB__/ASIO/include/asio ../../../../__LIB__/LUA/include ../../../../__LIB__/sol2/single/include ./ extern extern/Lua extern/Helper extern/ImageLite ./  extern  extern/Helper  extern/Helper/HelperIntrin/NDK  extern/ImageLite  extern/Lua  extern/lz4  extern/sws  extern/xml
CBP2NDK_CMDLINE :=
#LOCAL_ALLOW_UNDEFINED_SYMBOLS := true
LOCAL_MODULE_TAGS := eng
LIBCXX_FORCE_REBUILD := false
LOCAL_CPP_FEATURES := rtti
LOCAL_MODULE_PATH := $(TARGET_OUT)/dist
include $(BUILD_EXECUTABLE)
