#pragma once

#include <cstdint>

namespace ttyd::fadedrv {

enum class FadeType : int32_t
{
	kNone = 								0,
	kBlackMarioHeadFadeIn = 				1,
	kBlackMarioHeadFadeOut = 				2,
	kBlackCircleFadeIn = 					3,
	kBlackCircleFadeOut = 					4,
	kBlackCircleFadeIn2 = 					5,
	kBlackCircleFadeOut2 = 					6,
	kBlackCircleFadeIn3 = 					7,
	kBlackCircleFadeOut3 = 					8,
	kBlackFadeIn = 							9,
	kBlackFadeOut = 						10,
	kBlackFadeIn2 = 						11,
	kBlackFadeOut2 = 						12,
	kBlackStatic = 							13,
	kBlackFadeIn3 = 						14,
	kBlackFadeOut3 = 						15,
	kWhiteFadeIn = 							16,
	kWhiteFadeOut = 						17,
	kWhiteFadeIn2 = 						18,
	kWhiteFadeOut2 = 						19,
	kWhiteFadeIn3 = 						20,
	kWhiteFadeOut3 = 						21,
	kCurtainPullDown = 						22,
	kCurtainPullUp = 						23,
	kCurtainPullLeft = 						24,
	kCurtainPullRight = 					25,
	kCurtainStatic = 						26,
	kCurtainPullLeft2 = 					27,
	kCurtainPullRight2 = 					28,
	kTitleScreenCurtainOpen = 				29,
	kTitleScreenCurtainClose = 				30,
	kTitleScreenCurtainOpenStatic = 		31,
	kTitleScreenCurtainClosedStatic = 		32,
	kTitleScreenCurtainLeaveGradual = 		33,
	kTitleScreenCurtainAppearGradual = 		34,
	kTitleScreenCurtainOpen2 = 				35,
	kTitleScreenCurtainClosed2 = 			36,
	kTitleScreenCurtainOpenStatic2 = 		37,
	kTitleScreenCurtainClosedStatic2 = 		38,
	kTitleScreenCurtainLeaveGradual2 = 		39,
	kTitleScreenCurtainAppearGradual2 = 	40,
	kEnterPipeFromUp = 						41,
	kExitPipeFromUp = 						42,
	kEnterPipeFromDown = 					43,
	kExitPipeFromDown = 					44,
	kEnterPipeFromLeft = 					45,
	kExitPipeFromLeft = 					46,
	kEnterPipeFromRight = 					47,
	kExitPipeFromRight = 					48,
	kFreezeScreen = 						49,
	kUnfreezeScreenPullTopRight = 			50,
	kDemoTitleFadeOut = 					51,
	kMarioHeadFadeIn = 						54,
	kMarioHeadFadeOut = 					55,
	kPeachHeadFadeIn = 						56,
	kPeachHeadFadeOut = 					57,
	kBowserHeadFadeIn = 					58,
	kBowserHeadFadeOut = 					59,
	kMarioHeadStaticBlinking = 				60,
	kPeachHeadStatic = 						61,
	kBowserHeadStatic = 					62,
	kCurtainPullLeft3 = 					63,
	kCurtainPullRight3 = 					64,
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

extern "C" {

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

extern FadeWork *gpFadeWork;

}

}
