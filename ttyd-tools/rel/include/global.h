#pragma once

#include <gc/OSAlloc.h>
#include <ttyd/item_data.h>
#include <ttyd/mapdata.h>

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
    CHEATS_FRAME_ADVANCE,
    CHEATS_GENERATE_LAG_SPIKE,
    CHEATS_NPC_FORCE_DROP,
    CHEATS_RESOLVE_FADES,
    CHEATS_LOCK_FLAGS,
    CHEATS_MANAGE_FLAGS,
    CHEATS_MANAGE_FLAGS_MAIN,
    CHEATS_CLEAR_AREA_FLAGS,
    STATS_MARIO,
    STATS_PARTNERS,
    STATS_FOLLOWERS,
    MEMORY_WATCH,
    MEMORY_WATCH_MODIFY,
    MEMORY_WATCH_CHANGE_ADDRESS,
    MEMORY_EDITOR_SETUP,
    BATTLES_CURRENT_ACTOR,
    BATTLES_STATUSES,
    DISPLAYS_ONSCREEN_TIMER,
    DISPLAYS_MEMORY_USAGE,
    DISPLAYS_NO_BUTTON_COMBO,
    WARPS_EVENT,
    WARPS_INDEX,
    WARPS_BOSS,
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

// Inventory selection errors
#define INVENTORY_EMPTY -2
#define INVENTORY_FULL  -1

enum CHEATS_OPTIONS
{
    CHANGE_SEQUENCE = 0,
    WALK_THROUGH_WALLS,
    FALL_THROUGH_FLOORS,
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
    AUTO_MASH_TEXT,
    FRAME_ADVANCE,
    GENERATE_LAG_SPIKE,
    LOCK_MARIO_HP_TO_MAX,
    RUN_FROM_BATTLES,
    DISABLE_MENU_SOUNDS,
    BOBBERY_EARLY,
    FORCE_ITEM_DROP,
    RESOLVE_FADES,
    LOCK_FLAGS,
    MANAGE_FLAGS,
    CLEAR_AREA_FLAGS,
};

enum CHEATS_CHANGE_SEQUENCE_SELECTION
{
    CHANGE_SEQUENCE_VALUE = 1,
};

enum CHEATS_STANDARD_SELECTION
{
    CHEATS_STANDARD_TURN_ON_OR_OFF = 1,
    CHANGE_BUTTON_COMBO,
};

enum CHEATS_FORCE_ITEM_DROP_SELECTION
{
    ITEM_DROP_TURN_ON_OR_OFF = 1,
    ITEM_DROP_CHANGE_BY_ID,
};

// Fades return values
#define FADE_NOT_ACTIVE      -2
#define FADE_DONT_RESOLVE    -1
#define FADE_RESOLVE_SUCCESS 1

enum CHEATS_FRAME_ADVANCE
{
    CHEATS_FRAME_ADVANCE_TURN_ON_OR_OFF = 1,
    CHEATS_FRAME_ADVANCE_CHANGE_ADVANCE_FRAME_BUTTON_COMBO,
    CHEATS_FRAME_ADVANCE_CHANGE_PAUSE_BUTTON_COMBO,
};

enum CHEATS_GENERATE_LAG_SPIKE_SELECTION
{
    CHEATS_GENERATE_LAG_SPIKE_TURN_ON_OR_OFF = 1,
    CHEATS_GENERATE_LAG_SPIKE_CHANGE_BUTTON_COMBO,
    CHEATS_GENERATE_LAG_SPIKE_SET_DURATION,
};

enum CHEATS_LOCK_FLAGS_SELECTION
{
    LOCK_GSW_FLAGS = 1,
    LOCK_GSWF_FLAGS,
    LOCK_GW_FLAGS,
    LOCK_GF_FLAGS,
    LOCK_LSW_FLAGS,
    LOCK_LSWF_FLAGS,
    SET_NEW_LSW_AREA,
    SET_NEW_LSWF_AREA,
};

enum CHEATS_LOCK_FLAGS_INDEXES
{
    GSW = 0,
    GSWF,
    GW,
    GF,
    LSW,
    LSWF,
};

