#ifndef CLASSES_ITEMICONSELECTOR_H
#define CLASSES_ITEMICONSELECTOR_H

#include "classes/window.h"
#include "classes/menu.h"
#include "ttyd/item_data.h"

#include <cstdint>

#define TOTAL_ICONS_PER_ROW 16 // Total amount of item icons per row in the item icon selector window

/**
 * Callback function pointer for when the player selects an item.
 *
 * @param item Item that was selected.
 */
typedef void (*SelectedItemFunc)(ItemId item);

// Callback function pointer for when the player presses `B` to cancel selecting an item.
typedef void (*ItemSelectorCancelFunc)();

// Handles selecting an item from a range of items, which is provided via a starting and ending item. A help window with text
// and item icons is displayed to assist in this process.
class ItemIconSelector
{
   public:
    // Generic constructor.
    ItemIconSelector() {}

    // Generic deconstructor.
    ~ItemIconSelector() {}

    /**
     * Initializes the item icon selector.
     *
     * @param parentWindow The window to place the item icon selector's window in.
     * @param startingItem The item that the displayed icons will start at.
     * @param endingItem The item that the displayed icons will end at.
     *
     * @note This function calls the `init` function that takes `windowAlpha` as a parameter, in which `0xFF` is passed in for
     * that value.
     */
    void init(const Window *parentWindow, ItemId startingItem, ItemId endingItem);

    /**
     * Initializes the item icon selector.
     *
     * @param parentWindow The window to place the item icon selector's window in.
     * @param startingItem The item that the displayed icons will start at.
     * @param endingItem The item that the displayed icons will end at.
     * @param windowAlpha The value to set the item icon selector's window alpha to.
     *
     * @overload
     */
    void init(const Window *parentWindow, ItemId startingItem, ItemId endingItem, uint8_t windowAlpha);

    /**
     * Updates the current value of the `currentIndex` variable.
     *
     * @param index The value to set the `currentIndex` variable to.
     */
    void setCurrentIndex(uint32_t index) { this->currentIndex = static_cast<uint8_t>(index); }

    /**
     * Updates the current value of the `currentIndex` variable based on an item.
     *
     * @param item The item used to update the value of the `currentIndex` variable. This is done by subtracting the item's
     * value from the starting item's value.
     */
    void setCurrentIndexFromItem(ItemId item)
    {
        this->currentIndex = static_cast<uint8_t>(static_cast<uint32_t>(item) - static_cast<uint32_t>(this->startingItem));
    }

    /**
     * Checks to see if the item icon selector should be drawn this frame.
     *
     * @returns `true` if the item icon selector should be drawn this frame, otherwise `false`.
     */
    bool shouldDraw() const { return this->enabled; }

    /**
     * Sets the item icon selector to be drawn.
     *
     * @param selectedItemFunc Callback function for when the player selects an item.
     * @param cancelFunc Callback function for when the player presses `B` to cancel selecting an item.
     */
    void startDrawing(SelectedItemFunc selectedItemFunc, ItemSelectorCancelFunc cancelFunc)
    {
        this->selectedItemFunc = selectedItemFunc;
        this->cancelFunc = cancelFunc;
        this->enabled = true;
    }

    // Sets the item icon selector to not be drawn.
    void stopDrawing() { this->enabled = false; }

    /**
     * Handles the controls for the item icon selector.
     *
     * @param button The button that was pressed this frame.
     */
    void controls(MenuButtonInput button);

    // Draws the item icon selector.
    void draw();

   private:
    /**
     * Handles the D-Pad controls for the item icon selector.
     *
     * @param button The button that was pressed this frame.
     * @param totalIcons The total amount of icons to choose from.
     */
    void dpadControls(MenuButtonInput button, uint32_t totalIcons);

    // The window to place all of the item icon selector's text and icons in.
    Window window;

    // Callback function for when the player selects an item/badge.
    SelectedItemFunc selectedItemFunc;

    // Callback function for when the player presses `B` to stop selecting items/badges.
    ItemSelectorCancelFunc cancelFunc;

    // The item to start at.
    ItemId startingItem;

    // The item to end at.
    ItemId endingItem;

    // Used to handle automatically moving the cursor when a `D-Pad` direction is held.
    MenuAutoIncrement autoIncrement;

    // Whether the item icon selector is enabled/drawn or not.
    bool enabled;

    // Current cursor position.
    uint8_t currentIndex;
};

#endif
