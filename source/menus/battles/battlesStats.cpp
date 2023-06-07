#include "menuUtils.h"
#include "menus/battles.h"
#include "menus/root.h"
#include "ttyd/dispdrv.h"

#include <cstdint>

const MenuOption gBattlesMenuStatsOptions[] = {
    "Change HP",
    nullptr,

    "Change Max HP",
    nullptr,

    "Change FP",
    nullptr,

    "Change Max FP",
    nullptr,

    "Change Held Item",
    nullptr,

    "Clear Held Item",
    nullptr,

    "Change Statuses",
    nullptr,
};

const MenuFunctions gBattlesMenuStatsFuncs = {
    battlesMenuStatsControls,
    battlesMenuStatsDraw,
    nullptr, // Exit function not needed
};

void battlesMenuStatsInit()
{
    constexpr uint32_t totalOptions = sizeof(gBattlesMenuStatsOptions) / sizeof(MenuOption);
    enterNextMenu(gBattlesMenuStatsOptions, &gBattlesMenuStatsFuncs, totalOptions);
}

void battlesMenuStatsControls(Menu *menuPtr, MenuButtonInput button)
{
    // Close the Battles menu if not in a battle
    if (!getBattleWorkPtr())
    {
        rootMenuExitBattleMenu();
        return;
    }

    // Use the default controls
    controlsBasicMenuLayout(menuPtr, button);
}

void battlesMenuStatsDraw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    drawBasicMenuLayout(cameraId, user);
}
