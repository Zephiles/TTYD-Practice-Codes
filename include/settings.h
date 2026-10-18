#ifndef SETTINGS_H
#define SETTINGS_H

#include "mod.h"
#include "inline.h"
#include "cheats.h"
#include "displays.h"
#include "memoryWatch.h"
#include "memoryEditor.h"
#include "customState.h"
#include "gc/card.h"
#include "menus/rootMenu.h"
#include "misc/utils.h"
#include "ttyd/item_data.h"
#include "ttyd/mario_pouch.h"

#include <cstdint>
#include <cstring>

#define SETTINGS_VERSION 5

// Based on SETTINGS_VERSION
#define SETTINGS_ADDED_AMW_GLITCH_VALUES 4
#define SETTINGS_MOVED_MISC_SAVED_FLAGS 5

#define CARD_RESULT_CARD_IN_USE -200
#define CARD_RESULT_INVALID_SETTNGS_VERSION -201

#define PTR_CAST_U32(ptr) (reinterpret_cast<uint32_t>(ptr))
#define PTR_ADD_OFFSET(ptr, offset) (PTR_CAST_U32(ptr) + (offset))
#define PTR_CAST_TYPE_ADD_OFFSET(type, ptr, offset) (reinterpret_cast<type>(PTR_ADD_OFFSET(ptr, offset)))

// All of the headers should be aligned to 4 bytes to avoid misalignment
// Header in the Practice Codes gci for the file title and description
class PracticeCodesDataHeader
{
   public:
    PracticeCodesDataHeader() {}
    ~PracticeCodesDataHeader() {}

    INLINE_FUNC void setData(const char *title, const char *description)
    {
        strncpy(this->title, title, sizeof(PracticeCodesDataHeader::title));
        strncpy(this->description, description, sizeof(PracticeCodesDataHeader::description));
    }

   private:
    char title[32];       // Not null terminated
    char description[32]; // Not null terminated
} __attribute__((aligned(sizeof(uint32_t)), packed));

class SettingsHeader
{
   public:
    SettingsHeader() {}
    ~SettingsHeader() {}

    INLINE_FUNC uint32_t setData(uint32_t version, uint32_t dataSize)
    {
        this->version = version;
        this->dataSize = dataSize;

        return sizeof(SettingsHeader);
    }

    uint32_t getVersion() const { return this->version; }
    uint32_t getDataSize() const { return this->dataSize; }

    uint32_t getMiscHeaderOffset() const { return this->miscHeaderOffset; }
    void setMiscHeaderOffset(uint32_t offset) { this->miscHeaderOffset = offset; }

    uint32_t getCheatsHeaderOffset() const { return this->cheatsHeaderOffset; }
    void setCheatsHeaderOffset(uint32_t offset) { this->cheatsHeaderOffset = offset; }

    uint32_t getDisplaysHeaderOffset() const { return this->displaysHeaderOffset; }
    void setDisplaysHeaderOffset(uint32_t offset) { this->displaysHeaderOffset = offset; }

    uint32_t getMemoryWatchesHeaderOffset() const { return this->memoryWatchesHeaderOffset; }
    void setMemoryWatchesHeaderOffset(uint32_t offset) { this->memoryWatchesHeaderOffset = offset; }

    uint32_t getMemoryEditorHeaderOffset() const { return this->memoryEditorHeaderOffset; }
    void setMemoryEditorHeaderOffset(uint32_t offset) { this->memoryEditorHeaderOffset = offset; }

    uint32_t getCustomStatesHeaderOffset() const { return this->customStatesHeaderOffset; }
    void setCustomStatesHeaderOffset(uint32_t offset) { this->customStatesHeaderOffset = offset; }

   private:
    uint32_t version; // SETTINGS_VERSION

    uint32_t dataSize; // Total size of all data written to the settings file, including PracticeCodesDataHeader,
                       // SettingsHeader, and all other headers

