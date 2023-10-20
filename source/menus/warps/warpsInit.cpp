#include "menuUtils.h"
#include "cxx.h"
#include "drawText.h"
#include "mod.h"
#include "cheats.h"
#include "menus/warpsMenu.h"
#include "menus/rootMenu.h"
#include "misc/utils.h"
#include "ttyd/camdrv.h"
#include "ttyd/swdrv.h"
#include "ttyd/seq_mapchange.h"

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cinttypes>

static void controls(Menu *menuPtr, MenuButtonInput button);
static void draw(CameraId cameraId, void *user);
static void exit();
static void selectedOptionSelectWarp(Menu *menuPtr);

static const char *gDestinations[] = {
    "gor_01",   "tik_00", "hei_00", "gon_00", "win_06", "mri_01", "tou_02", "tou_05", "usu_00", "jin_00", "muj_01", "dou_00",
    "rsh_02_a", "eki_00", "pik_00", "bom_00", "moo_00", "aji_00", "aji_13", "las_00", "las_09", "las_27", "jon_0x", "title",
};

static const char *gDestinationsDescriptions[] = {
    "Rogueport Central",
    "Rogueport Sewers - Underground Shop",
    "Petal Meadows - Entrance",
    "Hooktail Castle - Entrance",
    "Boggly Woods - Entrance",
    "The Great Tree - Entrance",
    "Glitzville - Lobby",
    "Glitzville - Grubba's Office",
    "Twilight Town - West Side",
    "Creepy Steeple - Entrance",
    "Keelhaul Key - Shantytown",
    "Pirate's Grotto - Entrance",
    "Excess Express - Passenger Car - Cabins 3-5 - Day",
    "Riverside Station - Entrance",
    "Poshley Heights - Station",
    "Fahr Outpost - Pipe Entrance",
    "Moon - Entrance",
    "X-Naut Fortress - Entrance",
    "X-Naut Fortress - Factory",
    "Palace of Shadow - Entrance",
    "Palace of Shadow - Outside Riddle Tower",
    "Palace of Shadow - Room Before Grodus",
    "Pit of 100 Trials - You must select a level when this\noption is chosen",
    "Title Screen",
};

const char *gWarpsMenuCannotWarpText = "To warp, you must have a file loaded and\nnot be in a battle nor a screen transition.";

WarpsMenu *gWarpsMenu = nullptr;

static const MenuOption gOptions[] = {
    "Select Warp",
    selectedOptionSelectWarp,

    "Warp By Event",
    warpsMenuEventInit,

    "Warp By Index",
    warpsMenuIndexInit,

    "Warp To Boss",
    warpsMenuBossInit,

    "Custom States",
    warpsMenuCustomStatesInit,
};

static const MenuFunctions gFuncs = {
    gOptions,
    controls,
    draw,
    exit,
};

void warpsMenuInit(Menu *menuPtr)
{
    (void)menuPtr;

    // Failsafe: Make sure memory isn't already allocated for gWarpsMenu
    WarpsMenu *warpsMenuPtr = gWarpsMenu;
    if (warpsMenuPtr)
    {
        delete warpsMenuPtr;
    }

    warpsMenuPtr = new WarpsMenu;
    gWarpsMenu = warpsMenuPtr;

    constexpr uint32_t totalOptions = sizeof(gOptions) / sizeof(MenuOption);
    enterNextMenu(&gFuncs, totalOptions);
}

static void flagSetControls(MenuButtonInput button, uint8_t *currentIndexPtr)
{
    constexpr uint32_t totalOptions = sizeof(gDestinations) / sizeof(const char *);
    constexpr uint32_t totalRows = intCeil(totalOptions, WARPS_MENU_INIT_MAX_OPTIONS_PER_ROW);
    constexpr uint32_t totalOptionsPerPage = totalRows * WARPS_MENU_INIT_MAX_OPTIONS_PER_ROW;

    menuControlsVertical(button,
                         currentIndexPtr,
                         nullptr,
                         totalOptions,
                         totalOptionsPerPage,
                         WARPS_MENU_INIT_MAX_OPTIONS_PER_ROW,
                         true);
}

