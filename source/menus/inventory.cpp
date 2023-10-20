#include "menuUtils.h"
#include "drawIcon.h"
#include "drawText.h"
#include "cxx.h"
#include "classes/menu.h"
#include "classes/window.h"
#include "menus/inventoryMenu.h"
#include "menus/rootMenu.h"
#include "misc/utils.h"
#include "ttyd/mario_pouch.h"
#include "ttyd/item_data.h"
#include "ttyd/camdrv.h"
#include "ttyd/fontmgr.h"

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cinttypes>

// Called to handle the controls for the main part of the inventory menu
static void mainControls(Menu *menuPtr, MenuButtonInput button);

// Called to handle drawing the main part of the inventory menu
static void mainDraw(CameraId cameraId, void *user);

// Called when exiting the main part of the inventory menu
static void mainExit();

// Functions for the main part of the inventory menu
static void selectedOptionAddById(Menu *menuPtr);
static void selectedOptionAddByIcon(Menu *menuPtr);
static void selectedOptionDuplicate(Menu *menuPtr);
static void selectedOptionSwapItems(Menu *menuPtr);
static void selectedOptionMoveItem(Menu *menuPtr);
static void selectedOptionChangeById(Menu *menuPtr);
static void selectedOptionChangeByIcon(Menu *menuPtr);
static void selectedOptionDelete(Menu *menuPtr);

InventoryMenu *gInventoryMenu = nullptr;

static const MenuOption gMainOptions[] = {
    "Add By Id",
    selectedOptionAddById,

    "Add By Icon",
    selectedOptionAddByIcon,

    "Duplicate",
    selectedOptionDuplicate,

    "Swap Items",
    selectedOptionSwapItems,

    "Move Item",
    selectedOptionMoveItem,

    "Change By Id",
    selectedOptionChangeById,

    "Change By Icon",
    selectedOptionChangeByIcon,

    "Delete",
    selectedOptionDelete,
};

static const MenuFunctions gMainFuncs = {
    gMainOptions,
    mainControls,
    mainDraw,
    mainExit,
};

static const char *gInventoryIsEmpty = "The inventory is currently empty.";
static const char *gInventoryIsFull = "The inventory is currently full.";
static const char *gNotEnoughItemsToSwapMove = "There needs to be at least two items/\nbadges to be able to swap/move them.";

static void cancelAddItemFromId()
{
    gInventoryMenu->getValueEditorPtr()->stopDrawing();
}

static void addItemFromId(const ValueType *valuePtr)
{
    InventoryMenu *inventoryMenuPtr = gInventoryMenu;

    // Make sure the inventory isn't full
    uint32_t totalItems = inventoryMenuPtr->getTotalItemsInInventory();
    if (totalItems >= inventoryMenuPtr->getInventorySize())
    {
        cancelAddItemFromId();
        return;
    }

    // Set the new item
    const ItemId item = static_cast<ItemId>(valuePtr->s32);
    inventoryMenuPtr->getInventoryItemPtr()[totalItems] = item;

    // The battle menu will not properly update when an upgrade is given, so manually update it
    recheckBattleUpgrades(item);

    // The Important Items in the pause menu will not visually update when an important item is added, so manually reset the
    // Important Items menu
    resetPauseMenuImportantItems();

    // If the inventory is now full, then stop adding items
    if (inventoryMenuPtr->inventoryIsFull())
    {
        cancelAddItemFromId();
    }
}

static void selectedOptionAddById(Menu *menuPtr)
{
    (void)menuPtr;

    // If the inventory is full, then show an error message
    InventoryMenu *inventoryMenuPtr = gInventoryMenu;
    if (inventoryMenuPtr->inventoryIsFull())
    {
        inventoryMenuPtr->initErrorWindow(gInventoryIsFull);
        return;
    }

    // Initialize the value editor
    ValueEditor *valueEditorPtr = inventoryMenuPtr->getValueEditorPtr();

    uint32_t flags = 0;
    // flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_UP_DOWN);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Y_SET_MAX);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Z_SET_MIN);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_ITEM_ICON_AND_TEXT);

    const int32_t *minValuePtr = inventoryMenuPtr->getMinValuePtr();
    const int32_t *maxValuePtr = inventoryMenuPtr->getMaxValuePtr();
    const Window *rootWindowPtr = gRootWindow;

    valueEditorPtr
        ->init(minValuePtr, minValuePtr, maxValuePtr, rootWindowPtr, flags, VariableType::s16, rootWindowPtr->getAlpha());

    valueEditorPtr->startDrawing(addItemFromId, cancelAddItemFromId);
}

static void cancelAddItemFromIcon()
{
    gInventoryMenu->getItemIconSelectorPtr()->stopDrawing();
}

static void addItemFromIcon(ItemId item)
{
    InventoryMenu *inventoryMenuPtr = gInventoryMenu;

    // Make sure the inventory isn't full
    uint32_t totalItems = inventoryMenuPtr->getTotalItemsInInventory();
    if (totalItems >= inventoryMenuPtr->getInventorySize())
    {
        cancelAddItemFromIcon();
        return;
    }

    // Add the new item
    inventoryMenuPtr->getInventoryItemPtr()[totalItems] = item;

    // The battle menu will not properly update when an upgrade is given, so manually update it
    recheckBattleUpgrades(item);

    // The Important Items in the pause menu will not visually update when an important item is added, so manually reset the
    // Important Items menu
    resetPauseMenuImportantItems();

    // If the inventory is now full, then stop adding items
    if (inventoryMenuPtr->inventoryIsFull())
    {
        cancelAddItemFromIcon();
    }
}

