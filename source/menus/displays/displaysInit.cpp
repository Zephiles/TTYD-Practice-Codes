#include "menuUtils.h"
#include "cxx.h"
#include "displays.h"
#include "mod.h"
#include "classes/buttonComboEditor.h"
#include "classes/window.h"
#include "menus/displaysMenu.h"
#include "menus/rootMenu.h"

#include <cstdint>

DisplaysMenu *gDisplaysMenu = nullptr;

const MenuOption gDisplaysMenuInitOptions[] = {
    "On-Screen Timer",
    displaysMenuOnScreenTimerInit,

    "Frame Counter",
    displaysMenuOnScreenTimerInit,

    "Mario's Coordinates",
    displaysMenuGenericNoButtonComboInit,

    "Mario's Speed XZ",
    displaysMenuGenericNoButtonComboInit,

    "Jump Storage",
    displaysMenuGenericHasButtonComboInit,

    "Button Inputs",
    displaysMenuGenericNoButtonComboInit,

    "Stick Angle",
    displaysMenuGenericNoButtonComboInit,

    "Star Power Value",
    displaysMenuGenericNoButtonComboNoManualPositionInit,

    "D-Pad Options",
    displaysMenuGenericNoButtonComboNoManualPositionInit,

    "Guard/Superguard Timings",
    displaysMenuGenericNoButtonComboInit,

    "Art Attack Hitboxes",
    displaysMenuGenericHasButtonComboNoManualPositionInit,

    "Memory Usage",
    displaysMenuMemoryUsageInit,

    "Effs Active",
    displaysMenuGenericNoButtonComboInit,

    "Evts Active",
    displaysMenuGenericNoButtonComboInit,

    "Enemy Encounter Notifier",
    displaysMenuGenericNoButtonComboInit,

    "Hit Check Visualization (Emulator Only)",
    displaysMenuHitCheckVisualizationInit,

    "Yoshi Skip",
    displaysMenuGenericHasButtonComboInit,

    "Palace Skip",
    displaysMenuGenericHasButtonComboInit,

    "Palace Skip (Minimal)",
    displaysMenuGenericHasButtonComboInit,

    "Jabbi Hive Skip",
    displaysMenuGenericHasButtonComboInit,

    "Bridge Skip",
    displaysMenuGenericHasButtonComboInit,

    "Blimp Ticket Skip",
    displaysMenuGenericHasButtonComboInit,
};

const MenuFunctions gDisplaysMenuInitFuncs = {
    gDisplaysMenuInitOptions,
    basicMenuLayoutControls,
    basicMenuLayoutDrawMenuLineHeight,
    displaysMenuInitExit,
};

void displaysMenuInit(Menu *menuPtr)
{
    (void)menuPtr;

    // Failsafe: Make sure memory isn't already allocated for gDisplaysMenu
    DisplaysMenu *displaysMenuPtr = gDisplaysMenu;
    if (displaysMenuPtr)
    {
        delete displaysMenuPtr;
    }

    displaysMenuPtr = new DisplaysMenu;
    gDisplaysMenu = displaysMenuPtr;

    constexpr uint32_t totalOptions = sizeof(gDisplaysMenuInitOptions) / sizeof(MenuOption);
    enterNextMenu(&gDisplaysMenuInitFuncs, totalOptions, MAX_DISPLAYS_PER_PAGE);
}

void displaysMenuInitExit()
{
    delete gDisplaysMenu;
    gDisplaysMenu = nullptr;
}

void displaysMenuDefaultControlsWithButtonComboEditor(Menu *menuPtr, MenuButtonInput button)
{
    // If the button combo editor is open, then handle the controls for that
    ButtonComboEditor *buttonComboEditorPtr;
    if (buttonComboEditorPtr = gDisplaysMenu->getButtonComboEditorPtr(), buttonComboEditorPtr->shouldDraw())
    {
        buttonComboEditorPtr->controls(button);
        return;
    }

    // Use the default controls
    basicMenuLayoutControls(menuPtr, button);
}

bool displaysMenuToggleEnabledFlag(uint32_t displayEnabledFlag)
{
    Displays *displaysPtr = gDisplays;
    const bool ret = displaysPtr->toggleEnabledFlag(displayEnabledFlag);

    if (ret)
    {
        // Only one trick display should be drawn at a time, so handle disabling other flags if a trick display was just enabled
        displaysPtr->handleEnablingTrickDisplayFlag(displayEnabledFlag);
    }

    return ret;
}

void displaysMenuSetDisplayButtonCombo(uint32_t displayButtonComboFlag, uint32_t buttonCombo)
{
    gDisplays->setDisplayButtonCombo(displayButtonComboFlag, buttonCombo);
}

void displaysMenuCancelSetNewButtonCombo()
{
    gDisplaysMenu->getButtonComboEditorPtr()->stopDrawing();
    gMod->clearFlag(ModFlag::MOD_FLAG_CHANGING_BUTTON_COMBO);
}

void displaysMenuSetNewButtonCombo(uint32_t displayButtonComboFlag, uint32_t buttonCombo)
{
    displaysMenuSetDisplayButtonCombo(displayButtonComboFlag, buttonCombo);

    // Close the button combo editor
    displaysMenuCancelSetNewButtonCombo();
}

