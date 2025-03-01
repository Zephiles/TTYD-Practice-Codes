#ifndef TTYD_FADEDRV_H
#define TTYD_FADEDRV_H

#include <cstdint>

enum class FadeType : int32_t
{
    kNone = 0,
    kBlackMarioHeadFadeIn,
    kBlackMarioHeadFadeOut,
    kBlackCircleFadeIn,
    kBlackCircleFadeOut,
    kBlackCircleFadeIn2,
    kBlackCircleFadeOut2,
    kBlackCircleFadeIn3,
    kBlackCircleFadeOut3,
    kBlackFadeIn,
    kBlackFadeOut,
    kBlackFadeIn2,
    kBlackFadeOut2,
    kBlackStatic,
    kBlackFadeIn3,
    kBlackFadeOut3,
    kWhiteFadeIn,
    kWhiteFadeOut,
    kWhiteFadeIn2,
    kWhiteFadeOut2,
    kWhiteFadeIn3,
    kWhiteFadeOut3,
    kCurtainPullDown,
    kCurtainPullUp,
    kCurtainPullLeft,
    kCurtainPullRight,
    kCurtainStatic,
    kCurtainPullLeft2,
    kCurtainPullRight2,
    kTitleScreenCurtainOpen,
    kTitleScreenCurtainClose,
    kTitleScreenCurtainOpenStatic,
    kTitleScreenCurtainClosedStatic,
    kTitleScreenCurtainLeaveGradual,
    kTitleScreenCurtainAppearGradual,
    kTitleScreenCurtainOpen2,
    kTitleScreenCurtainClosed2,
    kTitleScreenCurtainOpenStatic2,
    kTitleScreenCurtainClosedStatic2,
    kTitleScreenCurtainLeaveGradual2,
    kTitleScreenCurtainAppearGradual2,
    kEnterPipeFromUp,
    kExitPipeFromUp,
    kEnterPipeFromDown,
    kExitPipeFromDown,
    kEnterPipeFromLeft,
    kExitPipeFromLeft,
    kEnterPipeFromRight,
    kExitPipeFromRight,
    kFreezeScreen,
    kUnfreezeScreenPullTopRight,
    kDemoTitleFadeOut,
    kMarioHeadFadeIn = 54,
    kMarioHeadFadeOut,
    kPeachHeadFadeIn,
    kPeachHeadFadeOut,
    kBowserHeadFadeIn,
    kBowserHeadFadeOut,
    kMarioHeadStaticBlinking,
    kPeachHeadStatic,
    kBowserHeadStatic,
    kCurtainPullLeft3,
    kCurtainPullRight3,
};

struct FadeEntry
{
    uint16_t flags;
    uint8_t unk_2[2];
    FadeType fadeType;
    uint8_t unk_8[160];
} __attribute__((__packed__));

struct FadeWork
{
    uint8_t unk_0[8];
    FadeEntry entry[5];
    uint8_t unk_350[16];
} __attribute__((__packed__));

static_assert(sizeof(FadeEntry) == 0xA8);
static_assert(sizeof(FadeWork) == 0x360);

extern "C"
{
    extern FadeWork *_fadeWorkPtr; // wp

    // fadeSetNarrowFast
    // fadeGetTpl
    // fadeReset
    // fadeIsFinish
    // disp_dummy_black
    // disp_tec
    // disp_maku
    // zFill
    // z
    // disp_texture
    // fadeMain
    // fadeSetOffscreenCallback
    // fadeSetAnimVirtualPos
    // fadeSetAnimOfsPos
    // fadeSetSpotPos
    // fadeWhiteOutOff
    // fadeWhiteOutOn
    // fadeSoftFocusOff
    // fadeSoftFocusOn
    // fadeTecSoftOff
    // fadeTecSoftOn
    // fadeTecOff
    // fadeTecOn
    void fadeEntry(FadeType type, int32_t duration, uint8_t color[4]);
    // fadeTexSetup
    // fadeInit
    // _callback
}

#endif