static void selectedOptionAddByIcon(Menu *menuPtr)
{
    (void)menuPtr;

    // If the inventory is full, then show an error message
    InventoryMenu *inventoryMenuPtr = gInventoryMenu;
    if (inventoryMenuPtr->inventoryIsFull())
    {
        inventoryMenuPtr->initErrorWindow(gInventoryIsFull);
        return;
    }

    // Initialize the item icon selector
    const Window *rootWindowPtr = gRootWindow;
    ItemIconSelector *itemIconSelectorPtr = inventoryMenuPtr->getItemIconSelectorPtr();

    const ItemId startingItem = static_cast<ItemId>(*inventoryMenuPtr->getMinValuePtr());
    const ItemId endingItem = static_cast<ItemId>(*inventoryMenuPtr->getMaxValuePtr());

    itemIconSelectorPtr->init(rootWindowPtr, startingItem, endingItem, rootWindowPtr->getAlpha());
    itemIconSelectorPtr->startDrawing(addItemFromIcon, cancelAddItemFromIcon);
}

static void selectedOptionDuplicate(Menu *menuPtr)
{
    // If the inventory is empty, then show an error message
    InventoryMenu *inventoryMenuPtr = gInventoryMenu;
    if (inventoryMenuPtr->inventoryIsEmpty())
    {
        menuPtr->clearFlag(InventoryFlag::INVENTORY_FLAG_DUPLICATE);
        inventoryMenuPtr->initErrorWindow(gInventoryIsEmpty);
        return;
    }

    // If the inventory is full, then show an error message
    if (inventoryMenuPtr->inventoryIsFull())
    {
        menuPtr->clearFlag(InventoryFlag::INVENTORY_FLAG_DUPLICATE);
        inventoryMenuPtr->initErrorWindow(gInventoryIsFull);
        return;
    }

    // If the Duplicate option was just selected, then allow the player to choose an item/badge to duplicate
    if (!menuPtr->flagIsSet(InventoryFlag::INVENTORY_FLAG_DUPLICATE))
    {
        menuPtr->setFlag(InventoryFlag::INVENTORY_FLAG_DUPLICATE);

        // Reset the current index to the top of the current page
        inventoryMenuPtr->setCurrentIndex(inventoryMenuPtr->getCurrentPage() * INVENTORY_ITEMS_PER_PAGE);
        return;
    }

    // Duplicate the selected item/badge
    // Make sure the current index is valid
    const uint32_t index = inventoryMenuPtr->getCurrentIndex();
    if (index >= inventoryMenuPtr->getInventorySize())
    {
        menuPtr->clearFlag(InventoryFlag::INVENTORY_FLAG_DUPLICATE);
        return;
    }

    // Get the item to be duplicated
    ItemId *inventoryItemPtr = inventoryMenuPtr->getInventoryItemPtr();
    const ItemId item = inventoryItemPtr[index];

    // Make sure the item is valid
    if (!itemIsValid(item))
    {
        return;
    }

    // Add the item to the next empty slot in the array
    inventoryItemPtr[inventoryMenuPtr->getTotalItemsInInventory()] = item;

    // The Important Items in the pause menu will not visually update when an important item is added, so manually reset the
    // Important Items menu
    resetPauseMenuImportantItems();

    // If the inventory is now full, then stop duplicating items
    if (inventoryMenuPtr->inventoryIsFull())
    {
        menuPtr->clearFlag(InventoryFlag::INVENTORY_FLAG_DUPLICATE);
    }
}

bool InventoryMenu::initSwapMoveItems(Menu *menuPtr)
{
    // If there are not at least two items/badges, then show an error message
    const uint32_t totalItems = this->getTotalItemsInInventory();
    if (totalItems < 2)
    {
        menuPtr->clearFlag(InventoryFlag::INVENTORY_FLAG_SWAP_MOVE_ITEMS_INIT);
        menuPtr->clearFlag(InventoryFlag::INVENTORY_FLAG_SWAP_MOVE_ITEMS_SELECTED_ITEM_TO_SWAP_MOVE);
        this->initErrorWindow(gNotEnoughItemsToSwapMove);
        return false;
    }

    // Make sure the current index is valid
    const uint32_t currentIndex = this->currentIndex;
    if (currentIndex >= totalItems)
    {
        menuPtr->clearFlag(InventoryFlag::INVENTORY_FLAG_SWAP_MOVE_ITEMS_INIT);
        menuPtr->clearFlag(InventoryFlag::INVENTORY_FLAG_SWAP_MOVE_ITEMS_SELECTED_ITEM_TO_SWAP_MOVE);
        return false;
    }

    // Make sure the selected item/badge is valid
    if (!itemIsValid(this->inventoryItemPtr[currentIndex]))
    {
        return false;
    }

    // If the Swap/Move option was just selected, then allow the player to choose an item/badge to swap/move
    if (!menuPtr->flagIsSet(InventoryFlag::INVENTORY_FLAG_SWAP_MOVE_ITEMS_INIT))
    {
        menuPtr->setFlag(InventoryFlag::INVENTORY_FLAG_SWAP_MOVE_ITEMS_INIT);

        // Reset the current index to the top of the current page
        this->setCurrentIndex(this->currentPage * INVENTORY_ITEMS_PER_PAGE);
        return false;
    }

    // If an item/badge hasn't been selected yet, then select the current one to be swapped/moved
    if (!menuPtr->flagIsSet(InventoryFlag::INVENTORY_FLAG_SWAP_MOVE_ITEMS_SELECTED_ITEM_TO_SWAP_MOVE))
    {
        menuPtr->setFlag(InventoryFlag::INVENTORY_FLAG_SWAP_MOVE_ITEMS_SELECTED_ITEM_TO_SWAP_MOVE);
        this->setSelectedIndex(currentIndex);
        return false;
    }

    // Make sure the selected item/badge is not the current item/badge
    if (this->selectedIndex == currentIndex)
    {
        return false;
    }

    return true;
}

