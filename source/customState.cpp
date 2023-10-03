#include "customState.h"
#include "cxx.h"
#include "misc/utils.h"
#include "ttyd/mario_pouch.h"
#include "ttyd/mario.h"
#include "ttyd/mario_motion.h"
#include "ttyd/seq_mapchange.h"
#include "ttyd/mario_party.h"

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cinttypes>

// The CustomState class is very small, so making it dynamic would waste a lot of memory
CustomState gCustomState;

void CustomStateEntry::init(const char *stateNamePtr)
{
    // Make sure the memory for the state is cleared before doing anything
    clearMemory(this, sizeof(CustomStateEntry));

    // Back up the inventory
    const PouchData *pouchPtr = pouchGetPtr();
    memcpy(this->items, pouchPtr->items, sizeof(PouchData::items));
    memcpy(this->keyItems, pouchPtr->keyItems, sizeof(PouchData::keyItems));
    memcpy(this->storedItems, pouchPtr->storedItems, sizeof(PouchData::storedItems));
    memcpy(this->badges, pouchPtr->badges, sizeof(PouchData::badges));
    memcpy(this->equippedBadges, pouchPtr->equippedBadges, sizeof(PouchData::equippedBadges));

    // Back up some of Mario's data
    CustomStateMarioData *marioData = &this->marioData;
    marioData->currentHP = pouchPtr->currentHp;
    marioData->maxHP = pouchPtr->maxHp;
    marioData->currentFP = pouchPtr->currentFp;
    marioData->maxFP = pouchPtr->maxFp;
    marioData->coins = pouchPtr->coins;
    marioData->currentSP = pouchPtr->currentSp;
    marioData->maxSP = pouchPtr->maxSp;
    marioData->rank = pouchPtr->rank;
    marioData->level = pouchPtr->level;
    marioData->starPowersObtained = pouchPtr->starPowersObtained;
    marioData->baseMaxHp = pouchPtr->baseMaxHp;
    marioData->baseMaxFp = pouchPtr->baseMaxFp;
    marioData->unallocatedBp = pouchPtr->unallocatedBp;
    marioData->totalBp = pouchPtr->totalBp;
    marioData->starPoints = pouchPtr->starPoints;

    // Back up the partner data
    // pouchPtr->partyData[0] is unused
    memcpy(this->partyData, &pouchPtr->partyData[1], sizeof(CustomStateEntry::partyData));

    // Back up the sequence position
    this->sequencePosition = static_cast<uint16_t>(getSequencePosition());

    // Back up which partner is currently out
    const PartyEntry *partnerPtr = getPartnerPtr();
    if (partnerPtr)
    {
        this->partnerOut = partnerPtr->currentMemberId;
    }

    // Back up which follower is currently out
    const PartyEntry *followerPtr = getFollowerPtr();
    if (followerPtr)
    {
        this->followerOut = followerPtr->currentMemberId;
    }

    // Back up if Mario is currently in Boat Mode
    if (marioGetPtr()->currentMotionId == MarioMotion::kShip)
    {
        this->inBoatMode = true;
    }

    // Back up the current map
    strncpy(this->currentMap, _next_map, sizeof(CustomStateEntry::currentMap));

    // Back up the current loading zone
    strncpy(this->currentBero, _next_bero, sizeof(CustomStateEntry::currentBero));

    // Set the name of the current state
    strncpy(this->stateName, stateNamePtr, sizeof(CustomStateEntry::stateName));
}