    // All of the offsets start at the start of SettingsHeader
    // If the offset is 0, then the code will assume that the section is not included
    uint32_t miscHeaderOffset;
    uint32_t cheatsHeaderOffset;
    uint32_t displaysHeaderOffset;
    uint32_t memoryWatchesHeaderOffset;
    uint32_t memoryEditorHeaderOffset;
    uint32_t customStatesHeaderOffset;
} __attribute__((aligned(sizeof(uint32_t)), packed));

class MiscSettingsHeader
{
   public:
    MiscSettingsHeader() {}
    ~MiscSettingsHeader() {}

    uint32_t getDataOffset() const { return this->dataOffset; }
    uint32_t getTotalFlags() const { return this->totalFlags; }

    INLINE_FUNC uint32_t setData(SettingsHeader *settingsHeaderPtr, uint32_t offset)
    {
        // Write the offset to the misc data
        settingsHeaderPtr->setMiscHeaderOffset(offset);

        constexpr uint32_t headerSize = sizeof(MiscSettingsHeader);
        this->dataOffset = offset + headerSize;
        this->totalFlags = TOTAL_MOD_SAVE_FLAGS;

        return headerSize;
    }

   private:
    // Starts at the start of SettingsHeader. If the value is 0, then the code will assume that the section is not included.
    uint32_t dataOffset;
    uint16_t totalFlags; // TOTAL_MOD_SAVE_FLAGS
} __attribute__((aligned(sizeof(uint32_t)), packed));

class MiscSettingsData
{
   public:
    MiscSettingsData() {}
    ~MiscSettingsData() {}

    void getData(uint32_t version, uint32_t totalFlags) const;

    INLINE_FUNC uint32_t setData()
    {
        this->rootWindowColor = gRootWindow->getColor();
        memcpy(this->enabledFlags, gMod->getSaveFlagsPtr(), MOD_SAVE_FLAGS_ARRAY_SIZE * sizeof(uint32_t));

        return sizeof(MiscSettingsData);
    }

   private:
    uint32_t rootWindowColor;
    uint32_t enabledFlags[MOD_SAVE_FLAGS_ARRAY_SIZE];
} __attribute__((aligned(sizeof(uint32_t)), packed));

class CheatsSettingsHeader
{
   public:
    CheatsSettingsHeader() {}
    ~CheatsSettingsHeader() {}

    uint32_t getDataOffset() const { return this->dataOffset; }
    uint32_t getTotalCheats() const { return this->totalCheats; }
    uint32_t getTotalButtonCombos() const { return this->totalButtonCombos; }

    INLINE_FUNC uint32_t setData(SettingsHeader *settingsHeaderPtr, uint32_t offset)
    {
        // Write the offset to the cheats data
        settingsHeaderPtr->setCheatsHeaderOffset(offset);

        constexpr uint32_t headerSize = sizeof(CheatsSettingsHeader);
        this->dataOffset = offset + headerSize;

        this->totalCheats = TOTAL_CHEATS_ENABLED_FLAGS;
        this->totalButtonCombos = TOTAL_CHEATS_BUTTON_COMBOS;

        return headerSize;
    }

   private:
    // Starts at the start of SettingsHeader. If the value is 0, then the code will assume that the section is not included.
    uint32_t dataOffset;

    uint16_t totalCheats;       // TOTAL_CHEATS_ENABLED_FLAGS
    uint16_t totalButtonCombos; // TOTAL_CHEATS_BUTTON_COMBOS
} __attribute__((aligned(sizeof(uint32_t)), packed));

class CheatsSettingsData
{
   public:
    CheatsSettingsData() {}
    ~CheatsSettingsData() {}

    void getData(uint32_t totalCheats, uint32_t totalButtonCombos, uint32_t version) const;

    INLINE_FUNC uint32_t setData(Cheats *cheatsPtr)
    {
        memcpy(this->enabledFlags, cheatsPtr->getEnabledFlagsPtr(), CHEATS_ENABLED_FLAGS_ARRAY_SIZE * sizeof(uint32_t));
        memcpy(this->buttonCombos, cheatsPtr->getButtonCombosPtr(), TOTAL_CHEATS_BUTTON_COMBOS * sizeof(uint16_t));

        return sizeof(CheatsSettingsData);
    }

