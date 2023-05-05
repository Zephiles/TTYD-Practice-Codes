#ifndef TTYD_DISPDRV_H
#define TTYD_DISPDRV_H

#include "ttyd/camdrv.h"

#include <cstdint>

typedef void (*DispCallback)(CameraId cameraId, void *user);

struct DisplayWork
{
    CameraId cameraId;
    uint8_t renderMode;
    uint16_t padding_2;
    float order;
    DispCallback callback;
    void *user;
} __attribute__((__packed__));

static_assert(sizeof(DisplayWork) == 0x10);

extern "C"
{
    void dispInit();
    void dispReInit();
    void dispEntry(CameraId cameraId, uint8_t renderMode, float order, DispCallback callback, void *user);
    void dispSort();
    void dispDraw(CameraId cameraId);
    // float dispCalcZ(void *vecUnk);
    DisplayWork *dispGetCurWork();
}

#endif
