#ifndef CHEATS_H
#define CHEATS_H

#include "mod.h"
#include "cxx.h"
#include "classes/valueEditor.h"
#include "gc/pad.h"
#include "gc/DEMOPad.h"
#include "misc/utils.h"
#include "ttyd/item_data.h"

#include <cstdint>

class Cheats;
typedef void (*CheatsArrayFunc)(Cheats *cheatsPtr, Mod *modPtr);

// Enabled flags are mainly used for whether a cheat is enabled or not, but can also be for misc flags that are saved to the
// settings file
enum CheatsEnabledFlag
{
    CHEATS_ENABLED_FLAG_WALK_THROUGH_WALLS = 0,
    CHEATS_ENABLED_FLAG_FALL_THROUGH_FLOORS,
    CHEATS_ENABLED_FLAG_SAVE_COORDINATES,
    CHEATS_ENABLED_FLAG_LOAD_COORDINATES,
    CHEATS_ENABLED_FLAG_SPAWN_ITEM,
    CHEATS_ENABLED_FLAG_SAVE_ANYWHERE,
    CHEATS_ENABLED_FLAG_TEXT_STORAGE,
    CHEATS_ENABLED_FLAG_TIME_STOP_TEXT_STORAGE,
    CHEATS_ENABLED_FLAG_SPEED_UP_MARIO,
    CHEATS_ENABLED_FLAG_DISABLE_BATTLES,
    CHEATS_ENABLED_FLAG_AUTO_ACTION_COMMANDS,
    CHEATS_ENABLED_FLAG_INFINITE_ITEM_USAGE,
    CHEATS_ENABLED_FLAG_RELOAD_ROOM,
    CHEATS_ENABLED_FLAG_LEVITATE,
    CHEATS_ENABLED_FLAG_AUTO_MASH_TEXT,
    CHEATS_ENABLED_FLAG_FRAME_ADVANCE,
    CHEATS_ENABLED_FLAG_GENERATE_LAG_SPIKE,
    CHEATS_ENABLED_FLAG_LOCK_MARIO_HP_TO_MAX,
    CHEATS_ENABLED_FLAG_LOCK_PARTNER_HP_TO_MAX,
    CHEATS_ENABLED_FLAG_RUN_FROM_BATTLES,
    CHEATS_ENABLED_FLAG_BOBBERY_EARLY,
    CHEATS_ENABLED_FLAG_FORCE_NPC_ITEM_DROP,

    CHEATS_ENABLED_FLAG_MARIO_COORDINATES_MODIFY_AS_HEX, // Modify Mario's Coordinates

    // All of the flags for the Disable Certain Sounds cheat need to be next to each other in order to work correctly
    // The code assumes that PAUSE_MENU_Z_MENU is first
    CHEATS_ENABLED_FLAG_DISABLE_SOUNDS_PAUSE_MENU_Z_MENU,  // Disable Certain Sounds
    CHEATS_ENABLED_FLAG_DISABLE_SOUNDS_BACKGROUND_MUSIC,   // Disable Certain Sounds
    CHEATS_ENABLED_FLAG_DISABLE_SOUNDS_ENVIRONMENT_SOUNDS, // Disable Certain Sounds

#ifdef TTYD_JP
    CHEATS_ENABLED_FLAG_SIMULATE_MARIO_ZERO_HITBOX_GLITCH,
#endif

    CHEATS_ENABLED_FLAG_MAX_VALUE, // Don't use this directly other than for defines
};

#define TOTAL_CHEATS_ENABLED_FLAGS CheatsEnabledFlag::CHEATS_ENABLED_FLAG_MAX_VALUE
#define CHEATS_ENABLED_FLAGS_ARRAY_SIZE intCeil(TOTAL_CHEATS_ENABLED_FLAGS, sizeof(uint32_t) * 8)

// Misc flags are mainly for things that will not be saved to the settings file
enum CheatsMiscFlag
{
    CHEATS_MISC_FLAG_RESET_MARIO_PROPERTIES = 0, // Walk Through Most Objects

    CHEATS_MISC_FLAG_SAVE_SCRIPT_IS_RUNNING, // Save Anywhere

    CHEATS_MISC_FLAG_MANUALLY_RELOADING_ROOM,        // Reload Room
    CHEATS_MISC_FLAG_SYSTEM_LEVEL_SHOULD_BE_LOWERED, // Reload Room

