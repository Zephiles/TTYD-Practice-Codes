#ifndef TTYD_MARIO_CAM_H
#define TTYD_MARIO_CAM_H

#include "ttyd/camdrv.h"

#include <cstdint>

extern "C"
{
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
    void marioSetCamId(CameraId cameraId);
    // marioGetCamId
    // marioInitCamId
    // marioResetCamShiftRate
    // marioResetCamShift
}

#endif
