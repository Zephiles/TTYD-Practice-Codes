#ifndef MENUS_WARPS_MENU_H
#define MENUS_WARPS_MENU_H

#include "menuUtils.h"
#include "classes/valueEditor.h"
#include "classes/errorwindow.h"
#include "ttyd/camdrv.h"

#include <cstdint>

#define WARPS_MENU_INIT_MAX_OPTIONS_PER_ROW 4

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
    void setCurrentIndex(uint32_t index) { this->currentIndex = static_cast<uint8_t>(index); }

    void initFlagSetControls(MenuButtonInput button);
    void initErrorWindow(const char *text);

    void drawSelectWarpInfo();

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

void setNextMap(const char *map);
void setNextBero(const char *bero);

#endif