static void controls(Menu *menuPtr, MenuButtonInput button)
{
    WarpsMenu *warpsMenuPtr = gWarpsMenu;

    // If the value editor is open, then handle the controls for that
    ValueEditor *valueEditorPtr = warpsMenuPtr->getValueEditorPtr();
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
    const bool autoIncrement = handleMenuAutoIncrement(warpsMenuPtr->getAutoIncrementPtr());

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
                flagSetControls(buttonHeld, warpsMenuPtr->getCurrentIndexPtr());
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
            flagSetControls(button, warpsMenuPtr->getCurrentIndexPtr());
            break;
        }
        case MenuButtonInput::A:
        {
            menuPtr->runSelectedOptionFunc();
            break;
        }
        case MenuButtonInput::B:
        {
            if (menuPtr->flagIsSet(WarpsMenuInitFlag::WARPS_MENU_INIT_FLAG_SELECTING_WARP_OPTION))
            {
                menuPtr->clearFlag(WarpsMenuInitFlag::WARPS_MENU_INIT_FLAG_SELECTING_WARP_OPTION);
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

void WarpsMenu::drawSelectInitWarpInfo()
{
    // Get the text position for the top-left of the window two lines under the main text
    const Menu *menuPtr = gMenu;
    constexpr float scale = MENU_SCALE;
    const uint32_t totalOptions = menuPtr->getTotalOptions();

    float tempPosX;
    float tempPosY;
    gRootWindow->getTextPosXYUnderMainText(nullptr, WindowAlignment::TOP_LEFT, totalOptions, 2, scale, &tempPosX, &tempPosY);

    // Make sure the current index is valid
    WarpsMenu *warpsMenuPtr = gWarpsMenu;
    constexpr uint32_t totalWarpOptions = sizeof(gDestinations) / sizeof(const char *);
    uint32_t currentIndex = gWarpsMenu->getCurrentIndex();

    if (currentIndex >= totalWarpOptions)
    {
        // Failsafe: Reset the current index to 0
        currentIndex = 0;
        warpsMenuPtr->setCurrentIndex(currentIndex);
    }

    // Draw each warp option
    const bool selectingWarp = menuPtr->flagIsSet(WarpsMenuInitFlag::WARPS_MENU_INIT_FLAG_SELECTING_WARP_OPTION);
    constexpr uint32_t totalWarpRows = intCeil(totalWarpOptions, WARPS_MENU_INIT_MAX_OPTIONS_PER_ROW);
    const char **destinationsPtr = gDestinations;
    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    constexpr float posXIncrement = 100.f;

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    const float posXBase = tempPosX;
    const float posYBase = tempPosY;
    float posX = posXBase;
    float posY = posYBase;

    for (uint32_t i = 0, counter = 0; i < totalWarpOptions; i++, counter++)
    {
        if (counter >= totalWarpRows)
        {
            counter = 0;

            // Move to the next row
            posX += posXIncrement;
            posY = posYBase;
        }

        uint32_t color = getColorWhite(0xFF);
        if (selectingWarp)
        {
            color = getCurrentOptionColor(currentIndex == i, 0xFF);
        }

        drawText(destinationsPtr[i], posX, posY, scale, color);
        posY -= lineDecrement;
    }

    // Draw the description for the current option
    if (selectingWarp)
    {
        posY = posYBase - lineDecrement - (lineDecrement * intToFloat(totalWarpRows));
        drawText(gDestinationsDescriptions[currentIndex], posXBase, posY, scale, getColorWhite(0xFF));
    }
}

static void draw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDraw(cameraId, user);

    // Draw the info for selecting a warp
    WarpsMenu *warpsMenuPtr = gWarpsMenu;
    warpsMenuPtr->drawSelectInitWarpInfo();

    // Draw the value editor if applicable
    ValueEditor *valueEditorPtr = warpsMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->draw();
    }

    // Draw an error message if applicable
    ErrorWindow *errorWindowPtr = warpsMenuPtr->getErrorWindowPtr();
    if (errorWindowPtr->shouldDraw())
    {
        errorWindowPtr->draw();
    }
}

