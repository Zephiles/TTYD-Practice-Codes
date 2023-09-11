#include "mod.h"
#include "gc/pad.h"
#include "gc/DEMOPad.h"
#include "gc/OSModule.h"
#include "gc/types.h"
#include "gc/os.h"
#include "gc/OSCache.h"
#include "ttyd/system.h"
#include "ttyd/swdrv.h"
#include "ttyd/mariost.h"
#include "ttyd/seqdrv.h"
#include "ttyd/seq_mapchange.h"
#include "ttyd/party.h"
#include "ttyd/mario_party.h"
#include "ttyd/mario_pouch.h"
#include "ttyd/mario.h"
#include "ttyd/item_data.h"
#include "ttyd/win_main.h"
#include "ttyd/win_item.h"
#include "ttyd/win_party.h"
#include "ttyd/dispdrv.h"
#include "misc/utils.h"

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cinttypes>
#include <cmath>

bool checkButtonCombo(uint32_t combo)
{
    if ((keyGetButton(PadId::CONTROLLER_ONE) & combo) != combo)
    {
        return false;
    }

    return keyGetButtonTrg(PadId::CONTROLLER_ONE) & combo;
}

bool checkButtonsEveryFrame(uint32_t buttons)
{
    return keyGetButton(PadId::CONTROLLER_ONE) & buttons;
}

bool checkButtonComboEveryFrame(uint32_t combo)
{
    return (keyGetButton(PadId::CONTROLLER_ONE) & combo) == combo;
}

bool checkButtonComboDemo(uint32_t combo)
{
    const DEMOPadStatus *demoPadPtr = &DemoPad[static_cast<uint32_t>(PadId::CONTROLLER_ONE)];

    if ((demoPadPtr->buttons & combo) != combo)
    {
        return false;
    }

    return demoPadPtr->buttonsDown & combo;
}

uint32_t getSequencePosition()
{
    return swByteGet(0);
}

void setSequencePosition(uint32_t position)
{
    swByteSet(0, position);
}

bool compareStringToNextMap(const char *str)
{
    return strcmp(str, _next_map) == 0;
}

void setSeqMapChange(const char *map, const char *bero)
{
    seqSetSeq(SeqIndex::kMapChange, map, bero);
}

void *clearMemory(void *ptr, uint32_t size)
{
    return memset(ptr, 0, size);
}

bool checkForSpecificSeq(SeqIndex wantedSeq)
{
    if (seqGetNextSeq() != wantedSeq)
    {
        return false;
    }

    return seqGetSeq() == wantedSeq;
}

bool checkIfInGame()
{
    constexpr SeqIndex gameSeq = SeqIndex::kGame;
    if (seqGetNextSeq() != gameSeq)
    {
        return false;
    }

    if (seqGetSeq() != gameSeq)
    {
        return false;
    }

    const OSModuleInfo *relPtr = globalWorkPtr->relocationBase;
    if (!relPtr)
    {
        return false;
    }

    return relPtr->id != RelId::DMO;
}

bool systemLevelIsZero()
{
    return (marioStGetSystemLevel() == 0) && (_mariostSystemLevel == 0);
}

void setSystemLevel(int32_t level)
{
    // Make sure the level is valid
    if ((level < 0) || (level > 4))
    {
        return;
    }

    Mod *modPtr = gMod;
    const bool systemLevelIsSet = modPtr->flagIsSet(ModFlag::MOD_FLAG_SYSTEM_LEVEL);

    if (level == 0) // Being reset
    {
        // Only reset if was previously set
        if (!systemLevelIsSet)
        {
            return;
        }

        // The only reason the system level wouldn't be reset now is if in a screen ransion/battle/etc., so reset it
        modPtr->clearFlag(ModFlag::MOD_FLAG_SYSTEM_LEVEL);

        // Only reset if not in a battle nor a screen transition
        if (!checkIfInGame())
        {
            return;
        }
    }
    else
    {
        // Only set if not currently set
        if (systemLevelIsSet)
        {
            return;
        }

        // Only set if not in a battle nor a screen transition
        if (!checkIfInGame())
        {
            return;
        }

        // Don't set if it's already set by something else
        if (!systemLevelIsZero())
        {
            return;
        }

        modPtr->setFlag(ModFlag::MOD_FLAG_SYSTEM_LEVEL);
    }

    // Set the new system level
    marioStSystemLevel(level);
}

__attribute__((noinline)) float intToFloat(int32_t value)
{
    return static_cast<float>(value);
}

__attribute__((noinline)) int32_t floatToInt(float value)
{
    return static_cast<int32_t>(value);
}

void intToFloatArray(int32_t *values, float *valuesOut, int32_t numValues)
{
    for (int32_t i = 0; i < numValues; i++)
    {
        valuesOut[i] = intToFloat(values[i]);
    }
}

