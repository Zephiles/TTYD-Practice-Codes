#ifndef TTYD_MARIOST_H
#define TTYD_MARIOST_H

#include "gc/types.h"
#include "gc/OSModule.h"
#include "gc/OSContext.h"
#include "ttyd/fadedrv.h"
#include "ttyd/npcdrv.h"

#include <cstdint>

// Taken from NWPlayer123's decomp:
// https://github.com/NWPlayer123/PaperMario2/blob/master/include/mario/mariost.h
struct GlobalWork
{
    int32_t flags;
    int32_t framerate;
    int32_t unk8;
    int32_t unkC;
    BOOL dvdError;
    int32_t inBattle;
    int32_t systemLevelFlags;
    uint32_t retraceCount;
    OSTime retraceLocalTime;
    OSTime retraceDeltaTime;
    OSTime retraceTime;
    OSTime renderTime;      // Time since boot, minus menus
    OSTime renderFieldTime; // Time outside battle, minus menus
    OSTime systemLv0Time;
    OSTime systemLv1Time;
    OSTime systemLv2Time;
    OSTime systemLv3Time;
    uint8_t countdown[0x40];    // CountdownWork countdown;
    uint8_t mLotteryInfo[0x40]; // lottery_info mLotteryInfo;
    uint8_t field_0xE8[0xF8 - 0xE8];
    FadeType mapFadeOutType;
    int32_t mapFadeOutDuration;
    FadeType mapFadeInType;
    int32_t mapFadeInDuration;
    FadeType areaFadeOutType;
    int32_t areaFadeOutDuration;
    FadeType areaFadeInType;
    int32_t areaFadeInDuration;
    BOOL didAreaChange;
    char beroEnterName[0x10];
    char currentMapName[0x10];
    char currentAreaName[0x20];
    OSModuleInfo *relocationBase;
    void *mapAlloc;
    uint32_t field_0x164;
    FbatData *fbatData;
    int32_t language;
    uint16_t fbWidth;
    uint16_t efbHeight;
    int32_t mGSW0;
    uint32_t mGSWF[0x100];
    uint8_t mGSW[0x800];
    uint32_t mLSWF[0x10];
    uint8_t mLSW[0x400];
    int32_t unk11B8;
    uint8_t field_0x11BC[0x11D4 - 0x11BC];
    Vec3 savePlayerPos;
    int32_t savePartyId[2];
    OSTime saveTime;
    int32_t saveCount;
    uint8_t field_0x11F4[0x1274 - 0x11F4];
    int32_t unk1274;
    int32_t resetType; // 0 = no reset, 1 = soft reset, 2 = menu
    BOOL unk127C;
    BOOL softResetWait;
    uint8_t field_0x1284[0x1288 - 0x1284];
    OSTime softResetTimeout;
    BOOL field_0x1290;
    BOOL unk1294;
    uint8_t field_0x1298[0x12E8 - 0x1298];
    uint8_t field_0x12E8[4];
    uint8_t field_0x12EC[4];
    OSTime field_0x12F0[4];
    uint8_t unk1310[4];
    OSTick mDeltaGame;
    OSTick mDeltaRender;
    uint8_t field_0x131C[0x1324 - 0x131C];
    BOOL unk1324;
    uint32_t button[4];
    uint32_t buttonNew[4];
    uint32_t buttonRepeat[4];
    uint32_t buttonRepeatDelay[4];
    uint32_t buttonUp[4];
    uint32_t dirs[4];
    uint32_t dirsNew[4];
    uint32_t dirsRepeat[4];
    uint32_t dirsRepeatDelay[4];
    int8_t stickX[4];
    int8_t stickY[4];
    int8_t substickX[4];
    int8_t substickY[4];
    uint8_t triggerLeft[4];
    uint8_t triggerRight[4];
    uint8_t rumbleStatus[4];
    uint8_t prevRumbleStatus[4];
} __attribute__((__packed__));

static_assert(sizeof(GlobalWork) == 0x13D8);

extern "C"
{
    extern GlobalWork *globalWorkPtr; // gp
    extern int32_t _mariostSystemLevel;

    void marioStInit();
    void marioStMain();
    void marioStDisp();
    void marioStSystemLevel(int32_t level);
    uint32_t marioStGetSystemLevel();
    void viPostCallback(uint32_t retraceCount);
    void gcDvdCheckThread();
    void gcRumbleCheck();
    // L_gcResetCheck
    void N_systemErrorHandler(uint16_t error, OSContext *context, uint32_t dsisr, uint32_t dar);
    // N_setupErrorHandler
}

#endif
