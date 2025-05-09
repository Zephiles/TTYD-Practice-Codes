#include "customState.h"
#include "cxx.h"
#include "mod.h"
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
    // Free the memory used by the items
    this->freeItems();

    // Make sure the memory for the state is cleared before doing anything
    clearMemory(this, sizeof(CustomStateEntry));

    // Back up the inventory
    this->backupInventory();

    // Back up some of Mario's data
    const PouchData *pouchPtr = pouchGetPtr();
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

void CustomStateEntry::backupInventory()
{
    // Allocate memory for the items
    constexpr uint32_t maxStandardItems = sizeof(PouchData::items) / sizeof(ItemId);
    constexpr uint32_t maxImportantItems = sizeof(PouchData::keyItems) / sizeof(ItemId);
    constexpr uint32_t maxStoredItems = sizeof(PouchData::storedItems) / sizeof(ItemId);
    constexpr uint32_t maxBadges = sizeof(PouchData::badges) / sizeof(ItemId);
    constexpr uint32_t maxEquippedBadges = sizeof(PouchData::equippedBadges) / sizeof(ItemId);

    // Allocate to the front of the heap to avoid fragmentation
    constexpr uint32_t maxItems = maxStandardItems + maxImportantItems + maxStoredItems + maxBadges + maxEquippedBadges;
    ItemId *itemsBufferPtr = new (true) ItemId[maxItems];

    const PouchData *pouchPtr = pouchGetPtr();
    uint32_t totalItems = 0;

    // Backup the standard items
    const ItemId *standardItemsPtr = pouchPtr->items;
    for (uint32_t i = 0; i < maxStandardItems; i++)
    {
        const ItemId currentItem = standardItemsPtr[i];
        itemsBufferPtr[totalItems++] = currentItem;

        if (currentItem == ItemId::ITEM_NONE)
        {
            break;
        }
    }

    // Backup the important items
    const ItemId *importantItemsPtr = pouchPtr->keyItems;
    for (uint32_t i = 0; i < maxImportantItems; i++)
    {
        const ItemId currentItem = importantItemsPtr[i];
        itemsBufferPtr[totalItems++] = currentItem;

        if (currentItem == ItemId::ITEM_NONE)
        {
            break;
        }
    }

    // Backup the stored items
    const ItemId *storedItemsPtr = pouchPtr->storedItems;
    for (uint32_t i = 0; i < maxStoredItems; i++)
    {
        const ItemId currentItem = storedItemsPtr[i];
        itemsBufferPtr[totalItems++] = currentItem;

        if (currentItem == ItemId::ITEM_NONE)
        {
            break;
        }
    }

    // Backup the badges
    const ItemId *badgesPtr = pouchPtr->badges;
    uint32_t totalBadges = 0;

    for (; totalBadges < maxBadges; totalBadges++)
    {
        const ItemId currentItem = badgesPtr[totalBadges];
        itemsBufferPtr[totalItems++] = currentItem;

        if (currentItem == ItemId::ITEM_NONE)
        {
            break;
        }
    }

    // Backup the equipped badges

    // When a badge is not equipped, the entry for it will be ItemId::ITEM_NONE, so backup all equipped badges up to and
    // including totalBadges
    ItemId *equippedBadgesBufferPtr = &itemsBufferPtr[totalItems];

    if (totalBadges > 0)
    {
        memcpy(equippedBadgesBufferPtr, pouchPtr->equippedBadges, totalBadges * sizeof(ItemId));
        totalItems += totalBadges;
    }
    else
    {
        // Set the first entry to ItemId::ITEM_NONE to make sure the game sees that there are no equipped badges
        equippedBadgesBufferPtr[totalItems++] = ItemId::ITEM_NONE;
    }

    // Make sure there is at least one item to backup, excluding ItemId::ITEM_NONE entries
    bool atLeastOneItem = false;
    for (uint32_t i = 0; i < totalItems; i++)
    {
        if (itemsBufferPtr[i] != ItemId::ITEM_NONE)
        {
            atLeastOneItem = true;
            break;
        }
    }

    // Make sure memory isn't already allocated for the items
    this->freeItems();
    this->totalInventoryItems = 0;

    if (atLeastOneItem)
    {
        // Set the new total
        this->totalInventoryItems = static_cast<uint16_t>(totalItems);

        // Set up the minimum amount of memory needed to hold all of the items
        ItemId *itemsPtr = new ItemId[totalItems];
        this->items = itemsPtr;

        // Copy the items from the buffer to the main memory, and then free the buffer
        memcpy(itemsPtr, itemsBufferPtr, totalItems * sizeof(ItemId));
    }

    delete[] itemsBufferPtr;
}

void CustomStateEntry::load()
{
    // Restore the inventory
    this->restoreInventory();

    // Restore up some of Mario's data
    PouchData *pouchPtr = pouchGetPtr();
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

    // Make sure Mario's and the partners' stats are properly updated when entering a battle
    Mod *modPtr = gMod;
    modPtr->setFlag(ModFlag::MOD_FLAG_CLEAR_MARIO_STATS);
    modPtr->setFlag(ModFlag::MOD_FLAG_CLEAR_PARTNER_STATS);
}

