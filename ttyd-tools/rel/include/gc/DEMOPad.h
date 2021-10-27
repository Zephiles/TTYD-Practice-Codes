#pragma once

#include <cstdint>

namespace gc::DEMOPad {

#define PAD_RESULT_NONE            0
#define PAD_RESULT_NO_CONTROLLER   -1
#define PAD_RESULT_NOT_READY       -2
#define PAD_RESULT_TRANSFER        -3

struct DEMOPadStatus
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
    uint16_t buttonsDown;
    uint16_t buttonsUp;
    uint16_t directions;
    uint16_t directionsNew;
    uint16_t directionsReleased;
    int16_t analogStickDeltaX;
    int16_t analogStickDeltaY;
    int16_t cStickDeltaX;
    int16_t cStickDeltaY;
} __attribute__((__packed__));

static_assert(sizeof(DEMOPadStatus) == 0x1E);

extern "C" {

void DEMOPadInit();
void DEMOPadRead();
// DEMOPadCopy

extern DEMOPadStatus DemoPad[4]; // One for each controller port

}

}