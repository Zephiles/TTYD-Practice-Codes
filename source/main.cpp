#include "mod.h"
#include "gamePatches.h"
#include "cxx.h"
#include "drawText.h"
#include "cheats.h"
#include "displays.h"
#include "memoryWatch.h"
#include "memoryEditor.h"
#include "settings.h"
#include "patch.h"
#include "menuUtils.h"
#include "jpBattleHeap.h"
#include "classes/window.h"
#include "gc/types.h"
#include "gc/card.h"
#include "gc/OSModule.h"
#include "gc/DEMOPad.h"
#include "gc/OSAlloc.h"
#include "menus/rootMenu.h"
#include "menus/warpsMenu.h"
#include "misc/functionHooks.h"
#include "misc/utils.h"
#include "ttyd/mariost.h"
#include "ttyd/seq_title.h"
#include "ttyd/seq_load.h"
#include "ttyd/msgdrv.h"
#include "ttyd/win_root.h"
#include "ttyd/mario.h"
#include "ttyd/seq_battle.h"
#include "ttyd/battle_pad.h"
#include "ttyd/mario_pouch.h"
#include "ttyd/npcdrv.h"
#include "ttyd/pmario_sound.h"
#include "ttyd/statuswindow.h"
#include "ttyd/battle_ac.h"
#include "ttyd/sac_scissor.h"
#include "ttyd/win_mario.h"
#include "ttyd/hitdrv.h"
#include "ttyd/animdrv.h"
#include "ttyd/mapdata.h"
#include "ttyd/evt_bero.h"
#include "ttyd/seq_mapchange.h"
#include "ttyd/sound.h"
#include "ttyd/fontmgr.h"
#include "ttyd/windowdrv.h"
#include "ttyd/memory.h"

#include <cstdint>

