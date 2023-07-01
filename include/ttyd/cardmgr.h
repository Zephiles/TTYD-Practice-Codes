#ifndef TTYD_CARDMGR_H
#define TTYD_CARDMGR_H

#include "gc/card.h"

#include <cstdint>

struct CardMgrWork
{
    uint16_t wFlags;
    uint16_t unk_2;
    void *workArea; // Used for CARDMountAsync
    int32_t memCardSlot;
    uint32_t unk_C;
    uint32_t unk_10;
    uint32_t unk_14;
    uint32_t unk_18;
    CARDFileInfo fileInfo;
    CARDStat stat;
    int32_t code; // Return value from CARD functions
    uint32_t unk_A0;
    uint32_t unk_A4;
    void *fileData; // Place where all 4 files are loaded to for the file select menu
    void *unk_AC;
    void *unk_B0;
    uint32_t unk_B4;
    uint32_t unk_B8;
    uint32_t unk_BC;
    uint32_t unk_C0;
    uint32_t unk_C4;
    uint32_t unk_C8;
    uint32_t unk_CC;
    uint32_t unk_D0;
    uint32_t unk_D4;
    uint32_t unk_D8;
    uint32_t unk_DC;
    uint32_t unk_E0;
    uint32_t unk_E4;
    uint32_t unk_E8;
    uint32_t unk_EC;
} __attribute__((__packed__));

static_assert(sizeof(CardMgrWork) == 0xF0);

extern "C"
{
    // memcard_open
    // statusCallback
    // readCallback
    // writeCallback
    // createCallback
    // formatCallback
    // checkCallback
    // mountAttachCallback
    // mountDetachCallback
    // cardFormat
    // create_main
    // cardCreate
    // read_all_main
    // cardReadAll
    // read_main
    // write_header_main
    // cardWriteHeader
    // write_main
    // cardWrite
    // cardCopy
    // cardErase
    // cardCopy2Main
    // cardMain
    // cardGetCode
    bool cardIsExec();
    // cardInit
    // cardBufReset
    // cardGetFilePtr

    extern CardMgrWork *cardMgrWorkPtr;
}

#endif
