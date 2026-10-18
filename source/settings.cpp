#include "settings.h"
#include "mod.h"
#include "cheats.h"
#include "displays.h"
#include "memoryWatch.h"
#include "memoryEditor.h"
#include "customState.h"
#include "cxx.h"
#include "gc/card.h"
#include "menus/rootMenu.h"
#include "misc/utils.h"
#include "ttyd/mario_pouch.h"
#include "ttyd/party.h"
#include "ttyd/item_data.h"
#include "ttyd/cardmgr.h"

#include <cstdint>
#include <cstring>

static const char *gSettingsFileName = "Practice_Codes_Data";

#ifdef TTYD_US
static const char *gSettingsDescription = "Settings (US)";
#elif defined TTYD_JP
static const char *gSettingsDescription = "Settings (JP)";
#elif defined TTYD_EU
static const char *gSettingsDescription = "Settings (EU)";
#endif

void MiscSettingsData::getData(uint32_t version, uint32_t totalFlags) const
{
    const uint32_t *colorPtr = PTR_CAST_TYPE_ADD_OFFSET(const uint32_t *, this, 0);
    gRootWindow->setColor(*colorPtr);

    if (version >= SETTINGS_MOVED_MISC_SAVED_FLAGS)
    {
        // Get all of the enabled flags
        const uint32_t *enabledFlagsPtr = this->enabledFlags;
        Mod *modPtr = gMod;

        // Make sure totalFlags does not exceed the current max
        // Cannot change totalFlags, as doing so will give incorrect offsets for later data
        uint32_t maxFlags = totalFlags;
        if (maxFlags > TOTAL_MOD_SAVE_FLAGS)
        {
            maxFlags = TOTAL_MOD_SAVE_FLAGS;
        }

        for (uint32_t i = 0; i < maxFlags; i++)
        {
            bool flagIsSet;
            FLAG_IS_SET(enabledFlagsPtr, i, flagIsSet);

            if (flagIsSet)
            {
                modPtr->setSaveFlag(i);
            }
            else
            {
                modPtr->clearSaveFlag(i);
            }
        }
    }
}

void CheatsSettingsData::getData(uint32_t totalCheats, uint32_t totalButtonCombos, uint32_t version) const
{
    // Get all of the enabled flags
    const uint32_t *enabledFlagsPtr = PTR_CAST_TYPE_ADD_OFFSET(const uint32_t *, this, 0);
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
        bool flagIsSet;
        FLAG_IS_SET(enabledFlagsPtr, i, flagIsSet);

        if (flagIsSet)
        {
            cheatsPtr->setEnabledFlag(i);
        }
        else
        {
            cheatsPtr->clearEnabledFlag(i);
        }
    }

    // Certain things need to happen depending on whether the Frame Advance cheat is now enabled/disabled
    FrameAdvanceCheat *frameAdvancePtr = cheatsPtr->getFrameAdvanceCheatPtr();

    if (!cheatsPtr->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_FRAME_ADVANCE))
    {
        // The cheat is now disabled, so make sure memory is not allocated for it and that its misc flags are cleared
        cheatsPtr->clearMiscFlag(CheatsMiscFlag::CHEATS_MISC_FLAG_FRAME_ADVANCE_ADVANCE_FRAME);
        cheatsPtr->clearMiscFlag(CheatsMiscFlag::CHEATS_MISC_FLAG_FRAME_ADVANCE_GAME_IS_PAUSED);
        frameAdvancePtr->freePrevInputs();
    }
    else
    {
        // The cheat is now enabled, so allocate memory for the backup inputs
        frameAdvancePtr->allocMemoryForPrevInputs();

        // Clear any existing backup inputs to be safe

        // This is needed because memory could have already been allocated for the backup inputs before
        // allocMemoryForPrevInputs was called
        frameAdvancePtr->clearAllInputs();
    }

    // Get all of the button combos
    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
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

    if (version >= SETTINGS_ADDED_AMW_GLITCH_VALUES)
    {
        // Adjust the values modified by the Arbitrary Memory Write glitch
        adjustArbitraryMemoryWriteValues(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_SIMULATE_AMW_MARIO_ZERO_HITBOX_PAPER_MODE);
        adjustArbitraryMemoryWriteValues(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_SIMULATE_AMW_MARIO_ZERO_HITBOX_ROOM_TRANSITION);
        adjustArbitraryMemoryWriteValues(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_SIMULATE_AMW_MARIO_AIRWALK);
    }
}

