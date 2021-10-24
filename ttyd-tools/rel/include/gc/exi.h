#pragma once

#include <cstdint>

namespace gc::exi {

struct ECB
{
    uint8_t unk_0[0x20];
    uint32_t unk_20;
    uint8_t unk_24[0x3C];
    uint32_t unk_60;
    uint8_t unk_64[0x5C];
} __attribute__((__packed__));

static_assert(sizeof(ECB) == 0xC0);

extern "C" {

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

extern ECB Ecb;

// Manually-written functions
void EXIProbeReset();

}

}