uint32_t ptrIsValid(void *ptr)
{
    const uint32_t ptrRaw = reinterpret_cast<uint32_t>(ptr);

    // Cached memory
    if ((ptrRaw >= 0x80000000) && (ptrRaw < 0x81800000))
    {
        return PointerVerificationType::PTR_CACHED;
    }

    // Uncached memory
    if ((ptrRaw >= 0xC0000000) && (ptrRaw < 0xC1800000))
    {
        return PointerVerificationType::PTR_UNCACHED;
    }

    return PointerVerificationType::PTR_INVALID;
}

PartyEntry *getPartnerPtr()
{
    return partyGetPtr(marioGetPartyId());
}

PartyEntry *getFollowerPtr()
{
    return partyGetPtr(marioGetExtraPartyId());
}

PartyMembers getCurrentPartnerOrFollowerOut(bool getPartner)
{
    PartyEntry *partyPtr;
    if (getPartner)
    {
        partyPtr = getPartnerPtr();
    }
    else
    {
        partyPtr = getFollowerPtr();
    }

    PartyMembers currentPartnerOut;
    if (partyPtr)
    {
        currentPartnerOut = partyPtr->currentMemberId;
    }
    else
    {
        currentPartnerOut = PartyMembers::kNone;
    }

    return currentPartnerOut;
}

void removePartnerFromOverworld()
{
    partyKill2(marioGetPartyId());
}

void removeFollowerFromOverworld()
{
    partyKill2(marioGetExtraPartyId());
}

PartySlotId spawnPartnerOrFollower(PartyMembers id)
{
    // Make sure the partner/follower is valid
    if ((id <= PartyMembers::kNone) || (id > PartyMembers::kMsMowzFollower))
    {
        return PartySlotId::kNone;
    }

    PartySlotId slotId;

    // If spawning a partner, make sure they are enabled when marioPartyEntry is called
    if ((id >= PartyMembers::kGoombella) && (id <= PartyMembers::kMsMowz))
    {
        // Check if the selected partner is in the partner menu
        const bool partnerInPauseMenu = partyChkJoin(id);

        // Make sure the selected partner is in the partner menu, as otherwise marioPartyEntry will not spawn them
        partyJoin(id);

        // Spawn the partner
        slotId = marioPartyEntry(id);

        // If the selected partner was not in the partner menu, then remove them
        if (!partnerInPauseMenu)
        {
            partyLeft(id);
        }
    }
    else
    {
        // Spawn the follower
        slotId = marioPartyEntry(id);
    }

    // Make sure the partner/follower spawned properly
    if (slotId <= PartySlotId::kNone)
    {
        return slotId;
    }

    // Make sure the partner/follower is moving
    partyRun(partyGetPtr(slotId));

    return slotId;
}

PartySlotId spawnFailsafePartnerOrFollower(bool shouldSpawnPartner)
{
    const Player *playerPtr = marioGetPtr();
    PartyMembers previousOut;

    if (shouldSpawnPartner)
    {
        // Get the previous partner out
        previousOut = playerPtr->prevPartnerId;
    }
    else
    {
        // Get the previous follower out
        previousOut = playerPtr->prevFollowerId;
    }

    // Check if a partner/follower was previously out
    if (previousOut != PartyMembers::kNone)
    {
        // A partner/follower was previously out, so bring them back out
        return spawnPartnerOrFollower(previousOut);
    }
    else if (shouldSpawnPartner)
    {
        // No partner was previously out, so bring out Goombella
        return spawnPartnerOrFollower(PartyMembers::kGoombella);
    }
    else
    {
        // No follower was previously out, so bring out Frankly
        return spawnPartnerOrFollower(PartyMembers::kFrankly);
    }
}

void recheckJumpAndHammerLevels()
{
    pouchGetJumpLv();
    pouchGetHammerLv();
}

void recheckBattleUpgrades(ItemId item)
{
    // Only run while in a battle
    if (!checkForSpecificSeq(SeqIndex::kBattle))
    {
        return;
    }

    // Only run if the current item is a boot or hammer upgrade
    switch (item)
    {
        case ItemId::ITEM_NORMAL_BOOTS:
        case ItemId::ITEM_SUPER_BOOTS:
        case ItemId::ITEM_ULTRA_BOOTS:
        case ItemId::ITEM_NORMAL_HAMMER:
        case ItemId::ITEM_SUPER_HAMMER:
        case ItemId::ITEM_ULTRA_HAMMER:
        {
            // Call the functions for checking which upgrades the player has
            recheckJumpAndHammerLevels();
            break;
        }
        default:
        {
            break;
        }
    }
}

bool checkIfBadgeEquipped(ItemId badge)
{
    return pouchEquipCheckBadge(badge) > 0;
}

void resetPauseMenuImportantItems()
{
    // Only run if the pause menu is currently open
    if ((marioStGetSystemLevel() & 15) != 15)
    {
        // The pause menu is not open, so do nothing
        return;
    }

    // Get the current submenu that is open in the items menu
    void *pauseMenuPtr = winGetPtr();
    uint32_t *subMenu = reinterpret_cast<uint32_t *>(reinterpret_cast<uint32_t>(pauseMenuPtr) + 0x210);
    const uint32_t importantItemsSubMenu = *subMenu;

    // Clear the item menu
    winItemExit(pauseMenuPtr);

    // Re-init the item menu
    winItemInit(pauseMenuPtr);

    // Restore the submenu
    *subMenu = importantItemsSubMenu;
}

