#pragma once

#include <gc/card.h>

#include <cstdint>

namespace mod {

enum MENU_NAMES
{
	ROOT = 0,
	INVENTORY,
	CHEATS,
	STATS,
	SETTINGS,
	MEMORY,
	BATTLES,
	DISPLAYS,
	WARPS,
	INVENTORY_MAIN,
	CHEATS_CHANGE_SEQUENCE,
	CHEATS_STANDARD,
	CHEATS_NO_BUTTON_COMBO,
	CHEATS_NPC_FORCE_DROP,
	CHEATS_CLEAR_AREA_FLAGS,
	STATS_MARIO,
	STATS_PARTNERS,
	STATS_FOLLOWERS,
	MEMORY_MODIFY,
	MEMORY_CHANGE_ADDRESS,
	BATTLES_CURRENT_ACTOR,
	BATTLES_STATUSES,
	DISPLAYS_ONSCREEN_TIMER,
	DISPLAYS_NO_BUTTON_COMBO,
};

enum INVENTORY_MENU_NAMES
{
	INVENTORY_STANDARD = 0,
	INVENTORY_IMPORTANT,
	INVENTORY_BADGES,
	INVENTORY_STORED_ITEMS,
};

enum INVENTORY_SELECTION
{
	ADD_BY_ID = 1,
	ADD_BY_ICON,
	DUPLICATE,
	CHANGE_BY_ID,
	CHANGE_BY_ICON,
	DELETE,
};

enum INVENTORY_SELECTION_ERRORS
{
	INVENTORY_EMPTY = -2,
	INVENTORY_FULL,
};

enum CHEATS_OPTIONS
{
	CHANGE_SEQUENCE = 0,
	WALK_THROUGH_WALLS,
	SAVE_COORDINATES,
	LOAD_COORDINATES,
	SPAWN_ITEM,
	SAVE_ANYWHERE,
	TEXT_STORAGE,
	TIME_STOP_TEXT_STORAGE,
	SPEED_UP_MARIO,
	DISABLE_BATTLES,
	AUTO_ACTION_COMMANDS,
	INFINITE_ITEM_USAGE,
	RELOAD_ROOM,
	LEVITATE,
	LOCK_MARIO_HP_TO_MAX,
	RUN_FROM_BATTLES,
	DISABLE_MENU_SOUNDS,
	BOBBERY_EARLY,
	FORCE_ITEM_DROP,
	CLEAR_AREA_FLAGS,
};

enum CHEATS_CHANGE_SEQUENCE_SELECTION
{
	CHANGE_SEQUENCE_VALUE = 1,
};

enum CHEATS_STANDARD_SELECTION
{
	TURN_ON_OR_OFF = 1,
	CHANGE_BUTTON_COMBO,
};

enum CHEATS_FORCE_ITEM_DROP_SELECTION
{
	ITEM_DROP_TURN_ON_OR_OFF = 1,
	ITEM_DROP_CHANGE_BY_ID,
};

enum CHEATS_CLEAR_AREA_FLAGS_SELECTION
{
	SELECT_AREA = 1,
	CLEAR_FLAGS,
};

enum CHEATS_CLEAR_AREA_FLAGS_AREAS
{
	AREA_GOR = 0,
	AREA_TIK,
	AREA_HEI,
	AREA_NOK,
	AREA_GON,
	AREA_WIN,
	AREA_MRI,
	AREA_TOU,
	AREA_USU,
	AREA_GRA,
	AREA_JIN,
	AREA_MUJ,
	AREA_DOU,
	AREA_HOM,
	AREA_RSH,
	AREA_EKI,
	AREA_PIK,
	AREA_BOM,
	AREA_MOO,
	AREA_AJI,
	AREA_LAS,
	AREA_JON,
};

enum ADJUSTABLE_MENU_VALUES
{
	NO_NUMBERS_TO_DISPLAY 	= 0x1000,
	ADDING_BY_ID 			= 0x1008,
};

enum CHEATS_SPAWN_ITEM_MENU_VALUE
{
	SPAWN_ITEM_MENU_VALUE = 0x7000,
};

enum STATS_MARIO_SELECTION_OPTIONS
{
	COINS = 1,
	MARIO_HP,
	MARIO_FP,
	BP,
	MARIO_MAX_HP,
	MARIO_MAX_FP,
	MARIO_LEVEL,
	MARIO_RANK,
	STAR_POINTS,
	STAR_PIECES,
	SHINE_SPRITES,
	SPECIAL_MOVES,
	STAR_POWER,
	MAX_STAR_POWER,
	SHOP_POINTS,
	PIANTAS_STORED,
	CURRENT_PIANTAS,
};

enum STATS_PARTNER_SELECTION_OPTIONS
{
	PARTNER_HP = 1,
	PARTNER_MAX_HP,
	PARTNER_RANK,
	TOGGLE,
	BRING_OUT,
	REMOVE,
};

enum STATS_FOLLOWER_SELECTION_OPTIONS
{
	BRING_OUT_FOLLOWER = 1,
	REMOVE_FOLLOWER,
};

enum SETTINGS_OPTIONS
{
	LOAD_SETTINGS = 1,
	SAVE_SETTINGS,
};

enum SETTINGS_RETURN_CODES
{
	LOAD_FAILED = 1,
	LOAD_FAILED_NO_FILE,
	LOAD_SUCCESSFUL,
	SAVE_FAILED,
	SAVE_SUCCESSFUL,
};

enum MEMORY_TYPES
{
	string = 0,
	time,
	s8,
	s16,
	s32,
	s64,
	u8,
	u16,
	u32,
	u64,
	f32,
	f64,
};

enum MEMORY_OPTIONS
{
	ADD_WATCH = 1,
	MODIFY_WATCH,
	DELETE_WATCH,
};

enum MEMORY_MODIFY_OPTIONS
{
	CHANGE_ADDRESS = 1,
	CHANGE_TYPE,
	SHOW_AS_HEX,
	CHANGE_WATCH_POSITION,
	DISPLAY_OUT_OF_MENU,
};

enum MEMORY_MODIFY_ADDRESS_OPTIONS
{
	CHANGE_ADDRESS_OR_POINTERS = 1,
	ADD_POINTER_LEVEL,
	REMOVE_POINTER_LEVEL,
};

enum MEMORY_ERRORS
{
	ALL_SLOTS_EMPTY = -2,
	NO_SLOTS_LEFT,
};

enum BATTLES_CURRENT_ACTOR_STATS_SELECTION
{
	CHANGE_ACTOR_HP = 1,
	CHANGE_ACTOR_MAX_HP,
	CHANGE_ACTOR_FP,
	CHANGE_ACTOR_MAX_FP,
	CHANGE_HELD_ITEM,
	CLEAR_HELD_ITEM,
	CHANGE_ACTOR_STATUSES,
};

enum BATTLES_STATUSES_SELECTION
{
	SLEEP_TURNS_LEFT = 1,
	IMMOBILIZED_TURNS_LEFT,
	DIZZY_TURNS_LEFT,
	POISON_TURNS_LEFT,
	POISON_DAMAGE_AMOUNT,
	CONFUSED_TURNS_LEFT,
	ELECTRIFIED_TURNS_LEFT,
	DODGY_TURNS_LEFT,
	BURN_TURNS_LEFT,
	FROZEN_TURNS_LEFT,
	BIG_SHRINK_TURNS_LEFT,
	BIG_SHRINK_POWER_AMOUNT,
	ATTACK_UP_DOWN_TURNS_LEFT,
	ATTACK_UP_DOWN_POWER_AMOUNT,
	DEFENSE_UP_DOWN_TURNS_LEFT,
	DEFENSE_UP_DOWN_POWER_AMOUNT,
	CHARGED_TURNS_LEFT,
	ALLERGIC_TURNS_LEFT,
	INVISIBLE_TURNS_LEFT,
	PAYBACK_TURNS_LEFT,
	FAST_TURNS_LEFT,
	SLOW_TURNS_LEFT,
	HP_REGEN_TURNS_LEFT,
	HP_REGEN_AMOUNT,
	FP_REGEN_TURNS_LEFT,
	FP_REGEN_AMOUNT,
	DEFEATED_FLAG,
};

enum DISPLAYS_OPTIONS
{
	ONSCREEN_TIMER = 0,
	MARIO_COORDINATES,
	JUMP_STORAGE,
	BUTTON_INPUT_DISPLAY,
	STICK_ANGLE,
	GUARD_SUPERGUARD_TIMINGS,
	ART_ATTACK_HITBOXES,
	YOSHI_SKIP,
	PALACE_SKIP,
};

enum ONSCREEN_TIMER_OPTIONS
{
	ONSCREEN_TIMER_TURN_ON_OR_OFF = 1,
	CHANGE_START_PAUSE_RESUME_BUTTON_COMBO,
	CHANGE_RESET_BUTTON_COMBO,
};

enum DISPLAYS_NO_BUTTON_COMBO_OPTIONS
{
	DISPLAYS_TURN_ON_OR_OFF = 1,
};

enum ONSCREEN_TIMER_BUTTON_CODES
{
	START_PAUSE_RESUME = 0,
	RESET,
};

enum WARPS_OPTIONS
{
	SELECT_WARP = 1,
};

enum WARPS_SELECTION_OPTIONS
{
	ROGUEPORT_CENTRAL = 0,
	ROGUEPORT_SEWERS_UNDERGROUND_SHOP,
	PETAL_MEADOWS_ENTRANCE,
	HOOKTAIL_CASTLE_ENTRANCE,
	BOGGLY_WOODS_ENTRANCE,
	THE_GREAT_TREE_ENTRANCE,
	GLITZVILLE_LOBBY,
	GLITZVILLE_GRUBBAS_OFFICE,
	TWILIGHT_TOWN_WEST_SIDE,
	CREEPY_STEEPLE_ENTRANCE,
	KEELHAUL_KEY_SHANTYTOWN,
	PIRATES_GROTTO_ENTRANCE,
	EXCESS_EXPRESS_PASSENGER_CAR_CABINS_3_5_DAY,
	RIVERSIDE_STATION_ENTRANCE,
	POSHLEY_HEIGHTS_STATION,
	FAHR_OUTPOST_PIPE_ENTRANCE,
	MOON_ENTRANCE,
	X_NAUT_FORTRESS_ENTRANCE,
	X_NAUT_FORTRESS_FACTORY,
	PALACE_OF_SHADOW_ENTRANCE,
	PALACE_OF_SHADOW_OUTSIDE_RIDDLE_TOWER,
	PALACE_OF_SHADOW_ROOM_BEFORE_GRODUS,
	PALACE_OF_SHADOW_SHADOW_QUEEN_BATTLE_2,
	PIT_OF_100_TRIALS,
	TITLE_SCREEN,
};

enum WARPS_IN_ADJUSTABLE_VALUE_MENU_OPTIONS
{
	SELECTING_VALUE = 1,
};

enum CODE_RETURN_VALUES
{
	UNKNOWN_BEHAVIOR = -4,
	NULL_PTR,
	NOT_IN_BATTLE,
	NOT_IN_GAME,
	SUCCESS,
};

enum PAD_INPUTS
{
	PAD_DPAD_LEFT 	= 0x0001,
	PAD_DPAD_RIGHT 	= 0x0002,
	PAD_DPAD_DOWN 	= 0x0004,
	PAD_DPAD_UP 	= 0x0008,
	PAD_Z 			= 0x0010,
	PAD_R 			= 0x0020,
	PAD_L 			= 0x0040,
	// unused 		= 0x0080,
	PAD_A 			= 0x0100,
	PAD_B 			= 0x0200,
	PAD_X 			= 0x0400,
	PAD_Y 			= 0x0800,
	PAD_START 		= 0x1000,
};

enum CUSTOM_BUTTON_IDS
{
	DPADLEFT = 1,
	DPADRIGHT,
	DPADDOWN,
	DPADUP,
	Z,
	R,
	L,
	A,
	B,
	X,
	Y,
	START,
};

struct ItemData
{
	const char*		item_string_id;	// Used for recipe lookups, etc.
	const char*		item_name_msg;
	const char*		item_desc_msg;
	const char*		item_desc_menu_msg;
	uint16_t 		_unk_0x10;
	int16_t			type_sort_order; // For "By Type" ordering
	int16_t			buy_price;
	int16_t			discount_price;
	int16_t			star_piece_price;
	int16_t 		sell_price;
	int8_t 			bp_cost;
	int8_t 			hp_restored;
	int8_t 			fp_restored;
	uint8_t 		_unk_0x1f;
	int16_t 		icon_id;
	uint16_t 		_unk_0x22;
	void* 			attack_params;
} __attribute__((__packed__));

struct Menus
{
	uint8_t TotalMenuOptions;
	uint8_t TotalMenuColumns;
	uint8_t ColumnSplitAmount;
	uint8_t PreviousMenu;
	const char **Line;
	
