#ifndef TTYD_CAMDRV_H
#define TTYD_CAMDRV_H

#include <cstdint>

enum class CameraId : uint8_t
{
    kOffscreen = 0,
    kOffscreen2,
    kShadow,
    kBackground,
    k3d,
    k3dEffectA,
    k3dImage,
    k3dEffectB,
    k2d,
    kFade,
    kFade2,
    kDebug,
    kDebug3d,
};

extern "C"
{
    // camLetterBox
    // camLetterBoxDraw
    // getScreenPoint
    // camSetTypeOrtho
    // camSetTypePersp
    // camCtrlOff
    // camCtrlOn
    // L_camDispOff
    void L_camDispOn(CameraId id);
    // camSetMode
    // camEffMain
    // cam3dImgMain
    // cam3dMain
    // camSetCurNo
    // camGetCurNo
    // camGetCurPtr
    void *camGetPtr(CameraId id);
    // camUnLoadRoad
    // camLoadRoad
    // camDraw
    // camEvalNearFar
    // camMain
    // camEntryPersp
    // camInit
}

#endif
