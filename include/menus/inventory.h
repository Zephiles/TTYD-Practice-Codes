#ifndef MENUS_INVENTORY_H
#define MENUS_INVENTORY_H

#include "classes/menu.h"
#include "classes/errorWindow.h"
#include "classes/itemIconSelector.h"
#include "classes/valueEditor.h"
#include "classes/window.h"
#include "ttyd/item_data.h"
#include "ttyd/dispdrv.h"

#include <cstdint>
#include <cstring>

#define INVENTORY_WIDTH_ADJUSTMENT(scale) (10.f * (scale))

#define INVENTORY_ITEMS_PER_COLUMN 10
#define INVENTORY_ITEMS_PER_PAGE (INVENTORY_ITEMS_PER_COLUMN * 2)
#define INVENTORY_ITEMS_PER_ROW (INVENTORY_ITEMS_PER_PAGE / INVENTORY_ITEMS_PER_COLUMN)

enum InventoryType
{
    STANDARD = 0,
    IMPORTANT,
    BADGES,
    STORED,
};

enum InventoryFlag
{
    INVENTORY_FLAG_ADD_BY_ID = 0,
    // INVENTORY_FLAG_ADD_BY_ICON,
    INVENTORY_FLAG_DUPLICATE,
    INVENTORY_FLAG_CHANGE_BY_ID,
    INVENTORY_FLAG_CHANGE_BY_ICON,
    INVENTORY_FLAG_DELETE,
    INVENTORY_FLAG_CURRENTLY_SELECTING_ID,
    INVENTORY_FLAG_CURRENTLY_SELECTING_ICON,
};

class Inventory
{
   public:
    Inventory(float scale);
    ~Inventory() {}

    void setinventoryItemPtr(ItemId *ptr) { this->inventoryItemPtr = ptr; }
    void setMinValue(ItemId value) { this->minValue = value; }
    void setMaxValue(ItemId value) { this->maxValue = value; }
    void setInventoryType(uint32_t type) { this->inventoryType = static_cast<uint8_t>(type); }
    void setInventorySize(uint32_t size) { this->inventorySize = static_cast<uint8_t>(size); }
    void setCurrentPage(uint32_t page) { this->currentPage = static_cast<uint8_t>(page); }
    void setCurrentIndex(uint32_t index) { this->currentIndex = static_cast<uint8_t>(index); }

    ItemIconSelector *getItemIconSelector() { return &this->itemIconSelector; }
    ValueEditor *getValueEditor() { return &this->valueEditor; }
    ErrorWindow *getErrorWindow() { return &this->errorWindow; }
    float getScale() const { return this->scale; }

    ItemId *getInventoryItemPtr() { return this->inventoryItemPtr; }
    ItemId *getMinValuePtr() { return &this->minValue; }
    ItemId *getMaxValuePtr() { return &this->maxValue; }

    uint16_t *getWaitFramesToBeginPtr() { return &this->waitFramesToBegin; }
    bool *getShouldIncrementNowPtr() { return &this->shouldIncrementNow; }

    uint32_t getInventoryType() const { return this->inventoryType; }
    uint32_t getInventorySize() const { return this->inventorySize; }
    uint32_t getCurrentPage() const { return this->currentPage; }
    uint32_t getCurrentIndex() const { return this->currentIndex; }

    uint8_t *getCurrentPagePtr() { return &this->currentPage; }
    uint8_t *getCurrentIndexPtr() { return &this->currentIndex; }

    uint32_t getTotalItemsInInventory() const;
    bool inventoryIsEmpty() const { return this->getTotalItemsInInventory() == 0; }
    bool inventoryIsFull() const;

    void initErrorWindow(const char *text);
    void duplicateItem(Menu *menuPtr);
    void deleteItem(Menu *menuPtr);

    void verifyInventoryIndexAndPage(Menu *menuPtr);
    bool checkIfItemsOnNextPage(uint32_t currentItemCount) const;
    void inventoryMenuMoveUpOnePage();
    void inventoryMenuMoveDownOnePage();

    // Called to handle the controls for navigating through the items/badges on the right of the window
    void inventoryMenuItemControls(MenuButtonInput button);

    void drawCurrentInventory();

   private:
    // Window that the inventory will be placed in. This includes all of the space in the root window, excluding the space used
    // by the main text for the options.
    Window inventoryWindow;

    ItemIconSelector itemIconSelector;
    ValueEditor valueEditor;
    ErrorWindow errorWindow;
    float scale;

    ItemId *inventoryItemPtr; // Pointer to the inventory that is currently being worked with
    ItemId minValue;
    ItemId maxValue;

    uint16_t waitFramesToBegin; // Used to figure out when values should be auto-incremented when a D-Pad direction is held
    bool shouldIncrementNow;    // Used to figure out when values should be auto-incremented when a D-Pad direction is held

    uint8_t inventoryType;      // Specifier for the inventory that is currently being worked with
    uint8_t inventorySize;      // Size of the inventory that is currently being worked with
    uint8_t currentPage;
    uint8_t currentIndex;       // Current cursor position
};

extern Inventory *gInventory;

void inventoryMenuMainInit(Menu *menuPtr); // Called when initially entering the main part of the inventory menu
void inventoryMenuMainExit();              // Called when exiting the main part of the inventory menu

void selectedOptionAddById(Menu *menuPtr);
void selectedOptionAddByIcon(Menu *menuPtr);
void selectedOptionDuplicate(Menu *menuPtr);
void selectedOptionChangeById(Menu *menuPtr);
void selectedOptionChangeByIcon(Menu *menuPtr);
void selectedOptionDelete(Menu *menuPtr);

// Called to handle the controls for the main part of the inventory menu
void inventoryMenuMainControls(Menu *menuPtr, MenuButtonInput button);

void inventoryMenuMainDraw(CameraId cameraId, void *user); // Called to handle drawing the main part of the inventory menu

// Called when initially entering the part of the inventory menu for selecting which inventory to work with
void inventoryMenuInit(Menu *menuPtr);

#endif
