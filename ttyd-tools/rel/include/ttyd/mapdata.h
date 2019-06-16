#pragma once

#include <cstdint>

namespace ttyd::mapdata {

struct WorldData
{
	void *unk_0;
	void *unk_4;
	void *unk_8;
	void *unk_c;
	void *unk_10;
	void *unk_14;
	void *unk_18;
	void *unk_1c;
	void *unk_20;
	void *unk_24;
	void *unk_28;
	void *unk_2c;
	void *unk_30;
	void *unk_34;
	void *unk_38;
	void *unk_3c;
	void *unk_40;
	void *unk_44;
	void *unk_48;
	void *unk_4c;
	void *unk_50;
	void *unk_54;
	void *unk_58;
	void *unk_5c;
	void *unk_60;
	void *unk_64;
	void *unk_68;
	void *unk_6c;
	void *unk_70;
	void *unk_74;
	void *unk_78;
	void *unk_7c;
	void *unk_80;
	void *unk_84;
	void *unk_88;
	uint8_t unk_8c[124];
} __attribute__((__packed__));

static_assert(sizeof(WorldData) == 0x108);

extern "C" {

// relSetBtlAddr
// relSetEvtAddr
// areaDataPtr
// mapDataPtr

extern WorldData worldData;

}

}