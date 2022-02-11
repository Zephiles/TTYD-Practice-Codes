#pragma once

#include <ttyd/seqdrv.h>

#include <cstdint>

namespace ttyd::seq_load {

extern "C" {

// continueGame
// loadDraw
// loadMain
void seq_loadMain(ttyd::seqdrv::SeqInfo *seqInfo);
// seq_loadExit
// seq_loadInit

}

}