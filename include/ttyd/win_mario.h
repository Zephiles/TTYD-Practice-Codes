#ifndef TTYD_WIN_MARIO_H
#define TTYD_WIN_MARIO_H

#include "ttyd/camdrv.h"

#include <cstdint>

extern "C"
{
    void winMarioDisp(CameraId cameraId, void *winWorkPtr, int32_t index);
    // fukidashi
    // winZenkakuStr
    // winMarioMain2
    // winMarioMain
    // winMarioExit
    // winMarioInit2
    // winMarioInit
}

#endif
