#pragma once

#include <ttyd/dispdrv.h>

#include <cstdint>

namespace ttyd::win_mario {

extern "C" {

void winMarioDisp(ttyd::dispdrv::CameraId cameraId, void *winWorkPtr, int32_t unkIndex);
// fukidashi
// winZenkakuStr
// winMarioMain2
// winMarioMain
// winMarioExit
// winMarioInit2
// winMarioInit

}

}