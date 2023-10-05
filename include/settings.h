#ifndef SETTINGS_H
#define SETTINGS_H

#include "cheats.h"
#include "displays.h"
#include "mod.h"
#include "memoryWatch.h"
#include "memoryEditor.h"
#include "customState.h"
#include "gc/card.h"
#include "menus/rootMenu.h"
#include "ttyd/item_data.h"
#include "ttyd/mario_pouch.h"

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cinttypes>

#define SETTINGS_VERSION 1

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

    void setData(const char *title, const char *description)
    {
        strncpy(this->title, title, sizeof(this->title));
        strncpy(this->description, description, sizeof(this->description));
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

    uint32_t setData(uint32_t version, uint32_t dataSize)
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

    uint32_t setData(SettingsHeader *settingsHeaderPtr, uint32_t offset)
    {
        // Write the offset to the misc data
        settingsHeaderPtr->setMiscHeaderOffset(offset);

        constexpr uint32_t headerSize = sizeof(MiscSettingsHeader);
        this->dataOffset = offset + headerSize;

        return headerSize;
    }

   private:
    // Starts at the start of SettingsHeader. If the value is 0, then the code will assume that the section is not included.
    uint32_t dataOffset;
} __attribute__((aligned(sizeof(uint32_t)), packed));

class MiscSettingsData
{
   public:
    MiscSettingsData() {}
    ~MiscSettingsData() {}

    void getData() const
    {
        const uint32_t *colorPtr = PTR_CAST_TYPE_ADD_OFFSET(const uint32_t *, this, 0);
        gRootWindow->setColor(*colorPtr);
    }

    uint32_t setData()
    {
        this->rootWindowColor = gRootWindow->getColor();

        return sizeof(MiscSettingsData);
    }

   private:
    uint32_t rootWindowColor;
} __attribute__((aligned(sizeof(uint32_t)), packed));

class CheatsSettingsHeader
{
   public:
    CheatsSettingsHeader() {}
    ~CheatsSettingsHeader() {}

    uint32_t getDataOffset() const { return this->dataOffset; }
    uint32_t getTotalCheats() const { return this->totalCheats; }
    uint32_t getTotalButtonCombos() const { return this->totalButtonCombos; }

    uint32_t setData(SettingsHeader *settingsHeaderPtr, uint32_t offset)
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

    void getData(const uint32_t totalCheats, const uint32_t totalButtonCombos) const
    {
        // Get all of the enabled flags
        const uint32_t *enabledFlagsPtr = PTR_CAST_TYPE_ADD_OFFSET(const uint32_t *, this, 0);
        constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
        Cheats *cheatsPtr = gCheats;

        // Make sure totalCheats does not exceed the current max
        // Cannot change totalCheats, as doing so will give incorrect offsets for later data
        uint32_t maxCheats = totalCheats;
        if (maxCheats > TOTAL_CHEATS_ENABLED_FLAGS)
        {
            maxCheats = TOTAL_CHEATS_ENABLED_FLAGS;
        }

        for (uint32_t i = 0; i < maxCheats; i++)
        {
            if ((enabledFlagsPtr[i / bitsPerWord] >> (i % bitsPerWord)) & 1U)
            {
                cheatsPtr->setEnabledFlag(i);
            }
            else
            {
                cheatsPtr->clearEnabledFlag(i);
            }
        }

        // Get all of the button combos
        const uint32_t buttonCombosOffset = intCeil(totalCheats, bitsPerWord) * sizeof(uint32_t);
        const uint16_t *buttonCombosPtr = PTR_CAST_TYPE_ADD_OFFSET(const uint16_t *, this, buttonCombosOffset);

        // Make sure totalButtonCombos does not exceed the current max
        // Cannot change totalButtonCombos, as doing so will give incorrect offsets for later data
        uint32_t maxButtonCombos = totalButtonCombos;
        if (maxButtonCombos > TOTAL_CHEATS_BUTTON_COMBOS)
        {
            maxButtonCombos = TOTAL_CHEATS_BUTTON_COMBOS;
        }

        memcpy(cheatsPtr->getButtonCombosPtr(), buttonCombosPtr, maxButtonCombos * sizeof(uint16_t));
    }

    uint32_t setData(Cheats *cheatsPtr)
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

    uint32_t setData(SettingsHeader *settingsHeaderPtr, uint32_t offset)
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

