#ifndef MISC_MISC_H
#define MISC_MISC_H

#include "gc/OSModule.h"
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

#define roundIntUpUnsigned(value, alignment) ((((value) + (alignment)-1)) / (alignment))
#define roundUp(size, alignment) (((size) + (alignment)-1) & ~((alignment)-1))
#define roundDown(size, alignment) (((size) / (alignment)) * (alignment))

bool checkButtonCombo(uint32_t combo);
bool checkButtonComboEveryFrame(uint32_t combo);
void *clearMemory(void *ptr, uint32_t size);
bool checkForSpecificSeq(SeqIndex wantedSeq);
bool checkIfInGame();
uint32_t secondsToFrames(uint32_t seconds);
bool systemLevelIsZero();
void setSystemLevel(int32_t level);
float intToFloat(int32_t value);
void intToFloatArray(int32_t *values, float *valuesOut, int32_t numValues);
uint32_t ptrIsValid(void *ptr);
void *getPartnerPtr();
void *getFollowerPtr();
void removePartnerFromOverworld();
void removeFollowerFromOverworld();
void spawnPartnerOrFollower(PartyMembers id);
void spawnFailsafePartnerOrFollower(bool shouldSpawnPartner);
void recheckJumpAndHammerLevels();
void recheckBattleUpgrades(ItemId item);
bool checkIfBadgeEquipped(ItemId badge);
void resetPauseMenuImportantItems();
void drawOnDebugLayer(DispCallback func, float order);
void drawOn2DLayer(DispCallback func, float order);
void clear_DC_IC_Cache(void *ptr, uint32_t size);

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

#endif
