#ifndef GC_OSINTERRUPT_H
#define GC_OSINTERRUPT_H

#include <cstdint>

extern "C"
{
    bool OSDisableInterrupts();
    // OSEnableInterrupts
    bool OSRestoreInterrupts(bool enable);
    // __OSSetInterruptHandler
    // __OSGetInterruptHandler
    // __OSInterruptInit
    // SetInterruptMask
    // __OSMaskInterrupts
    // __OSUnmaskInterrupts
    // __OSDispatchInterrupt
    // ExternalInterruptHandler
}

#endif
