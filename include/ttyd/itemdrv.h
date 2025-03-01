#ifndef TTYD_ITEMDRV_H
#define TTYD_ITEMDRV_H

#include "gc/types.h"
#include "ttyd/effdrv.h"

#include <cstdint>

// Taken from NWPlayer123's decomp:
// https://github.com/NWPlayer123/PaperMario2/blob/master/include/drv/itemdrv.h
struct ItemEntry
{
    uint16_t flags;
    uint16_t field_0x2;
    int32_t itemId;
    int32_t evtId;
    char name[0x10];
    EffEntry *effect;
    uint8_t field_0x20[0x24 - 0x20];
    uint16_t mode;
    uint16_t field_0x26;
    uint8_t field_0x28[0x3C - 0x28];
    Vec3 position;
    float scale;
    uint8_t alpha;
    uint8_t field_0x4D[0x68 - 0x4D];
    OSTime field_0x68;
    uint8_t field_0x70[0x7C - 0x70];
    int32_t wDespawnTimer;
    uint8_t field_0x80[0x98 - 0x80];
} __attribute__((__packed__));

struct ItemWork
{
    int32_t count;
    ItemEntry *entries;
    uint32_t currId; // Next available itemId
    uint8_t field_0xC[0x1C - 0xC];
} __attribute__((__packed__));

static_assert(sizeof(ItemEntry) == 0x98);
static_assert(sizeof(ItemWork) == 0x1C);

extern "C"
{
    extern ItemWork itemDrvWork[2]; // Index 0 used for field, index 1 used for battles

    // itemPickUp
    // itemStatus
    // itemStatusOn
    // itemFlag
    // itemFlagOff
    // itemFlagOn
    // itemNokoForceGet
    // itemForceGet
    // itemHitCheckSide
    // itemSetPosition
    // itemseq_Bound
    // itemseq_GetItem
    // winFullDisp
    // winHelpDisp
    // winHelpMain
    // winNameDisp2
    // winNameDisp
    // itemNearDistCheck
    // itemHitCheck
    // itemNameToPtr
    // itemDelete

    void *itemEntry(const char *name,
                    int32_t itemId,
                    uint32_t mode,
                    int32_t wasCollectedExpr,
                    void *pickupScript,
                    float x,
                    float y,
                    float z);

    // itemModeChange
    // itemMain
    // N_itemPickUpFromFieldCheck
    // itemGetNokoCheck
    // itemGetCheck
    // itemCoinDrop
    // itemReInit
    // itemInit
}

#endif