    void getData(const uint32_t totalDisplays,
                 const uint32_t totalManuallyPositionEntries,
                 const uint32_t totalButtonCombos) const
    {
        // Get all of the enabled flags
        const uint32_t *enabledFlagsPtr = PTR_CAST_TYPE_ADD_OFFSET(const uint32_t *, this, 0);
        constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
        Displays *displaysPtr = gDisplays;

        // Make sure totalDisplays does not exceed the current max
        // Cannot change totalDisplays, as doing so will give incorrect offsets for later data
        uint32_t maxDisplays = totalDisplays;
        if (maxDisplays > TOTAL_DISPLAYS_ENABLED_FLAGS)
        {
            maxDisplays = TOTAL_DISPLAYS_ENABLED_FLAGS;
        }

        for (uint32_t i = 0; i < maxDisplays; i++)
        {
            if ((enabledFlagsPtr[i / bitsPerWord] >> (i % bitsPerWord)) & 1U)
            {
                displaysPtr->setEnabledFlag(i);
            }
            else
            {
                displaysPtr->clearEnabledFlag(i);
            }
        }

        // Get all of the manually position flags
        uint32_t offset = intCeil(totalDisplays, bitsPerWord) * sizeof(uint32_t);
        const uint32_t *manuallyPositionFlagsPtr = PTR_CAST_TYPE_ADD_OFFSET(const uint32_t *, this, offset);

        // Make sure totalManuallyPositionEntries does not exceed the current max
        // Cannot change totalManuallyPositionEntries, as doing so will give incorrect offsets for later data
        uint32_t maxManuallyPositionEntries = totalManuallyPositionEntries;
        if (maxManuallyPositionEntries > TOTAL_DISPLAYS_MANUALLY_POSITION_FLAGS)
        {
            maxManuallyPositionEntries = TOTAL_DISPLAYS_MANUALLY_POSITION_FLAGS;
        }

        for (uint32_t i = 0; i < maxManuallyPositionEntries; i++)
        {
            if ((manuallyPositionFlagsPtr[i / bitsPerWord] >> (i % bitsPerWord)) & 1U)
            {
                displaysPtr->setManuallyPositionFlag(i);
            }
            else
            {
                displaysPtr->clearManuallyPositionFlag(i);
            }
        }

        offset += intCeil(totalManuallyPositionEntries, bitsPerWord) * sizeof(uint32_t);

        // Get all of the manually position positions and scales
        const DisplayManuallyPosition *manuallyPositionPtr =
            PTR_CAST_TYPE_ADD_OFFSET(const DisplayManuallyPosition *, this, offset);

        DisplayManuallyPosition *entriesPtr = displaysPtr->getManuallyPositionEntriesPtr();

        for (uint32_t i = 0; i < totalManuallyPositionEntries; i++)
        {
            offset += sizeof(float); // Pos X
            offset += sizeof(float); // Pos Y
            offset += sizeof(float); // Scale

            // Only set the values if the max amount of entries has not been reached
            if (i < maxManuallyPositionEntries)
            {
                const DisplayManuallyPosition *currentManuallyPositionPtr = &manuallyPositionPtr[i];
                DisplayManuallyPosition *currentEntryPtr = &entriesPtr[i];

                currentEntryPtr->setPosX(currentManuallyPositionPtr->getPosX());
                currentEntryPtr->setPosY(currentManuallyPositionPtr->getPosY());
                currentEntryPtr->setScale(currentManuallyPositionPtr->getScale());
            }
        }

        // Make sure totalButtonCombos does not exceed the current max
        // Cannot change totalButtonCombos, as doing so will give incorrect offsets for later data
        uint32_t maxButtonCombos = totalButtonCombos;
        if (maxButtonCombos > TOTAL_DISPLAYS_BUTTON_COMBOS)
        {
            maxButtonCombos = TOTAL_DISPLAYS_BUTTON_COMBOS;
        }

        // Get all of the button combos
        const uint16_t *buttonCombosPtr = PTR_CAST_TYPE_ADD_OFFSET(const uint16_t *, this, offset);
        memcpy(displaysPtr->getButtonCombosPtr(), buttonCombosPtr, maxButtonCombos * sizeof(uint16_t));
        offset += roundUp(totalButtonCombos, sizeof(uint16_t)) * sizeof(uint16_t);

        // Get the Hit Check Visualization hits and misses colors
        HitCheckVisualizationDisplay *hitCheckVisualizationPtr = displaysPtr->getHitCheckVisualizationDisplayPtr();

        const uint32_t *hitsColorPtr = PTR_CAST_TYPE_ADD_OFFSET(const uint32_t *, this, offset);
        hitCheckVisualizationPtr->setHitsColor(*hitsColorPtr);
        offset += sizeof(uint32_t);

        const uint32_t *missesColorPtr = PTR_CAST_TYPE_ADD_OFFSET(const uint32_t *, this, offset);
        hitCheckVisualizationPtr->setMissesColor(*missesColorPtr);
    }

    uint32_t setData(Displays *displaysPtr)
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
    uint32_t setData(SettingsHeader *settingsHeaderPtr, MemoryWatch *memoryWatchPtr, uint32_t offset)
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

