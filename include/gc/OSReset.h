#ifndef GC_OSRESET_H
#define GC_OSRESET_H

#include <cstdint>

extern "C"
{
    // OSRegisterResetFunction
    // __OSCallResetFunctions
    // Reset
    // KillThreads
    // __OSDoHotReset
    // OSResetSystem
    uint32_t OSGetResetCode();
}

#endif
