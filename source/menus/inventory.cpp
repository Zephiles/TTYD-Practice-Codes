#include "menuUtils.h"
#include "drawIcon.h"
#include "drawText.h"
#include "classes/menu.h"
#include "classes/window.h"
#include "menus/inventory.h"
#include "menus/root.h"
#include "misc/utils.h"
#include "ttyd/mario_pouch.h"
#include "ttyd/item_data.h"
#include "ttyd/dispdrv.h"
#include "ttyd/fontmgr.h"

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cinttypes>

Inventory *gInventory = nullptr;

// Main menu
const MenuOption inventoryMenuMainOptions[] = {
    "Add By Id",
    selectedOptionAddById,

    "Add By Icon",
    selectedOptionAddByIcon,

    "Duplicate",
    selectedOptionDuplicate,

    "Change By Id",
    selectedOptionChangeById,

    "Change By Icon",
    selectedOptionChangeByIcon,

    "Delete",
    selectedOptionDelete,
};

const MenuFunctions inventoryMenuMainFuncs = {
    inventoryMenuMainControls,
    inventoryMenuMainDraw,
    inventoryMenuMainExit,
};

const char *inventoryIsEmptyText = "The inventory is currently empty.";
const char *inventoryIsFullText = "The inventory is currently full.";

void cancelAddItemFromId()
{
    gInventory->getValueEditor()->stopDrawing();
    gMenu->clearFlag(InventoryFlag::INVENTORY_FLAG_CURRENTLY_SELECTING_ID);
}

void addItemFromId(const ValueType *valuePtr)
{
    Inventory *inventoryPtr = gInventory;

    // Make sure the inventory isn't full
    uint32_t totalItems = inventoryPtr->getTotalItemsInInventory();
    if (totalItems >= inventoryPtr->getInventorySize())
    {
        cancelAddItemFromId();
        return;
    }

    // Set the new item
    const ItemId item = static_cast<ItemId>(valuePtr->s16);
    inventoryPtr->getInventoryItemPtr()[totalItems] = item;

    // The battle menu will not properly update when an upgrade is given, so manually update it
    recheckBattleUpgrades(item);

    // The Important Items in the pause menu will not visually update when an important item is added, so manually reset the
    // Important Items menu
    resetPauseMenuImportantItems();

    // If the inventory is now full, then stop adding items
    if (inventoryPtr->inventoryIsFull())
    {
        cancelAddItemFromId();
    }
}

void selectedOptionAddById(Menu *menuPtr)
{
    // If the inventory is full, then show an error message
    Inventory *inventoryPtr = gInventory;
    if (inventoryPtr->inventoryIsFull())
    {
        inventoryPtr->initErrorWindow(inventoryIsFullText);
        return;
    }

    // Bring up the window for selecting an id
    menuPtr->setFlag(InventoryFlag::INVENTORY_FLAG_CURRENTLY_SELECTING_ID);

    // Initialize the value editor
    ValueEditor *valueEditorPtr = inventoryPtr->getValueEditor();

    uint32_t flags = 0;
    // flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_UP_DOWN);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Y_SET_MAX);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Z_SET_MIN);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_ITEM_ICON_AND_TEXT);

    const ItemId *minValuePtr = inventoryPtr->getMinValuePtr();
    const ItemId *maxValuePtr = inventoryPtr->getMaxValuePtr();
    const Window *rootWindowPtr = gRootWindow;

    valueEditorPtr->init(minValuePtr,
                         minValuePtr,
                         maxValuePtr,
                         rootWindowPtr,
                         flags,
                         VariableType::s16,
                         rootWindowPtr->getAlpha(),
                         inventoryPtr->getScale());

    valueEditorPtr->startDrawing(addItemFromId, cancelAddItemFromId);
}

void cancelAddItemFromIcon()
{
    gInventory->getItemIconSelector()->stopDrawing();
    gMenu->clearFlag(InventoryFlag::INVENTORY_FLAG_CURRENTLY_SELECTING_ICON);
}

