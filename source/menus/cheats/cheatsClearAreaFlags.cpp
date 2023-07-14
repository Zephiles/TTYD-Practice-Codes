#include "menuUtils.h"
#include "drawText.h"
#include "cheats.h"
#include "classes/window.h"
#include "menus/cheatsMenu.h"
#include "misc/utils.h"
#include "ttyd/camdrv.h"
#include "ttyd/swdrv.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

const MenuOption gCheatsMenuClearAreaFlagsOptions[] = {
    "Select Area",
    cheatsMenuClearAreaFlagsStartSelectingArea,

    "Clear Flags",
    cheatsMenuClearAreaFlagsSelectedClearFlags,
};

const MenuFunctions gCheatsMenuClearAreaFlagsFuncs = {
    gCheatsMenuClearAreaFlagsOptions,
    cheatsMenuClearAreaFlagsControls,
    cheatsMenuClearAreaFlagsDraw,
    nullptr, // Exit function not needed
};

void cheatsMenuClearAreaFlagsInit(Menu *menuPtr)
{
    (void)menuPtr;

    // Make sure the current index is valid
    gCheatsMenu->setCurrentIndex(0);

    constexpr uint32_t totalOptions = sizeof(gCheatsMenuClearAreaFlagsOptions) / sizeof(MenuOption);
    enterNextMenu(&gCheatsMenuClearAreaFlagsFuncs, totalOptions);
}

void cheatsMenuClearAreaFlagsSelectAreaDPadControls(MenuButtonInput button, uint8_t *currentIndexPtr)
{
    constexpr uint32_t totalRows = intCeil(CHEATS_TOTAL_AREAS, CHEATS_AREA_FLAGS_MAX_OPTIONS_PER_ROW);
    constexpr uint32_t totalOptionsPerPage = totalRows * CHEATS_AREA_FLAGS_MAX_OPTIONS_PER_ROW;

    menuControlsVertical(button,
                         currentIndexPtr,
                         nullptr,
                         CHEATS_TOTAL_AREAS,
                         totalOptionsPerPage,
                         CHEATS_AREA_FLAGS_MAX_OPTIONS_PER_ROW,
                         false);
}