    uint32_t getData(const uint32_t totalWatches, const uint32_t watchNameLength) const
    {
        // Make sure totalWatches does not exceed the current max
        // Cannot change totalWatches, as doing so will give incorrect offsets for later data
        uint32_t maxWatches = totalWatches;
        if (maxWatches > MAX_MEMORY_WATCHES)
        {
            maxWatches = MAX_MEMORY_WATCHES;
        }

        // Reinitialize the memory for the entries
        MemoryWatch *memoryWatchPtr = &gMemoryWatch;
        MemoryWatchEntry *entriesPtr = memoryWatchPtr->reinitEntries(maxWatches);
        uint32_t offset = 0;

        // Make sure watchNameLength does not exceed the current max
        // Cannot change watchNameLength, as doing so will give incorrect offsets for later data
        uint32_t nameLength = watchNameLength;
        if (nameLength > MEMORY_WATCH_NAME_SIZE)
        {
            nameLength = MEMORY_WATCH_NAME_SIZE;
        }

        // Get the main data for each watch
        for (uint32_t i = 0; i < totalWatches; i++)
        {
            if (i >= maxWatches)
            {
                // Reached the max number of watches, so only increment the offset
                offset += sizeof(uint8_t *);    // Address
                offset += sizeof(float);        // Pos X
                offset += sizeof(float);        // Pos Y
                offset += sizeof(float);        // Scale
                offset += sizeof(uint8_t);      // Total Address Offsets
                offset += sizeof(VariableType); // Type
                offset += sizeof(bool);         // Show As Hex
                offset += sizeof(bool);         // Display
                offset += watchNameLength;      // Name

                // Make sure offset is aligned to 4 bytes, since the class is aligned to 4 bytes
                offset = roundUp(offset, sizeof(uint32_t));

                continue;
            }

            MemoryWatchEntry *currentEntryPtr = &entriesPtr[i];

            // Address
            uint8_t **addressPtr = PTR_CAST_TYPE_ADD_OFFSET(uint8_t **, this, offset);
            currentEntryPtr->setAddress(*addressPtr);
            offset += sizeof(uint8_t *);

            // Pos X
            const float *posXPtr = PTR_CAST_TYPE_ADD_OFFSET(const float *, this, offset);
            currentEntryPtr->setPosX(*posXPtr);
            offset += sizeof(float);

            // Pos Y
            const float *posYPtr = PTR_CAST_TYPE_ADD_OFFSET(const float *, this, offset);
            currentEntryPtr->setPosY(*posYPtr);
            offset += sizeof(float);

            // Scale
            const float *scalePtr = PTR_CAST_TYPE_ADD_OFFSET(const float *, this, offset);
            currentEntryPtr->setScale(*scalePtr);
            offset += sizeof(float);

            // Total Address Offsets
            const uint8_t *totalAddressOffsetsPtr = PTR_CAST_TYPE_ADD_OFFSET(const uint8_t *, this, offset);
            currentEntryPtr->setTotalAddressOffsets(*totalAddressOffsetsPtr);
            offset += sizeof(uint8_t);

            // Type
            const VariableType *typePtr = PTR_CAST_TYPE_ADD_OFFSET(const VariableType *, this, offset);
            currentEntryPtr->setType(*typePtr);
            offset += sizeof(VariableType);

            // Show As Hex
            const bool *showAsHexPtr = PTR_CAST_TYPE_ADD_OFFSET(const bool *, this, offset);
            currentEntryPtr->setShowAsHex(*showAsHexPtr);
            offset += sizeof(bool);

            // Display
            const bool *displayPtr = PTR_CAST_TYPE_ADD_OFFSET(const bool *, this, offset);
            currentEntryPtr->setDisplay(*displayPtr);
            offset += sizeof(bool);

            // Name
            // Use snprintf to make sure the buffer size is not exceeded, and that a null terminator is properly applied
            const char *namePtr = PTR_CAST_TYPE_ADD_OFFSET(const char *, this, offset);
            snprintf(currentEntryPtr->getNamePtr(), nameLength, namePtr);
            offset += watchNameLength;

            // Make sure offset is aligned to 4 bytes, since the class is aligned to 4 bytes
            offset = roundUp(offset, sizeof(uint32_t));
        }

        return offset;
    }

    void getAddressOffsetsData(uint32_t offset) const
    {
        MemoryWatch *memoryWatchPtr = &gMemoryWatch;
        MemoryWatchEntry *entriesPtr = memoryWatchPtr->getEntriesPtr();
        const uint32_t totalEntries = memoryWatchPtr->getTotalEntries();

        for (uint32_t i = 0; i < totalEntries; i++)
        {
            MemoryWatchEntry *currentEntryPtr = &entriesPtr[i];

            // Make sure there is at least one offset to get
            uint32_t totalOffsets = currentEntryPtr->getTotalAddressOffsets();
            if (totalOffsets == 0)
            {
                continue;
            }

            // Initialize the memory for the offsets
            totalOffsets = currentEntryPtr->initAddressOffsetsPtr(totalOffsets);

            // Get all of the offsets
            const uint32_t offsetsSize = totalOffsets * sizeof(int32_t);
            const int32_t *offsetsPtr = PTR_CAST_TYPE_ADD_OFFSET(const int32_t *, this, offset);
            memcpy(currentEntryPtr->getAddressOffsetsPtr(), offsetsPtr, offsetsSize);
            offset += offsetsSize;
        }
    }

    uint32_t setData(const MemoryWatchEntry *entryPtr)
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

        // Use snprintf to make sure the buffer size is not exceeded, and that a null terminator is properly applied
        snprintf(this->name, MEMORY_WATCH_NAME_SIZE, entryPtr->getNamePtrConst());

