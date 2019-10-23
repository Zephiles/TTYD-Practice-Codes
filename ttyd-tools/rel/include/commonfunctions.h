#pragma once

#include <gc/OSModule.h>
#include <ttyd/seqdrv.h>
#include <ttyd/party.h>

#include <cstdint>
#include <cstring>

namespace mod {

extern "C" {

bool checkButtonCombo(uint32_t combo);
bool checkButtonComboEveryFrame(uint32_t combo);
bool checkForSpecificSeq(ttyd::seqdrv::SeqIndex wantedSeq);
bool checkIfInGame();
gc::OSModule::OSModuleInfo *getCurrentRelModuleInfo();
void *getBattleWorkPointer();
void *getMarioBattlePointer();
void *getPartnerBattlePointer();
void *getActorPointer(uint32_t slot);
void *getNPCFieldWorkPointer(uint32_t npcSlot);
uint32_t secondsToFrames(uint32_t seconds);
uint32_t getSequencePosition();
void setSequencePosition(uint32_t value);
void setNextMap(const char *map);
void setNextBero(const char *bero);
bool compareStrings(const char *str1, const char *str2);
bool compareStringsSize(const char *str1, const char *str2, size_t size);
bool compareStringToNextMap(const char *str);
void setSeqMapChange(const char *map, const char *bero);
void *clearMemory(void *destination, std::size_t size);
void *getPartnerPointer();
void *getFollowerPointer();
void removePartnerFromOverworld();
void removeFollowerFromOverworld();
uint32_t getCurrentYoshiColorId();
void setNewYoshiColorId(uint32_t colorId);
void spawnPartnerOrFollower(ttyd::party::PartyMembers id);
bool checkIfBadgeEquipped(int16_t badge);
bool checkIfHaveItem(int16_t item);
void recheckJumpAndHammerLevels();
uint32_t getCurrentPitFloor();
uint32_t getCurrentFPS();
void clearGSWFsRange(uint32_t lowerBound, uint32_t upperBound);
bool checkIfPointerIsValid(void *ptr);
void *getLastPointerFromPath(void *address, int32_t *offset, uint32_t offsetAmount);

}

}