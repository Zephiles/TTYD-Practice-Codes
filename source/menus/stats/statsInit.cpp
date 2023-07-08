#include "menuUtils.h"
#include "cxx.h"
#include "drawText.h"
#include "menus/rootMenu.h"
#include "menus/statsMenu.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

StatsMenu *gStatsMenu = nullptr;

const MenuOption gStatsMenuInitOptions[] = {
    "Mario",
    statsMenuMarioInit,

    "Partners",
    statsMenuPartnersInit,

    "Followers",
    statsMenuFollowersInit,
};

const MenuFunctions gStatsMenuInitFuncs = {
    gStatsMenuInitOptions,
    controlsBasicMenuLayout,
    drawBasicMenuLayout,
    statsMenuInitExit,
};

void statsMenuInit(Menu *menuPtr)
{
    (void)menuPtr;

    constexpr uint32_t totalOptions = sizeof(gStatsMenuInitOptions) / sizeof(MenuOption);
    enterNextMenu(&gStatsMenuInitFuncs, totalOptions);

    // Failsafe: Make sure memory isn't already allocated for gStatsMenu
    StatsMenu *statsMenuPtr = gStatsMenu;
    if (statsMenuPtr)
    {
        delete statsMenuPtr;
    }

    gStatsMenu = new StatsMenu(gRootMenu->getScale());
}

void statsMenuInitExit()
{
    delete gStatsMenu;
    gStatsMenu = nullptr;
}

void StatsMenu::initErrorWindow(bool drawForPartner)
{
    // Get the text for either a partner or a follower
    const char *partnerOrFollowerText;
    if (drawForPartner)
    {
        partnerOrFollowerText = "partner";
    }
    else
    {
        partnerOrFollowerText = "follower";
    }

    // Set up the main text
    char *errorMessagePtr = this->errorMessage;

    snprintf(errorMessagePtr,
             sizeof(this->errorMessage),
             "To spawn a %s, you must have a\nfile loaded and not be in a battle nor\na screen transition.",
             partnerOrFollowerText);

    // Initialize the error window
    ErrorWindow *errorWindowPtr = &this->errorWindow;
    const Window *rootWindowPtr = gRootWindow;

    errorWindowPtr->setScale(this->scale);
    errorWindowPtr->setAlpha(rootWindowPtr->getAlpha());
    errorWindowPtr->setText(errorMessagePtr);

    errorWindowPtr->setTimer(3000);
    errorWindowPtr->placeInWindow(rootWindowPtr, WindowAlignment::MIDDLE_CENTER);
}
