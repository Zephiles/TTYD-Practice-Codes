#include "menuUtils.h"
#include "cxx.h"
#include "cheats.h"
#include "mod.h"
#include "classes/buttonComboEditor.h"
#include "classes/window.h"
#include "classes/valueEditor.h"
#include "menus/cheatsMenu.h"
#include "menus/rootMenu.h"

#include <cstdint>

CheatsMenu *gCheatsMenu = nullptr;

const MenuOption gCheatsMenuInitOptions[] = {
    "Change Sequence",
    cheatsMenuChangeSequenceInit,

    "Walk Through Most Objects",
    cheatsMenuGenericHasButtonComboInit,

    "Fall Through Most Objects",
    cheatsMenuGenericHasButtonComboInit,

    "Save Coordinates",
    cheatsMenuGenericHasButtonComboInit,

    "Load Coordinates",
    cheatsMenuGenericHasButtonComboInit,

    "Modify Mario's Coordinates",
    cheatsMenuModifyMariosCoordinatesInit,

    "Spawn Item",
    cheatsMenuGenericHasButtonComboInit,

    "Save Anywhere",
    cheatsMenuGenericHasButtonComboInit,

    "Text Storage",
    cheatsMenuGenericHasButtonComboInit,

    "Time Stop Text Storage",
    cheatsMenuGenericHasButtonComboInit,

    "Speed Up Mario",
    cheatsMenuGenericHasButtonComboInit,

    "Disable Non-Cutscene Battles",
    cheatsMenuGenericHasButtonComboInit,

    "Auto Action Commands",
    cheatsMenuGenericHasButtonComboInit,

    "Infinite Item Usage",
    cheatsMenuGenericHasButtonComboInit,

    "Reload Room",
    cheatsMenuGenericHasButtonComboInit,

    "Levitate",
    cheatsMenuGenericHasButtonComboInit,

    "Auto Mash Through Text",
    cheatsMenuGenericHasButtonComboInit,

    "Frame Advance (Experimental)",
    cheatsMenuFrameAdvanceInit,

    "Generate Lag Spike",
    cheatsMenuGenerateLagSpikeInit,

    "Lock Mario's HP To Its Max",
    cheatsMenuGenericNoButtonComboInit,

    "Allow Running From Any Battle",
    cheatsMenuGenericNoButtonComboInit,

    "Bobbery Early",
    cheatsMenuGenericNoButtonComboInit,

    "Disable Certain Sounds",
    cheatsMenuDisableCertainSoundsInit,

    "Force NPC Item Drop",
    cheatsMenuForceNpcItemDropInit,

    "Resolve Fades",
    cheatsMenuResolveFadesInit,

    "Lock Flags",
    cheatsMenuLockFlagsInit,

    "Manage Flags",
    cheatsMenuManageFlagsInit,

    "Clear Area Flags",
    cheatsMenuClearAreaFlagsInit,
};

const MenuFunctions gCheatsMenuInitFuncs = {
    gCheatsMenuInitOptions,
    basicMenuLayoutControls,
    basicMenuLayoutDrawMenuLineHeight,
    cheatsMenuInitExit,
};

const char *gCheatsAreaNames[CHEATS_TOTAL_AREAS][2] = {
    "gor", "Rogueport",
    "tik", "Rogueport Sewers",
    "hei", "Petal Meadows",
    "nok", "Petalburg",
    "gon", "Hooktail Castle",
    "win", "Boggly Woods",
    "mri", "The Great Tree",
    "tou", "Glitzville",
    "usu", "Twilight Town",
    "gra", "Twilight Trail",
    "jin", "Creepy Steeple",
    "muj", "Keelhaul Key",
    "dou", "Pirate's Grotto",
    "hom", "Riverside Station (Outside area with the train)",
    "rsh", "Excess Express",
    "eki", "Riverside Station",
    "pik", "Poshley Heights",
    "bom", "Fahr Outpost",
    "moo", "Moon",
    "aji", "X-Naut Fortress",
    "las", "Palace of Shadow",
    "jon", "Pit of 100 Trials",
};

void cheatsMenuInit(Menu *menuPtr)
{
    (void)menuPtr;

    // Failsafe: Make sure memory isn't already allocated for gCheatsMenu
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;
    if (cheatsMenuPtr)
    {
        delete cheatsMenuPtr;
    }

    cheatsMenuPtr = new CheatsMenu;
    gCheatsMenu = cheatsMenuPtr;

    constexpr uint32_t totalOptions = sizeof(gCheatsMenuInitOptions) / sizeof(MenuOption);
    enterNextMenu(&gCheatsMenuInitFuncs, totalOptions, MAX_CHEATS_PER_PAGE);
}

void cheatsMenuInitExit()
{
    delete gCheatsMenu;
    gCheatsMenu = nullptr;
}

bool cheatsMenuToggleEnabledFlag(uint32_t cheatEnabledFlag)
{
    return gCheats->toggleEnabledFlag(cheatEnabledFlag);
}