void cheatsMenuClearAreaFlagsControls(Menu *menuPtr, MenuButtonInput button)
{
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;

    // If the confirmation window is open, then handle the controls for that
    if (menuPtr->flagIsSet(CheatsMenuClearAreaFlags::CHEATS_CLEAR_AREA_FLAGS_FLAG_CURRENTLY_SELECTING_YES_NO))
    {
        cheatsMenuPtr->getConfirmationWindow()->controls(button);
        return;
    }

    // If not selecting an area, then use the default controls
    if (!menuPtr->flagIsSet(CheatsMenuClearAreaFlags::CHEATS_CLEAR_AREA_FLAGS_FLAG_CURRENTLY_SELECTING_AREA))
    {
        controlsBasicMenuLayout(menuPtr, button);
        return;
    }

    // Make sure the current index is valid
    const uint32_t currentIndex = cheatsMenuPtr->getCurrentIndex();
    if (currentIndex >= CHEATS_TOTAL_AREAS)
    {
        // Failsafe: Reset the current index to 0
        cheatsMenuPtr->setCurrentIndex(0);
        return;
    }

    // Handle the controls for selecting an area
    // The function for checking for auto-incrementing needs to run every frame to be handled correctly
    const bool shouldAutoIncrement = handleMenuAutoIncrement(cheatsMenuPtr->getAutoIncrementPtr());

    // Handle held button inputs if auto-incrementing should be done
    if (shouldAutoIncrement)
    {
        const MenuButtonInput buttonHeld = getMenuButtonInput(false);
        switch (buttonHeld)
        {
            case MenuButtonInput::DPAD_LEFT:
            case MenuButtonInput::DPAD_RIGHT:
            case MenuButtonInput::DPAD_DOWN:
            case MenuButtonInput::DPAD_UP:
            {
                cheatsMenuClearAreaFlagsSelectAreaDPadControls(buttonHeld, cheatsMenuPtr->getCurrentIndexPtr());
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
            cheatsMenuClearAreaFlagsSelectAreaDPadControls(button, cheatsMenuPtr->getCurrentIndexPtr());
            break;
        }
        case MenuButtonInput::A:
        {
            Cheats *cheatsPtr = gCheats;
            cheatsPtr->clearMiscFlag(CheatsMiscFlag::CHEATS_MISC_FLAG_AREA_FLAGS_SHOULD_BE_CLEARED);
            cheatsPtr->getClearAreaFlagsCheat()->setSelectedArea(currentIndex);

            menuPtr->clearFlag(CheatsMenuClearAreaFlags::CHEATS_CLEAR_AREA_FLAGS_FLAG_CURRENTLY_SELECTING_AREA);
            break;
        }
        case MenuButtonInput::B:
        {
            menuPtr->clearFlag(CheatsMenuClearAreaFlags::CHEATS_CLEAR_AREA_FLAGS_FLAG_CURRENTLY_SELECTING_AREA);
            break;
        }
        default:
        {
            break;
        }
    }
}

void CheatsMenu::drawClearAreaFlagsInfo() const
{
    // Set the the current area string
    char buf[32];
    constexpr uint32_t bufSize = sizeof(buf);

    const auto areaNames = gCheatsAreaNames;
    const uint32_t selectedArea = gCheats->getClearAreaFlagsCheat()->getSelectedArea();

    const char **currentArea = areaNames[selectedArea];
    snprintf(buf, bufSize, "Current Area: %s", currentArea[0]);

    // Get the text position for the top-left of the window two lines under the main text
    const Menu *menuPtr = gMenu;
    const float scale = this->scale;
    const uint32_t totalOptions = menuPtr->getTotalOptions();

    float tempPosX;
    float tempPosY;
    gRootWindow->getTextPosXYUnderMainText(nullptr, WindowAlignment::TOP_LEFT, totalOptions, 2, scale, &tempPosX, &tempPosY);

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    const float posXBase = tempPosX;
    float posY = tempPosY;

    // Draw the current area
    drawText(buf, posXBase, posY, scale, getColorWhite(0xFF));

    const float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    posY -= (lineDecrement * 2.f);

    // Get the width of one of the areas and set the width increment for the areas
    float width;
    getTextWidthHeight(areaNames[0][0], scale, &width, nullptr);
    const float incrementWidth = width + (40.f * scale);

    // Draw each area to choose from
    constexpr uint32_t totalRows = intCeil(CHEATS_TOTAL_AREAS, CHEATS_AREA_FLAGS_MAX_OPTIONS_PER_ROW);
    const uint32_t currentIndex = this->currentIndex;

    float posX = posXBase;
    const float posYBase = posY;

    const bool currentlySelectingArea =
        menuPtr->flagIsSet(CheatsMenuClearAreaFlags::CHEATS_CLEAR_AREA_FLAGS_FLAG_CURRENTLY_SELECTING_AREA);

    for (uint32_t i = 0, j = 0; i < CHEATS_TOTAL_AREAS; i++, j++)
    {
        if (j >= totalRows)
        {
            j = 0;

            // Move to the next column
            posX += incrementWidth;
            posY = posYBase;
        }

        const bool colorCheck = currentlySelectingArea && (currentIndex == i);
        const uint32_t color = getCurrentOptionColor(colorCheck, 0xFF);

        drawText(areaNames[i][0], posX, posY, scale, color);
        posY -= lineDecrement;
    }

    // If currently selecting an area, then draw the description for the current option
    if (currentlySelectingArea)
    {
        posY = posYBase - (lineDecrement * intToFloat(totalRows + 1));
        drawText(areaNames[currentIndex][1], posXBase, posY, scale, getColorWhite(0xFF));
    }
}

void cheatsMenuClearAreaFlagsDraw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    drawBasicMenuLayout(cameraId, user);

    // Draw the info for the areas
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;
    cheatsMenuPtr->drawClearAreaFlagsInfo();

    // Draw the confirmation window if applicable
    ConfirmationWindow *confirmationWindowPtr = cheatsMenuPtr->getConfirmationWindow();
    if (confirmationWindowPtr->shouldDraw())
    {
        confirmationWindowPtr->draw();
    }
}