    CHEATS_MISC_FLAG_FRAME_ADVANCE_ADVANCE_FRAME,  // Frame Advance
    CHEATS_MISC_FLAG_FRAME_ADVANCE_GAME_IS_PAUSED, // Frame Advance

    CHEATS_MISC_FLAG_AREA_FLAGS_SHOULD_BE_CLEARED,    // Clear Area Flags
    CHEATS_MISC_FLAG_AREA_FLAGS_START_CLEARING_FLAGS, // Clear Area Flags

    // All of the flags for the Lock Flags cheat need to be next to each other in order to work correctly
    // The code assumes that GSW is first and LSWF is last
    CHEATS_MISC_FLAG_LOCK_GSW,  // Lock Flags
    CHEATS_MISC_FLAG_LOCK_GSWF, // Lock Flags
    CHEATS_MISC_FLAG_LOCK_GW,   // Lock Flags
    CHEATS_MISC_FLAG_LOCK_GF,   // Lock Flags
    CHEATS_MISC_FLAG_LOCK_LSW,  // Lock Flags
    CHEATS_MISC_FLAG_LOCK_LSWF, // Lock Flags

    CHEATS_MISC_FLAG_MARIO_FREEZE,

    CHEATS_MISC_FLAG_MAX_VALUE, // Don't use this directly other than for defines
};

#define TOTAL_CHEATS_MISC_FLAGS CheatsMiscFlag::CHEATS_MISC_FLAG_MAX_VALUE
#define CHEATS_MISC_FLAGS_ARRAY_SIZE intCeil(TOTAL_CHEATS_MISC_FLAGS, sizeof(uint32_t) * 8)

enum CheatsWithButtonCombo
{
    CHEATS_BUTTON_COMBO_WALK_THROUGH_WALLS = 0,
    CHEATS_BUTTON_COMBO_FALL_THROUGH_FLOORS,
    CHEATS_BUTTON_COMBO_SAVE_COORDINATES,
    CHEATS_BUTTON_COMBO_LOAD_COORDINATES,
    CHEATS_BUTTON_COMBO_SPAWN_ITEM,
    CHEATS_BUTTON_COMBO_SAVE_ANYWHERE,
    CHEATS_BUTTON_COMBO_TEXT_STORAGE,
    CHEATS_BUTTON_COMBO_TIME_STOP_TEXT_STORAGE,
    CHEATS_BUTTON_COMBO_SPEED_UP_MARIO,
    CHEATS_BUTTON_COMBO_DISABLE_BATTLES,
    CHEATS_BUTTON_COMBO_AUTO_ACTION_COMMANDS,
    CHEATS_BUTTON_COMBO_INFINITE_ITEM_USAGE,
    CHEATS_BUTTON_COMBO_RELOAD_ROOM,
    CHEATS_BUTTON_COMBO_LEVITATE,
    CHEATS_BUTTON_COMBO_AUTO_MASH_TEXT,
    CHEATS_BUTTON_COMBO_FRAME_ADVANCE_ADVANCE_FRAME,
    CHEATS_BUTTON_COMBO_FRAME_ADVANCE_PAUSE_RESUME,
    CHEATS_BUTTON_COMBO_GENERATE_LAG_SPIKE,
    CHEATS_BUTTON_COMBO_MAX_VALUE, // Don't use this directly other than for defines
};

#define TOTAL_CHEATS_BUTTON_COMBOS CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_MAX_VALUE

enum class SpeedUpMarioChangedState : uint8_t
{
    SPEED_UP_MARIO_CHANGED_STATE_NO_CHANGES = 0,
    SPEED_UP_MARIO_CHANGED_STATE_VALUES_MODIFIED,
    SPEED_UP_MARIO_CHANGED_STATE_VALUES_SHOULD_BE_RESTORED,
};

#define TOTAL_SAVED_POS_VARIABLES 7
class SaveLoadCoordinatesCheats
{
   public:
    SaveLoadCoordinatesCheats() {}
    ~SaveLoadCoordinatesCheats() {}

    float *getMarioSavedPosPtr() { return &this->marioSavedPos[0]; }
    float *getPartnerSavedPosPtr() { return &this->partnerSavedPos[0]; }

