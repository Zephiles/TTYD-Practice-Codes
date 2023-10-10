#include "menuUtils.h"
#include "memoryWatch.h"
#include "drawText.h"
#include "menus/memoryMenu.h"
#include "menus/rootMenu.h"
#include "misc/utils.h"
#include "ttyd/camdrv.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

const char *gMemoryWatchesLimitReachedText = "The maximum amount of\nwatches has been reached.";
const char *gMemoryWatchesNoWatchesText = "There are currently no watches.";

const char *gMemoryWatchesNotEnoughWatchesToSwapMoveText =
    "There needs to be at least two watches\nto be able to swap/move them.";

const MenuOption gMemoryMenuMemoryWatchMenuOptions[] {
    "Add Watch",
    memoryMenuMemoryWatchAddWatch,

    "Modify Watch",
    memoryMenuMemoryWatchModifyWatch,

    "Duplicate Watch",
    memoryMenuMemoryWatchDuplicateWatch,

    "Swap Watches",
    memoryMenuMemoryWatchSwapWatches,

    "Move Watch",
    memoryMenuMemoryWatchMoveWatch,

    "Delete Watch",
    memoryMenuMemoryWatchDeleteWatch,
};

const MenuFunctions gMemoryMenuMemoryWatchMenuFuncs = {
    gMemoryMenuMemoryWatchMenuOptions,
    memoryMenuMemoryWatchMenuControls,
    memoryMenuMemoryWatchMenuDraw,
    nullptr, // Exit function not needed
};

void memoryMenuMemoryWatchMenuInit(Menu *menuPtr)
{
    (void)menuPtr;

    constexpr uint32_t totalOptions = sizeof(gMemoryMenuMemoryWatchMenuOptions) / sizeof(MenuOption);
    enterNextMenu(&gMemoryMenuMemoryWatchMenuFuncs, totalOptions);
}

void MemoryMenu::memoryWatchFlagSetControls(MenuButtonInput button)
{
    const uint32_t totalOptions = gMemoryWatch.getTotalEntries();

    // If there is only one option, then don't bother doing anything
    if (totalOptions <= 1)
    {
        return;
    }

    uint8_t *currentPagePtr = nullptr;
    if (totalOptions > MAX_MEMORY_WATCHES_PER_PAGE)
    {
        currentPagePtr = &this->currentPage;
    }

    menuControlsVertical(button, &this->currentIndex, currentPagePtr, totalOptions, MAX_MEMORY_WATCHES_PER_PAGE, 1, false);
}