void cheatsMenuSetCheatButtonCombo(uint32_t cheatButtonComboFlag, uint32_t buttonCombo)
{
    gCheats->setCheatButtonCombo(cheatButtonComboFlag, buttonCombo);
}

void cheatsMenuCancelSetNewButtonCombo()
{
    gCheatsMenu->getButtonComboEditorPtr()->stopDrawing();
    gMenu->clearFlag(CHEATS_MENU_CHANGING_BUTTON_COMBO_FLAG);
    gMod.stopChangingButtonCombo();
}

void cheatsMenuSetNewButtonCombo(uint32_t cheatButtonComboFlag, uint32_t buttonCombo)
{
    cheatsMenuSetCheatButtonCombo(cheatButtonComboFlag, buttonCombo);

    // Close the button combo editor
    cheatsMenuCancelSetNewButtonCombo();
}

void cheatsMenuChangeButtonCombo(Menu *menuPtr, ButtonComboEditorSetComboFunc setComboFunc)
{
    gMod.startChangingButtonCombo();

    // Bring up the window for changing button combos
    menuPtr->setFlag(CHEATS_MENU_CHANGING_BUTTON_COMBO_FLAG);

    // Initialize the button combo editor
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;
    ButtonComboEditor *buttonComboEditorPtr = cheatsMenuPtr->getButtonComboEditorPtr();

    const Window *rootWindowPtr = gRootWindow;
    buttonComboEditorPtr->init(rootWindowPtr, rootWindowPtr->getAlpha());
    buttonComboEditorPtr->startDrawing(setComboFunc, cheatsMenuCancelSetNewButtonCombo);
}

void cheatsMenuValueEditorCancelSetValue()
{
    gCheatsMenu->getValueEditorPtr()->stopDrawing();
    gMenu->clearFlag(CHEATS_MENU_USING_VALUE_EDITOR_FLAG);
}

void cheatsMenuInitValueEditor(Menu *menuPtr,
                               uint32_t currentValue,
                               uint32_t minValue,
                               uint32_t maxValue,
                               uint32_t flags,
                               VariableType type,
                               bool hasMinAndMax,
                               ValueEditorSetValueFunc setValueFunc)
{
    // Bring up the window for selecting an id
    menuPtr->setFlag(CHEATS_MENU_USING_VALUE_EDITOR_FLAG);

    // Initialize the value editor
    uint32_t *minValuePtr;
    uint32_t *maxValuePtr;

    if (hasMinAndMax)
    {
        minValuePtr = &minValue;
        maxValuePtr = &maxValue;
    }
    else
    {
        minValuePtr = nullptr;
        maxValuePtr = nullptr;
    }

    const Window *rootWindowPtr = gRootWindow;
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;
    ValueEditor *valueEditorPtr = cheatsMenuPtr->getValueEditorPtr();

    valueEditorPtr->init(&currentValue, minValuePtr, maxValuePtr, rootWindowPtr, flags, type, rootWindowPtr->getAlpha());
    valueEditorPtr->startDrawing(setValueFunc, cheatsMenuValueEditorCancelSetValue);
}