        return sizeof(MemoryWatchesSettingsData);
    }

    // This function returns 0 if there are no offsets to write
    uint32_t setAddressOffsetsData(const MemoryWatchEntry *entryPtr)
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

    uint32_t setData(SettingsHeader *settingsHeaderPtr, uint32_t offset)
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

    void getData(const uint32_t totalFlags) const
    {
        // Get all of the enabled flags
        const uint32_t *enabledFlagsPtr = PTR_CAST_TYPE_ADD_OFFSET(const uint32_t *, this, 0);
        constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
        MemoryEditor *memoryEditorPtr = gMemoryEditor;

        // Make sure totalFlags does not exceed the current max
        // Cannot change totalFlags, as doing so will give incorrect offsets for later data
        uint32_t maxFlags = totalFlags;
        if (maxFlags > TOTAL_MEMORY_EDITOR_ENABLED_FLAGS)
        {
            maxFlags = TOTAL_MEMORY_EDITOR_ENABLED_FLAGS;
        }

        for (uint32_t i = 0; i < maxFlags; i++)
        {
            if ((enabledFlagsPtr[i / bitsPerWord] >> (i % bitsPerWord)) & 1U)
            {
                memoryEditorPtr->setEnabledFlag(i);
            }
            else
            {
                memoryEditorPtr->clearEnabledFlag(i);
            }
        }

        // Get the current address
        uint32_t offset = intCeil(totalFlags, bitsPerWord) * sizeof(uint32_t);
        uint8_t **currentAddressPtr = PTR_CAST_TYPE_ADD_OFFSET(uint8_t **, this, offset);
        memoryEditorPtr->setCurrentAddress(*currentAddressPtr);
        offset += sizeof(uint8_t *);

        // Get the button combo
        const uint16_t *buttonComboPtr = PTR_CAST_TYPE_ADD_OFFSET(const uint16_t *, this, offset);
        memoryEditorPtr->setButtonCombo(*buttonComboPtr);
        offset += sizeof(uint16_t);

        // Get the number of bytes being edited
        const uint8_t *numBytesBeingEditedPtr = PTR_CAST_TYPE_ADD_OFFSET(const uint8_t *, this, offset);
        memoryEditorPtr->setNumBytesBeingEdited(*numBytesBeingEditedPtr);
    }

    uint32_t setData(const MemoryEditor *memoryEditorPtr)
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
    uint32_t setData(SettingsHeader *settingsHeaderPtr, CustomState *customStatePtr, uint32_t offset)
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

    uint32_t getData(CustomStateMarioData *dataPtr, uint32_t totalStates) const
    {
        uint32_t offset = 0;

        if (totalStates > MAX_CUSTOM_STATES)
        {
            // Reached the max number of states, so only increment the offset
            offset += sizeof(int16_t);  // Current HP
            offset += sizeof(int16_t);  // Max HP
            offset += sizeof(int16_t);  // Current FP
            offset += sizeof(int16_t);  // Max FP
            offset += sizeof(int16_t);  // Coins
            offset += sizeof(int16_t);  // Current SP
            offset += sizeof(int16_t);  // Max SP
            offset += sizeof(int16_t);  // Rank
            offset += sizeof(int16_t);  // Level
            offset += sizeof(uint16_t); // Star Powers Obtained
            offset += sizeof(int16_t);  // Base Max HP
            offset += sizeof(int16_t);  // Base Max FP
            offset += sizeof(int16_t);  // Unallocated BP
            offset += sizeof(int16_t);  // Total BP
            offset += sizeof(int16_t);  // Star Points

            return offset;
        }

        // Current HP
        const int16_t *currentHpPtr = PTR_CAST_TYPE_ADD_OFFSET(const int16_t *, this, offset);
        dataPtr->currentHP = *currentHpPtr;
        offset += sizeof(int16_t);

        // Max HP
        const int16_t *maxHpPtr = PTR_CAST_TYPE_ADD_OFFSET(const int16_t *, this, offset);
        dataPtr->maxHP = *maxHpPtr;
        offset += sizeof(int16_t);

        // Current FP
        const int16_t *currentFpPtr = PTR_CAST_TYPE_ADD_OFFSET(const int16_t *, this, offset);
        dataPtr->currentFP = *currentFpPtr;
        offset += sizeof(int16_t);

        // Max FP
        const int16_t *maxFpPtr = PTR_CAST_TYPE_ADD_OFFSET(const int16_t *, this, offset);
        dataPtr->maxFP = *maxFpPtr;
        offset += sizeof(int16_t);

        // Coins
        const int16_t *coinsPtr = PTR_CAST_TYPE_ADD_OFFSET(const int16_t *, this, offset);
        dataPtr->coins = *coinsPtr;
        offset += sizeof(int16_t);

        // Current SP
        const int16_t *currentSpPtr = PTR_CAST_TYPE_ADD_OFFSET(const int16_t *, this, offset);
        dataPtr->currentSP = *currentSpPtr;
        offset += sizeof(int16_t);

        // Max SP
        const int16_t *maxSpPtr = PTR_CAST_TYPE_ADD_OFFSET(const int16_t *, this, offset);
        dataPtr->maxSP = *maxSpPtr;
        offset += sizeof(int16_t);

        // Rank
        const int16_t *rankPtr = PTR_CAST_TYPE_ADD_OFFSET(const int16_t *, this, offset);
        dataPtr->rank = *rankPtr;
        offset += sizeof(int16_t);

        // Level
        const int16_t *levelPtr = PTR_CAST_TYPE_ADD_OFFSET(const int16_t *, this, offset);
        dataPtr->level = *levelPtr;
        offset += sizeof(uint16_t);

        // Star Powers Obtained
        const uint16_t *starPowersObtainedPtr = PTR_CAST_TYPE_ADD_OFFSET(const uint16_t *, this, offset);
        dataPtr->starPowersObtained = *starPowersObtainedPtr;
        offset += sizeof(int16_t);

        // Base Max HP
        const int16_t *baseMaxHpPtr = PTR_CAST_TYPE_ADD_OFFSET(const int16_t *, this, offset);
        dataPtr->baseMaxHp = *baseMaxHpPtr;
        offset += sizeof(int16_t);

        // Base Max FP
        const int16_t *baseMaxFpPtr = PTR_CAST_TYPE_ADD_OFFSET(const int16_t *, this, offset);
        dataPtr->baseMaxFp = *baseMaxFpPtr;
        offset += sizeof(int16_t);

        // Unallocated BP
        const int16_t *unallocatedBpPtr = PTR_CAST_TYPE_ADD_OFFSET(const int16_t *, this, offset);
        dataPtr->unallocatedBp = *unallocatedBpPtr;
        offset += sizeof(int16_t);

        // Total BP
        const int16_t *totalBpPtr = PTR_CAST_TYPE_ADD_OFFSET(const int16_t *, this, offset);
        dataPtr->totalBp = *totalBpPtr;
        offset += sizeof(int16_t);

        // Star Points
        const int16_t *starPointsPtr = PTR_CAST_TYPE_ADD_OFFSET(const int16_t *, this, offset);
        dataPtr->starPoints = *starPointsPtr;
        offset += sizeof(int16_t);

        return offset;
    }

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

    uint32_t getData(const uint32_t totalStates, const uint32_t stateNameLength) const
    {
        // Make sure totalStates does not exceed the current max
        // Cannot change totalStates, as doing so will give incorrect offsets for later data
        uint32_t maxStates = totalStates;
        if (maxStates > MAX_CUSTOM_STATES)
        {
            maxStates = MAX_CUSTOM_STATES;
        }

        // Reinitialize the memory for the entries
        CustomState *customStatePtr = &gCustomState;
        CustomStateEntry *entriesPtr = customStatePtr->reinitEntries(maxStates);

        // Make sure stateNameLength does not exceed the current max
        // Cannot change stateNameLength, as doing so will give incorrect offsets for later data
        uint32_t nameLength = stateNameLength;
        if (nameLength > CUSTOM_STATE_NAME_SIZE)
        {
            nameLength = CUSTOM_STATE_NAME_SIZE;
        }

        // Get the data for each state
        uint32_t offset = 0;
        for (uint32_t i = 0; i < totalStates; i++)
        {
            CustomStateEntry *currentEntryPtr = &entriesPtr[i];

            if (i >= maxStates)
            {
                // Reached the max number of states, so only increment the offset
                offset += sizeof(this->items);          // Standard items
                offset += sizeof(this->keyItems);       // Important items
                offset += sizeof(this->storedItems);    // Stored items
                offset += sizeof(this->badges);         // Badges
                offset += sizeof(this->equippedBadges); // Equipped badges
                offset += sizeof(uint16_t);             // Sequence Position

                const CustomStateSettingsMarioData *marioDataPtr =
                    PTR_CAST_TYPE_ADD_OFFSET(const CustomStateSettingsMarioData *, this, offset);

                offset += marioDataPtr->getData(currentEntryPtr->getMarioDataPtr(), totalStates); // Some of Mario's data

                offset += sizeof(this->partyData);   // Pouch data
                offset += sizeof(PartyMembers);      // Partner out
                offset += sizeof(PartyMembers);      // Follower out
                offset += sizeof(bool);              // In boat mode
                offset += sizeof(this->currentMap);  // Current map
                offset += sizeof(this->currentBero); // Current bero
                offset += stateNameLength;           // State name

                // Make sure offset is aligned to 4 bytes, since the class is aligned to 4 bytes
                offset = roundUp(offset, sizeof(uint32_t));

                continue;
            }

            // Standard items
            constexpr uint32_t itemsSize = sizeof(this->items);
            const ItemId *itemsPtr = PTR_CAST_TYPE_ADD_OFFSET(const ItemId *, this, offset);
            memcpy(currentEntryPtr->getItemsPtr(), itemsPtr, itemsSize);
            offset += itemsSize;

            // Important items
            constexpr uint32_t keyItemsSize = sizeof(this->keyItems);
            const ItemId *keyItemsPtr = PTR_CAST_TYPE_ADD_OFFSET(const ItemId *, this, offset);
            memcpy(currentEntryPtr->getKeyItemsPtr(), keyItemsPtr, keyItemsSize);
            offset += keyItemsSize;

            // Stored items
            constexpr uint32_t storedItemsSize = sizeof(this->storedItems);
            const ItemId *storedItemsPtr = PTR_CAST_TYPE_ADD_OFFSET(const ItemId *, this, offset);
            memcpy(currentEntryPtr->getStoredItemsPtr(), storedItemsPtr, storedItemsSize);
            offset += storedItemsSize;

            // Badges
            constexpr uint32_t badgesSize = sizeof(this->badges);
            const ItemId *badgesPtr = PTR_CAST_TYPE_ADD_OFFSET(const ItemId *, this, offset);
            memcpy(currentEntryPtr->getBadgesPtr(), badgesPtr, badgesSize);
            offset += badgesSize;

            // Equipped badges
            constexpr uint32_t equippedBadgesSize = sizeof(this->equippedBadges);
            const ItemId *equippedBadgesPtr = PTR_CAST_TYPE_ADD_OFFSET(const ItemId *, this, offset);
            memcpy(currentEntryPtr->getEquippedBadgesPtr(), equippedBadgesPtr, equippedBadgesSize);
            offset += equippedBadgesSize;

            // Sequence Position
            const uint16_t *sequencePositionPtr = PTR_CAST_TYPE_ADD_OFFSET(const uint16_t *, this, offset);
            currentEntryPtr->setSequencePosition(*sequencePositionPtr);
            offset += sizeof(uint16_t);

            // Some of Mario's data
            const CustomStateSettingsMarioData *marioDataPtr =
                PTR_CAST_TYPE_ADD_OFFSET(const CustomStateSettingsMarioData *, this, offset);

            offset += marioDataPtr->getData(currentEntryPtr->getMarioDataPtr(), totalStates);

            // Pouch data
            constexpr uint32_t pouchDataSize = sizeof(this->partyData);
            const PouchData *pouchDataPtr = PTR_CAST_TYPE_ADD_OFFSET(const PouchData *, this, offset);
            memcpy(currentEntryPtr->getPouchPartyDataPtr(), pouchDataPtr, pouchDataSize);
            offset += pouchDataSize;

            // Partner out
            const PartyMembers *partnerOutPtr = PTR_CAST_TYPE_ADD_OFFSET(const PartyMembers *, this, offset);
            currentEntryPtr->setPartnerOut(*partnerOutPtr);
            offset += sizeof(PartyMembers);

            // Follower out
            const PartyMembers *followerOutPtr = PTR_CAST_TYPE_ADD_OFFSET(const PartyMembers *, this, offset);
            currentEntryPtr->setFollowerOut(*followerOutPtr);
            offset += sizeof(PartyMembers);

            // In boat mode
            const bool *inBoatModePtr = PTR_CAST_TYPE_ADD_OFFSET(const bool *, this, offset);
            currentEntryPtr->setInBoatMoad(*inBoatModePtr);
            offset += sizeof(bool);

            // Current map
            constexpr uint32_t currentMapSize = sizeof(this->currentMap);
            const char *currentMapPtr = PTR_CAST_TYPE_ADD_OFFSET(const char *, this, offset);
            strncpy(currentEntryPtr->getCurrentMapPtr(), currentMapPtr, currentMapSize);
            offset += currentMapSize;

            // Current bero
            constexpr uint32_t currentBeroSize = sizeof(this->currentBero);
            const char *currentBeroPtr = PTR_CAST_TYPE_ADD_OFFSET(const char *, this, offset);
            strncpy(currentEntryPtr->getCurrentBeroPtr(), currentBeroPtr, currentBeroSize);
            offset += currentBeroSize;

            // State name
            const char *stateNamePtr = PTR_CAST_TYPE_ADD_OFFSET(const char *, this, offset);
            strncpy(currentEntryPtr->getStateNamePtr(), stateNamePtr, nameLength);
            offset += stateNameLength;

            // Make sure offset is aligned to 4 bytes, since the class is aligned to 4 bytes
            offset = roundUp(offset, sizeof(uint32_t));
        }

        return offset;
    }

    // This function assumes that there is at least one state to write
    uint32_t setData(const CustomState *customStatePtr)
    {
        // Write all of the states
        const uint32_t totalStatesSize = customStatePtr->getTotalEntries() * sizeof(CustomStatesSettingsData);
        memcpy(this, customStatePtr->getEntriesPtr(), totalStatesSize);

        return totalStatesSize;
    }

   private:
    ItemId items[sizeof(PouchData::items) / sizeof(ItemId)];
    ItemId keyItems[sizeof(PouchData::keyItems) / sizeof(ItemId)];
    ItemId storedItems[sizeof(PouchData::storedItems) / sizeof(ItemId)];
    ItemId badges[sizeof(PouchData::badges) / sizeof(ItemId)];
    ItemId equippedBadges[sizeof(PouchData::equippedBadges) / sizeof(ItemId)];
    uint16_t sequencePosition;
    CustomStateSettingsMarioData marioData;
    PouchPartyData partyData[7];
    PartyMembers partnerOut;
    PartyMembers followerOut;
    bool inBoatMode;                        // Mario should be in boat mode upon entering the room
    char currentMap[8];                     // Does not include a null terminator
    char currentBero[16];                   // Does not include a null terminator
    char stateName[CUSTOM_STATE_NAME_SIZE]; // Does not include a null terminator
} __attribute__((aligned(sizeof(uint32_t)), packed));

