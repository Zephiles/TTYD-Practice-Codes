#pragma once

#include <cstdint>

namespace mod {

class Mod
{
public:
	Mod();
	void init();
	
private:
	void run();
	void performBattleChecks();
	int32_t pauseMenuPreventUnpause(void *);
	bool infiniteItemUsage(int16_t, uint32_t);
	bool performPreBattleActions();
	int32_t preventMenuSounds(int32_t, uint32_t, uint32_t, uint32_t);
	const char *getCustomMessage(const char *);
	uint32_t pauseArtAttackTimer();
	uint32_t setIndexWarpEntrance(void *, uint32_t);

private:
	void (*mPFN_marioStMain_trampoline)() = nullptr;
	void (*mPFN_BattlePadManager_trampoline)() = nullptr;
	int32_t (*mPFN_winRootMain_trampoline)(void *) = nullptr;
	bool (*mPFN_pouchRemoveItemIndex_trampoline)(int16_t, uint32_t) = nullptr;
	bool (*mPFN_battle_init_trampoline)() = nullptr;
	int32_t (*mPFN_SoundEfxPlayEx_trampoline)(int32_t, uint32_t, 
		uint32_t, uint32_t) = nullptr;
	const char *(*mPFN_msgSearch_trampoline)(const char *) = nullptr;
	uint32_t (*mPFN_scissor_timer_main_trampoline)() = nullptr;
	uint32_t (*mPFN_evt_bero_get_info_trampoline)(void *, uint32_t) = nullptr;
};

}