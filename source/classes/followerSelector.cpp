#include "mod.h"
#include "drawText.h"
#include "menuUtils.h"
#include "classes/followerSelector.h"
#include "classes/window.h"
#include "misc/utils.h"
#include "ttyd/party.h"

#include <cstdint>

const char *followersOptions[TOTAL_FOLLOWERS] = {
    "Goombella",
    "Koops",
    "Flurrie",
    "Yoshi",
    "Vivian",
    "Bobbery",
    "Ms. Mowz",
    "Egg",
    "Flavio",
    "Punio",
    "Frankly",
    "Gus",
};

// Intended to be used alongside followersOptions
const PartyMembers followersOptionsId[TOTAL_FOLLOWERS] = {
    PartyMembers::kGoombellaFollower,
    PartyMembers::kKoopsFollower,
    PartyMembers::kFlurrieFollower,
    PartyMembers::kYoshiFollower,
    PartyMembers::kVivianFollower,
    PartyMembers::kBobberyFollower,
    PartyMembers::kMsMowzFollower,
    PartyMembers::kEgg,
    PartyMembers::kFlavio,
    PartyMembers::kPunio,
    PartyMembers::kFrankly,
    PartyMembers::kGus,
};

void FollowerSelector::init(const Window *parentWindow, float scale)
{
    this->init(parentWindow, scale, 0xFF);
}

void FollowerSelector::init(const Window *parentWindow, float scale, uint8_t alpha)
{
    this->scale = scale;
    this->alpha = alpha;
    this->cancelFunc = nullptr;
    this->errorFunc = nullptr;
    this->autoIncrement.waitFramesToBegin = 0;
    this->autoIncrement.framesBeforeIncrement = 0;
    this->enabled = false;
    this->currentIndex = 0;

    // Set up the window
    // Initialize it based on the help text
    Window *windowPtr = &this->window;
    windowPtr->setWidthHeightFromTextAndInit(helpTextAConfirmBCancel, scale, SPECIAL_WINDOW_COLOR | alpha, 20.f, 30.f);

    // Increase the width of the window to account for the options if necessary
    float longestWidthLeftColumn;
    getTextWidthHeight("Goombella", scale, &longestWidthLeftColumn, nullptr);

    float longestWidthRightColumn;
    getTextWidthHeight("Ms. Mowz", scale, &longestWidthRightColumn, nullptr);

    const float padding = windowPtr->getPadding() * scale;
    const float width = longestWidthLeftColumn + longestWidthRightColumn + (padding * 2.f) + (35.f * scale);
    if (width > windowPtr->getWidth())
    {
        windowPtr->setWidth(width);
    }

    // Increase the height of the window to account for the options
    const float height = (LINE_HEIGHT_FLOAT * scale) * TOTAL_FOLLOWERS_PER_COLUMN_FLOAT;
    windowPtr->setHeight(windowPtr->getHeight() + height + padding - (padding / 2.f));

    // Place the window inside of the parent window
    windowPtr->placeInWindow(parentWindow, WindowAlignment::MIDDLE_CENTER, scale);
}

void dpadControls(MenuButtonInput button, uint8_t *currentIndexPtr)
{
    menuControlsVertical(button,
                         currentIndexPtr,
                         nullptr,
                         TOTAL_FOLLOWERS,
                         TOTAL_FOLLOWERS_PER_COLUMN * TOTAL_FOLLOWERS_PER_ROW,
                         TOTAL_FOLLOWERS_PER_ROW,
                         true);
}

void FollowerSelector::controls(MenuButtonInput button)
{
    // The function for checking for auto-incrementing needs to run every frame to be handled correctly
    const bool autoIncrement = handleMenuAutoIncrement(&this->autoIncrement);

    // Handle held button inputs if auto-incrementing should be done
    if (autoIncrement)
    {
        const MenuButtonInput buttonHeld = getMenuButtonInput(false);
        switch (buttonHeld)
        {
            case MenuButtonInput::DPAD_LEFT:
            case MenuButtonInput::DPAD_RIGHT:
            case MenuButtonInput::DPAD_DOWN:
            case MenuButtonInput::DPAD_UP:
            {
                dpadControls(buttonHeld, &this->currentIndex);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    // Handle the button inputs pressed this frame
    switch (button)
    {
        case MenuButtonInput::DPAD_LEFT:
        case MenuButtonInput::DPAD_RIGHT:
        case MenuButtonInput::DPAD_DOWN:
        case MenuButtonInput::DPAD_UP:
        {
            dpadControls(button, &this->currentIndex);
            break;
        }
        case MenuButtonInput::A:
        {
            // Make sure the current index is valid
            const uint32_t currentIndex = this->currentIndex;
            if (currentIndex >= TOTAL_FOLLOWERS)
            {
                // Failsafe: Reset the current index to 0
                this->currentIndex = 0;
                break;
            }

            // Make sure a follower can actually be spawned right now
            if (checkIfInGame())
            {
                // Spawn the follower
                spawnPartnerOrFollower(followersOptionsId[currentIndex]);
            }
            else
            {
                const FollowerSelectorErrorFunc errorFunc = this->errorFunc;
                if (errorFunc)
                {
                    errorFunc();
                }
            }

            // Close the follower selector
            const FollowerSelectorCancelFunc cancelFunc = this->cancelFunc;
            if (cancelFunc)
            {
                cancelFunc();
            }
            break;
        }
        case MenuButtonInput::B:
        {
            // Cancel bringing out a follower
            const FollowerSelectorCancelFunc func = this->cancelFunc;
            if (func)
            {
                func();
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

void FollowerSelector::draw() const
{
    // Draw the window
    const Window *windowPtr = &this->window;
    windowPtr->draw();

    // Initialize text drawing
    drawTextInit(false);

    // Draw the help text
    float tempPosX;
    float tempPosY;

    const float scale = this->scale;
    const char *helpTextPtr = helpTextAConfirmBCancel;

    windowPtr->getTextPosXY(helpTextPtr, WindowAlignment::TOP_CENTER, scale, &tempPosX, &tempPosY);
    drawText(helpTextPtr, tempPosX, tempPosY, scale, getColorWhite(0xFF));

    // Get the position for the left column
    const char *rightColumnLongestText = "Ms. Mowz"; // Use here since any arbitrary string is needed, and this is needed later
    windowPtr->getTextPosXY(rightColumnLongestText, WindowAlignment::BOTTOM_LEFT, scale, &tempPosX, &tempPosY);

    const float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    const float posYBase = tempPosY + (lineDecrement * (TOTAL_FOLLOWERS_PER_COLUMN_FLOAT - 1.f));

    uint32_t counter = 0;
    float posX = tempPosX;
    float posY = posYBase;
    const uint32_t currentIndex = this->currentIndex;
    const char **followersOptionsPtr = followersOptions;

    // Draw the options
    for (uint32_t i = 0; i < TOTAL_FOLLOWERS; i++, counter++)
    {
        if (counter >= TOTAL_FOLLOWERS_PER_COLUMN)
        {
            counter = 0;

            // Move to the next column
            posX = windowPtr->getTextPosX(rightColumnLongestText, WindowAlignment::TOP_RIGHT, scale);
            posY = posYBase;
        }

        uint32_t color = getCurrentOptionColor(currentIndex == i, 0xFF);
        drawText(followersOptionsPtr[i], posX, posY, scale, color);

        posY -= lineDecrement;
    }
}
