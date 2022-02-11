#pragma once

#include <ttyd/seqdrv.h>

#include <cstdint>

namespace ttyd::seq_title {

extern "C" {

// pressStartGX
// disp2
// disp1
void titleMain(ttyd::seqdrv::SeqInfo *seqInfo);
// L_titleInit
// N_getDebugMode
// seq_titleMain
// seq_titleExit
// seq_titleInit
// DbgBtlSel_GetMsgDataPtr

extern void *seqTitleWorkPointer2;

}

}