void CustomStateEntry::restoreInventory()
{
    // Clear the entire inventory before doing anything
    constexpr uint32_t totalSize = sizeof(PouchData::keyItems) + sizeof(PouchData::items) + sizeof(PouchData::storedItems) +
                                   sizeof(PouchData::badges) + sizeof(PouchData::equippedBadges);

    PouchData *pouchPtr = pouchGetPtr();
    clearMemory(pouchPtr->keyItems, totalSize); // Clear all 5 fields at once

    // Make sure there is at least one item to restore
    const uint32_t totalInventoryItems = this->totalInventoryItems;
    const ItemId *itemsPtr = this->items;
    if ((totalInventoryItems == 0) || !itemsPtr)
    {
        return;
    }

    uint32_t totalItems = 0;

    // Make sure there is at least one standard item to restore
    if ((itemsPtr[totalItems] != ItemId::ITEM_NONE) && (totalItems < totalInventoryItems))
    {
        // Restore the standard items
        ItemId *standardItemsPtr = pouchPtr->items;
        constexpr uint32_t maxStandardItems = sizeof(PouchData::items) / sizeof(ItemId);

        for (uint32_t i = 0; (i < maxStandardItems) && (totalItems < totalInventoryItems); i++)
        {
            const ItemId currentItem = itemsPtr[totalItems++];
            standardItemsPtr[i] = currentItem;

            if (currentItem == ItemId::ITEM_NONE)
            {
                break;
            }
        }
    }
    else
    {
        // Advance to the next section
        totalItems++;
    }

    // Make sure there is at least one important item to restore
    if ((itemsPtr[totalItems] != ItemId::ITEM_NONE) && (totalItems < totalInventoryItems))
    {
        // Restore the important items
        ItemId *importantItemsPtr = pouchPtr->keyItems;
        constexpr uint32_t maxImportantItems = sizeof(PouchData::keyItems) / sizeof(ItemId);

        for (uint32_t i = 0; (i < maxImportantItems) && (totalItems < totalInventoryItems); i++)
        {
            const ItemId currentItem = itemsPtr[totalItems++];
            importantItemsPtr[i] = currentItem;

            if (currentItem == ItemId::ITEM_NONE)
            {
                break;
            }
        }
    }
    else
    {
        // Advance to the next section
        totalItems++;
    }

    // Make sure there is at least one stored item to restore
    if ((itemsPtr[totalItems] != ItemId::ITEM_NONE) && (totalItems < totalInventoryItems))
    {
        // Restore the stored items
        ItemId *storedItemsPtr = pouchPtr->storedItems;
        constexpr uint32_t maxStoredItems = sizeof(PouchData::storedItems) / sizeof(ItemId);

        for (uint32_t i = 0; (i < maxStoredItems) && (totalItems < totalInventoryItems); i++)
        {
            const ItemId currentItem = itemsPtr[totalItems++];
            storedItemsPtr[i] = currentItem;

            if (currentItem == ItemId::ITEM_NONE)
            {
                break;
            }
        }
    }
    else
    {
        // Advance to the next section
        totalItems++;
    }

    // Make sure there is at least one badge to restore
    uint32_t totalBadges = 0;

    if ((itemsPtr[totalItems] != ItemId::ITEM_NONE) && (totalItems < totalInventoryItems))
    {
        // Restore the badges
        ItemId *badgesPtr = pouchPtr->badges;
        constexpr uint32_t maxBadges = sizeof(PouchData::badges) / sizeof(ItemId);

        for (; (totalBadges < maxBadges) && (totalItems < totalInventoryItems); totalBadges++)
        {
            const ItemId currentItem = itemsPtr[totalItems++];
            badgesPtr[totalBadges] = currentItem;

            if (currentItem == ItemId::ITEM_NONE)
            {
                break;
            }
        }
    }
    else
    {
        // Advance to the next section
        totalItems++;
    }

    // Restore the equipped badges
    if ((totalBadges > 0) && (totalItems < totalInventoryItems))
    {
        // Failsafe: Make sure totalBadges is valid
        if ((totalItems + totalBadges) > totalInventoryItems)
        {
            totalBadges = totalInventoryItems - totalItems;
        }

        // When a badge is not equipped, the entry for it will be ItemId::ITEM_NONE, so all entries up to and including
        // totalBadges must be written
        memcpy(pouchPtr->equippedBadges, &itemsPtr[totalItems], totalBadges * sizeof(ItemId));
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
    CustomStateEntry *newEntryPtr = &entriesPtr[totalEntries];
    memcpy(newEntryPtr, &entriesPtr[index], stateSize);

    // The new state's items pointer is currently set to the original's, so set up new memory for it if it has at least one
    // item
    const uint32_t totalItems = newEntryPtr->getTotalInventoryItems();
    const ItemId *itemsPtr = newEntryPtr->getItemsPtr();

    if ((totalItems > 0) && itemsPtr)
    {
        ItemId *newItems = new ItemId[totalItems];
        newEntryPtr->setItemsPtr(newItems);
        memcpy(newItems, itemsPtr, totalItems * sizeof(ItemId));
    }

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

    // Free the memory used by the items of the selected state
    entriesPtr[index].freeItems();

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
