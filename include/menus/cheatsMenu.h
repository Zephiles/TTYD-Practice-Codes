#ifndef MENUS_CHEATS_MENU_H
#define MENUS_CHEATS_MENU_H

#include "classes/valueEditor.h"
#include "classes/buttonComboEditor.h"
#include "classes/confirmationWindow.h"
#include "classes/errorWindow.h"
#include "classes/menu.h"

#include <cstdint>

#define MAX_CHEATS_PER_PAGE 18

#define CHEATS_TOTAL_AREAS 22                   // Lock Flags and Clear Area Flags
#define CHEATS_AREA_FLAGS_MAX_OPTIONS_PER_ROW 4 // Clear Area Flags

#define CHEATS_TOTAL_EVENT_NAMES 406

enum CheatsMenuOptions
{
    CHEATS_MENU_CHANGE_SEQUENCE = 0,
    CHEATS_MENU_WALK_THROUGH_WALLS,
    CHEATS_MENU_FALL_THROUGH_FLOORS,
    CHEATS_MENU_SAVE_COORDINATES,
    CHEATS_MENU_LOAD_COORDINATES,
    CHEATS_MENU_MODIFY_COORDINATES,
    CHEATS_MENU_SPAWN_ITEM,
    CHEATS_MENU_SAVE_ANYWHERE,
    CHEATS_MENU_TEXT_STORAGE,
    CHEATS_MENU_TIME_STOP_TEXT_STORAGE,
    CHEATS_MENU_SPEED_UP_MARIO,
    CHEATS_MENU_DISABLE_BATTLES,
    CHEATS_MENU_AUTO_ACTION_COMMANDS,
    CHEATS_MENU_INFINITE_ITEM_USAGE,
    CHEATS_MENU_RELOAD_ROOM,
    CHEATS_MENU_LEVITATE,
    CHEATS_MENU_AUTO_MASH_TEXT,
    CHEATS_MENU_FRAME_ADVANCE,
    CHEATS_MENU_GENERATE_LAG_SPIKE,
    CHEATS_MENU_LOCK_MARIO_HP_TO_MAX,
    CHEATS_MENU_LOCK_PARTNER_HP_TO_MAX,
    CHEATS_MENU_RUN_FROM_BATTLES,
    CHEATS_MENU_BOBBERY_EARLY,
    CHEATS_MENU_DISABLE_MENU_SOUNDS,
    CHEATS_MENU_FORCE_NPC_ITEM_DROP,
    CHEATS_MENU_RESOLVE_FADES,
    CHEATS_MENU_LOCK_FLAGS,
    CHEATS_MENU_MANAGE_FLAGS,
    CHEATS_MENU_CLEAR_AREA_FLAGS,
};

enum CheatsMenuFrameAdvanceOptions
{
    CHEATS_MENU_FRAME_ADVANCE_TURN_ON_OFF = 0,
    CHEATS_MENU_FRAME_ADVANCE_CHANGE_FRAME_ADVANCE_COMBO,
    CHEATS_MENU_FRAME_ADVANCE_CHANGE_PAUSE_RESUME_COMBO,
};

enum LockFlagsOptions
{
    LOCK_FLAGS_GSW = 0,
    LOCK_FLAGS_GSWF,
    LOCK_FLAGS_GW,
    LOCK_FLAGS_GF,
    LOCK_FLAGS_LSW,
    LOCK_FLAGS_LSWF,
    LOCK_FLAGS_MAX_VALUE, // Don't use this directly other than for defines
};

enum ManageFlagsOptions
{
    MANAGE_FLAGS_GSW = 0,
    MANAGE_FLAGS_GSWF,
    MANAGE_FLAGS_GW,
    MANAGE_FLAGS_GF,
    MANAGE_FLAGS_LSW,
    MANAGE_FLAGS_LSWF,
};

enum CheatsMenuGenericFlags
{
    CHEATS_GENERIC_FLAG_CHEAT_HAS_BUTTON_COMBO = 0,
};

enum CheatsMenuClearAreaFlags
{
    CHEATS_CLEAR_AREA_FLAGS_FLAG_CURRENTLY_SELECTING_AREA = 0,
};

enum class ResolveFadeReturnValue
{
    RESOLVE_FADE_RETURN_TYPE_INVALID_INDEX = 0,
    RESOLVE_FADE_RETURN_TYPE_NOT_ACTIVE,
    RESOLVE_FADE_RETURN_TYPE_DONT_RESOLVE,
    RESOLVE_FADE_RETURN_TYPE_SUCCESS,
};

enum ClearAreaFlagsIndex
{
    CLEAR_AREA_FLAGS_GOR = 0,
    CLEAR_AREA_FLAGS_TIK,
    CLEAR_AREA_FLAGS_HEI,
    CLEAR_AREA_FLAGS_NOK,
    CLEAR_AREA_FLAGS_GON,
    CLEAR_AREA_FLAGS_WIN,
    CLEAR_AREA_FLAGS_MRI,
    CLEAR_AREA_FLAGS_TOU,
    CLEAR_AREA_FLAGS_USU,
    CLEAR_AREA_FLAGS_GRA,
    CLEAR_AREA_FLAGS_JIN,
    CLEAR_AREA_FLAGS_MUJ,
    CLEAR_AREA_FLAGS_DOU,
    CLEAR_AREA_FLAGS_HOM,
    CLEAR_AREA_FLAGS_RSH,
    CLEAR_AREA_FLAGS_EKI,
    CLEAR_AREA_FLAGS_PIK,
    CLEAR_AREA_FLAGS_BOM,
    CLEAR_AREA_FLAGS_MOO,
    CLEAR_AREA_FLAGS_AJI,
    CLEAR_AREA_FLAGS_LAS,
    CLEAR_AREA_FLAGS_JON,
};

