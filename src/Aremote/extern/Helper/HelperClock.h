
#pragma once

#include <chrono>

namespace Helper
{
    struct Clock
    {
	typedef std::chrono::high_resolution_clock::time_point TimeWatch;
	typedef std::chrono::seconds TimeSec;
    };
};
#define HClockNow() std::chrono::high_resolution_clock::now()
#define HClockDiff(A,B) std::chrono::duration_cast<std::chrono::milliseconds>(A - B).count()
#define HClockMark(A) [[ maybe_unused ]] auto A = HClockNow()
#define HClockDeclare(A) [[ maybe_unused ]] auto A
