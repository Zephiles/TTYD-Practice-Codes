#pragma once

#include <cstdint>

namespace ttyd::effdrv {

struct EffEntry
{
	uint32_t wFlags;
	uint32_t wbBattle;
	uint32_t field_8;
	void *pEffAllocation;
	void *pfnMainFunc;
	char *typeName;
	char wName[16];
} __attribute__((__packed__));

static_assert(sizeof(EffEntry) == 0x28);

struct EffWork
{
	uint32_t effCount;
	EffEntry *entries;
	uint32_t unk_8;
	uint32_t unk_c;
	uint32_t unk_10;
	uint32_t unk_14;
} __attribute__((__packed__));

static_assert(sizeof(EffWork) == 0x18);

extern "C" {

// effDrawMayaPoly
// effCalcMayaAnim
// effPlayMayaAnim
// effDeleteMayaAnim
// effMayaAnimAlloc
// effCalcMayaAnimMatrix
// effGetSet
// effNameToPtr
// effSoftDelete
// effDelete
// effMain
// effSetName
// effEntry
// effAutoRelease
// effGetTexObj
// effTexSetup
// effInit
// _callback_tpl

extern EffWork *effWorkPointer;

}

}