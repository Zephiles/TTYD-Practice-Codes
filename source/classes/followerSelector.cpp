#include "mod.h"
#include "drawText.h"
#include "menuUtils.h"
#include "classes/followerSelector.h"
#include "classes/window.h"
#include "misc/utils.h"
#include "ttyd/party.h"

#include <cstdint>

const char *gFollowersOptions[TOTAL_FOLLOWERS] = {
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

// Intended to be used alongside gFollowersOptions
const PartyMembers gFollowersOptionsId[TOTAL_FOLLOWERS] = {
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

void FollowerSelector::init(const Window *parentWindow)
{
    this->init(parentWindow, 0xFF);
}

void FollowerSelector::init(const Window *parentWindow, uint8_t alpha)
{
    this->alpha = alpha;
    this->selectFunc = nullptr;
    this->cancelFunc = nullptr;
    this->autoIncrement.waitFramesToBegin = 0;
    this->autoIncrement.framesBeforeIncrement = 0;
    this->enabled = false;
    this->currentIndex = 0;

    // Set up the window
    // Initialize it based on the help text
    Window *windowPtr = &this->window;
    constexpr float scale = MENU_SCALE;
    windowPtr->setWidthHeightFromTextAndInit(gHelpTextAConfirmBCancel, scale, SPECIAL_WINDOW_COLOR | alpha, 20.f, 30.f);

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
    constexpr float height = (LINE_HEIGHT_FLOAT * scale) * TOTAL_FOLLOWERS_PER_COLUMN_FLOAT;
    windowPtr->setHeight(windowPtr->getHeight() + height + padding - (padding / 2.f));

    // Place the window inside of the parent window
    windowPtr->placeInWindow(parentWindow, WindowAlignment::MIDDLE_CENTER, scale);
}

void followerSelectorDPadControls(MenuButtonInput button, uint8_t *currentIndexPtr)
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
                followerSelectorDPadControls(buttonHeld, &this->currentIndex);
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
            followerSelectorDPadControls(button, &this->currentIndex);
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

            // Run the function for when a follower is selected
            const FollowerSelectorSelectFunc selectFunc = this->selectFunc;
            if (selectFunc)
            {
                return selectFunc(gFollowersOptionsId[currentIndex]);
            }
            break;
        }
        case MenuButtonInput::B:
        {
            // Cancel selecting a follower
            const FollowerSelectorCancelFunc func = this->cancelFunc;
            if (func)
            {
                return func();
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

    constexpr float scale = MENU_SCALE;
    const char *helpTextPtr = gHelpTextAConfirmBCancel;

    windowPtr->getTextPosXY(helpTextPtr, WindowAlignment::TOP_CENTER, scale, &tempPosX, &tempPosY);
    drawText(helpTextPtr, tempPosX, tempPosY, scale, getColorWhite(0xFF));

    // Get the position for the left column
    const char *rightColumnLongestText = "Ms. Mowz"; // Use here since any arbitrary string is needed, and this is needed later
    windowPtr->getTextPosXY(rightColumnLongestText, WindowAlignment::BOTTOM_LEFT, scale, &tempPosX, &tempPosY);

    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    const float posYBase = tempPosY + (lineDecrement * (TOTAL_FOLLOWERS_PER_COLUMN_FLOAT - 1.f));

    uint32_t counter = 0;
    float posX = tempPosX;
    float posY = posYBase;
    const uint32_t currentIndex = this->currentIndex;
    const char **followersOptionsPtr = gFollowersOptions;

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