static void exit()
{
    delete gWarpsMenu;
    gWarpsMenu = nullptr;
}

void WarpsMenu::initErrorWindow(const char *text)
{
    ErrorWindow *errorWindowPtr = &this->errorWindow;
    const Window *rootWindowPtr = gRootWindow;

    errorWindowPtr->setAlpha(rootWindowPtr->getAlpha());
    errorWindowPtr->setText(text);
    errorWindowPtr->setTimer(3000);
    errorWindowPtr->placeInWindow(rootWindowPtr, WindowAlignment::MIDDLE_CENTER);
}

static uint32_t warpToMap(uint32_t index)
{
    // Make sure a file is currently loaded and the player is not currently transitioning screens nor in a battle
    if (!checkIfInGame())
    {
        return WarpsMenuWarpToMapReturnValue::WARPS_MENU_WARP_TO_MAP_NOT_IN_GAME;
    }

    // Make sure the index is valid
    constexpr uint32_t totalOptions = sizeof(gDestinations) / sizeof(const char *);
    if (index >= totalOptions)
    {
        return WarpsMenuWarpToMapReturnValue::WARPS_MENU_WARP_TO_MAP_INVALID_INDEX;
    }

    const char *map;
    if (index == WarpsMenuInitOptions::WARPS_MENU_INIT_OPTION_PIT_OF_100_TRIALS)
    {
        // Set the loading zone to always come out of the pipe at the top of the room
        setNextBero("dokan_2");

        const uint32_t currentPitLevel = getCurrentPitLevel();
        char newPitMapChar;

        // newPitMap needs to be a global variable
        static char newPitMap[7];

        // Get the proper map to use for the current level
        if (currentPitLevel % 10 == 0)
        {
            // Current level is a chest level or Bonetail
            if (currentPitLevel <= 40)
            {
                newPitMapChar = '3';
            }
            else if (currentPitLevel <= 70)
            {
                newPitMapChar = '4';
            }
            else if (currentPitLevel <= 90)
            {
                newPitMapChar = '5';
            }
            else // currentPitLevel == 100
            {
                newPitMapChar = '6';
            }
        }
        else // Current level is neither a chest level nor Bonetail
        {
            if (currentPitLevel < 50)
            {
                newPitMapChar = '0';
            }
            else if (currentPitLevel < 80)
            {
                newPitMapChar = '1';
            }
            else // currentPitLevel < 100
            {
                newPitMapChar = '2';
            }
        }

        // Set the new map string
        snprintf(newPitMap, sizeof(newPitMap), "jon_0%c", newPitMapChar);
        map = newPitMap;
    }
    else
    {
        map = gDestinations[index];
    }

    // Warp to the new map
    setNextMap(map);
    reloadRoomMain();

    return WarpsMenuWarpToMapReturnValue::WARPS_MENU_WARP_TO_MAP_SUCCESS;
}

static bool selectWarp()
{
    WarpsMenu *warpsMenuPtr = gWarpsMenu;
    switch (warpToMap(warpsMenuPtr->getCurrentIndex()))
    {
        case WarpsMenuWarpToMapReturnValue::WARPS_MENU_WARP_TO_MAP_SUCCESS:
        {
            // Warped successfully, so close the menu
            closeAllMenus();
            return true;
        }
        case WarpsMenuWarpToMapReturnValue::WARPS_MENU_WARP_TO_MAP_NOT_IN_GAME:
        {
            warpsMenuPtr->initErrorWindow(gWarpsMenuCannotWarpText);
            return false;
        }
        default:
        {
            break;
        }
    }

    return false;
}