   private:
    uint32_t enabledFlags[CHEATS_ENABLED_FLAGS_ARRAY_SIZE];
    uint16_t buttonCombos[roundUp(TOTAL_CHEATS_BUTTON_COMBOS, sizeof(uint16_t))];
} __attribute__((aligned(sizeof(uint32_t)), packed));

class DisplaysSettingsHeader
{
   public:
    DisplaysSettingsHeader() {}
    ~DisplaysSettingsHeader() {}

    uint32_t getDataOffset() const { return this->dataOffset; }
    uint32_t getTotalDisplays() const { return this->totalDisplays; }
    uint32_t getTotalManuallyPositionEntries() const { return this->totalManuallyPositionEntries; }
    uint32_t getTotalButtonCombos() const { return this->totalButtonCombos; }

    INLINE_FUNC uint32_t setData(SettingsHeader *settingsHeaderPtr, uint32_t offset)
    {
        // Write the offset to the displays data
        settingsHeaderPtr->setDisplaysHeaderOffset(offset);

        constexpr uint32_t headerSize = sizeof(DisplaysSettingsHeader);
        this->dataOffset = offset + headerSize;

        this->totalDisplays = TOTAL_DISPLAYS_ENABLED_FLAGS;
        this->totalManuallyPositionEntries = TOTAL_DISPLAYS_MANUALLY_POSITION_FLAGS;
        this->totalButtonCombos = TOTAL_DISPLAYS_BUTTON_COMBOS;

        return headerSize;
    }

   private:
    // Starts at the start of SettingsHeader. If the value is 0, then the code will assume that the section is not included.
    uint32_t dataOffset;

    uint16_t totalDisplays;                // TOTAL_DISPLAYS_ENABLED_FLAGS
    uint16_t totalManuallyPositionEntries; // TOTAL_DISPLAYS_MANUALLY_POSITION_FLAGS
    uint16_t totalButtonCombos;            // TOTAL_DISPLAYS_BUTTON_COMBOS
    uint16_t padding;
} __attribute__((aligned(sizeof(uint32_t)), packed));

class DisplaysSettingsData
{
   public:
    DisplaysSettingsData() {}
    ~DisplaysSettingsData() {}

    void getData(uint32_t totalDisplays, uint32_t totalManuallyPositionEntries, uint32_t totalButtonCombos) const;

    INLINE_FUNC uint32_t setData(Displays *displaysPtr)
    {
        const HitCheckVisualizationDisplay *hitCheckVisualizationPtr = displaysPtr->getHitCheckVisualizationDisplayPtr();
        this->hitsColor = hitCheckVisualizationPtr->getHitsColor();
        this->missesColor = hitCheckVisualizationPtr->getMissesColor();

        memcpy(this->enabledFlags, displaysPtr->getEnabledFlagsPtr(), DISPLAYS_ENABLED_FLAGS_ARRAY_SIZE * sizeof(uint32_t));

        memcpy(this->manuallyPositionFlags,
               displaysPtr->getManuallyPositionFlagsPtr(),
               DISPLAYS_MANUALLY_POSITION_FLAGS_ARRAY_SIZE * sizeof(uint32_t));

        memcpy(this->manuallyPosition,
               displaysPtr->getManuallyPositionEntriesPtr(),
               TOTAL_DISPLAYS_MANUALLY_POSITION_FLAGS * sizeof(DisplayManuallyPosition));

        memcpy(this->buttonCombos, displaysPtr->getButtonCombosPtr(), TOTAL_DISPLAYS_BUTTON_COMBOS * sizeof(uint16_t));

        return sizeof(DisplaysSettingsData);
    }