   private:
    float marioSavedPos[TOTAL_SAVED_POS_VARIABLES];
    float partnerSavedPos[TOTAL_SAVED_POS_VARIABLES];
};

class SaveAnywhereCheat
{
   public:
    SaveAnywhereCheat() {}
    ~SaveAnywhereCheat() {}

    int32_t getThreadId() const { return this->threadId; }
    void setThreadId(int32_t threadId) { this->threadId = threadId; }

   private:
    int32_t threadId;
};

class ForceNpcItemDropCheat
{
   public:
    ForceNpcItemDropCheat() { this->item = ItemId::ITEM_SLEEPY_SHEEP; }
    ~ForceNpcItemDropCheat() {}

    ItemId getItemDrop() const { return this->item; }
    void setItemDrop(ItemId item) { this->item = item; }

   private:
    ItemId item;
};

#define LAG_SPIKE_DEFAULT_DURATION 468
#define LAG_SPIKE_MAX_DURATION 10000
class GenerateLagSpikeCheat
{
   public:
    GenerateLagSpikeCheat() { this->duration = LAG_SPIKE_DEFAULT_DURATION; }
    ~GenerateLagSpikeCheat() {}

    uint32_t getDuration() const { return this->duration; }
    void setDuration(uint32_t duration) { this->duration = static_cast<uint16_t>(duration); }

   private:
    uint16_t duration;
};

class SpeedUpMarioCheat
{
   public:
    SpeedUpMarioCheat() {}
    ~SpeedUpMarioCheat() {}

    float *getMarioVarsPtr() { return &this->marioVar[0]; }
    SpeedUpMarioChangedState getChangedState() const { return this->changedState; }
    void setChangedState(SpeedUpMarioChangedState state) { this->changedState = state; }

   private:
    float marioVar[2];
    SpeedUpMarioChangedState changedState;
};

class ReloadRoomCheat
{
   public:
    ReloadRoomCheat() {}
    ~ReloadRoomCheat() {}

    uint32_t getNewBeroSize() const { return sizeof(this->newBero); }
    uint32_t getNewMapSize() const { return sizeof(this->newMap); }

    char *getNewBeroPtr() { return &this->newBero[0]; }
    char *getNewMapPtr() { return &this->newMap[0]; }

   private:
    char newBero[17]; // 16 bytes for NewBero, one byte for null
    char newMap[9];   // 8 bytes for NewMap, one byte for null
};

class SpawnItemCheat
{
   public:
    SpawnItemCheat() { prevItem = static_cast<int32_t>(ItemId::ITEM_STRANGE_SACK); }
    ~SpawnItemCheat() {}

    uint32_t getItemCounter() const { return this->itemCounter; }
    void setItemCounter(uint32_t value) { this->itemCounter = value; }

    int32_t *getPrevItemPtr() { return &this->prevItem; }
    void setPrevItem(int32_t item) { this->prevItem = item; }

    ValueEditor *getValueEditorPtr() const { return this->valueEditor; }

    ValueEditor *allocMemoryForValueEditor()
    {
        // Failsafe: Make sure memory is not already allocated for it
        ValueEditor *valueEditorPtr = this->valueEditor;

        if (!valueEditorPtr)
        {
            valueEditorPtr = new ValueEditor;
            this->valueEditor = valueEditorPtr;
        }

        return valueEditorPtr;
    }

    ValueEditor *freeValueEditor()
    {
        ValueEditor *valueEditorPtr = this->valueEditor;
        if (valueEditorPtr)
        {
            delete valueEditorPtr;
            valueEditorPtr = nullptr;
            this->valueEditor = valueEditorPtr;
        }

        return valueEditorPtr;
    }

   private:
    int32_t prevItem;
    uint32_t itemCounter;
    ValueEditor *valueEditor;
};

class FrameAdvancePrevInputs
{
   public:
    FrameAdvancePrevInputs() {}
    ~FrameAdvancePrevInputs() {}

    PadStatus *getPadPrevFramePtr() { return &this->padPrevFrame[0]; }
    PadStatus *getPadCurrentFramePtr() { return &this->padCurrentFrame[0]; }

    DEMOPadStatus *getDemoPadPrevFramePtr() { return &this->demoPadPrevFrame[0]; }
    DEMOPadStatus *getDemoPadCurrentFramePtr() { return &this->demoPadCurrentFrame[0]; }

