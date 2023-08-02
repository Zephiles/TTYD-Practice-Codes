#include "menuUtils.h"
#include "cxx.h"
#include "cheats.h"
#include "classes/menu.h"
#include "gc/pad.h"
#include "menus/rootMenu.h"
#include "menus/cheatsMenu.h"
#include "misc/utils.h"
#include "ttyd/system.h"
#include "ttyd/win_main.h"
#include "ttyd/event.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

Menu *gMenu = nullptr;

Menu *enterNextMenu(const MenuFunctions *funcs, uint32_t totalOptions)
{
    return enterNextMenu(funcs, totalOptions, totalOptions);
}

Menu *enterNextMenu(const MenuFunctions *funcs, uint32_t totalOptions, uint32_t totalOptionsPerPage)
{
    Menu *nextMenu = new Menu(funcs, totalOptions, totalOptionsPerPage);
    nextMenu->setPrevMenu(gMenu);
    gMenu = nextMenu;
    return nextMenu;
}

Menu *enterPrevMenu()
{
    // Make sure there is at least one menu
    Menu *menuPtr = gMenu;
    if (!menuPtr)
    {
        return menuPtr;
    }

    menuPtr->runExitFunc();
    Menu *prevMenu = menuPtr->getPrevMenu();
    delete menuPtr;

    gMenu = prevMenu;
    return prevMenu;
}

void closeAllMenus()
{
    Menu *menuPtr = gMenu;
    while (menuPtr)
    {
        menuPtr = enterPrevMenu();
    }
}

bool handleMenuAutoIncrement(MenuAutoIncrement *autoIncrement)
{
    // Check to see if any of the D-Pad buttons are held
    constexpr uint32_t buttons =
        PadInput::PAD_DPAD_LEFT | PadInput::PAD_DPAD_RIGHT | PadInput::PAD_DPAD_DOWN | PadInput::PAD_DPAD_UP;

    if (!(keyGetButton(PadId::CONTROLLER_ONE) & buttons))
    {
        // Reset the counters
        autoIncrement->waitFramesToBegin = 0;
        autoIncrement->framesBeforeIncrement = 0;
        return false;
    }

    // Check to see if the value should begin to auto-increment
    // Require holding a direction for 500ms before auto-incrementing starts
    uint32_t waitFramesToBegin = autoIncrement->waitFramesToBegin;
    if (waitFramesToBegin < sysMsec2Frame(500))
    {
        autoIncrement->waitFramesToBegin = static_cast<uint16_t>(++waitFramesToBegin);
        return false;
    }

    // Check to see if the number should be auto-incremented or not
    // Increment at a rate of 50ms
    uint32_t framesBeforeIncrement = autoIncrement->framesBeforeIncrement;
    if (framesBeforeIncrement < sysMsec2Frame(50))
    {
        autoIncrement->framesBeforeIncrement = static_cast<uint16_t>(++framesBeforeIncrement);
        return false;
    }

    // Auto-increment the value
    autoIncrement->framesBeforeIncrement = 0;
    return true;
}

MenuButtonInput getMenuButtonInput(bool singleFrame)
{
    uint32_t buttonInput;
    if (singleFrame)
    {
        buttonInput = keyGetButtonTrg(PadId::CONTROLLER_ONE);
    }
    else
    {
        buttonInput = keyGetButton(PadId::CONTROLLER_ONE);
    }

    for (uint32_t i = 0, counter = 1; i < TOTAL_MENU_INPUT_BUTTONS; i++, counter++)
    {
        if (i == 7)
        {
            // Skip unused value
            i++;
        }

        if ((buttonInput >> i) & 1U)
        {
            return static_cast<MenuButtonInput>(counter);
        }
    }

    // Return 0 if no input was found
    return MenuButtonInput::BUTTON_NONE;
}

