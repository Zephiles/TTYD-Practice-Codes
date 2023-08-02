#ifndef CLASSES_ITEMICONSELECTOR_H
#define CLASSES_ITEMICONSELECTOR_H

#include "classes/menu.h"
#include "classes/window.h"
#include "ttyd/item_data.h"

#include <cstdint>

#define TOTAL_ICONS_PER_ROW 16

typedef void (*SelectedItemFunc)(ItemId item);
typedef void (*ItemSelectorCancelFunc)();

class ItemIconSelector
{
   public:
    ItemIconSelector() {}
    ~ItemIconSelector() {}

    // Sets windowAlpha to 0xFF
    void init(const Window *parentWindow, ItemId startingItem, ItemId endingItem);

    void init(const Window *parentWindow, ItemId startingItem, ItemId endingItem, uint8_t windowAlpha);

    void setCurrentIndex(uint32_t index) { this->currentIndex = static_cast<uint8_t>(index); }

    void setCurrentIndexFromItem(ItemId item)
    {
        this->currentIndex = static_cast<uint8_t>(static_cast<uint32_t>(item) - static_cast<uint32_t>(this->startingItem));
    }

    void dpadControls(MenuButtonInput button, uint32_t totalIcons);
    void controls(MenuButtonInput button);

    void startDrawing(SelectedItemFunc selectedItemFunc, ItemSelectorCancelFunc cancelFunc)
    {
        this->selectedItemFunc = selectedItemFunc;
        this->cancelFunc = cancelFunc;
        this->enabled = true;
    }

    bool shouldDraw() const { return this->enabled; }
    void stopDrawing() { this->enabled = false; }

    void draw();

   private:
    Window window;

    SelectedItemFunc selectedItemFunc; // Called when the player presses A to select an item/badge
    ItemSelectorCancelFunc cancelFunc; // Called when the player presses B to stop selecting items/badges

    ItemId startingItem; // Starting index
    ItemId endingItem;   // Ending index

    MenuAutoIncrement autoIncrement;
    bool enabled;         // Whether this window is enabled/drawn or not
    uint8_t currentIndex; // Current cursor position
};

#endif
