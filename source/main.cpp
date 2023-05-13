#include "mod.h"
#include "gamePatches.h"
#include "cxx.h"
#include "drawText.h"
#include "patch.h"
#include "classes/menu.h"
#include "classes/window.h"
#include "gc/OSModule.h"
#include "menus/root.h"
#include "misc/functionHooks.h"
#include "ttyd/mariost.h"
#include "ttyd/seq_title.h"
#include "ttyd/seq_load.h"
#include "ttyd/msgdrv.h"
#include "ttyd/win_root.h"
#include "ttyd/mario.h"
#include "ttyd/seq_battle.h"
#include "ttyd/fontmgr.h"
#include "ttyd/windowdrv.h"

#include <cstdint>

Mod *gMod = nullptr;

void init()
{
    // Apply fixes to various bugs in the game
    applyGameFixes();

    // Apply various game patches
    applyVariousGamePatches();

    Mod *modPtr = new (true) Mod;
    gMod = modPtr;

    gDisplayBuffer = new (true) char[DISPLAY_BUFFER_SIZE];

    // The root window is used for various things outside of the menu, so it can just exist at all times
    Window *windowPtr = new (true) Window;
    windowPtr->init(0x000000F4, -245.f, 190.f, 490.f, 375.f, 0.f, 20.f);
    gRootWindow = windowPtr;

    // Handle function hooks
    // For code that runs once per frame
    marioStMain_trampoline = hookFunction(marioStMain, runOncePerFrame);

    // For stuff that is displayed on the title screen and file select screen
    titleMain_trampoline = hookFunction(titleMain, displayTitleScreenInfo);
    seq_loadMain_trampoline = hookFunction(seq_loadMain, displayFileSelectScreenInfo);

    // For custom textbox messages
    msgSearch_trampoline = hookFunction(msgSearch, getCustomMessage);

    // For preventing leaving the pause menu if L + Start is pressed
    winRootMain_trampoline = hookFunction(winRootMain, pauseMenuPreventUnpause);

    // For preventing marioKeyOn from setting the key value to a negative value
    marioKeyOn_trampoline = hookFunction(marioKeyOn, fixMarioKeyOn);

    // For fixing various issues within rel files
    OSLink_trampoline = hookFunction(OSLink, applyRelPatches);

    // For performing various actions immediately before a battle starts
    battle_init_trampoline = hookFunction(battle_init, performPreBattleActions);

    // Initialize text stuff early
    fontmgrTexSetup();
    applyAssemblyPatch(fontmgrTexSetup, 0x4E800020); // blr

    // Initialize window stuff early
    windowTexSetup();
    applyAssemblyPatch(windowTexSetup, 0x4E800020); // blr
}

void exit() {}

void runOncePerFrame()
{
    // Handle the menu
    handleMenu();

    // Call the original function
    return marioStMain_trampoline();
}
