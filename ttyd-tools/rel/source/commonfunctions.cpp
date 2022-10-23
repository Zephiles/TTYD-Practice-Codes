#include "commonfunctions.h"

#include <gc/DEMOPad.h>
#include <gc/OSModule.h>
#include <gc/OSCache.h>
#include <ttyd/system.h>
#include <ttyd/seqdrv.h>
#include <ttyd/mariost.h>
#include <ttyd/battle.h>
#include <ttyd/npcdrv.h>
#include <ttyd/swdrv.h>
#include <ttyd/seq_mapchange.h>
#include <ttyd/party.h>
#include <ttyd/mario_party.h>
#include <ttyd/mario_pouch.h>
#include <ttyd/mario.h>

#include <cstring>

namespace mod {

bool checkButtonCombo(uint32_t combo)
{
    uint32_t ButtonInput = ttyd::system::keyGetButton(0);
    if ((ButtonInput & combo) != combo)
    {
        return false;
    }
    
    uint32_t ButtonInputTrg = ttyd::system::keyGetButtonTrg(0);
    return ButtonInputTrg & combo;
}

bool checkButtonComboEveryFrame(uint32_t combo)
{
    uint32_t ButtonInput = ttyd::system::keyGetButton(0);
    return (ButtonInput & combo) == combo;
}

bool checkButtonComboDemo(uint32_t combo)
{
    gc::DEMOPad::DEMOPadStatus *demoPadPtr = &gc::DEMOPad::DemoPad[0];
    uint32_t ButtonInput = demoPadPtr->buttons;
    
    if ((ButtonInput & combo) != combo)
    {
        return false;
    }
    
    uint32_t ButtonInputTrg = demoPadPtr->buttonsDown;
    return ButtonInputTrg & combo;
}

bool checkForSpecificSeq(ttyd::seqdrv::SeqIndex wantedSeq)
{
    ttyd::seqdrv::SeqIndex NextSeq = ttyd::seqdrv::seqGetNextSeq();
    if (NextSeq != wantedSeq)
    {
        return false;
    }
    
    ttyd::seqdrv::SeqIndex CurrentSeq = ttyd::seqdrv::seqGetSeq();
    return CurrentSeq == wantedSeq;
}

bool checkIfInGame()
{
    ttyd::seqdrv::SeqIndex NextSeq = ttyd::seqdrv::seqGetNextSeq();
    ttyd::seqdrv::SeqIndex Game = ttyd::seqdrv::SeqIndex::kGame;
    
    if (NextSeq != Game)
    {
        return false;
    }
    
    ttyd::seqdrv::SeqIndex CurrentSeq = ttyd::seqdrv::seqGetSeq();
    if (CurrentSeq != Game)
    {
        return false;
    }
    
    gc::OSModule::OSModuleInfo *CurrentRelPtr = getCurrentRelPtr();
    if (!CurrentRelPtr)
    {
        return false;
    }
    
    return CurrentRelPtr->id != ttyd::seq_mapchange::REL_ID::DMO;
}

gc::OSModule::OSModuleInfo *getCurrentRelPtr()
{
    return *reinterpret_cast<gc::OSModule::OSModuleInfo **>(
        reinterpret_cast<uint32_t>(
            ttyd::mariost::globalWorkPointer) + 0x15C);
}

void *getBattleWorkPointer()
{
    // Make sure the battle byte is set, as otherwise not actually in a battle
    uint32_t InBattleByte = *reinterpret_cast<uint32_t *>(
        reinterpret_cast<uint32_t>(ttyd::mariost::globalWorkPointer) + 0x14);
    
    if (InBattleByte == 0)
    {
        return nullptr;
    }
    
    return ttyd::battle::battleWorkPointer;
}

void *getMarioBattlePointer()
{
    void *BattlePointer = getBattleWorkPointer();
    if (!BattlePointer)
    {
        return nullptr;
    }
    
    return ttyd::battle::BattleGetMarioPtr(BattlePointer);
}

void *getPartnerBattlePointer()
{
    void *MarioBattlePointer = getMarioBattlePointer();
    if (!MarioBattlePointer)
    {
        return nullptr;
    }
    
    void *BattlePointer = getBattleWorkPointer();
    return ttyd::battle::BattleGetPartnerPtr(BattlePointer, MarioBattlePointer);
}

void *getActorPointer(uint32_t slot)
{
    void *BattlePointer = getBattleWorkPointer();
    if (!BattlePointer)
    {
        return nullptr;
    }
    
    return ttyd::battle::BattleGetUnitPtr(BattlePointer, slot);
}

ttyd::npcdrv::NpcEntry *getNpcEntryData(uint32_t slot, bool getBattleData)
{
    uint32_t Index = static_cast<uint32_t>(getBattleData);
    ttyd::npcdrv::NpcWork *NpcWorkPointer = &ttyd::npcdrv::npcWork[Index];
    return &NpcWorkPointer->entries[slot];
}

uint32_t secondsToFrames(uint32_t seconds)
{
    return ttyd::system::sysMsec2Frame(seconds * 1000);
}

uint32_t getSequencePosition()
{
    return ttyd::swdrv::swByteGet(0);
}

bool checkIfSystemLevelIsSet()
{
    uint32_t SystemLevelFlags = ttyd::mariost::marioStGetSystemLevel();
    int32_t SystemLevel = ttyd::mariost::_mariostSystemLevel;
    
    if ((SystemLevelFlags == 0) && (SystemLevel == 0))
    {
        return false;
    }
    
    return true;
}

void setSequencePosition(uint32_t value)
{
    ttyd::swdrv::swByteSet(0, value);
}

void setNextMap(const char *map)
{
    char tempMap[9];
    strncpy(tempMap, map, sizeof(tempMap) - 1);
    tempMap[sizeof(tempMap) - 1] = '\0';
    
    strcpy(ttyd::seq_mapchange::NextMap, tempMap);
    strncpy(ttyd::seq_mapchange::NextArea, tempMap, 3);
}

void setNextBero(const char *bero)
{
    char tempBero[17];
    strncpy(tempBero, bero, sizeof(tempBero) - 1);
    tempBero[sizeof(tempBero) - 1] = '\0';
    
    strcpy(ttyd::seq_mapchange::NextBero, tempBero);
}

bool compareStrings(const char *str1, const char *str2)
{
    return strcmp(str1, str2) == 0;
}

bool compareStringsSize(const char *str1, const char *str2, size_t size)
{
    return strncmp(str1, str2, size) == 0;
}

bool compareStringToNextMap(const char *str)
{
    return compareStrings(str, ttyd::seq_mapchange::NextMap);
}

void setSeqMapChange(const char *map, const char *bero)
{
    ttyd::seqdrv::seqSetSeq(ttyd::seqdrv::SeqIndex::kMapChange, map, bero);
}

void *clearMemory(void *destination, std::size_t size)
{
    return memset(destination, 0, size);
}

ttyd::memory::SmartAllocationData *allocFromSmartHeap(uint32_t size, uint32_t group)
{
    // Allocate the memory
    ttyd::memory::SmartAllocationData *SmartData = ttyd::memory::smartAlloc(size, group);
    
    // Set up a temporary local variable to use for getting the memory
    void *Ptr = SmartData->pMemory;
    
    // Clear the memory, as this is not done automatically
    Ptr = clearMemory(Ptr, size);
    
    // Be 100% certain that no memory issues occur
    gc::OSCache::DCFlushRange(Ptr, size);
    return SmartData;
}

void *getPartnerPointer()
{
    ttyd::party::PartySlotId Id = ttyd::mario_party::marioGetPartyId();
    return ttyd::party::partyGetPtr(Id);
}

void *getFollowerPointer()
{
    ttyd::party::PartySlotId Id = ttyd::mario_party::marioGetExtraPartyId();
    return ttyd::party::partyGetPtr(Id);
}

void removePartnerFromOverworld()
{
    ttyd::party::PartySlotId Id = ttyd::mario_party::marioGetPartyId();
    ttyd::party::partyKill2(Id);
}

void removeFollowerFromOverworld()
{
    ttyd::party::PartySlotId Id = ttyd::mario_party::marioGetExtraPartyId();
    ttyd::party::partyKill2(Id);
}

uint16_t getSpecialMoveBits()
{
    uint32_t PouchPtr = reinterpret_cast<uint32_t>(ttyd::mario_pouch::pouchGetPtr());
    return *reinterpret_cast<uint16_t *>(PouchPtr + 0x8C);
}

uint32_t getCurrentYoshiColorId()
{
    const uint32_t YoshiPartnerId = 4;
    return ttyd::mario_pouch::pouchGetPartyColor(YoshiPartnerId);
}

void setNewYoshiColorId(uint32_t colorId)
{
    // Make sure the desired color is valid
    const uint32_t ColorIdWhite = 6;
    if (colorId > ColorIdWhite)
    {
        // The desired color is not valid
        return;
    }
    
    const uint32_t YoshiPartnerId = 4;
    ttyd::mario_pouch::pouchSetPartyColor(YoshiPartnerId, colorId);
}

void spawnPartnerOrFollower(ttyd::party::PartyMembers id)
{
    // Make sure the partner/follower is valid
    if ((id <= ttyd::party::PartyMembers::kNone) || 
        (id >= ttyd::party::PartyMembers::kMsMowzFollower))
    {
        return;
    }
    
    ttyd::party::PartySlotId PartySlotId;
    
    // If spawning a partner, make sure they are enabled when marioPartyEntry is called
    if ((id >= ttyd::party::PartyMembers::kGoombella) && 
        (id <= ttyd::party::PartyMembers::kMsMowz))
    {
        uint32_t PouchPtr = reinterpret_cast<uint32_t>(ttyd::mario_pouch::pouchGetPtr());
        
        bool *PartnerEnabled = reinterpret_cast<bool *>(
            PouchPtr + (static_cast<uint32_t>(id) * 0xE) + 0x1);
        
        // Preserve the current value of the enabled bool
        bool CurrentPartnerEnabled = *PartnerEnabled;
        
        // Make sure the selected partner is enabled
        *PartnerEnabled = true;
        
        // Spawn the partner
        PartySlotId = ttyd::mario_party::marioPartyEntry(id);
        
        // Restore the value of the enabled bool
        *PartnerEnabled = CurrentPartnerEnabled;
    }
    else
    {
        // Spawn the follower
        PartySlotId = ttyd::mario_party::marioPartyEntry(id);
    }
    
    // Make sure the partner/follower spawned properly
    if (PartySlotId <= ttyd::party::PartySlotId::kNone)
    {
        return;
    }
    
    // Make sure the partner/follower is moving
    ttyd::party::partyRun(ttyd::party::partyGetPtr(PartySlotId));
}

void spawnFailsafePartnerOrFollower(bool shouldSpawnPartner)
{
    ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
    ttyd::party::PartyMembers PreviousOut;
    
    if (shouldSpawnPartner)
    {
        // Get the previous partner out
        PreviousOut = player->prevPartyId[0];
    }
    else
    {
        // Get the previous follower out
        PreviousOut = player->prevPartyId[1];
    }

    // Check if a partner/follower was previously out
    if (PreviousOut != ttyd::party::PartyMembers::kNone)
    {
        // A partner/follower was previously out, so bring them back out
        spawnPartnerOrFollower(PreviousOut);
    }
    else if (shouldSpawnPartner)
    {
        // No partner was previously out, so bring out Goombella
        spawnPartnerOrFollower(ttyd::party::PartyMembers::kGoombella);
    }
    else
    {
        // No follower was previously out, so bring out Frankly
        spawnPartnerOrFollower(ttyd::party::PartyMembers::kFrankly);
    }
}

bool checkIfBadgeEquipped(int16_t badge)
{
    return ttyd::mario_pouch::pouchEquipCheckBadge(badge) > 0;
}

bool checkIfHaveItem(int16_t item)
{
    return ttyd::mario_pouch::pouchCheckItem(item) > 0;
}

void recheckJumpAndHammerLevels()
{
    ttyd::mario_pouch::pouchGetJumpLv();
    ttyd::mario_pouch::pouchGetHammerLv();
}

uint32_t getCurrentPitFloor()
{
    return ttyd::swdrv::swByteGet(1321) + 1; // GSW(1321)
}

void setPitFloor(int32_t floor) // floor is 1-100
{
    // Make sure the desired floor is valid
    if ((floor >= 1) && (floor <= 100))
    {
        ttyd::swdrv::swByteSet(1321, static_cast<uint32_t>(floor - 1)); // GSW(1321)
    }
}

uint32_t getCurrentFPS()
{
    return *reinterpret_cast<uint32_t *>(
        reinterpret_cast<uint32_t>(
            ttyd::mariost::globalWorkPointer) + 0x4);
}

void clearGSWFsRange(uint32_t lowerBound, uint32_t upperBound)
{
    for (uint32_t i = lowerBound; i <= upperBound; i++)
    {
        ttyd::swdrv::swClear(i);
    }
}

float __attribute__ ((noinline)) intToFloat(int32_t value)
{
    return static_cast<float>(value);
}

void intToFloatArray(int32_t *values, float *valuesOut, int32_t numValues)
{
    for (int32_t i = 0; i < numValues; i++)
    {
        valuesOut[i] = intToFloat(values[i]);
    }
}

uint32_t checkIfPointerIsValid(const void *ptr)
{
    uint32_t ptrRaw = reinterpret_cast<uint32_t>(ptr);
    
    // Cached memory
    if ((ptrRaw >= 0x80000000) && (ptrRaw < 0x81800000))
    {
        return PTR_CACHED;
    }
    
    // Uncached memory
    if ((ptrRaw >= 0xC0000000) && (ptrRaw < 0xC1800000))
    {
        return PTR_UNCACHED;
    }
    
    return PTR_INVALID;
}

void *getLastPointerFromPath(void *address, int32_t *offset, uint32_t offsetAmount)
{
    uint32_t tempAddress = reinterpret_cast<uint32_t>(address);
    if (offsetAmount == 0)
    {
        return address;
    }
    else if (checkIfPointerIsValid(address))
    {
        tempAddress = *reinterpret_cast<uint32_t *>(tempAddress);
    }
    else
    {
        return nullptr;
    }
    
    uint32_t Counter = 0;
    for (uint32_t i = 0; i < (offsetAmount - 1); i++)
    {
        uint32_t *tempAddress2 = reinterpret_cast<uint32_t *>(tempAddress + offset[i]);
        if (checkIfPointerIsValid(tempAddress2))
        {
            tempAddress = *tempAddress2;
        }
        else
        {
            return nullptr;
        }
        Counter++;
    }
    
    return reinterpret_cast<void *>(tempAddress + offset[Counter]);
}

}