void CustomStateEntry::load()
{
    // Restore the inventory
    PouchData *pouchPtr = pouchGetPtr();
    memcpy(pouchPtr->items, this->items, sizeof(PouchData::items));
    memcpy(pouchPtr->keyItems, this->keyItems, sizeof(PouchData::keyItems));
    memcpy(pouchPtr->storedItems, this->storedItems, sizeof(PouchData::storedItems));
    memcpy(pouchPtr->badges, this->badges, sizeof(PouchData::badges));
    memcpy(pouchPtr->equippedBadges, this->equippedBadges, sizeof(PouchData::equippedBadges));

    // Restore up some of Mario's data
    const CustomStateMarioData *marioData = &this->marioData;
    pouchPtr->currentHp = marioData->currentHP;
    pouchPtr->maxHp = marioData->maxHP;
    pouchPtr->currentFp = marioData->currentFP;
    pouchPtr->maxFp = marioData->maxFP;
    pouchPtr->coins = marioData->coins;
    pouchPtr->currentSp = marioData->currentSP;
    pouchPtr->maxSp = marioData->maxSP;
    pouchPtr->rank = marioData->rank;
    pouchPtr->level = marioData->level;
    pouchPtr->starPowersObtained = marioData->starPowersObtained;
    pouchPtr->baseMaxHp = marioData->baseMaxHp;
    pouchPtr->baseMaxFp = marioData->baseMaxFp;
    pouchPtr->totalBp = marioData->totalBp;
    pouchPtr->unallocatedBp = marioData->unallocatedBp;
    pouchPtr->starPoints = marioData->starPoints;

    // Restore the partner data
    // pouchPtr->partyData[0] is unused
    memcpy(&pouchPtr->partyData[1], this->partyData, sizeof(CustomStateEntry::partyData));

    // Restore the sequence position
    setSequencePosition(this->sequencePosition);

    // Remove any partners and followers that are currently out from the overworld
    marioPartyKill();

    // Reset the previous partner and follower ids
    Player *playerPtr = marioGetPtr();
    playerPtr->prevPartnerId = PartyMembers::kNone;
    playerPtr->prevFollowerId = PartyMembers::kNone;

    // Restore which partner was out
    const PartyMembers partnerOut = this->partnerOut;
    if (partnerOut != PartyMembers::kNone)
    {
        spawnPartnerOrFollower(partnerOut);
    }

    // Restore which follower was out
    const PartyMembers followerOut = this->followerOut;
    if (followerOut != PartyMembers::kNone)
    {
        spawnPartnerOrFollower(followerOut);
    }

    // Properly update Mario and partner values
    recheckJumpAndHammerLevels();
    pouchReviseMarioParam();
    pouchRevisePartyParam();

    // Reset Mario's motion
    marioChgMot(MarioMotion::kStay);

    // Transform Mario into boat mode if he was in that form
    if (this->inBoatMode)
    {
        marioChgShipMotion();
    }
    else
    {
        marioChgStayMotion();
    }
}

void CustomStateEntry::setNewStateName(const char *newStateNamePtr)
{
    strncpy(this->stateName, newStateNamePtr, sizeof(CustomStateEntry::stateName));
}

bool CustomState::loadState(uint32_t index)
{
    // Make sure the index is valid
    const uint32_t totalEntries = this->totalEntries;
    if (index >= totalEntries)
    {
        return false;
    }

    // Make sure the entries pointer is set
    CustomStateEntry *entriesPtr = this->entries;
    if (!entriesPtr)
    {
        return false;
    }

    // Load the selected state
    CustomStateEntry *currentEntry = &entriesPtr[index];
    currentEntry->load();
    return true;
}

CustomStateEntry *CustomState::addState(const char *stateNamePtr)
{
    CustomStateEntry *entriesPtr = this->entries;
    if (!entriesPtr)
    {
        // No current entries, so make a new one
        constexpr uint32_t totalEntries = 1;
        this->totalEntries = static_cast<uint8_t>(totalEntries);

        entriesPtr = new CustomStateEntry[totalEntries];
        this->entries = entriesPtr;

        entriesPtr->init(stateNamePtr);
        return entriesPtr;
    }

    // Make sure the limit has not been reached
    const uint32_t totalEntries = this->totalEntries;
    if (totalEntries >= MAX_CUSTOM_STATES)
    {
        return nullptr;
    }

    // Add a new entry
    const uint32_t newTotalEntries = totalEntries + 1;
    this->totalEntries = static_cast<uint8_t>(newTotalEntries);

    // Make a backup of the current entries
    // Allocate to the front of the heap to avoid fragmentation
    CustomStateEntry *entriesBackupPtr = new (true) CustomStateEntry[totalEntries];
    constexpr uint32_t stateSize = sizeof(CustomStateEntry);
    memcpy(entriesBackupPtr, entriesPtr, stateSize * totalEntries);

    // Reallocate memory for the states and restore the current entries
    delete[] entriesPtr;
    entriesPtr = new CustomStateEntry[newTotalEntries];
    this->entries = entriesPtr;

    memcpy(entriesPtr, entriesBackupPtr, stateSize * totalEntries);
    delete[] entriesBackupPtr;

    // Initialize the new entry
    CustomStateEntry *currentEntry = &entriesPtr[totalEntries];
    currentEntry->init(stateNamePtr);
    return currentEntry;
}

