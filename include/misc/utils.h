#ifndef MISC_MISC_H
#define MISC_MISC_H

#include "gc/types.h"
#include "ttyd/seqdrv.h"
#include "ttyd/party.h"
#include "ttyd/item_data.h"
#include "ttyd/dispdrv.h"

#include <cstdint>

enum PointerVerificationType
{
    PTR_INVALID = 0,
    PTR_CACHED,
    PTR_UNCACHED,
};

#define intCeil(value, alignment) (((value) + ((alignment)-1)) / (alignment))
#define roundUp(size, alignment) (((size) + ((alignment)-1)) & ~((alignment)-1))
#define roundDown(size, alignment) (((size) / (alignment)) * (alignment))

bool checkButtonCombo(uint32_t combo);
bool checkButtonsEveryFrame(uint32_t buttons);
bool checkButtonComboEveryFrame(uint32_t combo);
bool checkButtonComboDemo(uint32_t combo);
bool compareStringToNextMap(const char *str);
uint32_t getSequencePosition();
void setSequencePosition(uint32_t value);
void setSeqMapChange(const char *map, const char *bero);
void *clearMemory(void *ptr, uint32_t size);
bool checkForSpecificSeq(SeqIndex wantedSeq);
bool checkIfInGame();
bool systemLevelIsZero();
void setSystemLevel(int32_t level);
float intToFloat(int32_t value);
int32_t floatToInt(float value);
void intToFloatArray(int32_t *values, float *valuesOut, int32_t numValues);
uint32_t ptrIsValid(void *ptr);
PartyEntry *getPartnerPtr();
PartyEntry *getFollowerPtr();
PartyMembers getCurrentPartnerOrFollowerOut(bool getPartner);
void removePartnerFromOverworld();
void removeFollowerFromOverworld();
PartySlotId spawnPartnerOrFollower(PartyMembers id);
PartySlotId spawnFailsafePartnerOrFollower(bool shouldSpawnPartner);
void recheckJumpAndHammerLevels();
void recheckBattleUpgrades(ItemId item);
bool checkIfBadgeEquipped(ItemId badge);
void resetPauseMenuImportantItems();
void resetPauseMenuPartners();
void drawOnDebugLayer(DispCallback func, float order);
void drawOn2DLayer(DispCallback func, float order);
void getTimeString(char *stringOut, uint32_t stringSize, OSTime time);
bool getStickAngle(int32_t stickXYOut[2], double *stickAngleOut);
void getStickAngleString(char *stringOut, uint32_t stringSize);
void clear_DC_IC_Cache(void *ptr, uint32_t size);

// Functions for calling fpclassify on floats/doubles and checking if they are either FP_ZERO or FP_NORMAL
bool floatCanBeWorkedWith(float value);
bool doubleCanBeWorkedWith(double value);

template<typename Ptr>
uint32_t getOffset(Ptr ptr, uint32_t alignment)
{
    return reinterpret_cast<uint32_t>(ptr) & (alignment - 1);
}

template<typename Ptr>
uint32_t ptrIsValid(Ptr ptr)
{
    return ptrIsValid(reinterpret_cast<void *>(ptr));
}

template<typename Ptr>
Ptr clearMemory(Ptr ptr, uint32_t size)
{
    return reinterpret_cast<Ptr>(clearMemory(reinterpret_cast<void *>(ptr), size));
}

template<typename Ptr>
void clear_DC_IC_Cache(Ptr ptr, uint32_t size)
{
    return clear_DC_IC_Cache(reinterpret_cast<void *>(ptr), size);
}

#endif
