
#pragma once

#if defined(_DEBUG_IMAGEPACK_STAT) || defined(_BUILD_STREAMPACK_STAT)
#  define CLKWATCH     TimeWatch
#  define CLKNOW()     HClockNow()
#  define CLKDIFF(A,B) HClockDiff(A,B)
#  define CLKMARK(A)   HClockMark(A)
#else
#  define CLKWATCH
#  define CLKNOW()   0U;
#  define CLKDIFF(A,B)
#  define CLKMARK(A)
#endif
