#pragma once

#include <cstdint>

namespace ttyd::fadedrv {

extern "C" {

// fadeSetNarrowFast
// fadeGetTpl
// fadeReset
// fadeIsFinish
// disp_dummy_black
// disp_tec
// disp_maku
// zFill
// z
// disp_texture
// fadeMain
// fadeSetOffscreenCallback
// fadeSetAnimVirtualPos
// fadeSetAnimOfsPos
// fadeSetSpotPos
// fadeWhiteOutOff
// fadeWhiteOutOn
// fadeSoftFocusOff
// fadeSoftFocusOn
// fadeTecSoftOff
// fadeTecSoftOn
// fadeTecOff
// fadeTecOn
void fadeEntry(int32_t type, int32_t duration, uint8_t color[4]);
// fadeTexSetup
// fadeInit
// _callback

}

}