void DisplaysSettingsData::getData(uint32_t totalDisplays,
                                   uint32_t totalManuallyPositionEntries,
                                   uint32_t totalButtonCombos) const
{
    // Get all of the enabled flags
    const uint32_t *enabledFlagsPtr = PTR_CAST_TYPE_ADD_OFFSET(const uint32_t *, this, 0);
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
        bool flagIsSet;
        FLAG_IS_SET(enabledFlagsPtr, i, flagIsSet);

        if (flagIsSet)
        {
            displaysPtr->setEnabledFlag(i);
        }
        else
        {
            displaysPtr->clearEnabledFlag(i);
        }
    }

    // If the Memory Usage display is now disabled, then make sure memory is not allocated for the memory usage buffer
    if (!displaysPtr->anyHeapDisplayIsEnabled())
    {
        displaysPtr->getMemoryUsageDisplayPtr()->freeMemoryUsageBuffer();
    }

    // Certain things need to happen depending on whether the Hit Check Visualization display is now enabled/disabled
    // Reset the entry count to be safe
    HitCheckVisualizationDisplay *hitCheckVisualizationPtr = displaysPtr->getHitCheckVisualizationDisplayPtr();
    hitCheckVisualizationPtr->setEntryCount(0);

    if (!displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_HIT_CHECK_VISUALIZATION))
    {
        // The display is now disabled, so make sure memory is not allocated for it
        hitCheckVisualizationPtr->freeBuffer();
    }
    else
    {
        // The display is now enabled, so if both the hit and miss flags are disabled, then make sure memory is not
        // allocated for it

        // The memory for the buffer will be allocated automatically when it is needed, so just check for freeing the memory
        // for the buffer when both flags are disabled
        if (!displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_SHOULD_DRAW_HITS) &&
            !displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_SHOULD_DRAW_MISSES))
        {
            hitCheckVisualizationPtr->freeBuffer();
        }
    }

    // Get all of the manually position flags
    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
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
        bool flagIsSet;
        FLAG_IS_SET(manuallyPositionFlagsPtr, i, flagIsSet);

        if (flagIsSet)
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
    const uint32_t *hitsColorPtr = PTR_CAST_TYPE_ADD_OFFSET(const uint32_t *, this, offset);
    hitCheckVisualizationPtr->setHitsColor(*hitsColorPtr);
    offset += sizeof(uint32_t);

    const uint32_t *missesColorPtr = PTR_CAST_TYPE_ADD_OFFSET(const uint32_t *, this, offset);
    hitCheckVisualizationPtr->setMissesColor(*missesColorPtr);
}

uint32_t DisplaysSettingsData::setData(Displays *displaysPtr)
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

// This function returns 0 if there are no watches to write
uint32_t MemoryWatchesSettingsHeader::setData(SettingsHeader *settingsHeaderPtr, MemoryWatch *memoryWatchPtr, uint32_t offset)
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

uint32_t MemoryWatchesSettingsData::getData(uint32_t totalWatches, uint32_t watchNameLength) const
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
        const char *namePtr = PTR_CAST_TYPE_ADD_OFFSET(const char *, this, offset);
        copyStringAndNullTerminate(currentEntryPtr->getNamePtr(), nameLength, namePtr);
        offset += watchNameLength;

        // Make sure offset is aligned to 4 bytes, since the class is aligned to 4 bytes
        offset = roundUp(offset, sizeof(uint32_t));
    }

    return offset;
}

void MemoryWatchesSettingsData::getAddressOffsetsData(uint32_t offset) const
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

uint32_t MemoryWatchesSettingsData::setData(const MemoryWatchEntry *entryPtr)
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
uint32_t MemoryWatchesSettingsData::setAddressOffsetsData(const MemoryWatchEntry *entryPtr)
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

void MemoryEditorSettingsData::getData(uint32_t totalFlags) const
{
    // Get all of the enabled flags
    const uint32_t *enabledFlagsPtr = PTR_CAST_TYPE_ADD_OFFSET(const uint32_t *, this, 0);
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
        bool flagIsSet;
        FLAG_IS_SET(enabledFlagsPtr, i, flagIsSet);

        if (flagIsSet)
        {
            memoryEditorPtr->setEnabledFlag(i);
        }
        else
        {
            memoryEditorPtr->clearEnabledFlag(i);
        }
    }

    // Get the current address
    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
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

// This function returns 0 if there are no states to write
uint32_t CustomStatesSettingsHeader::setData(SettingsHeader *settingsHeaderPtr, CustomState *customStatePtr, uint32_t offset)
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

uint32_t CustomStateSettingsMarioData::getData(CustomStateMarioData *dataPtr, uint32_t totalStates) const
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

