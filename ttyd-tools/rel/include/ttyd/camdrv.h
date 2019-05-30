#pragma once

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
void camDispOn(uint32_t camId);
// camSetMode
// camEffMain
// cam3dImgMain
// cam3dMain
// camSetCurNo
// camGetCurNo
// camGetCurPtr
void *camGetPtr(uint32_t camId);
// camUnLoadRoad
// camLoadRoad
// camDraw
// camEvalNearFar
// camMain
// camEntryPersp
// camInit

}

}