static void swapItemPositions(uint32_t currentIndex, uint32_t selectedIndex)
{
    ItemId *inventoryItemPtr = gInventoryMenu->getInventoryItemPtr();

    // Backup the selected item/badge
    const ItemId selectedItemBackup = inventoryItemPtr[selectedIndex];

    // Move the current item to the selected item/badge
    inventoryItemPtr[selectedIndex] = inventoryItemPtr[currentIndex];

    // Move the selected item to the current item/badge
    inventoryItemPtr[currentIndex] = selectedItemBackup;
}

static void selectedOptionSwapItems(Menu *menuPtr)
{
    InventoryMenu *inventoryMenuPtr = gInventoryMenu;
    if (!inventoryMenuPtr->initSwapMoveItems(menuPtr))
    {
        return;
    }

    // Swap the selected items/badges
    swapItemPositions(inventoryMenuPtr->getCurrentIndex(), inventoryMenuPtr->getSelectedIndex());

    // Go back to selecting the first item/badge
    menuPtr->clearFlag(InventoryFlag::INVENTORY_FLAG_SWAP_MOVE_ITEMS_SELECTED_ITEM_TO_SWAP_MOVE);
}

static void selectedOptionMoveItem(Menu *menuPtr)
{
    InventoryMenu *inventoryMenuPtr = gInventoryMenu;
    if (!inventoryMenuPtr->initSwapMoveItems(menuPtr))
    {
        return;
    }

    // Move the selected item/badge to the new location
    // If the items/badges are right next to each other, then just swap them
    const uint32_t currentIndex = inventoryMenuPtr->getCurrentIndex();
    const uint32_t selectedIndex = inventoryMenuPtr->getSelectedIndex();
    const bool movingItemDown = currentIndex > selectedIndex;
    int32_t indexDistance;

    if (movingItemDown)
    {
        indexDistance = currentIndex - selectedIndex;
    }
    else
    {
        indexDistance = selectedIndex - currentIndex;
    }

    if (indexDistance < 2)
    {
        swapItemPositions(currentIndex, selectedIndex);
    }
    else
    {
        // Backup the selected item/badge
        ItemId *inventoryItemPtr = inventoryMenuPtr->getInventoryItemPtr();
        const ItemId selectedItemBackup = inventoryItemPtr[selectedIndex];

        if (movingItemDown)
        {
            // Move the items/badges from the current item/badge to the selected item/badge up by one
            ItemId *selectedItemPtr = &inventoryItemPtr[selectedIndex];
            memmove(selectedItemPtr, &selectedItemPtr[1], sizeof(ItemId) * indexDistance);
        }
        else
        {
            // Move the items/badges from the current item/badge to the selected item/badge down by one
            ItemId *currentItemPtr = &inventoryItemPtr[currentIndex];
            memmove(&currentItemPtr[1], currentItemPtr, sizeof(ItemId) * indexDistance);
        }

        // Move the selected item/badge to the current item/badge
        inventoryItemPtr[currentIndex] = selectedItemBackup;
    }

    // Go back to selecting an item/badge to move
    menuPtr->clearFlag(InventoryFlag::INVENTORY_FLAG_SWAP_MOVE_ITEMS_SELECTED_ITEM_TO_SWAP_MOVE);
}

static void changeItemFromId(const ValueType *valuePtr)
{
    InventoryMenu *inventoryMenuPtr = gInventoryMenu;

    // Make sure the current index is valid
    const uint32_t index = inventoryMenuPtr->getCurrentIndex();
    if (index >= inventoryMenuPtr->getInventorySize())
    {
        cancelAddItemFromId();
        return;
    }

    // Set the new item
    const ItemId item = static_cast<ItemId>(valuePtr->s32);
    inventoryMenuPtr->getInventoryItemPtr()[index] = item;

    // The battle menu will not properly update when an upgrade is changed, so manually update it
    recheckBattleUpgrades(item);

    // The Important Items in the pause menu will not visually update when an important item is changed, so manually reset the
    // Important Items menu
    resetPauseMenuImportantItems();

    // Close the value editor
    cancelAddItemFromId();
}

