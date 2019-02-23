#pragma once

#include <ttyd/seqdrv.h>

#include <cstdint>

namespace mod {

bool checkButtonCombo(uint16_t combo);
bool checkButtonComboEveryFrame(uint16_t combo);
bool checkForSpecificSeq(ttyd::seqdrv::SeqIndex wantedSeq);
bool checkIfInGame();
void *getBattlePointer();
void *getMarioBattlePointer();
void *getPartnerBattlePointer();
void *getActorPointer(uint32_t slot);
uint32_t secondsToFrames(uint32_t seconds);
uint32_t getSequencePosition();
void setSequencePosition(uint32_t value);
void setNextMap(const char *map);
void setNextBero(const char *bero);
bool compareStrings(const char *str1, const char *str2);
bool compareStringsSize(const char *str1, const char *str2, uint32_t size);
bool compareStringToNextMap(const char *str);
uint32_t getSystemLevel();
void setSystemLevel(uint32_t value);
void setSeq(const char *map, const char *bero);
void setSpecificSeq(ttyd::seqdrv::SeqIndex seq, const char *map, const char *bero);
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

}