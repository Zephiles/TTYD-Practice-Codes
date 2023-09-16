#ifndef MENUS_INVENTORY_MENU_H
#define MENUS_INVENTORY_MENU_H

#include "classes/menu.h"
#include "classes/errorWindow.h"
#include "classes/itemIconSelector.h"
#include "classes/valueEditor.h"
#include "classes/window.h"
#include "ttyd/item_data.h"
#include "ttyd/camdrv.h"

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
    INVENTORY_FLAG_DUPLICATE = 0,
    INVENTORY_FLAG_CHANGE_BY_ID,
    INVENTORY_FLAG_CHANGE_BY_ICON,
    INVENTORY_FLAG_SWAP_MOVE_ITEMS_INIT,
    INVENTORY_FLAG_SWAP_MOVE_ITEMS_SELECTED_ITEM_TO_SWAP_MOVE,
    INVENTORY_FLAG_DELETE,
};

class InventoryMenu
{
   public:
    InventoryMenu();
    ~InventoryMenu() {}

    void setinventoryItemPtr(ItemId *ptr) { this->inventoryItemPtr = ptr; }
    void setMinValue(ItemId value) { this->minValue = static_cast<int32_t>(value); }
    void setMaxValue(ItemId value) { this->maxValue = static_cast<int32_t>(value); }
    void setInventoryType(uint32_t type) { this->inventoryType = static_cast<uint8_t>(type); }
    void setInventorySize(uint32_t size) { this->inventorySize = static_cast<uint8_t>(size); }
    void setCurrentPage(uint32_t page) { this->currentPage = static_cast<uint8_t>(page); }
    void setCurrentIndex(uint32_t index) { this->currentIndex = static_cast<uint8_t>(index); }

    ItemIconSelector *getItemIconSelectorPtr() { return &this->itemIconSelector; }
    ValueEditor *getValueEditorPtr() { return &this->valueEditor; }
    ErrorWindow *getErrorWindowPtr() { return &this->errorWindow; }

    ItemId *getInventoryItemPtr() { return this->inventoryItemPtr; }
    int32_t *getMinValuePtr() { return &this->minValue; }
    int32_t *getMaxValuePtr() { return &this->maxValue; }

    MenuAutoIncrement *getAutoIncrementPtr() { return &this->autoIncrement; }
    uint32_t getInventoryType() const { return this->inventoryType; }
    uint32_t getInventorySize() const { return this->inventorySize; }

    uint32_t getSelectedIndex() const { return this->selectedIndex; }
    void setSelectedIndex(uint32_t index) { this->selectedIndex = static_cast<uint8_t>(index); }

    uint32_t getCurrentPage() const { return this->currentPage; }
    uint32_t getCurrentIndex() const { return this->currentIndex; }
    uint8_t *getCurrentPagePtr() { return &this->currentPage; }
    uint8_t *getCurrentIndexPtr() { return &this->currentIndex; }

    uint32_t getTotalItemsInInventory() const;
    bool inventoryIsEmpty() const { return this->getTotalItemsInInventory() == 0; }
    bool inventoryIsFull() const;

    void initErrorWindow(const char *text);
    bool initSwapMoveItems(Menu *menuPtr);

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

    ItemId *inventoryItemPtr; // Pointer to the inventory that is currently being worked with
    int32_t minValue;
    int32_t maxValue;

    MenuAutoIncrement autoIncrement;
    uint8_t inventoryType; // Specifier for the inventory that is currently being worked with
    uint8_t inventorySize; // Size of the inventory that is currently being worked with
    uint8_t selectedIndex; // For swapping/moving items/badges
    uint8_t currentPage;
    uint8_t currentIndex; // Current cursor position
};

extern InventoryMenu *gInventoryMenu;

void inventoryMenuMainInit(Menu *menuPtr); // Called when initially entering the main part of the inventory menu
void inventoryMenuMainExit();              // Called when exiting the main part of the inventory menu

void inventoryMenuSelectedOptionAddById(Menu *menuPtr);
void inventoryMenuSelectedOptionAddByIcon(Menu *menuPtr);
void inventoryMenuSelectedOptionDuplicate(Menu *menuPtr);
void inventoryMenuSelectedOptionSwapItems(Menu *menuPtr);
void inventoryMenuSelectedOptionMoveItem(Menu *menuPtr);
void inventoryMenuSelectedOptionChangeById(Menu *menuPtr);
void inventoryMenuSelectedOptionChangeByIcon(Menu *menuPtr);
void inventoryMenuSelectedOptionDelete(Menu *menuPtr);

// Called to handle the controls for the main part of the inventory menu
void inventoryMenuMainControls(Menu *menuPtr, MenuButtonInput button);

void inventoryMenuMainDraw(CameraId cameraId, void *user); // Called to handle drawing the main part of the inventory menu

// Called when initially entering the part of the inventory menu for selecting which inventory to work with
void inventoryMenuInit(Menu *menuPtr);

#endif
