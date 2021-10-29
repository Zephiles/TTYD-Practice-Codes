#pragma once

#include <cstdint>

namespace mod {

extern "C" {

// Assembly overwrite functions
// main.cpp
void asmReplaceJumpFallAnim();
void asmAllowRunningFromBattles();
void asmForceNPCItemDrop();
void asmDisplayMegaJumpBadgeBattleMenu();
void asmDisplayMegaHammerBadgesBattleMenu();
void asmFixBlooperCrash1();
void asmFixBlooperCrash2();
void asmPreventTextboxSelection();
void asmDisableDPadOptionsDisplay();
void asmFixEvtMapBlendSetFlagPartnerCrash();
void asmFixEvtMapBlendSetFlagFollowerCrash();
void asmFallThroughMostObjectsStandard();
void asmFallThroughMostObjectsBowser();

#ifdef TTYD_EU
void asmJumpOnWater();
#endif

// Functions accessed by assembly overwrites
// main.cpp
bool displayMegaJumpBadgeInMenu(uint32_t checkBit);
bool displayMegaHammerBadgesInMenu(uint32_t checkBit);
uint32_t fixBlooperCrash1(uint32_t unkValue, void *battleUnitPointer);
void preventTextboxOptionSelection(char *currentText, void *storeAddress, int32_t selectedOption);
void *fixEvtMapBlendSetFlagPartnerCrash(void *partnerPtr);
void *fixEvtMapBlendSetFlagFollowerCrash(void *followerPtr);
const char *replaceJumpFallAnim(char *jumpFallString);
void *jumpOnWater(void *ptr);

// codes.cpp
uint32_t allowRunningFromBattles(void *ptr);
int32_t forceNPCItemDrop(void *ptr);
void *fallThroughMostObjects(void *ptr);

// draw.cpp
bool disableDPadOptionsDisplay(uint16_t unkVar);

// Misc assembly functions
bool asmErrorHandlerCheckPairedSinglesEnabled();
void asmErrorHandlerInterrupts();
void asmErrorHandlerEnableFPU();
void asmSetTime(int64_t time);

}

}