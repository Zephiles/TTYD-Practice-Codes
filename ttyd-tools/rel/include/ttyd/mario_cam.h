#pragma once

#include <ttyd/dispdrv.h>

#include <cstdint>

namespace ttyd::mario_cam {

extern "C" {

// marioCamZoomUpLevelMain
// marioCamZoomUp
// marioCamZoomOff
// marioCamZoomOffReq2
// marioCamZoomOffReq
// camFollowYOff
// camFollowYOn
// marioNoUpdateCamPos
// marioUpdateCamPos
// marioUpdateCam
// marioUpdateCamCollision
// marioCamMain
// marioPreCamera
// marioGetCamFollowRate
// marioSetCamFollowRate
// marioResetCamFollowRate
void marioSetCamId(ttyd::dispdrv::CameraId cameraId);
// marioGetCamId
// marioInitCamId
// marioResetCamShiftRate
// marioResetCamShift

}

}
