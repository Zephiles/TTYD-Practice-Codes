#ifndef MENUS_WARPS_MENU_H
#define MENUS_WARPS_MENU_H

#include "menuUtils.h"
#include "classes/valueEditor.h"
#include "classes/errorwindow.h"
#include "ttyd/camdrv.h"

#include <cstdint>

#define WARPS_MENU_INIT_MAX_OPTIONS_PER_ROW 4
#define WARPS_MENU_BOSS_MAX_OPTIONS_PER_ROW 2

enum WarpsMenuInitOptions
{
    WARPS_MENU_INIT_OPTION_ROGUEPORT_CENTRAL = 0,
    WARPS_MENU_INIT_OPTION_ROGUEPORT_SEWERS_UNDERGROUND_SHOP,
    WARPS_MENU_INIT_OPTION_PETAL_MEADOWS_ENTRANCE,
    WARPS_MENU_INIT_OPTION_HOOKTAIL_CASTLE_ENTRANCE,
    WARPS_MENU_INIT_OPTION_BOGGLY_WOODS_ENTRANCE,
    WARPS_MENU_INIT_OPTION_THE_GREAT_TREE_ENTRANCE,
    WARPS_MENU_INIT_OPTION_GLITZVILLE_LOBBY,
    WARPS_MENU_INIT_OPTION_GLITZVILLE_GRUBBAS_OFFICE,
    WARPS_MENU_INIT_OPTION_TWILIGHT_TOWN_WEST_SIDE,
    WARPS_MENU_INIT_OPTION_CREEPY_STEEPLE_ENTRANCE,
    WARPS_MENU_INIT_OPTION_KEELHAUL_KEY_SHANTYTOWN,
    WARPS_MENU_INIT_OPTION_PIRATES_GROTTO_ENTRANCE,
    WARPS_MENU_INIT_OPTION_EXCESS_EXPRESS_PASSENGER_CAR_CABINS_3_5_DAY,
    WARPS_MENU_INIT_OPTION_RIVERSIDE_STATION_ENTRANCE,
    WARPS_MENU_INIT_OPTION_POSHLEY_HEIGHTS_STATION,
    WARPS_MENU_INIT_OPTION_FAHR_OUTPOST_PIPE_ENTRANCE,
    WARPS_MENU_INIT_OPTION_MOON_ENTRANCE,
    WARPS_MENU_INIT_OPTION_X_NAUT_FORTRESS_ENTRANCE,
    WARPS_MENU_INIT_OPTION_X_NAUT_FORTRESS_FACTORY,
    WARPS_MENU_INIT_OPTION_PALACE_OF_SHADOW_ENTRANCE,
    WARPS_MENU_INIT_OPTION_PALACE_OF_SHADOW_OUTSIDE_RIDDLE_TOWER,
    WARPS_MENU_INIT_OPTION_PALACE_OF_SHADOW_ROOM_BEFORE_GRODUS,
    WARPS_MENU_INIT_OPTION_PIT_OF_100_TRIALS,
    WARPS_MENU_INIT_OPTION_TITLE_SCREEN,
};

enum WarpsMenuBossOptions
{
    WARPS_MENU_BOSS_OPTION_CRUMP_PROLOGUE = 0,
    WARPS_MENU_BOSS_OPTION_GUS,
    WARPS_MENU_BOSS_OPTION_BLOOPER,
    WARPS_MENU_BOSS_OPTION_GOLD_FUZZY,
    WARPS_MENU_BOSS_OPTION_RED_BONES,
    WARPS_MENU_BOSS_OPTION_HOOKTAIL,
    WARPS_MENU_BOSS_OPTION_SHADOW_SIRENS_CH2,
    WARPS_MENU_BOSS_OPTION_MAGNUS_CH2,
    WARPS_MENU_BOSS_OPTION_MACHO_GRUBBA,
    WARPS_MENU_BOSS_OPTION_ATOMIC_BOO,
    WARPS_MENU_BOSS_OPTION_DOOPLISS_1,
    WARPS_MENU_BOSS_OPTION_DOOPLISS_2,
    WARPS_MENU_BOSS_OPTION_CORTEZ,
    WARPS_MENU_BOSS_OPTION_CRUMP_CH5,
    WARPS_MENU_BOSS_OPTION_SMORG,
    WARPS_MENU_BOSS_OPTION_MAGNUS_CH8,
    WARPS_MENU_BOSS_OPTION_DARK_BONES,
    WARPS_MENU_BOSS_OPTION_GLOOMTAIL,
    WARPS_MENU_BOSS_OPTION_SHADOW_SIRENS_CH8,
    WARPS_MENU_BOSS_OPTION_GRODUS,
    WARPS_MENU_BOSS_OPTION_BOWSER_AND_KAMMY,
    WARPS_MENU_BOSS_OPTION_SHADOW_QUEEN_1,
    WARPS_MENU_BOSS_OPTION_SHADOW_QUEEN_2,
    WARPS_MENU_BOSS_OPTION_BONETAIL,
};

enum WarpsMenuWarpToMapReturnValue
{
    WARPS_MENU_WARP_TO_MAP_SUCCESS = 0,
    WARPS_MENU_WARP_TO_MAP_NOT_IN_GAME,
    WARPS_MENU_WARP_TO_MAP_INVALID_INDEX,
};

enum WarpsMenuInitFlag
{
    WARPS_MENU_INIT_FLAG_SELECTING_WARP_OPTION = 0,
};

class WarpsMenu
{
   public:
    WarpsMenu() {}
    ~WarpsMenu() {}

    ValueEditor *getValueEditorPtr() { return &this->valueEditor; }
    ErrorWindow *getErrorWindowPtr() { return &this->errorWindow; }

    MenuAutoIncrement *getAutoIncrementPtr() { return &this->autoIncrement; }

    uint32_t getCurrentIndex() const { return this->currentIndex; }
    uint8_t *getCurrentIndexPtr() { return &this->currentIndex; }
    void setCurrentIndex(uint32_t index) { this->currentIndex = static_cast<uint8_t>(index); }

    void initErrorWindow(const char *text);

    void drawSelectInitWarpInfo();
    void drawSelectBossWarpInfo() const;

   private:
    ValueEditor valueEditor;
    ErrorWindow errorWindow;

    MenuAutoIncrement autoIncrement;
    uint8_t currentIndex;
};

extern WarpsMenu *gWarpsMenu;
extern const char *gWarpsMenuCannotWarpText;

// warpsInit
void warpsMenuInit(Menu *menuPtr);
void warpsMenuInitControls(Menu *menuPtr, MenuButtonInput button);
void warpsMenuInitDraw(CameraId cameraId, void *user);
void warpsMenuInitExit();

void warpsMenuInitSelectWarp(Menu *menuPtr);

uint32_t getCurrentPitLevel();
void setCurrentPitLevel(uint32_t level);
void setNextMap(const char *map);
void setNextBero(const char *bero);

// warpsBoss
void warpsMenuBossInit(Menu *menuPtr);
void warpsMenuBossControls(Menu *menuPtr, MenuButtonInput button);
void warpsMenuBossDraw(CameraId cameraId, void *user);

void warpsMenuBossSelectWarp(Menu *menuPtr);

#endif