enum CHEATS_MANAGE_FLAGS_SELECTION
{
    SET_GSW = 1,
    SET_GSWF,
    SET_GW,
    SET_GF,
    SET_LSW,
    SET_LSWF,
};

enum CHEATS_MANAGE_GLOBAL_WORDS_SELECTION
{
    CHANGE_GLOBAL_WORD = 0,
    CHANGE_GLOBAL_WORD_VALUE,
    SET_GLOBAL_WORD_VALUE,
};

enum CHEATS_MANAGE_GLOBAL_FLAGS_SELECTION
{
    CHANGE_GLOBAL_FLAG = 0,
    SET_GLOBAL_FLAG,
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

// Various menu values
#define NO_NUMBERS_TO_DISPLAY              0x2000
#define ADDING_BY_ID                       0x2008
#define ADDING_BY_ICON                     0x2108
#define SPAWN_ITEM_MENU_VALUE              0x3000
#define STATS_PARTNER_DISPLAY_YOSHI_COLORS 100

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
};

enum STATS_FOLLOWER_SELECTION_OPTIONS
{
    BRING_OUT_FOLLOWER = 1,
    REMOVE_FOLLOWER,
};

enum SETTINGS_OPTIONS
{
    CHANGE_MEMORY_CARD_SLOT = 1,
    RENAME_SETTINGS_FILE,
    DELETE_SETTINGS_FILE,
    SAVE_SETTINGS,
    LOAD_SETTINGS,
};

enum SETTINGS_RETURN_CODES
{
    MEMCARD_IN_USE = -200,
    FILE_NOT_FOUND = 1,
    RENAME_FAILED,
    RENAME_SUCCESSFUL,
    FILE_ALREADY_RENAMED,
    DELETE_FAILED,
    DELETE_SUCCESSFUL,
    SAVE_FAILED,
    SAVE_SUCCESSFUL,
    LOAD_FAILED,
    LOAD_SUCCESSFUL,
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
    OPEN_WATCHES_MENU = 1,
    OPEN_EDITOR_MENU,
};

enum MEMORY_WATCH_OPTIONS
{
    ADD_WATCH = 1,
    MODIFY_WATCH,
    DUPLICATE_WATCH,
    DELETE_WATCH,
};

enum MEMORY_WATCH_MODIFY_OPTIONS
{
    CHANGE_ADDRESS = 1,
    CHANGE_TYPE,
    SHOW_AS_HEX,
    CHANGE_WATCH_POSITION,
    DISPLAY_OUT_OF_MENU,
};

enum MEMORY_WATCH_MODIFY_ADDRESS_OPTIONS
{
    CHANGE_ADDRESS_OR_POINTERS = 1,
    ADD_POINTER_LEVEL,
    REMOVE_POINTER_LEVEL,
};

// Memory watch error return values
#define ALL_SLOTS_EMPTY -2
#define NO_SLOTS_LEFT   -1