   private:
    // These are ordered this way to allow memcpying from both current pads to both previous pads at once
    // The code assumes that padPrevFrame is first
    PadStatus padPrevFrame[4];            // One for each controller port
    DEMOPadStatus demoPadPrevFrame[4];    // One for each controller port
    PadStatus padCurrentFrame[4];         // One for each controller port
    DEMOPadStatus demoPadCurrentFrame[4]; // One for each controller port
};

class FrameAdvanceCheat
{
   public:
    FrameAdvanceCheat() {}
    ~FrameAdvanceCheat() {}

    FrameAdvancePrevInputs *allocMemoryForPrevInputs()
    {
        // Allocate memory for prevInputs if not already allocated
        FrameAdvancePrevInputs *prevInputs = this->prevInputs;

        if (!prevInputs)
        {
            prevInputs = new FrameAdvancePrevInputs;
            this->prevInputs = prevInputs;
        }

        return prevInputs;
    }

    FrameAdvancePrevInputs *freePrevInputs()
    {
        FrameAdvancePrevInputs *prevInputsPtr = this->prevInputs;
        if (prevInputsPtr)
        {
            delete prevInputsPtr;
            prevInputsPtr = nullptr;
            this->prevInputs = prevInputsPtr;
        }

        return prevInputsPtr;
    }

    void clearAllInputs()
    {
        FrameAdvancePrevInputs *prevInputs = this->prevInputs;
        if (prevInputs)
        {
            clearMemory(prevInputs->getPadPrevFramePtr(), sizeof(FrameAdvancePrevInputs));
        }
    }

   private:
    FrameAdvancePrevInputs *prevInputs;
};

#define TOTAL_LOCK_FLAGS_OPTIONS (CheatsMiscFlag::CHEATS_MISC_FLAG_LOCK_LSWF - CheatsMiscFlag::CHEATS_MISC_FLAG_LOCK_GSW + 1)
class LockFlagsCheat
{
   public:
    LockFlagsCheat() {}
    ~LockFlagsCheat() {}

    uint32_t getLockFlagsSequencePosition() const { return this->sequencePosition; }
    void setLockFlagsSequencePosition(uint32_t sequence) { this->sequencePosition = static_cast<uint16_t>(sequence); }

    char *getLswAreaLocked() { return &this->lswAreaLocked[0]; }
    char *getLswfAreaLocked() { return &this->lswfAreaLocked[0]; }

    uint8_t *getPrevMemoryPtr(uint32_t region)
    {
        // Make sure the region is valid
        if (region >= TOTAL_LOCK_FLAGS_OPTIONS)
        {
            return nullptr;
        }

        return this->prevMemory[region];
    }

    uint8_t *allocMemoryForPrevMemory(uint32_t region, uint32_t size)
    {
        // Make sure the region is valid
        if (region >= TOTAL_LOCK_FLAGS_OPTIONS)
        {
            return nullptr;
        }

        uint8_t **prevMemory = &this->prevMemory[region];
        uint8_t *prevMemoryPtr = *prevMemory;

        if (!prevMemoryPtr)
        {
            prevMemoryPtr = new uint8_t[size];
            *prevMemory = prevMemoryPtr;
        }

        return prevMemoryPtr;
    }

    uint8_t *freePrevMemory(uint32_t region)
    {
        // Make sure the region is valid
        if (region >= TOTAL_LOCK_FLAGS_OPTIONS)
        {
            return nullptr;
        }

        uint8_t **prevMemory = &this->prevMemory[region];
        uint8_t *prevMemoryPtr = *prevMemory;

        if (prevMemoryPtr)
        {
            delete prevMemoryPtr;
            prevMemoryPtr = nullptr;
            *prevMemory = prevMemoryPtr;
        }

        return prevMemoryPtr;
    }

   private:
    uint16_t sequencePosition; // The sequence position is handled separately in the GSWs, so need to back it up separately
    char lswAreaLocked[4];     // 3 bytes for the area, 1 byte for null
    char lswfAreaLocked[4];    // 3 bytes for the area, 1 byte for null
    uint8_t *prevMemory[TOTAL_LOCK_FLAGS_OPTIONS];
};

class ClearAreaFlagsCheat
{
   public:
    ClearAreaFlagsCheat() {}
    ~ClearAreaFlagsCheat() {}

