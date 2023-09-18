#ifndef GC_EXI_H
#define GC_EXI_H

#include "gc/OSContext.h"

#include <cstdint>

typedef void (*EXICallback)(int32_t channel, OSContext *context);

struct EXIQueue
{
    uint32_t dev;
    EXICallback callback;
} __attribute__((__packed__));

struct EXIControl
{
    EXICallback exiCallback;
    EXICallback tcCallback;
    EXICallback extCallback;
    uint32_t state;
    int32_t immLength;
    uint8_t *immBuffer;
    uint32_t dev;
    uint32_t id;
    int32_t idTime;
    int32_t items;
    EXIQueue queue[3];
} __attribute__((__packed__));

static_assert(sizeof(EXIQueue) == 0x8);
static_assert(sizeof(EXIControl) == 0x40);

extern "C"
{
    extern EXIControl Ecb[3];

    // SetExiInterruptMask
    // EXIImm
    // EXIImmEx
    // EXIDma
    // EXISync
    // EXIClearInterrupts
    // EXISetExiCallback
    bool __EXIProbe(int32_t channel);
    // EXIProbe
    // EXIProbeEx
    // EXIAttach
    // EXIDetach
    // EXISelect
    // EXIDeselect
    // EXIIntrruptHandler
    // TCIntrruptHandler
    // EXTIntrruptHandler
    // EXIInit
    // EXILock
    // EXIUnlock
    // EXIGetState
    // UnlockedHandler
    // EXIGetID
    // ProbeBarnacle
    // __OSEnableBarnacle
    // InitializeUART
    // WriteUARTN

    // Manually-written functions
    void EXIProbeReset();
}

#endif
