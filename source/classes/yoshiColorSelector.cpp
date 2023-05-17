#include "mod.h"
#include "drawText.h"
#include "menuUtils.h"
#include "classes/yoshiColorSelector.h"
#include "classes/window.h"

#include <cstdint>

const char *yoshiColorsStrings[TOTAL_YOSHI_COLORS] = {
    "Green",
    "Red",
    "Blue",
    "Orange",
    "Pink",
    "Black",
    "White",
};

void YoshiColorSelector::init(const Window *parentWindow, float scale)
{
    this->init(parentWindow, scale, 0xFF);
}

void YoshiColorSelector::init(const Window *parentWindow, float scale, uint8_t alpha)
{
    this->scale = scale;
    this->alpha = alpha;
    this->setColorFunc = nullptr;
    this->cancelFunc = nullptr;
    this->autoIncrement.waitFramesToBegin = 0;
    this->autoIncrement.framesBeforeIncrement = 0;
    this->enabled = false;
    this->currentIndex = 0;

    // Set up the window
    // Initialize it based on the help text
    Window *windowPtr = &this->window;
    windowPtr->setWidthHeightFromTextAndInit(helpTextAConfirmBCancel, scale, SPECIAL_WINDOW_COLOR | alpha, 20.f, 30.f);

    // Increase the height of the window to account for the options
    const float padding = windowPtr->getPadding() * scale;
    const float height = (LINE_HEIGHT_FLOAT * scale) * TOTAL_YOSHI_COLORS_FLOAT;
    windowPtr->setHeight(windowPtr->getHeight() + height + padding - (padding / 2.f));

    // Place the window inside of the parent window
    windowPtr->placeInWindow(parentWindow, WindowAlignment::MIDDLE_CENTER, scale);
}

void YoshiColorSelector::controlsMoveDownOnce()
{
    int32_t currentIndex = static_cast<int32_t>(this->currentIndex);

    // Move down once
    currentIndex++;

    // Make sure the index is valid
    if (currentIndex >= TOTAL_YOSHI_COLORS)
    {
        // Loop to the first option
        currentIndex = 0;
    }

    this->currentIndex = static_cast<uint8_t>(currentIndex);
}

void YoshiColorSelector::controlsMoveUpOnce()
{
    int32_t currentIndex = static_cast<int32_t>(this->currentIndex);

    // Move up once
    currentIndex--;

    // Make sure the index is valid
    if (currentIndex < 0)
    {
        // Loop to the last option
        currentIndex = TOTAL_YOSHI_COLORS - 1;
    }

    this->currentIndex = static_cast<uint8_t>(currentIndex);
}

void YoshiColorSelector::controls(MenuButtonInput button)
{
    // The function for checking for auto-incrementing needs to run every frame to be handled correctly
    const bool autoIncrement = handleMenuAutoIncrement(&this->autoIncrement);

    // Handle held button inputs if auto-incrementing should be done
    if (autoIncrement)
    {
        switch (getMenuButtonInput(false))
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
            // Make sure the current index is valid
            const uint32_t currentIndex = this->currentIndex;
            if (currentIndex >= TOTAL_YOSHI_COLORS)
            {
                // Failsafe: Reset the current index to 0
                this->currentIndex = 0;
                break;
            }

            // Call the function for setting the new color
            const YoshiColorSelectorSetColorFunc setColorFunc = this->setColorFunc;
            if (setColorFunc)
            {
                setColorFunc(currentIndex);
            }

            // Close the color selector
            const YoshiColorSelectorCancelFunc cancelFunc = this->cancelFunc;
            if (cancelFunc)
            {
                cancelFunc();
            }
            break;
        }
        case MenuButtonInput::B:
        {
            // Cancel toggling special moves
            const YoshiColorSelectorCancelFunc func = this->cancelFunc;
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

void YoshiColorSelector::draw()
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

    // Get the position for the options
    windowPtr->getTextPosXY("Orange", WindowAlignment::BOTTOM_CENTER, scale, &tempPosX, &tempPosY);

    const float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    const float posYBase = tempPosY + (lineDecrement * (TOTAL_YOSHI_COLORS_FLOAT - 1.f));

    float posX = tempPosX;
    float posY = posYBase;
    const char **colorStrings = yoshiColorsStrings;
    uint32_t currentIndex = this->currentIndex;

    // Make sure the current index is valid
    if (currentIndex >= TOTAL_YOSHI_COLORS)
    {
        // Failsafe: Reset the current index to 0
        currentIndex = 0;
        this->currentIndex = static_cast<uint8_t>(currentIndex);
    }

    // Draw the options
    for (uint32_t i = 0; i < TOTAL_YOSHI_COLORS; i++)
    {
        const uint32_t color = getCurrentOptionColor(currentIndex == i, 0xFF);
        drawText(colorStrings[i], posX, posY, scale, color);
        posY -= lineDecrement;
    }
}