bool CustomState::duplicateState(uint32_t index)
{
    // Make sure the limit has not been reached
    const uint32_t totalEntries = this->totalEntries;
    if (totalEntries >= MAX_CUSTOM_STATES)
    {
        return false;
    }

    // Make sure the index is valid
    if (index >= totalEntries)
    {
        return false;
    }

    // Make sure the entries pointer is set
    CustomStateEntry *entriesPtr = this->entries;
    if (!entriesPtr)
    {
        return false;
    }

    // Add a new entry
    const uint32_t newTotalEntries = totalEntries + 1;
    this->totalEntries = static_cast<uint8_t>(newTotalEntries);

    // Make a backup of the current entries
    // Allocate to the front of the heap to avoid fragmentation
    CustomStateEntry *entriesBackupPtr = new (true) CustomStateEntry[totalEntries];
    constexpr uint32_t stateSize = sizeof(CustomStateEntry);
    memcpy(entriesBackupPtr, entriesPtr, stateSize * totalEntries);

    // Reallocate memory for the states and restore the current entries
    delete[] entriesPtr;
    entriesPtr = new CustomStateEntry[newTotalEntries];
    this->entries = entriesPtr;

    memcpy(entriesPtr, entriesBackupPtr, stateSize * totalEntries);
    delete[] entriesBackupPtr;

    // Copy the selected state to the new state
    memcpy(&entriesPtr[totalEntries], &entriesPtr[index], stateSize);
    return true;
}

bool CustomState::swapStates(uint32_t stateOneIndex, uint32_t stateTwoIndex)
{
    // Make sure both indexes are valid
    const uint32_t totalEntries = this->totalEntries;
    if (stateOneIndex >= totalEntries)
    {
        return false;
    }

    if (stateTwoIndex >= totalEntries)
    {
        return false;
    }

    // Make sure the entries pointer is set
    CustomStateEntry *entriesPtr = this->entries;
    if (!entriesPtr)
    {
        return false;
    }

    // If both indexes are the same, then don't bother doing anything
    if (stateOneIndex == stateTwoIndex)
    {
        return true;
    }

    // Make a temporary backup for the first state
    CustomStateEntry *tempEntryPtr = new CustomStateEntry;

    CustomStateEntry *firstStatePtr = &entriesPtr[stateOneIndex];
    constexpr uint32_t stateSize = sizeof(CustomStateEntry);
    memcpy(tempEntryPtr, firstStatePtr, stateSize);

    // Move the second state to the first state
    CustomStateEntry *secondStatePtr = &entriesPtr[stateTwoIndex];
    memcpy(firstStatePtr, secondStatePtr, stateSize);

    // Move the first state to the second state
    memcpy(secondStatePtr, tempEntryPtr, stateSize);
    delete tempEntryPtr;
    return true;
}

