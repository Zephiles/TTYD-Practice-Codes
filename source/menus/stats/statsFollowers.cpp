#include "menuUtils.h"
#include "drawText.h"
#include "classes/window.h"
#include "classes/followerSelector.h"
#include "menus/statsMenu.h"
#include "menus/rootMenu.h"
#include "ttyd/party.h"
#include "ttyd/camdrv.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

const MenuOption gStatsMenuFollowersOptions[] = {
    "Bring Follower Out",
    selectedOptionMenuFollowersBringFollowerOut,

    "Remove Current Follower",
    selectedOptionMenuFollowersRemoveFollower,
};

const MenuFunctions gStatsMenuFollowersFuncs = {
    gStatsMenuFollowersOptions,
    statsMenuFollowersControls,
    statsMenuFollowersDraw,
    nullptr, // Exit function not needed
};

void errorMenuFollowersBringOutFollower()
{
    // cancelMenuFollowersBringOutFollower gets called by the follower selector automatically, so no need to call it here

    // Initialize the error window
    gStatsMenu->initErrorWindow(false);
}

void cancelMenuFollowersBringOutFollower()
{
    gStatsMenu->getFollowerSelector()->stopDrawing();
    gMenu->clearFlag(StatsFlagFollower::STATS_FLAG_FOLLOWER_CURRENTLY_SELECTING_FOLLOWER);
}

void selectedOptionMenuFollowersBringFollowerOut(Menu *menuPtr)
{
    // If a follower cannot be spawned right now, then show an error message
    StatsMenu *statsMenuPtr = gStatsMenu;
    if (!checkIfInGame())
    {
        statsMenuPtr->initErrorWindow(false);
        return;
    }

    // Bring up the window for selecting a follower
    menuPtr->setFlag(StatsFlagFollower::STATS_FLAG_FOLLOWER_CURRENTLY_SELECTING_FOLLOWER);

    // Initialize the follower selector
    const Window *rootWindowPtr = gRootWindow;
    FollowerSelector *followerSelectorPtr = statsMenuPtr->getFollowerSelector();

    followerSelectorPtr->init(rootWindowPtr, statsMenuPtr->getScale(), rootWindowPtr->getAlpha());
    followerSelectorPtr->startDrawing(cancelMenuFollowersBringOutFollower, errorMenuFollowersBringOutFollower);
}

void selectedOptionMenuFollowersRemoveFollower(Menu *menuPtr)
{
    (void)menuPtr;

    removeFollowerFromOverworld();
}

void statsMenuFollowersInit(Menu *menuPtr)
{
    (void)menuPtr;

    // Reset currentIndex
    gStatsMenu->setCurrentIndex(0);

    constexpr uint32_t totalOptions = sizeof(gStatsMenuFollowersOptions) / sizeof(MenuOption);
    enterNextMenu(&gStatsMenuFollowersFuncs, totalOptions);
}

void statsMenuFollowersControls(Menu *menuPtr, MenuButtonInput button)
{
    // If the follower selector is open, then handle the controls for that
    if (menuPtr->flagIsSet(StatsFlagFollower::STATS_FLAG_FOLLOWER_CURRENTLY_SELECTING_FOLLOWER))
    {
        gStatsMenu->getFollowerSelector()->controls(button);
        return;
    }
    else
    {
        // Use the default controls
        basicMenuLayoutControls(menuPtr, button);
        return;
    }
}

void StatsMenu::drawFollowerOut() const
{
    // Get the text for the current follower out
    const PartyMembers follower = getCurrentPartnerOrFollowerOut(false);
    const char *currentFollower = "Invalid";

    if (follower == PartyMembers::kNone)
    {
        currentFollower = "None";
    }
    else
    {
        // Make sure the current follower out is valid
        if ((follower >= PartyMembers::kEgg) && (follower <= PartyMembers::kMsMowzFollower))
        {
            // Get the text for the current follower
            const PartyMembers *followersOptionsIdPtr = gFollowersOptionsId;
            for (uint32_t i = 0; i < TOTAL_FOLLOWERS; i++)
            {
                if (followersOptionsIdPtr[i] == follower)
                {
                    currentFollower = gFollowersOptions[i];
                    break;
                }
            }
        }
    }

    char buf[64];
    snprintf(buf, sizeof(buf), "Current Follower Out\n%s", currentFollower);

    // Draw the text for the current follower out two lines under the main text
    const uint32_t totalOptions = gMenu->getTotalOptions();
    const float scale = this->scale;

    float tempPosX;
    float tempPosY;
    gRootWindow->getTextPosXYUnderMainText(nullptr, WindowAlignment::TOP_LEFT, totalOptions, 2, scale, &tempPosX, &tempPosY);
    drawText(buf, tempPosX, tempPosY, scale, getColorWhite(0xFF));
}

void statsMenuFollowersDraw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDraw(cameraId, user);

    // Draw the current follower out
    StatsMenu *statsMenuPtr = gStatsMenu;
    statsMenuPtr->drawFollowerOut();

    // Draw the follower selector if applicable
    FollowerSelector *followerSelector = statsMenuPtr->getFollowerSelector();
    if (followerSelector->shouldDraw())
    {
        followerSelector->draw();
    }

    // Draw the error message if applicable
    ErrorWindow *errorWindowPtr = statsMenuPtr->getErrorWindow();
    if (errorWindowPtr->shouldDraw())
    {
        errorWindowPtr->draw();
    }
}
