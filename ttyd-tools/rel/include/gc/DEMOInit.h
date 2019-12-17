#pragma once

#include <cstdint>

namespace gc::DEMOInit {

extern "C" {

// __DEMODiagnoseHang
// __NoHangDoneRender
// __NoHangRetraceCallback
// DEMOEnableGPHangWorkaround
void *DEMOGetCurrentBuffer();
// DEMOGetRenderModeObj
// DEMODoneRender
// DEMOBeforeRender
// DEMOInit

extern void *DemoCurrentBuffer;
extern void *DemoFrameBuffer2;
extern void *DemoFrameBuffer1;

}

}