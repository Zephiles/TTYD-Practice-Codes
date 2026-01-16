#include "menuUtils.h"
#include "cheats.h"
#include "menus/cheatsMenu.h"
#include "ttyd/camdrv.h"

static void draw(CameraId cameraId, void *user);
static void selectedOptionTurnOnOff(Menu *menuPtr);

static const MenuOption gOptions[] = {
    "Turn On/Off",
    selectedOptionTurnOnOff,
};

static const MenuFunctions gFuncs = {
    gOptions,
    basicMenuLayoutControls,
    draw,
    nullptr, // Exit function not needed
};

void cheatsMenuSimulateArbitraryMemoryWriteInit(Menu *menuPtr)
{
    // Backup the selected cheat
    gCheatsMenu->setSelectedCheat(menuPtr->getCurrentIndex());

    constexpr uint32_t totalOptions = sizeof(gOptions) / sizeof(MenuOption);
    enterNextMenu(&gFuncs, totalOptions);
}

static void draw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDraw(cameraId, user);

    // Draw the info for the Simulate Sound Overwrite cheats
    drawGenericCheatInfo();
}

static void selectedOptionTurnOnOff(Menu *menuPtr)
{
    (void)menuPtr;

    const uint32_t cheatEnabledFlag = indexToCheatEnabledFlag(gCheatsMenu->getSelectedCheat());
    cheatsMenuToggleEnabledFlag(cheatEnabledFlag);
    adjustArbitraryMemoryWriteValues(cheatEnabledFlag);
}