   private:
    uint32_t enabledFlags[DISPLAYS_ENABLED_FLAGS_ARRAY_SIZE];
    uint32_t manuallyPositionFlags[DISPLAYS_MANUALLY_POSITION_FLAGS_ARRAY_SIZE];
    DisplayManuallyPosition manuallyPosition[TOTAL_DISPLAYS_MANUALLY_POSITION_FLAGS];
    uint16_t buttonCombos[roundUp(TOTAL_DISPLAYS_BUTTON_COMBOS, sizeof(uint16_t))];

    uint32_t hitsColor;   // Hit Check Visualization
    uint32_t missesColor; // Hit Check Visualization
} __attribute__((aligned(sizeof(uint32_t)), packed));

class MemoryWatchesSettingsHeader
{
   public:
    MemoryWatchesSettingsHeader() {}
    ~MemoryWatchesSettingsHeader() {}

    uint32_t getDataOffset() const { return this->dataOffset; }
    uint32_t getTotalWatches() const { return this->totalWatches; }
    uint32_t getWatchNameLength() const { return this->watchNameLength; }

    // This function returns 0 if there are no watches to write
    INLINE_FUNC uint32_t setData(SettingsHeader *settingsHeaderPtr, MemoryWatch *memoryWatchPtr, uint32_t offset)
    {
        // If there are currently no entries, then don't write anything
        const uint32_t totalEntries = memoryWatchPtr->getTotalEntries();
        const MemoryWatchEntry *entriesPtr = memoryWatchPtr->getEntriesPtr();
        if ((totalEntries == 0) || !entriesPtr)
        {
            return 0;
        }

        // Write the offset to the memory watches data
        settingsHeaderPtr->setMemoryWatchesHeaderOffset(offset);

        constexpr uint32_t headerSize = sizeof(MemoryWatchesSettingsHeader);
        this->dataOffset = offset + headerSize;

        this->totalWatches = static_cast<uint16_t>(totalEntries);
        this->watchNameLength = MEMORY_WATCH_NAME_SIZE;

        return headerSize;
    }

   private:
    // Starts at the start of SettingsHeader. If the value is 0, then the code will assume that the section is not included.
    uint32_t dataOffset;

    uint16_t totalWatches;   // MemoryWatch::totalEntries
    uint8_t watchNameLength; // MEMORY_WATCH_NAME_SIZE
    uint8_t padding;
} __attribute__((aligned(sizeof(uint32_t)), packed));

class MemoryWatchesSettingsData
{
   public:
    MemoryWatchesSettingsData() {}
    ~MemoryWatchesSettingsData() {}

    uint32_t getData(uint32_t totalWatches, uint32_t watchNameLength) const;
    void getAddressOffsetsData(uint32_t offset) const;

    INLINE_FUNC uint32_t setData(const MemoryWatchEntry *entryPtr)
    {
        this->address = entryPtr->getAddressPtr();
        this->posX = entryPtr->getPosX();
        this->posY = entryPtr->getPosY();
        this->scale = entryPtr->getScale();

        const uint32_t totalOffsets = entryPtr->getTotalAddressOffsets();
        if ((totalOffsets > 0) && entryPtr->getAddressOffsetsPtr())
        {
            this->totalAddressOffsets = static_cast<uint8_t>(totalOffsets);
        }

        this->type = entryPtr->getType();
        this->showAsHex = entryPtr->shouldShowAsHex();
        this->display = entryPtr->shouldDisplay();
        copyStringAndNullTerminate(this->name, MEMORY_WATCH_NAME_SIZE, entryPtr->getNamePtrConst());

        return sizeof(MemoryWatchesSettingsData);
    }

    // This function returns 0 if there are no offsets to write
    INLINE_FUNC uint32_t setAddressOffsetsData(const MemoryWatchEntry *entryPtr)
    {
        // Make sure there is at least one offset to write
        const uint32_t totalOffsets = entryPtr->getTotalAddressOffsets();
        const int32_t *sourceOffsetsPtr = entryPtr->getAddressOffsetsPtr();
        if ((totalOffsets == 0) || !sourceOffsetsPtr)
        {
            return 0;
        }

        const uint32_t offsetsSize = totalOffsets * sizeof(int32_t);
        int32_t *addressOffsetsPtr = PTR_CAST_TYPE_ADD_OFFSET(int32_t *, this, 0);
        memcpy(addressOffsetsPtr, sourceOffsetsPtr, offsetsSize);

        return offsetsSize;
    }

