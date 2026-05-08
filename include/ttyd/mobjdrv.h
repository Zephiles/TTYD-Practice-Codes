#ifndef TTYD_MOBJDRV_H
#define TTYD_MOBJDRV_H

#include "gc/types.h"
#include "gc/mtx.h"
#include "gc/gx.h"
#include "ttyd/hitdrv.h"

#include <cstdint>

// Taken from the doldecomp repo:
// https://github.com/doldecomp/ttyd/blob/main/include/driver/mobjdrv.h
struct MapObjectHitEntry
{
    HitEntry *hitObject;
    uint8_t unk4[0x88 - 0x4];
} __attribute__((__packed__));

struct MapObjectEntry
{
    int32_t flags;
    uint8_t unk4[0x38 - 0x4];
    Vec3 position;
    Vec3 scale;
    Vec3 rotation;
    uint8_t unk5C[0x70 - 0x5C];
    int32_t poseId;
    MapObjectHitEntry hitObj[2];
    Vec3 scale2;
    uint8_t unk190[0x19C - 0x190];
    int32_t offscreenId;
    uint8_t unk1A0[0x1E8 - 0x1A0];
    GXTexObj paperTexObj;
    void *unk208;
    mtx34 paperTexMtx;
} __attribute__((__packed__));

struct MapObjectWork
{
    int32_t count;
    MapObjectEntry *entries;
    uint8_t field_0x8[0x1C - 0x8];
} __attribute__((__packed__));

static_assert(sizeof(MapObjectHitEntry) == 0x88);
static_assert(sizeof(MapObjectEntry) == 0x23C);
static_assert(sizeof(MapObjectWork) == 0x1C);

extern "C"
{
    extern MapObjectWork mobjWork[3]; // Index 0 used for field, index 1 used for battles, index 2 used for Bowser stages
    extern BOOL koopaRunFlag;         // Set to true when in a Bowser stage

    // mobjNearDistCheck2
    // mobjCheckItemboxOpen
    // mobjCheckExec
    // mobjGetHint
    // mobjCalcMtx2
    // mobjCalcMtx
    // mobjRunEvent
    // mobjHitObjPtrToPtr
    // mobjNameToPtrNoAssert
    MapObjectEntry *mobjNameToPtr(const char *mobjName);
    // mobjMain
    // mobjSetPosition
    // mobjDelete
    // mobjEntry
    // mobjHitEntry
    // mobjReset
    // mobjInit
    // mobjDisp_OffscreenXLU
    // mobjDisp
    // mobjDispXLU
    // mobjKoopaOn
}

#endif
