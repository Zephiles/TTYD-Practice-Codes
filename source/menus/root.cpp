#include "menuUtils.h"
#include "drawText.h"
#include "mod.h"
#include "cxx.h"
#include "classes/window.h"
#include "menus/rootMenu.h"
#include "menus/inventoryMenu.h"
#include "menus/cheatsMenu.h"
#include "menus/statsMenu.h"
#include "menus/memoryMenu.h"
#include "menus/battlesMenu.h"
#include "menus/displaysMenu.h"
#include "menus/warpsMenu.h"
#include "misc/utils.h"
#include "ttyd/camdrv.h"
#include "ttyd/win_main.h"

#include <cstdint>

Window *gRootWindow = nullptr;
RootMenu *gRootMenu = nullptr;

const MenuOption gRootMenuOptions[] = {
    "Inventory",
    inventoryMenuInit,

    "Cheats",
    cheatsMenuInit,

    "Stats",
    statsMenuInit,

    "Settings",
    nullptr,

    "Memory",
    memoryMenuInit,

    "Battles",
    battlesMenuInit,

    "Displays",
    displaysMenuInit,

    "Warps",
    warpsMenuInit,
};

const MenuFunctions gRootMenuFuncs = {
    gRootMenuOptions,
    basicMenuLayoutControls,
    rootMenuDraw,
    rootMenuExit,
};

void rootMenuInit()
{
    // Set the system level if not in a battle
    setSystemLevel(1);

    // Disable the pause menu
    winOpenDisable();

    // Make sure the menu is actually being displayed
    gMod->clearFlag(ModFlag::MOD_FLAG_MENU_IS_HIDDEN);

    // Need to enter the root menu before initializing gRootMenu
    constexpr uint32_t totalOptions = sizeof(gRootMenuOptions) / sizeof(MenuOption);
    Menu *menuPtr = enterNextMenu(&gRootMenuFuncs, totalOptions);

    // Failsafe: Make sure memory isn't already allocated for gRootMenu
    RootMenu *rootMenuPtr = gRootMenu;
    if (rootMenuPtr)
    {
        delete rootMenuPtr;
    }

    const char *battlesErrorMessage = "You must be in a battle\nto use the Battles menu.";
    gRootMenu = new RootMenu(menuPtr, battlesErrorMessage);
}

void rootMenuExit()
{
    delete gRootMenu;
    gRootMenu = nullptr;

    // Reset the flag for the menu being shown to be safe
    gMod->clearFlag(ModFlag::MOD_FLAG_MENU_IS_HIDDEN);

    // Enable the pause menu
    winOpenEnable();

    // Reset the system level if not in a battle
    setSystemLevel(0);
}

void rootMenuDraw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDrawMenuLineHeight(cameraId, user);

    // Draw the version number at the top-right of the main window
    RootMenu *rootMenuPtr = gRootMenu;
    constexpr float scale = MENU_SCALE;

    float posX;
    float posY;

    const char *text = VERSION_STRING;
    gRootWindow->getTextPosXY(text, WindowAlignment::TOP_RIGHT, scale, &posX, &posY);
    drawText(text, posX, posY, scale, getColorWhite(0xFF));

    // Draw the error message for trying to enter the Battles menu while not in a battle
    ErrorWindow *errorWindowPtr = rootMenuPtr->getErrorWindowPtr();
    if (errorWindowPtr->shouldDraw())
    {
        errorWindowPtr->draw();
    }
}

void rootMenuExitBattlesMenu()
{
    // Close all menus until the root menu is reached
    RootMenu *rootMenuPtr = gRootMenu;
    Menu *menuPtr = gMenu;
    const Menu *rootMenu = rootMenuPtr->getRootMenuPtr();

    while (menuPtr != rootMenu)
    {
        menuPtr = enterPrevMenu();

        // Failsafe: Make sure gMenu doesn't end up being nullptr
        if (!menuPtr)
        {
            return;
        }
    }

    // Draw the error message for trying to enter the Battles menu while not in a battle
    rootMenuPtr->getErrorWindowPtr()->setTimer(3000);
}