void init()
{
    // Apply fixes to various bugs in the game
    applyGameFixes();

    // Apply various game patches
    applyVariousGamePatches();

    // Apply assembly injects for cheats and displays
    applyCheatAndDisplayInjects();

    // The root window is used for various things outside of the menu, so it can just exist at all times
    Window *windowPtr = new (true, true) Window;
    windowPtr->init(getColorBlack(0xF4), -245.f, 190.f, 490.f, 375.f, 0.f, 20.f);
    gRootWindow = windowPtr;

    gMod = new (true, true) Mod;
    gCheats = new (true, true) Cheats;
    gDisplays = new (true, true, sizeof(OSTime)) Displays;
    gMemoryEditor = new (true, true) MemoryEditor;

#ifdef TTYD_JP
    // Various things need to be done for setting up/handling the battle heap
    battleHeapVarsInit();
#endif

    // Handle function hooks
    // For code that runs once per frame
    g_marioStMain_trampoline = hookFunctionArena(marioStMain, runOncePerFrame);

    // For stuff that is displayed on the title screen and file select screen
    g_titleMain_trampoline = hookFunctionArena(titleMain, displayTitleScreenInfo);
    g_seq_loadMain_trampoline = hookFunctionArena(seq_loadMain, displayFileSelectScreenInfo);

    // For custom textbox messages
    g_msgSearch_trampoline = hookFunctionArena(msgSearch, getCustomMessage);

    // For preventing leaving the pause menu if L + Start is pressed
    g_winRootMain_trampoline = hookFunctionArena(winRootMain, pauseMenuPreventUnpause);

    // For preventing marioKeyOn from setting the key value to a negative value
    g_marioKeyOn_trampoline = hookFunctionArena(marioKeyOn, fixMarioKeyOn);

    // For fixing various issues within rel files
    g_OSLink_trampoline = hookFunctionArena(OSLink, applyRelPatches);

    // For performing various actions immediately before a battle starts
    g_battle_init_trampoline = hookFunctionArena(battle_init, performPreBattleActions);

    // For handling various checks during a battle
    g_BattlePadManager_trampoline = hookFunctionArena(BattlePadManager, performBattleChecks);

    // For handling the Infinite Item Usage cheat
    g_pouchRemoveItemIndex_trampoline = hookFunctionArena(pouchRemoveItemIndex, infiniteItemUsage);

    // For handling the Frame Advance cheat
    g_DEMOPadRead_trampoline = hookFunctionArena(DEMOPadRead, frameAdvance);

    // For preventing most of the code in viPostCallback from running when advancing frames via the Frame Advance cheat, as
    // allowing the code to run wull break various things
    g_viPostCallback_trampoline = hookFunctionArena(viPostCallback, preventViPostCallBackOnFramePause);

    // For handling certain checks when making contact with enemies
    g_fbatHitCheck_trampoline = hookFunctionArena(fbatHitCheck, fbatHitCheck_Work);

    // For allowing the Arbitrary Memory Write glitch to work with current builds of the Practice Codes, as well as for
    // disabling the random chance for it to fail
    g_psndSFXOff_trampoline = hookFunctionArena(psndSFXOff, psndSFXOff_Work);

    // For preventing being able to reload the current room and starting a battle at the same time, as this will cause the game
    // to softlock
    g_seqSetSeq_trampoline = hookFunctionArena(seqSetSeq, disableBattlesOnReload);

    // For handling the Star Power Value display
    g_statusWinDisp_trampoline = hookFunctionArena(statusWinDisp, drawStarPowerValue);

    // For handling the Guard/Superguard Timings display
    g_BattleActionCommandCheckDefence_trampoline =
        hookFunctionArena(BattleActionCommandCheckDefence, initGuardSuperguardTimings);

    // For handling the Art Attack Hitboxes display
    g_scissor_disp_control_trampoline = hookFunctionArena(scissor_disp_control, drawArtAttackHitboxes);

    // For handling pausing the Art Attack timer when using the Art Attack Hitboxes display
    g_scissor_timer_main_trampoline = hookFunctionArena(scissor_timer_main, pauseArtAttackTimer);

    // For drawing the current sequence position in the pause menu
    g_winMarioDisp_trampoline = hookFunctionArena(winMarioDisp, drawSequenceInPauseMenu);

    // For handling the Hit Check Visualization display
    g_hitCheckVecFilter_trampoline = hookFunctionArena(hitCheckVecFilter, checkForVecHits);

    // For handling checking for errors in npcNameToPtr
    g_npcNameToPtr_trampoline = hookFunctionArena(npcNameToPtr, checkForNpcNameToPtrError);

    // For handling checking for errors in animPoseMain
    g_animPoseMain_trampoline = hookFunctionArena(animPoseMain, preventAnimPoseMainCrash);

    // For allowing mapDataPtr to propaly handle unused maps
    g_mapDataPtr_trampoline = hookFunctionArena(mapDataPtr, mapDataPtrHandleUnusedMaps);

    // For handling setting the loading zone when warping by index
    g_evt_bero_get_info_trampoline = hookFunctionArena(evt_bero_get_info, setIndexWarpEntrances);

    // For clearing WarpByIndex::currentMapInitScript and WarpByIndex::totalEntrances upon unloading a map, as some maps don't
    // have init scripts
    g__unload_trampoline = hookFunctionArena(_unload, unloadClearCurrentMapInitScript);

    // For setting WarpByIndex::currentMapInitScript upon a map being loaded
    g_relSetEvtAddr_trampoline = hookFunctionArena(relSetEvtAddr, relSetEvtAddrSetCurrentMapInitScript);

    // For handling the Disable Certain Sounds cheat, specifically for disabling the pause menu/Z menu sounds
    g_SoundEfxPlayEx_trampoline = hookFunctionArena(SoundEfxPlayEx, disableMenuSounds);

    // For handling the Disable Certain Sounds cheat, specifically for disabling background music
    g_psndBGMOn_f_d_trampoline = hookFunctionArena(psndBGMOn_f_d, disableBgm);

    // For handling the Disable Certain Sounds cheat, specifically for disabling environment sounds
    g_psndENVOn_f_d_trampoline = hookFunctionArena(psndENVOn_f_d, disableEnvSounds);
}

void exit() {}

uint32_t initAfterHeapsCreated()
{
#ifdef TTYD_JP
    // The code for hooking OSCreateHeap only needs to run at boot, so it can be unhooked
    // unhookFunctionArena(g_OSCreateHeap_trampoline);
#endif

    // Load the custom settings from the settings file if it exists
    // Try both memory card slots
    if (loadSettings(CARD_SLOT_A) != CARD_RESULT_READY)
    {
        loadSettings(CARD_SLOT_B);
    }

    // Initialize text stuff early
    fontmgrTexSetup();
    applyAssemblyPatch(fontmgrTexSetup, 0x4E800020); // blr; prevent fontmgrTexSetup from running again

    // Initialize window stuff early
    windowTexSetup();
    applyAssemblyPatch(windowTexSetup, 0x4E800020); // blr; prevent windowTexSetup from running again

    // The original instruction set r3 to 0, so returning 0 will give the same result
    return 0;
}

