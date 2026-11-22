#ifndef MENUS_DISPLAYS_MENU_H
#define MENUS_DISPLAYS_MENU_H

#include "mod.h"
#include "inline.h"
#include "classes/valueEditor.h"
#include "classes/buttonComboEditor.h"
#include "classes/positionEditor.h"
#include "classes/confirmationWindow.h"
#include "classes/errorWindow.h"
#include "classes/menu.h"

#include <cstdint>

#define MAX_DISPLAYS_PER_PAGE 18

enum DisplaysMenuOptions
{
    DISPLAYS_MENU_ONSCREEN_TIMER = 0,
    DISPLAYS_MENU_FRAME_COUNTER,
    DISPLAYS_MENU_MARIO_COORDINATES,
    DISPLAYS_MENU_MARIO_SPEED_X_Z,
    DISPLAYS_MENU_JUMP_STORAGE,
    DISPLAYS_MENU_BUTTON_INPUTS,
    DISPLAYS_MENU_STICK_ANGLE,
    DISPLAYS_MENU_STAR_POWER_VALUE,
    DISPLAYS_MENU_DPAD_OPTIONS,
    DISPLAYS_MENU_GUARD_SUPERGUARD_TIMINGS,
    DISPLAYS_MENU_ART_ATTACK_HITBOXES,
    DISPLAYS_MENU_MEMORY_USAGE,
    DISPLAYS_MENU_EFFS_ACTIVE,
    DISPLAYS_MENU_EVTS_ACTIVE,
    DISPLAYS_MENU_ENEMY_ENCOUNTER_NOTIFIER,
    DISPLAYS_MENU_HIT_CHECK_VISUALIZATION,
    DISPLAYS_MENU_EVT_ID_VARIABLE,
    DISPLAYS_MENU_ASE_VIEWER,
    DISPLAYS_MENU_YOSHI_SKIP,
    DISPLAYS_MENU_PALACE_SKIP,
    DISPLAYS_MENU_PALACE_SKIP_MINIMAL,
    DISPLAYS_MENU_ARBITRARY_MEMORY_WRITE,
    DISPLAYS_MENU_JABBI_HIVE_SKIP,
    DISPLAYS_MENU_BRIDGE_SKIP,
    DISPLAYS_MENU_BLIMP_TICKET_SKIP,
};

enum DisplaysMenuHitCheckVisualizationOptions
{
    DISPLAYS_MENU_HIT_CHECK_VISUALIZATION_TURN_ON_OFF = 0,
    DISPLAYS_MENU_HIT_CHECK_VISUALIZATION_DRAW_HITS,
    DISPLAYS_MENU_HIT_CHECK_VISUALIZATION_DRAW_MISSES,
    DISPLAYS_MENU_HIT_CHECK_VISUALIZATION_CHANGE_HITS_COLOR,
    DISPLAYS_MENU_HIT_CHECK_VISUALIZATION_CHANGE_MISSES_COLOR,
};

enum DisplaysMenuGenericFlags
{
    DISPLAYS_GENERIC_FLAG_DISPLAY_HAS_BUTTON_COMBO = 0,
    DISPLAYS_GENERIC_FLAG_HANDLE_AS_HEX,
};

enum DisplaysMenuGenericSelectionFlags
{
    DISPLAYS_GENERIC_OPTION_FLAG_HAS_BUTTON_COMBO = 0,
    DISPLAYS_GENERIC_OPTION_FLAG_NO_BUTTON_COMBO,
    DISPLAYS_GENERIC_OPTION_FLAG_NO_MANUAL_POSITION,
    DISPLAYS_GENERIC_OPTION_HANDLE_AS_HEX,
};

// Flag functions take values 0-31. The functions return if the flag parameter has a higher value than 31.
INLINE_FUNC bool displaysMenuGenericSelectionFlagIsSet(uint32_t flags, uint32_t flag)
{
    // Make sure the flag is valid
    constexpr uint32_t maxFlags = sizeof(flags) * 8;
    if (flag >= maxFlags)
    {
        return false;
    }

    bool ret;
    FLAG_IS_SET(&flags, flag, ret);
    return ret;
}