static void selectedOptionChangeById(Menu *menuPtr)
{
    // If the inventory is empty, then show an error message
    InventoryMenu *inventoryMenuPtr = gInventoryMenu;
    if (inventoryMenuPtr->inventoryIsEmpty())
    {
        menuPtr->clearFlag(InventoryFlag::INVENTORY_FLAG_CHANGE_BY_ID);
        inventoryMenuPtr->initErrorWindow(gInventoryIsEmpty);
        return;
    }

    // If the Change By Id option was just selected, then allow the player to choose an item/badge to change
    if (!menuPtr->flagIsSet(InventoryFlag::INVENTORY_FLAG_CHANGE_BY_ID))
    {
        menuPtr->setFlag(InventoryFlag::INVENTORY_FLAG_CHANGE_BY_ID);

        // Reset the current index to the top of the current page
        inventoryMenuPtr->setCurrentIndex(inventoryMenuPtr->getCurrentPage() * INVENTORY_ITEMS_PER_PAGE);
        return;
    }

    // Make sure the current index is valid
    uint32_t index = inventoryMenuPtr->getCurrentIndex();
    if (index >= inventoryMenuPtr->getInventorySize())
    {
        menuPtr->clearFlag(InventoryFlag::INVENTORY_FLAG_CHANGE_BY_ID);
        return;
    }

    // Set the cursor of the item icon selector to the item that is being changed
    int32_t item = static_cast<int32_t>(inventoryMenuPtr->getInventoryItemPtr()[index]);

    // Make sure the item is valid
    const int32_t *minValuePtr = inventoryMenuPtr->getMinValuePtr();
    if (!itemIsValid(static_cast<ItemId>(item)))
    {
        item = *minValuePtr;
    }

    // Initialize the value editor
    ValueEditor *valueEditorPtr = inventoryMenuPtr->getValueEditorPtr();

    uint32_t flags = 0;
    // flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_UP_DOWN);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Y_SET_MAX);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Z_SET_MIN);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_ITEM_ICON_AND_TEXT);

    const int32_t *maxValuePtr = inventoryMenuPtr->getMaxValuePtr();
    const Window *rootWindowPtr = gRootWindow;

    valueEditorPtr->init(&item, minValuePtr, maxValuePtr, rootWindowPtr, flags, VariableType::s16, rootWindowPtr->getAlpha());
    valueEditorPtr->startDrawing(changeItemFromId, cancelAddItemFromId);
}

static void changeItemFromIcon(ItemId item)
{
    InventoryMenu *inventoryMenuPtr = gInventoryMenu;

    // Make sure the current index is valid
    const uint32_t index = inventoryMenuPtr->getCurrentIndex();
    if (index >= inventoryMenuPtr->getInventorySize())
    {
        cancelAddItemFromIcon();
        return;
    }

    // Set the new item
    inventoryMenuPtr->getInventoryItemPtr()[index] = item;

    // The battle menu will not properly update when an upgrade is changed, so manually update it
    recheckBattleUpgrades(item);

    // The Important Items in the pause menu will not visually update when an important item is changed, so manually reset the
    // Important Items menu
    resetPauseMenuImportantItems();

    // Close the item icon selector
    cancelAddItemFromIcon();
}

static void selectedOptionChangeByIcon(Menu *menuPtr)
{
    // If the inventory is empty, then show an error message
    InventoryMenu *inventoryMenuPtr = gInventoryMenu;
    if (inventoryMenuPtr->inventoryIsEmpty())
    {
        menuPtr->clearFlag(InventoryFlag::INVENTORY_FLAG_CHANGE_BY_ICON);
        inventoryMenuPtr->initErrorWindow(gInventoryIsEmpty);
        return;
    }

    // If the Change By Icon option was just selected, then allow the player to choose an item/badge to change
    if (!menuPtr->flagIsSet(InventoryFlag::INVENTORY_FLAG_CHANGE_BY_ICON))
    {
        menuPtr->setFlag(InventoryFlag::INVENTORY_FLAG_CHANGE_BY_ICON);

        // Reset the current index to the top of the current page
        inventoryMenuPtr->setCurrentIndex(inventoryMenuPtr->getCurrentPage() * INVENTORY_ITEMS_PER_PAGE);
        return;
    }

    // Make sure the current index is valid
    uint32_t index = inventoryMenuPtr->getCurrentIndex();
    if (index >= inventoryMenuPtr->getInventorySize())
    {
        menuPtr->clearFlag(InventoryFlag::INVENTORY_FLAG_CHANGE_BY_ICON);
        return;
    }

    // Initialize the item icon selector
    const Window *rootWindowPtr = gRootWindow;
    ItemIconSelector *itemIconSelectorPtr = inventoryMenuPtr->getItemIconSelectorPtr();

    const ItemId startingItem = static_cast<ItemId>(*inventoryMenuPtr->getMinValuePtr());
    const ItemId endingItem = static_cast<ItemId>(*inventoryMenuPtr->getMaxValuePtr());

    itemIconSelectorPtr->init(rootWindowPtr, startingItem, endingItem, rootWindowPtr->getAlpha());

    // Set the cursor of the item icon selector to the item that is being changed
    ItemId item = inventoryMenuPtr->getInventoryItemPtr()[index];

    // Make sure the item is valid
    if (!itemIsValid(item))
    {
        item = startingItem;
    }

    itemIconSelectorPtr->setCurrentIndexFromItem(item);
    itemIconSelectorPtr->startDrawing(changeItemFromIcon, cancelAddItemFromIcon);
}

