#pragma once

#include <cstdint>

namespace gc::pad {

#define PAD_RESULT_NONE            0
#define PAD_RESULT_NO_CONTROLLER   -1
#define PAD_RESULT_NOT_READY       -2
#define PAD_RESULT_TRANSFER        -3

struct PadStatus
{
    uint16_t buttons;
    int8_t analogStickX;
    int8_t analogStickY;
    int8_t cStickX;
    int8_t cStickY;
    uint8_t analogL;
    uint8_t analogR;
    uint8_t analogA;
    uint8_t analogB;
    int8_t error; // see PAD_RESULT defines above
    uint8_t padding;
} __attribute__((__packed__));

static_assert(sizeof(PadStatus) == 0xC);

extern "C" {

// ClampStick
// PADClamp
// UpdateOrigin
// PADOriginCallback
// PADOriginUpdateCallback
// PADProbeCallback
// PADTypeAndStatusCallback
// PADReceiveCheckCallback
// PADReset
// PADRecalibrate
// PADInit
// PADRead
// U_PADControlMotor
// PADSetSpec
// SPEC0_MakeStatus
// SPEC1_MakeStatus
// SPEC2_MakeStatus
// OnReset
// SamplingHandler
// PADSetSamplingCallback
// __PADDisableRecalibration

}

}