// Avoid inlining `checkHeaps`, as it makes debugging `runOncePerFrame` harder
__attribute__((noinline)) static void checkHeaps()
{
    // Check the standard heaps
    const uint8_t *memoryUsageFlagsArrayPtr = memoryUsageFlagsArray;
    uint32_t enabledFlag = memoryUsageFlagsArrayPtr[0];
    const HeapInfo *heapArrayPtr = HeapArray;
    Displays *displaysPtr = gDisplays;
    uint32_t memoryUsageCounter = 0;
    const void *addressWithError;
    uint32_t i = 0;

    for (; i < DISPLAYS_TOTAL_MAIN_HEAPS; i++, enabledFlag = memoryUsageFlagsArrayPtr[i])
    {
        const HeapInfo *heapPtr = &heapArrayPtr[i];

        // Check the used entries
        const ChunkInfo *tempChunk = heapPtr->firstUsed;
        addressWithError = checkIndividualStandardHeap(tempChunk);

        displaysPtr
            ->handleStandardHeapChunkResults(addressWithError, tempChunk, heapPtr, i, memoryUsageCounter++, enabledFlag, true);

        // Check the free entries
        tempChunk = heapPtr->firstFree;
        addressWithError = checkIndividualStandardHeap(tempChunk);

        displaysPtr
            ->handleStandardHeapChunkResults(addressWithError, tempChunk, heapPtr, i, memoryUsageCounter++, enabledFlag, false);
    }

    // Check the smart heap
    const SmartWork *smartWorkPtr = _smartWorkPtr;

    // Check the used entries
    const SmartAllocationData *tempChunk = smartWorkPtr->pFirstUsed;
    addressWithError = checkIndividualSmartHeap(tempChunk);
    displaysPtr->handleSmartHeapChunkResults(addressWithError, tempChunk, memoryUsageCounter++, enabledFlag, true);

    // Check the free entries
    // Don't incrememt MemoryUsageCounter since free entries are not drawn
    tempChunk = smartWorkPtr->pFirstFree;
    addressWithError = checkIndividualSmartHeap(tempChunk);
    displaysPtr->handleSmartHeapChunkResults(addressWithError, tempChunk, memoryUsageCounter, enabledFlag, false);

    // Check the map heap
    const MapAllocEntry *mapHeapPtr = mapalloc_base_ptr;
    addressWithError = checkIndividualMapHeap(mapHeapPtr);

#ifdef TTYD_JP
    enabledFlag = memoryUsageFlagsArrayPtr[++i];
    displaysPtr->handleMapHeapChunkResults(addressWithError, mapHeapPtr, memoryUsageCounter, enabledFlag);
#else
    enabledFlag = memoryUsageFlagsArrayPtr[++i];
    displaysPtr->handleMapHeapChunkResults(addressWithError, mapHeapPtr, memoryUsageCounter, enabledFlag, false);

    // Check the battle map heap
    mapHeapPtr = R_battlemapalloc_base_ptr;
    enabledFlag = memoryUsageFlagsArrayPtr[i];
    addressWithError = checkIndividualMapHeap(mapHeapPtr);
    displaysPtr->handleMapHeapChunkResults(addressWithError, mapHeapPtr, ++memoryUsageCounter, enabledFlag, true);
#endif
}

void runOncePerFrame()
{
    // Check the heaps for errors and set up their memory usages
    checkHeaps();

    // Handle the menu
    handleMenu();

    // Run cheat functions
    runCheatFuncsEveryFrame();

    // Run display functions
    runDisplayFuncsEveryFrame();

    // Handle the memory editor
    handleMemoryEditor();

    // Handle memory watches
    handleMemoryWatches();

    // Call the original function
    g_marioStMain_trampoline();

    // The Hit Check Visualization code needs to run after the original function has ran
    handleHitCheckVisualization();
}
