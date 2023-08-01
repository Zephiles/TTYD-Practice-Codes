#include "mod.h"
#include "gamePatches.h"
#include "cxx.h"
#include "cheats.h"
#include "displays.h"
#include "patch.h"
#include "classes/menu.h"
#include "classes/window.h"
#include "gc/OSModule.h"
#include "gc/DEMOPad.h"
#include "gc/OSAlloc.h"
#include "menus/rootMenu.h"
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
#include "ttyd/statuswindow.h"
#include "ttyd/battle_ac.h"
#include "ttyd/sac_scissor.h"
#include "ttyd/win_mario.h"
#include "ttyd/hitdrv.h"
#include "ttyd/animdrv.h"
#include "ttyd/sound.h"
#include "ttyd/pmario_sound.h"
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

    gCheats = new (true) Cheats;
    gDisplays = new (true) Displays;

    // The root window is used for various things outside of the menu, so it can just exist at all times
    Window *windowPtr = new (true) Window;
    windowPtr->init(0x000000F4, -245.f, 190.f, 490.f, 375.f, 0.f, 20.f);
    gRootWindow = windowPtr;

    // Handle function hooks
    // For code that runs once per frame
    g_marioStMain_trampoline = hookFunction(marioStMain, runOncePerFrame);

    // For stuff that is displayed on the title screen and file select screen
    g_titleMain_trampoline = hookFunction(titleMain, displayTitleScreenInfo);
    g_seq_loadMain_trampoline = hookFunction(seq_loadMain, displayFileSelectScreenInfo);

    // For custom textbox messages
    g_msgSearch_trampoline = hookFunction(msgSearch, getCustomMessage);

    // For preventing leaving the pause menu if L + Start is pressed
    g_winRootMain_trampoline = hookFunction(winRootMain, pauseMenuPreventUnpause);

    // For preventing marioKeyOn from setting the key value to a negative value
    g_marioKeyOn_trampoline = hookFunction(marioKeyOn, fixMarioKeyOn);

    // For fixing various issues within rel files
    g_OSLink_trampoline = hookFunction(OSLink, applyRelPatches);

    // For performing various actions immediately before a battle starts
    g_battle_init_trampoline = hookFunction(battle_init, performPreBattleActions);

    // For handling various checks during a battle
    g_BattlePadManager_trampoline = hookFunction(BattlePadManager, performBattleChecks);

    // For handling the Infinite Item Usage cheat
    g_pouchRemoveItemIndex_trampoline = hookFunction(pouchRemoveItemIndex, infiniteItemUsage);

    // For handling the Frame Advance cheat
    g_DEMOPadRead_trampoline = hookFunction(DEMOPadRead, frameAdvance);

    // For preventing most of the code in viPostCallback from running when advancing frames via the Frame Advance cheat, as
    // allowing the code to run wull break various things
    g_viPostCallback_trampoline = hookFunction(viPostCallback, preventViPostCallBackOnFramePause);

    // For handling certain checks when making contact with enemies
    g_fbatHitCheck_trampoline = hookFunction(fbatHitCheck, fbatHitCheck_Work);

    // For preventing being able to reload the current room and starting a battle at the same time, as this will cause the game
    // to softlock
    g_seqSetSeq_trampoline = hookFunction(seqSetSeq, disableBattlesOnReload);

    // For handling the Star Power Value display
    g_statusWinDisp_trampoline = hookFunction(statusWinDisp, drawStarPowerValue);

    // For handling the Guard/Superguard Timings display
    g_BattleActionCommandCheckDefence_trampoline = hookFunction(BattleActionCommandCheckDefence, initGuardSuperguardTimings);

    // For handling the Art Attack Hitboxes display
    g_scissor_disp_control_trampoline = hookFunction(scissor_disp_control, drawArtAttackHitboxes);

    // For handling pausing the Art Attack timer when using the Art Attack Hitboxes display
    g_scissor_timer_main_trampoline = hookFunction(scissor_timer_main, pauseArtAttackTimer);

    // For drawing the current sequence position in the pause menu
    g_winMarioDisp_trampoline = hookFunction(winMarioDisp, drawSequenceInPauseMenu);

    // For handling the Hit Check Visualization display
    g_hitCheckVecFilter_trampoline = hookFunction(hitCheckVecFilter, checkForVecHits);

    // For handling checking for errors in npcNameToPtr
    g_npcNameToPtr_trampoline = hookFunction(npcNameToPtr, checkForNpcNameToPtrError);

    // For handling checking for errors in animPoseMain
    g_animPoseMain_trampoline = hookFunction(animPoseMain, preventAnimPoseMainCrash);

    // For handling the Disable Certain Sounds cheat, specifically for disabling the pause menu/Z menu sounds
    g_SoundEfxPlayEx_trampoline = hookFunction(SoundEfxPlayEx, disableMenuSounds);

    // For handling the Disable Certain Sounds cheat, specifically for disabling background music
    g_psndBGMOn_f_d_trampoline = hookFunction(psndBGMOn_f_d, disableBgm);

    // For handling the Disable Certain Sounds cheat, specifically for disabling environment sounds
    g_psndENVOn_f_d_trampoline = hookFunction(psndENVOn_f_d, disableEnvSounds);

    // Initialize text stuff early
    fontmgrTexSetup();
    applyAssemblyPatch(fontmgrTexSetup, 0x4E800020); // blr

    // Initialize window stuff early
    windowTexSetup();
    applyAssemblyPatch(windowTexSetup, 0x4E800020); // blr
}