INLINE_FUNC uint32_t setDisplaysMenuGenericSelectionFlag(uint32_t flags, uint32_t flag)
{
    // Make sure the flag is valid
    constexpr uint32_t maxFlags = sizeof(flags) * 8;
    if (flag >= maxFlags)
    {
        return flags;
    }

    SET_FLAG(&flags, flag);
    return flags;
}

class DisplaysMenu
{
   public:
    DisplaysMenu() {}
    ~DisplaysMenu() {}

    ValueEditor *getValueEditorPtr() { return &this->valueEditor; }
    ButtonComboEditor *getButtonComboEditorPtr() { return &this->buttonComboEditor; }
    PositionEditor *getPositionEditorPtr() { return &this->positionEditor; }
    ConfirmationWindow *getConfirmationWindowPtr() { return &this->confirmationWindow; }
    ErrorWindow *getErrorWindowPtr() { return &this->errorWindow; }

    uint32_t getSelectedDisplay() const { return this->selectedDisplay; }
    void setSelectedDisplay(uint32_t selectedDisplay) { this->selectedDisplay = static_cast<uint8_t>(selectedDisplay); }

    void initErrorWindow(const char *text);

   private:
    ValueEditor valueEditor;
    ButtonComboEditor buttonComboEditor;
    PositionEditor positionEditor;
    ConfirmationWindow confirmationWindow;
    ErrorWindow errorWindow;

    uint8_t selectedDisplay;
};

extern DisplaysMenu *gDisplaysMenu;
extern const MenuOption gDisplaysMenuInitOptions[];

// displaysInit
void displaysMenuInit(Menu *menuPtr);

void displaysMenuStopDrawingValueEditor();
void displaysMenuDefaultControlsWithButtonComboEditor(Menu *menuPtr, MenuButtonInput button);
bool displaysMenuToggleEnabledFlag(uint32_t displayEnabledFlag);
void displaysMenuSetNewButtonCombo(uint32_t displayButtonComboFlag, uint32_t buttonCombo);
void displaysMenuChangeButtonCombo(ButtonComboEditorSetComboFunc setComboFunc);

uint32_t indexToDisplayEnabledFlag(uint32_t index);
uint32_t indexToDisplayManuallyPositionFlag(uint32_t index);
uint32_t indexToDisplayButtonComboFlag(uint32_t index);
uint32_t indexToDisplayHandleAsHexFlag(uint32_t index);

// displaysGeneric
void displaysMenuGenericNoButtonComboInit(Menu *menuPtr);
void displaysMenuGenericHasButtonComboInit(Menu *menuPtr);
void displaysMenuGenericHasButtonComboNoManualPositionInit(Menu *menuPtr);
void displaysMenuGenericNoButtonComboNoManualPositionInit(Menu *menuPtr);
void displaysMenuGenericHandleAsHex(Menu *menuPtr);
void drawGenericDisplayInfo();
void displaysGenericChangeButtonCombo(Menu *menuPtr);

// displaysOnScreenTimer
// The Frame Founter display is also handled by these
void displaysMenuOnScreenTimerInit(Menu *menuPtr);

// displaysMemoryUsage
void displaysMenuMemoryUsageInit(Menu *menuPtr);

// displaysHitCheckVisualization
void displaysMenuHitCheckVisualizationInit(Menu *menuPtr);

// displaysAdjustManualPosition
void displaysAdjustManualPositionInit(Menu *menuPtr);

// displaysArbitraryMemoryWrite
void displaysArbitraryMemoryWriteInit(Menu *menuPtr);

// displaysEvtIdVariable
void displaysEvtIdVariableInit(Menu *menuPtr);

// displaysArbitraryScriptExecutionViewer
void displaysArbitraryScriptExecutionViewerInit(Menu *menuPtr);

#endif
