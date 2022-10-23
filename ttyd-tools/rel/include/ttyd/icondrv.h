#pragma once

#include <gc/mtx.h>

#include <cstdint>

namespace ttyd::icondrv {

extern "C" {

// iconNumberDispGx3D
void iconNumberDispGx(gc::mtx::mtx34 mtx, int32_t number, bool isTextSmall, uint8_t color[4]);
// iconSetAlpha
// iconSetScale
// iconFlagOff
// iconFlagOn
// iconSetPos
// iconNameToPtr
// iconGX
// iconGetWidthHight
// iconGetTexObj
// iconDispGxCol
// iconDispGx2
void iconDispGx(gc::mtx::Vec3 *position, uint16_t unkBits, int16_t iconNum, float scale);
void iconDispGxAlpha(gc::mtx::Vec3 *position, uint16_t unkBits, int16_t iconNum, uint8_t alpha, float scale);
// iconDisp
// iconChange
// iconDelete
// iconEntry2D
// iconEntry
// iconMain
// iconReInit
// iconTexSetup
// iconInit
// _callback_bin
// _callback_tpl

}

}