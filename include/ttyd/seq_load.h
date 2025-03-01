#ifndef TTYD_SEQ_LOAD_H
#define TTYD_SEQ_LOAD_H

#include "ttyd/seqdrv.h"

#include <cstdint>

extern "C"
{
    // continueGame
    // loadDraw
    // loadMain
    void seq_loadMain(SeqWork *seqWork);
    // seq_loadExit
    // seq_loadInit
}
#endif