static_assert(sizeof(PracticeCodesDataHeader) == 0x40);
static_assert(sizeof(SettingsHeader) == 0x20);
static_assert(sizeof(MiscSettingsHeader) == 0x4);
static_assert(sizeof(MiscSettingsData) == 0x4);
static_assert(sizeof(CheatsSettingsHeader) == 0x8);
static_assert(sizeof(CheatsSettingsData) == 0x28);
static_assert(sizeof(DisplaysSettingsHeader) == 0xC);
static_assert(sizeof(DisplaysSettingsData) == 0x100);
static_assert(sizeof(MemoryWatchesSettingsHeader) == 0x8);
static_assert(sizeof(MemoryWatchesSettingsData) == 0x24); // Excludes addressOffsets
static_assert(sizeof(MemoryEditorSettingsHeader) == 0x8);
static_assert(sizeof(MemoryEditorSettingsData) == 0xC);
static_assert(sizeof(CustomStatesSettingsHeader) == 0x8);
static_assert(sizeof(CustomStateSettingsMarioData) == 0x1E);
static_assert(sizeof(CustomStatesSettingsData) == 0x528);

inline uint32_t getWrittenSettingsSize()
{
    // Get the size used by all of the memory watches and all of their pointer levels
    MemoryWatch *memoryWatchPtr = &gMemoryWatch;
    const uint32_t totalMemoryWatches = memoryWatchPtr->getTotalEntries();
    const MemoryWatchEntry *memoryWatchEntriesPtr = memoryWatchPtr->getEntriesPtr();

    uint32_t memoryWatchesSize = 0;
    if ((totalMemoryWatches > 0) && memoryWatchEntriesPtr)
    {
        memoryWatchesSize += sizeof(MemoryWatchesSettingsHeader);
        memoryWatchesSize += totalMemoryWatches * sizeof(MemoryWatchesSettingsData);

        for (uint32_t i = 0; i < totalMemoryWatches; i++)
        {
            memoryWatchesSize += memoryWatchEntriesPtr[i].getTotalAddressOffsets() * sizeof(int32_t);
        }
    }

    // Get the size used by all of the custom states
    const CustomState *customStatePtr = &gCustomState;
    const uint32_t totalCustomStates = customStatePtr->getTotalEntries();
    const CustomStateEntry *customStatesEntriesPtr = customStatePtr->getEntriesPtr();

    uint32_t customStatesSize = 0;
    if ((totalCustomStates > 0) && customStatesEntriesPtr)
    {
        customStatesSize += sizeof(CustomStatesSettingsHeader);
        customStatesSize += totalCustomStates * sizeof(CustomStatesSettingsData);
    }

    return sizeof(PracticeCodesDataHeader) + sizeof(SettingsHeader) + sizeof(MiscSettingsHeader) + sizeof(MiscSettingsData) +
           sizeof(CheatsSettingsHeader) + sizeof(CheatsSettingsData) + sizeof(DisplaysSettingsHeader) +
           sizeof(DisplaysSettingsData) + sizeof(MemoryEditorSettingsHeader) + sizeof(MemoryEditorSettingsData) +
           memoryWatchesSize + customStatesSize;
}