bool CustomState::moveState(uint32_t destinationIndex, uint32_t sourceIndex)
{
    // Make sure both indexes are valid
    const uint32_t totalEntries = this->totalEntries;
    if (destinationIndex >= totalEntries)
    {
        return false;
    }

    if (sourceIndex >= totalEntries)
    {
        return false;
    }

    // Make sure the entries pointer is set
    CustomStateEntry *entriesPtr = this->entries;
    if (!entriesPtr)
    {
        return false;
    }

    // If both indexes are the same, then don't bother doing anything
    if (destinationIndex == sourceIndex)
    {
        return true;
    }

    // If the indexes are right next to each other, then just swap them
    const bool movingStateDown = destinationIndex > sourceIndex;
    int32_t indexDistance;

    if (movingStateDown)
    {
        indexDistance = destinationIndex - sourceIndex;
    }
    else
    {
        indexDistance = sourceIndex - destinationIndex;
    }

    if (indexDistance < 2)
    {
        return this->swapStates(destinationIndex, sourceIndex);
    }

    // Make a temporary backup for the source state
    CustomStateEntry *tempEntryPtr = new CustomStateEntry;
    constexpr uint32_t stateSize = sizeof(CustomStateEntry);

    CustomStateEntry *sourceStatePtr = &entriesPtr[sourceIndex];
    memcpy(tempEntryPtr, sourceStatePtr, stateSize);

    CustomStateEntry *destinationStatePtr = &entriesPtr[destinationIndex];

    if (movingStateDown)
    {
        // Move the states from the destination to the source up by one
        memmove(sourceStatePtr, &sourceStatePtr[1], stateSize * indexDistance);
    }
    else
    {
        // Move the states from the destination to the source down by one
        memmove(&destinationStatePtr[1], destinationStatePtr, stateSize * indexDistance);
    }

    // Move the source state to the destination state
    memcpy(destinationStatePtr, tempEntryPtr, stateSize);
    delete tempEntryPtr;
    return true;
}

bool CustomState::overwriteState(uint32_t index)
{
    // Make sure the index is valid
    const uint32_t totalEntries = this->totalEntries;
    if (index >= totalEntries)
    {
        return false;
    }

    // Make sure the entries pointer is set
    CustomStateEntry *entriesPtr = this->entries;
    if (!entriesPtr)
    {
        return false;
    }

    // Get the name of the selected state
    // The string will be cleared, so copy it to a temporary buffer
    // Use snprintf to make sure the buffer size is not exceeded, and that a null terminator is properly applied
    char nameBuf[CUSTOM_STATE_NAME_SIZE + 1];
    CustomStateEntry *currentStatePtr = &entriesPtr[index];
    snprintf(nameBuf, sizeof(nameBuf), currentStatePtr->getStateNamePtr());

    // Overwrite the selected state
    currentStatePtr->init(nameBuf);
    return true;
}

bool CustomState::renameState(const char *newStateNamePtr, uint32_t index)
{
    // Make sure the index is valid
    const uint32_t totalEntries = this->totalEntries;
    if (index >= totalEntries)
    {
        return false;
    }

    // Make sure there is at least one state to delete
    CustomStateEntry *entriesPtr = this->entries;
    if ((totalEntries == 0) || !entriesPtr)
    {
        return false;
    }

    // Rename the selected state
    CustomStateEntry *currentStatePtr = &entriesPtr[index];
    currentStatePtr->setNewStateName(newStateNamePtr);
    return true;
}

bool CustomState::deleteState(uint32_t index)
{
    // Make sure the index is valid
    const uint32_t totalEntries = this->totalEntries;
    if (index >= totalEntries)
    {
        return false;
    }

    // Make sure there is at least one state to delete
    CustomStateEntry *entriesPtr = this->entries;
    if ((totalEntries == 0) || !entriesPtr)
    {
        return false;
    }

    // Remove one entry
    const uint32_t newTotalEntries = totalEntries - 1;
    this->totalEntries = static_cast<uint8_t>(newTotalEntries);

    // If the total entries are now at 0, then free the memory for the entries
    if (newTotalEntries == 0)
    {
        delete[] entriesPtr;
        this->entries = nullptr;
        return true;
    }

    // Make a backup of the current entries
    // Allocate to the front of the heap to avoid fragmentation
    CustomStateEntry *entriesBackupPtr = new (true) CustomStateEntry[totalEntries];
    constexpr uint32_t stateSize = sizeof(CustomStateEntry);
    memcpy(entriesBackupPtr, entriesPtr, stateSize * totalEntries);

    // Reallocate memory for the states
    delete[] entriesPtr;
    entriesPtr = new CustomStateEntry[newTotalEntries];
    this->entries = entriesPtr;

    // Restore all of the states up to the current one
    if (index > 0)
    {
        memcpy(entriesPtr, entriesBackupPtr, stateSize * index);
    }

    // Restore the rest of the states if there are any after the current one
    if (index < newTotalEntries)
    {
        memcpy(&entriesPtr[index], &entriesBackupPtr[index + 1], stateSize * (totalEntries - index - 1));
    }

    delete[] entriesBackupPtr;
    return true;
}
