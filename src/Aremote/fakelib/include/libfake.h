
#pragma once

#if !defined(__ANDROID__)
#  error "__ANDROID__ build only !"
#endif

#include <string>

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define LOG(...) fprintf(stderr, __VA_ARGS__)
#define LOGPF() fprintf(stderr, __PRETTY_FUNCTION__)
#define ATTR_UNUSED __attribute__ (( __unused__ ))
#define BUILD_FAKE_LIBRARY 1

typedef int32_t status_t;

namespace android {

template <typename T> class sp {
public:
    T* m_ptr;
};
    
}