// Helper functions for getting the pointers to the headers
inline PracticeCodesDataHeader *getPracticeCodesDataHeaderPtr(void *dataPtr, uint32_t dataOffset)
{
    (void)dataOffset;

    // PracticeCodesDataHeader should always be at the start of the data
    return PTR_CAST_TYPE_ADD_OFFSET(PracticeCodesDataHeader *, dataPtr, 0);
}

inline SettingsHeader *getSettingsHeaderPtr(void *dataPtr, uint32_t dataOffset)
{
    (void)dataOffset;

    // SettingsHeader should always be directly after PracticeCodesDataHeader
    return PTR_CAST_TYPE_ADD_OFFSET(SettingsHeader *, dataPtr, sizeof(PracticeCodesDataHeader));
}

inline MiscSettingsHeader *getMiscSettingsHeaderPtr(void *dataPtr, uint32_t dataOffset)
{
    // Add PracticeCodesDataHeader to start at the settings header
    return PTR_CAST_TYPE_ADD_OFFSET(MiscSettingsHeader *, dataPtr, sizeof(PracticeCodesDataHeader) + dataOffset);
}

inline MiscSettingsData *getMiscSettingsDataPtr(void *dataPtr, uint32_t dataOffset)
{
    // Add PracticeCodesDataHeader to start at the settings header
    return PTR_CAST_TYPE_ADD_OFFSET(MiscSettingsData *, dataPtr, sizeof(PracticeCodesDataHeader) + dataOffset);
}