void displaysMenuChangeButtonCombo(ButtonComboEditorSetComboFunc setComboFunc)
{
    gMod->setFlag(ModFlag::MOD_FLAG_CHANGING_BUTTON_COMBO);

    // Initialize the button combo editor
    DisplaysMenu *displaysMenuPtr = gDisplaysMenu;
    ButtonComboEditor *buttonComboEditorPtr = displaysMenuPtr->getButtonComboEditorPtr();

    const Window *rootWindowPtr = gRootWindow;
    buttonComboEditorPtr->init(rootWindowPtr, rootWindowPtr->getAlpha());
    buttonComboEditorPtr->startDrawing(setComboFunc, displaysMenuCancelSetNewButtonCombo);
}

uint32_t indexToDisplayEnabledFlag(uint32_t index)
{
    switch (index)
    {
        case DisplaysMenuOptions::DISPLAYS_MENU_ONSCREEN_TIMER:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_ONSCREEN_TIMER;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_FRAME_COUNTER:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_FRAME_COUNTER;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_MARIO_COORDINATES:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MARIO_COORDINATES;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_MARIO_SPEED_X_Z:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MARIO_SPEED_X_Z;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_JUMP_STORAGE:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_JUMP_STORAGE;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_BUTTON_INPUTS:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_BUTTON_INPUTS;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_STICK_ANGLE:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_STICK_ANGLE;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_STAR_POWER_VALUE:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_STAR_POWER_VALUE;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_DPAD_OPTIONS:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_DPAD_OPTIONS;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_GUARD_SUPERGUARD_TIMINGS:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_GUARD_SUPERGUARD_TIMINGS;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_ART_ATTACK_HITBOXES:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_ART_ATTACK_HITBOXES;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_MEMORY_USAGE:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_HEAP_0;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_EFFS_ACTIVE:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_EFFS_ACTIVE;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_EVTS_ACTIVE:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_EVTS_ACTIVE;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_ENEMY_ENCOUNTER_NOTIFIER:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_ENEMY_ENCOUNTER_NOTIFIER;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_HIT_CHECK_VISUALIZATION:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_HIT_CHECK_VISUALIZATION;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_YOSHI_SKIP:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_YOSHI_SKIP;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_PALACE_SKIP:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_PALACE_SKIP;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_PALACE_SKIP_MINIMAL:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_PALACE_SKIP_MINIMAL;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_JABBI_HIVE_SKIP:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_JABBI_HIVE_SKIP;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_BRIDGE_SKIP:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_BRIDGE_SKIP;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_BLIMP_TICKET_SKIP:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_BLIMP_TICKET_SKIP;
        }
        default:
        {
            // Return an arbitrary invalid flag
            return static_cast<uint32_t>(-1);
        }
    }
}

uint32_t indexToDisplayManuallyPositionFlag(uint32_t index)
{
    switch (index)
    {
        case DisplaysMenuOptions::DISPLAYS_MENU_ONSCREEN_TIMER:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_ONSCREEN_TIMER;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_FRAME_COUNTER:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_FRAME_COUNTER;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_MARIO_COORDINATES:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_MARIO_COORDINATES;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_MARIO_SPEED_X_Z:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_MARIO_SPEED_X_Z;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_JUMP_STORAGE:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_JUMP_STORAGE;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_BUTTON_INPUTS:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_BUTTON_INPUTS;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_STICK_ANGLE:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_STICK_ANGLE;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_GUARD_SUPERGUARD_TIMINGS:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_GUARD_SUPERGUARD_TIMINGS;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_MEMORY_USAGE:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_MEMORY_USAGE;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_EFFS_ACTIVE:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_EFFS_ACTIVE;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_EVTS_ACTIVE:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_EVTS_ACTIVE;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_ENEMY_ENCOUNTER_NOTIFIER:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_ENEMY_ENCOUNTER_NOTIFIER;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_YOSHI_SKIP:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_YOSHI_SKIP;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_PALACE_SKIP:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_PALACE_SKIP;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_PALACE_SKIP_MINIMAL:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_PALACE_SKIP_MINIMAL;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_JABBI_HIVE_SKIP:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_JABBI_HIVE_SKIP;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_BRIDGE_SKIP:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_BRIDGE_SKIP;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_BLIMP_TICKET_SKIP:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_BLIMP_TICKET_SKIP;
        }
        default:
        {
            // Return an arbitrary invalid flag
            return static_cast<uint32_t>(-1);
        }
    }
}

uint32_t indexToDisplayButtonComboFlag(uint32_t index)
{
    switch (index)
    {
        case DisplaysMenuOptions::DISPLAYS_MENU_JUMP_STORAGE:
        {
            return DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_JUMP_STORAGE;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_ART_ATTACK_HITBOXES:
        {
            return DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_ART_ATTACK_HITBOXES;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_YOSHI_SKIP:
        {
            return DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_YOSHI_SKIP;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_PALACE_SKIP:
        {
            return DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_PALACE_SKIP;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_PALACE_SKIP_MINIMAL:
        {
            return DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_PALACE_SKIP_MINIMAL;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_JABBI_HIVE_SKIP:
        {
            return DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_JABBI_HIVE_SKIP;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_BRIDGE_SKIP:
        {
            return DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_BRIDGE_SKIP;
        }
        case DisplaysMenuOptions::DISPLAYS_MENU_BLIMP_TICKET_SKIP:
        {
            return DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_BLIMP_TICKET_SKIP;
        }
        default:
        {
            // Return an arbitrary invalid flag
            return static_cast<uint32_t>(-1);
        }
    }
}