void addItemFromIcon(ItemId item)
{
    Inventory *inventoryPtr = gInventory;

    // Make sure the inventory isn't full
    uint32_t totalItems = inventoryPtr->getTotalItemsInInventory();
    if (totalItems >= inventoryPtr->getInventorySize())
    {
        cancelAddItemFromIcon();
        return;
    }

    // Add the new item
    inventoryPtr->getInventoryItemPtr()[totalItems] = item;

    // The battle menu will not properly update when an upgrade is given, so manually update it
    recheckBattleUpgrades(item);

    // The Important Items in the pause menu will not visually update when an important item is added, so manually reset the
    // Important Items menu
    resetPauseMenuImportantItems();

    // If the inventory is now full, then stop adding items
    if (inventoryPtr->inventoryIsFull())
    {
        cancelAddItemFromIcon();
    }
}

void selectedOptionAddByIcon(Menu *menuPtr)
{
    // If the inventory is full, then show an error message
    Inventory *inventoryPtr = gInventory;
    if (inventoryPtr->inventoryIsFull())
    {
        inventoryPtr->initErrorWindow(inventoryIsFullText);
        return;
    }

    // Bring up the window for selecting an icon
    menuPtr->setFlag(InventoryFlag::INVENTORY_FLAG_CURRENTLY_SELECTING_ICON);

    // Initialize the item icon selector
    const Window *rootWindowPtr = gRootWindow;
    ItemIconSelector *itemIconSelectorPtr = inventoryPtr->getItemIconSelector();

    itemIconSelectorPtr->init(rootWindowPtr,
                              *inventoryPtr->getMinValuePtr(),
                              *inventoryPtr->getMaxValuePtr(),
                              rootWindowPtr->getAlpha(),
                              inventoryPtr->getScale());

    itemIconSelectorPtr->startDrawing(addItemFromIcon, cancelAddItemFromIcon);
}

void Inventory::duplicateItem(Menu *menuPtr)
{
    // Make sure the current index is valid
    const uint32_t index = this->currentIndex;
    if (index >= this->inventorySize)
    {
        menuPtr->clearFlag(InventoryFlag::INVENTORY_FLAG_DUPLICATE);
        return;
    }

    // Get the item to be duplicated
    ItemId *inventoryItemPtr = this->inventoryItemPtr;
    const ItemId item = inventoryItemPtr[index];

    // Make sure the item is valid
    if (!itemIsValid(item))
    {
        return;
    }

    // Add the item to the next empty slot in the array
    inventoryItemPtr[this->getTotalItemsInInventory()] = item;

    // The Important Items in the pause menu will not visually update when an important item is added, so manually reset the
    // Important Items menu
    resetPauseMenuImportantItems();

    // If the inventory is now full, then stop duplicating items
    if (this->inventoryIsFull())
    {
        menuPtr->clearFlag(InventoryFlag::INVENTORY_FLAG_DUPLICATE);
    }
}

void selectedOptionDuplicate(Menu *menuPtr)
{
    // If the inventory is empty, then show an error message
    Inventory *inventoryPtr = gInventory;
    if (inventoryPtr->inventoryIsEmpty())
    {
        inventoryPtr->initErrorWindow(inventoryIsEmptyText);
        return;
    }

    // If the inventory is full, then show an error message
    if (inventoryPtr->inventoryIsFull())
    {
        inventoryPtr->initErrorWindow(inventoryIsFullText);
        return;
    }

    // If the Duplicate option was just selected, then allow the player to choose an item/badge to duplicate
    if (!menuPtr->flagIsSet(InventoryFlag::INVENTORY_FLAG_DUPLICATE))
    {
        menuPtr->setFlag(InventoryFlag::INVENTORY_FLAG_DUPLICATE);

        // Reset the current index to the top of the current page
        inventoryPtr->setCurrentIndex(inventoryPtr->getCurrentPage() * INVENTORY_ITEMS_PER_PAGE);
        return;
    }

    // Duplicate the selected item/badge
    inventoryPtr->duplicateItem(menuPtr);
}

