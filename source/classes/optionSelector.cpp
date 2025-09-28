#include "drawText.h"
#include "menuUtils.h"
#include "classes/optionSelector.h"
#include "classes/window.h"
#include "misc/utils.h"

#include <cstdint>

void OptionSelector::init(const char *helpText,
                          const char **options,
                          uint32_t totalOptions,
                          uint32_t totalColumns,
                          const Window *parentWindow,
                          float spaceBetweenHelpTextAndOptions)
{
    return this->init(helpText, options, totalOptions, totalColumns, parentWindow, 0xFF, spaceBetweenHelpTextAndOptions);
}

void OptionSelector::init(const char *helpText,
                          const char **options,
                          uint32_t totalOptions,
                          uint32_t totalColumns,
                          const Window *parentWindow,
                          uint8_t windowAlpha,
                          float spaceBetweenHelpTextAndOptions)
{
    this->helpText = helpText;
    this->options = options;
    this->totalOptions = static_cast<uint8_t>(totalOptions);
    this->totalColumns = static_cast<uint8_t>(totalColumns);

    this->selectOptionFunc = nullptr;
    this->cancelFunc = nullptr;
    this->autoIncrement.waitFramesToBegin = 0;
    this->autoIncrement.framesBeforeIncrement = 0;
    this->currentIndex = 0;
    this->enabled = false;

    // Set up the window
    // Initialize it based on the help text
    Window *windowPtr = &this->window;
    constexpr float scale = MENU_SCALE;
    windowPtr->setWidthHeightFromTextAndInit(helpText, scale, getSpecialWindowColor(windowAlpha), 20.f, 20.f);

    // Get the height used by the options
    const uint32_t totalOptionsPerColumn = intCeil(totalOptions, totalColumns);
    const float totalOptionsPerColumnFloat = intToFloat(static_cast<int32_t>(totalOptionsPerColumn));
    float height = (LINE_HEIGHT_FLOAT * scale) * totalOptionsPerColumnFloat;

    // Increase the height to account for spaceBetweenHelpTextAndOptions
    height += spaceBetweenHelpTextAndOptions * scale;

    // Increase the height to account for the padding between the help text and the options
    const float padding = windowPtr->getPadding() * scale;
    height += padding / 2.f;

    // Set the new height
    windowPtr->setHeight(windowPtr->getHeight() + height);

    // Get the total width of all of the options
    float optionsWidth = padding * 2.f; // The options will be placed between padding

    // Assume that each column will have OPTION_SELECTOR_COLUMN_PADDING padding between them
    optionsWidth += OPTION_SELECTOR_COLUMN_PADDING(scale) * (intToFloat(totalColumns - 1));

    bool reachedEndOfOptions = false;
    for (uint32_t j = 0; j < totalColumns; j++)
    {
        if (reachedEndOfOptions)
        {
            // Reached the end of the options, so exit
            break;
        }

        // Get the width of the longest text in the current column
        const uint32_t startingIndex = j * totalOptionsPerColumn;
        const uint32_t endingIndex = startingIndex + totalOptionsPerColumn;
        float columnWidth = 0.f;

        for (uint32_t i = startingIndex; i < endingIndex; i++)
        {
            if (i >= totalOptions)
            {
                // Reached the end of the options, so exit
                reachedEndOfOptions = true;
                break;
            }

            // Get the width of the current option and update columnWidth if necessary
            const float currentWidth = getTextWidth(options[i], scale);
            if (currentWidth > columnWidth)
            {
                columnWidth = currentWidth;
            }
        }

        optionsWidth += columnWidth;
    }

    this->totalWidth = optionsWidth;

    // If the width of the options will be larger than the current width, then reset the width to the width of the options
    if (optionsWidth > windowPtr->getWidth())
    {
        windowPtr->setWidth(optionsWidth);
    }

    // Place the window inside of the parent window
    windowPtr->placeInWindow(parentWindow, WindowAlignment::MIDDLE_CENTER, scale);
}

void OptionSelector::dpadControls(MenuButtonInput button)
{
    const uint32_t totalOptions = this->totalOptions;
    const uint32_t totalColumns = this->totalColumns;
    const uint32_t totalOptionsPerColumn = intCeil(totalOptions, totalColumns);

    menuControlsVertical(button,
                         &this->currentIndex,
                         nullptr,
                         totalOptions,
                         totalOptionsPerColumn * totalColumns,
                         totalColumns,
                         true);
}

