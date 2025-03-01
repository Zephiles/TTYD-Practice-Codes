#ifndef TTYD_SEQ_TITLE_H
#define TTYD_SEQ_TITLE_H

#include "ttyd/seqdrv.h"

#include <cstdint>

// Taken from NWPlayer123's decomp:
// https://github.com/NWPlayer123/PaperMario2/blob/master/include/seq/seq_title.h
struct SeqTitleWork2
{
    void *texture;                   // FileEntry* texture
    uint8_t field_0x4[0x8 - 0x4];    // 0x4
    int32_t field_0x8;               // 0x8
    float field_0xC;                 // 0xC
    float field_0x10;                // 0x10
    float field_0x14;                // 0x14
    float field_0x18;                // 0x18
    float field_0x1C;                // 0x1C
    uint8_t field_0x20[0x30 - 0x20]; // 0x20
    int32_t debug;                   // 0x30
} __attribute__((__packed__));

static_assert(sizeof(SeqTitleWork2) == 0x34);

extern "C"
{
    extern SeqTitleWork2 *seqTitleWorkPtr2;

    // pressStartGX
    // disp2
    // disp1
    void titleMain(SeqWork *seqWork);
    // L_titleInit
    // N_getDebugMode
    // seq_titleMain
    // seq_titleExit
    // seq_titleInit
    // DbgBtlSel_GetMsgDataPtr
}

#endif