static void selectedOptionDelete(Menu *menuPtr)
{
    // If the inventory is empty, then show an error message
    InventoryMenu *inventoryMenuPtr = gInventoryMenu;
    if (inventoryMenuPtr->inventoryIsEmpty())
    {
        menuPtr->clearFlag(InventoryFlag::INVENTORY_FLAG_DELETE);
        inventoryMenuPtr->initErrorWindow(gInventoryIsEmpty);
        return;
    }

    // If the Delete option was just selected, then allow the player to choose an item/badge to delete
    if (!menuPtr->flagIsSet(InventoryFlag::INVENTORY_FLAG_DELETE))
    {
        menuPtr->setFlag(InventoryFlag::INVENTORY_FLAG_DELETE);

        // Reset the current index to the top of the current page
        inventoryMenuPtr->setCurrentIndex(inventoryMenuPtr->getCurrentPage() * INVENTORY_ITEMS_PER_PAGE);
        return;
    }

    // Remove the selected item/badge
    // Make sure the current index is valid
    uint32_t index = inventoryMenuPtr->getCurrentIndex();
    const uint32_t inventorySize = inventoryMenuPtr->getInventorySize();

    if (index >= inventorySize)
    {
        menuPtr->clearFlag(InventoryFlag::INVENTORY_FLAG_DELETE);
        return;
    }

    // Make a backup of the current item being deleted
    ItemId *inventoryItemPtr = inventoryMenuPtr->getInventoryItemPtr();
    const ItemId deletedItem = inventoryItemPtr[index];

    // Move the array back one slot
    const uint32_t remainingSize = inventorySize - index - 1;
    memcpy(&inventoryItemPtr[index], &inventoryItemPtr[index + 1], remainingSize * sizeof(ItemId));

    // Clear the last slot to make sure that the last item/badge in the array does not get duplicated
    inventoryItemPtr[inventorySize - 1] = ItemId::ITEM_NONE;

    // If the current index was at the last valid index, then it needs to be moved back one
    const uint32_t totalItems = inventoryMenuPtr->getTotalItemsInInventory();
    if (index >= totalItems)
    {
        inventoryMenuPtr->setCurrentIndex(--index);

        // Check if moving back one should be placed on the previous page
        const uint32_t currentPage = inventoryMenuPtr->getCurrentPage();
        if (index < (currentPage * INVENTORY_ITEMS_PER_PAGE))
        {
            inventoryMenuPtr->setCurrentPage(currentPage - 1);
        }
    }

    // The battle menu will not properly update if an upgrade is removed, so manually update it
    recheckBattleUpgrades(deletedItem);

    // The Important Items in the pause menu will not visually update when an important item is removed, so manually reset the
    // Important Items menu
    resetPauseMenuImportantItems();

    // If the inventory is now empty, then stop deleting items
    if (totalItems == 0)
    {
        menuPtr->clearFlag(InventoryFlag::INVENTORY_FLAG_DELETE);
    }
}