uint32_t CustomStatesSettingsData::getData(uint32_t totalStates, uint32_t stateNameLength) const
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
            const CustomStateSettingsMarioData *marioDataPtr =
                PTR_CAST_TYPE_ADD_OFFSET(const CustomStateSettingsMarioData *, this, offset);

            offset += marioDataPtr->getData(currentEntryPtr->getMarioDataPtr(), totalStates); // Some of Mario's data

            offset += sizeof(CustomStatesSettingsData::partyData);   // Pouch data
            offset += sizeof(uint16_t);                              // Total inventory items
            offset += sizeof(uint16_t);                              // Sequence position
            offset += sizeof(PartyMembers);                          // Partner out
            offset += sizeof(PartyMembers);                          // Follower out
            offset += sizeof(bool);                                  // In boat mode
            offset += sizeof(CustomStatesSettingsData::currentMap);  // Current map
            offset += sizeof(CustomStatesSettingsData::currentBero); // Current bero
            offset += stateNameLength;                               // State name

            // Make sure offset is aligned to 4 bytes, since the class is aligned to 4 bytes
            offset = roundUp(offset, sizeof(uint32_t));

            continue;
        }

        // Some of Mario's data
        const CustomStateSettingsMarioData *marioDataPtr =
            PTR_CAST_TYPE_ADD_OFFSET(const CustomStateSettingsMarioData *, this, offset);

        offset += marioDataPtr->getData(currentEntryPtr->getMarioDataPtr(), totalStates);

        // Pouch data
        constexpr uint32_t pouchDataSize = sizeof(CustomStatesSettingsData::partyData);
        const PouchData *pouchDataPtr = PTR_CAST_TYPE_ADD_OFFSET(const PouchData *, this, offset);
        memcpy(currentEntryPtr->getPouchPartyDataPtr(), pouchDataPtr, pouchDataSize);
        offset += pouchDataSize;

        // Total inventory items
        const uint16_t *totalInventoryItemsPtr = PTR_CAST_TYPE_ADD_OFFSET(const uint16_t *, this, offset);
        currentEntryPtr->setTotalInventoryItems(*totalInventoryItemsPtr);
        offset += sizeof(uint16_t);

        // Sequence position
        const uint16_t *sequencePositionPtr = PTR_CAST_TYPE_ADD_OFFSET(const uint16_t *, this, offset);
        currentEntryPtr->setStateSequencePosition(*sequencePositionPtr);
        offset += sizeof(uint16_t);

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
        currentEntryPtr->setInBoatMode(*inBoatModePtr);
        offset += sizeof(bool);

        // Current map
        constexpr uint32_t currentMapSize = sizeof(CustomStatesSettingsData::currentMap);
        const char *currentMapPtr = PTR_CAST_TYPE_ADD_OFFSET(const char *, this, offset);
        strncpy(currentEntryPtr->getCurrentMapPtr(), currentMapPtr, currentMapSize);
        offset += currentMapSize;

        // Current bero
        constexpr uint32_t currentBeroSize = sizeof(CustomStatesSettingsData::currentBero);
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

void CustomStatesSettingsData::getItemsData(uint32_t offset) const
{
    CustomState *customStatePtr = &gCustomState;
    CustomStateEntry *entriesPtr = customStatePtr->getEntriesPtr();
    const uint32_t totalEntries = customStatePtr->getTotalEntries();

    for (uint32_t i = 0; i < totalEntries; i++)
    {
        CustomStateEntry *currentEntryPtr = &entriesPtr[i];

        // Make sure there is at least one item to get
        uint32_t totalItems = currentEntryPtr->getTotalInventoryItems();
        if (totalItems == 0)
        {
            continue;
        }

        // Initialize the memory for the items
        totalItems = currentEntryPtr->initItemsPtr(totalItems);

        // Get all of the items
        const uint32_t itemsSize = totalItems * sizeof(ItemId);
        const ItemId *itemsPtr = PTR_CAST_TYPE_ADD_OFFSET(const ItemId *, this, offset);
        memcpy(currentEntryPtr->getItemsPtr(), itemsPtr, itemsSize);
        offset += itemsSize;
    }
}

uint32_t CustomStatesSettingsData::setData(CustomStateEntry *entryPtr)
{
    constexpr uint32_t marioDataSize = sizeof(CustomStateMarioData);
    memcpy(&this->marioData, entryPtr->getMarioDataPtr(), marioDataSize);

    constexpr uint32_t pouchDataSize = sizeof(CustomStatesSettingsData::partyData);
    memcpy(this->partyData, entryPtr->getPouchPartyDataPtr(), pouchDataSize);

    const uint32_t totalItems = entryPtr->getTotalInventoryItems();
    if ((totalItems > 0) && entryPtr->getItemsPtr())
    {
        this->totalInventoryItems = static_cast<uint16_t>(totalItems);
    }

    this->sequencePosition = entryPtr->getStateSequencePosition();
    this->partnerOut = entryPtr->getPartnerOut();
    this->followerOut = entryPtr->getFollowerOut();
    this->inBoatMode = entryPtr->shouldBeInBoatMode();

    constexpr uint32_t currentMapSize = sizeof(CustomStatesSettingsData::currentMap);
    strncpy(this->currentMap, entryPtr->getCurrentMapPtr(), currentMapSize);

    constexpr uint32_t currentBeroSize = sizeof(CustomStatesSettingsData::currentBero);
    strncpy(this->currentBero, entryPtr->getCurrentBeroPtr(), currentBeroSize);

    strncpy(this->stateName, entryPtr->getStateNamePtr(), CUSTOM_STATE_NAME_SIZE);

    return sizeof(CustomStatesSettingsData);
}

// This function returns 0 if there are no items to write
uint32_t CustomStatesSettingsData::setItemsData(CustomStateEntry *entryPtr)
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

