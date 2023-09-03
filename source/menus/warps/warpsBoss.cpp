#include "menuUtils.h"
#include "drawText.h"
#include "cheats.h"
#include "menus/warpsMenu.h"
#include "menus/rootMenu.h"
#include "misc/utils.h"
#include "ttyd/camdrv.h"
#include "ttyd/swdrv.h"
#include "ttyd/party.h"

#include <cstdint>

const MenuOption gWarpsMenuBossOptions[] = {
    "Crump",
    warpsMenuBossSelectWarp,

    "Gus",
    warpsMenuBossSelectWarp,

    "Blooper",
    warpsMenuBossSelectWarp,

    "Gold Fuzzy",
    warpsMenuBossSelectWarp,

    "Red Bones (Ch1)",
    warpsMenuBossSelectWarp,

    "Hooktail",
    warpsMenuBossSelectWarp,

    "Shadow Sirens (Ch2)",
    warpsMenuBossSelectWarp,

    "Magnus Von Grapple",
    warpsMenuBossSelectWarp,

    "Macho Grubba",
    warpsMenuBossSelectWarp,

    "Atomic Boo",
    warpsMenuBossSelectWarp,

    "Doopliss 1",
    warpsMenuBossSelectWarp,

    "Doopliss 2",
    warpsMenuBossSelectWarp,

    "Cortez",
    warpsMenuBossSelectWarp,

    "Crump (Ch5)",
    warpsMenuBossSelectWarp,

    "Smorg",
    warpsMenuBossSelectWarp,

    "Magnus Von Grapple 2.0",
    warpsMenuBossSelectWarp,

    "Dark Bones",
    warpsMenuBossSelectWarp,

    "Gloomtail",
    warpsMenuBossSelectWarp,

    "Shadow Sirens (Ch8)",
    warpsMenuBossSelectWarp,

    "Grodus",
    warpsMenuBossSelectWarp,

    "Bowser & Kammy",
    warpsMenuBossSelectWarp,

    "Shadow Queen (Battle 1)",
    warpsMenuBossSelectWarp,

    "Shadow Queen (Battle 2)",
    warpsMenuBossSelectWarp,

    "Bonetail",
    warpsMenuBossSelectWarp,
};

const MenuFunctions gWarpsMenuBossFuncs = {
    gWarpsMenuBossOptions,
    warpsMenuBossControls,
    warpsMenuBossDraw,
    nullptr, // Exit function not needed
};

void warpsMenuBossInit(Menu *menuPtr)
{
    (void)menuPtr;

    constexpr uint32_t totalOptions = sizeof(gWarpsMenuBossOptions) / sizeof(MenuOption);
    enterNextMenu(&gWarpsMenuBossFuncs, totalOptions);
}

void warpsMenuBossDPadControls(MenuButtonInput button, uint8_t *currentIndexPtr, uint32_t totalOptions)
{
    const uint32_t totalRows = intCeil(totalOptions, WARPS_MENU_BOSS_MAX_OPTIONS_PER_ROW);
    const uint32_t totalOptionsPerPage = totalRows * WARPS_MENU_BOSS_MAX_OPTIONS_PER_ROW;

    menuControlsVertical(button,
                         currentIndexPtr,
                         nullptr,
                         totalOptions,
                         totalOptionsPerPage,
                         WARPS_MENU_BOSS_MAX_OPTIONS_PER_ROW,
                         true);
}

void warpsMenuBossControls(Menu *menuPtr, MenuButtonInput button)
{
    // The function for checking for auto-incrementing needs to run every frame to be handled correctly
    const bool autoIncrement = handleMenuAutoIncrement(gWarpsMenu->getAutoIncrementPtr());
    const uint32_t totalOptions = menuPtr->getTotalOptions();

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
                warpsMenuBossDPadControls(buttonHeld, menuPtr->getCurrentIndexPtr(), totalOptions);
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
            warpsMenuBossDPadControls(button, menuPtr->getCurrentIndexPtr(), totalOptions);
            break;
        }

        // Using the default controls function will cause auto-incrementing to occur twice per frame, so have to manually
        // implement the default controls
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

void WarpsMenu::drawSelectBossWarpInfo() const
{
    // Initialize text drawing
    drawTextInit(false);

    // Get the text position for the top-left of the window
    float tempPosX;
    float tempPosY;
    constexpr float scale = MENU_SCALE;
    gRootWindow->getTextPosXY(nullptr, WindowAlignment::TOP_LEFT, scale, &tempPosX, &tempPosY);

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    float posX = tempPosX;
    float posY = tempPosY;

    // Draw the help text
    const char *helpText = "Note: Using this menu will modify the Sequence position";
    drawText(helpText, posX, posY, scale, getColorWhite(0xFF));

    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    posY -= (lineDecrement * 2.f);

    // Draw each warp option
    const MenuOption *bossOptionsPtr = gWarpsMenuBossOptions;
    constexpr float posXIncrement = 200.f;
    const float posYBase = posY;

    const Menu *menuPtr = gMenu;
    const uint32_t totalOptions = menuPtr->getTotalOptions();
    const uint32_t currentIndex = menuPtr->getCurrentIndex();
    const uint32_t totalWarpRows = intCeil(totalOptions, WARPS_MENU_BOSS_MAX_OPTIONS_PER_ROW);

    for (uint32_t i = 0, counter = 0; i < totalOptions; i++, counter++)
    {
        if (counter >= totalWarpRows)
        {
            counter = 0;

            // Move to the next row
            posX += posXIncrement;
            posY = posYBase;
        }

        const uint32_t color = getCurrentOptionColor(currentIndex == i, 0xFF);
        drawText(bossOptionsPtr[i].name, posX, posY, scale, color);
        posY -= lineDecrement;
    }
}