static void mainControls(Menu *menuPtr, MenuButtonInput button)
{
    // If a file is loaded while this menu is open, then the inventory will change, so make sure no issues occur because of this
    InventoryMenu *inventoryMenuPtr = gInventoryMenu;
    inventoryMenuPtr->verifyInventoryIndexAndPage(menuPtr);

    // If a separate window is currently open for selecting an item/badge, then only call the control functions for those
    ItemIconSelector *itemIconSelectorPtr = inventoryMenuPtr->getItemIconSelectorPtr();

    if (itemIconSelectorPtr->shouldDraw())
    {
        itemIconSelectorPtr->controls(button);
        return;
    }

    ValueEditor *valueEditorPtr = inventoryMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->controls(button);
        return;
    }

    // If no flags are set, then use the default controls
    if (!menuPtr->anyFlagIsSet())
    {
        basicMenuLayoutControls(menuPtr, button);
        return;
    }

    // The function for checking for auto-incrementing needs to run every frame to be handled correctly
    const bool autoIncrement = handleMenuAutoIncrement(inventoryMenuPtr->getAutoIncrementPtr());

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
                inventoryMenuPtr->inventoryMenuItemControls(buttonHeld);
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
            inventoryMenuPtr->inventoryMenuItemControls(button);
            break;
        }
        case MenuButtonInput::R:
        {
            inventoryMenuPtr->inventoryMenuMoveDownOnePage();
            break;
        }
        case MenuButtonInput::L:
        {
            inventoryMenuPtr->inventoryMenuMoveUpOnePage();
            break;
        }
        case MenuButtonInput::A:
        {
            menuPtr->runSelectedOptionFunc();
            break;
        }
        case MenuButtonInput::B:
        {
            // If swapping/moving items/badges and have selected an item/badge to swap/move, then go back to selecting an
            // item/badge
            if (menuPtr->flagIsSet(InventoryFlag::INVENTORY_FLAG_SWAP_MOVE_ITEMS_SELECTED_ITEM_TO_SWAP_MOVE))
            {
                menuPtr->clearFlag(InventoryFlag::INVENTORY_FLAG_SWAP_MOVE_ITEMS_SELECTED_ITEM_TO_SWAP_MOVE);
            }
            else
            {
                menuPtr->clearAllFlags();
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

void InventoryMenu::drawCurrentInventory()
{
    /*
    // Draw the inventory window for debugging purposes
    Window *inventoryWindow = &this->inventoryWindow;
    inventoryWindow->setColor(0x505050FF);
    inventoryWindow->draw();
    */

    // To avoid a lot of unnecessary extra GX calls, draw the text first, as text drawing should already be initialized
    // Figure out how many items/badges the player currently has out of the max
    const int32_t currentItemCount = static_cast<int32_t>(this->getTotalItemsInInventory());
    const int32_t totalItems = this->inventorySize;

    // Draw the text for how many items/badges the player currently has out of the max
    // Draw the counts as ints, to prevent long text if they somehow become negative
    char buf[32];
    snprintf(buf, sizeof(buf), "%" PRId32 "/%" PRId32, currentItemCount, totalItems);

    const Window *inventoryWindow = &this->inventoryWindow;
    constexpr float scale = MENU_SCALE;

    float itemCountPosX;
    float itemCountPosY;
    inventoryWindow->getTextPosXY(buf, WindowAlignment::TOP_CENTER, scale, &itemCountPosX, &itemCountPosY);
    drawText(buf, itemCountPosX, itemCountPosY, scale, getColorWhite(0xFF));

    // Start drawing the items/badges with their icons
    const float padding = inventoryWindow->getPadding() * scale;
    float tempItemPosXBase;
    float tempItemPosYBase;
    inventoryWindow->getIconPosXY(WindowAlignment::TOP_LEFT, scale, &tempItemPosXBase, &tempItemPosYBase);

    const float itemPosXBase = tempItemPosXBase - padding;
    const float itemPosYBase = tempItemPosYBase - (LINE_HEIGHT_FLOAT * scale) + LINE_HEIGHT_ADJUSTMENT_5(scale) - padding;

    constexpr float textWidth = 140.f;
    constexpr float widthAdjustment = INVENTORY_WIDTH_ADJUSTMENT(scale);
    constexpr float itemPosXIncrement = textWidth + (ICON_SIZE_FLOAT * scale) + widthAdjustment;
    constexpr float itemLineDecrement = SPACE_USED_PER_ICON(scale);

    float textPosXBase;
    float textPosYBase;
    getTextPosXYByIcon(itemPosXBase, itemPosYBase, scale, &textPosXBase, &textPosYBase);

    float textPosX = textPosXBase;
    float textPosY = textPosYBase;

    const Menu *menuPtr = gMenu;
    const uint32_t inventoryType = this->inventoryType;
    const bool drawVertical = (inventoryType == InventoryType::BADGES) || (inventoryType == InventoryType::STORED);

    const bool anyFlagIsSet = menuPtr->anyFlagIsSet();

    const bool swappingOrMovingItemsSelectedItem =
        menuPtr->flagIsSet(InventoryFlag::INVENTORY_FLAG_SWAP_MOVE_ITEMS_SELECTED_ITEM_TO_SWAP_MOVE);

    const ItemId *inventoryItemPtr = this->inventoryItemPtr;

    const uint32_t currentPage = this->currentPage;
    const uint32_t currentIndex = this->currentIndex;
    const uint32_t selectedIndex = this->selectedIndex;
    const uint32_t startingIndex = INVENTORY_ITEMS_PER_PAGE * currentPage;
    const uint32_t endingIndex = startingIndex + INVENTORY_ITEMS_PER_PAGE;

    uint32_t counter = 0;
    float iconScale;

    // Draw the item/badge texts
    for (uint32_t i = startingIndex; i < endingIndex; i++, counter++)
    {
        if (static_cast<int32_t>(i) >= currentItemCount)
        {
            // Reached the end of the array, so exit
            break;
        }

        if (drawVertical)
        {
            if (counter >= INVENTORY_ITEMS_PER_COLUMN)
            {
                counter = 0;

                // Move to the next column
                textPosX += itemPosXIncrement;
                textPosY = textPosYBase;
            }
        }
        else
        {
            if (counter >= INVENTORY_ITEMS_PER_ROW)
            {
                counter = 0;

                // Move to the next row
                textPosX = textPosXBase;
                textPosY -= itemLineDecrement;
            }
        }

        const ItemId currentItem = inventoryItemPtr[i];
        const bool currentItemIsValid = itemIsValid(currentItem);
        uint32_t textColor = getColorWhite(0xFF);

        if (anyFlagIsSet)
        {
            if (currentIndex == i)
            {
                textColor = getColorBlue(0xFF);
            }
            else if (swappingOrMovingItemsSelectedItem && (selectedIndex == i))
            {
                textColor = getColorGreen(0xFF);
            }
            else if (!currentItemIsValid)
            {
                textColor = getColorRed(0xFF);
            }
        }
        else if (!currentItemIsValid)
        {
            textColor = getColorRed(0xFF);
        }

        const char *itemName = getItemName(currentItem);
        drawText(itemName, textPosX, textPosY, scale, textWidth, textColor);

        if (drawVertical)
        {
            textPosY -= itemLineDecrement;
        }
        else
        {
            textPosX += itemPosXIncrement;
        }
    }

    // Draw the item/badge icons
    float itemPosX = itemPosXBase;
    float itemPosY = itemPosYBase;
    counter = 0;

    for (uint32_t i = startingIndex; i < endingIndex; i++, counter++)
    {
        if (static_cast<int32_t>(i) >= currentItemCount)
        {
            // Reached the end of the array, so exit
            break;
        }

        if (drawVertical)
        {
            if (counter >= INVENTORY_ITEMS_PER_COLUMN)
            {
                counter = 0;

                // Move to the next column
                itemPosX += itemPosXIncrement;
                itemPosY = itemPosYBase;
            }
        }
        else
        {
            if (counter >= INVENTORY_ITEMS_PER_ROW)
            {
                counter = 0;

                // Move to the next row
                itemPosX = itemPosXBase;
                itemPosY -= itemLineDecrement;
            }
        }

        const ItemId currentItem = inventoryItemPtr[i];

        // Several items need to have their icon scales adjusted
        iconScale = adjustItemIconScale(currentItem, scale);

        drawIcon(itemPosX, itemPosY, iconScale, getIconFromItem(currentItem));

        if (drawVertical)
        {
            itemPosY -= itemLineDecrement;
        }
        else
        {
            itemPosX += itemPosXIncrement;
        }
    }

    // Draw the icons for the L and R buttons if applicable
    // The standard inventory will only have one page, so neither are required
    if (inventoryType != InventoryType::STANDARD)
    {
        const float iconLRPosX = inventoryWindow->getIconPosX(WindowAlignment::TOP_RIGHT, scale);
        constexpr float iconLRScale = scale - 0.1f;

        // Draw the L button if currently not on the first page
        if (currentPage > 0)
        {
            const float iconLPosY = inventoryWindow->getIconPosY(WindowAlignment::TOP_RIGHT, scale);
            drawIcon(iconLRPosX, iconLPosY, iconLRScale, IconId::ICON_BUTTON_L_UNPRESSED);
        }

        // Draw the R button if more items are on the next page
        if (this->checkIfItemsOnNextPage(static_cast<uint32_t>(currentItemCount)))
        {
            const float iconRPosY = inventoryWindow->getIconPosY(WindowAlignment::BOTTOM_RIGHT, scale);
            drawIcon(iconLRPosX, iconRPosY, iconLRScale, IconId::ICON_BUTTON_R_UNPRESSED);
        }
    }
}

static void mainDraw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDraw(cameraId, user);

    // If a file is loaded while this menu is open, then the inventory will change, so make sure no issues occur because of this
    InventoryMenu *inventoryMenuPtr = gInventoryMenu;
    inventoryMenuPtr->verifyInventoryIndexAndPage(gMenu);

    // Draw the current inventory
    inventoryMenuPtr->drawCurrentInventory();

    // Draw the value editor if applicable
    ValueEditor *valueEditorPtr = inventoryMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->draw();
    }

    // Draw the item icon selector if applicable
    ItemIconSelector *itemIconSelector = inventoryMenuPtr->getItemIconSelectorPtr();
    if (itemIconSelector->shouldDraw())
    {
        itemIconSelector->draw();
    }

    // Draw an error message if applicable
    ErrorWindow *errorWindowPtr = inventoryMenuPtr->getErrorWindowPtr();
    if (errorWindowPtr->shouldDraw())
    {
        errorWindowPtr->draw();
    }
}