void changeItemFromId(const ValueType *valuePtr)
{
    Inventory *inventoryPtr = gInventory;

    // Make sure the current index is valid
    const uint32_t index = inventoryPtr->getCurrentIndex();
    if (index >= inventoryPtr->getInventorySize())
    {
        cancelAddItemFromId();
        return;
    }

    // Set the new item
    const ItemId item = static_cast<ItemId>(valuePtr->s16);
    inventoryPtr->getInventoryItemPtr()[index] = item;

    // The battle menu will not properly update when an upgrade is changed, so manually update it
    recheckBattleUpgrades(item);

    // The Important Items in the pause menu will not visually update when an important item is changed, so manually reset the
    // Important Items menu
    resetPauseMenuImportantItems();

    // Close the value editor
    cancelAddItemFromId();
}

void selectedOptionChangeById(Menu *menuPtr)
{
    // If the inventory is empty, then show an error message
    Inventory *inventoryPtr = gInventory;
    if (inventoryPtr->inventoryIsEmpty())
    {
        inventoryPtr->initErrorWindow(inventoryIsEmptyText);
        return;
    }

    // If the Change By Id option was just selected, then allow the player to choose an item/badge to change
    if (!menuPtr->flagIsSet(InventoryFlag::INVENTORY_FLAG_CHANGE_BY_ID))
    {
        menuPtr->setFlag(InventoryFlag::INVENTORY_FLAG_CHANGE_BY_ID);

        // Reset the current index to the top of the current page
        inventoryPtr->setCurrentIndex(inventoryPtr->getCurrentPage() * INVENTORY_ITEMS_PER_PAGE);
        return;
    }

    // Make sure the current index is valid
    uint32_t index = inventoryPtr->getCurrentIndex();
    if (index >= inventoryPtr->getInventorySize())
    {
        return;
    }

    // Set the cursor of the item icon selector to the item that is being changed
    ItemId item = inventoryPtr->getInventoryItemPtr()[index];

    // Make sure the item is valid
    const ItemId *minValuePtr = inventoryPtr->getMinValuePtr();
    if (!itemIsValid(item))
    {
        item = *minValuePtr;
    }

    // Bring up the window for selecting an id
    menuPtr->setFlag(InventoryFlag::INVENTORY_FLAG_CURRENTLY_SELECTING_ID);

    // Initialize the value editor
    ValueEditor *valueEditorPtr = inventoryPtr->getValueEditor();

    uint32_t flags = 0;
    // flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_UP_DOWN);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Y_SET_MAX);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Z_SET_MIN);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_ITEM_ICON_AND_TEXT);

    const ItemId *maxValuePtr = inventoryPtr->getMaxValuePtr();
    const Window *rootWindowPtr = gRootWindow;

    valueEditorPtr->init(&item,
                         minValuePtr,
                         maxValuePtr,
                         rootWindowPtr,
                         flags,
                         VariableType::s16,
                         rootWindowPtr->getAlpha(),
                         inventoryPtr->getScale());

    valueEditorPtr->startDrawing(changeItemFromId, cancelAddItemFromId);
}

void changeItemFromIcon(ItemId item)
{
    Inventory *inventoryPtr = gInventory;

    // Make sure the current index is valid
    const uint32_t index = inventoryPtr->getCurrentIndex();
    if (index >= inventoryPtr->getInventorySize())
    {
        cancelAddItemFromIcon();
        return;
    }

    // Set the new item
    inventoryPtr->getInventoryItemPtr()[index] = item;

    // The battle menu will not properly update when an upgrade is changed, so manually update it
    recheckBattleUpgrades(item);

    // The Important Items in the pause menu will not visually update when an important item is changed, so manually reset the
    // Important Items menu
    resetPauseMenuImportantItems();

    // Close the item icon selector
    cancelAddItemFromIcon();
}

