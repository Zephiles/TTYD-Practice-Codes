#pragma once

#include <ttyd/seqdrv.h>

#include <cstdint>

namespace ttyd::seq_logo {

extern "C" {

void seq_logoInit(ttyd::seqdrv::SeqInfo *info);
void seq_logoExit(ttyd::seqdrv::SeqInfo *info);
void seq_logoMain(ttyd::seqdrv::SeqInfo *info);

}

}