enum MEMORY_EDITOR_SETUP_OPTIONS
{
    ENABLE_EDITOR = 1,
    EDITOR_CHANGE_BUTTON_COMBO,
    EDITOR_OPEN_SETTINGS_MENU,
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
    ONSCREEN_TIMER_FRAME_COUNTER,
    MARIO_COORDINATES,
    MARIO_SPEED_XZ,
    JUMP_STORAGE,
    BUTTON_INPUT_DISPLAY,
    STICK_ANGLE,
    DPAD_OPTIONS_DISPLAY,
    GUARD_SUPERGUARD_TIMINGS,
    ART_ATTACK_HITBOXES,
    MEMORY_USAGE,
    EFFS_ACTIVE,
    EVTS_ACTIVE,
    ENEMY_ENCOUNTER_NOTIFIER,
    YOSHI_SKIP,
    PALACE_SKIP,
    BRIDGE_SKIP,
    BLIMP_TICKET_SKIP,
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

// On-screen timer button codse
#define START_PAUSE_RESUME 0
#define RESET              1

enum WARPS_OPTIONS
{
    SELECT_WARP = 1,
    WARP_BY_EVENT,
    WARP_BY_INDEX,
    WARP_TO_BOSS,
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
    PIT_OF_100_TRIALS,
    TITLE_SCREEN,
};

enum WARPS_IN_ADJUSTABLE_VALUE_MENU_OPTIONS
{
    SELECTING_VALUE = 1,
};

enum WARPS_EVENT_OPTIONS
{
    EVENT_SELECT_EVENT = 1,
    EVENT_KEEP_INVENTORY,
    EVENT_SET_FLAGS,
    EVENT_WARP_NOW,
};

enum WARPS_BOSS_OPTIONS
{
    BOSS_CRUMP_PROLOGUE = 0,
    BOSS_GUS,
    BOSS_BLOOPER,
    BOSS_GOLD_FUZZY,
    BOSS_RED_BONES,
    BOSS_HOOKTAIL,
    BOSS_SHADOW_SIRENS_CH2,
    BOSS_MAGNUS_CH2,
    BOSS_MACHO_GRUBBA,
    BOSS_ATOMIC_BOO,
    BOSS_DOOPLISS_1,
    BOSS_DOOPLISS_2,
    BOSS_CORTEZ,
    BOSS_CRUMP_CH5,
    BOSS_SMORG,
    BOSS_MAGNUS_CH8,
    BOSS_DARK_BONES,
    BOSS_GLOOMTAIL,
    BOSS_SHADOW_SIRENS_CH8,
    BOSS_GRODUS,
    BOSS_BOWSER_AND_KAMMY,
    BOSS_SHADOW_QUEEN_1,
    BOSS_SHADOW_QUEEN_2,
    BOSS_BONETAIL,
};

enum WARPS_INDEX_OPTIONS
{
    INDEX_SELECT_MAP = 1,
    INDEX_SELECT_ENTRANCE,
    INDEX_VIEW_CURRENT_MAP_ENTRANCES,
    INDEX_WARP_NOW,
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
    PAD_DPAD_LEFT  = 0x0001,
    PAD_DPAD_RIGHT = 0x0002,
    PAD_DPAD_DOWN  = 0x0004,
    PAD_DPAD_UP    = 0x0008,
    PAD_Z          = 0x0010,
    PAD_R          = 0x0020,
    PAD_L          = 0x0040,
    // unused      = 0x0080,
    PAD_A          = 0x0100,
    PAD_B          = 0x0200,
    PAD_X          = 0x0400,
    PAD_Y          = 0x0800,
    PAD_START      = 0x1000,
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

struct MenuVars
{
    bool HideMenu;
    bool MenuIsDisplayed;
    bool ChangingCheatButtonCombo;
    uint8_t CurrentMenu;
    uint8_t CurrentMenuOption;
    uint8_t MenuSelectedOption;
    uint8_t SecondaryMenuOption;
    uint8_t SelectedOption;
    uint8_t CurrentPage;
    uint8_t SecondaryPage;
    int8_t FunctionReturnCode;
    uint8_t FrameCounter;
    uint8_t MenuSelectionStates;
    bool DrawChangingMemoryWatchPosition;
    bool SystemLevelIsSet;
    bool ResetMarioProperties;
    bool MarioFreeze;
    uint16_t LagSpikeDuration; // Milliseconds
    int16_t ForcedNPCItemDrop;
    uint16_t JumpStorageSetCounter;
    uint32_t Timer;
    int32_t MenuSecondaryValue;
    uint32_t MenuSecondaryValueUnsigned;
    const uint32_t *CurrentMapInitScript;
    
    MenuVars()
    {
        ForcedNPCItemDrop = ttyd::item_data::Item::SleepySheep;
        LagSpikeDuration  = 468;
    }
};

struct Menus
{
    uint8_t TotalMenuOptions;
    uint8_t TotalMenuColumns;
    uint8_t ColumnSplitAmount;
    const char **Line;
    