static void selectedPitLevel(const ValueType *valuePtr)
{
    setCurrentPitLevel(valuePtr->u32);

    // Warp to the selected level of the Pit
    if (selectWarp())
    {
        return;
    }

    // Close the value editor
    warpsMenuCloseValueEditor();
}

static void selectedOptionSelectWarp(Menu *menuPtr)
{
    WarpsMenu *warpsMenuPtr = gWarpsMenu;

    // If the Select Warp option was just selected, then allow the player to select a warp
    if (!menuPtr->flagIsSet(WarpsMenuInitFlag::WARPS_MENU_INIT_FLAG_SELECTING_WARP_OPTION))
    {
        menuPtr->setFlag(WarpsMenuInitFlag::WARPS_MENU_INIT_FLAG_SELECTING_WARP_OPTION);

        // Reset the current index to the first option
        warpsMenuPtr->setCurrentIndex(0);
        return;
    }

    const uint32_t currentIndex = warpsMenuPtr->getCurrentIndex();
    if (currentIndex == WarpsMenuInitOptions::WARPS_MENU_INIT_OPTION_PIT_OF_100_TRIALS)
    {
        // Make sure a file is currently loaded and the player is not currently transitioning screens nor in a battle
        if (!checkIfInGame())
        {
            warpsMenuPtr->initErrorWindow(gWarpsMenuCannotWarpText);
            return;
        }

        // Initialize the value editor
        ValueEditor *valueEditorPtr = warpsMenuPtr->getValueEditorPtr();

        uint32_t flags = 0;
        flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);
        flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Y_SET_MAX);
        flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Z_SET_MIN);

        const uint32_t currentValue = getCurrentPitLevel();
        constexpr uint32_t minValue = 1;
        constexpr uint32_t maxValue = 100;
        const Window *rootWindowPtr = gRootWindow;

        valueEditorPtr
            ->init(&currentValue, &minValue, &maxValue, rootWindowPtr, flags, VariableType::u8, rootWindowPtr->getAlpha());

        valueEditorPtr->startDrawing(selectedPitLevel, warpsMenuCloseValueEditor);
    }
    else
    {
        // Warp to the selected map
        if (selectWarp())
        {
            return;
        }
    }
}

void warpsMenuEventIndexControls(Menu *menuPtr, MenuButtonInput button)
{
    // If the value editor is open, then handle the controls for that
    ValueEditor *valueEditorPtr = gWarpsMenu->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->controls(button);
        return;
    }

    // Use the default controls
    basicMenuLayoutControls(menuPtr, button);
}

void warpsMenuCloseValueEditor()
{
    gWarpsMenu->getValueEditorPtr()->stopDrawing();
}

uint32_t getCurrentPitLevel()
{
    uint32_t level = swByteGet(1321);

    // Make sure the current level is valid
    if (level >= 100) // Level is zero-indexed
    {
        // Default to the first level
        level = 0;
    }

    return level + 1; // Start at one
}

void setCurrentPitLevel(uint32_t level)
{
    // Make sure the desired level is valid
    if ((level >= 1) && (level <= 100))
    {
        // Adjust the level to be zero-indexed
        level -= 1;
    }
    else
    {
        // Default to the first level
        level = 0;
    }

    swByteSet(1321, level);
}

void setNextMap(const char *map)
{
    // Make sure the length of the map string is valid
    if (strlen(map) >= sizeof(_next_map))
    {
        return;
    }

    strcpy(_next_map, map);
    strncpy(_next_area, map, 3);
}

void setNextBero(const char *bero)
{
    // Make sure the length of the bero string is valid
    if (strlen(bero) >= sizeof(_next_bero))
    {
        return;
    }

    strcpy(_next_bero, bero);
}
