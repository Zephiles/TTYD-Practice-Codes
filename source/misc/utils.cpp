#include "mod.h"
#include "gc/pad.h"
#include "gc/OSModule.h"
#include "gc/OSCache.h"
#include "ttyd/system.h"
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

bool checkButtonCombo(uint32_t combo)
{
    if ((keyGetButton(PadId::CONTROLLER_ONE) & combo) != combo)
    {
        return false;
    }

    return keyGetButtonTrg(PadId::CONTROLLER_ONE) & combo;
}

bool checkButtonComboEveryFrame(uint32_t combo)
{
    return (keyGetButton(PadId::CONTROLLER_ONE) & combo) == combo;
}

void *clearMemory(void *ptr, uint32_t size)
{
    return memset(ptr, 0, size);
}

bool checkForSpecificSeq(SeqIndex wantedSeq)
{
    SeqIndex nextSeq = seqGetNextSeq();
    if (nextSeq != wantedSeq)
    {
        return false;
    }

    SeqIndex currentSeq = seqGetSeq();
    return currentSeq == wantedSeq;
}

bool checkIfInGame()
{
    SeqIndex nextSeq = seqGetNextSeq();
    SeqIndex game = SeqIndex::kGame;

    if (nextSeq != game)
    {
        return false;
    }

    SeqIndex currentSeq = seqGetSeq();
    if (currentSeq != game)
    {
        return false;
    }

    OSModuleInfo *relPtr = globalWorkPtr->relocationBase;
    if (!relPtr)
    {
        return false;
    }

    return relPtr->id != RelId::DMO;
}

uint32_t secondsToFrames(uint32_t seconds)
{
    return sysMsec2Frame(seconds * 1000);
}

bool systemLevelIsZero()
{
    return (_mariostSystemLevel == 0) && (marioStGetSystemLevel() == 0);
}

void setSystemLevel(int32_t level)
{
    // Make sure the level is valid
    if ((level < 0) || (level > 4))
    {
        return;
    }

    Mod *modPtr = gMod;
    const bool systemLevelIsSet = modPtr->checkIfSystemLevelIsSet();

    if (level == 0) // Being reset
    {
        // Only reset if was previously set
        if (!systemLevelIsSet)
        {
            return;
        }

        // The only reason the system level wouldn't be reset now is if in a screen ransion/battle/etc., so reset it
        modPtr->clearSystemLevel();

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

        modPtr->setSystemLevel();
    }

    // Set the new system level
    marioStSystemLevel(level);
}

float __attribute__((noinline)) intToFloat(int32_t value)
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

uint32_t ptrIsValid(void *ptr)
{
    uint32_t ptrRaw = reinterpret_cast<uint32_t>(ptr);

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

void *getPartnerPtr()
{
    PartySlotId Id = marioGetPartyId();
    return partyGetPtr(Id);
}

void *getFollowerPtr()
{
    PartySlotId Id = marioGetExtraPartyId();
    return partyGetPtr(Id);
}

PartyMembers getCurrentPartnerOrFollowerOut(bool getPartner)
{
    uint32_t ptrRaw;
    if (getPartner)
    {
        ptrRaw = reinterpret_cast<uint32_t>(getPartnerPtr());
    }
    else
    {
        ptrRaw = reinterpret_cast<uint32_t>(getFollowerPtr());
    }

    PartyMembers currentPartnerOut = PartyMembers::kNone;
    if (ptrRaw)
    {
        currentPartnerOut = *reinterpret_cast<PartyMembers *>(ptrRaw + 0x31);
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

void spawnPartnerOrFollower(PartyMembers id)
{
    // Make sure the partner/follower is valid
    if ((id <= PartyMembers::kNone) || (id > PartyMembers::kMsMowzFollower))
    {
        return;
    }

    PartySlotId partySlotId;

    // If spawning a partner, make sure they are enabled when marioPartyEntry is called
    if ((id >= PartyMembers::kGoombella) && (id <= PartyMembers::kMsMowz))
    {
        // Check if the selected partner is in the partner menu
        const bool partnerInPauseMenu = partyChkJoin(id);

        // Make sure the selected partner is in the partner menu, as otherwise marioPartyEntry will not spawn them
        partyJoin(id);

        // Spawn the partner
        partySlotId = marioPartyEntry(id);

        // If the selected partner was not in the partner menu, then remove them
        if (!partnerInPauseMenu)
        {
            partyLeft(id);
        }
    }
    else
    {
        // Spawn the follower
        partySlotId = marioPartyEntry(id);
    }

    // Make sure the partner/follower spawned properly
    if (partySlotId <= PartySlotId::kNone)
    {
        return;
    }

    // Make sure the partner/follower is moving
    partyRun(partyGetPtr(partySlotId));
}

void spawnFailsafePartnerOrFollower(bool shouldSpawnPartner)
{
    Player *player = marioGetPtr();
    PartyMembers previousOut;

    if (shouldSpawnPartner)
    {
        // Get the previous partner out
        previousOut = player->prevPartnerId;
    }
    else
    {
        // Get the previous follower out
        previousOut = player->prevFollowerId;
    }

    // Check if a partner/follower was previously out
    if (previousOut != PartyMembers::kNone)
    {
        // A partner/follower was previously out, so bring them back out
        spawnPartnerOrFollower(previousOut);
    }
    else if (shouldSpawnPartner)
    {
        // No partner was previously out, so bring out Goombella
        spawnPartnerOrFollower(PartyMembers::kGoombella);
    }
    else
    {
        // No follower was previously out, so bring out Frankly
        spawnPartnerOrFollower(PartyMembers::kFrankly);
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

void clear_DC_IC_Cache(void *ptr, uint32_t size)
{
    DCFlushRange(ptr, size);
    ICInvalidateRange(ptr, size);
}