    Menus()
    {
        TotalMenuColumns  = 1;
        ColumnSplitAmount = 1;
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

struct CheatsManageFlags
{
    uint16_t FlagToSet;
    int32_t ValueToSet;
};

struct MarioPartnerPositionsStruct
{
    float MarioPosition[7];
    float PartnerPosition[7];
};

struct SaveAnywhereStruct
{
    int32_t ThreadID;
    bool ScriptIsRunning;
};

struct SpeedUpMarioStruct
{
    float MarioVar[2];
    uint8_t ValuesChangedState;
};

struct ReloadRoomStruct
{
    bool ManuallyReloadingRoom;
    bool SystemLevelShouldBeLowered;
    char NewBero[32]; // 31 bytes for NextBero, 1 byte for NULL
    char NewMap[9]; // 8 bytes for NextMap, 1 byte for NULL
};

struct SpawnItems
{
    uint32_t SpawnItemCounter;
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

struct LockFlagsRegion
{
    uint8_t *MemoryRegion;
    uint16_t Size;
    uint16_t Offset;
    bool MemoryRegionLocked;
};

struct LockFlagsStruct
{
    LockFlagsRegion Region[6];
    char AreaLocked[2][4]; // 3 bytes for the area, 1 byte for NULL
    uint16_t SequencePosition;
    
    LockFlagsStruct()
    {
        Region[GSW].Size    = 0x800;
        Region[GSW].Offset  = 0x578;
        
        Region[GSWF].Size   = 0x400;
        Region[GSWF].Offset = 0x178;
        
        Region[GW].Size     = 0x80;
        
        Region[GF].Size     = 0xC;
        
        Region[LSW].Size    = 0x400;
        Region[LSW].Offset  = 0xDB8;
        
        Region[LSWF].Size   = 0x40;
        Region[LSWF].Offset = 0xD78;
    }
};

struct TrickDisplay
{
    uint32_t MainTimer;
    uint32_t ResetTimer;
    bool TimerPaused;
    bool TimerStopped;
    
    TrickDisplay()
    {
        TimerStopped = true;
    }
};

struct BridgeSkipStruct
{
    uint32_t MainTimer;
    uint32_t ResetTimer;
    bool PressedEarly;
    bool PressedEarlyShouldDisable;
    bool TimerPaused;
    bool TimerStopped;
    
    BridgeSkipStruct()
    {
        TimerStopped = true;
    }
};

struct BlimpTicketSkipStruct
{
    uint32_t UpRightTimer;
    uint32_t StraightUpTimer;
    uint32_t ResetTimer;
    bool TimersPaused;
    bool UpRightTimerStopped;
    bool StraightUpTimerStopped;
    
    BlimpTicketSkipStruct()
    {
        UpRightTimerStopped = true;
        StraightUpTimerStopped = true;
    }
};

struct OnScreenTimerDisplay
{
    int64_t MainTimer;
    int64_t PreviousFrameTime;
    uint16_t ButtonCombo[2];
    bool TimerPaused;
    
    OnScreenTimerDisplay()
    {
        TimerPaused = true;
    }
};

struct OnScreenTimerDisplayFrameCounter
{
    uint32_t MainTimer;
    uint16_t ButtonCombo[2];
    bool TimerPaused;
    
    OnScreenTimerDisplayFrameCounter()
    {
        TimerPaused = true;
    }
};

struct HeapInfoDisplay
{
    #define MEMORY_USAGE_LINE_BUFFER_SIZE 64
    int32_t ArrayCount;
    bool *DisplayHeapInfo;
    char *MemoryUsageBuffer;
    char HeapCorruptionBuffer[512];
    
    HeapInfoDisplay()
    {
        int32_t NumHeaps = gc::OSAlloc::NumHeaps + 1; // Add one for the smart heap
        ArrayCount = NumHeaps;
        DisplayHeapInfo = new bool[NumHeaps];
        
        // Subtract 1 for not displaying the free portion of the smart heap
        int32_t MemoryUsageArrays = (NumHeaps * 2) - 1;
        MemoryUsageBuffer = new char[MemoryUsageArrays * MEMORY_USAGE_LINE_BUFFER_SIZE];
    }
    
    ~HeapInfoDisplay()
    {
        if (DisplayHeapInfo)
        {
            delete[] (DisplayHeapInfo);
        }
        
        if (MemoryUsageBuffer)
        {
            delete[] (MemoryUsageBuffer);
        }
    }
};

struct DisplayActionCommandsTiming
{
    uint16_t DisplayTimer;
    uint8_t TypeToDraw;
    int8_t Last_A_Frame;
    int8_t Last_B_Frame;
};

struct MemoryCardStruct
{
    int32_t MemoryCardSlot;
    int32_t ReturnCode;
    const char *SettingsFileName;
    const char *SettingsDescription;
    const char *RelFileName;
    
    MemoryCardStruct()
    {
        SettingsFileName = "Practice_Codes_Settings";
        
#ifdef TTYD_US
        SettingsDescription = "Practice Codes Settings (US)";
#elif defined TTYD_JP
        SettingsDescription = "Practice Codes Settings (JP)";
#elif defined TTYD_EU
        SettingsDescription = "Practice Codes Settings (EU)";
#endif
        
        RelFileName = "rel";
    }
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
} __attribute__((__packed__));

struct MemoryEditorStruct
{
    #define EDITOR_BYTES_PER_ROW             13
    #define EDITOR_TOTAL_ROWS                13
    #define EDITOR_DIGITS_PER_ROW            (EDITOR_BYTES_PER_ROW * 2)
    #define EDITOR_MAX_NUM_BYTES_DISPLAYED   (EDITOR_BYTES_PER_ROW * EDITOR_TOTAL_ROWS)
    #define EDITOR_MAX_NUM_DIGITS_DISPLAYED  (EDITOR_DIGITS_PER_ROW * EDITOR_TOTAL_ROWS)
    
    #define EDITOR_HEADER_TOTAL_OPTIONS      3
    #define EDITOR_HEADER_CHANGE_ADDRESS     0
    #define EDITOR_HEADER_CHANGE_BYTE_SIZE   1
    #define EDITOR_HEADER_GOTO_EDITOR        2
    
    #define EDITOR_SETTINGS_CLEAR_CACHE      0
    #define EDITOR_SETTINGS_SYSTEM_LEVEL     1
    #define EDITOR_SETTINGS_PAUSE_MENU       2
    #define EDITOR_SETTINGS_VERTICAL_LINES   3
    #define EDITOR_SETTINGS_HORIZONTAL_LINES 4
    
    // CurrentSelectionStatus values
    #define EDITOR_SELECT_CHANGE_ADDRESS     0x00000001
    #define EDITOR_SELECT_CHANGE_BYTES       0x00000002
    #define EDITOR_SELECT_GOTO_EDITOR        0x00000004
    #define EDITOR_SELECTED_BYTES_TO_EDIT    0x00000008
    #define EDITOR_EDITING_BYTES             0x00000010
    #define EDITOR_CLEAR_CACHE               0x00000020
    #define EDITOR_SET_SYSTEM_LEVEL          0x00000040
    #define EDITOR_DISABLE_PAUSE_MENU        0x00000080
    #define EDITOR_VERTICAL_LINES            0x00000100
    #define EDITOR_HORIZONTAL_LINES          0x00000200
    #define EDITOR_OPEN_SETTINGS             0x00000400
    #define EDITOR_ENABLED                   0x00000800
    
    #define MEMORY_EDITOR_MENU               0x3100
    #define MEMORY_EDITOR_ADJUSTABLE_VALUE   0x3200
    
    uint8_t *CurrentAddress;
    uint8_t *BytesBeingEdited;
    uint32_t CurrentSelectionStatus; // Bitfield
    uint16_t ButtonCombo;
    uint16_t CurrentEditorSelectedMenuOption;
    AutoIncrement AdjustEachFrame;
    uint8_t CurrentEditorMenuOption;
    uint8_t CurrentSettingsMenuOption;
    uint8_t NumBytesBeingEdited;
    bool EditorCurrentlyDisplayed;
    
    MemoryEditorStruct()
    {
        CurrentAddress = reinterpret_cast<uint8_t *>(0x80000000);
        CurrentSelectionStatus = (EDITOR_SET_SYSTEM_LEVEL | EDITOR_DISABLE_PAUSE_MENU);
        NumBytesBeingEdited = 1;
    }
};

struct MemoryEditorSaveStruct
{
    uint8_t *CurrentAddress;
    uint32_t CurrentSelectionStatus; // Bitfield
    uint16_t ButtonCombo;
    uint8_t NumBytesBeingEdited;
    bool EditorCurrentlyDisplayed;
} __attribute__((__packed__));

struct FrameAdvanceButtonCombosStruct
{
    uint16_t AdvanceFrameButtonCombo;
    uint16_t PauseButtonCombo;
} __attribute__((__packed__));

struct FrameAdvanceStruct
{
    FrameAdvanceButtonCombosStruct FrameAdvanceButtonCombos;
    bool AdvanceFrame;
    bool GameIsPaused;
    bool JustResumedGameplay;
};

struct SettingsStruct
{
    bool CheatsActive[100];
    uint16_t CheatButtonCombos[100];
    bool DisplaysActive[100];
    uint16_t DisplaysButtonCombos[100];
    MemoryWatchStruct MemoryWatchSettings[60];
    MemoryEditorSaveStruct MemoryEditorSave;
    FrameAdvanceButtonCombosStruct FrameAdvanceButtonCombos;
} __attribute__((__packed__));

struct SaveFileDecriptionInfo
{
    char Description1[32];
    char Description2[32];
    uint32_t FileSize;
} __attribute__((__packed__));

struct WarpByEventStruct
{
    int32_t CurrentIndex;
    bool ShouldKeepInventory;
    bool ShouldSetFlags;
    bool ShouldInit;
    
    WarpByEventStruct()
    {
        ShouldKeepInventory = true;
        ShouldSetFlags = true;
    }
};

struct WarpByEventInventoryStruct
{
    int16_t StandardItems[20];
    int16_t Badges[200];
};

struct WarpByEventDetailsStruct
{
#ifdef TTYD_JP
    char Stage[8];
#else
    const char *Stage;
#endif
    
    const char *Event;
    const char *Partner;
    const char *Follower;
    const char *Map;
    const char *Bero;
    uint16_t SequencePosition;
};

struct WarpByIndexStruct
{
    bool RunIndexWarpCode;
    uint16_t MapId;
    uint8_t EntranceId;
    const char *EntranceList[32];
    
    WarpByIndexStruct()
    {
        EntranceId = 1;
    }
};

struct MenuPrevMenuAndOption
{
    uint8_t CurrentIndex;
    uint8_t PreviousPage[20];
    uint8_t PreviousMenu[20];
    uint8_t PreviousMenuOption[20];
};

struct ErrorHandlerStrings
{
    #define MAX_LEVELS 32
    const char *OSError;
    char ContextAddress[32];
    char GeneralRegisterValues[32][11];
    char FloatRegisterValues[32][19];
    char PairedSinglesValues[32][19];
    char AdditionalRegisterValues[6][11];
    char AddressList[MAX_LEVELS * 3][11];
    char InstructionAndAddress[128];
};

struct NpcNameToPtrErrorStruct
{
    uint32_t Counter;
    uint16_t Timer;
};

struct EnemyEncounterNotifierStruct
{
    uint32_t Counter;
    uint16_t Timer;
};

struct UnusedMapStruct
{
    ttyd::mapdata::MapData UnusedMapData;
    char UnusedMapName[9]; // 8 bytes for UnusedMapName, 1 byte for NULL
    
    UnusedMapStruct()
    {
        UnusedMapData.mapName = UnusedMapName;
    }
};

extern MenuVars MenuVar;
extern Menus Menu[36];
extern Cheats Cheat[27];
extern bool Displays[18];
extern char DisplayBuffer[256];
extern MemoryWatchStruct MemoryWatch[60];
extern MemoryEditorStruct MemoryEditor;

extern AutoIncrement AdjustableValueMenu;
extern AutoIncrement MemoryWatchAdjustableValueMenu;
extern AutoIncrementCoordinates MemoryWatchPosition;
extern CheatsHandleDisplayButtons CheatsDisplayButtons;
extern CheatsManageFlags ManageFlags;
extern MarioPartnerPositionsStruct MarioPartnerPositions;
extern SaveAnywhereStruct SaveAnywhere;
extern SpeedUpMarioStruct SpeedUpMario;
extern ReloadRoomStruct ReloadRoom;
extern SpawnItems SpawnItem;
extern ClearAreaFlagsStruct ClearAreaFlags;
extern ClearCacheForBattlesStruct ClearCacheForBattles;
extern LockFlagsStruct LockFlags;
extern TrickDisplay YoshiSkip;
extern TrickDisplay PalaceSkip;
extern BridgeSkipStruct BridgeSkip;
extern BlimpTicketSkipStruct BlimpTicketSkip;
extern OnScreenTimerDisplay OnScreenTimer;
extern OnScreenTimerDisplayFrameCounter FrameCounter;
extern HeapInfoDisplay HeapInfo;
extern DisplayActionCommandsTiming DisplayActionCommands;
extern MemoryCardStruct MenuSettings;
extern WarpByEventStruct WarpByEvent;
extern WarpByIndexStruct WarpByIndex;
extern MenuPrevMenuAndOption PrevMenuAndOption;
extern NpcNameToPtrErrorStruct NpcNameToPtrError;
extern EnemyEncounterNotifierStruct EnemyEncounterNotifier;
extern FrameAdvanceStruct FrameAdvance;
extern UnusedMapStruct UnusedMap;

extern uint8_t CheatsOrder[];
extern uint16_t StatsMarioIcons[];
extern uint16_t BattlesStatusesIcons[];
extern uint8_t DisplaysOrder[];

extern const char *VersionNumber;
extern const char *CheatsLines[];
extern const char *CheatsClearAreaFlagsAreas[];
extern const char *CheatsClearAreaFlagsAreasFullNames[];
extern const char *CheatsManageGlobalWordsOptions[];
extern const char *CheatsManageGlobalFlagsOptions[];
extern const char *StatsMarioOptionsLines[];
extern const char *MarioStatsSpecialMovesOptions[];
extern const char *StatsPartnerOptionsLines[];
extern const char *StatsYoshiColorOptionsLines[];
extern const char *StatsFollowerOptionsLines[];
extern const char *MemoryWatchModifyLines[];
extern const char *MemoryTypeLines[];
extern const char *MemoryEditorSettingsLines[];
extern const char *BattlesActorsLines[];
extern const char *BattlesCurrentActorStats[];
extern const char *BattlesStatusesLines[];
extern const char *DisplaysLines[];
extern const char *OnScreenTimerOptions[];
extern const char *WarpDestinations[];
extern const char *WarpDescriptions[];
extern const char *WarpBossLines[];
extern const char *UnusedMapsArray[];

extern uint8_t CheatsManageGlobalWordsOptionsSize;
extern uint8_t CheatsManageGlobalFlagsOptionsSize;
extern uint8_t CheatsClearAreaFlagsAreasSize;
extern uint8_t StatsMarioOptionsLinesSize;
extern uint8_t MarioStatsSpecialMovesOptionsSize;
extern uint8_t StatsPartnerOptionsLinesSize;
extern uint8_t StatsYoshiColorOptionsLinesSize;
extern uint8_t StatsFollowerOptionsLinesSize;
extern uint8_t MemoryWatchModifyLinesSize;
extern uint8_t MemoryTypeLinesSize;
extern uint8_t MemoryEditorSettingsLinesSize;
extern uint8_t BattlesCurrentActorStatsSize;
extern uint8_t BattlesStatusesLinesSize;
extern uint8_t OnScreenTimerOptionsSize;
extern uint8_t WarpDestinationsSize;
extern uint8_t UnusedMapsArraySize;

#ifdef TTYD_JP
extern const char *ButtonInputDisplay[];
extern const char *CheatsEventNames[];
extern const char *WarpsEventNames[];
extern const char *PointerText;
extern uint16_t WarpsEventNamesSize;
#endif

#ifndef TTYD_JP
extern const char ButtonInputDisplay[];
#endif

void initMenuVars();
void setInitialSettings();
void initAddressOverwrites();

}