void selectedOptionChangeByIcon(Menu *menuPtr)
{
    // If the inventory is empty, then show an error message
    Inventory *inventoryPtr = gInventory;
    if (inventoryPtr->inventoryIsEmpty())
    {
        inventoryPtr->initErrorWindow(inventoryIsEmptyText);
        return;
    }

    // If the Change By Icon option was just selected, then allow the player to choose an item/badge to change
    if (!menuPtr->flagIsSet(InventoryFlag::INVENTORY_FLAG_CHANGE_BY_ICON))
    {
        menuPtr->setFlag(InventoryFlag::INVENTORY_FLAG_CHANGE_BY_ICON);

        // Reset the current index to the top of the current page
        inventoryPtr->setCurrentIndex(inventoryPtr->getCurrentPage() * INVENTORY_ITEMS_PER_PAGE);
        return;
    }

    // Make sure the current index is valid
    uint32_t index = inventoryPtr->getCurrentIndex();
    if (index >= inventoryPtr->getInventorySize())
    {
        return;
    }

    // Bring up the window for selecting an icon
    menuPtr->setFlag(InventoryFlag::INVENTORY_FLAG_CURRENTLY_SELECTING_ICON);

    // Initialize the item icon selector
    const Window *rootWindowPtr = gRootWindow;
    ItemIconSelector *itemIconSelectorPtr = inventoryPtr->getItemIconSelector();

    itemIconSelectorPtr->init(rootWindowPtr,
                              *inventoryPtr->getMinValuePtr(),
                              *inventoryPtr->getMaxValuePtr(),
                              rootWindowPtr->getAlpha(),
                              inventoryPtr->getScale());

    // Set the cursor of the item icon selector to the item that is being changed
    ItemId item = inventoryPtr->getInventoryItemPtr()[index];

    // Make sure the item is valid
    if (!itemIsValid(item))
    {
        item = *inventoryPtr->getMinValuePtr();
    }

    itemIconSelectorPtr->setCurrentIndexFromItem(item);
    itemIconSelectorPtr->startDrawing(changeItemFromIcon, cancelAddItemFromIcon);
}

void Inventory::deleteItem(Menu *menuPtr)
{
    // Make sure the current index is valid
    uint32_t index = this->currentIndex;
    if (index >= this->inventorySize)
    {
        menuPtr->clearFlag(InventoryFlag::INVENTORY_FLAG_DELETE);
        return;
    }

    // Make a backup of the current item being deleted
    ItemId *inventoryItemPtr = this->inventoryItemPtr;
    const ItemId deletedItem = inventoryItemPtr[index];

    // Move the array back one slot
    const uint32_t inventorySize = this->inventorySize;
    const uint32_t remainingSize = inventorySize - index - 1;
    memcpy(&inventoryItemPtr[index], &inventoryItemPtr[index + 1], remainingSize * sizeof(ItemId));

    // Clear the last slot to make sure that the last item/badge in the array does not get duplicated
    inventoryItemPtr[inventorySize - 1] = ItemId::ITEM_NONE;

    // If the current index was at the last valid index, then it needs to be moved back one
    if (index >= this->getTotalItemsInInventory())
    {
        index--;
        this->setCurrentIndex(index);

        // Check if moving back one should be placed on the previous page
        const uint32_t currentPage = this->currentPage;
        if (index < (currentPage * INVENTORY_ITEMS_PER_PAGE))
        {
            this->setCurrentPage(currentPage - 1);
        }
    }

    // The battle menu will not properly update if an upgrade is removed, so manually update it
    recheckBattleUpgrades(deletedItem);

    // The Important Items in the pause menu will not visually update when an important item is removed, so manually reset the
    // Important Items menu
    resetPauseMenuImportantItems();

    // If the inventory is now empty, then stop deleting items
    if (this->inventoryIsEmpty())
    {
        menuPtr->clearFlag(InventoryFlag::INVENTORY_FLAG_DELETE);
    }
}