void cheatsMenuClearAreaFlagsStartSelectingArea(Menu *menuPtr)
{
    menuPtr->setFlag(CheatsMenuClearAreaFlags::CHEATS_CLEAR_AREA_FLAGS_FLAG_CURRENTLY_SELECTING_AREA);
}

void cheatsMenuClearAreaFlagsClearFlags(bool selectedYes)
{
    if (selectedYes)
    {
        gCheats->setMiscFlag(CheatsMiscFlag::CHEATS_MISC_FLAG_AREA_FLAGS_SHOULD_BE_CLEARED);
    }

    // Close the confirmation window
    gCheatsMenu->getConfirmationWindow()->stopDrawing();
    gMenu->clearFlag(CheatsMenuClearAreaFlags::CHEATS_CLEAR_AREA_FLAGS_FLAG_CURRENTLY_SELECTING_YES_NO);
}

void cheatsMenuClearAreaFlagsSelectedClearFlags(Menu *menuPtr)
{
    // Bring up the confirmation window
    menuPtr->setFlag(CheatsMenuClearAreaFlags::CHEATS_CLEAR_AREA_FLAGS_FLAG_CURRENTLY_SELECTING_YES_NO);

    // Initialize the confirmation window
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;
    ConfirmationWindow *confirmationWindowPtr = cheatsMenuPtr->getConfirmationWindow();

    const char *helpText =
        "This will clear all of the GSWF's for\nthe selected area when the area is\nreloaded.\n\nAre you sure you want to do "
        "this?";

    const Window *rootWindowPtr = gRootWindow;

    confirmationWindowPtr->init(rootWindowPtr, helpText, cheatsMenuPtr->getScale(), rootWindowPtr->getAlpha());
    confirmationWindowPtr->startDrawing(cheatsMenuClearAreaFlagsClearFlags);
}

void clearGSWFsRange(uint32_t lowerBound, uint32_t upperBound)
{
    for (uint32_t i = lowerBound; i <= upperBound; i++)
    {
        swClear(i);
    }
}

