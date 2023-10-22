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

static void controls(Menu *menuPtr, MenuButtonInput button);
static void draw(CameraId cameraId, void *user);
static void selectedOptionBringFollowerOut(Menu *menuPtr);
static void selectedOptionRemoveCurrentFollower(Menu *menuPtr);

static const MenuOption gOptions[] = {
    "Bring Follower Out",
    selectedOptionBringFollowerOut,

    "Remove Current Follower",
    selectedOptionRemoveCurrentFollower,
};

static const MenuFunctions gFuncs = {
    gOptions,
    controls,
    draw,
    nullptr, // Exit function not needed
};

static void cancelBringOutFollower()
{
    gStatsMenu->getFollowerSelectorPtr()->stopDrawing();
}

static void bringOutFollower(PartyMembers followerSelected)
{
    // Make sure a follower can actually be spawned right now
    if (checkIfInGame())
    {
        // Spawn the follower
        spawnPartnerOrFollower(followerSelected);
    }
    else
    {
        // A follower cannot be spawned right now, so initialize the error window
        gStatsMenu->initErrorWindow(false);
    }

    // Close the follower selector
    cancelBringOutFollower();
}

static void selectedOptionBringFollowerOut(Menu *menuPtr)
{
    (void)menuPtr;

    // If a follower cannot be spawned right now, then show an error message
    StatsMenu *statsMenuPtr = gStatsMenu;
    if (!checkIfInGame())
    {
        statsMenuPtr->initErrorWindow(false);
        return;
    }

    // Initialize the follower selector
    const Window *rootWindowPtr = gRootWindow;
    FollowerSelector *followerSelectorPtr = statsMenuPtr->getFollowerSelectorPtr();

    followerSelectorPtr->init(rootWindowPtr, rootWindowPtr->getAlpha());
    followerSelectorPtr->startDrawing(bringOutFollower, cancelBringOutFollower);
}

static void selectedOptionRemoveCurrentFollower(Menu *menuPtr)
{
    (void)menuPtr;

    removeFollowerFromOverworld();
}

void statsMenuFollowersInit(Menu *menuPtr)
{
    (void)menuPtr;

    // Reset currentIndex
    gStatsMenu->setCurrentIndex(0);

    constexpr uint32_t totalOptions = sizeof(gOptions) / sizeof(MenuOption);
    enterNextMenu(&gFuncs, totalOptions);
}

static void controls(Menu *menuPtr, MenuButtonInput button)
{
    // If the follower selector is open, then handle the controls for that
    FollowerSelector *followerSelectorPtr = gStatsMenu->getFollowerSelectorPtr();
    if (followerSelectorPtr->shouldDraw())
    {
        followerSelectorPtr->controls(button);
        return;
    }

    // Use the default controls
    basicMenuLayoutControls(menuPtr, button);
}

static void drawFollowerOut()
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
    constexpr float scale = MENU_SCALE;

    float tempPosX;
    float tempPosY;
    gRootWindow->getTextPosXYUnderMainText(nullptr, WindowAlignment::TOP_LEFT, totalOptions, 2, scale, &tempPosX, &tempPosY);
    drawText(buf, tempPosX, tempPosY, scale, getColorWhite(0xFF));
}

static void draw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDraw(cameraId, user);

    // Draw the current follower out
    drawFollowerOut();

    // Draw the follower selector if applicable
    StatsMenu *statsMenuPtr = gStatsMenu;
    FollowerSelector *followerSelector = statsMenuPtr->getFollowerSelectorPtr();
    if (followerSelector->shouldDraw())
    {
        followerSelector->draw();
    }

    // Draw the error message if applicable
    ErrorWindow *errorWindowPtr = statsMenuPtr->getErrorWindowPtr();
    if (errorWindowPtr->shouldDraw())
    {
        errorWindowPtr->draw();
    }
}
