#include "menuUtils.h"
#include "customState.h"
#include "drawText.h"
#include "cheats.h"
#include "classes/window.h"
#include "menus/warpsMenu.h"
#include "menus/rootMenu.h"
#include "misc/utils.h"
#include "ttyd/camdrv.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

const char *gCustomStatesCannotCreateText =
    "To create a custom state, you must have\na file loaded and not be in a battle nor a\nscreen transition.";

const char *gCustomStatesNoStatesText = "No custom states currently exist.";
const char *gCustomStatesLimitReachedText = "The maximum amount of custom states currently exist.";

const char *gCustomStatesSuccessfullyOverwriteStateText = "The custom state was successfully overwritten.";

const char *gCustomStatesNotEnoughStatesToSwapMoveText =
    "There needs to be at least two custom\nstates to be able to swap/move them.";

const MenuOption gWarpsMenuCustomStatesOptions[] {
    "Load State",
    warpsMenuCustomStatesLoadState,

    "Create State",
    warpsMenuCustomStatesCreateState,

    "Duplicate State",
    warpsMenuCustomStatesDuplicateState,

    "Swap States",
    warpsMenuCustomStatesSwapStates,

    "Move State",
    warpsMenuCustomStatesMoveState,

    "Overwrite State",
    warpsMenuCustomStatesOverwriteState,

    "Rename State",
    warpsMenuCustomStatesRenameState,

    "Delete State",
    warpsMenuCustomStatesDeleteState,
};

const MenuFunctions gWarpsMenuCustomStatesFuncs = {
    gWarpsMenuCustomStatesOptions,
    warpsMenuCustomStatesControls,
    warpsMenuCustomStatesDraw,
    nullptr, // Exit function not needed
};

void warpsMenuCustomStatesInit(Menu *menuPtr)
{
    (void)menuPtr;

    constexpr uint32_t totalOptions = sizeof(gWarpsMenuCustomStatesOptions) / sizeof(MenuOption);
    enterNextMenu(&gWarpsMenuCustomStatesFuncs, totalOptions);
}

void WarpsMenu::customStatesFlagSetControls(MenuButtonInput button)
{
    const uint32_t totalOptions = gCustomState.getTotalEntries();

    // If there is only one option, then don't bother doing anything
    if (totalOptions <= 1)
    {
        return;
    }

    uint8_t *currentPagePtr = nullptr;
    if (totalOptions > MAX_CUSTOM_STATES_PER_PAGE)
    {
        currentPagePtr = &this->currentPage;
    }

    menuControlsVertical(button, &this->currentIndex, currentPagePtr, totalOptions, MAX_CUSTOM_STATES_PER_PAGE, 1, false);
}