inline CheatsSettingsHeader *getCheatsSettingsHeaderPtr(void *dataPtr, uint32_t dataOffset)
{
    // Add PracticeCodesDataHeader to start at the settings header
    return PTR_CAST_TYPE_ADD_OFFSET(CheatsSettingsHeader *, dataPtr, sizeof(PracticeCodesDataHeader) + dataOffset);
}

inline CheatsSettingsData *getCheatsSettingsDataPtr(void *dataPtr, uint32_t dataOffset)
{
    // Add PracticeCodesDataHeader to start at the settings header
    return PTR_CAST_TYPE_ADD_OFFSET(CheatsSettingsData *, dataPtr, sizeof(PracticeCodesDataHeader) + dataOffset);
}

inline DisplaysSettingsHeader *getDisplaysSettingsHeaderPtr(void *dataPtr, uint32_t dataOffset)
{
    // Add PracticeCodesDataHeader to start at the settings header
    return PTR_CAST_TYPE_ADD_OFFSET(DisplaysSettingsHeader *, dataPtr, sizeof(PracticeCodesDataHeader) + dataOffset);
}

inline DisplaysSettingsData *getDisplaysSettingsDataPtr(void *dataPtr, uint32_t dataOffset)
{
    // Add PracticeCodesDataHeader to start at the settings header
    return PTR_CAST_TYPE_ADD_OFFSET(DisplaysSettingsData *, dataPtr, sizeof(PracticeCodesDataHeader) + dataOffset);
}

