#ifndef MENUS_WARPS_MENU_H
#define MENUS_WARPS_MENU_H

#include "menuUtils.h"
#include "classes/valueEditor.h"
#include "classes/errorwindow.h"
#include "ttyd/item_data.h"
#include "ttyd/mario_pouch.h"
#include "ttyd/camdrv.h"
#include "ttyd/party.h"
#include "ttyd/evtmgr.h"
#include "ttyd/mapdata.h"

#include <cstdint>

#define WARPS_MENU_INIT_MAX_OPTIONS_PER_ROW 4
#define WARPS_MENU_BOSS_MAX_OPTIONS_PER_ROW 2

#define WARPS_TOTAL_EVENT_NAMES 494

#define WARPS_INDEX_TOTAL_UNUSED_MAPS 7
#define WARPS_INDEX_MAX_INDEX 278

#define WARPS_INDEX_MAX_ENTRANCES_SINGLE_PAGE 13 // Used when there is only one page

#define WARPS_INDEX_MAX_ENTRANCES_MULTIPLE_PAGES \
    (WARPS_INDEX_MAX_ENTRANCES_SINGLE_PAGE - 1) // Used when there is more than one page

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

struct WarpByEventInventory
{
    ItemId items[sizeof(PouchData::items) / sizeof(ItemId)];
    ItemId badges[sizeof(PouchData::badges) / sizeof(ItemId)];
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
    ErrorWindow *getErrorWindowPtr() { return &this->errorWindow; }

    MenuAutoIncrement *getAutoIncrementPtr() { return &this->autoIncrement; }

    uint32_t getCurrentIndex() const { return this->currentIndex; }
    uint8_t *getCurrentIndexPtr() { return &this->currentIndex; }
    void setCurrentIndex(uint32_t index) { this->currentIndex = static_cast<uint8_t>(index); }

    void initErrorWindow(const char *text);

    void drawSelectInitWarpInfo();
    void drawSelectEventWarpInfo(float offsetY) const;
    void drawSelectIndexWarpInfo() const;
    void drawSelectBossWarpInfo() const;

   private:
    ValueEditor valueEditor;
    ErrorWindow errorWindow;

    MenuAutoIncrement autoIncrement;
    uint8_t currentIndex;
};

extern WarpsMenu *gWarpsMenu;
extern const char *gWarpsMenuCannotWarpText;
extern const char *gUnusedMaps[WARPS_INDEX_TOTAL_UNUSED_MAPS];

#ifdef TTYD_JP
extern const char *gWarpsEventNames[WARPS_TOTAL_EVENT_NAMES];
#endif

// warpsInit
void warpsMenuInit(Menu *menuPtr);
void warpsMenuInitControls(Menu *menuPtr, MenuButtonInput button);
void warpsMenuInitDraw(CameraId cameraId, void *user);
void warpsMenuInitExit();

void warpsMenuInitSelectWarp(Menu *menuPtr);
void warpsMenuEventIndexControls(Menu *menuPtr, MenuButtonInput button);
void warpsMenuCloseValueEditor();

uint32_t getCurrentPitLevel();
void setCurrentPitLevel(uint32_t level);
void setNextMap(const char *map);
void setNextBero(const char *bero);

// warpsEvent
void warpsMenuEventInit(Menu *menuPtr);
void warpsMenuEventDraw(CameraId cameraId, void *user);

void warpsMenuEventSelectEvent(Menu *menuPtr);
void warpsMenuEventToggleKeepInventory(Menu *menuPtr);
void warpsMenuEventToggleSetFlags(Menu *menuPtr);
void warpsMenuEventWarp(Menu *menuPtr);

void handleWarpByEvent();
uint32_t getTotalStageEvents();
bool indexToStageAndEvent(uint32_t index, int32_t stageEventIdsOut[2]);
bool checkForValidStageAndEvent(int32_t stageId, int32_t eventId);
uint32_t getSequenceForEvent(int32_t stageId, int32_t eventId);
const char *getPartyName(PartyMembers id);
void getMapAndBeroTextAndColor(const char *mapTextPtr, const char *beroTextPtr, MapAndBeroDetails *mapAndBeroDetailsPtr);
bool getEventDetails(uint32_t index, WarpByEventDetails *warpByEventDetailsPtr);
void drawEventDetails(uint32_t index, float posX, float posY);

// warpsIndex
void warpsMenuIndexInit(Menu *menuPtr);
void warpsMenuIndexDraw(CameraId cameraId, void *user);

void warpsMenuIndexSelectNewMapOrEntranceId(Menu *menuPtr);
void warpsMenuIndexWarp(Menu *menuPtr);

const char *getMapFromIndex(uint32_t index);
int32_t getMapIndex();
MapData *mapDataPtrHandleUnusedMaps(const char *mapName);
int32_t setIndexWarpEntrances(EvtEntry *evtPtr, bool isFirstCall);
void unloadClearCurrentMapInitScript(const char *currentMap, const char *nextMap, const char *nextBero);
void relSetEvtAddrSetCurrentMapInitScript(const char *mapName, const void *pInitEvtCode);

// warpsIndexViewEntrances
void warpsMenuIndexViewEntrancesInit(Menu *menuPtr);
void warpsMenuIndexViewEntrancesControls(Menu *menuPtr, MenuButtonInput button);
void warpsMenuIndexViewEntrancesDraw(CameraId cameraId, void *user);

// warpsBoss
void warpsMenuBossInit(Menu *menuPtr);
void warpsMenuBossControls(Menu *menuPtr, MenuButtonInput button);
void warpsMenuBossDraw(CameraId cameraId, void *user);

void warpsMenuBossWarp(Menu *menuPtr);

#endif