uint32_t indexToCheatEnabledFlag(uint32_t index)
{
    switch (index)
    {
        case CheatsMenuOptions::CHEATS_MENU_WALK_THROUGH_WALLS:
        {
            return CheatsEnabledFlag::CHEATS_ENABLED_FLAG_WALK_THROUGH_WALLS;
        }
        case CheatsMenuOptions::CHEATS_MENU_FALL_THROUGH_FLOORS:
        {
            return CheatsEnabledFlag::CHEATS_ENABLED_FLAG_FALL_THROUGH_FLOORS;
        }
        case CheatsMenuOptions::CHEATS_MENU_SAVE_COORDINATES:
        {
            return CheatsEnabledFlag::CHEATS_ENABLED_FLAG_SAVE_COORDINATES;
        }
        case CheatsMenuOptions::CHEATS_MENU_LOAD_COORDINATES:
        {
            return CheatsEnabledFlag::CHEATS_ENABLED_FLAG_LOAD_COORDINATES;
        }
        case CheatsMenuOptions::CHEATS_MENU_SPAWN_ITEM:
        {
            return CheatsEnabledFlag::CHEATS_ENABLED_FLAG_SPAWN_ITEM;
        }
        case CheatsMenuOptions::CHEATS_MENU_SAVE_ANYWHERE:
        {
            return CheatsEnabledFlag::CHEATS_ENABLED_FLAG_SAVE_ANYWHERE;
        }
        case CheatsMenuOptions::CHEATS_MENU_TEXT_STORAGE:
        {
            return CheatsEnabledFlag::CHEATS_ENABLED_FLAG_TEXT_STORAGE;
        }
        case CheatsMenuOptions::CHEATS_MENU_TIME_STOP_TEXT_STORAGE:
        {
            return CheatsEnabledFlag::CHEATS_ENABLED_FLAG_TIME_STOP_TEXT_STORAGE;
        }
        case CheatsMenuOptions::CHEATS_MENU_SPEED_UP_MARIO:
        {
            return CheatsEnabledFlag::CHEATS_ENABLED_FLAG_SPEED_UP_MARIO;
        }
        case CheatsMenuOptions::CHEATS_MENU_DISABLE_BATTLES:
        {
            return CheatsEnabledFlag::CHEATS_ENABLED_FLAG_DISABLE_BATTLES;
        }
        case CheatsMenuOptions::CHEATS_MENU_AUTO_ACTION_COMMANDS:
        {
            return CheatsEnabledFlag::CHEATS_ENABLED_FLAG_AUTO_ACTION_COMMANDS;
        }
        case CheatsMenuOptions::CHEATS_MENU_INFINITE_ITEM_USAGE:
        {
            return CheatsEnabledFlag::CHEATS_ENABLED_FLAG_INFINITE_ITEM_USAGE;
        }
        case CheatsMenuOptions::CHEATS_MENU_RELOAD_ROOM:
        {
            return CheatsEnabledFlag::CHEATS_ENABLED_FLAG_RELOAD_ROOM;
        }
        case CheatsMenuOptions::CHEATS_MENU_LEVITATE:
        {
            return CheatsEnabledFlag::CHEATS_ENABLED_FLAG_LEVITATE;
        }
        case CheatsMenuOptions::CHEATS_MENU_AUTO_MASH_TEXT:
        {
            return CheatsEnabledFlag::CHEATS_ENABLED_FLAG_AUTO_MASH_TEXT;
        }
        case CheatsMenuOptions::CHEATS_MENU_LOCK_MARIO_HP_TO_MAX:
        {
            return CheatsEnabledFlag::CHEATS_ENABLED_FLAG_LOCK_MARIO_HP_TO_MAX;
        }
        case CheatsMenuOptions::CHEATS_MENU_RUN_FROM_BATTLES:
        {
            return CheatsEnabledFlag::CHEATS_ENABLED_FLAG_RUN_FROM_BATTLES;
        }
        case CheatsMenuOptions::CHEATS_MENU_BOBBERY_EARLY:
        {
            return CheatsEnabledFlag::CHEATS_ENABLED_FLAG_BOBBERY_EARLY;
        }
        case CheatsMenuOptions::CHEATS_MENU_FORCE_NPC_ITEM_DROP:
        {
            return CheatsEnabledFlag::CHEATS_ENABLED_FLAG_FORCE_NPC_ITEM_DROP;
        }
        default:
        {
            // Return an arbitrary invalid flag
            return static_cast<uint32_t>(-1);
        }
    }
}

uint32_t indexToCheatButtonComboFlag(uint32_t index)
{
    switch (index)
    {
        case CheatsMenuOptions::CHEATS_MENU_WALK_THROUGH_WALLS:
        {
            return CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_WALK_THROUGH_WALLS;
        }
        case CheatsMenuOptions::CHEATS_MENU_FALL_THROUGH_FLOORS:
        {
            return CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_FALL_THROUGH_FLOORS;
        }
        case CheatsMenuOptions::CHEATS_MENU_SAVE_COORDINATES:
        {
            return CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_SAVE_COORDINATES;
        }
        case CheatsMenuOptions::CHEATS_MENU_LOAD_COORDINATES:
        {
            return CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_LOAD_COORDINATES;
        }
        case CheatsMenuOptions::CHEATS_MENU_SPAWN_ITEM:
        {
            return CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_SPAWN_ITEM;
        }
        case CheatsMenuOptions::CHEATS_MENU_SAVE_ANYWHERE:
        {
            return CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_SAVE_ANYWHERE;
        }
        case CheatsMenuOptions::CHEATS_MENU_TEXT_STORAGE:
        {
            return CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_TEXT_STORAGE;
        }
        case CheatsMenuOptions::CHEATS_MENU_TIME_STOP_TEXT_STORAGE:
        {
            return CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_TIME_STOP_TEXT_STORAGE;
        }
        case CheatsMenuOptions::CHEATS_MENU_SPEED_UP_MARIO:
        {
            return CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_SPEED_UP_MARIO;
        }
        case CheatsMenuOptions::CHEATS_MENU_DISABLE_BATTLES:
        {
            return CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_DISABLE_BATTLES;
        }
        case CheatsMenuOptions::CHEATS_MENU_AUTO_ACTION_COMMANDS:
        {
            return CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_AUTO_ACTION_COMMANDS;
        }
        case CheatsMenuOptions::CHEATS_MENU_INFINITE_ITEM_USAGE:
        {
            return CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_INFINITE_ITEM_USAGE;
        }
        case CheatsMenuOptions::CHEATS_MENU_RELOAD_ROOM:
        {
            return CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_RELOAD_ROOM;
        }
        case CheatsMenuOptions::CHEATS_MENU_LEVITATE:
        {
            return CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_LEVITATE;
        }
        case CheatsMenuOptions::CHEATS_MENU_AUTO_MASH_TEXT:
        {
            return CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_AUTO_MASH_TEXT;
        }
        default:
        {
            // Return an arbitrary invalid flag
            return static_cast<uint32_t>(-1);
        }
    }
}
