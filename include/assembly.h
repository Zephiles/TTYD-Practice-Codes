#ifndef ASSEMBLY_H
#define ASSEMBLY_H

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

    // Functions accessed by assembly overwrites
    uint32_t cFixBlooperCrash1(uint32_t unkValue, void *battleUnitPtr);

    void *cFixEvtMapBlendSetFlagPartnerCrash(void *partnerPtr);
    void *cFixEvtMapBlendSetFlagFollowerCrash(void *followerPtr);

#ifdef TTYD_JP
    int32_t crashScreenDecrementYPos();
#endif

    void preventDiaryTextboxOptionSelection(const char *currentText, int32_t *storeAddress, int32_t selectedOption);

    bool displayMegaBadgesInBattleMenu(bool checkJumpBadge, uint32_t bitfield);

    bool cFontDrawMessageMtxHandleCommand(const char *command, const char *commandValueString);
}

#endif