    uint32_t getSelectedArea() const { return this->selectedArea; }
    void setSelectedArea(uint32_t area) { this->selectedArea = static_cast<uint8_t>(area); }

   private:
    uint8_t selectedArea;
};

class Cheats
{
   public:
    Cheats();
    ~Cheats() {}

    bool checkCheatButtonCombo(uint32_t cheatWithCombo) const;
    bool checkCheatButtonComboEveryFrame(uint32_t cheatWithCombo) const;
    bool checkCheatButtonComboDemo(uint32_t cheatWithCombo) const;
    uint32_t getCheatButtonCombo(uint32_t cheatWithCombo);
    void setCheatButtonCombo(uint32_t cheatWithCombo, uint32_t buttonCombo);
    uint16_t *getButtonCombosPtr() { return this->buttonCombos; }

    bool enabledFlagIsSet(uint32_t flag) const;
    void setEnabledFlag(uint32_t flag);
    void clearEnabledFlag(uint32_t flag);
    bool toggleEnabledFlag(uint32_t flag);
    const uint32_t *getEnabledFlagsPtr() const { return this->enabledFlags; }

    bool miscFlagIsSet(uint32_t flag) const;
    void setMiscFlag(uint32_t flag);
    void clearMiscFlag(uint32_t flag);
    bool toggleMiscFlag(uint32_t flag);

    SaveLoadCoordinatesCheats *getSaveLoadCoordinatesCheatsPtr() { return &this->saveLoadCoordinates; }
    SaveAnywhereCheat *getSaveAnywhereCheatPtr() { return &this->saveAnywhere; }
    ForceNpcItemDropCheat *getForceNpcItemDropCheatPtr() { return &this->forceNpcItemDrop; }
    GenerateLagSpikeCheat *getGenerateLagSpikeCheatPtr() { return &this->generateLagSpike; }
    SpeedUpMarioCheat *getSpeedUpMarioCheatPtr() { return &this->speedUpMario; }
    ReloadRoomCheat *getReloadRoomCheatPtr() { return &this->reloadRoom; }
    SpawnItemCheat *getSpawnItemCheatPtr() { return &this->spawnItem; }
    FrameAdvanceCheat *getFrameAdvanceCheatPtr() { return &this->frameAdvance; }
    ClearAreaFlagsCheat *getClearAreaFlagsCheat() { return &this->clearAreaFlags; }
    LockFlagsCheat *getLockFlagsCheatPtr() { return &this->lockFlags; }

   private:
    uint32_t enabledFlags[CHEATS_ENABLED_FLAGS_ARRAY_SIZE];
    uint32_t miscFlags[CHEATS_MISC_FLAGS_ARRAY_SIZE];
    uint16_t buttonCombos[TOTAL_CHEATS_BUTTON_COMBOS];

    SaveLoadCoordinatesCheats saveLoadCoordinates;
    SaveAnywhereCheat saveAnywhere;
    FrameAdvanceCheat frameAdvance;
    SpawnItemCheat spawnItem;
    LockFlagsCheat lockFlags;
    ForceNpcItemDropCheat forceNpcItemDrop;
    GenerateLagSpikeCheat generateLagSpike;
    SpeedUpMarioCheat speedUpMario;
    ReloadRoomCheat reloadRoom;
    ClearAreaFlagsCheat clearAreaFlags;
};

extern Cheats *gCheats;

void handleAutoActionCommands();
bool disableBattles();
bool infiniteItemUsage(ItemId item, uint32_t index);
void reloadRoomMain();
void disableBattlesOnReload(SeqIndex seq, const char *map, const char *bero);
uint32_t autoMashText(PadId controllerId);
void frameAdvance();
void preventViPostCallBackOnFramePause(uint32_t retraceCount);
int32_t disableMenuSounds(int32_t soundId, uint8_t a2, uint8_t vol, uint8_t pan);
bool disableBgm(int32_t flags, const char *bgmName, uint16_t wFadeTime, uint16_t wFadeTime2, bool unused);
bool disableEnvSounds(int32_t flags, const char *envName, int32_t wFadeTime, bool unused);
void lockFlags(Cheats *cheatsPtr, Mod *modPtr);
void runCheatFuncsEveryFrame();

#endif