int32_t finishAsyncMemoryCardFunc(int32_t ret, int32_t channel)
{
    if (ret == CARD_RESULT_READY)
    {
        // CARDGetResultCode will return CARD_RESULT_BUSY until the current async memory card operation is complete
        do
        {
            ret = CARDGetResultCode(channel);
        } while (ret == CARD_RESULT_BUSY);
    }
    return ret;
}

static int32_t checkForMemoryCard(int32_t channel)
{
    // Check if a memory card is inserted
    int32_t ret;

    // To prevent an infinite loop, limit the loop count to 1,000,000
    for (uint32_t i = 0; i < 1000000; i++)
    {
        ret = CARDProbeEx(channel, nullptr, nullptr);
        if (ret != CARD_RESULT_BUSY)
        {
            break;
        }
    }

    return ret;
}

int32_t mountMemoryCard(int32_t channel)
{
    // Make sure the memory card is already unmounted before trying to mount it, as otherwise problems may occur
    CARDUnmount(channel);

    // Check if a memory card is inserted
    int32_t ret = checkForMemoryCard(channel);
    if (ret != CARD_RESULT_READY)
    {
        // Error occured
        return ret;
    }

    // Mount the memory card
    ret = CARDMountAsync(channel, _cardMgrWorkPtr->workArea, nullptr, nullptr);
    ret = finishAsyncMemoryCardFunc(ret, channel);

    // Check if the memory card was successfully mounted, or if there are problems with it
    if ((ret == CARD_RESULT_READY) || (ret == CARD_RESULT_BROKEN))
    {
        // Attempt to repair any possible problems with the memory card
        ret = CARDCheckAsync(channel, nullptr);
        ret = finishAsyncMemoryCardFunc(ret, channel);
    }

    return ret;
}

