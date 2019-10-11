#pragma once

#include <ttyd/dispdrv.h>

#include <cstdint>

namespace ttyd::camdrv {

extern "C" {

// camLetterBox
// camLetterBoxDraw
// getScreenPoint
// camSetTypeOrtho
// camSetTypePersp
// camCtrlOff
// camCtrlOn
// camDispOff
void camDispOn(ttyd::dispdrv::CameraId cameraId);
// camSetMode
// camEffMain
// cam3dImgMain
// cam3dMain
// camSetCurNo
// camGetCurNo
// camGetCurPtr
void *camGetPtr(ttyd::dispdrv::CameraId cameraId);
// camUnLoadRoad
// camLoadRoad
// camDraw
// camEvalNearFar
// camMain
// camEntryPersp
// camInit

}

}