void exit() {}

void checkHeaps()
{
    // Reset heapCorruptioBufferIndex and clear the memory usage buffer before doing anything
    MemoryUsageDisplay *memoryUsageDisplayPtr = gDisplays->getMemoryUsageDisplayPtr();
    memoryUsageDisplayPtr->setHeapCorruptionBufferIndex(0);
    memoryUsageDisplayPtr->clearMemoryUsageBuffer();

    // Check the standard heaps
    const HeapInfo *heapArrayPtr = HeapArray;
    uint32_t memoryUsageCounter = 0;
    const void *addressWithError;

    for (int32_t i = 0; i < DISPLAYS_TOTAL_MAIN_HEAPS; i++)
    {
        const HeapInfo *heapPtr = &heapArrayPtr[i];

        // Check the used entries
        const ChunkInfo *tempChunk = heapPtr->firstUsed;
        addressWithError = checkIndividualStandardHeap(tempChunk);

        memoryUsageDisplayPtr
            ->handleStandardHeapChunkResults(addressWithError, tempChunk, heapPtr, i, memoryUsageCounter++, true);

        // Check the free entries
        tempChunk = heapPtr->firstFree;
        addressWithError = checkIndividualStandardHeap(tempChunk);

        memoryUsageDisplayPtr
            ->handleStandardHeapChunkResults(addressWithError, tempChunk, heapPtr, i, memoryUsageCounter++, false);
    }

    // Check the smart heap
    const SmartWork *tempSmartWorkPtr = smartWorkPtr;

    // Check the used entries
    const SmartAllocationData *tempChunk = tempSmartWorkPtr->pFirstUsed;
    addressWithError = checkIndividualSmartHeap(tempChunk);
    memoryUsageDisplayPtr->handleSmartHeapChunkResults(addressWithError, tempChunk, memoryUsageCounter++, true);

    // Check the free entries
    // Don't incrememt MemoryUsageCounter since free entries are not drawn
    tempChunk = tempSmartWorkPtr->pFirstFree;
    addressWithError = checkIndividualSmartHeap(tempChunk);
    memoryUsageDisplayPtr->handleSmartHeapChunkResults(addressWithError, tempChunk, memoryUsageCounter, false);

    // Check the map heap
    const MapAllocEntry *mapHeapPtr = mapalloc_base_ptr;
    addressWithError = checkIndividualMapHeap(mapHeapPtr);

#ifdef TTYD_JP
    memoryUsageDisplayPtr->handleMapHeapChunkResults(addressWithError, mapHeapPtr, memoryUsageCounter);
#else
    memoryUsageDisplayPtr->handleMapHeapChunkResults(addressWithError, mapHeapPtr, memoryUsageCounter, false);

    // Check the battle map heap
    mapHeapPtr = R_battlemapalloc_base_ptr;
    addressWithError = checkIndividualMapHeap(mapHeapPtr);
    memoryUsageDisplayPtr->handleMapHeapChunkResults(addressWithError, mapHeapPtr, ++memoryUsageCounter, true);
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

    // Call the original function
    return g_marioStMain_trampoline();
}