class CheatsMenu
{
   public:
    CheatsMenu() {}
    ~CheatsMenu() {}

    ValueEditor *getValueEditorPtr() { return &this->valueEditor; }
    ButtonComboEditor *getButtonComboEditorPtr() { return &this->buttonComboEditor; }
    ConfirmationWindow *getConfirmationWindowPtr() { return &this->confirmationWindow; }
    ErrorWindow *getErrorWindowPtr() { return &this->errorWindow; }

    MenuAutoIncrement *getAutoIncrementPtr() { return &this->autoIncrement; }

    uint32_t getFlagVariableValue() const { return this->flagVariableValue; }
    void setFlagVariableValue(uint32_t value) { this->flagVariableValue = value; }

    uint32_t getFlagVariableToSet() const { return this->flagVariableToSet; }
    void setFlagVariableToSet(uint32_t flagVariable) { this->flagVariableToSet = static_cast<uint16_t>(flagVariable); }

    uint32_t getSelectedCheat() const { return this->selectedCheat; }
    void setSelectedCheat(uint32_t selectedCheat) { this->selectedCheat = static_cast<uint8_t>(selectedCheat); }

    uint8_t *getCurrentIndexPtr() { return &this->currentIndex; }
    uint32_t getCurrentIndex() const { return this->currentIndex; }
    void setCurrentIndex(uint32_t index) { this->currentIndex = static_cast<uint8_t>(index); }

   private:
    ValueEditor valueEditor;
    ButtonComboEditor buttonComboEditor;
    ConfirmationWindow confirmationWindow;
    ErrorWindow errorWindow;

    MenuAutoIncrement autoIncrement;

    uint32_t flagVariableValue; // Manage Flags
    uint16_t flagVariableToSet; // Manage Flags

    uint8_t selectedCheat;
    uint8_t currentIndex; // Manage Flags, Clear Area Flags
};

extern CheatsMenu *gCheatsMenu;
extern const MenuOption gCheatsMenuInitOptions[];
extern const char *gCheatsAreaNames[CHEATS_TOTAL_AREAS][2];

#ifdef TTYD_JP
extern const char *gCheatsEventNames[CHEATS_TOTAL_EVENT_NAMES];
#endif

// cheatsInit
void cheatsMenuInit(Menu *menuPtr);

void cheatsMenuDefaultControlsWithValueEditor(Menu *menuPtr, MenuButtonInput button);
void cheatsMenuDefaultControlsWithButtonComboEditor(Menu *menuPtr, MenuButtonInput button);

bool cheatsMenuToggleEnabledFlag(uint32_t cheatEnabledFlag);
void cheatsMenuSetNewButtonCombo(uint32_t cheatButtonComboFlag, uint32_t buttonCombo);
void cheatsMenuChangeButtonCombo(ButtonComboEditorSetComboFunc setComboFunc);

void cheatsMenuValueEditorCancelSetValue();

void cheatsMenuInitValueEditor(uint32_t currentValue,
                               uint32_t minValue,
                               uint32_t maxValue,
                               uint32_t flags,
                               VariableType type,
                               bool hasMinAndMax,
                               ValueEditorSetValueFunc setValueFunc);

uint32_t indexToCheatEnabledFlag(uint32_t index);
uint32_t indexToCheatButtonComboFlag(uint32_t index);

// cheatsGeneric
void cheatsMenuGenericNoButtonComboInit(Menu *menuPtr);
void cheatsMenuGenericHasButtonComboInit(Menu *menuPtr);

// cheatsChangeSequence
void cheatsMenuChangeSequenceInit(Menu *menuPtr);

#ifdef TTYD_JP
bool getStageString(char *stageNameBuffer, uint32_t stageNameSize, uint32_t sequencePosition);
#endif

void drawStageAndEvent(uint32_t sequencePosition, float posX, float posY);

// cheatsModifyMariosCoordinates
void cheatsMenuModifyMariosCoordinatesInit(Menu *menuPtr);

// cheatsFrameAdvance
void cheatsMenuFrameAdvanceInit(Menu *menuPtr);

// cheatsGenerateLagSpike
void cheatsMenuGenerateLagSpikeInit(Menu *menuPtr);

// cheatsDisableCertainSounds
void cheatsMenuDisableCertainSoundsInit(Menu *menuPtr);

// cheatsForceNpcItemDrop
void cheatsMenuForceNpcItemDropInit(Menu *menuPtr);

// cheatsResolveFades
void cheatsMenuResolveFadesInit(Menu *menuPtr);

ResolveFadeReturnValue resolveFade(uint32_t index);

// cheatsLockFlags
void cheatsMenuLockFlagsInit(Menu *menuPtr);

void getLockFlagsRegionPtrAndSize(uint32_t region, void **ptr, uint32_t *size);

// cheatsManageFlags
void cheatsMenuManageFlagsInit(Menu *menuPtr);

uint32_t getGW(uint32_t gw);
void setGW(uint32_t gw, uint32_t value);
bool getGF(uint32_t gf);
void toggleGF(uint32_t gf);
uint32_t getGlobalVariableFlagValue(uint32_t selectedOption, uint32_t variableFlag);

// cheatsClearAreaFlags
void cheatsMenuClearAreaFlagsInit(Menu *menuPtr);

void clearAreaFlags(uint32_t index);

#endif