void selectedOptionDelete(Menu *menuPtr)
{
    // If the inventory is empty, then show an error message
    Inventory *inventoryPtr = gInventory;
    if (inventoryPtr->inventoryIsEmpty())
    {
        inventoryPtr->initErrorWindow(inventoryIsEmptyText);
        return;
    }

    // If the Delete option was just selected, then allow the player to choose an item/badge to delete
    if (!menuPtr->flagIsSet(InventoryFlag::INVENTORY_FLAG_DELETE))
    {
        menuPtr->setFlag(InventoryFlag::INVENTORY_FLAG_DELETE);

        // Reset the current index to the top of the current page
        inventoryPtr->setCurrentIndex(inventoryPtr->getCurrentPage() * INVENTORY_ITEMS_PER_PAGE);
        return;
    }

    // Remove the selected item/badge
    inventoryPtr->deleteItem(menuPtr);
}

void inventoryMenuMainControls(Menu *menuPtr, MenuButtonInput button)
{
    // If a file is loaded while this menu is open, then the inventory will change, so make sure no issues occur because of this
    Inventory *inventoryPtr = gInventory;
    inventoryPtr->verifyInventoryIndexAndPage(menuPtr);

    // If no flags are set, then use the default controls
    if (!menuPtr->anyFlagIsSet())
    {
        controlsBasicMenuLayout(menuPtr, button);
        return;
    }

    // If a separate window is currently open for selecting an item/badge, then only call the control functions for those
    // This should include all flags except for selecting an item/icon
    if (menuPtr->flagIsSet(InventoryFlag::INVENTORY_FLAG_CURRENTLY_SELECTING_ICON))
    {
        inventoryPtr->getItemIconSelector()->controls(button);
        return;
    }
    else if (menuPtr->flagIsSet(InventoryFlag::INVENTORY_FLAG_CURRENTLY_SELECTING_ID))
    {
        inventoryPtr->getValueEditor()->controls(button);
        return;
    }

    // The function for checking for auto-incrementing needs to run every frame to be handled correctly
    const bool autoIncrement = handleMenuAutoIncrement(inventoryPtr->getAutoIncrementPtr());

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
                inventoryPtr->inventoryMenuItemControls(buttonHeld);
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
            inventoryPtr->inventoryMenuItemControls(button);
            break;
        }
        case MenuButtonInput::R:
        {
            inventoryPtr->inventoryMenuMoveDownOnePage();
            break;
        }
        case MenuButtonInput::L:
        {
            inventoryPtr->inventoryMenuMoveUpOnePage();
            break;
        }
        case MenuButtonInput::A:
        {
            menuPtr->runSelectedOptionFunc();
            break;
        }
        case MenuButtonInput::B:
        {
            menuPtr->clearAllFlags();
            break;
        }
        default:
        {
            break;
        }
    }
}