void warpsMenuBossDraw(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Draw the main window
    drawMainWindow();

    // Draw the info for selecting a warp
    WarpsMenu *warpsMenuPtr = gWarpsMenu;
    warpsMenuPtr->drawSelectBossWarpInfo();

    // Draw an error message if applicable
    ErrorWindow *errorWindowPtr = warpsMenuPtr->getErrorWindowPtr();
    if (errorWindowPtr->shouldDraw())
    {
        errorWindowPtr->draw();
    }
}

uint32_t warpToBoss(Menu *menuPtr)
{
    // Make sure a file is currently loaded and the player is not currently transitioning screens nor in a battle
    if (!checkIfInGame())
    {
        return WarpsMenuWarpToMapReturnValue::WARPS_MENU_WARP_TO_MAP_NOT_IN_GAME;
    }

    // Make sure the index is valid
    const uint32_t currentIndex = menuPtr->getCurrentIndex();
    const uint32_t totalOptions = menuPtr->getTotalOptions();
    if (currentIndex >= totalOptions)
    {
        return WarpsMenuWarpToMapReturnValue::WARPS_MENU_WARP_TO_MAP_INVALID_INDEX;
    }

    // Initialize the default sequence position to an arbitrary invalid value
    int32_t sequencePosition = -1;

    const char *bossMap;
    const char *bossBero = "";

    switch (currentIndex)
    {
        case WarpsMenuBossOptions::WARPS_MENU_BOSS_OPTION_CRUMP_PROLOGUE:
        default:
        {
            sequencePosition = 4;
            bossMap = "gor_00";
            break;
        }
        case WarpsMenuBossOptions::WARPS_MENU_BOSS_OPTION_GUS:
        {
            bossMap = "gor_02";
            bossBero = "w_bero";

            // Turn off GSWF(1213) to allow Gus to be fought again
            swClear(1213);
            break;
        }
        case WarpsMenuBossOptions::WARPS_MENU_BOSS_OPTION_BLOOPER:
        {
            sequencePosition = 20;
            bossMap = "tik_02";
            bossBero = "w_bero_1";
            break;
        }
        case WarpsMenuBossOptions::WARPS_MENU_BOSS_OPTION_GOLD_FUZZY:
        {
            sequencePosition = 31;
            bossMap = "hei_10";

            // Turn on GSWF(1774) and GSWF(1775) to allow the Gold Fuzzy to be fought again
            static constexpr const uint16_t flagsToSet[] = {
                1774,
                1775,
            };

            // Set the flags
            constexpr uint32_t loopCount = sizeof(flagsToSet) / sizeof(flagsToSet[0]);
            for (uint32_t i = 0; i < loopCount; i++)
            {
                swSet(flagsToSet[i]);
            }
            break;
        }
        case WarpsMenuBossOptions::WARPS_MENU_BOSS_OPTION_RED_BONES:
        {
            sequencePosition = 38;
            bossMap = "gon_03";
            bossBero = "e_bero";
            break;
        }
        case WarpsMenuBossOptions::WARPS_MENU_BOSS_OPTION_HOOKTAIL:
        {
            sequencePosition = 54;
            bossMap = "gon_11";

            // Turn off GSWF(1498) to allow Hooktail to be fought again
            swClear(1498);
            break;
        }
        case WarpsMenuBossOptions::WARPS_MENU_BOSS_OPTION_SHADOW_SIRENS_CH2:
        {
            sequencePosition = 84;
            bossMap = "win_00";
            bossBero = "w_bero";
            break;
        }
        case WarpsMenuBossOptions::WARPS_MENU_BOSS_OPTION_MAGNUS_CH2:
        {
            sequencePosition = 110;
            bossMap = "mri_01";
            bossBero = "e_bero"; // Required to avoid a crash
            break;
        }
        case WarpsMenuBossOptions::WARPS_MENU_BOSS_OPTION_MACHO_GRUBBA:
        {
            sequencePosition = 163;
            bossMap = "tou_03";
            bossBero = "w_bero";
            break;
        }
        case WarpsMenuBossOptions::WARPS_MENU_BOSS_OPTION_ATOMIC_BOO:
        {
            sequencePosition = 197;
            bossMap = "jin_00";

            // Turn off GSWF(2226) to allow the Atomic Boo to be fought again
            swClear(2226);
            break;
        }
        case WarpsMenuBossOptions::WARPS_MENU_BOSS_OPTION_DOOPLISS_1:
        {
            sequencePosition = 199;
            bossMap = "jin_04";
            break;
        }
        case WarpsMenuBossOptions::WARPS_MENU_BOSS_OPTION_DOOPLISS_2:
        {
            sequencePosition = 210;
            bossMap = "jin_04";

            // Force Vivian to be the current partner out
            spawnPartnerOrFollower(PartyMembers::kVivian);
            break;
        }
        case WarpsMenuBossOptions::WARPS_MENU_BOSS_OPTION_CORTEZ:
        {
            sequencePosition = 252;
            bossMap = "muj_12";
            break;
        }
        case WarpsMenuBossOptions::WARPS_MENU_BOSS_OPTION_CRUMP_CH5:
        {
            sequencePosition = 259;
            bossMap = "muj_00";
            break;
        }
        case WarpsMenuBossOptions::WARPS_MENU_BOSS_OPTION_SMORG:
        {
            sequencePosition = 331;
            bossMap = "rsh_06_a";
            break;
        }
        case WarpsMenuBossOptions::WARPS_MENU_BOSS_OPTION_MAGNUS_CH8:
        {
            sequencePosition = 372;
            bossMap = "aji_14";
            bossBero = "w_bero";
            break;
        }
        case WarpsMenuBossOptions::WARPS_MENU_BOSS_OPTION_DARK_BONES:
        {
            sequencePosition = 382;
            bossMap = "las_05";
            bossBero = "e_bero_1";
            break;
        }
        case WarpsMenuBossOptions::WARPS_MENU_BOSS_OPTION_GLOOMTAIL:
        {
            sequencePosition = 387;
            bossMap = "las_26";
            break;
        }
        case WarpsMenuBossOptions::WARPS_MENU_BOSS_OPTION_SHADOW_SIRENS_CH8:
        {
            sequencePosition = 390;
            bossMap = "las_09";
            bossBero = "majyorin_evt";
            break;
        }
        case WarpsMenuBossOptions::WARPS_MENU_BOSS_OPTION_GRODUS:
        {
            sequencePosition = 399;
            bossMap = "las_28";
            bossBero = "e_bero";
            break;
        }
        case WarpsMenuBossOptions::WARPS_MENU_BOSS_OPTION_BOWSER_AND_KAMMY:
        {
            sequencePosition = 399;
            bossMap = "las_28";
            bossBero = "koopa_evt";
            break;
        }
        case WarpsMenuBossOptions::WARPS_MENU_BOSS_OPTION_SHADOW_QUEEN_1:
        {
            sequencePosition = 400;
            bossMap = "las_29";
            bossBero = "sekai_yami2";
            break;
        }
        case WarpsMenuBossOptions::WARPS_MENU_BOSS_OPTION_SHADOW_QUEEN_2:
        {
            sequencePosition = 400;
            bossMap = "las_29";
            bossBero = "minnnanokoe";
            break;
        }
        case WarpsMenuBossOptions::WARPS_MENU_BOSS_OPTION_BONETAIL:
        {
            bossMap = "jon_06";
            bossBero = "dokan_2";

            // Set the Pit level for Bonetail
            setCurrentPitLevel(100);

            // Turn off GSWF(5084) and GSWF(5085) to allow Bonetail to be fought again
            static constexpr const uint16_t flagsToSet[] = {
                5084,
                5085,
            };

            // Set the flags
            constexpr uint32_t loopCount = sizeof(flagsToSet) / sizeof(flagsToSet[0]);
            for (uint32_t i = 0; i < loopCount; i++)
            {
                swClear(flagsToSet[i]);
            }
            break;
        }
    }

    // Bring out a partner if one is not currently out
    // Do not bring a partner out for the first Crump fight
    if ((currentIndex != WarpsMenuBossOptions::WARPS_MENU_BOSS_OPTION_CRUMP_PROLOGUE) && !getPartnerPtr())
    {
        spawnFailsafePartnerOrFollower(true);
    }

    // Set the new map, bero, and sequence position
    setNextMap(bossMap);
    setNextBero(bossBero);

    // Make sure the sequence position should be changed
    if (sequencePosition >= 0)
    {
        setSequencePosition(static_cast<uint32_t>(sequencePosition));
    }

    // Warp to the boss
    reloadRoomMain();

    return WarpsMenuWarpToMapReturnValue::WARPS_MENU_WARP_TO_MAP_SUCCESS;
}

void warpsMenuBossSelectWarp(Menu *menuPtr)
{
    switch (warpToBoss(menuPtr))
    {
        case WarpsMenuWarpToMapReturnValue::WARPS_MENU_WARP_TO_MAP_SUCCESS:
        {
            // Warped successfully, so close the menu
            return closeAllMenus();
        }
        case WarpsMenuWarpToMapReturnValue::WARPS_MENU_WARP_TO_MAP_NOT_IN_GAME:
        {
            gWarpsMenu->initErrorWindow(gWarpsMenuCannotWarpText);
            break;
        }
        case WarpsMenuWarpToMapReturnValue::WARPS_MENU_WARP_TO_MAP_INVALID_INDEX:
        default:
        {
            break;
        }
    }
}