   private:
    uint8_t *address;
    float posX;
    float posY;
    float scale;

    uint8_t totalAddressOffsets;
    VariableType type;
    bool showAsHex;
    bool display;

    // The address offsets will be directly after the name, so make sure the name is aligned to 4 bytes
    char name[roundUp(MEMORY_WATCH_NAME_SIZE, sizeof(uint32_t))];

    // addressOffsets will be handled separately from the rest of the class since its size is dynamic
    // int32_t addressOffsets[]; // Amount is determined by totalAddressOffsets
} __attribute__((aligned(sizeof(uint32_t)), packed));

class MemoryEditorSettingsHeader
{
   public:
    MemoryEditorSettingsHeader() {}
    ~MemoryEditorSettingsHeader() {}

    uint32_t getDataOffset() const { return this->dataOffset; }
    uint32_t getTotalFlags() const { return this->totalFlags; }

    INLINE_FUNC uint32_t setData(SettingsHeader *settingsHeaderPtr, uint32_t offset)
    {
        // Write the offset to the memory editor data
        settingsHeaderPtr->setMemoryEditorHeaderOffset(offset);

        constexpr uint32_t headerSize = sizeof(MemoryEditorSettingsHeader);
        this->dataOffset = offset + headerSize;

        this->totalFlags = TOTAL_MEMORY_EDITOR_ENABLED_FLAGS;

        return headerSize;
    }

   private:
    // Starts at the start of SettingsHeader. If the value is 0, then the code will assume that the section is not included.
    uint32_t dataOffset;

    uint16_t totalFlags; // TOTAL_MEMORY_EDITOR_ENABLED_FLAGS
    uint16_t padding;
} __attribute__((aligned(sizeof(uint32_t)), packed));

class MemoryEditorSettingsData
{
   public:
    MemoryEditorSettingsData() {}
    ~MemoryEditorSettingsData() {}

    void getData(uint32_t totalFlags) const;

    INLINE_FUNC uint32_t setData(const MemoryEditor *memoryEditorPtr)
    {
        this->currentAddress = memoryEditorPtr->getCurrentAddressPtr();
        this->buttonCombo = static_cast<uint16_t>(memoryEditorPtr->getButtonCombo());
        this->numBytesBeingEdited = static_cast<uint8_t>(memoryEditorPtr->getNumBytesBeingEdited());

        memcpy(this->enabledFlags,
               memoryEditorPtr->getEnabledFlagsPtr(),
               MEMORY_EDITOR_ENABLED_FLAGS_ARRAY_SIZE * sizeof(uint32_t));

        return sizeof(MemoryEditorSettingsData);
    }

   private:
    uint32_t enabledFlags[MEMORY_EDITOR_ENABLED_FLAGS_ARRAY_SIZE];
    uint8_t *currentAddress;
    uint16_t buttonCombo;
    uint8_t numBytesBeingEdited;
} __attribute__((aligned(sizeof(uint32_t)), packed));

class CustomStatesSettingsHeader
{
   public:
    CustomStatesSettingsHeader() {}
    ~CustomStatesSettingsHeader() {}

    uint32_t getDataOffset() const { return this->dataOffset; }
    uint32_t getTotalStates() const { return this->totalStates; }
    uint32_t getStateNameLength() const { return this->stateNameLength; }

