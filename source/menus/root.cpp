#include "menuUtils.h"
#include "drawText.h"
#include "mod.h"
#include "classes/window.h"
#include "menus/root.h"
#include "menus/inventory.h"
#include "menus/stats.h"
#include "menus/battles.h"
#include "misc/utils.h"
#include "ttyd/dispdrv.h"
#include "ttyd/win_main.h"

#include <cstdint>

Window *gRootWindow = nullptr;
Root *gRoot = nullptr;

const MenuOption gRootMenuOptions[] = {
    "Inventory",
    inventoryMenuInit,

    "Cheats",
    nullptr,

    "Stats",
    statsMenuInit,

    "Settings",
    nullptr,

    "Memory",
    nullptr,

    "Battles",
    battlesMenuSelectActorInit,

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

    // Need to enter the root menu before initializing gRoot
    constexpr uint32_t totalOptions = sizeof(gRootMenuOptions) / sizeof(MenuOption);
    enterNextMenu(gRootMenuOptions, &gRootMenuFuncs, totalOptions);

    // Failsafe: Make sure memory isn't already allocated for gRoot
    Root *rootPtr = gRoot;
    if (rootPtr)
    {
        delete rootPtr;
    }

    const char *battlesErrorMessage = "You must be in a battle\nto use the Battles menu.";
    gRoot = new Root(battlesErrorMessage, 0.6);
}

void rootMenuExit()
{
    delete gRoot;
    gRoot = nullptr;

    // Reset the system level if not in a battle
    setSystemLevel(0);

    // Enable the pause menu
    winOpenEnable();
}

void rootMenuDraw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    drawBasicMenuLayout(cameraId, user);

    Window *rootWindowPtr = gRootWindow;
    Root *rootPtr = gRoot;

    const float scale = rootPtr->getScale();

    // Draw the version number at the top-right of the main window
    float posX;
    float posY;

    const char *text = VERSION_STRING;
    rootWindowPtr->getTextPosXY(text, WindowAlignment::TOP_RIGHT, scale, &posX, &posY);
    drawText(text, posX, posY, scale, getColorWhite(0xFF));

    // Draw the error message for trying to enter the Battles menu while not in a battle
    ErrorWindow *errorWindowPtr = rootPtr->getErrorWindow();
    if (errorWindowPtr->shouldDraw())
    {
        errorWindowPtr->draw();
    }
}

void rootMenuExitBattlesMenu()
{
    // Close all menus until the root menu is reached
    Root *rootPtr = gRoot;
    Menu *menuPtr = gMenu;
    const Menu *rootMenu = rootPtr->getRootMenu();

    while (menuPtr != rootMenu)
    {
        enterPrevMenu();
        menuPtr = gMenu;

        // Failsafe: Make sure gMenu doesn't end up being nullptr
        if (!menuPtr)
        {
            return;
        }
    }

    // Draw the error message for trying to enter the Battles menu while not in a battle
    rootPtr->getErrorWindow()->setTimer(3000);
}
