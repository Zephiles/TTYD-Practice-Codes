#ifndef ASSEMBLY_H
#define ASSEMBLY_H

#include "gc/types.h"
#include "ttyd/party.h"

#include <cstdint>

extern "C"
{
    // Assembly overwrite functions
    void asmFixBlooperCrash1();
    void asmFixBlooperCrash2();

    void asmFixEvtMapBlendSetFlagPartnerCrashStart();
    void asmFixEvtMapBlendSetFlagPartnerCrashBranchBack();
    void asmFixEvtMapBlendSetFlagFollowerCrashStart();
    void asmFixEvtMapBlendSetFlagFollowerCrashBranchBack();

#ifdef TTYD_JP
    void asmCrashScreenDecrementYPosStart();
    void asmCrashScreenDecrementYPosBranchBack();
#endif

    void asmPreventDiaryTextboxSelection();

    void asmDisplayMegaJumpBadgeInBattleMenu();
    void asmDisplayMegaHammerBadgesInBattleMenu();

    void asmFontDrawMessageMtxHandleCommandStart();
    void asmFontDrawMessageMtxHandleCommandBranchBack();

    void asmAllowRunningFromBattlesStart();
    void asmAllowRunningFromBattlesBranchBack();

    void asmForceNpcItemDrop();

    void asmFallThroughMostObjectsStandard();
    void asmFallThroughMostObjectsBowser();

    void asmReplaceJumpFallAnim();

    void asmDisableDPadOptionsDisplay();

#ifdef TTYD_EU
    void asmJumpOnWaterStart();
    void asmJumpOnWaterBranchBack();
#endif

    // Functions accessed by assembly overwrites
    uint32_t cFixBlooperCrash1(uint32_t unkValue, void *battleUnitPtr);

    PartyEntry *cFixEvtMapSetFlagCrash(PartyEntry *partyPtr, bool shouldSpawnPartner);

#ifdef TTYD_JP
    int32_t cCrashScreenDecrementYPos();
#endif

    void cPreventDiaryTextboxOptionSelection(const char *currentText, int32_t *storeAddress, int32_t selectedOption);

    bool cDisplayMegaBadgesInBattleMenu(bool checkJumpBadge, uint32_t bitfield);

    bool cFontDrawMessageMtxHandleCommand(const char *command, const char *commandValueString);

    uint32_t cAllowRunningFromBattles(void *ptr);

    int32_t cForceNpcItemDrop(void *ptr);

    void *cFallThroughMostObjects(void *ptr);

    const char *cReplaceJumpFallAnim(char *jumpFallString);

    bool cDisableDPadOptionsDisplay(uint32_t unkVar);

#ifdef TTYD_EU
    void *cJumpOnWater(void *ptr);
#endif

    // Misc assembly functions
    void asmClearDCICCache(void *ptr, uint32_t size);
    void asmSetTime(OSTime time);
}

#endif