void clearAreaFlags(uint32_t index)
{
    switch (index)
    {
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_GOR:
        {
            // Clear the regular flags
            clearGSWFsRange(1175, 1250);

            // Clear the Shine Sprites
            clearGSWFsRange(5525, 5529);

            // Clear the Star Pieces
            clearGSWFsRange(5567, 5584);

            // Clear misc flags not part of the standard gor memory area
            static constexpr const uint8_t flagsToClear[] = {
                9,  // Post-Chapter 3 Koopie Koo Email
                10, // Post-Chapter 4 Petuni Email
                11, // Email from Boo after Chapter 5 Intermission
                14, // Real Zip Toad Email after Chapter 6
                20, // RDM #4 Email after Chapter 5
                23, // Koopook Email after Chapter 2
                24, // Email from Pine T. Jr. after Chapter 3
                25, // Email from Jolene after Zess Trouble
                26, // Email from Bub-ulber after Mowz Trouble
                27, // Eve Email after Chapter 6
            };

            constexpr uint32_t loopCount = sizeof(flagsToClear) / sizeof(flagsToClear[0]);
            for (uint32_t i = 0; i < loopCount; i++)
            {
                swClear(flagsToClear[i]);
            }
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_TIK:
        {
            // Clear the regular flags
            clearGSWFsRange(1325, 1371);

            // Clear the Shine Sprites
            clearGSWFsRange(5530, 5537);

            // Clear the Star Pieces
            clearGSWFsRange(5585, 5597);

            // Clear misc flags not part of the standard tik memory area
            static constexpr const uint8_t flagsToClear[] = {
                16, // RDM #7 Email after Chapter 7
            };

            constexpr uint32_t loopCount = sizeof(flagsToClear) / sizeof(flagsToClear[0]);
            for (uint32_t i = 0; i < loopCount; i++)
            {
                swClear(flagsToClear[i]);
            }
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_HEI:
        {
            // Clear the regular flags
            clearGSWFsRange(1774, 1806);

            // Clear the Star Pieces
            clearGSWFsRange(5598, 5600);
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_NOK:
        {
            // Clear the regular flags
            clearGSWFsRange(1624, 1629);

            // Clear the Star Pieces
            clearGSWFsRange(5601, 5602);

            // Clear misc flags not part of the standard nok memory area
            static constexpr const uint8_t flagsToClear[] = {
                7, // Frankly Email at right side of town
            };

            constexpr uint32_t loopCount = sizeof(flagsToClear) / sizeof(flagsToClear[0]);
            for (uint32_t i = 0; i < loopCount; i++)
            {
                swClear(flagsToClear[i]);
            }
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_GON:
        {
            // Clear the regular flags
            clearGSWFsRange(1476, 1511);

            // Clear the Shine Sprites
            clearGSWFsRange(5538, 5540);

            // Clear the Star Pieces
            clearGSWFsRange(5603, 5607);
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_WIN:
        {
            // Clear the regular flags
            clearGSWFsRange(2675, 2687);

            // Clear the Shine Sprites
            clearGSWFsRange(5541, 5541);

            // Clear the Star Pieces
            clearGSWFsRange(5608, 5611);

            // Clear misc flags not part of the standard win memory area
            static constexpr const uint8_t flagsToClear[] = {
                17, // RDM #1 Email after getting Flurrie
            };

            constexpr uint32_t loopCount = sizeof(flagsToClear) / sizeof(flagsToClear[0]);
            for (uint32_t i = 0; i < loopCount; i++)
            {
                swClear(flagsToClear[i]);
            }
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_MRI:
        {
            // Clear the regular flags
            clearGSWFsRange(2825, 2885);

            // Clear the Shine Sprites
            clearGSWFsRange(5542, 5545);

            // Clear the Star Pieces
            clearGSWFsRange(5612, 5617);
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_TOU:
        {
            // Clear the regular flags
            clearGSWFsRange(2374, 2533);

            // Clear the Shine Sprites
            clearGSWFsRange(5546, 5547);

            // Clear the Star Pieces
            clearGSWFsRange(5618, 5627);

            // Clear misc flags not part of the standard tou memory area
            static constexpr const uint8_t flagsToClear[] = {
                18,  // RDM #2 Email after getting Egg
                250, // RDM #2 Email after getting Egg
                251, // RDM #4 Email after Chapter 5
            };

            constexpr uint32_t loopCount = sizeof(flagsToClear) / sizeof(flagsToClear[0]);
            for (uint32_t i = 0; i < loopCount; i++)
            {
                swClear(flagsToClear[i]);
            }
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_USU:
        {
            // Clear the regular flags
            clearGSWFsRange(1925, 1939);

            // Clear the Star Pieces
            clearGSWFsRange(5628, 5630);
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_GRA:
        {
            // Clear the regular flags
            clearGSWFsRange(2075, 2091);

            // Clear the Shine Sprites
            clearGSWFsRange(5548, 5548);

            // Clear the Star Pieces
            clearGSWFsRange(5631, 5633);
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_JIN:
        {
            // Clear the regular flags
            clearGSWFsRange(2226, 2241);

            // Clear the Shine Sprites
            clearGSWFsRange(5549, 5551);

            // Clear the Star Pieces
            clearGSWFsRange(5634, 5637);
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_MUJ:
        {
            // Clear the regular flags
            clearGSWFsRange(3126, 3158);

            // Clear the Shine Sprites
            clearGSWFsRange(5552, 5553);

            // Clear the Star Pieces
            clearGSWFsRange(5638, 5643);
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_DOU:
        {
            // Clear the regular flags
            clearGSWFsRange(2975, 2994);

            // Clear the Shine Sprites
            clearGSWFsRange(5554, 5558);

            // Clear the Star Pieces
            clearGSWFsRange(5644, 5647);
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_HOM:
        {
            // Clear the regular flags
            clearGSWFsRange(3574, 3575);

            // Clear misc flags not part of the standard hom memory area
            static constexpr const uint8_t flagsToClear[] = {
                13, // King K. Email after fixing drawbridge
            };

            constexpr uint32_t loopCount = sizeof(flagsToClear) / sizeof(flagsToClear[0]);
            for (uint32_t i = 0; i < loopCount; i++)
            {
                swClear(flagsToClear[i]);
            }
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_RSH:
        {
            // Clear the regular flags
            clearGSWFsRange(3425, 3462);

            // Clear the Shine Sprites
            clearGSWFsRange(5559, 5560);

            // Clear the Star Pieces
            clearGSWFsRange(5648, 5652);

            // Clear misc flags not part of the standard rsh memory area
            static constexpr const uint8_t flagsToClear[] = {
                12, // Email from Pa-Patch on Day 3
                21, // RDM #5 Email on second day
            };

            constexpr uint32_t loopCount = sizeof(flagsToClear) / sizeof(flagsToClear[0]);
            for (uint32_t i = 0; i < loopCount; i++)
            {
                swClear(flagsToClear[i]);
            }
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_EKI:
        {
            // Clear the regular flags
            clearGSWFsRange(3725, 3754);

            // Clear the Shine Sprites
            clearGSWFsRange(5561, 5562);

            // Clear the Star Pieces
            clearGSWFsRange(5653, 5653);
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_PIK:
        {
            // Clear the regular flags
            clearGSWFsRange(3276, 3279);

            // Clear the Shine Sprites
            clearGSWFsRange(5563, 5564);

            // Clear the Star Pieces
            clearGSWFsRange(5654, 5657);

            // Clear misc flags not part of the standard pik memory area
            static constexpr const uint8_t flagsToClear[] = {
                22,  // RDM #6 Email after Goldbob cannon
                252, // RDM #6 Email after Goldbob cannon
            };

            constexpr uint32_t loopCount = sizeof(flagsToClear) / sizeof(flagsToClear[0]);
            for (uint32_t i = 0; i < loopCount; i++)
            {
                swClear(flagsToClear[i]);
            }
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_BOM:
        {
            // Clear the regular flags
            clearGSWFsRange(3874, 3892);

            // Clear the Shine Sprites
            clearGSWFsRange(5565, 5566);

            // Clear the Star Pieces
            clearGSWFsRange(5658, 5663);

            // Clear misc flags not part of the standard bom memory area
            static constexpr const uint8_t flagsToClear[] = {
                15, // Rawk Hawk Email after General White quest
            };

            constexpr uint32_t loopCount = sizeof(flagsToClear) / sizeof(flagsToClear[0]);
            for (uint32_t i = 0; i < loopCount; i++)
            {
                swClear(flagsToClear[i]);
            }
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_MOO:
        {
            // Clear the regular flags
            clearGSWFsRange(4025, 4039);

            // Clear the Star Pieces
            clearGSWFsRange(5664, 5664);
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_AJI:
        {
            // Clear the regular flags
            clearGSWFsRange(4175, 4217);

            // Clear the Star Pieces
            clearGSWFsRange(5665, 5666);
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_LAS:
        {
            // Clear the regular flags
            clearGSWFsRange(4326, 4394);
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_JON:
        {
            // Clear the regular flags
            clearGSWFsRange(5075, 5085);
            break;
        }
        default:
        {
            break;
        }
    }
}
