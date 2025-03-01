#ifndef TTYD_CAMDRV_H
#define TTYD_CAMDRV_H

#include "gc/types.h"

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

// Taken from NWPlayer123's decomp:
// https://github.com/NWPlayer123/PaperMario2/blob/master/include/drv/camdrv.h
struct CameraEntry;
typedef void (*CameraCallback)(CameraEntry *entry);

struct CameraEntry
{
    uint16_t flags;
    uint16_t mode;
    uint16_t field_0x4;
    uint16_t field_0x6; // padding?
    float field_0x8;
    Vec3 cameraPos;
    Vec3 target;
    Vec3 cameraUp;
    float near;
    float far;
    float fovY;
    float aspect;
    uint8_t field_0x40[0xF4 - 0x40];
    uint16_t mScissor[4]; // TODO retype?
    float mProjection[6]; // TODO retype?
    float field_0x114;
    float field_0x118;
    mtx34 view;
    float bankRotation;
    Vec3 postTranslation;
    mtx44 projection;
    uint32_t gxProjectionType; // GXProjectionType type
    uint8_t field_0x1A0[0x1E8 - 0x1A0];
    uint32_t field_0x1E8;
    CameraCallback callback;
    Vec3 field_0x1F0;
    uint32_t field_0x1FC; // unknown
    Vec3 field_0x200;
    Vec3 field_0x20C;
    uint16_t field_0x218;
    uint16_t field_0x21A;
    uint16_t field_0x21C;
    uint16_t field_0x21E;
    uint32_t field_0x220; // unknown
    Vec3 field_0x224;
    Vec3 field_0x230;
    Vec3 field_0x23C;
    Vec3 field_0x248;
    char name[0xC];
} __attribute__((__packed__));

static_assert(sizeof(CameraEntry) == 0x260);

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
    CameraEntry *camGetPtr(CameraId id);
    // camUnLoadRoad
    // camLoadRoad
    // camDraw
    // camEvalNearFar
    // camMain
    // camEntryPersp
    // camInit
}

#endif
