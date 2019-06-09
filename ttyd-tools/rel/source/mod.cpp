#include "mod.h"
#include "global.h"
#include "codes.h"
#include "patch.h"

#include <ttyd/mariost.h>
#include <ttyd/battle_pad.h>
#include <ttyd/win_root.h>
#include <ttyd/mario_pouch.h>
#include <ttyd/seq_battle.h>
#include <ttyd/sound.h>
#include <ttyd/msgdrv.h>
#include <ttyd/sac_scissor.h>
#include <ttyd/evt_bero.h>
#include <ttyd/mario.h>
#include <ttyd/fontmgr.h>
#include <ttyd/windowdrv.h>
#include <ttyd/seq_logo.h>

#include <cstdio>

namespace mod {

Mod *gMod = nullptr;

void main()
{
	Mod *mod = new Mod();
	mod->init();
}

Mod::Mod()
{
	
}

void Mod::init()
{
	gMod = this;
	initMenuVars();
	initAddressOverwrites();
	actionCommandsTimingsInit();
	
	mPFN_marioStMain_trampoline = patch::hookFunction(
		ttyd::mariost::marioStMain, []()
	{
		gMod->run();
	});
	
	mPFN_BattlePadManager_trampoline = patch::hookFunction(
		ttyd::battle_pad::BattlePadManager, []()
	{
		gMod->performBattleChecks();
	});
	
	mPFN_winRootMain_trampoline = patch::hookFunction(
		ttyd::win_root::winRootMain, [](void *pauseMenuPointer)
	{
		return gMod->pauseMenuPreventUnpause(pauseMenuPointer);
	});
	
	mPFN_pouchRemoveItemIndex_trampoline = patch::hookFunction(
		ttyd::mario_pouch::pouchRemoveItemIndex, [](int16_t item, uint32_t index)
	{
		return gMod->infiniteItemUsage(item, index);
	});
	
	mPFN_battle_init_trampoline = patch::hookFunction(
		ttyd::seq_battle::battle_init, []()
	{
		return gMod->performPreBattleActions();
	});
	
	mPFN_SoundEfxPlayEx_trampoline = patch::hookFunction(
		ttyd::sound::SoundEfxPlayEx, [](int32_t soundId, 
			uint32_t unk1, uint32_t unk2, uint32_t unk3)
	{
		return gMod->preventMenuSounds(soundId, unk1, unk2, unk3);
	});
	
	mPFN_msgSearch_trampoline = patch::hookFunction(
		ttyd::msgdrv::msgSearch, [](const char *key)
	{
		return gMod->getCustomMessage(key);
	});
	
	mPFN_scissor_timer_main_trampoline = patch::hookFunction(
		ttyd::sac_scissor::scissor_timer_main, []()
	{
		return gMod->pauseArtAttackTimer();
	});
	
	mPFN_evt_bero_get_info_trampoline = patch::hookFunction(
		ttyd::evt_bero::evt_bero_get_info, [](void *event, uint32_t waitMode)
	{
		return gMod->setIndexWarpEntrance(event, waitMode);
	});
	
	mPFN_marioKeyOn_trampoline = patch::hookFunction(
		ttyd::mario::marioKeyOn, []()
	{
		return gMod->fixMarioKeyOn();
	});

	// Initialize typesetting early
	ttyd::fontmgr::fontmgrTexSetup();
	patch::hookFunction(ttyd::fontmgr::fontmgrTexSetup, [](){});
	
	// Initialize typesetting early
	ttyd::windowdrv::windowTexSetup();
	patch::hookFunction(ttyd::windowdrv::windowTexSetup, [](){});

	// Skip the logo
	/*patch::hookFunction(ttyd::seq_logo::seq_logoMain, [](ttyd::seqdrv::SeqInfo *)
	{
		ttyd::seqdrv::seqSetSeq(ttyd::seqdrv::SeqIndex::kTitle, nullptr, nullptr);
	});*/
}

}