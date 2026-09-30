#include "mod.h"
#include "gc/OSModule.h"
#include "ttyd/rel/tou.h"
#include "ttyd/rel/tou2.h"
#include "ttyd/swdrv.h"
#include "ttyd/mariost.h"
#include "ttyd/seq_mapchange.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

Mod *gMod = nullptr;

const char *gHelpTextAConfirmBCancel = "Press A to confirm\nPress B to cancel";
const char *gHelpTextButtonCombo = "Button Combo (Can be used in any order)";
const char *gTimeStringFormat = "%02" PRIu32 ":%02" PRIu32 ":%02" PRIu32 ".%02" PRIu32;

GlitzPitVariables::GlitzPitVariables()
{
    // Initialize the variables
    // If the player is not currently in Glitzville, then assume that GSW(580) has Mario's current rank
    this->marioRank = static_cast<uint8_t>(swByteGet(580));

    this->rankWorkPtr = nullptr;
    this->rankingControllFuncPtr = nullptr;

    // Make sure a rel is actually loaded
    const OSModuleInfo *relPtr = _globalWorkPtr->relocationBase;
    if (!relPtr)
    {
        return;
    }

    // The variables can only be retrieved if the player is currently in Glitzville
    RankingControllFunc tempRankingControllFuncPtr;
    RankingData *tempRankWorkPtr;

    switch (relPtr->id)
    {
        case RelId::TOU: // Glitzville (excluding arena)
        {
            tempRankWorkPtr = tou_rank_wp;
            tempRankingControllFuncPtr = tou_rankingControll;
            break;
        }
        case RelId::TOU2: // Glitzville arena
        {
            tempRankWorkPtr = tou2_rank_wp;
            tempRankingControllFuncPtr = tou2_rankingControll;
            break;
        }
        default:
        {
            // Not currently in Glitzville, so no need to do anything else
            return;
        }
    }

    this->rankWorkPtr = tempRankWorkPtr;
    this->rankingControllFuncPtr = tempRankingControllFuncPtr;

    // Find Mario's current rank
    const uint32_t totalEntries = tempRankWorkPtr->count;
    const RankingEntry *entriesPtr = tempRankWorkPtr->entries;

    for (uint32_t i = 0; i < totalEntries; i++)
    {
        if (entriesPtr[i].fighterId == GlitzPitFighterId::GLITZ_PIT_FIGHTER_ID_MARIO)
        {
            this->marioRank = static_cast<uint8_t>(i);
            break;
        }
    }
}
