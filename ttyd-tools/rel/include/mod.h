#pragma once

#include <gc/OSModule.h>
#include <gc/OSContext.h>
#include <ttyd/evtmgr.h>
#include <ttyd/npcdrv.h>
#include <ttyd/dispdrv.h>
#include <ttyd/battle_unit.h>
#include <ttyd/mapdata.h>

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
	uint32_t setIndexWarpEntrance(ttyd::evtmgr::EvtEntry *, uint32_t);
	int32_t fixMarioKeyOn();
	bool performRelPatches(gc::OSModule::OSModuleInfo *, void *);
	ttyd::npcdrv::NpcEntry *disableBattles(uint32_t, void *);
	void drawArtAttackHitboxes(ttyd::dispdrv::CameraId);
	int32_t displayActionCommandsTimingHook(void *, ttyd::battle_unit::AttackParams *);
	void errorHandler(uint16_t, gc::OSContext::OSContext *, uint32_t, uint32_t);
	ttyd::npcdrv::NpcEntry *checkForNpcNameToPtrError(const char *);
	ttyd::mapdata::MapData *mapDataPtrHandleUnusedMaps(const char *);
	void _unloadHook(const char *, const char *, const char *);
	void relSetEvtAddrHook(const char *, const void *);

private:
	void (*mPFN_marioStMain_trampoline)() = nullptr;
	void (*mPFN_BattlePadManager_trampoline)() = nullptr;
	int32_t (*mPFN_winRootMain_trampoline)(void *) = nullptr;
	bool (*mPFN_pouchRemoveItemIndex_trampoline)(int16_t, uint32_t) = nullptr;
	bool (*mPFN_battle_init_trampoline)() = nullptr;
	
	int32_t (*mPFN_SoundEfxPlayEx_trampoline)(int32_t, 
		uint32_t, uint32_t, uint32_t) = nullptr;
	
	const char *(*mPFN_msgSearch_trampoline)(const char *) = nullptr;
	uint32_t (*mPFN_scissor_timer_main_trampoline)() = nullptr;
	uint32_t (*mPFN_evt_bero_get_info_trampoline)(ttyd::evtmgr::EvtEntry *, uint32_t) = nullptr;
	int32_t (*mPFN_marioKeyOn_trampoline)() = nullptr;
	bool (*mPFN_OSLink_trampoline)(gc::OSModule::OSModuleInfo *, void *) = nullptr;
	ttyd::npcdrv::NpcEntry *(*mPFN_fbatHitCheck_trampoline)(uint32_t, void *) = nullptr;
	void (*mPFN_scissor_disp_control_trampoline)(ttyd::dispdrv::CameraId) = nullptr;
	
	int32_t (*mPFN_BattleActionCommandCheckDefence_trampoline)(
		void *, ttyd::battle_unit::AttackParams *) = nullptr;
	
	void (*mPFN_systemErrorHandler_trampoline)(uint16_t, 
		gc::OSContext::OSContext *, uint32_t, uint32_t) = nullptr;
	
	ttyd::npcdrv::NpcEntry *(*mPFN_npcNameToPtr_trampoline)(const char *) = nullptr;
	ttyd::mapdata::MapData *(*mPFN_mapDataPtr_trampoline)(const char *) = nullptr;
	void (*mPFN__unload_trampoline)(const char *, const char *, const char *) = nullptr;
	void (*mPFN_relSetEvtAddr_trampoline)(const char *, const void *) = nullptr;
};

}