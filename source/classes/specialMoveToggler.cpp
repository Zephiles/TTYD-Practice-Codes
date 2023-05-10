#include "drawIcon.h"
#include "menuUtils.h"
#include "drawText.h"
#include "classes/specialMoveToggler.h"
#include "misc/utils.h"
#include "ttyd/icondrv.h"
#include "ttyd/fontmgr.h"
#include "ttyd/mario_pouch.h"

#include <cstdint>

const IconId specialMoveIcons[TOTAL_SPECIAL_MOVES] {
    IconId::ICON_MAGICAL_MAP_SMALL,
    IconId::ICON_DIAMOND_STAR,
    IconId::ICON_EMERALD_STAR,
    IconId::ICON_GOLD_STAR,
    IconId::ICON_RUBY_STAR,
    IconId::ICON_SAPPHIRE_STAR,
    IconId::ICON_GARNET_STAR,
    IconId::ICON_CRYSTAL_STAR,
};

const char *helpText = "Press A to turn on/off\nPress B to cancel";

const char *specialMoves[TOTAL_SPECIAL_MOVES] = {
    "Sweet Treat",
    "Earth Tremor",
    "Clock Out",
    "Power Lift",
    "Art Attack",
    "Sweet Feast",
    "Showstopper",
    "Supernova",
};

void SpecialMoveToggler::init(const Window *parentWindow, float scale)
{
    this->init(parentWindow, scale, 0xFF);
}

void SpecialMoveToggler::init(const Window *parentWindow, float scale, uint8_t alpha)
{
    this->scale = scale;
    this->alpha = alpha;
    this->cancelFunc = nullptr;
    this->waitFramesToBegin = 0;
    this->shouldIncrementNow = false;
    this->enabled = false;
    this->currentIndex = 0;

    // Set up the window
    // Initialize it based on the help text
    Window *windowPtr = &this->window;
    windowPtr->setWidthHeightFromTextAndInit(helpText, scale, SPECIAL_WINDOW_COLOR | alpha, 20.f, 30.f);

    // Increase the height of the window to account for the icon and item texts
    const float spaceUsedByIcons = SPACE_USED_PER_ICON(scale) * (TOTAL_SPECIAL_MOVES_FLOAT - 1.f);
    const float padding = windowPtr->getPadding() * scale;

    windowPtr->setHeight(windowPtr->getHeight() + spaceUsedByIcons + (padding * 2.f));

    // Place the window inside of the parent window
    windowPtr->placeInWindow(parentWindow, WindowAlignment::MIDDLE_CENTER, scale);
}

void SpecialMoveToggler::controlsMoveDownOnce()
{
    int32_t currentIndex = static_cast<int32_t>(this->currentIndex);

    // Move down once
    currentIndex++;

    // Make sure the index is valid
    if (currentIndex >= TOTAL_SPECIAL_MOVES)
    {
        // Loop to the first option
        currentIndex = 0;
    }

    this->currentIndex = static_cast<uint8_t>(currentIndex);
}

void SpecialMoveToggler::controlsMoveUpOnce()
{
    int32_t currentIndex = static_cast<int32_t>(this->currentIndex);

    // Move up once
    currentIndex--;

    // Make sure the index is valid
    if (currentIndex < 0)
    {
        // Loop to the last option
        currentIndex = TOTAL_SPECIAL_MOVES - 1;
    }

    this->currentIndex = static_cast<uint8_t>(currentIndex);
}