	Menus()
	{
		TotalMenuColumns 	= 1;
		ColumnSplitAmount 	= 1;
	}
};

struct Cheats
{
	uint16_t ButtonCombo;
	bool Active;
	
	Cheats()
	{
		Active = true;
	}
};

struct AutoIncrement
{
	uint16_t WaitFramesToBeginIncrement;
	uint16_t WaitFramesToPerformIncrement;
};

struct AutoIncrementCoordinates
{
	uint16_t WaitFramesToBeginIncrement;
	uint16_t WaitFramesToPerformIncrement;
	int32_t PosX;
	int32_t PosY;
};

struct CheatsHandleDisplayButtons
{
	uint8_t CheatsBButtonCounter;
	uint8_t CheatsPreviousButtonsHeld[14]; // Extra spot for a 0 at the end of the array
	uint8_t CheatsCurrentButtonsHeld[14]; // Extra spot for a 0 at the end of the array
};

struct MarioPartnerPositionsStruct
{
	float MarioPosition[4];
	float PartnerPosition[4];
};

struct SaveAnywhereStruct
{
	uint32_t ThreadID;
	bool ScriptIsRunning;
};

struct SpeedUpMarioStruct
{
	float MarioVar[2];
	uint8_t ValuesChangedState;
};

struct ReloadRoomStruct
{
	bool SystemLevelShouldBeLowered;
	char NewBero[32]; // 31 bytes for NextBero, 1 byte for NULL
	char NewMap[9]; // 8 bytes for NextMap, 1 byte for NULL
};

struct SpawnItems
{
	uint16_t SpawnItemCounter;
	bool InAdjustableValueMenu;
};

struct ClearAreaFlagsStruct
{
	uint8_t CurrentOption;
	bool FlagsShouldBeCleared;
	bool StartClearingFlags;
};

struct ClearCacheForBattlesStruct
{
	bool MarioStatsShouldBeCleared;
	bool PartnerStatsShouldBeCleared;
};

struct TrickDisplay
{
	uint32_t MainTimer;
	uint16_t ResetTimer;
	bool TimerPaused;
	bool TimerStopped;
	
