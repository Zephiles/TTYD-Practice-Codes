#include "menuUtils.h"
#include "mod.h"
#include "drawText.h"
#include "classes/window.h"
#include "menus/warpsMenu.h"
#include "menus/rootMenu.h"
#include "ttyd/camdrv.h"
#include "ttyd/seq_mapchange.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

const MenuFunctions gWarpsMenuIndexViewEntrancesFuncs = {
    nullptr, // There are no menu options to select in this menu
    warpsMenuIndexViewEntrancesControls,
    warpsMenuIndexViewEntrancesDraw,
    nullptr, // Exit function not needed
};

void warpsMenuIndexViewEntrancesInit(Menu *menuPtr)
{
    (void)menuPtr;

    enterNextMenu(&gWarpsMenuIndexViewEntrancesFuncs,
                  gMod->getWarpByIndexPtr()->getTotalEntrances(),
                  WARPS_INDEX_MAX_ENTRANCES_MULTIPLE_PAGES);
}

void warpsMenuIndexViewEntrancesControls(Menu *menuPtr, MenuButtonInput button)
{
    const uint32_t totalEntrances = gMod->getWarpByIndexPtr()->getTotalEntrances();
    const uint32_t currentPage = menuPtr->getCurrentPage();

    switch (button)
    {
        case MenuButtonInput::DPAD_DOWN:
        {
            // If there is only one page, then do nothing
            if (totalEntrances <= WARPS_INDEX_MAX_ENTRANCES_SINGLE_PAGE)
            {
                break;
            }

            // Make sure entrances exist on the next page
            const uint32_t nextPage = currentPage + 1;
            if ((WARPS_INDEX_MAX_ENTRANCES_MULTIPLE_PAGES * nextPage) < totalEntrances)
            {
                // Go to the next page
                menuPtr->setCurrentPage(nextPage);
            }
            else
            {
                // No entrances on the next page, so go back to the first page
                menuPtr->setCurrentPage(0);
            }
            break;
        }
        case MenuButtonInput::DPAD_UP:
        {
            // If there is only one page, then do nothing
            if (totalEntrances <= WARPS_INDEX_MAX_ENTRANCES_SINGLE_PAGE)
            {
                break;
            }

            // Make sure the current page isn't already at 0
            if (currentPage > 0)
            {
                // Go to the previous page
                menuPtr->setCurrentPage(currentPage - 1);
            }
            else
            {
                // On the first page, so go to the last page
                menuPtr->setCurrentPage(totalEntrances / WARPS_INDEX_MAX_ENTRANCES_MULTIPLE_PAGES);
            }
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

void warpsMenuIndexViewEntrancesDraw(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Draw the main window
    drawMainWindow();

    // Initialize text drawing
    drawTextInit(false);

    // Get the position of the help text, which changes depending on if there is more than one page of entrances
    WarpByIndex *warpByIndexPtr = gMod->getWarpByIndexPtr();
    const uint32_t totalEntrances = warpByIndexPtr->getTotalEntrances();
    const char *helpText;

    const bool moreThanOnePage = totalEntrances > WARPS_INDEX_MAX_ENTRANCES_SINGLE_PAGE;
    if (moreThanOnePage)
    {
        helpText = "Press D-Pad Up/Down to adjust the page\nPress B to close this window";
    }
    else
    {
        helpText = "Press B to close this window";
    }

    // Get the position for the top-center of the window
    const Window *rootWindowPtr = gRootWindow;
    constexpr float scale = MENU_SCALE;

    float tempPosX;
    float tempPosY;
    rootWindowPtr->getTextPosXY(helpText, WindowAlignment::TOP_CENTER, scale, &tempPosX, &tempPosY);

    // Draw the help text at the top-center of the window
    drawText(helpText, tempPosX, tempPosY, scale, getColorWhite(0xFF));

    // If there is more than one page of entrances, then decrement posY by an additional line
    constexpr float lineDecrement = MENU_LINE_HEIGHT * scale;
    float posY = tempPosY;

    if (moreThanOnePage)
    {
        posY -= lineDecrement;
    }

    // Get the text position two lines under the help text
    posY -= (lineDecrement * 2.f);
    const float posX = rootWindowPtr->getTextPosX(nullptr, WindowAlignment::TOP_LEFT, scale);

    // Draw the labels for the current map and entrance
    const char *mapsAndEntrancesLabels = "Current Map\nCurrent Entrance";
    drawText(mapsAndEntrancesLabels, posX, posY, scale, getColorWhite(0xFF));

    // Get the text and color for the current map and entrance
    MapAndBeroDetails mapAndBeroDetails;
    getMapAndBeroTextAndColor(_next_map, _next_bero, &mapAndBeroDetails);

    // Set up the text and color for the current map and entrance
    char buf[64];
    snprintf(buf,
             sizeof(buf),
             "<col %" PRIx32 ">%s\n<col %" PRIx32 ">%s",
             mapAndBeroDetails.mapColor,
             mapAndBeroDetails.mapTextPtr,
             mapAndBeroDetails.beroColor,
             mapAndBeroDetails.beroTextPtr);

    // Get the width that is being used by the labels
    float textWidth = getTextWidth(mapsAndEntrancesLabels, scale);

    // Set the text to be a bit to the right of the labels
    constexpr float incrementAmount = 30.f * scale;
    float posXIncrement = textWidth + incrementAmount;

    // Draw the text and color for the current map and entrance
    drawText(buf, posX + posXIncrement, posY, scale, getColorWhite(0xFF));

    // Make a backup of the current posY value, as the page number will need it if will be drawn
    const float pagePosY = posY;
    posY -= (lineDecrement * 3.f);

    // Make sure at least one entrance exists for the current map
    if (totalEntrances == 0)
    {
        drawText("No entrances exist for this map", posX, posY, scale, getColorWhite(0xFF));
        return;
    }

    // Draw the page number at the right of the window if there is more than one page of entrances
    const uint32_t currentPage = gMenu->getCurrentPage();
    if (moreThanOnePage)
    {
        // Draw the page as an int, to prevent long text if it somehow becomes negative
        snprintf(buf, sizeof(buf), "Page %" PRId32, currentPage + 1);

        const float pagePosX = rootWindowPtr->getTextPosX(buf, WindowAlignment::TOP_RIGHT, scale);
        drawText(buf, pagePosX, pagePosY, scale, getColorWhite(0xFF));
    }

    // Draw the entrances
    // Get the width that is being used by the entrance number text
    const char *entranceTextBase;
    if (moreThanOnePage)
    {
        entranceTextBase = "Entrance 00";
    }
    else
    {
        entranceTextBase = "Entrance 0";
    }

    textWidth = getTextWidth(entranceTextBase, scale);

    // Set the text to be a bit to the right of the entrance number text
    posXIncrement = textWidth + incrementAmount;

    const uint32_t startingIndex = WARPS_INDEX_MAX_ENTRANCES_MULTIPLE_PAGES * currentPage;
    const uint32_t endingIndex = startingIndex + WARPS_INDEX_MAX_ENTRANCES_MULTIPLE_PAGES;
    const char **entranceListPtr = warpByIndexPtr->getEntranceListPtr();

    for (uint32_t i = startingIndex; i < endingIndex; i++)
    {
        if (i >= totalEntrances)
        {
            // Reached the end of the array
            break;
        }

        // Draw the entrance number
        snprintf(buf, sizeof(buf), "Entrance %" PRIu32, i + 1);
        drawText(buf, posX, posY, scale, getColorWhite(0xFF));

        // Draw the entrance name
        drawText(entranceListPtr[i], posX + posXIncrement, posY, scale, getColorWhite(0xFF));
        posY -= lineDecrement;
    }
}