    // This function returns 0 if there are no states to write
    INLINE_FUNC uint32_t setData(SettingsHeader *settingsHeaderPtr, CustomState *customStatePtr, uint32_t offset)
    {
        // If there are currently no states, then don't write anything
        const uint32_t totalEntries = customStatePtr->getTotalEntries();
        const CustomStateEntry *entriesPtr = customStatePtr->getEntriesPtr();
        if ((totalEntries == 0) || !entriesPtr)
        {
            return 0;
        }

        // Write the offset to the memory editor data
        settingsHeaderPtr->setCustomStatesHeaderOffset(offset);

        constexpr uint32_t headerSize = sizeof(CustomStatesSettingsHeader);
        this->dataOffset = offset + headerSize;

        this->totalStates = static_cast<uint16_t>(totalEntries);
        this->stateNameLength = CUSTOM_STATE_NAME_SIZE;

        return headerSize;
    }

   private:
    // Starts at the start of SettingsHeader. If the value is 0, then the code will assume that the section is not included.
    uint32_t dataOffset;

    uint16_t totalStates;    // CustomState::totalEntries
    uint8_t stateNameLength; // CUSTOM_STATE_NAME_SIZE
    uint8_t padding;
} __attribute__((aligned(sizeof(uint32_t)), packed));

class CustomStateSettingsMarioData
{
   public:
    CustomStateSettingsMarioData() {}
    ~CustomStateSettingsMarioData() {}

    uint32_t getData(CustomStateMarioData *dataPtr, uint32_t totalStates) const;

   private:
    int16_t currentHP;
    int16_t maxHP;
    int16_t currentFP;
    int16_t maxFP;
    int16_t coins;
    int16_t currentSP;
    int16_t maxSP;
    int16_t rank;
    int16_t level;
    uint16_t starPowersObtained; // Bitfield
    int16_t baseMaxHp;
    int16_t baseMaxFp;
    int16_t unallocatedBp;
    int16_t totalBp;
    int16_t starPoints;
} __attribute__((__packed__));

class CustomStatesSettingsData
{
   public:
    CustomStatesSettingsData() {}
    ~CustomStatesSettingsData() {}

    uint32_t getData(uint32_t totalStates, uint32_t stateNameLength) const;
    void getItemsData(uint32_t offset) const;

    uint32_t setData(CustomStateEntry *entryPtr);

    // This function returns 0 if there are no items to write
    INLINE_FUNC uint32_t setItemsData(CustomStateEntry *entryPtr)
    {
        // Make sure there is at least one item to write
        const uint32_t totalItems = entryPtr->getTotalInventoryItems();
        const ItemId *sourceItemsPtr = entryPtr->getItemsPtr();
        if ((totalItems == 0) || !sourceItemsPtr)
        {
            return 0;
        }

        const uint32_t itemsSize = totalItems * sizeof(ItemId);
        ItemId *itemsPtr = PTR_CAST_TYPE_ADD_OFFSET(ItemId *, this, 0);
        memcpy(itemsPtr, sourceItemsPtr, itemsSize);

        return itemsSize;
    }

   private:
    CustomStateSettingsMarioData marioData;
    PouchPartyData partyData[7];
    uint16_t totalInventoryItems;
    uint16_t sequencePosition;
    PartyMembers partnerOut;
    PartyMembers followerOut;
    bool inBoatMode;                        // Mario should be in boat mode upon entering the room
    char currentMap[8];                     // Does not include a null terminator
    char currentBero[16];                   // Does not include a null terminator
    char stateName[CUSTOM_STATE_NAME_SIZE]; // Does not include a null terminator
} __attribute__((aligned(sizeof(uint32_t)), packed));

static_assert(sizeof(PracticeCodesDataHeader) == 0x40);
static_assert(sizeof(SettingsHeader) == 0x20);
static_assert(sizeof(MiscSettingsHeader) == 0x8);
static_assert(sizeof(MiscSettingsData) == 0x8);
static_assert(sizeof(CheatsSettingsHeader) == 0x8);
static_assert(sizeof(CheatsSettingsData) == 0x28);
static_assert(sizeof(DisplaysSettingsHeader) == 0xC);
static_assert(sizeof(DisplaysSettingsData) == 0x114);
static_assert(sizeof(MemoryWatchesSettingsHeader) == 0x8);
static_assert(sizeof(MemoryWatchesSettingsData) == 0x24); // Excludes addressOffsets
static_assert(sizeof(MemoryEditorSettingsHeader) == 0x8);
static_assert(sizeof(MemoryEditorSettingsData) == 0xC);
static_assert(sizeof(CustomStatesSettingsHeader) == 0x8);
static_assert(sizeof(CustomStateSettingsMarioData) == 0x1E);
static_assert(sizeof(CustomStatesSettingsData) == 0xB0);