void InventoryMenu::verifyInventoryIndexAndPage(Menu *menuPtr)
{
    // Verify that the current page is correct
    uint32_t totalItems = this->getTotalItemsInInventory();
    if (totalItems == 0)
    {
        // Set totalItems to 1 to allow the following checks to work correctly
        totalItems = 1;
    }

    const uint32_t totalPages = intCeil(totalItems, INVENTORY_ITEMS_PER_PAGE);
    bool foundIssue = false;

    if (this->currentPage >= totalPages)
    {
        // Reset to the first page
        this->setCurrentPage(0);
        foundIssue = true;
    }

    // Verify that the current index is correct
    if (this->currentIndex >= totalItems)
    {
        // Reset to the first item of the current page
        this->setCurrentIndex(this->currentPage * INVENTORY_ITEMS_PER_PAGE);
        foundIssue = true;
    }

    if (foundIssue)
    {
        // Clear all flags to be safe
        menuPtr->clearAllFlags();

        // Make sure the windows for selecting an item/badge are closed
        cancelAddItemFromId();
        cancelAddItemFromIcon();
    }
}

bool InventoryMenu::checkIfItemsOnNextPage(uint32_t currentItemCount) const
{
    if (currentItemCount == 0)
    {
        return false;
    }

    return (static_cast<uint32_t>((this->currentPage + 1) * INVENTORY_ITEMS_PER_PAGE)) <= (currentItemCount - 1);
}

void InventoryMenu::inventoryMenuMoveUpOnePage()
{
    // If currently on the first page, then do nothing
    uint32_t currentPage = this->currentPage;
    if (currentPage == 0)
    {
        return;
    }

    // Go to the previous page
    this->setCurrentPage(--currentPage);

    // Set currentIndex to the first option on the current page
    this->setCurrentIndex(currentPage * INVENTORY_ITEMS_PER_PAGE);
}

void InventoryMenu::inventoryMenuMoveDownOnePage()
{
    // If there are no items on the next page, then do nothing
    if (!this->checkIfItemsOnNextPage(this->getTotalItemsInInventory()))
    {
        return;
    }

    // Go to the next page
    uint32_t currentPage = this->currentPage + 1;
    this->setCurrentPage(currentPage);

    // Set currentIndex to the first option on the current page
    this->setCurrentIndex(currentPage * INVENTORY_ITEMS_PER_PAGE);
}

void InventoryMenu::inventoryMenuItemControls(MenuButtonInput button)
{
    uint32_t totalOptions = this->getTotalItemsInInventory();
    uint8_t *currentIndexPtr = &this->currentIndex;
    uint8_t *currentPagePtr = nullptr;

    const uint32_t inventoryType = this->inventoryType;
    if (inventoryType != InventoryType::STANDARD)
    {
        // Check if there are multiple pages
        if (totalOptions > INVENTORY_ITEMS_PER_PAGE)
        {
            currentPagePtr = &this->currentPage;
        }
    }

    switch (inventoryType)
    {
        case InventoryType::STANDARD:
        case InventoryType::IMPORTANT:
        {
            menuControlsHorizontal(button,
                                   currentIndexPtr,
                                   currentPagePtr,
                                   totalOptions,
                                   INVENTORY_ITEMS_PER_PAGE,
                                   INVENTORY_ITEMS_PER_ROW,
                                   true);
            break;
        }
        default:
        {
            menuControlsVertical(button,
                                 currentIndexPtr,
                                 currentPagePtr,
                                 totalOptions,
                                 INVENTORY_ITEMS_PER_PAGE,
                                 INVENTORY_ITEMS_PER_ROW,
                                 true);
            break;
        }
    }
}