int32_t loadSettings(int32_t channel)
{
    // Make sure the vanilla code isn't currently doing memory card stuff
    if (cardIsExec())
    {
        // vanilla code is currently doing memory card stuff
        return CARD_RESULT_CARD_IN_USE;
    }

    // Mount the memory card
    int32_t ret = mountMemoryCard(channel);
    if (ret != CARD_RESULT_READY)
    {
        // Error occured
        return ret;
    }

    // Open the settings file
    CARDFileInfo fileInfo;
    ret = CARDOpen(channel, gSettingsFileName, &fileInfo);
    if (ret != CARD_RESULT_READY)
    {
        // Error occured
        CARDUnmount(channel);
        return ret;
    }

    // Set up the array to hold the area of the file that contains the size
    char *fileData = new char[CARD_READ_SIZE];

    // Get the data from the area that holds the size
    ret = readFromFileOnMemoryCard(channel, &fileInfo, fileData, CARD_READ_SIZE, 0x2000);
    if (ret != CARD_RESULT_READY)
    {
        // Error occured
        delete[] fileData;
        CARDClose(&fileInfo);
        CARDUnmount(channel);
        return ret;
    }

    // Make sure the version number of the settings file is supported
    const SettingsHeader *settingsHeaderPtr = getSettingsHeaderPtr(fileData, 0);
    if (settingsHeaderPtr->getVersion() > SETTINGS_VERSION)
    {
        // Invalid settings version
        delete[] fileData;
        CARDClose(&fileInfo);
        CARDUnmount(channel);
        return CARD_RESULT_INVALID_SETTNGS_VERSION;
    }

    // Get the size of the file
    // Adjust the data size to be in multiples of CARD_READ_SIZE, rounding up
    const int32_t dataSizeAdjusted = roundUp(settingsHeaderPtr->getDataSize(), CARD_READ_SIZE);

    // Delete the data that holds the size, as it's not needed anymore
    delete[] fileData;

    // Set up the memory to be copied from the file
    // Allocate to the front of the heap to avoid possible fragmentation later
    fileData = new (true) char[dataSizeAdjusted];

    // Get the data from the file
    ret = readFromFileOnMemoryCard(channel, &fileInfo, fileData, dataSizeAdjusted, 0x2000);

    // Close and unmount the memory card, as it's no longer needed
    CARDClose(&fileInfo);
    CARDUnmount(channel);

    if (ret != CARD_RESULT_READY)
    {
        // Error occured
        delete[] fileData;
        return ret;
    }

    // Reinitialize settingsHeaderPtr
    settingsHeaderPtr = getSettingsHeaderPtr(fileData, 0);
    const uint32_t version = settingsHeaderPtr->getVersion();

    // Get the misc header data if it is included
    const uint32_t miscHeaderOffset = settingsHeaderPtr->getMiscHeaderOffset();
    if (miscHeaderOffset > 0)
    {
        // Get the misc data if it is included
        const MiscSettingsHeader *miscSettingsHeaderPtr = getMiscSettingsHeaderPtr(fileData, miscHeaderOffset);
        const uint32_t miscDataOffset = miscSettingsHeaderPtr->getDataOffset();

        if (miscDataOffset > 0)
        {
            const MiscSettingsData *miscSettingsDataPtr = getMiscSettingsDataPtr(fileData, miscDataOffset);
            miscSettingsDataPtr->getData(version, miscSettingsHeaderPtr->getTotalFlags());
        }
    }

    // Get the cheats header data if it is included
    const uint32_t cheatsHeaderOffset = settingsHeaderPtr->getCheatsHeaderOffset();
    if (cheatsHeaderOffset > 0)
    {
        // Get the cheats data if it is included
        const CheatsSettingsHeader *cheatsSettingsHeaderPtr = getCheatsSettingsHeaderPtr(fileData, cheatsHeaderOffset);
        const uint32_t cheatsDataOffset = cheatsSettingsHeaderPtr->getDataOffset();

        if (cheatsDataOffset > 0)
        {
            const CheatsSettingsData *cheatsSettingsDataPtr = getCheatsSettingsDataPtr(fileData, cheatsDataOffset);

            cheatsSettingsDataPtr->getData(cheatsSettingsHeaderPtr->getTotalCheats(),
                                           cheatsSettingsHeaderPtr->getTotalButtonCombos(),
                                           version);
        }
    }

    // Get the displays header data if it is included
    const uint32_t displaysHeaderOffset = settingsHeaderPtr->getDisplaysHeaderOffset();
    if (displaysHeaderOffset > 0)
    {
        // Get the displays data if it is included
        const DisplaysSettingsHeader *displaysSettingsHeaderPtr = getDisplaysSettingsHeaderPtr(fileData, displaysHeaderOffset);
        const uint32_t displaysDataOffset = displaysSettingsHeaderPtr->getDataOffset();

        if (displaysDataOffset > 0)
        {
            const DisplaysSettingsData *displaysSettingsDataPtr = getDisplaysSettingsDataPtr(fileData, displaysDataOffset);

            displaysSettingsDataPtr->getData(displaysSettingsHeaderPtr->getTotalDisplays(),
                                             displaysSettingsHeaderPtr->getTotalManuallyPositionEntries(),
                                             displaysSettingsHeaderPtr->getTotalButtonCombos());
        }
    }

    // Get the memory watches header data if it is included
    const uint32_t memoryWatchesHeaderOffset = settingsHeaderPtr->getMemoryWatchesHeaderOffset();
    if (memoryWatchesHeaderOffset > 0)
    {
        // Get the memory watches data if it is included
        const MemoryWatchesSettingsHeader *memoryWatchesSettingsHeaderPtr =
            getMemoryWatchesSettingsHeaderPtr(fileData, memoryWatchesHeaderOffset);

        const uint32_t memoryWatchesDataOffset = memoryWatchesSettingsHeaderPtr->getDataOffset();
        if (memoryWatchesDataOffset > 0)
        {
            const MemoryWatchesSettingsData *memoryWatchesSettingsDataPtr =
                getMemoryWatchesSettingsDataPtr(fileData, memoryWatchesDataOffset);

            // Get the main data for each watch
            const uint32_t addressOffsetsOffset =
                memoryWatchesSettingsDataPtr->getData(memoryWatchesSettingsHeaderPtr->getTotalWatches(),
                                                      memoryWatchesSettingsHeaderPtr->getWatchNameLength());

            // Get the data for each watch's address offsets
            memoryWatchesSettingsDataPtr->getAddressOffsetsData(addressOffsetsOffset);
        }
    }

    // Get the memory editor header data if it is included
    const uint32_t memoryEditorHeaderOffset = settingsHeaderPtr->getMemoryEditorHeaderOffset();
    if (memoryEditorHeaderOffset > 0)
    {
        // Get the memory watches data if it is included
        const MemoryEditorSettingsHeader *memoryEditorSettingsHeaderPtr =
            getMemoryEditorSettingsHeaderPtr(fileData, memoryEditorHeaderOffset);

        const uint32_t memoryEditorDataOffset = memoryEditorSettingsHeaderPtr->getDataOffset();
        if (memoryEditorDataOffset > 0)
        {
            const MemoryEditorSettingsData *memoryEditorSettingsDataPtr =
                getMemoryEditorSettingsDataPtr(fileData, memoryEditorDataOffset);

            memoryEditorSettingsDataPtr->getData(memoryEditorSettingsHeaderPtr->getTotalFlags());
        }
    }

    // Get the custom states header data if it is included
    const uint32_t customStatesHeaderOffset = settingsHeaderPtr->getCustomStatesHeaderOffset();
    if (customStatesHeaderOffset > 0)
    {
        // Get the custom states data if it is included
        const CustomStatesSettingsHeader *customStatesSettingsHeaderPtr =
            getCustomStatesSettingsHeaderPtr(fileData, customStatesHeaderOffset);

        const uint32_t customStatesDataOffset = customStatesSettingsHeaderPtr->getDataOffset();
        if (customStatesDataOffset > 0)
        {
            const CustomStatesSettingsData *customStatesSettingsDataPtr =
                getCustomStatesSettingsDataPtr(fileData, customStatesDataOffset);

            // Get the main data for each state
            const uint32_t itemsOffset =
                customStatesSettingsDataPtr->getData(customStatesSettingsHeaderPtr->getTotalStates(),
                                                     customStatesSettingsHeaderPtr->getStateNameLength());

            // Get the data for each state's items
            customStatesSettingsDataPtr->getItemsData(itemsOffset);
        }
    }

    // Done
    delete[] fileData;
    return CARD_RESULT_READY;
}