void warpsMenuCustomStatesControls(Menu *menuPtr, MenuButtonInput button)
{
    WarpsMenu *warpsMenuPtr = gWarpsMenu;

    // If the name editor is open, then handle the controls for that
    NameEditor *nameEditorPtr = warpsMenuPtr->getNameEditorPtr();
    if (nameEditorPtr->shouldDraw())
    {
        nameEditorPtr->controls(button);
        return;
    }

    // If no flags are set, then use the default controls
    if (!menuPtr->anyFlagIsSet())
    {
        basicMenuLayoutControls(menuPtr, button);
        return;
    }

    // The function for checking for auto-incrementing needs to run every frame to be handled correctly
    const bool autoIncrement = handleMenuAutoIncrement(warpsMenuPtr->getAutoIncrementPtr());

    // Handle held button inputs if auto-incrementing should be done
    if (autoIncrement)
    {
        const MenuButtonInput buttonHeld = getMenuButtonInput(false);
        switch (buttonHeld)
        {
            case MenuButtonInput::DPAD_DOWN:
            case MenuButtonInput::DPAD_UP:
            {
                warpsMenuPtr->customStatesFlagSetControls(buttonHeld);
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
        case MenuButtonInput::DPAD_DOWN:
        case MenuButtonInput::DPAD_UP:
        {
            warpsMenuPtr->customStatesFlagSetControls(button);
            break;
        }
        case MenuButtonInput::A:
        {
            menuPtr->runSelectedOptionFunc();
            break;
        }
        case MenuButtonInput::B:
        {
            // If swapping/moving states and have selected a state to swap/move, then go back to selecting a state
            if (menuPtr->flagIsSet(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_SWAP_MOVE_SELECTED_STATE_TO_SWAP))
            {
                menuPtr->clearFlag(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_SWAP_MOVE_SELECTED_STATE_TO_SWAP);
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

void WarpsMenu::drawCustomStatesInfo() const
{
    // Get the total states out of the maximum
    const CustomState *customStatePtr = &gCustomState;
    const uint32_t totalEntries = customStatePtr->getTotalEntries();

    char buf[32];
    constexpr uint32_t bufSize = sizeof(buf);
    snprintf(buf, bufSize, "Total States\n%" PRIu32 "/%" PRIu32, totalEntries, MAX_CUSTOM_STATES);

    // Get the text position for the top-left of the window two lines under the main text
    float tempPosX;
    float tempPosY;
    constexpr float scale = MENU_SCALE;
    const Window *rootWindowPtr = gRootWindow;
    rootWindowPtr->getTextPosXY(nullptr, WindowAlignment::TOP_LEFT, scale, &tempPosX, &tempPosY);

    const Menu *menuPtr = gMenu;
    const uint32_t totalOptions = menuPtr->getTotalOptions();

    const float posXBase = tempPosX;
    const float posYBase = tempPosY;

    constexpr float lineDecrement = MENU_LINE_HEIGHT * scale;
    float posY = posYBase - (intToFloat(totalOptions + 1) * lineDecrement);
    float posX = posXBase;

    // Draw the total states out of the maximum
    drawText(buf, posX, posY, scale, getColorWhite(0xFF));

    // Make sure there is at least one custom state
    CustomStateEntry *entriesPtr = customStatePtr->getEntriesPtr();
    if ((totalEntries == 0) || !entriesPtr)
    {
        return;
    }

    // Draw the page number at the top-right of the window if there is more than one page of states
    const uint32_t currentPage = this->currentPage;
    if (totalEntries > MAX_CUSTOM_STATES_PER_PAGE)
    {
        // Draw the page as an int, to prevent long text if it somehow becomes negative
        snprintf(buf, bufSize, "Page %" PRId32, currentPage + 1);

        const float pageNumPosX = rootWindowPtr->getTextPosX(buf, WindowAlignment::TOP_RIGHT, scale);
        drawText(buf, pageNumPosX, posYBase, scale, getColorWhite(0xFF));
    }

    // Draw the names of each custom state
    // Set the text position to be a bit to the right of the main text
    float width;
    getTextWidthHeight("Overwrite State", scale, &width, nullptr);
    posX = posXBase + width + (50.f * scale);
    posY = posYBase;

    const bool swappingOrMovingStatesSelectedState =
        menuPtr->flagIsSet(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_SWAP_MOVE_SELECTED_STATE_TO_SWAP);

    const bool anyFlagIsSet = menuPtr->anyFlagIsSet();

    const uint32_t currentIndex = this->currentIndex;
    const uint32_t selectedIndex = this->selectedIndex;
    const uint32_t startingIndex = MAX_CUSTOM_STATES_PER_PAGE * currentPage;
    const uint32_t endingIndex = startingIndex + MAX_CUSTOM_STATES_PER_PAGE;

    for (uint32_t i = startingIndex; i < endingIndex; i++)
    {
        if (i >= totalEntries)
        {
            // Reached the end of the states, so exit
            break;
        }

        // Draw the name
        uint32_t color = getColorWhite(0xFF);

        if (anyFlagIsSet)
        {
            if (currentIndex == i)
            {
                color = getColorBlue(0xFF);
            }
            else if (swappingOrMovingStatesSelectedState && (selectedIndex == i))
            {
                color = getColorGreen(0xFF);
            }
        }

        // The names are not null terminated, so copy them to a buffer
        // Failsafe: Make sure the size does not exceed bufSize
        uint32_t size = CUSTOM_STATE_NAME_SIZE + 1;
        if (size > bufSize)
        {
            size = bufSize;
        }

        // Use snprintf to make sure bufSize is not exceeded, and that a null terminator is properly applied
        snprintf(buf, size, "%s", entriesPtr[i].getStateNamePtr());

        drawText(buf, posX, posY, scale, color);
        posY -= lineDecrement;
    }
}

void warpsMenuCustomStatesDraw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDrawMenuLineHeight(cameraId, user);

    // Draw the info for the custom states
    WarpsMenu *warpsMenuPtr = gWarpsMenu;
    warpsMenuPtr->drawCustomStatesInfo();

    // Draw the name editor if applicable
    NameEditor *nameEditorPtr = warpsMenuPtr->getNameEditorPtr();
    if (nameEditorPtr->shouldDraw())
    {
        nameEditorPtr->draw();
    }

    // Draw an error message if applicable
    ErrorWindow *errorWindowPtr = warpsMenuPtr->getErrorWindowPtr();
    if (errorWindowPtr->shouldDraw())
    {
        errorWindowPtr->draw();
    }
}

void warpsMenuCustomStatesLoadState(Menu *menuPtr)
{
    (void)menuPtr;

    WarpsMenu *warpsMenuPtr = gWarpsMenu;

    // If there are currently no states, then show an error message
    CustomState *customStatePtr = &gCustomState;
    const CustomStateEntry *entriesPtr = customStatePtr->getEntriesPtr();

    if ((customStatePtr->getTotalEntries() == 0) || !entriesPtr)
    {
        menuPtr->clearFlag(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_LOAD);
        warpsMenuPtr->initErrorWindow(gCustomStatesNoStatesText);
        return;
    }

    // If the Load option was just selected, then allow the player to choose a state to load
    if (!menuPtr->flagIsSet(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_LOAD))
    {
        menuPtr->setFlag(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_LOAD);

        // Reset the current index to the top of the current page
        warpsMenuPtr->setCurrentIndex(warpsMenuPtr->getCurrentPage() * MAX_CUSTOM_STATES_PER_PAGE);
        return;
    }

    // Make sure a file is currently loaded and the player is not currently transitioning screens nor in a battle
    if (!checkIfInGame())
    {
        warpsMenuPtr->initErrorWindow(gCustomStatesCannotCreateText);
        return;
    }

    // Initialize warping via the custom state
    const uint32_t currentIndex = warpsMenuPtr->getCurrentIndex();
    customStatePtr->setSelectedState(currentIndex);
    customStatePtr->setInit();

    // Set the next map and loading zone based on the selected custom state
    const CustomStateEntry *currentEntry = &entriesPtr[currentIndex];

    // Neither are null terminated, so must copy them to a temporary buffer
    char buf[32];
    constexpr uint32_t bufSize = sizeof(buf);

    // Failsafe: Make sure the size does not exceed bufSize
    uint32_t size = currentEntry->getCurrentMapSize();
    if (size > bufSize)
    {
        size = bufSize;
    }

    // Use snprintf to make sure bufSize is not exceeded, and that a null terminator is properly applied
    snprintf(buf, size, "%s", currentEntry->getCurrentMapPtr());
    setNextMap(buf);

    // Failsafe: Make sure the size does not exceed bufSize
    size = currentEntry->getCurrentBeroSize();
    if (size > bufSize)
    {
        size = bufSize;
    }

    // Use snprintf to make sure bufSize is not exceeded, and that a null terminator is properly applied
    snprintf(buf, size, "%s", currentEntry->getCurrentBeroPtr());
    setNextBero(buf);

    // Start the warping process
    reloadRoomMain();

    // Done, so close the menu
    return closeAllMenus();
}

void warpsMenuCustomStatesCloseNameEditor()
{
    gWarpsMenu->getNameEditorPtr()->stopDrawing();
}

void warpsMenuCustomStatesCreateStateNameSet(char *newNamePtr, uint32_t newNameSize)
{
    (void)newNameSize;

    // Create the new state
    if (!gCustomState.addState(newNamePtr))
    {
        // The maximum amount of states have been created, so show an error message
        // Shouldn't ever be reached, but add anyway as a failsafe
        gWarpsMenu->initErrorWindow(gCustomStatesLimitReachedText);
    }

    // Close the name editor
    warpsMenuCustomStatesCloseNameEditor();
}

void warpsMenuCustomStatesCreateState(Menu *menuPtr)
{
    (void)menuPtr;

    CustomState *customStatePtr = &gCustomState;
    WarpsMenu *warpsMenuPtr = gWarpsMenu;

    // If the maximum amount of states have been created, then show an error message
    if (customStatePtr->limitHasBeenReached())
    {
        warpsMenuPtr->initErrorWindow(gCustomStatesLimitReachedText);
        return;
    }

    // Initialize the name editor
    NameEditor *nameEditorPtr = warpsMenuPtr->getNameEditorPtr();
    const Window *rootWindowPtr = gRootWindow;

    nameEditorPtr->init(rootWindowPtr,
                        nullptr,
                        warpsMenuPtr->getStateNamePtr(),
                        CUSTOM_STATE_NAME_SIZE + 1,
                        true,
                        rootWindowPtr->getAlpha());

    nameEditorPtr->startDrawing(warpsMenuCustomStatesCreateStateNameSet, warpsMenuCustomStatesCloseNameEditor);
}

void WarpsMenu::duplicateState(Menu *menuPtr)
{
    CustomState *customStatePtr = &gCustomState;

    // Duplicate the selected state
    if (!customStatePtr->duplicateState(this->currentIndex))
    {
        // Error occurred
        menuPtr->clearFlag(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_DUPLICATE);
        return;
    }

    // If the maximum amount of states have been added, then stop duplicating states
    if (customStatePtr->limitHasBeenReached())
    {
        menuPtr->clearFlag(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_DUPLICATE);
    }
}

void warpsMenuCustomStatesDuplicateState(Menu *menuPtr)
{
    CustomState *customStatePtr = &gCustomState;
    WarpsMenu *warpsMenuPtr = gWarpsMenu;

    // If there are currently no states, then show an error message
    if (customStatePtr->getTotalEntries() == 0)
    {
        menuPtr->clearFlag(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_DUPLICATE);
        warpsMenuPtr->initErrorWindow(gCustomStatesNoStatesText);
        return;
    }

    // If the maximum amount of states have been added, then show an error message
    if (customStatePtr->limitHasBeenReached())
    {
        menuPtr->clearFlag(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_DUPLICATE);
        warpsMenuPtr->initErrorWindow(gCustomStatesLimitReachedText);
        return;
    }

    // If the Duplicate option was just selected, then allow the player to choose a state to duplicate
    if (!menuPtr->flagIsSet(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_DUPLICATE))
    {
        menuPtr->setFlag(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_DUPLICATE);

        // Reset the current index to the top of the current page
        warpsMenuPtr->setCurrentIndex(warpsMenuPtr->getCurrentPage() * MAX_CUSTOM_STATES_PER_PAGE);
        return;
    }

    // Duplicate the selected state
    warpsMenuPtr->duplicateState(menuPtr);
}

bool WarpsMenu::initSwapMoveStates(uint32_t currentIndex, uint32_t selectedIndex, Menu *menuPtr)
{
    // If there are not at least two states, then show an error message
    if (gCustomState.getTotalEntries() < 2)
    {
        menuPtr->clearFlag(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_SWAP_MOVE_INIT);
        menuPtr->clearFlag(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_SWAP_MOVE_SELECTED_STATE_TO_SWAP);
        this->initErrorWindow(gCustomStatesNotEnoughStatesToSwapMoveText);
        return false;
    }

    // If the Swap/Move option was just selected, then allow the player to choose a state to swap/move
    if (!menuPtr->flagIsSet(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_SWAP_MOVE_INIT))
    {
        menuPtr->setFlag(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_SWAP_MOVE_INIT);

        // Reset the current index to the top of the current page
        this->setCurrentIndex(this->currentPage * MAX_CUSTOM_STATES_PER_PAGE);
        return false;
    }

    // If a state hasn't been selected yet, then select the current one to be swapped/moved
    if (!menuPtr->flagIsSet(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_SWAP_MOVE_SELECTED_STATE_TO_SWAP))
    {
        menuPtr->setFlag(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_SWAP_MOVE_SELECTED_STATE_TO_SWAP);
        this->setSelectedIndex(currentIndex);
        return false;
    }

    // Make sure the selected state is not the current state
    if (selectedIndex == currentIndex)
    {
        return false;
    }

    return true;
}

void WarpsMenu::swapStates(uint32_t currentIndex, uint32_t selectedIndex, Menu *menuPtr)
{
    // Swap the states
    if (!gCustomState.swapStates(currentIndex, selectedIndex))
    {
        // Error occurred
        menuPtr->clearFlag(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_SWAP_MOVE_INIT);
        menuPtr->clearFlag(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_SWAP_MOVE_SELECTED_STATE_TO_SWAP);
        return;
    }

    // Go back to selecting the first state
    menuPtr->clearFlag(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_SWAP_MOVE_SELECTED_STATE_TO_SWAP);
}

void warpsMenuCustomStatesSwapStates(Menu *menuPtr)
{
    WarpsMenu *warpsMenuPtr = gWarpsMenu;
    const uint32_t currentIndex = warpsMenuPtr->getCurrentIndex();
    const uint32_t selectedIndex = warpsMenuPtr->getSelectedIndex();

    if (warpsMenuPtr->initSwapMoveStates(currentIndex, selectedIndex, menuPtr))
    {
        // Swap the selected states
        warpsMenuPtr->swapStates(currentIndex, selectedIndex, menuPtr);
    }
}

void WarpsMenu::moveState(uint32_t currentIndex, uint32_t selectedIndex, Menu *menuPtr)
{
    // Move the states
    if (!gCustomState.moveState(currentIndex, selectedIndex))
    {
        // Error occurred
        menuPtr->clearFlag(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_SWAP_MOVE_INIT);
        menuPtr->clearFlag(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_SWAP_MOVE_SELECTED_STATE_TO_SWAP);
        return;
    }

    // Go back to selecting a state to move
    menuPtr->clearFlag(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_SWAP_MOVE_SELECTED_STATE_TO_SWAP);
}

void warpsMenuCustomStatesMoveState(Menu *menuPtr)
{
    WarpsMenu *warpsMenuPtr = gWarpsMenu;
    const uint32_t currentIndex = warpsMenuPtr->getCurrentIndex();
    const uint32_t selectedIndex = warpsMenuPtr->getSelectedIndex();

    if (warpsMenuPtr->initSwapMoveStates(currentIndex, selectedIndex, menuPtr))
    {
        // Move the selected state to the new location
        warpsMenuPtr->moveState(currentIndex, selectedIndex, menuPtr);
    }
}

void WarpsMenu::overwriteState(Menu *menuPtr)
{
    // Overwrite the current state
    if (!gCustomState.overwriteState(this->currentIndex))
    {
        // Error occurred
        menuPtr->clearFlag(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_OVERWRITE);
        return;
    }

    // By default there is no way to tell that the overwrite was successful, so draw text for that
    this->initErrorWindow(gCustomStatesSuccessfullyOverwriteStateText);
}

void warpsMenuCustomStatesOverwriteState(Menu *menuPtr)
{
    WarpsMenu *warpsMenuPtr = gWarpsMenu;

    // Make sure there is at least one state
    if (gCustomState.getTotalEntries() == 0)
    {
        menuPtr->clearFlag(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_OVERWRITE);
        warpsMenuPtr->initErrorWindow(gCustomStatesNoStatesText);
        return;
    }

    // If the Overwrite option was just selected, then allow the player to choose a state to overwrite
    if (!menuPtr->flagIsSet(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_OVERWRITE))
    {
        menuPtr->setFlag(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_OVERWRITE);

        // Reset the current index to the top of the current page
        warpsMenuPtr->setCurrentIndex(warpsMenuPtr->getCurrentPage() * MAX_CUSTOM_STATES_PER_PAGE);
        return;
    }

    // Overwrite the selected state
    warpsMenuPtr->overwriteState(menuPtr);
}

void warpsMenuCustomStatesRenameStateNameSet(char *newNamePtr, uint32_t newNameSize)
{
    (void)newNamePtr;
    (void)newNameSize;

    /*
    // Rename the current state
    if (!gCustomState.renameState(newNamePtr, gWarpsMenu->getCurrentIndex()))
    {
        // Error occurred
        gMenu->clearFlag(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_RENAME);
    }
    */

    // Close the name editor
    warpsMenuCustomStatesCloseNameEditor();
}

void warpsMenuCustomStatesRenameState(Menu *menuPtr)
{
    WarpsMenu *warpsMenuPtr = gWarpsMenu;

    // Make sure there is at least one state, and that the entries pointer is set
    CustomState *customStatePtr = &gCustomState;
    CustomStateEntry *entriesPtr = customStatePtr->getEntriesPtr();

    if ((customStatePtr->getTotalEntries() == 0) || !entriesPtr)
    {
        menuPtr->clearFlag(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_RENAME);
        warpsMenuPtr->initErrorWindow(gCustomStatesNoStatesText);
        return;
    }

    // If the Rename option was just selected, then allow the player to choose a state to rename
    if (!menuPtr->flagIsSet(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_RENAME))
    {
        menuPtr->setFlag(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_RENAME);

        // Reset the current index to the top of the current page
        warpsMenuPtr->setCurrentIndex(warpsMenuPtr->getCurrentPage() * MAX_CUSTOM_STATES_PER_PAGE);
        return;
    }

    // Initialize the name editor
    NameEditor *nameEditorPtr = warpsMenuPtr->getNameEditorPtr();
    const Window *rootWindowPtr = gRootWindow;

    const uint32_t currentIndex = warpsMenuPtr->getCurrentIndex();
    char *stateNamePtr = entriesPtr[currentIndex].getStateNamePtr();

    // The initial string needs to be null terminated, so copy the state name to a temporary buffer
    // Use snprintf to make sure the buffer size is not exceeded, and that a null terminator is properly applied
    char *stateNameBufferPtr = warpsMenuPtr->getStateNamePtr();
    snprintf(stateNameBufferPtr, CUSTOM_STATE_NAME_SIZE + 1, "%s", stateNamePtr);

    nameEditorPtr
        ->init(rootWindowPtr, stateNameBufferPtr, stateNamePtr, CUSTOM_STATE_NAME_SIZE, false, rootWindowPtr->getAlpha());

    nameEditorPtr->startDrawing(warpsMenuCustomStatesRenameStateNameSet, warpsMenuCustomStatesCloseNameEditor);
}

void WarpsMenu::deleteState(Menu *menuPtr)
{
    CustomState *customStatePtr = &gCustomState;
    uint32_t currentIndex = this->currentIndex;

    // Delete the current state
    if (!customStatePtr->deleteState(currentIndex))
    {
        // Error occurred
        menuPtr->clearFlag(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_DELETE);
        return;
    }

    // If the current index was at the last valid index, then it needs to be moved up one
    if (currentIndex >= customStatePtr->getTotalEntries())
    {
        this->setCurrentIndex(--currentIndex);

        // Check if moving up one should be placed on the previous page
        const uint32_t currentPage = this->currentPage;
        if (currentIndex < (currentPage * MAX_CUSTOM_STATES_PER_PAGE))
        {
            this->setCurrentPage(currentPage - 1);
        }
    }

    // If there are no more states, then stop deleting them
    if (customStatePtr->getTotalEntries() == 0)
    {
        menuPtr->clearFlag(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_DELETE);
    }
}

void warpsMenuCustomStatesDeleteState(Menu *menuPtr)
{
    WarpsMenu *warpsMenuPtr = gWarpsMenu;

    // Make sure there is at least one state
    if (gCustomState.getTotalEntries() == 0)
    {
        menuPtr->clearFlag(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_DELETE);
        warpsMenuPtr->initErrorWindow(gCustomStatesNoStatesText);
        return;
    }

    // If the Delete option was just selected, then allow the player to choose a state to delete
    if (!menuPtr->flagIsSet(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_DELETE))
    {
        menuPtr->setFlag(WarpsMenuCustomStatesFlag::WARPS_MENU_CUSTOM_STATES_FLAG_DELETE);

        // Reset the current index to the top of the current page
        warpsMenuPtr->setCurrentIndex(warpsMenuPtr->getCurrentPage() * MAX_CUSTOM_STATES_PER_PAGE);
        return;
    }

    // Delete the selected state
    warpsMenuPtr->deleteState(menuPtr);
}
