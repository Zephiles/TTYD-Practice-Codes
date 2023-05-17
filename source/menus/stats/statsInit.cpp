#include "menuUtils.h"
#include "drawText.h"
#include "menus/root.h"
#include "menus/stats.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

Stats *gStats = nullptr;

const MenuOption statsMenuInitOptions[] = {
    "Mario",
    statsMenuMarioInit,

    "Partners",
    statsMenuPartnersInit,

    "Followers",
    statsMenuFollowersInit,
};

const MenuFunctions statsMenuInitFuncs = {
    controlsBasicMenuLayout,
    drawBasicMenuLayout,
    statsMenuInitExit,
};

void statsMenuInit(Menu *menuPtr)
{
    (void)menuPtr;

    constexpr uint32_t totalOptions = sizeof(statsMenuInitOptions) / sizeof(MenuOption);
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

void Stats::initErrorWindow(bool drawForPartner)
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
    const Window *rootWindowPtr = gRootWindow;
    ErrorWindow *errorWindowPtr = &this->errorWindow;

    errorWindowPtr->setTimer(3000);
    errorWindowPtr->setScale(this->scale);
    errorWindowPtr->setAlpha(rootWindowPtr->getAlpha());
    errorWindowPtr->setText(errorMessagePtr);
    errorWindowPtr->placeInWindow(rootWindowPtr, WindowAlignment::MIDDLE_CENTER);
}
