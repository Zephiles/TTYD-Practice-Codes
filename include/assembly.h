#ifndef ASSEMBLY_H
#define ASSEMBLY_H

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

    // Functions accessed by assembly overwrites
    uint32_t cFixBlooperCrash1(uint32_t unkValue, void *battleUnitPtr);

    PartyEntry *cFixEvtMapBlendSetFlagPartnerCrash(PartyEntry *partnerPtr);
    PartyEntry *cFixEvtMapBlendSetFlagFollowerCrash(PartyEntry *followerPtr);

#ifdef TTYD_JP
    int32_t crashScreenDecrementYPos();
#endif

    void preventDiaryTextboxOptionSelection(const char *currentText, int32_t *storeAddress, int32_t selectedOption);

    bool displayMegaBadgesInBattleMenu(bool checkJumpBadge, uint32_t bitfield);

    bool cFontDrawMessageMtxHandleCommand(const char *command, const char *commandValueString);

    uint32_t allowRunningFromBattles(void *ptr);

    int32_t forceNpcItemDrop(void *ptr);

    void *fallThroughMostObjects(void *ptr);

    const char *replaceJumpFallAnim(char *jumpFallString);

    // Misc assembly functions
    void asmSetTime(int64_t time);
}

#endif
