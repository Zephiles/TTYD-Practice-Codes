#ifndef TTYD_STATUSWINDOW_H
#define TTYD_STATUSWINDOW_H

#include "gc/types.h"

#include <cstdint>

// Taken from NWPlayer123's decomp:
// https://github.com/NWPlayer123/PaperMario2/blob/master/include/statuswindow.h
struct StatusWindowWork
{
    uint8_t field_0x0[0x4 - 0x0];
    int16_t field_0x4;
    uint8_t align_0x6[2];
    float field_0x8;
    float field_0xC;
    float field_0x10;
    uint8_t field_0x14[0x18 - 0x14];
    float field_0x18;
    float field_0x1C;
    float field_0x20;
    float field_0x24;
    uint8_t field_0x28[0x30 - 0x28];
    float field_0x30;
    float field_0x34;
    uint8_t field_0x38[0x40 - 0x38];
    float field_0x40;
    float field_0x44;
    uint8_t field_0x48[0x62 - 0x48];
    int16_t field_0x62;
    uint8_t field_0x64[0x68 - 0x64];
    Vec3 field_0x68;
    int32_t field_0x74;
    int32_t field_0x78;
    int32_t field_0x7C;
    int32_t field_0x80;
    uint8_t field_0x84[0x88 - 0x84];
    int32_t field_0x88;
} __attribute__((__packed__));

static_assert(sizeof(StatusWindowWork) == 0x8C);

extern "C"
{
    extern StatusWindowWork *_statusWindowWorkPtr;

    // statusPartyHPBlink
    // statusMarioHPBlink
    // statusFPBlink
    // N_statusClearBlink
    // statusAPBlink
    // statusGetApPos
    // statusWinForceUpdateCoin
    // statusWinForceUpdate
    // statusWinCheckUpdate
    // statusWinCheck
    // statusWinDispOff
    // statusWinDispOn
    // statusWinForceOff
    // statusWinForceCloseClear
    // statusWinForceClose
    // statusWinForceOpen
    // statusWinClose
    // statusWinOpen
    // valueUpdate
    // valueCheck
    // statusGetValue
    void statusWinDisp();
    // gaugeDisp
    // statusWinMain
    // statusWinReInit
    // statusWinInit
}

#endif
