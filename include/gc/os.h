#ifndef GC_OS_H
#define GC_OS_H

#include "gc/types.h"

#include <cstdint>

extern "C"
{
    extern uint32_t OSBusClock;
    extern int32_t EXIProbeStartTime[2];
    extern OSTime OSSystemTime;
}

#endif
