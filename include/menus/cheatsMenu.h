#ifndef MENUS_CHEATS_MENU_H
#define MENUS_CHEATS_MENU_H

#include "classes/valueEditor.h"
#include "classes/menu.h"
#include "ttyd/camdrv.h"

#include <cstdint>

#define MAX_CHEATS_PER_PAGE 18

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
    CHEATS_MENU_RUN_FROM_BATTLES,
    CHEATS_MENU_BOBBERY_EARLY,
    CHEATS_MENU_DISABLE_MENU_SOUNDS,
    CHEATS_MENU_FORCE_NPC_ITEM_DROP,
    CHEATS_MENU_RESOLVE_FADES,
    CHEATS_MENU_LOCK_FLAGS,
    CHEATS_MENU_MANAGE_FLAGS,
    CHEATS_MENU_CLEAR_AREA_FLAGS,
};

enum CheatsMenuGenericFlags
{
    CHEATS_GENERIC_FLAG_CHEAT_HAS_BUTTON_COMBO = 0,
};

enum class ResolveFadeReturnValue : int32_t
{
    RESOLVE_FADE_RETURN_TYPE_INVALID_INDEX = -1,
    RESOLVE_FADE_RETURN_TYPE_NOT_ACTIVE,
    RESOLVE_FADE_RETURN_TYPE_DONT_RESOLVE,
    RESOLVE_FADE_RETURN_TYPE_SUCCESS,
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

enum class ClearAreaFlagsIndex : uint8_t
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
    CheatsMenu(float scale) { this->scale = scale; }
    ~CheatsMenu() {}

    ValueEditor *getValueEditor() { return &this->valueEditor; }
    float getScale() const { return this->scale; }
    MenuAutoIncrement *getAutoIncrementPtr() { return &this->autoIncrement; }

    void setSelectedCheat(uint32_t selectedCheat) { this->selectedCheat = static_cast<uint8_t>(selectedCheat); }
    uint32_t getSelectedCheat() const { return this->selectedCheat; }

    void drawGenericCheatInfo() const;

   private:
    ValueEditor valueEditor;
    float scale;

    MenuAutoIncrement autoIncrement;
    uint8_t selectedCheat;
};

extern CheatsMenu *gCheatsMenu;
extern const MenuOption gCheatsMenuInitOptions[];

void cheatsMenuInit(Menu *menuPtr);
void cheatsMenuInitExit();

void cheatsMenuGenericNoButtonComboInit(Menu *menuPtr);
void cheatsMenuGenericHasButtonComboInit(Menu *menuPtr);
void cheatsMenuGenericControls(Menu *menuPtr, MenuButtonInput button);
void cheatsMenuGenericDraw(CameraId cameraId, void *user);

void cheatsMenuGenericToggleFlag(Menu *menuPtr);
void cheatsMenuGenericChangeButtonCombo(Menu *menuPtr);

uint32_t convertIndexToCheatEnabledFlag(uint32_t index);

ResolveFadeReturnValue resolveFade(uint32_t index);

void *getLockFlagsRegionPtr(uint32_t region);
uint32_t getLockFlagsRegionSize(uint32_t region);
void getLockFlagsRegionPtrAndSize(uint32_t region, void **ptr, uint32_t *size);

void clearAreaFlags(ClearAreaFlagsIndex index);

#endif