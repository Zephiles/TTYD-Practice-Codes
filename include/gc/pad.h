#ifndef GC_PAD_H
#define GC_PAD_H

#include <cstdint>

#define PAD_RESULT_NONE 0
#define PAD_RESULT_NO_CONTROLLER -1
#define PAD_RESULT_NOT_READY -2
#define PAD_RESULT_TRANSFER -3

enum class PadId
{
    CONTROLLER_ONE = 0,
    CONTROLLER_TWO,
    CONTROLLER_THREE,
    CONTROLLER_FOUR,
};

enum PadInput
{
    PAD_DPAD_LEFT = 0x0001,
    PAD_DPAD_RIGHT = 0x0002,
    PAD_DPAD_DOWN = 0x0004,
    PAD_DPAD_UP = 0x0008,
    PAD_Z = 0x0010,
    PAD_R = 0x0020,
    PAD_L = 0x0040,
    // unused = 0x0080,
    PAD_A = 0x0100,
    PAD_B = 0x0200,
    PAD_X = 0x0400,
    PAD_Y = 0x0800,
    PAD_START = 0x1000,
};

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

extern "C"
{
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

#endif