void basicMenuLayoutControls(Menu *menuPtr, MenuButtonInput button)
{
    static MenuAutoIncrement autoIncrement = {0, 0};

    // The function for checking for auto-incrementing needs to run every frame to be handled correctly
    const bool shouldAutoIncrement = handleMenuAutoIncrement(&autoIncrement);

    // Handle held button inputs if auto-incrementing should be done
    if (shouldAutoIncrement)
    {
        const MenuButtonInput buttonHeld = getMenuButtonInput(false);
        switch (buttonHeld)
        {
            case MenuButtonInput::DPAD_DOWN:
            case MenuButtonInput::DPAD_UP:
            {
                menuPtr->basicLayoutControls(buttonHeld);
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
            menuPtr->basicLayoutControls(button);
            break;
        }
        case MenuButtonInput::A:
        {
            menuPtr->runSelectedOptionFunc();
            break;
        }
        case MenuButtonInput::B:
        {
            enterPrevMenu();
            break;
        }
        default:
        {
            break;
        }
    }
}

void drawMainWindow()
{
    gRootWindow->draw();
}

void basicMenuLayoutDraw(CameraId cameraId, void *user)
{
    basicMenuLayoutDraw(cameraId, user, 0.f, 0.f);
}

void basicMenuLayoutDraw(CameraId cameraId, void *user, float offsetX, float offsetY)
{
    (void)cameraId;
    (void)user;

    // Draw the main window
    drawMainWindow();

    // Draw the main text
    gMenu->basicLayoutDraw(offsetX, offsetY);
}

void handleMenu()
{
    // Check if the menu should be opened/closed
    // Prevent checking it if currently in the process of spawning an item
    // Prevent checking it if the memory editor is open

    // TODO: Add check for memory editor being open
    if (gCheats->getSpawnItemCheatPtr()->getValueEditorPtr())
    {
        return;
    }

    // Check if the menu is being manually opened/closed via the button combo
    if (checkButtonCombo(OPEN_CLOSE_MENU_BUTTON_COMBO) && !gMod.changingButtonCombo())
    {
        if (!gMenu)
        {
            // Opening the menu
            rootMenuInit();
        }
        else
        {
            // Closing the menu
            closeAllMenus();
        }

        // Do not accept inputs on the frame that the menu was opened/closed
        return;
    }

    Menu *menuPtr = gMenu;
    if (menuPtr)
    {
        // Handle button inputs for the menu
        menuPtr->runControlsFunc(getMenuButtonInput(true));

        // Draw the menu
        // gMenu may have been modified, so have to re-get it
        menuPtr = gMenu;
        if (menuPtr)
        {
            menuPtr->runDrawFunc();
        }
    }
}

void menuControlsVertical(MenuButtonInput button,
                          uint8_t *currentIndexPtr,
                          uint8_t *currentPagePtr,
                          uint32_t totalOptions,
                          uint32_t totalOptionsPerPage,
                          uint32_t totalOptionsPerRow,
                          bool allowUpwardsSnap)
{
    uint32_t currentPage = 0;
    if (currentPagePtr)
    {
        currentPage = *currentPagePtr;
    }

    uint32_t currentIndex = *currentIndexPtr;

    const uint32_t totalRowsPerPage = intCeil(totalOptionsPerPage, totalOptionsPerRow);
    const uint32_t totalColumns = intCeil(totalOptionsPerPage, totalRowsPerPage);
    const uint32_t columnSplitAmount = intCeil(totalOptionsPerPage, totalColumns);

    const uint32_t lastValidOption = totalOptions - 1;
    const uint32_t firstOptionOnPage = totalOptionsPerPage * currentPage;
    const uint32_t lastOptionOnPage = (totalOptionsPerPage * (currentPage + 1)) - 1;

    uint32_t lastValidOptionOnPage = lastOptionOnPage;
    int32_t newCurrentMenuOption;

    // Make sure the last option on the page is valid
    if (lastValidOptionOnPage > lastValidOption)
    {
        lastValidOptionOnPage = lastValidOption;
    }

    switch (button)
    {
        case MenuButtonInput::DPAD_LEFT:
        {
            // Check if currently on the furthest left column
            newCurrentMenuOption = currentIndex - columnSplitAmount;
            if (newCurrentMenuOption < static_cast<int32_t>(firstOptionOnPage))
            {
                // Currently on the furthest left column, so go to the furthest right column
                // Check to see if there are more options to the right of the current column
                newCurrentMenuOption = currentIndex + columnSplitAmount;
                const uint32_t lastColumnFirstOption = lastOptionOnPage - columnSplitAmount + 1;

                if ((lastValidOptionOnPage >= lastColumnFirstOption) && (lastValidOptionOnPage <= lastOptionOnPage))
                {
                    // Move to the furthest right option on the page
                    for (uint32_t i = 0; i < (totalColumns - 1); i++)
                    {
                        currentIndex += columnSplitAmount;
                    }

                    // Make sure the value is valid
                    if (allowUpwardsSnap)
                    {
                        // Move to the furthest right option on the page
                        while (currentIndex > lastValidOptionOnPage)
                        {
                            currentIndex--;
                        }
                    }
                    else
                    {
                        // Move to the furthest right option without moving up
                        while (currentIndex > lastValidOptionOnPage)
                        {
                            currentIndex -= columnSplitAmount;
                        }
                    }
                }
                else
                {
                    // There are no options in the right columns, so do nothing
                    return;
                }
            }
            else
            {
                // Move to the next left column
                currentIndex -= columnSplitAmount;
            }
            break;
        }
        case MenuButtonInput::DPAD_RIGHT:
        {
            // Check if currently on the furthest right column
            const uint32_t furthestRightColumnStart = (lastOptionOnPage - columnSplitAmount) + 1;
            if (currentIndex >= furthestRightColumnStart)
            {
                // Move to the furthest left option on the page
                for (uint32_t i = 0; i < (totalColumns - 1); i++)
                {
                    currentIndex -= columnSplitAmount;
                }
            }
            else
            {
                // Check to see if there are more options to the right of the current column
                currentIndex += columnSplitAmount;
                const uint32_t lastColumnFirstOption = lastOptionOnPage - columnSplitAmount + 1;

                if ((lastValidOptionOnPage >= lastColumnFirstOption) && (lastValidOptionOnPage <= lastOptionOnPage))
                {
                    // Make sure the current option is valid
                    if (allowUpwardsSnap)
                    {
                        while (currentIndex > lastValidOptionOnPage)
                        {
                            // Go to the option in the previous row
                            currentIndex--;
                        }
                    }
                    else
                    {
                        if (currentIndex > lastValidOptionOnPage)
                        {
                            // Move to the furthest left option
                            uint32_t optionAfterFirstColumn = firstOptionOnPage + columnSplitAmount;
                            do
                            {
                                currentIndex -= columnSplitAmount;
                            } while (currentIndex >= optionAfterFirstColumn);
                        }
                    }
                }
                else
                {
                    // There are no options in the right columns, so do nothing
                    return;
                }
            }
            break;
        }
        case MenuButtonInput::DPAD_DOWN:
        {
            // Check if currently at the bottom of the current column
            bool currentlyAtBottom = false;
            for (uint32_t i = 0; i < totalColumns; i++)
            {
                uint32_t currentOption = lastOptionOnPage - (columnSplitAmount * i);
                if (currentIndex == currentOption)
                {
                    currentlyAtBottom = true;
                    break;
                }
            }

            if (currentlyAtBottom)
            {
                // Check to see if the next page has a valid option
                // Get the valid option on the next page if it exists
                newCurrentMenuOption = currentIndex + 1;
                for (uint32_t i = 0; i < (totalColumns - 1); i++)
                {
                    newCurrentMenuOption += columnSplitAmount;
                }

                const uint32_t nextPageFirstColumnOption = lastOptionOnPage + 1;
                const uint32_t tempNewCurrentMenuOption = static_cast<uint32_t>(newCurrentMenuOption);
                bool foundValidOption = false;

                for (uint32_t i = nextPageFirstColumnOption; i <= tempNewCurrentMenuOption; i++)
                {
                    if (i > lastValidOption)
                    {
                        break;
                    }

                    newCurrentMenuOption = i;
                    foundValidOption = true;
                }

                if (foundValidOption)
                {
                    // Go to the valid option on the next page
                    currentIndex = static_cast<uint32_t>(newCurrentMenuOption);
                    currentPage += 1;
                }
                else
                {
                    // Go to the top of the column on the first page
                    currentIndex = (currentIndex % totalOptionsPerPage) - columnSplitAmount + 1;
                    currentPage = 0;
                }
            }
            else if (currentIndex == lastValidOption)
            {
                // Currently on the last option, so go to the top of the column on the first page
                uint32_t lastOptionCurrentColumn = 0;
                for (uint32_t i = 0; i <= lastValidOption; i += columnSplitAmount)
                {
                    lastOptionCurrentColumn += columnSplitAmount;
                }

                const uint32_t totalFreeSpaces = lastOptionCurrentColumn - currentIndex - 1;
                currentIndex = (((currentIndex % totalOptionsPerPage) - columnSplitAmount) + totalFreeSpaces) + 1;
                currentPage = 0;
            }
            else
            {
                // Move to the next option
                currentIndex += 1;
            }
            break;
        }
        case MenuButtonInput::DPAD_UP:
        {
            // Check if currently at the top of the current column
            bool currentlyAtTop = false;
            for (uint32_t i = 0; i < totalColumns; i++)
            {
                const uint32_t currentOption = firstOptionOnPage + (columnSplitAmount * i);
                if (currentIndex == currentOption)
                {
                    currentlyAtTop = true;
                    break;
                }
            }

            if (currentlyAtTop)
            {
                // Check if currently on the first page
                if (currentPage == 0)
                {
                    // Go to the last option of the current column on the last page
                    const uint32_t lastPage = intCeil(totalOptions, totalOptionsPerPage) - 1;
                    const uint32_t lastPageLastColumnLastOption = (totalOptionsPerPage * (lastPage + 1)) - 1;

                    uint32_t decrementAmount = columnSplitAmount * (totalColumns - 1);
                    for (uint32_t i = 0; i < currentIndex; i += columnSplitAmount)
                    {
                        decrementAmount -= columnSplitAmount;
                    }

                    currentIndex = lastPageLastColumnLastOption - decrementAmount;

                    // Make sure the new option is valid
                    while (currentIndex > lastValidOption)
                    {
                        currentIndex--;
                    }

                    currentPage = lastPage;
                }
                else
                {
                    // Go to the last option of the current column on the previous page
                    newCurrentMenuOption = currentIndex - 1;
                    for (uint32_t i = 0; i < (totalColumns - 1); i++)
                    {
                        newCurrentMenuOption -= columnSplitAmount;
                    }

                    currentIndex = static_cast<uint32_t>(newCurrentMenuOption);
                    currentPage -= 1;
                }
            }
            else
            {
                // Move to the next option
                currentIndex -= 1;
            }
            break;
        }
        default:
        {
            return;
        }
    }

    *currentIndexPtr = static_cast<uint8_t>(currentIndex);

    if (currentPagePtr)
    {
        *currentPagePtr = static_cast<uint8_t>(currentPage);
    }
}

void menuControlsHorizontal(MenuButtonInput button,
                            uint8_t *currentIndexPtr,
                            uint8_t *currentPagePtr,
                            uint32_t totalOptions,
                            uint32_t totalOptionsPerPage,
                            uint32_t totalOptionsPerRow,
                            bool allowUpwardsSnap)
{
    uint32_t currentPage = 0;
    if (currentPagePtr)
    {
        currentPage = *currentPagePtr;
    }

    uint32_t currentIndex = *currentIndexPtr;

    const uint32_t lastValidOption = totalOptions - 1;
    const uint32_t firstOptionOnPage = totalOptionsPerPage * currentPage;
    const uint32_t currentColumn = currentIndex % totalOptionsPerRow;
    const uint32_t lastColumn = totalOptionsPerRow - 1;
    const uint32_t lastPage = intCeil(totalOptions, totalOptionsPerPage) - 1;

    const uint32_t totalRows = intCeil(totalOptions, totalOptionsPerRow);
    const uint32_t totalFreeSpaces = ((totalRows * totalOptionsPerRow) - 1) - (totalOptions - 1);

    uint32_t totalColumns = totalOptionsPerRow;

    // Make sure the total columns is valid
    if (totalColumns > totalOptions)
    {
        totalColumns = totalOptions;
    }

    switch (button)
    {
        case MenuButtonInput::DPAD_LEFT:
        {
            // Check to see if currently on the furthest left column
            if (currentColumn == 0)
            {
                // Check to see if currently on the last option
                if ((currentIndex + lastColumn) > lastValidOption)
                {
                    if (allowUpwardsSnap)
                    {
                        // If currently on the first option of the current page, then there are no valid options to the right,
                        // so do nothing
                        if (currentIndex == firstOptionOnPage)
                        {
                            return;
                        }

                        // Go to the furthest right option and up one column
                        currentIndex -= 1;
                    }
                    else
                    {
                        // Go to the furthest left option
                        currentIndex = (currentIndex + lastColumn) - totalFreeSpaces;
                    }
                }
                else
                {
                    // Go to the furthest right option in the current row
                    currentIndex += lastColumn;
                }
            }
            else
            {
                // Go to the next left option in the current row
                currentIndex -= 1;
            }
            break;
        }
        case MenuButtonInput::DPAD_RIGHT:
        {
            // Check to see if currently on the furthest right column
            if (currentColumn == lastColumn)
            {
                // Currently on the furthest right side, so go to the furthest left option
                currentIndex -= lastColumn;
            }
            else
            {
                // Check to see if currently on the last option
                if (currentIndex >= lastValidOption)
                {
                    if (allowUpwardsSnap)
                    {
                        // If currently on the first option of the current page, then there are no valid options to the right,
                        // so do nothing
                        if (currentIndex == firstOptionOnPage)
                        {
                            return;
                        }

                        // Go to the next right option and up one column
                        currentIndex -= lastColumn;
                    }
                    else
                    {
                        // Go to the furthest left option
                        currentIndex = (currentIndex + 1) - (totalOptionsPerRow - totalFreeSpaces);
                    }
                }
                else
                {
                    // Go to the next right option in the current row
                    currentIndex += 1;
                }
            }
            break;
        }
        case MenuButtonInput::DPAD_DOWN:
        {
            // Check if currently at the bottom of the current column
            const uint32_t lastOptionOnPage = (totalOptionsPerPage * (currentPage + 1)) - 1;
            bool currentlyAtBottom = false;

            for (uint32_t i = 0; i < totalColumns; i++)
            {
                const uint32_t currentOption = lastOptionOnPage - i;
                if (currentIndex == currentOption)
                {
                    currentlyAtBottom = true;
                    break;
                }
            }

            if (currentlyAtBottom)
            {
                // Check to see if currently on the last page or not
                if (currentPage == lastPage)
                {
                    // Currently on the last page, so go to the first page
                    currentPage = 0;

                    // Go to the top of the current column
                    currentIndex = currentColumn;
                }
                else
                {
                    // Go to the next page
                    currentPage += 1;
                    currentIndex += totalOptionsPerRow;

                    // Make sure the current option is valid
                    if (currentIndex > lastValidOption)
                    {
                        currentIndex = lastValidOption;
                    }
                }
            }
            else if ((currentIndex + totalOptionsPerRow) > lastValidOption)
            {
                // Go to the first page
                currentPage = 0;

                // Go to the top of the current column
                currentIndex = currentColumn;
            }
            else
            {
                // Go to the next option in the current column
                currentIndex += totalOptionsPerRow;
            }
            break;
        }
        case MenuButtonInput::DPAD_UP:
        {
            // Check if currently at the top of the current column
            bool currentlyAtTop = false;

            for (uint32_t i = 0; i < totalColumns; i++)
            {
                const uint32_t currentOption = firstOptionOnPage + i;
                if (currentIndex == currentOption)
                {
                    currentlyAtTop = true;
                    break;
                }
            }

            if (currentlyAtTop)
            {
                // Check if currently on the first page
                if (currentPage == 0)
                {
                    // Go to the last page
                    currentPage = lastPage;

                    // Go to the last option in the current column
                    currentIndex += (totalRows - 1) * totalOptionsPerRow;

                    // Make sure the current option is valid
                    while (currentIndex > lastValidOption)
                    {
                        // Go to the option in the previous row
                        currentIndex -= totalOptionsPerRow;
                    }

                    // If the option in the previous row is on the previous page, then go to the last valid option
                    if (currentIndex < (currentPage * totalOptionsPerPage))
                    {
                        currentIndex = lastValidOption;
                    }
                }
                else
                {
                    // Go to the previous page
                    currentPage -= 1;

                    // Go to the last option in the current column
                    currentIndex -= totalOptionsPerRow;
                }
            }
            else
            {
                // Go to the previous option in the current column
                currentIndex -= totalOptionsPerRow;
            }
            break;
        }
        default:
        {
            return;
        }
    }

    *currentIndexPtr = static_cast<uint8_t>(currentIndex);

    if (currentPagePtr)
    {
        *currentPagePtr = static_cast<uint8_t>(currentPage);
    }
}

#ifdef TTYD_JP
bool getStageString(char *stageNameBuffer, uint32_t stageNameSize, uint32_t sequencePosition)
{
    // Make sure the sequence position is valid
    if (sequencePosition >= CHEATS_TOTAL_EVENT_NAMES)
    {
        return false;
    }

    if (sequencePosition <= 22)
    {
        // Use snprintf to make sure stageNameSize is not exceeded, and that a null terminator is properly applied
        snprintf(stageNameBuffer, stageNameSize, "Opening");
        return true;
    }

    if ((sequencePosition >= 403) && (sequencePosition <= 405))
    {
        // Use snprintf to make sure stageNameSize is not exceeded, and that a null terminator is properly applied
        snprintf(stageNameBuffer, stageNameSize, "Ending");
        return true;
    }

    uint32_t stageNumber;
    if ((sequencePosition >= 23) && (sequencePosition <= 70))
    {
        stageNumber = 1;
    }
    else if ((sequencePosition >= 71) && (sequencePosition <= 126))
    {
        stageNumber = 2;
    }
    else if ((sequencePosition >= 127) && (sequencePosition <= 177))
    {
        stageNumber = 3;
    }
    else if ((sequencePosition >= 178) && (sequencePosition <= 229))
    {
        stageNumber = 4;
    }
    else if ((sequencePosition >= 230) && (sequencePosition <= 281))
    {
        stageNumber = 5;
    }
    else if ((sequencePosition >= 282) && (sequencePosition <= 351))
    {
        stageNumber = 6;
    }
    else if ((sequencePosition >= 352) && (sequencePosition <= 381))
    {
        stageNumber = 7;
    }
    else // if ((sequencePosition >= 382) && (sequencePosition <= 402))
    {
        stageNumber = 8;
    }

    snprintf(stageNameBuffer, stageNameSize, "Stage %" PRIu32, stageNumber);
    return true;
}
#endif

#ifdef TTYD_JP
bool getSequenceStageAndEvent(char *stageNameBuffer, uint32_t stageNameSize, uint32_t sequencePosition, const char *namesOut[2])
#else
bool getSequenceStageAndEvent(uint32_t sequencePosition, const char *namesOut[2])
#endif
{
    const char *stageName;
    const char *eventName;

#ifdef TTYD_JP
    if (!getStageString(stageNameBuffer, stageNameSize, sequencePosition))
    {
        return false;
    }

    stageName = stageNameBuffer;
    eventName = gCheatsEventNames[sequencePosition];
#else
    const int32_t totalStages = eventStgNum();
    bool foundName = false;

    for (int32_t i = 0; i < totalStages; i++)
    {
        const EventStageDescription *stageDesc = eventStgDtPtr(i);
        const EventStageEventDescription *eventDesc = &stageDesc->pEvents[0];
        const int32_t eventCount = stageDesc->eventCount;

        for (int32_t j = 0; j < eventCount; j++)
        {
            if (eventDesc->gsw0 >= sequencePosition)
            {
                stageName = stageDesc->nameEn;
                eventName = eventDesc->nameEn;
                foundName = true;
                break;
            }

            eventDesc++;
        }

        if (foundName)
        {
            break;
        }
    }

    if (!foundName)
    {
        return false;
    }
#endif

    namesOut[0] = stageName;
    namesOut[1] = eventName;
    return true;
}
