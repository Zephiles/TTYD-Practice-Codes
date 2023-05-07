#include "menuUtils.h"
#include "menus/root.h"
#include "menus/stats.h"

#include <cstdint>

Stats *gStats = nullptr;

const MenuOption statsMenuInitOptions[] = {
    "Mario",
    statsMenuMarioInit,

    "Partners",
    nullptr,

    "Followers",
    nullptr,
};

const MenuFunctions statsMenuInitFuncs = {
    controlsBasicMenuLayout,
    drawBasicMenuLayout,
    statsMenuInitExit,
};

void statsMenuInit(Menu *menuPtr)
{
    (void)menuPtr;

    constexpr uint32_t totalOptions = sizeof(statsMenuInitOptions) / sizeof(statsMenuInitOptions[0]);
    enterNextMenu(statsMenuInitOptions, &statsMenuInitFuncs, totalOptions);

    // Failsafe: Make sure memory isn't already allocated for gStats
    Stats *statsPtr = gStats;
    if (statsPtr)
    {
        delete statsPtr;
    }

    gStats = new Stats(gRoot->getScale());
}

void statsMenuInitExit()
{
    delete gStats;
    gStats = nullptr;
}