// Helper functions for getting the pointers to the headers
INLINE_FUNC PracticeCodesDataHeader *getPracticeCodesDataHeaderPtr(void *dataPtr, uint32_t dataOffset)
{
    (void)dataOffset;

    // PracticeCodesDataHeader should always be at the start of the data
    return PTR_CAST_TYPE_ADD_OFFSET(PracticeCodesDataHeader *, dataPtr, 0);
}

INLINE_FUNC SettingsHeader *getSettingsHeaderPtr(void *dataPtr, uint32_t dataOffset)
{
    (void)dataOffset;

    // SettingsHeader should always be directly after PracticeCodesDataHeader
    return PTR_CAST_TYPE_ADD_OFFSET(SettingsHeader *, dataPtr, sizeof(PracticeCodesDataHeader));
}

INLINE_FUNC MiscSettingsHeader *getMiscSettingsHeaderPtr(void *dataPtr, uint32_t dataOffset)
{
    // Add PracticeCodesDataHeader to start at the settings header
    return PTR_CAST_TYPE_ADD_OFFSET(MiscSettingsHeader *, dataPtr, sizeof(PracticeCodesDataHeader) + dataOffset);
}

INLINE_FUNC MiscSettingsData *getMiscSettingsDataPtr(void *dataPtr, uint32_t dataOffset)
{
    // Add PracticeCodesDataHeader to start at the settings header
    return PTR_CAST_TYPE_ADD_OFFSET(MiscSettingsData *, dataPtr, sizeof(PracticeCodesDataHeader) + dataOffset);
}

INLINE_FUNC CheatsSettingsHeader *getCheatsSettingsHeaderPtr(void *dataPtr, uint32_t dataOffset)
{
    // Add PracticeCodesDataHeader to start at the settings header
    return PTR_CAST_TYPE_ADD_OFFSET(CheatsSettingsHeader *, dataPtr, sizeof(PracticeCodesDataHeader) + dataOffset);
}

INLINE_FUNC CheatsSettingsData *getCheatsSettingsDataPtr(void *dataPtr, uint32_t dataOffset)
{
    // Add PracticeCodesDataHeader to start at the settings header
    return PTR_CAST_TYPE_ADD_OFFSET(CheatsSettingsData *, dataPtr, sizeof(PracticeCodesDataHeader) + dataOffset);
}

INLINE_FUNC DisplaysSettingsHeader *getDisplaysSettingsHeaderPtr(void *dataPtr, uint32_t dataOffset)
{
    // Add PracticeCodesDataHeader to start at the settings header
    return PTR_CAST_TYPE_ADD_OFFSET(DisplaysSettingsHeader *, dataPtr, sizeof(PracticeCodesDataHeader) + dataOffset);
}

INLINE_FUNC DisplaysSettingsData *getDisplaysSettingsDataPtr(void *dataPtr, uint32_t dataOffset)
{
    // Add PracticeCodesDataHeader to start at the settings header
    return PTR_CAST_TYPE_ADD_OFFSET(DisplaysSettingsData *, dataPtr, sizeof(PracticeCodesDataHeader) + dataOffset);
}

INLINE_FUNC MemoryWatchesSettingsHeader *getMemoryWatchesSettingsHeaderPtr(void *dataPtr, uint32_t dataOffset)
{
    // Add PracticeCodesDataHeader to start at the settings header
    return PTR_CAST_TYPE_ADD_OFFSET(MemoryWatchesSettingsHeader *, dataPtr, sizeof(PracticeCodesDataHeader) + dataOffset);
}