void resetPauseMenuPartners()
{
    // Only run if the pause menu is currently open
    if ((marioStGetSystemLevel() & 15) != 15)
    {
        // The pause menu is not open, so do nothing
        return;
    }

    void *pauseMenuPtr = winGetPtr();

    // Clear the partner menu
    winPartyExit(pauseMenuPtr);

    // Re-init the partner menu
    winPartyInit(pauseMenuPtr);
}

void drawOnDebugLayer(DispCallback func, float order)
{
    dispEntry(CameraId::kDebug3d, 2, order, func, nullptr);
}

void drawOn2DLayer(DispCallback func, float order)
{
    dispEntry(CameraId::k2d, 2, order, func, nullptr);
}

void getTimeString(char *stringOut, uint32_t stringSize, OSTime time)
{
    const uint32_t busClock = OSBusClock;
    const uint32_t timeBase = busClock / 4;
    const uint32_t currentFps = static_cast<uint32_t>(globalWorkPtr->framerate);

    OSTime currentTime = time / (timeBase / currentFps);

    // Check if the value is negative
    char format[32];
    char *formatPtr = format;

    if (currentTime < 0)
    {
        // Convert the number to positive
        currentTime = -currentTime;

        // Initialize the format string to have the minus sign for negative
        formatPtr[0] = '-';

        // Increment formatPtr to not overwrite the minus sign
        formatPtr++;
    }

    // Set up the rest of the format string
    strcpy(formatPtr, gTimeStringFormat);

    // Handle the value as unsigned
    const uint64_t currentTimeUnsigned = static_cast<uint64_t>(currentTime);
    const uint32_t totalSeconds = currentTimeUnsigned / currentFps;
    const uint32_t totalMinutes = totalSeconds / 60;

    const uint32_t hours = totalMinutes / 60;
    const uint32_t minutes = totalMinutes % 60;
    const uint32_t seconds = totalSeconds % 60;

    // Cast currentTimeUnsigned to avoid using __umoddi3 for modding
    const uint32_t frames = static_cast<uint32_t>(currentTimeUnsigned) % currentFps;

    // Create the string
    snprintf(stringOut, stringSize, format, hours, minutes, seconds, frames);
}

bool getStickAngle(int32_t stickXYAnglesOut[2], double *stickAngleOut)
{
    int32_t stickXInt = static_cast<int32_t>(keyGetStickX(PadId::CONTROLLER_ONE));
    int32_t stickYInt = static_cast<int32_t>(keyGetStickY(PadId::CONTROLLER_ONE));

    // Check if the stick is at the neutral position
    if ((stickXInt == 0) && (stickYInt == 0))
    {
        // The stick is currently at the neutral position
        if (stickXYAnglesOut)
        {
            stickXYAnglesOut[0] = stickXInt;
            stickXYAnglesOut[1] = stickYInt;
        }
        return false;
    }

    if (stickXInt > 127)
    {
        stickXInt -= 256;
    }

    if (stickYInt > 127)
    {
        stickYInt -= 256;
    }

    // Store the individual stick values if desired
    if (stickXYAnglesOut)
    {
        stickXYAnglesOut[0] = stickXInt;
        stickXYAnglesOut[1] = stickYInt;
    }

    const double stickX = static_cast<double>(stickXInt);
    const double stickY = static_cast<double>(stickYInt);
    constexpr double PI = 3.14159265358979323846;

    double stickAngle = atan2(stickX, stickY) * (180.0 / PI);
    if (stickAngle < 0.0)
    {
        stickAngle += 360.0;
    }

    *stickAngleOut = stickAngle;
    return true;
}

void getStickAngleString(char *stringOut, uint32_t stringSize)
{
    int32_t stickXYAngles[2];
    double stickAngle;

    // Check if the stick is at the neutral position
    if (!getStickAngle(stickXYAngles, &stickAngle))
    {
        // The stick is currently at the neutral position
        // Use snprintf to make sure stringSize is not exceeded, and that a null terminator is properly applied
        snprintf(stringOut, stringSize, "Neutral");
        return;
    }

    // Create the string
    snprintf(stringOut, stringSize, "%.2f  %" PRId32 "  %" PRId32, stickAngle, stickXYAngles[0], stickXYAngles[1]);
}

void clear_DC_IC_Cache(void *ptr, uint32_t size)
{
    DCFlushRange(ptr, size);
    ICInvalidateRange(ptr, size);
}

bool floatCanBeWorkedWith(float value)
{
    const int32_t ret = std::fpclassify(value);
    return (ret == FP_ZERO) || (ret == FP_NORMAL);
}

bool doubleCanBeWorkedWith(double value)
{
    const int32_t ret = std::fpclassify(value);
    return (ret == FP_ZERO) || (ret == FP_NORMAL);
}
