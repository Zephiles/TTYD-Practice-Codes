#include "mod.h"
#include "rel_shrink.h"
#include "global.h"
#include "patch.h"
#include "assembly.h"

#include <gc/OSModule.h>
#include <gc/OSContext.h>
#include <gc/DEMOPad.h>
#include <ttyd/mariost.h>
#include <ttyd/battle_pad.h>
#include <ttyd/win_root.h>
#include <ttyd/mario_pouch.h>
#include <ttyd/seq_battle.h>
#include <ttyd/sound.h>
#include <ttyd/msgdrv.h>
#include <ttyd/sac_scissor.h>
#include <ttyd/evt_bero.h>
#include <ttyd/evtmgr.h>
#include <ttyd/mario.h>
#include <ttyd/npcdrv.h>
#include <ttyd/dispdrv.h>
#include <ttyd/battle_ac.h>
#include <ttyd/battle_unit.h>
#include <ttyd/mapdata.h>
#include <ttyd/seq_mapchange.h>
#include <ttyd/seqdrv.h>
#include <ttyd/pmario_sound.h>
#include <ttyd/fontmgr.h>
#include <ttyd/seq_title.h>
#include <ttyd/seq_load.h>
#include <ttyd/windowdrv.h>
#include <ttyd/seq_logo.h>

namespace mod {

Mod *gMod = nullptr;

void main()
{
    // Free the memory used by the relocation data of this project's REL file, as this REL file shouldn't ever be unlinked
    gc::OSModule::OSModuleInfo *rel = *reinterpret_cast<gc::OSModule::OSModuleInfo **>(0x80004150); // Stored by the REL Loader code
    MakeRelFixed(0, rel);
    
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
        ttyd::evt_bero::evt_bero_get_info, [](ttyd::evtmgr::EvtEntry *evt, bool isFirstCall)
    {
        return gMod->setIndexWarpEntrance(evt, isFirstCall);
    });
    
    mPFN_marioKeyOn_trampoline = patch::hookFunction(
        ttyd::mario::marioKeyOn, []()
    {
        return gMod->fixMarioKeyOn();
    });
    
    mPFN_OSLink_trampoline = patch::hookFunction(
        gc::OSModule::OSLink, [](gc::OSModule::OSModuleInfo *newModule, void *bss)
    {
        return gMod->performRelPatches(newModule, bss);
    });
    
    mPFN_fbatHitCheck_trampoline = patch::hookFunction(
        ttyd::npcdrv::fbatHitCheck, [](uint32_t flags, void *unk)
    {
        return gMod->fbatHitCheck_Work(flags, unk);
    });
    
    mPFN_scissor_disp_control_trampoline = patch::hookFunction(
        ttyd::sac_scissor::scissor_disp_control, [](ttyd::dispdrv::CameraId cameraId)
    {
        return gMod->drawArtAttackHitboxes(cameraId);
    });
    
    mPFN_BattleActionCommandCheckDefence_trampoline = patch::hookFunction(
        ttyd::battle_ac::BattleActionCommandCheckDefence, [](
            void *battleUnitPtr, ttyd::battle_unit::AttackParams *attackParams)
    {
        return gMod->displayActionCommandsTimingHook(battleUnitPtr, attackParams);
    });
    
    /* mPFN_systemErrorHandler_trampoline = patch::hookFunction(
        ttyd::mariost::N_systemErrorHandler, [](uint16_t error, 
            gc::OSContext::OSContext *context, uint32_t dsisr, uint32_t dar)
    {
        // Enable the FPU registers
        asmErrorHandlerEnableFPU();
        gMod->errorHandler(error, context, dsisr, dar);
    }); */
    
    mPFN_npcNameToPtr_trampoline = patch::hookFunction(
        ttyd::npcdrv::npcNameToPtr, [](const char *name)
    {
        return gMod->checkForNpcNameToPtrError(name);
    });
    
    mPFN_mapDataPtr_trampoline = patch::hookFunction(
        ttyd::mapdata::mapDataPtr, [](const char *mapName)
    {
        return gMod->mapDataPtrHandleUnusedMaps(mapName);
    });
    
    mPFN__unload_trampoline = patch::hookFunction(
        ttyd::seq_mapchange::_unload, [](
            const char *currentMap, const char *nextMap, const char *nextBero)
    {
        return gMod->_unloadHook(currentMap, nextMap, nextBero);
    });
    
    mPFN_relSetEvtAddr_trampoline = patch::hookFunction(
        ttyd::mapdata::relSetEvtAddr, [](const char *mapName, const void *pInitEvtCode)
    {
        gMod->relSetEvtAddrHook(mapName, pInitEvtCode);
    });
    
    mPFN_seqSetSeq_trampoline = patch::hookFunction(
        ttyd::seqdrv::seqSetSeq, [](ttyd::seqdrv::SeqIndex seq, const char *map, const char *bero)
    {
        gMod->preventBattlesOnReload(seq, map, bero);
    });
    
    mPFN_DEMOPadRead_trampoline = patch::hookFunction(
        gc::DEMOPad::DEMOPadRead, []()
    {
        gMod->frameAdvance();
    });
    
    mPFN_viPostCallback_trampoline = patch::hookFunction(
        ttyd::mariost::viPostCallback, [](uint32_t retraceCount)
    {
        gMod->preventViPostCallBackOnPause(retraceCount);
    });
    
    mPFN_psndBGMOn_f_d_trampoline = patch::hookFunction(
        ttyd::pmario_sound::psndBGMOn_f_d, [](int32_t flags, const char *bgmName, 
            uint16_t wFadeTime, uint16_t wFadeTime2, bool unused)
    {
        return gMod->disableBGM(flags, bgmName, wFadeTime, wFadeTime2, unused);
    });
    
    mPFN_psndENVOn_f_d_trampoline = patch::hookFunction(
        ttyd::pmario_sound::psndENVOn_f_d, [](int32_t flags, 
            const char *envName, int32_t wFadeTime, bool unused)
    {
        return gMod->disableENVSounds(flags, envName, wFadeTime, unused);
    });
    
    mPFN_titleMain_trampoline = patch::hookFunction(
        ttyd::seq_title::titleMain, [](ttyd::seqdrv::SeqInfo *seqInfo)
    {
        gMod->displayTitleScreenInfo(seqInfo);
    });
    
    mPFN_seq_loadMain_trampoline = patch::hookFunction(
        ttyd::seq_load::seq_loadMain, [](ttyd::seqdrv::SeqInfo *seqInfo)
    {
        gMod->displayFileSelectScreenInfo(seqInfo);
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