void SpecialMoveToggler::controls(MenuButtonInput button)
{
    // The function for checking for auto-incrementing needs to run every frame to be handled correctly
    const bool autoIncrement = handleMenuAutoIncrementDigit(&this->waitFramesToBegin, &this->shouldIncrementNow);

    // Check for D-Pad Up/Down first to handle auto-incrementing
    const MenuButtonInput buttonHeld = getMenuButtonInput(false);

    // Handle held button inputs if auto-incrementing should be done
    if (autoIncrement)
    {
        switch (buttonHeld)
        {
            case MenuButtonInput::DPAD_DOWN:
            {
                this->controlsMoveDownOnce();
                break;
            }
            case MenuButtonInput::DPAD_UP:
            {
                this->controlsMoveUpOnce();
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
        case MenuButtonInput::DPAD_DOWN:
        {
            this->controlsMoveDownOnce();
            break;
        }
        case MenuButtonInput::DPAD_UP:
        {
            this->controlsMoveUpOnce();
            break;
        }
        case MenuButtonInput::A:
        {
            // Make sure currentIndex is valid
            const uint32_t currentIndex = this->currentIndex;
            if (currentIndex >= TOTAL_SPECIAL_MOVES)
            {
                // Failsafe: Reset currentIndex to 0
                this->currentIndex = 0;
                break;
            }

            // Toggle the bit for the current special move
            pouchGetPtr()->starPowersObtained ^= (1UL << currentIndex);
            break;
        }
        case MenuButtonInput::B:
        {
            // Cancel toggling special moves
            const SpecialMoveTogglerCancelFunc func = this->cancelFunc;
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

void SpecialMoveToggler::draw()
{
    // Draw the window
    const Window *windowPtr = &this->window;
    windowPtr->draw();

    // To avoid a lot of unnecessary extra GX calls, draw the icons first
    // Get the starting position for the icons
    float iconPosXBase;
    float iconPosYBase;

    const float scale = this->scale;
    windowPtr->getIconPosXY(WindowAlignment::BOTTOM_LEFT, scale, &iconPosXBase, &iconPosYBase);

    // Move the starting position so that it is below the help text
    const float lineDecrement = SPACE_USED_PER_ICON(scale);
    const float spaceUsedByIcons = lineDecrement * (TOTAL_SPECIAL_MOVES_FLOAT - 1.f);

    iconPosYBase += spaceUsedByIcons;

    float iconPosX = iconPosXBase;
    float iconPosY = iconPosYBase;
    const IconId *icons = specialMoveIcons;

    // Draw the icons
    for (uint32_t i = 0; i < TOTAL_SPECIAL_MOVES; i++)
    {
        drawIcon(iconPosX, iconPosY, scale, icons[i]);
        iconPosY -= lineDecrement;
    }

    // Initialize text drawing
    drawTextInit(false);

    // Draw the help text
    float textPosXBase;
    float textPosYBase;

    const char *helpTextPtr = helpText;
    windowPtr->getTextPosXY(helpTextPtr, WindowAlignment::TOP_CENTER, scale, &textPosXBase, &textPosYBase);
    drawText(helpTextPtr, textPosXBase, textPosYBase, scale, getColorWhite(0xFF));

    // Get the starting position for the special move texts
    getTextPosXYByIcon(iconPosXBase, iconPosYBase, scale, &textPosXBase, &textPosYBase);

    // Get the special moves that are currently enabled
    const uint32_t starPowersObtained = pouchGetPtr()->starPowersObtained;

    // Set up the bools to be drawn on the right edge of the window minus the padding
    const float boolsPosX = windowPtr->getTextPosX("Off", WindowAlignment::TOP_RIGHT, scale);

    // Retrieve textPosX and textPosY as separate variables to avoid repeatedly loading them from the stack when using them
    const float textPosX = textPosXBase;
    float textPosY = textPosYBase;

    // Draw the texts for each special move as well as the bools for whether they are enabled or not
    const char **specialMovesPtr = specialMoves;
    const uint32_t currentIndex = this->currentIndex;

    for (uint32_t i = 0; i < TOTAL_SPECIAL_MOVES; i++)
    {
        // Draw the text
        uint32_t color = getCurrentOptionColor(currentIndex == i, 0xFF);
        drawText(specialMovesPtr[i], textPosX, textPosY, scale, color);

        // Draw the bool
        const bool specialMoveEnabled = (starPowersObtained >> i) & 1U;
        const char *onOffText;
        getOnOffTextAndColor(specialMoveEnabled, &onOffText, &color, 0xFF);
        drawText(onOffText, boolsPosX, textPosY, scale, color);

        textPosY -= lineDecrement;
    }
}
