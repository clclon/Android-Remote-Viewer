
#pragma once

#include <iomanip>
#include <algorithm>
#include <atomic>
#include <memory>
#include <tuple>
#include <array>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>
#include <utility>
#include <future>
#include <functional>
#include <exception>
#include <shared_mutex>
#include <system_error>
#include <optional>
#include <cmath>
#include <ctime>
#include <cstdlib>

#if defined(_DEBUG)
#  if defined(_DEBUG_ASIO_TRACKING)
#    define ASIO_ENABLE_HANDLER_TRACKING
#  endif
#endif

#if defined(__WIN32__)
// for ASIO test in windows
#  define _WIN32_WINDOWS 1
#  define _WIN32_WINNT 0x0601
#endif

#include <asio.hpp>

#include <binder/IBinder.h>
#include <gui/ISurfaceComposer.h>
#include <gui/SurfaceComposerClient.h>
#include <system/graphics.h>

#include <binder/ProcessState.h>
#include <binder/IPCThreadState.h>

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>

#if !defined(_DEBUG)
#  if defined(_BUILD_STRICT)
#    undef _BUILD_STRICT
#  endif
#endif

#if !defined(USE_STANDALONE_ASIO)
#  define USE_STANDALONE_ASIO
#endif
#include <asio.hpp>
#include "extern/sws/server_http.hpp"
#include "extern/toojpeg.h"
#include "extern/lodepng.h"

#include "version.h"
#include "extern/Helper/HelperLog.h"
#include "extern/Helper/HelperArray2D.h"
#include "extern/Helper/HelperAutoShared.h"
#include "extern/Helper/HelperClock.h"
#include "extern/Helper/HelperMap.h"
#include "extern/Helper/HelperXml.h"
#include "extern/Helper/HelperXmlParse.h"
#include "App.Error.h"
#include "App.Cmdl.h"
#include "Event.Display.h"
#include "Event.Input.h"
#include "Net.IP.h"
#include "Net.HTTPServer.h"
#include "App.IsRunning.h"
#include "App.Config.h"
#include "extern/ImageLite/ImageLite.h"
#include "extern/Lua/Lua.h"

#define __DEFAULT_HTTP_PORT 53777
#define __NELE(a) (sizeof(a) / sizeof(a[0]))
