#ifndef MENUS_WARPS_MENU_H
#define MENUS_WARPS_MENU_H

#include "customState.h"
#include "classes/menu.h"
#include "classes/valueEditor.h"
#include "classes/nameEditor.h"
#include "classes/errorwindow.h"
#include "ttyd/item_data.h"
#include "ttyd/mario_pouch.h"
#include "ttyd/evtmgr.h"

#include <cstdint>

#define WARPS_MENU_INIT_MAX_OPTIONS_PER_ROW 4
#define WARPS_MENU_BOSS_MAX_OPTIONS_PER_ROW 2

#define WARPS_TOTAL_EVENT_NAMES 494

#define WARPS_INDEX_TOTAL_UNUSED_MAPS 7
#define WARPS_INDEX_MAX_INDEX 278

#define WARPS_INDEX_MAX_ENTRANCES_SINGLE_PAGE 13 // Used when there is only one page

#define WARPS_INDEX_MAX_ENTRANCES_MULTIPLE_PAGES \
    (WARPS_INDEX_MAX_ENTRANCES_SINGLE_PAGE - 1) // Used when there is more than one page

#define MAX_CUSTOM_STATES_PER_PAGE 18

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

enum WarpsMenuIndexOptions
{
    WARPS_MENU_INDEX_OPTION_SELECT_NEW_MAP = 0,
    WARPS_MENU_INDEX_OPTION_SELECT_NEW_ENTRANCE_ID,
    WARPS_MENU_INDEX_OPTION_VIEW_CURRENT_MAP_ENTRANCES,
    WARPS_MENU_INDEX_OPTION_WARP,
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
    WARPS_MENU_WARP_TO_MAP_FATAL_ERROR,
};

enum WarpsMenuInitFlag
{
    WARPS_MENU_INIT_FLAG_SELECTING_WARP_OPTION = 0,
};

enum WarpsMenuCustomStatesFlag
{
    WARPS_MENU_CUSTOM_STATES_FLAG_LOAD = 0,
    WARPS_MENU_CUSTOM_STATES_FLAG_DUPLICATE,
    WARPS_MENU_CUSTOM_STATES_FLAG_SWAP_MOVE_INIT,
    WARPS_MENU_CUSTOM_STATES_FLAG_SWAP_MOVE_SELECTED_STATE_TO_SWAP,
    WARPS_MENU_CUSTOM_STATES_FLAG_OVERWRITE,
    WARPS_MENU_CUSTOM_STATES_FLAG_RENAME,
    WARPS_MENU_CUSTOM_STATES_FLAG_DELETE,
};

struct WarpByEventInventory
{
    ItemId items[sizeof(PouchData::items) / sizeof(ItemId)];
    ItemId badges[sizeof(PouchData::badges) / sizeof(ItemId)];
    ItemId equippedBadges[sizeof(PouchData::equippedBadges) / sizeof(ItemId)];
};

struct WarpByEventDetails
{
    const char *getStagePtr()
    {
#ifdef TTYD_JP
        return this->stage;
#else
        return this->stagePtr;
#endif
    }

#ifdef TTYD_JP
    char stage[8];
#else
    const char *stagePtr;
#endif

    const char *eventPtr;
    const char *partnerPtr;
    const char *followerPtr;
    const char *mapPtr;
    const char *beroPtr;
    uint16_t sequencePosition;
};

struct MapAndBeroDetails
{
    uint32_t mapColor;
    const char *mapTextPtr;
    uint32_t beroColor;
    const char *beroTextPtr;
};

class WarpsMenu
{
   public:
    WarpsMenu() {}
    ~WarpsMenu() {}

    ValueEditor *getValueEditorPtr() { return &this->valueEditor; }
    NameEditor *getNameEditorPtr() { return &this->nameEditor; }
    ErrorWindow *getErrorWindowPtr() { return &this->errorWindow; }

    MenuAutoIncrement *getAutoIncrementPtr() { return &this->autoIncrement; }

    uint32_t getCurrentIndex() const { return this->currentIndex; }
    uint8_t *getCurrentIndexPtr() { return &this->currentIndex; }
    void setCurrentIndex(uint32_t index) { this->currentIndex = static_cast<uint8_t>(index); }

    uint32_t getSelectedIndex() const { return this->selectedIndex; }
    void setSelectedIndex(uint32_t index) { this->selectedIndex = static_cast<uint8_t>(index); }

    uint32_t getCurrentPage() const { return this->currentPage; }
    void setCurrentPage(uint32_t page) { this->currentPage = static_cast<uint8_t>(page); }

    char *getStateNamePtr() { return this->stateName; }

    bool initSwapMoveStates(Menu *menuPtr);
    void initErrorWindow(const char *text);
    void customStatesFlagSetControls(MenuButtonInput button);

    void drawSelectInitWarpInfo();
    void drawSelectEventWarpInfo(float offsetY) const;
    void drawSelectIndexWarpInfo() const;
    void drawSelectBossWarpInfo() const;
    void drawCustomStatesInfo() const;

   private:
    ValueEditor valueEditor;
    NameEditor nameEditor;
    ErrorWindow errorWindow;

    MenuAutoIncrement autoIncrement;

    uint8_t currentIndex;
    uint8_t selectedIndex; // Custom States
    uint8_t currentPage;   // Custom States

    char stateName[CUSTOM_STATE_NAME_SIZE + 1]; // Custom States - Temporary name buffer
};

extern WarpsMenu *gWarpsMenu;
extern const char *gWarpsMenuCannotWarpText;

#ifdef TTYD_JP
extern const char *gWarpsEventNames[WARPS_TOTAL_EVENT_NAMES];
#endif

// warpsInit
void warpsMenuInit(Menu *menuPtr);

void warpsMenuEventIndexControls(Menu *menuPtr, MenuButtonInput button);
void warpsMenuCloseValueEditor();

uint32_t getCurrentPitLevel();
void setCurrentPitLevel(uint32_t level);
void setNextMap(const char *map);
void setNextBero(const char *bero);

// warpsEvent
void warpsMenuEventInit(Menu *menuPtr);

void handleWarpByEvent();
void getMapAndBeroTextAndColor(const char *mapTextPtr, const char *beroTextPtr, MapAndBeroDetails *mapAndBeroDetailsPtr);
void drawEventDetails(uint32_t index, float posX, float posY);

// warpsIndex
void warpsMenuIndexInit(Menu *menuPtr);

const char *getMapFromIndex(uint32_t index);
MapData *mapDataPtrHandleUnusedMaps(const char *mapName);
int32_t setIndexWarpEntrances(EvtEntry *evtPtr, bool isFirstCall);
void unloadClearCurrentMapInitScript(const char *currentMap, const char *nextMap, const char *nextBero);
void relSetEvtAddrSetCurrentMapInitScript(const char *mapName, const void *pInitEvtCode);

// warpsIndexViewEntrances
void warpsMenuIndexViewEntrancesInit(Menu *menuPtr);

// warpsBoss
void warpsMenuBossInit(Menu *menuPtr);

// warpsCustomStates
void warpsMenuCustomStatesInit(Menu *menuPtr);

#endif
