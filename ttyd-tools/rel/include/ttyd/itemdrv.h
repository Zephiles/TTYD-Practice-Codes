#pragma once

#include <cstdint>

namespace ttyd::itemdrv {

struct ItemDrvWork
{
	uint32_t entries;
	void *wItemsInField;
	uint8_t unk_8[48];
} __attribute__((__packed__));

static_assert(sizeof(ItemDrvWork) == 0x38);

extern "C" {

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

void *itemEntry(const char *name, int32_t id, uint32_t mode, 
	int32_t wasCollectedExpr, void *pickupScript, float x, float y, float z);

// itemModeChange
// itemMain
// itemGetNokoCheck
// itemGetCheck
// itemCoinDrop
// itemReInit
// itemInit

extern ItemDrvWork itemDrvWork;

}

}