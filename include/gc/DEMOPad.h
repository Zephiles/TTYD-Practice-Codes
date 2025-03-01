#ifndef GC_DEMOPAD_H
#define GC_DEMOPAD_H

#include "gc/pad.h"

#include <cstdint>

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
    int8_t error; // see PAD_RESULT defines in pad.h
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

extern "C"
{
    extern DEMOPadStatus DemoPad[4]; // One for each controller port
    extern PadStatus Pad[4];         // One for each controller port

    void DEMOPadInit();
    void DEMOPadRead();
    // DEMOPadCopy
}

#endif
