#ifndef CUSTOM_STATE_H
#define CUSTOM_STATE_H

#include "ttyd/item_data.h"
#include "ttyd/mario_pouch.h"
#include "ttyd/party.h"

#include <cstdint>

#define MAX_CUSTOM_STATES 25
#define CUSTOM_STATE_NAME_SIZE 16

struct CustomStateMarioData
{
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

class CustomStateEntry
{
   public:
    CustomStateEntry() {}
    ~CustomStateEntry() {}

    void init(const char *stateName);
    void load();

    const char *getCurrentMapPtr() const { return this->currentMap; }
    uint32_t getCurrentMapSize() const { return sizeof(this->currentMap); }

    const char *getCurrentBeroPtr() const { return this->currentBero; }
    uint32_t getCurrentBeroSize() const { return sizeof(this->currentBero); }

    char *getStateNamePtr() { return this->stateName; }
    void setNewStateName(const char *newStateNamePtr);

   private:
    ItemId items[sizeof(PouchData::items) / sizeof(ItemId)];
    ItemId keyItems[sizeof(PouchData::keyItems) / sizeof(ItemId)];
    ItemId storedItems[sizeof(PouchData::storedItems) / sizeof(ItemId)];
    ItemId badges[sizeof(PouchData::badges) / sizeof(ItemId)];
    ItemId equippedBadges[sizeof(PouchData::equippedBadges) / sizeof(ItemId)];
    uint16_t sequencePosition;
    CustomStateMarioData marioData;
    PouchPartyData partyData[7];
    PartyMembers partnerOut;
    PartyMembers followerOut;
    bool inBoatMode;                        // Mario should be in boat mode upon entering the room
    char currentMap[8];                     // Does not include a null terminator
    char currentBero[16];                   // Does not include a null terminator
    char stateName[CUSTOM_STATE_NAME_SIZE]; // Does not include a null terminator
    uint8_t padding;
} __attribute__((__packed__));

static_assert(sizeof(CustomStateMarioData) == 0x1E);
static_assert(sizeof(CustomStateEntry) == 0x528);

class CustomState
{
   public:
    CustomState() {}
    ~CustomState() {}

    bool loadState(uint32_t index);
    CustomStateEntry *addState(const char *stateNamePtr);
    bool duplicateState(uint32_t index);
    bool swapStates(uint32_t stateOneIndex, uint32_t stateTwoIndex);
    bool moveState(uint32_t destinationIndex, uint32_t sourceIndex);
    bool overwriteState(uint32_t index);
    bool renameState(const char *newStateName, uint32_t index);
    bool deleteState(uint32_t index);

    CustomStateEntry *getEntriesPtr() const { return this->entries; }

    uint32_t getTotalEntries() const { return this->totalEntries; }
    bool limitHasBeenReached() const { return this->totalEntries >= MAX_CUSTOM_STATES; }

    uint32_t getSelectedState() const { return this->selectedState; }
    void setSelectedState(uint32_t state) { this->selectedState = static_cast<uint8_t>(state); }

    bool stateWasSelected() const { return this->init; }
    void setInit() { this->init = true; }
    void clearInit() { this->init = false; }

   private:
    CustomStateEntry *entries; // Amount is determined by totalEntries
    uint8_t totalEntries;
    uint8_t selectedState; // The state that was selected to be loaded
    bool init;             // A custom state was selected to be loaded
};

extern CustomState gCustomState;

#endif