void Inventory::drawCurrentInventory()
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
    char *displayBufferPtr = gDisplayBuffer;
    snprintf(displayBufferPtr, DISPLAY_BUFFER_SIZE, "%" PRId32 "/%" PRId32, currentItemCount, totalItems);

    const Window *inventoryWindow = &this->inventoryWindow;
    const float scale = this->scale;

    float itemCountPosX;
    float itemCountPosY;
    inventoryWindow->getTextPosXY(displayBufferPtr, WindowAlignment::TOP_CENTER, scale, &itemCountPosX, &itemCountPosY);

    uint32_t textColor = getColorWhite(0xFF);
    drawText(displayBufferPtr, itemCountPosX, itemCountPosY, scale, textColor);

    // Start drawing the items/badges with their icons
    const float padding = inventoryWindow->getPadding() * scale;
    float tempItemPosXBase;
    float tempItemPosYBase;
    inventoryWindow->getIconPosXY(WindowAlignment::TOP_LEFT, scale, &tempItemPosXBase, &tempItemPosYBase);

    const float itemPosXBase = tempItemPosXBase - padding;
    const float itemPosYBase = tempItemPosYBase - (LINE_HEIGHT_FLOAT * scale) + LINE_HEIGHT_ADJUSTMENT_5(scale) - padding;

    constexpr float textWidth = 140.f;
    const float widthAdjustment = INVENTORY_WIDTH_ADJUSTMENT(scale);
    const float itemPosXIncrement = textWidth + (ICON_SIZE_FLOAT * scale) + widthAdjustment;
    const float itemLineDecrement = SPACE_USED_PER_ICON(scale);

    float textPosXBase;
    float textPosYBase;
    getTextPosXYByIcon(itemPosXBase, itemPosYBase, scale, &textPosXBase, &textPosYBase);

    float textPosX = textPosXBase;
    float textPosY = textPosYBase;

    const Menu *menuPtr = gMenu;
    const uint32_t inventoryType = this->inventoryType;
    const bool drawVertical = (inventoryType == InventoryType::BADGES) || (inventoryType == InventoryType::STORED);
    const bool anyFlagIsSet = menuPtr->anyFlagIsSet();

    const bool currentlyChangingById = menuPtr->flagIsSet(InventoryFlag::INVENTORY_FLAG_CHANGE_BY_ID);
    const bool currentlySelectingId = menuPtr->flagIsSet(InventoryFlag::INVENTORY_FLAG_CURRENTLY_SELECTING_ID);
    const bool changeByIdWindowOpen = !currentlyChangingById && currentlySelectingId;

    const bool currentlyChangingByIcon = menuPtr->flagIsSet(InventoryFlag::INVENTORY_FLAG_CHANGE_BY_ICON);
    const bool currentlySelectingIcon = menuPtr->flagIsSet(InventoryFlag::INVENTORY_FLAG_CURRENTLY_SELECTING_ICON);
    const bool changeByIconWindowOpen = !currentlyChangingByIcon && currentlySelectingIcon;

    const ItemId *inventoryItemPtr = this->inventoryItemPtr;

    const uint32_t currentPage = this->currentPage;
    const uint32_t currentIndex = this->currentIndex;
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

        // If none of the flags are currently set, then the inventory is currently not being interacted with, so draw all of the
        // text as white
        // If a separate window is currently open for selecting an item/badge, then draw all of the text as white
        // If changing an item/badge by id/icon, then still draw the text as either white or blue
        if (!anyFlagIsSet || changeByIdWindowOpen || changeByIconWindowOpen)
        {
            // If the item has an invalid id, then set the text to red
            if (!currentItemIsValid)
            {
                textColor = getColorRed(0xFF);
            }
            else
            {
                textColor = getColorWhite(0xFF);
            }
        }
        else if (currentIndex == i)
        {
            textColor = getColorBlue(0xFF);
        }
        else
        {
            // If the item has an invalid id, then set the text to red
            if (!currentItemIsValid)
            {
                textColor = getColorRed(0xFF);
            }
            else
            {
                textColor = getColorWhite(0xFF);
            }
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
        const float iconLRScale = scale - 0.1f;

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

void inventoryMenuMainDraw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    drawBasicMenuLayout(cameraId, user);

    // If a file is loaded while this menu is open, then the inventory will change, so make sure no issues occur because of this
    Inventory *inventoryPtr = gInventory;
    inventoryPtr->verifyInventoryIndexAndPage(gMenu);

    // Draw the current inventory
    inventoryPtr->drawCurrentInventory();

    // Draw the value editor if applicable
    ValueEditor *valueEditorPtr = inventoryPtr->getValueEditor();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->draw();
    }

    // Draw the item icon selector if applicable
    ItemIconSelector *itemIconSelector = inventoryPtr->getItemIconSelector();
    if (itemIconSelector->shouldDraw())
    {
        itemIconSelector->draw();
    }

    // Draw an error message if applicable
    ErrorWindow *errorWindowPtr = inventoryPtr->getErrorWindow();
    if (errorWindowPtr->shouldDraw())
    {
        errorWindowPtr->draw();
    }
}

