#pragma once

#include <gc/OSModule.h>
#include <ttyd/seqdrv.h>
#include <ttyd/npcdrv.h>
#include <ttyd/memory.h>
#include <ttyd/party.h>

#include <cstdint>
#include <cstring>

namespace mod {

enum PointerVerificationType : uint32_t
{
    PTR_INVALID = 0,
    PTR_CACHED,
    PTR_UNCACHED,
};

extern "C" {

bool checkButtonCombo(uint32_t combo);
bool checkButtonComboEveryFrame(uint32_t combo);
bool checkButtonComboDemo(uint32_t combo);
bool checkForSpecificSeq(ttyd::seqdrv::SeqIndex wantedSeq);
bool checkIfInGame();
gc::OSModule::OSModuleInfo *getCurrentRelPtr();
void *getBattleWorkPointer();
void *getMarioBattlePointer();
void *getPartnerBattlePointer();
void *getActorPointer(uint32_t slot);
ttyd::npcdrv::NpcEntry *getNpcEntryData(uint32_t slot, bool getBattleData);
uint32_t secondsToFrames(uint32_t seconds);
uint32_t getSequencePosition();
bool checkIfSystemLevelIsSet();
void setSequencePosition(uint32_t value);
void setNextMap(const char *map);
void setNextBero(const char *bero);
bool compareStrings(const char *str1, const char *str2);
bool compareStringsSize(const char *str1, const char *str2, size_t size);
bool compareStringToNextMap(const char *str);
void setSeqMapChange(const char *map, const char *bero);
void *clearMemory(void *destination, std::size_t size);
ttyd::memory::SmartAllocationData *allocFromSmartHeap(uint32_t size, uint32_t group);
void *getPartnerPointer();
void *getFollowerPointer();
void removePartnerFromOverworld();
void removeFollowerFromOverworld();
uint16_t getSpecialMoveBits();
uint32_t getCurrentYoshiColorId();
void setNewYoshiColorId(uint32_t colorId);
void spawnPartnerOrFollower(ttyd::party::PartyMembers id);
void spawnFailsafePartnerOrFollower(bool shouldSpawnPartner);
bool checkIfBadgeEquipped(int16_t badge);
bool checkIfHaveItem(int16_t item);
void recheckJumpAndHammerLevels();
uint32_t getCurrentPitFloor();
void setPitFloor(int32_t floor);
uint32_t getCurrentFPS();
void clearGSWFsRange(uint32_t lowerBound, uint32_t upperBound);
float intToFloat(int32_t value);
void intToFloatArray(int32_t *values, float *valuesOut, int32_t numValues);
uint32_t checkIfPointerIsValid(const void *ptr);
void *getLastPointerFromPath(void *address, int32_t *offset, uint32_t offsetAmount);

}

}