INLINE_FUNC MemoryWatchesSettingsData *getMemoryWatchesSettingsDataPtr(void *dataPtr, uint32_t dataOffset)
{
    // Add PracticeCodesDataHeader to start at the settings header
    return PTR_CAST_TYPE_ADD_OFFSET(MemoryWatchesSettingsData *, dataPtr, sizeof(PracticeCodesDataHeader) + dataOffset);
}

INLINE_FUNC MemoryEditorSettingsHeader *getMemoryEditorSettingsHeaderPtr(void *dataPtr, uint32_t dataOffset)
{
    // Add PracticeCodesDataHeader to start at the settings header
    return PTR_CAST_TYPE_ADD_OFFSET(MemoryEditorSettingsHeader *, dataPtr, sizeof(PracticeCodesDataHeader) + dataOffset);
}

INLINE_FUNC MemoryEditorSettingsData *getMemoryEditorSettingsDataPtr(void *dataPtr, uint32_t dataOffset)
{
    // Add PracticeCodesDataHeader to start at the settings header
    return PTR_CAST_TYPE_ADD_OFFSET(MemoryEditorSettingsData *, dataPtr, sizeof(PracticeCodesDataHeader) + dataOffset);
}

INLINE_FUNC CustomStatesSettingsHeader *getCustomStatesSettingsHeaderPtr(void *dataPtr, uint32_t dataOffset)
{
    // Add PracticeCodesDataHeader to start at the settings header
    return PTR_CAST_TYPE_ADD_OFFSET(CustomStatesSettingsHeader *, dataPtr, sizeof(PracticeCodesDataHeader) + dataOffset);
}

INLINE_FUNC CustomStatesSettingsData *getCustomStatesSettingsDataPtr(void *dataPtr, uint32_t dataOffset)
{
    // Add PracticeCodesDataHeader to start at the settings header
    return PTR_CAST_TYPE_ADD_OFFSET(CustomStatesSettingsData *, dataPtr, sizeof(PracticeCodesDataHeader) + dataOffset);
}

// Misc memory card functions
int32_t finishAsyncMemoryCardFunc(int32_t ret, int32_t channel);
int32_t loadSettings(int32_t channel);
int32_t saveSettings(int32_t channel);
int32_t deleteSettingsFile(int32_t channel);

INLINE_FUNC int32_t createFileOnMemoryCard(int32_t channel, const char *fileNamePtr, uint32_t size, CARDFileInfo *fileInfoPtr)
{
    const int32_t ret = CARDCreateAsync(channel, fileNamePtr, size, fileInfoPtr, nullptr);
    return finishAsyncMemoryCardFunc(ret, channel);
}

INLINE_FUNC int32_t
    readFromFileOnMemoryCard(int32_t channel, CARDFileInfo *fileInfoPtr, void *bufferPtr, int32_t length, int32_t offset)
{
    const int32_t ret = CARDReadAsync(fileInfoPtr, bufferPtr, length, offset, nullptr);
    return finishAsyncMemoryCardFunc(ret, channel);
}

INLINE_FUNC int32_t
    writeToFileOnMemoryCard(int32_t channel, CARDFileInfo *fileInfoPtr, void *bufferPtr, int32_t length, int32_t offset)
{
    const int32_t ret = CARDWriteAsync(fileInfoPtr, bufferPtr, length, offset, nullptr);
    return finishAsyncMemoryCardFunc(ret, channel);
}

INLINE_FUNC int32_t setMemoryCardFileStatus(int32_t channel, int32_t fileNum, CARDStat *statPtr)
{
    const int32_t ret = CARDSetStatusAsync(channel, fileNum, statPtr, nullptr);
    return finishAsyncMemoryCardFunc(ret, channel);
}

INLINE_FUNC int32_t renameFileOnMemoryCard(int32_t channel, const char *oldNamePtr, const char *newNamePtr)
{
    const int32_t ret = card_rename_async(channel, oldNamePtr, newNamePtr, nullptr);
    return finishAsyncMemoryCardFunc(ret, channel);
}

#endif
