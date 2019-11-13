#pragma once

#include <cstdint>

namespace mod {

extern "C" {

// Assembly overwrite functions
// main.cpp
void StartReplaceJumpFallAnim();
void StartAllowRunningFromBattles();
void StartForceNPCItemDrop();
void StartDisplayMegaJumpBadgeBattleMenu();
void StartDisplayMegaHammerBadgesBattleMenu();
void StartFixBlooperCrash1();
void StartFixBlooperCrash2();
void StartPreventTextboxSelection();
void StartDisableDPadOptionsDisplay();
void StartFixEvtMapBlendSetFlagPartnerCrash();
void StartFixEvtMapBlendSetFlagFollowerCrash();
void StartFallThroughMostObjectsStandard();
void StartFallThroughMostObjectsBowser();

// Functions accessed by assembly overwrites
// main.cpp
bool displayMegaJumpBadgeInMenu(uint32_t checkBit);
bool displayMegaHammerBadgesInMenu(uint32_t checkBit);
uint32_t fixBlooperCrash1(uint32_t unkValue, void *battleUnitPointer);
void preventTextboxOptionSelection(char *currentText, void *storeAddress, int32_t selectedOption);
uint32_t fixRoomProblems();
void *fixEvtMapBlendSetFlagPartnerCrash(void *partnerPtr);
void *fixEvtMapBlendSetFlagFollowerCrash(void *followerPtr);
const char *replaceJumpFallAnim(char *jumpFallString);

// codes.cpp
uint32_t allowRunningFromBattles(void *ptr);
int32_t forceNPCItemDrop(void *ptr);
void *fallThroughMostObjects(void *ptr);

// draw.cpp
bool disableDPadOptionsDisplay(uint16_t unkVar);

}

}