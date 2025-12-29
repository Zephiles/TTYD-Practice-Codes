#ifndef TTYD_SYSTEM_H
#define TTYD_SYSTEM_H

#include "gc/types.h"
#include "gc/pad.h"

#include <cstdint>

enum StickInput
{
    C_STICK_INPUT_UP = 0x100,
    C_STICK_INPUT_DOWN = 0x200,
    C_STICK_INPUT_LEFT = 0x400,
    C_STICK_INPUT_RIGHT = 0x800,
    ANALOG_STICK_INPUT_UP = 0x1000,
    ANALOG_STICK_INPUT_DOWN = 0x2000,
    ANALOG_STICK_INPUT_LEFT = 0x4000,
    ANALOG_STICK_INPUT_RIGHT = 0x8000,
};

extern "C"
{
    void *memcpy_as4(void *dst, void *src, uint32_t size);
    // mtxGetRotationElement

    float sysMsec2FrameFloat(float milliseconds);
    uint32_t sysMsec2Frame(uint32_t milliseconds);
    float sysFrame2SecFloat(float frames);
    // getV60FPS
    void sysDummyDraw(mtx34 matrix);
    void sysWaitDrawSync();
    uint16_t sysGetToken();

    uint32_t irand(uint32_t range);
    void movePos(float magnitude, float direction, float *x, float *y);
    void sincosf(float angle, float *sin, float *cos);

    uint8_t padGetRumbleStatus(PadId id);
    void padRumbleHardOff(PadId id);
    void padRumbleOff(PadId id);
    void padRumbleOn(PadId id);

    uint8_t keyGetSubStickY(PadId id);
    uint8_t keyGetStickY(PadId id);
    uint8_t keyGetStickX(PadId id);
    uint32_t keyGetButtonTrg(PadId id);
    uint32_t keyGetDirTrg(PadId id);
    uint32_t keyGetButtonRep(PadId id);
    uint32_t keyGetDirRep(PadId id);
    uint32_t keyGetButton(PadId id);
    uint32_t keyGetDir(PadId id);
    void makeKey();

    // qqsort
    // intplGetValue
    // angleABf
    // compAngle
    // distABf
    // reviseAngle
    const char *getMarioStDvdRoot();
}

#endif