InventoryMenu::InventoryMenu()
{
    const Window *rootWindowPtr = gRootWindow;
    ErrorWindow *errorWindowPtr = &this->errorWindow;

    errorWindowPtr->setAlpha(rootWindowPtr->getAlpha());

    // Place the inventory window inside of the root window
    Window *inventoryWindowPtr = &this->inventoryWindow;
    inventoryWindowPtr->copyWindow(rootWindowPtr);

    // Get the width that is being used by the main text options
    constexpr float scale = MENU_SCALE;
    const float textWidth = getTextWidth("Change By Icon", scale);
    const float mainTextWidth = textWidth + (inventoryWindowPtr->getPadding() * scale);

    // Adjust the window to exclude the space used by the main text options, plus a bit extra
    constexpr float widthAdjustment = INVENTORY_WIDTH_ADJUSTMENT(scale);
    inventoryWindowPtr->setPosX(inventoryWindowPtr->getPosX() + mainTextWidth + widthAdjustment);
    inventoryWindowPtr->setWidth(inventoryWindowPtr->getWidth() - mainTextWidth - widthAdjustment);
}

void InventoryMenu::initErrorWindow(const char *text)
{
    ErrorWindow *errorWindowPtr = &this->errorWindow;
    errorWindowPtr->setText(text);
    errorWindowPtr->setTimer(3000);
    errorWindowPtr->placeInWindow(gRootWindow, WindowAlignment::MIDDLE_CENTER);
}

uint32_t InventoryMenu::getTotalItemsInInventory() const
{
    const uint32_t totalItems = this->inventorySize;
    const ItemId *inventoryItemPtr = this->inventoryItemPtr;
    uint32_t currentItemCount = 0;

    for (uint32_t i = 0; i < totalItems; i++, currentItemCount++)
    {
        if (inventoryItemPtr[i] == ItemId::ITEM_NONE)
        {
            break;
        }
    }

    return currentItemCount;
}

bool InventoryMenu::inventoryIsFull() const
{
    return this->getTotalItemsInInventory() == this->inventorySize;
}

// Called when initially entering the main part of the inventory menu
static void mainInit(Menu *menuPtr)
{
    // Failsafe: Make sure memory isn't already allocated for gInventoryMenu
    InventoryMenu *inventoryMenuPtr = gInventoryMenu;
    if (inventoryMenuPtr)
    {
        delete inventoryMenuPtr;
    }

    inventoryMenuPtr = new InventoryMenu;
    gInventoryMenu = inventoryMenuPtr;

    PouchData *pouchPtr = pouchGetPtr();
    uint32_t inventoryType;

    switch (menuPtr->getCurrentIndex())
    {
        case InventoryType::STANDARD:
        {
            inventoryMenuPtr->setinventoryItemPtr(pouchPtr->items);
            inventoryMenuPtr->setInventorySize(sizeof(pouchPtr->items) / sizeof(pouchPtr->items[0]));
            inventoryType = InventoryType::STANDARD;

            inventoryMenuPtr->setMinValue(ItemId::ITEM_GOLD_BAR);
            inventoryMenuPtr->setMaxValue(ItemId::ITEM_FRESH_JUICE);
            break;
        }
        case InventoryType::IMPORTANT:
        {
            inventoryMenuPtr->setinventoryItemPtr(pouchPtr->keyItems);
            inventoryMenuPtr->setInventorySize(sizeof(pouchPtr->keyItems) / sizeof(pouchPtr->keyItems[0]));
            inventoryType = InventoryType::IMPORTANT;

            inventoryMenuPtr->setMinValue(ItemId::ITEM_STRANGE_SACK);
            inventoryMenuPtr->setMaxValue(ItemId::ITEM_CRYSTAL_STAR);
            break;
        }
        case InventoryType::BADGES:
        {
            inventoryMenuPtr->setinventoryItemPtr(pouchPtr->badges);
            inventoryMenuPtr->setInventorySize(sizeof(pouchPtr->badges) / sizeof(pouchPtr->badges[0]));
            inventoryType = InventoryType::BADGES;

            inventoryMenuPtr->setMinValue(ItemId::ITEM_POWER_JUMP);
            inventoryMenuPtr->setMaxValue(ItemId::ITEM_SUPER_CHARGE_P);
            break;
        }
        case InventoryType::STORED:
        {
            inventoryMenuPtr->setinventoryItemPtr(pouchPtr->storedItems);
            inventoryMenuPtr->setInventorySize(sizeof(pouchPtr->storedItems) / sizeof(pouchPtr->storedItems[0]));
            inventoryType = InventoryType::STORED;

            inventoryMenuPtr->setMinValue(ItemId::ITEM_GOLD_BAR);
            inventoryMenuPtr->setMaxValue(ItemId::ITEM_FRESH_JUICE);
            break;
        }
        default:
        {
            // Failsafe: Close everything if this is somehow reached
            closeAllMenus();
            return;
        }
    }

    inventoryMenuPtr->setInventoryType(inventoryType);

    constexpr uint32_t totalOptions = sizeof(gMainOptions) / sizeof(MenuOption);
    enterNextMenu(&gMainFuncs, totalOptions);
}

static void mainExit()
{
    delete gInventoryMenu;
    gInventoryMenu = nullptr;
}

// Menu for determining which inventory to work with
static const MenuOption gInitOptions[] = {
    "Standard Items",
    mainInit,

    "Important Items",
    mainInit,

    "Badges",
    mainInit,

    "Stored Items",
    mainInit,
};

static const MenuFunctions gInitFuncs = {
    gInitOptions,
    basicMenuLayoutControls,
    basicMenuLayoutDrawMenuLineHeight,
    nullptr, // Exit function not needed
};

void inventoryMenuInit(Menu *menuPtr)
{
    (void)menuPtr;

    constexpr uint32_t totalOptions = sizeof(gInitOptions) / sizeof(MenuOption);
    enterNextMenu(&gInitFuncs, totalOptions);
}
