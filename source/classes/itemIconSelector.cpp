#include "drawIcon.h"
#include "drawText.h"
#include "menuUtils.h"
#include "classes/itemIconSelector.h"
#include "menus/inventory.h"
#include "misc/utils.h"
#include "ttyd/item_data.h"
#include "ttyd/windowdrv.h"

#include <cstdint>

const char *textOptions = "Press A to confirm\nPress B to cancel";

void ItemIconSelector::init(const Window *parentWindow, ItemId startingItem, ItemId endingItem, float scale)
{
    this->init(parentWindow, startingItem, endingItem, 0xFF, scale);
}

void ItemIconSelector::init(const Window *parentWindow,
                            ItemId startingItem,
                            ItemId endingItem,
                            uint8_t windowAlpha,
                            float scale)
{
    this->scale = scale;
    this->startingItem = startingItem;
    this->endingItem = endingItem;
    this->selectedItemFunc = nullptr;
    this->cancelFunc = nullptr;
    this->autoIncrement.waitFramesToBegin = 0;
    this->autoIncrement.shouldIncrementNow = false;
    this->enabled = false;
    this->currentIndex = 0;

    // Get the height that is being used by the text
    const char *text = textOptions;
    float textHeight;
    getTextWidthHeight(text, scale, nullptr, &textHeight);

    // Initialize the main window
    Window *windowPtr = &this->window;

    windowPtr->copyWindow(parentWindow);
    windowPtr->setColor(SPECIAL_WINDOW_COLOR | windowAlpha);
    windowPtr->setHeight(textHeight);

    // Adjust the height of the main window to account for all of the icons
    const int32_t totalIcons = static_cast<int32_t>(endingItem) - static_cast<int32_t>(startingItem) + 1;
    const int32_t totalIconRows = roundIntUpUnsigned(totalIcons, TOTAL_ICONS_PER_ROW);

    const float iconsWindowPaddingAdjustment = 12.f * scale;
    const float currentHeight = windowPtr->getHeight() - iconsWindowPaddingAdjustment;
    const float spaceUsedByIcons = intToFloat(totalIconRows) * SPACE_USED_PER_ICON(scale);
    const float padding = (windowPtr->getPadding() * 3.f) * scale;

    windowPtr->setHeight(currentHeight + spaceUsedByIcons + padding);

    // Place the main window in the parent window
    windowPtr->placeInWindow(parentWindow, WindowAlignment::MIDDLE_CENTER, scale);
}

void ItemIconSelector::dpadControls(MenuButtonInput button, uint32_t totalIcons)
{
    menuControlsHorizontal(button, &this->currentIndex, nullptr, totalIcons, totalIcons, TOTAL_ICONS_PER_ROW, false);
}

void ItemIconSelector::controls(MenuButtonInput button)
{
    // The function for checking for auto-incrementing needs to run every frame to be handled correctly
    const bool autoIncrement = handleMenuAutoIncrement(&this->autoIncrement);

    const uint32_t startingItem = static_cast<uint32_t>(this->startingItem);
    const uint32_t totalIcons = static_cast<uint32_t>(this->endingItem) - startingItem + 1;

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
                this->dpadControls(buttonHeld, totalIcons);
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
            this->dpadControls(button, totalIcons);
            break;
        }
        case MenuButtonInput::A:
        {
            // Make sure the current index is valid
            const uint32_t index = this->currentIndex;
            if (index >= totalIcons)
            {
                // Failsafe: Reset the current index to 0
                this->currentIndex = 0;
                break;
            }

            // Get the selected item
            const ItemId item = static_cast<ItemId>(index + startingItem);

            // Call the selected item function
            const SelectedItemFunc func = this->selectedItemFunc;
            if (func)
            {
                func(item);
            }

            break;
        }
        case MenuButtonInput::B:
        {
            // Call the cancel function
            const ItemSelectorCancelFunc func = this->cancelFunc;
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

void ItemIconSelector::draw()
{
    // Draw the main window
    const Window *windowPtr = &this->window;
    windowPtr->draw();

    // Initialize text drawing
    drawTextInit(false);

    // Draw the text
    const char *text = textOptions;
    const float scale = this->scale;

    float tempPosX;
    float tempPosY;
    windowPtr->getTextPosXY(text, WindowAlignment::TOP_CENTER, scale, &tempPosX, &tempPosY);
    drawText(text, tempPosX, tempPosY, scale, getColorWhite(0xFF));

    // Get the intitial position for the icons
    windowPtr->getIconPosXY(WindowAlignment::BOTTOM_LEFT, scale, &tempPosX, &tempPosY);

    int32_t index = static_cast<int32_t>(this->startingItem);
    const int32_t endingIndex = static_cast<int32_t>(this->endingItem);

    // Get the amount of space used by the icons based on the number of rows that there are
    const int32_t totalIconRows = roundIntUpUnsigned(endingIndex - index + 1, TOTAL_ICONS_PER_ROW);
    const float posXYIncrement = SPACE_USED_PER_ICON(scale);
    const float spaceUsedByIcons = intToFloat(totalIconRows - 1) * posXYIncrement;

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    float posX = tempPosX - (8.f * scale); // Account for padding difference on X axis; 20.f for text vs 12.f for icons
    float posY = tempPosY + spaceUsedByIcons;
    const float posXBase = posX;

    const uint32_t currentIndex = this->currentIndex + index;
    uint32_t counter = 0;
    float iconScale;

    // Draw the icons
    for (; index <= endingIndex; index++, counter++)
    {
        if (counter >= TOTAL_ICONS_PER_ROW)
        {
            counter = 0;

            // Move to the next row
            posX = posXBase;
            posY -= posXYIncrement;
        }

        if (static_cast<uint32_t>(index) == currentIndex)
        {
            // Draw the window for the cursor
            const uint32_t color = getColorBlue(0xFF);
            const float windowPosX = posX - (posXYIncrement / 2.f);
            const float windowPosY = posY + posXYIncrement - (5.f * scale);

            windowDispGX_Waku_col(0,
                                  reinterpret_cast<const uint8_t *>(&color),
                                  windowPosX,
                                  windowPosY,
                                  posXYIncrement,
                                  posXYIncrement,
                                  0.f);
        }

        const ItemId currentItem = static_cast<ItemId>(index);

        // Several items need to have their icon scales adjusted
        iconScale = adjustItemIconScale(currentItem, scale);

        drawIcon(posX, posY, iconScale, getIconFromItem(currentItem));
        posX += posXYIncrement;
    }
}
