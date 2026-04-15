#ifndef TTYD_WINMGR_H
#define TTYD_WINMGR_H

#include <cstdint>

// Taken from the doldecomp repo:
// https://github.com/doldecomp/ttyd/blob/main/include/manager/winmgr.h
struct WinMgrDesc;
struct WinMgrEntry;

struct WinMgrDesc
{
    uint8_t field_0x0[0x8 - 0x0];
    int32_t camId; // CameraId
    int32_t field_0xC;
    int32_t field_0x10;
    int32_t field_0x14;
    int32_t field_0x18;
    uint8_t field_0x1C[0x20 - 0x1C];
    void (*main)(WinMgrEntry *entry);
    void *disp;
} __attribute__((__packed__));

struct WinMgrEntry
{
    int32_t flags;
    int32_t field_0x4;
    int32_t field_0x8;
    uint8_t field_0xC[0x10 - 0xC];
    float scale;
    float rotation;
    int32_t field_0x18;
    int32_t field_0x1C;
    int32_t field_0x20;
    int32_t field_0x24;
    WinMgrDesc *description;
    void *param;
    int32_t priority;
    int32_t field_0x34;
    int32_t field_0x38;
    int32_t field_0x3C;
    uint8_t field_0x40[0x44 - 0x40];
} __attribute__((__packed__));

struct WinMgrWork
{
    uint32_t mNumEntries;
    WinMgrEntry *mEntries;
} __attribute__((__packed__));

static_assert(sizeof(WinMgrDesc) == 0x28);
static_assert(sizeof(WinMgrEntry) == 0x44);
static_assert(sizeof(WinMgrWork) == 0x8);

extern "C"
{
    extern WinMgrWork winMgrWork;

    // select_disp3_mario
    // select_disp3_party
    // select_disp3
    // select_main3
    // select_disp2
    // select_disp_luigi
    // select_disp_mario
    // select_disp_party
    // select_disp
    // select_main
    // winMgrSelectDelete
    // winMgrSelectOther
    // winMgrSelect
    // winMgrSelectEntry
    // winMgrHelpDraw
    // winMgrHelpMain
    // winMgrHelpInit
    // winMgrGetPtr
    // winMgrSetPriority
    // winMgrSetSize
    // winMgrAction
    // winMgrDelete
    // winMgrCloseAutoDelete
    // winMgrClose
    // winMgrOpen
    // winMgrSetParam
    // winMgrEntry
    // winMgrSeq
    // winMgrMain
    // winMgrDisp
    // winMgrReInit
    // winMgrInit
}

#endif