	TrickDisplay()
	{
		TimerStopped = true;
	}
};

struct OnScreenTimerDisplay
{
	uint32_t MainTimer;
	uint16_t ButtonCombo[2];
	bool TimerPaused;
	
	OnScreenTimerDisplay()
	{
		TimerPaused = true;
	}
};

struct DisplayActionCommandTiming
{
	int32_t (*Trampoline)(void *, void *);
	uint16_t DisplayTimer;
	uint8_t TypeToDraw;
	int8_t Last_A_Frame;
	int8_t Last_B_Frame;
};

struct MemoryCardStruct
{
	uint8_t ReturnCode;
	// uint8_t CurrentAction;
	gc::card::card_file FileInfo[1];
	uint8_t *WorkArea;
	char *FileName;
	char *Description;
};

struct MemoryWatchStruct
{
	uint32_t Address;
	int32_t AddressOffset[10];
	uint8_t AddressOffsetAmount;
	uint8_t Type;
	bool ShowAsHex;
	bool Display;
	int32_t PosX;
	int32_t PosY;
};

struct SettingsStruct
{
	bool CheatsActive[100];
	uint16_t CheatButtonCombos[100];
	bool DisplaysActive[100];
	uint16_t DisplaysButtonCombos[100];
	MemoryWatchStruct MemoryWatchSettings[30];
};

extern Menus Menu[24];
extern Cheats Cheat[19];
extern bool Displays[9];
extern char DisplayBuffer[256];
extern MemoryWatchStruct MemoryWatch[30];

extern AutoIncrement AdjustableValueMenu;
extern AutoIncrement MemoryWatchAdjustableValueMenu;
extern AutoIncrementCoordinates MemoryWatchPosition;
extern CheatsHandleDisplayButtons CheatsDisplayButtons;
extern MarioPartnerPositionsStruct MarioPartnerPositions;
extern SaveAnywhereStruct SaveAnywhere;
extern SpeedUpMarioStruct SpeedUpMario;
extern ReloadRoomStruct ReloadRoom;
extern SpawnItems SpawnItem;
extern ClearAreaFlagsStruct ClearAreaFlags;
extern ClearCacheForBattlesStruct ClearCacheForBattles;
extern TrickDisplay YoshiSkip;
extern TrickDisplay PalaceSkip;
extern OnScreenTimerDisplay OnScreenTimer;
extern DisplayActionCommandTiming DisplayActionCommands;
extern MemoryCardStruct MenuSettings;

extern const char *VersionNumber;
extern uint8_t CheatsOrder[];
extern const char *CheatsLines[];
extern const char *CheatsForceItemDropAreas[];
extern uint8_t CheatsForceItemDropAreasSize;
extern const char *CheatsForceItemDropAreasFullNames[];
extern const char *StatsMarioOptionsLines[];
extern uint8_t StatsMarioOptionsLinesSize;
extern uint16_t StatsMarioIcons[];
extern const char *MarioStatsSpecialMovesOptions[];
extern uint8_t MarioStatsSpecialMovesOptionsSize;
extern const char *StatsPartnerOptionsLines[];
extern uint8_t StatsPartnerOptionsLinesSize;
extern const char *StatsFollowerOptionsLines[];
extern uint8_t StatsFollowerOptionsLinesSize;
extern const char *MemoryModifyLines[];
extern uint8_t MemoryModifyLinesSize;
extern const char *MemoryTypeLines[];
extern uint8_t MemoryTypeLinesSize;
extern const char *BattlesActorsLines[];
extern const char *BattlesCurrentActorStats[];
extern uint8_t BattlesCurrentActorStatsSize;
extern const char *BattlesStatusesLines[];
extern uint8_t BattlesStatusesLinesSize;
extern uint16_t BattlesStatusesIcons[];
extern uint8_t DisplaysOrder[];
extern const char *DisplaysLines[];
extern const char *OnScreenTimerOptions[];
extern uint8_t OnScreenTimerOptionsSize;
extern const char *WarpDestinations[];
extern uint8_t WarpDestinationsSize;
extern const char *WarpDescriptions[];

#ifndef TTYD_JP
extern const char ButtonInputDisplay[];
#else
extern const char *ButtonInputDisplay[];
extern const char *PointerText;
#endif

extern bool HideMenu;
extern bool MenuIsDisplayed;
extern bool PreventClosingMenu;
extern bool ChangingCheatButtonCombo;
extern uint8_t CurrentMenu;
extern uint8_t CurrentMenuOption;
extern uint8_t MenuSelectedOption;
extern uint8_t SecondaryMenuOption;
extern uint8_t SelectedOption;
extern uint8_t CurrentPage;
extern uint8_t SecondaryPage;
extern int8_t FunctionReturnCode;
extern uint32_t Timer;
extern uint8_t MenuSelectionStates;
extern int32_t MenuSecondaryValue;
extern uint32_t MemoryWatchSecondaryValue;
extern uint8_t FrameCounter;

extern uint32_t r13;
extern char *NextBero;
extern char *NextMap;
extern char *NextArea;
extern ItemData *ItemDataTable;
extern uint8_t *GuardFrames;
extern uint8_t *SuperguardFrames;
extern uint32_t PauseMenuStartAddress;
extern uint32_t wp_fadedrv_Address;
extern uint32_t _mapEntAddress;
extern uint32_t NPCAddressesStart;
extern uint32_t BattleAddressesStart;
extern uint32_t PiantaParlorAddressesStart;
extern uint32_t seqMainAddress;
extern uint32_t WorkAreaAddress;
extern uint32_t FieldItemsAddressesStart;

extern uint32_t GlobalWorkPointer;
extern uint32_t titleMainAddress;
extern uint32_t ConsoleBusSpeedAddress;

extern bool ResetMarioProperties;
extern int16_t ForcedNPCItemDrop;
extern bool MarioFreeze;
extern uint16_t JumpStorageSetCounter;

void initMenuVars();
void assignCheatButtonCombo(uint32_t cheat);
void assignDisplayButtonCombo(uint32_t display);
void initArtAttackAssemblyOverwrites();
void initAssemblyOverwrites();

}