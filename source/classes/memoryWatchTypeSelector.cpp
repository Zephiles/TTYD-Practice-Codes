#include "mod.h"
#include "drawText.h"
#include "menuUtils.h"
#include "classes/memoryWatchTypeSelector.h"
#include "classes/window.h"

#include <cstdint>

const char *gMemoryWatchTypeStrings[TOTAL_MEMORY_WATCH_TYPES] = {
    "int8_t",
    "int16_t",
    "int32_t",
    "int64_t",
    "uint8_t",
    "uint16_t",
    "uint32_t",
    "uint64_t",
    "float",
    "double",
    "string",
    "time",
};

void MemoryWatchTypeSelector::init(const Window *parentWindow)
{
    this->init(parentWindow, 0xFF);
}

void MemoryWatchTypeSelector::init(const Window *parentWindow, uint8_t windowAlpha)
{
    this->setTypeFunc = nullptr;
    this->cancelFunc = nullptr;
    this->autoIncrement.waitFramesToBegin = 0;
    this->autoIncrement.framesBeforeIncrement = 0;
    this->enabled = false;
    this->currentIndex = 0;

    // Set up the window
    // Initialize it based on the help text
    Window *windowPtr = &this->window;
    constexpr float scale = MENU_SCALE;
    windowPtr->setWidthHeightFromTextAndInit(gHelpTextAConfirmBCancel, scale, SPECIAL_WINDOW_COLOR | windowAlpha, 20.f, 20.f);

    // Increase the height of the window to account for the options
    const float padding = windowPtr->getPadding() * scale;
    constexpr float height = (LINE_HEIGHT_FLOAT * scale) * TOTAL_MEMORY_WATCH_TYPES_FLOAT;
    windowPtr->setHeight(windowPtr->getHeight() + height + padding - (padding / 2.f));

    // Place the window inside of the parent window
    windowPtr->placeInWindow(parentWindow, WindowAlignment::MIDDLE_CENTER, scale);
}

void MemoryWatchTypeSelector::controlsMoveDownOnce()
{
    int32_t currentIndex = static_cast<int32_t>(this->currentIndex);

    // Move down once
    currentIndex++;

    // Make sure the index is valid
    if (currentIndex >= TOTAL_MEMORY_WATCH_TYPES)
    {
        // Loop to the first option
        currentIndex = 0;
    }

    this->currentIndex = static_cast<uint8_t>(currentIndex);
}

void MemoryWatchTypeSelector::controlsMoveUpOnce()
{
    int32_t currentIndex = static_cast<int32_t>(this->currentIndex);

    // Move up once
    currentIndex--;

    // Make sure the index is valid
    if (currentIndex < 0)
    {
        // Loop to the last option
        currentIndex = TOTAL_MEMORY_WATCH_TYPES - 1;
    }

    this->currentIndex = static_cast<uint8_t>(currentIndex);
}

void MemoryWatchTypeSelector::controls(MenuButtonInput button)
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
        case MenuButtonInput::A:
        {
            // Make sure the current index is valid
            const uint32_t currentIndex = this->currentIndex;
            if (currentIndex >= TOTAL_MEMORY_WATCH_TYPES)
            {
                // Failsafe: Reset the current index to 0
                this->currentIndex = 0;
                break;
            }

            // Call the function for setting the new color
            const MemoryWatchTypeSelectorSetTypeFunc func = this->setTypeFunc;
            if (func)
            {
                return func(static_cast<VariableType>(currentIndex));
            }
            break;
        }
        case MenuButtonInput::B:
        {
            // Cancel toggling special moves
            const MemoryWatchTypeSelectorCancelFunc func = this->cancelFunc;
            if (func)
            {
                return func();
            }
            break;
        }
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

void MemoryWatchTypeSelector::draw()
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

    windowPtr->getTextPosXY(nullptr, WindowAlignment::TOP_LEFT, scale, &tempPosX, &tempPosY);
    drawText(gHelpTextAConfirmBCancel, tempPosX, tempPosY, scale, getColorWhite(0xFF));

    // Get the position for the options
    windowPtr->getTextPosXY("uint64_t", WindowAlignment::BOTTOM_CENTER, scale, &tempPosX, &tempPosY);

    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    const float posYBase = tempPosY + (lineDecrement * (TOTAL_MEMORY_WATCH_TYPES_FLOAT - 1.f));

    float posX = tempPosX;
    float posY = posYBase;
    const char **typeStrings = gMemoryWatchTypeStrings;
    uint32_t currentIndex = this->currentIndex;

    // Make sure the current index is valid
    if (currentIndex >= TOTAL_MEMORY_WATCH_TYPES)
    {
        // Failsafe: Reset the current index to 0
        currentIndex = 0;
        this->currentIndex = static_cast<uint8_t>(currentIndex);
    }

    // Draw the options
    for (uint32_t i = 0; i < TOTAL_MEMORY_WATCH_TYPES; i++)
    {
        const uint32_t color = getCurrentOptionColor(currentIndex == i, 0xFF);
        drawText(typeStrings[i], posX, posY, scale, color);
        posY -= lineDecrement;
    }
}
