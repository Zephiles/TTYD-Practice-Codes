#include "menuUtils.h"
#include "classes/menu.h"
#include "gc/pad.h"
#include "menus/root.h"
#include "misc/utils.h"
#include "ttyd/system.h"
#include "ttyd/win_main.h"

#include <cstdint>

Menu *gMenu = nullptr;

void enterNextMenu(const MenuOption *options, const MenuFunctions *funcs, uint32_t totalOptions)
{
    enterNextMenu(options, funcs, totalOptions, totalOptions);
}

void enterNextMenu(const MenuOption *options, const MenuFunctions *funcs, uint32_t totalOptions, uint32_t totalOptionsPerPage)
{
    Menu *nextMenu = new Menu(options, funcs, totalOptions, totalOptionsPerPage);
    nextMenu->setPrevMenu(gMenu);
    gMenu = nextMenu;
}

void enterPrevMenu()
{
    // Make sure there is at least one menu
    Menu *menuPtr = gMenu;
    if (!menuPtr)
    {
        return;
    }

    menuPtr->runExitFunc();
    Menu *prevMenu = menuPtr->getPrevMenu();
    delete menuPtr;

    gMenu = prevMenu;
}

void closeAllMenus()
{
    while (gMenu)
    {
        enterPrevMenu();
    }
}

bool handleMenuAutoIncrementDigit(uint16_t *waitFramesToBeginPtr, bool *shouldIncrementNowPtr)
{
    // Check to see if any of the D-Pad buttons are held
    constexpr uint32_t buttons =
        PadInput::PAD_DPAD_LEFT | PadInput::PAD_DPAD_RIGHT | PadInput::PAD_DPAD_DOWN | PadInput::PAD_DPAD_UP;

    if (!(keyGetButton(PadId::CONTROLLER_ONE) & buttons))
    {
        // Reset the counter and bool
        *waitFramesToBeginPtr = 0;
        *shouldIncrementNowPtr = false;
        return false;
    }

    // Check to see if the value should begin to auto-increment
    uint32_t waitFramesToBegin = *waitFramesToBeginPtr;
    if (waitFramesToBegin < sysMsec2Frame(500))
    {
        *waitFramesToBeginPtr = static_cast<uint16_t>(++waitFramesToBegin);
        return false;
    }

    // Check to see if the number should increment or not
    if (!*shouldIncrementNowPtr)
    {
        *shouldIncrementNowPtr = true;
        return false;
    }

    // Auto-increment the value
    *shouldIncrementNowPtr = false;
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

    uint32_t counter = 1;

    for (uint32_t i = 0; i < 13; i++)
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

        counter++;
    }

    // Return 0 if no input was found
    return MenuButtonInput::BUTTON_NONE;
}

void controlsBasicMenuLayout(Menu *menuPtr, MenuButtonInput button)
{
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

void drawBasicMenuLayout(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Draw the main window
    gRootWindow->draw();

    // Draw the main text
    gMenu->basicLayoutDraw(gRoot->getScale());
}

void handleMenu()
{
    constexpr uint32_t openMenuButtonCombo = PadInput::PAD_L | PadInput::PAD_START;

    // Check if the menu is being manually opened/closed via the button combo
    if (checkButtonCombo(openMenuButtonCombo))
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

    const uint32_t totalRowsPerPage = 1 + ((totalOptionsPerPage - 1) / totalOptionsPerRow); // Round up
    const uint32_t totalColumns = 1 + ((totalOptionsPerPage - 1) / totalRowsPerPage);       // Round up
    const uint32_t columnSplitAmount = 1 + ((totalOptionsPerPage - 1) / totalColumns);      // Round up

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
                    const uint32_t lastPage = (1 + ((totalOptions - 1) / totalOptionsPerPage)) - 1;
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
    const uint32_t lastPage = (1 + ((totalOptions - 1) / totalOptionsPerPage)) - 1;

    const uint32_t totalRows = 1 + ((totalOptions - 1) / totalOptionsPerRow); // Round up
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