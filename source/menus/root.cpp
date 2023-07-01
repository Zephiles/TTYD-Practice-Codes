#include "menuUtils.h"
#include "drawText.h"
#include "mod.h"
#include "classes/window.h"
#include "menus/rootMenu.h"
#include "menus/inventoryMenu.h"
#include "menus/cheatsMenu.h"
#include "menus/statsMenu.h"
#include "menus/battlesMenu.h"
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
    nullptr,

    "Battles",
    battlesMenuInit,

    "Displays",
    nullptr,

    "Warps",
    nullptr,
};

const MenuFunctions gRootMenuFuncs = {
    controlsBasicMenuLayout,
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
    gMod.showMenu();

    // Need to enter the root menu before initializing gRootMenu
    constexpr uint32_t totalOptions = sizeof(gRootMenuOptions) / sizeof(MenuOption);
    enterNextMenu(gRootMenuOptions, &gRootMenuFuncs, totalOptions);

    // Failsafe: Make sure memory isn't already allocated for gRootMenu
    RootMenu *rootMenuPtr = gRootMenu;
    if (rootMenuPtr)
    {
        delete rootMenuPtr;
    }

    const char *battlesErrorMessage = "You must be in a battle\nto use the Battles menu.";
    gRootMenu = new RootMenu(battlesErrorMessage, 0.6);
}

void rootMenuExit()
{
    delete gRootMenu;
    gRootMenu = nullptr;

    // Reset the flag for the menu being shown to be safe
    gMod.showMenu();

    // Enable the pause menu
    winOpenEnable();

    // Reset the system level if not in a battle
    setSystemLevel(0);
}

void rootMenuDraw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    drawBasicMenuLayout(cameraId, user);

    RootMenu *rootMenuPtr = gRootMenu;
    const float scale = rootMenuPtr->getScale();

    // Draw the version number at the top-right of the main window
    float posX;
    float posY;

    const char *text = VERSION_STRING;
    gRootWindow->getTextPosXY(text, WindowAlignment::TOP_RIGHT, scale, &posX, &posY);
    drawText(text, posX, posY, scale, getColorWhite(0xFF));

    // Draw the error message for trying to enter the Battles menu while not in a battle
    ErrorWindow *errorWindowPtr = rootMenuPtr->getErrorWindow();
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
    const Menu *rootMenu = rootMenuPtr->getRootMenu();

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
    rootMenuPtr->getErrorWindow()->setTimer(3000);
}