inline MemoryWatchesSettingsHeader *getMemoryWatchesSettingsHeaderPtr(void *dataPtr, uint32_t dataOffset)
{
    // Add PracticeCodesDataHeader to start at the settings header
    return PTR_CAST_TYPE_ADD_OFFSET(MemoryWatchesSettingsHeader *, dataPtr, sizeof(PracticeCodesDataHeader) + dataOffset);
}

inline MemoryWatchesSettingsData *getMemoryWatchesSettingsDataPtr(void *dataPtr, uint32_t dataOffset)
{
    // Add PracticeCodesDataHeader to start at the settings header
    return PTR_CAST_TYPE_ADD_OFFSET(MemoryWatchesSettingsData *, dataPtr, sizeof(PracticeCodesDataHeader) + dataOffset);
}

inline MemoryEditorSettingsHeader *getMemoryEditorSettingsHeaderPtr(void *dataPtr, uint32_t dataOffset)
{
    // Add PracticeCodesDataHeader to start at the settings header
    return PTR_CAST_TYPE_ADD_OFFSET(MemoryEditorSettingsHeader *, dataPtr, sizeof(PracticeCodesDataHeader) + dataOffset);
}

inline MemoryEditorSettingsData *getMemoryEditorSettingsDataPtr(void *dataPtr, uint32_t dataOffset)
{
    // Add PracticeCodesDataHeader to start at the settings header
    return PTR_CAST_TYPE_ADD_OFFSET(MemoryEditorSettingsData *, dataPtr, sizeof(PracticeCodesDataHeader) + dataOffset);
}

inline CustomStatesSettingsHeader *getCustomStatesSettingsHeaderPtr(void *dataPtr, uint32_t dataOffset)
{
    // Add PracticeCodesDataHeader to start at the settings header
    return PTR_CAST_TYPE_ADD_OFFSET(CustomStatesSettingsHeader *, dataPtr, sizeof(PracticeCodesDataHeader) + dataOffset);
}

inline CustomStatesSettingsData *getCustomStatesSettingsDataPtr(void *dataPtr, uint32_t dataOffset)
{
    // Add PracticeCodesDataHeader to start at the settings header
    return PTR_CAST_TYPE_ADD_OFFSET(CustomStatesSettingsData *, dataPtr, sizeof(PracticeCodesDataHeader) + dataOffset);
}

// Misc memory card functions
int32_t finishAsyncMemoryCardFunc(int32_t ret, int32_t channel);
int32_t loadSettings(int32_t channel);
int32_t saveSettings(int32_t channel);
int32_t deleteSettingsFile(int32_t channel);

__attribute__((always_inline)) inline int32_t createFileOnMemoryCard(int32_t channel,
                                                                     const char *fileNamePtr,
                                                                     uint32_t size,
                                                                     CARDFileInfo *fileInfoPtr)
{
    const int32_t ret = CARDCreateAsync(channel, fileNamePtr, size, fileInfoPtr, nullptr);
    return finishAsyncMemoryCardFunc(ret, channel);
}

__attribute__((always_inline)) inline int32_t readFromFileOnMemoryCard(int32_t channel,
                                                                       CARDFileInfo *fileInfoPtr,
                                                                       void *bufferPtr,
                                                                       int32_t length,
                                                                       int32_t offset)
{
    const int32_t ret = CARDReadAsync(fileInfoPtr, bufferPtr, length, offset, nullptr);
    return finishAsyncMemoryCardFunc(ret, channel);
}

__attribute__((always_inline)) inline int32_t writeToFileOnMemoryCard(int32_t channel,
                                                                      CARDFileInfo *fileInfoPtr,
                                                                      void *bufferPtr,
                                                                      int32_t length,
                                                                      int32_t offset)
{
    const int32_t ret = CARDWriteAsync(fileInfoPtr, bufferPtr, length, offset, nullptr);
    return finishAsyncMemoryCardFunc(ret, channel);
}

__attribute__((always_inline)) inline int32_t setMemoryCardFileStatus(int32_t channel, int32_t fileNum, CARDStat *statPtr)
{
    const int32_t ret = CARDSetStatusAsync(channel, fileNum, statPtr, nullptr);
    return finishAsyncMemoryCardFunc(ret, channel);
}

__attribute__((always_inline)) inline int32_t renameFileOnMemoryCard(int32_t channel,
                                                                     const char *oldNamePtr,
                                                                     const char *newNamePtr)
{
    const int32_t ret = card_rename_async(channel, oldNamePtr, newNamePtr, nullptr);
    return finishAsyncMemoryCardFunc(ret, channel);
}

#endif