void memoryMenuMemoryWatchMenuControls(Menu *menuPtr, MenuButtonInput button)
{
    // If no flags are set, then use the default controls
    if (!menuPtr->anyFlagIsSet())
    {
        basicMenuLayoutControls(menuPtr, button);
        return;
    }

    // The function for checking for auto-incrementing needs to run every frame to be handled correctly
    MemoryMenu *memoryMenuPtr = gMemoryMenu;
    const bool autoIncrement = handleMenuAutoIncrement(memoryMenuPtr->getAutoIncrementPtr());

    // Handle held button inputs if auto-incrementing should be done
    if (autoIncrement)
    {
        const MenuButtonInput buttonHeld = getMenuButtonInput(false);
        switch (buttonHeld)
        {
            case MenuButtonInput::DPAD_DOWN:
            case MenuButtonInput::DPAD_UP:
            {
                memoryMenuPtr->memoryWatchFlagSetControls(buttonHeld);
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
            memoryMenuPtr->memoryWatchFlagSetControls(button);
            break;
        }
        case MenuButtonInput::A:
        {
            menuPtr->runSelectedOptionFunc();
            break;
        }
        case MenuButtonInput::B:
        {
            // If swapping/moving watches and have selected a watch to swap/move, then go back to selecting a watch
            if (menuPtr->flagIsSet(MemoryMenuMemoryWatchFlag::MEMORY_MENU_MEMORY_WATCH_FLAG_SWAP_MOVE_SELECTED_WATCH_TO_SWAP))
            {
                menuPtr->clearFlag(MemoryMenuMemoryWatchFlag::MEMORY_MENU_MEMORY_WATCH_FLAG_SWAP_MOVE_SELECTED_WATCH_TO_SWAP);
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

void MemoryMenu::drawMemoryWatchMenuInfo() const
{
    // Draw the page number at the top-right of the main window if there is more than one page
    const uint32_t currentPage = this->currentPage;
    const Window *rootWindowPtr = gRootWindow;
    constexpr float scale = MENU_SCALE;
    float tempPosX;
    float tempPosY;

    MemoryWatch *memoryWatchPtr = &gMemoryWatch;
    const uint32_t totalEntries = memoryWatchPtr->getTotalEntries();

    char buf[128];
    constexpr uint32_t bufSize = sizeof(buf);

    if (totalEntries > MAX_MEMORY_WATCHES_PER_PAGE)
    {
        // Draw the page as an int, to prevent long text if it somehow becomes negative
        snprintf(buf, bufSize, "Page %" PRId32, currentPage + 1);

        rootWindowPtr->getTextPosXY(buf, WindowAlignment::TOP_RIGHT, scale, &tempPosX, &tempPosY);
        drawText(buf, tempPosX, tempPosY, scale, getColorWhite(0xFF));
    }

    // Draw the memory watch count at the top-center of the main window
    // Draw the counts as ints, to prevent long text if they somehow become negative
    snprintf(buf, bufSize, "%" PRId32 "/%" PRId32, static_cast<int32_t>(totalEntries), MAX_MEMORY_WATCHES);

    rootWindowPtr->getTextPosXY(buf, WindowAlignment::TOP_CENTER, scale, &tempPosX, &tempPosY);
    drawText(buf, tempPosX, tempPosY, scale, getColorWhite(0xFF));

    // Make sure the pointer for the entries is set
    const MemoryWatchEntry *entriesPtr = memoryWatchPtr->getEntriesPtr();
    if (!entriesPtr)
    {
        return;
    }

    // Get the text position for the top-left of the window two lines under the main text
    rootWindowPtr->getTextPosXY(nullptr, WindowAlignment::TOP_LEFT, scale, &tempPosX, &tempPosY);

    Menu *menuPtr = gMenu;
    const uint32_t totalOptions = menuPtr->getTotalOptions();

    constexpr float lineDecrement = MENU_LINE_HEIGHT * scale;
    float posY = tempPosY - (intToFloat(totalOptions + 1) * lineDecrement);
    const float posX = tempPosX;

    constexpr float displayHeaderOffset = 186.f;
    constexpr float valueHeaderOffset = displayHeaderOffset + 73.f;

    // Draw the headers for the values
    drawText("Name", posX, posY, scale, getColorWhite(0xFF));

    const char *displayHeaderText = "Display";
    drawText(displayHeaderText, posX + displayHeaderOffset, posY, scale, getColorWhite(0xFF));

    drawText("Value", posX + valueHeaderOffset, posY, scale, getColorWhite(0xFF));
    posY -= lineDecrement;

    // The on/off text will be centered based on the Display text
    const float displayTextWidth = getTextWidth(displayHeaderText, scale);

    const bool swappingOrMovingWatchesSelectedWatch =
        menuPtr->flagIsSet(MemoryMenuMemoryWatchFlag::MEMORY_MENU_MEMORY_WATCH_FLAG_SWAP_MOVE_SELECTED_WATCH_TO_SWAP);

    const bool anyFlagIsSet = menuPtr->anyFlagIsSet();

    const uint32_t currentIndex = this->currentIndex;
    const uint32_t selectedIndex = this->selectedIndex;
    const uint32_t startingIndex = MAX_MEMORY_WATCHES_PER_PAGE * currentPage;
    const uint32_t endingIndex = startingIndex + MAX_MEMORY_WATCHES_PER_PAGE;

    // Draw the watches on the current page
    for (uint32_t i = startingIndex; i < endingIndex; i++)
    {
        if (i >= totalEntries)
        {
            // Reached the end of the watches, so exit
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
            else if (swappingOrMovingWatchesSelectedWatch && (selectedIndex == i))
            {
                color = getColorGreen(0xFF);
            }
        }

        const MemoryWatchEntry *currentWatchPtr = &entriesPtr[i];
        drawText(currentWatchPtr->getNamePtrConst(), posX, posY, scale, 170.f, color);

        // Draw whether the watch will be drawn or not
        // Get the text and color
        const char *onOffText;
        getOnOffTextAndColor(currentWatchPtr->shouldDisplay(), &onOffText, &color, 0xFF);

        // Draw the text
        drawText(onOffText, posX + displayHeaderOffset, posY, scale, displayTextWidth, color, TextAlignment::CENTER);

        // Draw the value
        currentWatchPtr->getValueString(buf, bufSize);
        drawText(buf, posX + valueHeaderOffset, posY, scale, 206.f, getColorWhite(0xFF));
        posY -= lineDecrement;
    }
}

void memoryMenuMemoryWatchMenuDraw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDrawMenuLineHeight(cameraId, user);

    // Draw the info for the memory watches
    MemoryMenu *memoryMenuPtr = gMemoryMenu;
    memoryMenuPtr->drawMemoryWatchMenuInfo();

    // Draw an error message if applicable
    ErrorWindow *errorWindowPtr = memoryMenuPtr->getErrorWindowPtr();
    if (errorWindowPtr->shouldDraw())
    {
        errorWindowPtr->draw();
    }
}

void memoryMenuMemoryWatchAddWatch(Menu *menuPtr)
{
    (void)menuPtr;

    if (!gMemoryWatch.addWatch())
    {
        // The maximum amount of watches have been added, so show an error message
        gMemoryMenu->initErrorWindow(gMemoryWatchesLimitReachedText);
    }
}

void memoryMenuMemoryWatchModifyWatch(Menu *menuPtr)
{
    MemoryMenu *memoryMenuPtr = gMemoryMenu;

    // If there are currently no watches, then show an error message
    if (gMemoryWatch.getTotalEntries() == 0)
    {
        menuPtr->clearFlag(MemoryMenuMemoryWatchFlag::MEMORY_MENU_MEMORY_WATCH_FLAG_MODIFY);
        memoryMenuPtr->initErrorWindow(gMemoryWatchesNoWatchesText);
        return;
    }

    // If the Modify option was just selected, then allow the player to choose a watch to modify
    if (!menuPtr->flagIsSet(MemoryMenuMemoryWatchFlag::MEMORY_MENU_MEMORY_WATCH_FLAG_MODIFY))
    {
        menuPtr->setFlag(MemoryMenuMemoryWatchFlag::MEMORY_MENU_MEMORY_WATCH_FLAG_MODIFY);

        // Reset the current index to the top of the current page
        memoryMenuPtr->setCurrentIndex(memoryMenuPtr->getCurrentPage() * MAX_MEMORY_WATCHES_PER_PAGE);
        return;
    }

    // Enter the Modify menu
    memoryMenuPtr->setSelectedIndex(memoryMenuPtr->getCurrentIndex());
    return memoryMenuMemoryWatchModifyInit(menuPtr);
}

void memoryMenuMemoryWatchDuplicateWatch(Menu *menuPtr)
{
    MemoryWatch *memoryWatchPtr = &gMemoryWatch;
    MemoryMenu *memoryMenuPtr = gMemoryMenu;

    // If there are currently no watches, then show an error message
    if (memoryWatchPtr->getTotalEntries() == 0)
    {
        menuPtr->clearFlag(MemoryMenuMemoryWatchFlag::MEMORY_MENU_MEMORY_WATCH_FLAG_DUPLICATE);
        memoryMenuPtr->initErrorWindow(gMemoryWatchesNoWatchesText);
        return;
    }

    // If the maximum amount of watches have been added, then show an error message
    if (memoryWatchPtr->limitHasBeenReached())
    {
        menuPtr->clearFlag(MemoryMenuMemoryWatchFlag::MEMORY_MENU_MEMORY_WATCH_FLAG_DUPLICATE);
        memoryMenuPtr->initErrorWindow(gMemoryWatchesLimitReachedText);
        return;
    }

    // If the Duplicate option was just selected, then allow the player to choose a watch to duplicate
    if (!menuPtr->flagIsSet(MemoryMenuMemoryWatchFlag::MEMORY_MENU_MEMORY_WATCH_FLAG_DUPLICATE))
    {
        menuPtr->setFlag(MemoryMenuMemoryWatchFlag::MEMORY_MENU_MEMORY_WATCH_FLAG_DUPLICATE);

        // Reset the current index to the top of the current page
        memoryMenuPtr->setCurrentIndex(memoryMenuPtr->getCurrentPage() * MAX_MEMORY_WATCHES_PER_PAGE);
        return;
    }

    // Duplicate the selected watch
    if (!memoryWatchPtr->duplicateWatch(memoryMenuPtr->getCurrentIndex()))
    {
        // Error occurred
        menuPtr->clearFlag(MemoryMenuMemoryWatchFlag::MEMORY_MENU_MEMORY_WATCH_FLAG_DUPLICATE);
        return;
    }

    // If the maximum amount of watches have been added, then stop duplicating watches
    if (memoryWatchPtr->limitHasBeenReached())
    {
        menuPtr->clearFlag(MemoryMenuMemoryWatchFlag::MEMORY_MENU_MEMORY_WATCH_FLAG_DUPLICATE);
    }
}

bool MemoryMenu::initSwapMoveWatches(Menu *menuPtr)
{
    // If there are not at least two watches, then show an error message
    if (gMemoryWatch.getTotalEntries() < 2)
    {
        menuPtr->clearFlag(MemoryMenuMemoryWatchFlag::MEMORY_MENU_MEMORY_WATCH_FLAG_SWAP_MOVE_INIT);
        menuPtr->clearFlag(MemoryMenuMemoryWatchFlag::MEMORY_MENU_MEMORY_WATCH_FLAG_SWAP_MOVE_SELECTED_WATCH_TO_SWAP);
        this->initErrorWindow(gMemoryWatchesNotEnoughWatchesToSwapMoveText);
        return false;
    }

    // If the Swap/Move option was just selected, then allow the player to choose a watch to swap/move
    if (!menuPtr->flagIsSet(MemoryMenuMemoryWatchFlag::MEMORY_MENU_MEMORY_WATCH_FLAG_SWAP_MOVE_INIT))
    {
        menuPtr->setFlag(MemoryMenuMemoryWatchFlag::MEMORY_MENU_MEMORY_WATCH_FLAG_SWAP_MOVE_INIT);

        // Reset the current index to the top of the current page
        this->setCurrentIndex(this->currentPage * MAX_MEMORY_WATCHES_PER_PAGE);
        return false;
    }

    // If a watch hasn't been selected yet, then select the current one to be swapped/moved
    const uint32_t currentIndex = this->currentIndex;
    if (!menuPtr->flagIsSet(MemoryMenuMemoryWatchFlag::MEMORY_MENU_MEMORY_WATCH_FLAG_SWAP_MOVE_SELECTED_WATCH_TO_SWAP))
    {
        menuPtr->setFlag(MemoryMenuMemoryWatchFlag::MEMORY_MENU_MEMORY_WATCH_FLAG_SWAP_MOVE_SELECTED_WATCH_TO_SWAP);
        this->setSelectedIndex(currentIndex);
        return false;
    }

    // Make sure the selected watch is not the current watch
    if (this->selectedIndex == currentIndex)
    {
        return false;
    }

    return true;
}

void memoryMenuMemoryWatchSwapWatches(Menu *menuPtr)
{
    MemoryMenu *memoryMenuPtr = gMemoryMenu;
    if (!memoryMenuPtr->initSwapMoveWatches(menuPtr))
    {
        return;
    }

    // Swap the selected watches
    if (!gMemoryWatch.swapWatches(memoryMenuPtr->getCurrentIndex(), memoryMenuPtr->getSelectedIndex()))
    {
        // Error occurred
        menuPtr->clearFlag(MemoryMenuMemoryWatchFlag::MEMORY_MENU_MEMORY_WATCH_FLAG_SWAP_MOVE_INIT);
        menuPtr->clearFlag(MemoryMenuMemoryWatchFlag::MEMORY_MENU_MEMORY_WATCH_FLAG_SWAP_MOVE_SELECTED_WATCH_TO_SWAP);
        return;
    }

    // Go back to selecting the first watch
    menuPtr->clearFlag(MemoryMenuMemoryWatchFlag::MEMORY_MENU_MEMORY_WATCH_FLAG_SWAP_MOVE_SELECTED_WATCH_TO_SWAP);
}

void memoryMenuMemoryWatchMoveWatch(Menu *menuPtr)
{
    MemoryMenu *memoryMenuPtr = gMemoryMenu;
    if (!memoryMenuPtr->initSwapMoveWatches(menuPtr))
    {
        return;
    }

    // Move the selected watch to the new location
    if (!gMemoryWatch.moveWatch(memoryMenuPtr->getCurrentIndex(), memoryMenuPtr->getSelectedIndex()))
    {
        // Error occurred
        menuPtr->clearFlag(MemoryMenuMemoryWatchFlag::MEMORY_MENU_MEMORY_WATCH_FLAG_SWAP_MOVE_INIT);
        menuPtr->clearFlag(MemoryMenuMemoryWatchFlag::MEMORY_MENU_MEMORY_WATCH_FLAG_SWAP_MOVE_SELECTED_WATCH_TO_SWAP);
        return;
    }

    // Go back to selecting a watch to move
    menuPtr->clearFlag(MemoryMenuMemoryWatchFlag::MEMORY_MENU_MEMORY_WATCH_FLAG_SWAP_MOVE_SELECTED_WATCH_TO_SWAP);
}

void memoryMenuMemoryWatchDeleteWatch(Menu *menuPtr)
{
    MemoryWatch *memoryWatchPtr = &gMemoryWatch;
    MemoryMenu *memoryMenuPtr = gMemoryMenu;

    // Make sure there is at least one watch
    if (memoryWatchPtr->getTotalEntries() == 0)
    {
        menuPtr->clearFlag(MemoryMenuMemoryWatchFlag::MEMORY_MENU_MEMORY_WATCH_FLAG_DELETE);
        memoryMenuPtr->initErrorWindow(gMemoryWatchesNoWatchesText);
        return;
    }

    // If the Delete option was just selected, then allow the player to choose a watch to delete
    if (!menuPtr->flagIsSet(MemoryMenuMemoryWatchFlag::MEMORY_MENU_MEMORY_WATCH_FLAG_DELETE))
    {
        menuPtr->setFlag(MemoryMenuMemoryWatchFlag::MEMORY_MENU_MEMORY_WATCH_FLAG_DELETE);

        // Reset the current index to the top of the current page
        memoryMenuPtr->setCurrentIndex(memoryMenuPtr->getCurrentPage() * MAX_MEMORY_WATCHES_PER_PAGE);
        return;
    }

    // Delete the selected watch
    uint32_t currentIndex = memoryMenuPtr->getCurrentIndex();
    if (!memoryWatchPtr->deleteWatch(currentIndex))
    {
        // Error occurred
        menuPtr->clearFlag(MemoryMenuMemoryWatchFlag::MEMORY_MENU_MEMORY_WATCH_FLAG_DELETE);
        return;
    }

    // If the current index was at the last valid index, then it needs to be moved up one
    const uint32_t totalEntries = memoryWatchPtr->getTotalEntries();
    if (currentIndex >= totalEntries)
    {
        memoryMenuPtr->setCurrentIndex(--currentIndex);

        // Check if moving up one should be placed on the previous page
        const uint32_t currentPage = memoryMenuPtr->getCurrentPage();
        if (currentIndex < (currentPage * MAX_MEMORY_WATCHES_PER_PAGE))
        {
            memoryMenuPtr->setCurrentPage(currentPage - 1);
        }
    }

    // If there are no more watches, then stop deleting them
    if (totalEntries == 0)
    {
        menuPtr->clearFlag(MemoryMenuMemoryWatchFlag::MEMORY_MENU_MEMORY_WATCH_FLAG_DELETE);
    }
}
