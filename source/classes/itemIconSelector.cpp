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
    const int32_t totalIconRows = 1 + ((totalIcons - 1) / TOTAL_ICONS_PER_ROW); // Round up

    const float iconsWindowPaddingAdjustment = 12.f * scale;
    const float currentHeight = windowPtr->getHeight() - iconsWindowPaddingAdjustment;
    const float spaceUsedByIcons = intToFloat(totalIconRows) * SPACE_USED_PER_ICON(scale);
    const float padding = (windowPtr->getPadding() * 3.f) * scale;

    windowPtr->setHeight(currentHeight + spaceUsedByIcons + padding);

    // Place the main window in the parent window
    windowPtr->placeInWindow(parentWindow, WindowAlignment::MIDDLE_CENTER, scale);

    // Set the icons window to be a bit under the text
    Window *iconsWindowPtr = &this->iconsWindow;
    iconsWindowPtr->copyWindow(windowPtr);

    // Adjust the window to exclude the space used by the text, plus a bit extra
    const float totalTextHeight = textHeight - iconsWindowPaddingAdjustment + padding;
    iconsWindowPtr->setPosY(iconsWindowPtr->getPosY() - totalTextHeight);
    iconsWindowPtr->setHeight(iconsWindowPtr->getHeight() - totalTextHeight);
    iconsWindowPtr->setPadding(12.f);

    // Place the icons window in the main window
    iconsWindowPtr->placeInWindow(windowPtr, WindowAlignment::BOTTOM_CENTER, scale);
}

void ItemIconSelector::controls(MenuButtonInput button)
{
    const uint32_t startingItem = static_cast<uint32_t>(this->startingItem);
    const uint32_t totalIcons = static_cast<uint32_t>(this->endingItem) - startingItem + 1;

    switch (button)
    {
        case MenuButtonInput::DPAD_LEFT:
        case MenuButtonInput::DPAD_RIGHT:
        case MenuButtonInput::DPAD_DOWN:
        case MenuButtonInput::DPAD_UP:
        {
            menuControlsHorizontal(button, &this->currentIndex, nullptr, totalIcons, totalIcons, TOTAL_ICONS_PER_ROW, false);
            break;
        }
        case MenuButtonInput::A:
        {
            // Make sure currentIndex is valid
            const uint32_t index = this->currentIndex;
            if (index >= totalIcons)
            {
                // Failsafe: Reset currentIndex to 0
                this->currentIndex = 0;
                break;
            }

            // Get the selected item
            const ItemId item = static_cast<ItemId>(index + startingItem);

            // Call the selected item function
            SelectedItemFunc func = this->selectedItemFunc;
            if (func)
            {
                func(item);
            }

            break;
        }
        case MenuButtonInput::B:
        {
            // Call the cancel function
            ItemSelectorCancelFunc func = this->cancelFunc;
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

    /*
    // Draw the icons window for debugging purposes
    Window *iconsWindowPtr = &this->iconsWindow;
    iconsWindowPtr->setColor(0x505050FF);
    iconsWindowPtr->draw();
    */

    // Initialize text drawing
    drawTextInit(false);

    // Draw the text
    const char *text = textOptions;
    const float scale = this->scale;

    float tempPosX;
    float tempPosY;
    windowPtr->getTextPosXY(text, WindowAlignment::TOP_CENTER, scale, &tempPosX, &tempPosY);
    drawText(text, tempPosX, tempPosY, scale, getColorWhite(0xFF));

    // Draw the icons
    const Window *iconsWindowPtr = &this->iconsWindow;
    iconsWindowPtr->getIconPosXY(WindowAlignment::TOP_LEFT, scale, &tempPosX, &tempPosY);

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    float posX = tempPosX;
    float posY = tempPosY;

    int32_t index = static_cast<int32_t>(this->startingItem);
    const int32_t endingIndex = static_cast<int32_t>(this->endingItem);
    const uint32_t currentIndex = this->currentIndex + index;

    const float posXBase = posX;
    const float posXYIncrement = SPACE_USED_PER_ICON(scale);
    uint32_t counter = 0;
    float iconScale;

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