void OptionSelector::controls(MenuButtonInput button)
{
    // The function for checking for auto-incrementing needs to run every frame to be handled correctly
    const bool autoIncrement = handleMenuAutoIncrement(&this->autoIncrement);

    // Handle held button input if auto-incrementing should be done
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
                this->dpadControls(buttonHeld);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    // Handle the button input pressed this frame
    switch (button)
    {
        case MenuButtonInput::DPAD_LEFT:
        case MenuButtonInput::DPAD_RIGHT:
        case MenuButtonInput::DPAD_DOWN:
        case MenuButtonInput::DPAD_UP:
        {
            this->dpadControls(button);
            break;
        }
        case MenuButtonInput::A:
        {
            // Make sure the current index is valid
            const uint32_t currentIndex = this->currentIndex;
            if (currentIndex >= this->totalOptions)
            {
                // Failsafe: Reset the current index to 0
                this->currentIndex = 0;
                break;
            }

            // Call the select option function
            const OptionSelectorSelectOptionFunc func = this->selectOptionFunc;
            if (func)
            {
                return func(currentIndex, this->classPtr);
            }
            break;
        }
        case MenuButtonInput::B:
        {
            // Call the cancel function
            const OptionSelectorCancelFunc func = this->cancelFunc;
            if (func)
            {
                return func(this->classPtr);
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

void OptionSelector::draw() const
{
    // Draw the window
    const Window *windowPtr = &this->window;
    windowPtr->draw();

    // Initialize text drawing
    drawTextInit(false);

    // Draw the help text
    constexpr float scale = MENU_SCALE;
    const char *helpTextPtr = this->helpText;

    float tempPosX;
    float tempPosY;
    windowPtr->getTextPosXY(helpTextPtr, WindowAlignment::TOP_CENTER, scale, &tempPosX, &tempPosY);
    drawText(helpTextPtr, tempPosX, tempPosY, scale, getColorWhite(0xFF));

    // Get the initial position for the options
    const char **optionsPtr = this->options;
    windowPtr->getTextPosXY(optionsPtr[0], WindowAlignment::BOTTOM_LEFT, scale, &tempPosX, &tempPosY);

    const uint32_t totalOptions = this->totalOptions;
    const uint32_t totalColumns = this->totalColumns;
    const uint32_t totalOptionsPerColumn = intCeil(totalOptions, totalColumns);

    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    const float posYBase = tempPosY + (lineDecrement * intToFloat(totalOptionsPerColumn - 1));
    float posY = posYBase;

    // Assume that each column will have OPTION_SELECTOR_COLUMN_PADDING padding between them
    constexpr float spaceBetweenColumns = OPTION_SELECTOR_COLUMN_PADDING(scale);
    float posX = tempPosX + (windowPtr->getWidth() / 2.f) - (this->totalWidth / 2.f);

    // Draw the options
    const uint32_t currentIndex = this->currentIndex;
    bool reachedEndOfOptions = false;

    for (uint32_t j = 0; j < totalColumns; j++)
    {
        if (reachedEndOfOptions)
        {
            // Reached the end of the options, so exit
            break;
        }

        // Draw each option in the current column
        const uint32_t startingIndex = j * totalOptionsPerColumn;
        const uint32_t endingIndex = startingIndex + totalOptionsPerColumn;
        float columnWidth = 0.f;

        for (uint32_t i = startingIndex; i < endingIndex; i++)
        {
            if (i >= totalOptions)
            {
                // Reached the end of the options, so exit
                reachedEndOfOptions = true;
                break;
            }

            // Draw the current option
            const char *currentOptionPtr = optionsPtr[i];
            const uint32_t color = getCurrentOptionColor(currentIndex == i, 0xFF);
            drawText(currentOptionPtr, posX, posY, scale, color);
            posY -= lineDecrement;

            // Get the width of the current option and update columnWidth if necessary
            const float currentWidth = getTextWidth(currentOptionPtr, scale);
            if (currentWidth > columnWidth)
            {
                columnWidth = currentWidth;
            }
        }

        posX += columnWidth + spaceBetweenColumns;
        posY = posYBase;
    }
}