void Inventory::verifyInventoryIndexAndPage(Menu *menuPtr)
{
    // Verify that the current page is correct
    uint32_t totalItems = this->getTotalItemsInInventory();
    if (totalItems == 0)
    {
        // Set totalItems to 1 to allow the following checks to work correctly
        totalItems = 1;
    }

    const uint32_t totalPages = roundIntUpUnsigned(totalItems, INVENTORY_ITEMS_PER_PAGE);
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

bool Inventory::checkIfItemsOnNextPage(uint32_t currentItemCount) const
{
    if (currentItemCount == 0)
    {
        return false;
    }

    return (static_cast<uint32_t>((this->currentPage + 1) * INVENTORY_ITEMS_PER_PAGE)) <= (currentItemCount - 1);
}

void Inventory::inventoryMenuMoveUpOnePage()
{
    // If currently on the first page, then do nothing
    uint32_t currentPage = this->currentPage;
    if (currentPage == 0)
    {
        return;
    }

    // Go to the previous page
    currentPage--;
    this->setCurrentPage(currentPage);

    // Set currentIndex to the first option on the current page
    this->setCurrentIndex(currentPage * INVENTORY_ITEMS_PER_PAGE);
}

void Inventory::inventoryMenuMoveDownOnePage()
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

void Inventory::inventoryMenuItemControls(MenuButtonInput button)
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

Inventory::Inventory(float scale)
{
    this->scale = scale;

    const Window *rootWindowPtr = gRootWindow;
    ErrorWindow *errorWindowPtr = &this->errorWindow;

    errorWindowPtr->setScale(scale);
    errorWindowPtr->setAlpha(rootWindowPtr->getAlpha());

    // Place the inventory window inside of the root window
    Window *inventoryWindowPtr = &this->inventoryWindow;
    inventoryWindowPtr->copyWindow(rootWindowPtr);

    // Get the width that is being used by the main text options
    float textWidth;
    getTextWidthHeight("Change By Icon", scale, &textWidth, nullptr);
    const float mainTextWidth = textWidth + (inventoryWindowPtr->getPadding() * scale);

    // Adjust the window to exclude the space used by the main text options, plus a bit extra
    const float widthAdjustment = INVENTORY_WIDTH_ADJUSTMENT(scale);
    inventoryWindowPtr->setPosX(inventoryWindowPtr->getPosX() + mainTextWidth + widthAdjustment);
    inventoryWindowPtr->setWidth(inventoryWindowPtr->getWidth() - mainTextWidth - widthAdjustment);
}

void Inventory::initErrorWindow(const char *text)
{
    ErrorWindow *errorWindowPtr = &this->errorWindow;
    errorWindowPtr->setText(text);
    errorWindowPtr->setTimer(3000);
    errorWindowPtr->placeInWindow(gRootWindow, WindowAlignment::MIDDLE_CENTER);
}

uint32_t Inventory::getTotalItemsInInventory() const
{
    const uint32_t totalItems = this->inventorySize;
    const ItemId *inventoryItemPtr = this->inventoryItemPtr;
    uint32_t currentItemCount = 0;

    for (uint32_t i = 0; i < totalItems; i++)
    {
        if (inventoryItemPtr[i] == ItemId::ITEM_NONE)
        {
            break;
        }
        currentItemCount++;
    }

    return currentItemCount;
}

bool Inventory::inventoryIsFull() const
{
    return this->getTotalItemsInInventory() == this->inventorySize;
}

void inventoryMenuMainInit(Menu *menuPtr)
{
    // Failsafe: Make sure memory isn't already allocated for gInventory
    Inventory *inventoryPtr = gInventory;
    if (inventoryPtr)
    {
        delete inventoryPtr;
    }

    inventoryPtr = new Inventory(gRoot->getScale());
    gInventory = inventoryPtr;

    PouchData *pouchPtr = pouchGetPtr();
    uint32_t inventoryType;

    switch (menuPtr->getCurrentIndex())
    {
        case InventoryType::STANDARD:
        {
            inventoryPtr->setinventoryItemPtr(pouchPtr->items);
            inventoryPtr->setInventorySize(sizeof(pouchPtr->items) / sizeof(pouchPtr->items[0]));
            inventoryType = InventoryType::STANDARD;

            inventoryPtr->setMinValue(ItemId::ITEM_GOLD_BAR);
            inventoryPtr->setMaxValue(ItemId::ITEM_FRESH_JUICE);
            break;
        }
        case InventoryType::IMPORTANT:
        {
            inventoryPtr->setinventoryItemPtr(pouchPtr->keyItems);
            inventoryPtr->setInventorySize(sizeof(pouchPtr->keyItems) / sizeof(pouchPtr->keyItems[0]));
            inventoryType = InventoryType::IMPORTANT;

            inventoryPtr->setMinValue(ItemId::ITEM_STRANGE_SACK);
            inventoryPtr->setMaxValue(ItemId::ITEM_CRYSTAL_STAR);
            break;
        }
        case InventoryType::BADGES:
        {
            inventoryPtr->setinventoryItemPtr(pouchPtr->badges);
            inventoryPtr->setInventorySize(sizeof(pouchPtr->badges) / sizeof(pouchPtr->badges[0]));
            inventoryType = InventoryType::BADGES;

            inventoryPtr->setMinValue(ItemId::ITEM_POWER_JUMP);
            inventoryPtr->setMaxValue(ItemId::ITEM_SUPER_CHARGE_P);
            break;
        }
        case InventoryType::STORED:
        {
            inventoryPtr->setinventoryItemPtr(pouchPtr->storedItems);
            inventoryPtr->setInventorySize(sizeof(pouchPtr->storedItems) / sizeof(pouchPtr->storedItems[0]));
            inventoryType = InventoryType::STORED;

            inventoryPtr->setMinValue(ItemId::ITEM_GOLD_BAR);
            inventoryPtr->setMaxValue(ItemId::ITEM_FRESH_JUICE);
            break;
        }
        default:
        {
            // Failsafe: Close everything if this is somehow reached
            closeAllMenus();
            return;
        }
    }

    inventoryPtr->setInventoryType(inventoryType);

    constexpr uint32_t totalOptions = sizeof(inventoryMenuMainOptions) / sizeof(MenuOption);
    enterNextMenu(inventoryMenuMainOptions, &inventoryMenuMainFuncs, totalOptions);
}

void inventoryMenuMainExit()
{
    delete gInventory;
    gInventory = nullptr;
}

// Menu for determining which inventory to work with
const MenuOption inventoryMenuInitOptions[] = {
    "Standard Items",
    inventoryMenuMainInit,

    "Important Items",
    inventoryMenuMainInit,

    "Badges",
    inventoryMenuMainInit,

    "Stored Items",
    inventoryMenuMainInit,
};

const MenuFunctions inventoryMenuInitFuncs = {
    controlsBasicMenuLayout,
    drawBasicMenuLayout,
    nullptr, // Exit function not needed
};

void inventoryMenuInit(Menu *menuPtr)
{
    (void)menuPtr;

    constexpr uint32_t totalOptions = sizeof(inventoryMenuInitOptions) / sizeof(MenuOption);
    enterNextMenu(inventoryMenuInitOptions, &inventoryMenuInitFuncs, totalOptions);
}