// This function keeps the current memory card file open, but closes it and unmounts the memory card if it fails to create the
// settings file
static int32_t createSettingsFile(int32_t channel, CARDFileInfo *settingsFileInfoPtr, uint32_t dataSizeAdjusted)
{
    int32_t ret;

    // If creating the file on slot B, unmount the current slot and mount slot A
    if (channel != CARD_SLOT_A)
    {
        ret = CARDUnmount(channel);
        if (ret != CARD_RESULT_READY)
        {
            // Error occured
            return ret;
        }

        ret = mountMemoryCard(CARD_SLOT_A);
        if (ret != CARD_RESULT_READY)
        {
            // Error occured
            return ret;
        }
    }

    // Get the banner and icon data from the Practice Codes gci
    CARDFileInfo fileInfo;
    ret = CARDOpen(CARD_SLOT_A, "rel", &fileInfo);
    if (ret != CARD_RESULT_READY)
    {
        // Error occured
        CARDUnmount(CARD_SLOT_A);
        return ret;
    }

    // Set up the memory for the banner and icon data to go in
    char *bannerIconData = new char[0x2000];

    // Get the banner and icon data
    ret = readFromFileOnMemoryCard(CARD_SLOT_A, &fileInfo, bannerIconData, 0x2000, 0);
    if (ret != CARD_RESULT_READY)
    {
        // Error occured
        delete[] bannerIconData;
        CARDClose(&fileInfo);
        CARDUnmount(CARD_SLOT_A);
        return ret;
    }

    // The Practice Codes gci is no longer needed, so close it
    ret = CARDClose(&fileInfo);
    if (ret != CARD_RESULT_READY)
    {
        // Error occured
        delete[] bannerIconData;
        CARDUnmount(CARD_SLOT_A);
        return ret;
    }

    // If creating the file on slot B, unmount the current slot and mount slot B
    if (channel != CARD_SLOT_A)
    {
        ret = CARDUnmount(CARD_SLOT_A);
        if (ret != CARD_RESULT_READY)
        {
            // Error occured
            return ret;
        }

        ret = mountMemoryCard(channel);
        if (ret != CARD_RESULT_READY)
        {
            // Error occured
            return ret;
        }
    }

    // Create the new file
    // The new file should be opened automatically via CARDCreateAsync
    ret = createFileOnMemoryCard(channel, gSettingsFileName, dataSizeAdjusted, settingsFileInfoPtr);
    if (ret != CARD_RESULT_READY)
    {
        // Error occured
        delete[] bannerIconData;
        CARDUnmount(channel);
        return ret;
    }

    // Get the stats for the new file
    const int32_t fileNum = settingsFileInfoPtr->fileNum;
    CARDStat stat;

    ret = CARDGetStatus(channel, fileNum, &stat);
    if (ret != CARD_RESULT_READY)
    {
        // Error occured
        delete[] bannerIconData;
        CARDClose(settingsFileInfoPtr);
        CARDUnmount(channel);
        return ret;
    }

    // Set the stats for the new file
    stat.bannerFormat = 2;
    stat.iconAddress = 0;
    stat.iconFormat = 2;
    stat.iconSpeed = 3;
    stat.commentAddress = 0x2000;

    // Set the stats for the new file
    ret = setMemoryCardFileStatus(channel, fileNum, &stat);
    if (ret != CARD_RESULT_READY)
    {
        // Error occured
        delete[] bannerIconData;
        CARDClose(settingsFileInfoPtr);
        CARDUnmount(channel);
        return ret;
    }

    // Write the banner and icon data to the new file
    ret = writeToFileOnMemoryCard(channel, settingsFileInfoPtr, bannerIconData, 0x2000, 0);

    // Done
    delete[] bannerIconData;

    if (ret != CARD_RESULT_READY)
    {
        // Error occured
        CARDClose(settingsFileInfoPtr);
        CARDUnmount(channel);
    }

    return ret;
}

static uint32_t getWrittenSettingsSize()
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

        for (uint32_t i = 0; i < totalCustomStates; i++)
        {
            customStatesSize += customStatesEntriesPtr[i].getTotalInventoryItems() * sizeof(ItemId);
        }

        // Make sure the size is aligned to 4 bytes, as a class will be after the items that is aligned to 4 bytes
        customStatesSize = roundUp(customStatesSize, sizeof(uint32_t));
    }

    return sizeof(PracticeCodesDataHeader) + sizeof(SettingsHeader) + sizeof(MiscSettingsHeader) + sizeof(MiscSettingsData) +
           sizeof(CheatsSettingsHeader) + sizeof(CheatsSettingsData) + sizeof(DisplaysSettingsHeader) +
           sizeof(DisplaysSettingsData) + sizeof(MemoryEditorSettingsHeader) + sizeof(MemoryEditorSettingsData) +
           memoryWatchesSize + customStatesSize;
}

