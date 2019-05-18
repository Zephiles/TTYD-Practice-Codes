#pragma once

#include <ttyd/seqdrv.h>

#include <cstdint>

namespace mod {

extern "C" {

bool checkButtonCombo(uint16_t combo);
bool checkButtonComboEveryFrame(uint16_t combo);
bool checkForSpecificSeq(ttyd::seqdrv::SeqIndex wantedSeq);
bool checkIfInGame();
void *getCurrentRELPointer();
void *getBattlePointer();
void *getMarioBattlePointer();
void *getPartnerBattlePointer();
void *getActorPointer(uint32_t slot);
uint32_t secondsToFrames(uint32_t seconds);
uint32_t getSequencePosition();
void setSequencePosition(uint32_t value);
void setNextMap(const char *map);
void setNextBero(const char *bero);
char *copyString(char *destination, const char *source);
void *copyMemory(void *destination, const void *source, uint32_t size);
bool compareStrings(const char *str1, const char *str2);
bool compareStringsSize(const char *str1, const char *str2, uint32_t size);
bool compareStringToNextMap(const char *str);
uint32_t getStringSize(const char *str);
uint32_t getSystemLevel();
void setSystemLevel(uint32_t value);
void setSeqMapChange(const char *map, const char *bero);
void *clearMemory(void *destination, uint32_t size);
int32_t getPartnerID();
int32_t getFollowerID();
void *getPartnerPointer();
void *getFollowerPointer();
void removePartnerFromOverworld();
void removeFollowerFromOverworld();
bool checkIfBadgeEquipped(int16_t badge);
void recheckJumpAndHammerLevels();
uint32_t getCurrentPitFloor();
void clearGSWFsRange(uint32_t lowerBound, uint32_t upperBound);
void clear_DC_IC_Cache(void *ptr, uint32_t size);
bool checkIfPointerIsValid(void *ptr);
void *getLastPointerFromPath(void *address, int32_t *offset, uint32_t offsetAmount);;

}

}