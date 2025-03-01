#ifndef GC_OSTIME_H
#define GC_OSTIME_H

#include "gc/types.h"

#include <cstdint>

extern "C"
{
    OSTime OSGetTime();
    OSTick OSGetTick();
    // __OSGetSystemTime
    // GetDates
    // OSTicksToCalendarTime
}

#endif