int32_t saveSettings(int32_t channel)
{
    // Make sure the vanilla code isn't currently doing memory card stuff
    if (cardIsExec())
    {
        // vanilla code is currently doing memory card stuff
        return CARD_RESULT_CARD_IN_USE;
    }

    // Mount the memory card
    int32_t ret = mountMemoryCard(channel);
    if (ret != CARD_RESULT_READY)
    {
        // Error occured
        return ret;
    }

    // Get the size thats going to be written
    const uint32_t dataSize = getWrittenSettingsSize();

    // Adjust the data size to be in multiples of CARD_WRITE_SIZE, rounding up
    const uint32_t dataSizeAdjusted = roundUp(dataSize, CARD_WRITE_SIZE);

    // Open the settings file if it exists
    const char *settingsFileName = gSettingsFileName;
    CARDFileInfo fileInfo;

    ret = CARDOpen(channel, settingsFileName, &fileInfo);
    switch (ret)
    {
        case CARD_RESULT_READY:
        {
            // Set up the array to hold the area of the file that contains the size
            char *fileData = new char[CARD_READ_SIZE];

            // Get the data from the area that holds the size
            ret = readFromFileOnMemoryCard(channel, &fileInfo, fileData, CARD_READ_SIZE, 0x2000);
            if (ret != CARD_RESULT_READY)
            {
                // Error occured
                delete[] fileData;
                CARDClose(&fileInfo);
                CARDUnmount(channel);
                return ret;
            }

            // Get the size of the file
            // Adjust the file size to be in multiples of CARD_WRITE_SIZE, rounding up
            const SettingsHeader *settingsHeaderPtr = getSettingsHeaderPtr(fileData, 0);
            const uint32_t storedDataSizeAdjusted = roundUp(settingsHeaderPtr->getDataSize(), CARD_WRITE_SIZE);

            // Delete the data that holds the size, as it's not needed anymore
            delete[] fileData;

            // Make sure the size being written matches the current size
            if (dataSizeAdjusted != storedDataSizeAdjusted)
            {
                // The new size differs from the current size, so a new file must be created
                // Close the file
                ret = CARDClose(&fileInfo);
                if (ret != CARD_RESULT_READY)
                {
                    // Error occured
                    CARDUnmount(channel);
                    return ret;
                }

                // Delete the file
                ret = CARDDelete(channel, settingsFileName);
                if (ret != CARD_RESULT_READY)
                {
                    // Error occured
                    CARDUnmount(channel);
                    return ret;
                }

                // Make the new file
                // createSettingsFile keeps the file open, but closes and unmounts if it fails to create the file
                // Add 0x2000 to account for the banner and icon data
                ret = createSettingsFile(channel, &fileInfo, dataSizeAdjusted + 0x2000);
                if (ret != CARD_RESULT_READY)
                {
                    // Error occured
                    return ret;
                }
            }
            break;
        }
        case CARD_RESULT_NOFILE:
        {
            // Settings file does not exist, so create it
            // createSettingsFile keeps the file open, but closes and unmounts if it fails to create the file
            // Add 0x2000 to account for the banner and icon data
            ret = createSettingsFile(channel, &fileInfo, dataSizeAdjusted + 0x2000);
            if (ret != CARD_RESULT_READY)
            {
                // Error occured
                return ret;
            }
            break;
        }
        default:
        {
            // Unhandled error
            CARDUnmount(channel);
            return ret;
        }
    }

    // Set up the memory to be written to the file
    char *fileData = new char[dataSizeAdjusted];

    // Start writing the data
    // Write the title and description
    PracticeCodesDataHeader *practiceCodesDataHeaderPtr = getPracticeCodesDataHeaderPtr(fileData, 0);
    practiceCodesDataHeaderPtr->setData("Paper Mario", gSettingsDescription);

    // Write the intiial settings header data
    uint32_t offset = 0;
    SettingsHeader *settingsHeaderPtr = getSettingsHeaderPtr(fileData, offset);
    offset += settingsHeaderPtr->setData(SETTINGS_VERSION, dataSize);

    // Write the misc header data
    MiscSettingsHeader *miscSettingsHeaderPtr = getMiscSettingsHeaderPtr(fileData, offset);
    offset += miscSettingsHeaderPtr->setData(settingsHeaderPtr, offset);

    // Write the misc data
    MiscSettingsData *miscSettingsDataPtr = getMiscSettingsDataPtr(fileData, offset);
    offset += miscSettingsDataPtr->setData();

    // Write the cheats header data
    CheatsSettingsHeader *cheatsSettingsHeaderPtr = getCheatsSettingsHeaderPtr(fileData, offset);
    offset += cheatsSettingsHeaderPtr->setData(settingsHeaderPtr, offset);

    // Write the cheats data
    CheatsSettingsData *cheatsSettingsDataPtr = getCheatsSettingsDataPtr(fileData, offset);
    offset += cheatsSettingsDataPtr->setData(gCheats);

    // Write the displays header data
    DisplaysSettingsHeader *displaysSettingsHeaderPtr = getDisplaysSettingsHeaderPtr(fileData, offset);
    offset += displaysSettingsHeaderPtr->setData(settingsHeaderPtr, offset);

    // Write the displays data
    DisplaysSettingsData *displaysSettingsDataPtr = getDisplaysSettingsDataPtr(fileData, offset);
    offset += displaysSettingsDataPtr->setData(gDisplays);

    // Write the memory watches header data
    MemoryWatch *memoryWatchPtr = &gMemoryWatch;
    MemoryWatchesSettingsHeader *memoryWatchesSettingsHeaderPtr = getMemoryWatchesSettingsHeaderPtr(fileData, offset);
    const uint32_t memoryWatchesDataOffset = memoryWatchesSettingsHeaderPtr->setData(settingsHeaderPtr, memoryWatchPtr, offset);

    // Make sure there is at least one watch to write
    if (memoryWatchesDataOffset > 0)
    {
        offset += memoryWatchesDataOffset;

        // Write the main data for each watch
        const uint32_t totalEntries = memoryWatchPtr->getTotalEntries();
        const MemoryWatchEntry *entriesPtr = memoryWatchPtr->getEntriesPtr();
        MemoryWatchesSettingsData *memoryWatchesSettingsDataPtr = getMemoryWatchesSettingsDataPtr(fileData, offset);

        for (uint32_t i = 0; i < totalEntries; i++)
        {
            offset += memoryWatchesSettingsDataPtr->setData(&entriesPtr[i]);

            // Update the pointer to get the next entry
            memoryWatchesSettingsDataPtr = getMemoryWatchesSettingsDataPtr(fileData, offset);
        }

        // Write the data for each watch's address offsets
        for (uint32_t i = 0; i < totalEntries; i++)
        {
            offset += memoryWatchesSettingsDataPtr->setAddressOffsetsData(&entriesPtr[i]);

            // Update the pointer to get the next entry
            memoryWatchesSettingsDataPtr = getMemoryWatchesSettingsDataPtr(fileData, offset);
        }
    }

    // Write the memory editor header data
    MemoryEditorSettingsHeader *memoryEditorSettingsHeaderPtr = getMemoryEditorSettingsHeaderPtr(fileData, offset);
    offset += memoryEditorSettingsHeaderPtr->setData(settingsHeaderPtr, offset);

    // Write the memory editor data
    MemoryEditorSettingsData *memoryEditorSettingsDataPtr = getMemoryEditorSettingsDataPtr(fileData, offset);
    offset += memoryEditorSettingsDataPtr->setData(gMemoryEditor);

    // Write the custom states header data
    CustomState *customStatePtr = &gCustomState;
    CustomStatesSettingsHeader *customStatesSettingsHeaderPtr = getCustomStatesSettingsHeaderPtr(fileData, offset);
    const uint32_t customStatesDataOffset = customStatesSettingsHeaderPtr->setData(settingsHeaderPtr, customStatePtr, offset);

    // Make sure there is at least one state to write
    if (customStatesDataOffset > 0)
    {
        offset += customStatesDataOffset;

        // Write the main data for each state
        const uint32_t totalEntries = customStatePtr->getTotalEntries();
        CustomStateEntry *entriesPtr = customStatePtr->getEntriesPtr();
        CustomStatesSettingsData *customSattesSettingsDataPtr = getCustomStatesSettingsDataPtr(fileData, offset);

        for (uint32_t i = 0; i < totalEntries; i++)
        {
            offset += customSattesSettingsDataPtr->setData(&entriesPtr[i]);

            // Update the pointer to get the next entry
            customSattesSettingsDataPtr = getCustomStatesSettingsDataPtr(fileData, offset);
        }

        // Write the data for each state's items
        for (uint32_t i = 0; i < totalEntries; i++)
        {
            offset += customSattesSettingsDataPtr->setItemsData(&entriesPtr[i]);

            // Update the pointer to get the next entry
            customSattesSettingsDataPtr = getCustomStatesSettingsDataPtr(fileData, offset);
        }

        // Make sure the offset is aligned to 4 bytes, as the classes used here are aligned to 4 bytes
        offset = roundUp(offset, sizeof(uint32_t));
    }

    // Write the data to the file
    ret = writeToFileOnMemoryCard(channel, &fileInfo, fileData, dataSizeAdjusted, 0x2000);

    // Done
    delete[] fileData;
    CARDClose(&fileInfo);
    CARDUnmount(channel);
    return ret;
}

int32_t deleteSettingsFile(int32_t channel)
{
    // Make sure the vanilla code isn't currently doing memory card stuff
    if (cardIsExec())
    {
        // vanilla code is currently doing memory card stuff
        return CARD_RESULT_CARD_IN_USE;
    }

    // Mount the memory card
    int32_t ret = mountMemoryCard(channel);
    if (ret != CARD_RESULT_READY)
    {
        // Error occured
        return ret;
    }

    // Delete the settings file
    ret = CARDDelete(channel, gSettingsFileName);

    // Done
    CARDUnmount(channel);
    return ret;
}
