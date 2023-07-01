#include "menuUtils.h"
#include "cheats.h"
#include "menus/cheatsMenu.h"
#include "menus/rootMenu.h"

#include <cstdint>

CheatsMenu *gCheatsMenu = nullptr;

const MenuOption gCheatsMenuInitOptions[] = {
    "Change Sequence",
    nullptr,

    "Walk Through Most Objects",
    cheatsMenuGenericHasButtonComboInit,

    "Fall Through Most Objects",
    cheatsMenuGenericHasButtonComboInit,

    "Save Coordinates",
    cheatsMenuGenericHasButtonComboInit,

    "Load Coordinates",
    cheatsMenuGenericHasButtonComboInit,

    "Modify Mario's Coordinates",
    nullptr,

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
    nullptr,

    "Generate Lag Spike",
    nullptr,

    "Lock Mario's HP To Its Max",
    cheatsMenuGenericNoButtonComboInit,

    "Allow Running From Any Battle",
    cheatsMenuGenericNoButtonComboInit,

    "Bobbery Early",
    cheatsMenuGenericNoButtonComboInit,

    "Disable Certain Sounds",
    nullptr,

    "Force NPC Item Drop",
    nullptr,

    "Resolve Fades",
    nullptr,

    "Lock Flags",
    nullptr,

    "Manage Flags",
    nullptr,

    "Clear Area Flags",
    nullptr,
};

const MenuFunctions gCheatsMenuInitFuncs = {
    controlsBasicMenuLayout,
    drawBasicMenuLayout,
    cheatsMenuInitExit,
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

    cheatsMenuPtr = new CheatsMenu(gRootMenu->getScale());
    gCheatsMenu = cheatsMenuPtr;

    constexpr uint32_t totalOptions = sizeof(gCheatsMenuInitOptions) / sizeof(MenuOption);
    enterNextMenu(gCheatsMenuInitOptions, &gCheatsMenuInitFuncs, totalOptions, MAX_CHEATS_PER_PAGE);
}

void cheatsMenuInitExit()
{
    delete gCheatsMenu;
    gCheatsMenu = nullptr;
}

uint32_t convertIndexToCheatEnabledFlag(uint32_t index)
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
