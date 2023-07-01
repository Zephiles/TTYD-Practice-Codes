#include "drawText.h"
#include "menus/cheatsMenu.h"
#include "ttyd/fadedrv.h"

#include <cstdint>

ResolveFadeReturnValue resolveFade(uint32_t index)
{
    // Make sure the index is valid
    constexpr uint32_t totalEntries = sizeof(FadeWork::entry) / sizeof(FadeEntry);
    if (index >= totalEntries)
    {
        return ResolveFadeReturnValue::RESOLVE_FADE_RETURN_TYPE_INVALID_INDEX;
    }

    // Check if the current fade is active
    FadeEntry *entry = &fadeWorkPtr->entry[index];
    if (!(entry->flags & 1U))
    {
        return ResolveFadeReturnValue::RESOLVE_FADE_RETURN_TYPE_NOT_ACTIVE;
    }

    // Check if the selected fade needs to be resolved
    FadeType type = entry->fadeType;

    switch (type)
    {
        case FadeType::kBlackMarioHeadFadeOut:
        {
            type = FadeType::kBlackMarioHeadFadeIn;
            break;
        }
        case FadeType::kBlackCircleFadeOut:
        {
            type = FadeType::kBlackCircleFadeIn;
            break;
        }
        case FadeType::kBlackCircleFadeOut2:
        {
            type = FadeType::kBlackCircleFadeIn2;
            break;
        }
        case FadeType::kBlackCircleFadeOut3:
        {
            type = FadeType::kBlackCircleFadeIn3;
            break;
        }
        case FadeType::kBlackFadeOut:
        {
            type = FadeType::kBlackFadeIn;
            break;
        }
        case FadeType::kBlackFadeOut2:
        {
            type = FadeType::kBlackFadeIn2;
            break;
        }
        case FadeType::kBlackStatic:
        case FadeType::kBlackFadeOut3:
        {
            type = FadeType::kBlackFadeIn3;
            break;
        }
        case FadeType::kWhiteFadeOut:
        {
            type = FadeType::kWhiteFadeIn;
            break;
        }
        case FadeType::kWhiteFadeOut2:
        {
            type = FadeType::kWhiteFadeIn2;
            break;
        }
        case FadeType::kWhiteFadeOut3:
        {
            type = FadeType::kWhiteFadeIn3;
            break;
        }
        case FadeType::kCurtainPullDown:
        {
            type = FadeType::kCurtainPullUp;
            break;
        }
        case FadeType::kCurtainPullLeft:
        {
            type = FadeType::kCurtainPullRight;
            break;
        }
        case FadeType::kCurtainStatic:
        case FadeType::kCurtainPullLeft2:
        {
            type = FadeType::kCurtainPullRight2;
            break;
        }
        case FadeType::kTitleScreenCurtainClose:
        case FadeType::kTitleScreenCurtainClosedStatic:
        {
            type = FadeType::kTitleScreenCurtainOpen;
            break;
        }
        case FadeType::kTitleScreenCurtainOpen:
        case FadeType::kTitleScreenCurtainOpenStatic:
        case FadeType::kTitleScreenCurtainAppearGradual:
        {
            type = FadeType::kTitleScreenCurtainLeaveGradual;
            break;
        }
        case FadeType::kTitleScreenCurtainOpen2:
        case FadeType::kTitleScreenCurtainOpenStatic2:
        case FadeType::kTitleScreenCurtainAppearGradual2:
        {
            type = FadeType::kTitleScreenCurtainLeaveGradual2;
            break;
        }
        case FadeType::kTitleScreenCurtainClosed2:
        case FadeType::kTitleScreenCurtainClosedStatic2:
        {
            type = FadeType::kTitleScreenCurtainOpen2;
            break;
        }
        case FadeType::kEnterPipeFromUp:
        {
            type = FadeType::kExitPipeFromUp;
            break;
        }
        case FadeType::kEnterPipeFromDown:
        {
            type = FadeType::kExitPipeFromDown;
            break;
        }
        case FadeType::kEnterPipeFromLeft:
        {
            type = FadeType::kExitPipeFromLeft;
            break;
        }
        case FadeType::kEnterPipeFromRight:
        {
            type = FadeType::kExitPipeFromRight;
            break;
        }
        case FadeType::kFreezeScreen:
        {
            type = FadeType::kUnfreezeScreenPullTopRight;
            break;
        }
        case FadeType::kMarioHeadFadeOut:
        case FadeType::kMarioHeadStaticBlinking:
        {
            type = FadeType::kMarioHeadFadeIn;
            break;
        }
        case FadeType::kPeachHeadFadeOut:
        case FadeType::kPeachHeadStatic:
        {
            type = FadeType::kPeachHeadFadeIn;
            break;
        }
        case FadeType::kBowserHeadFadeOut:
        case FadeType::kBowserHeadStatic:
        {
            type = FadeType::kBowserHeadFadeIn;
            break;
        }
        case FadeType::kCurtainPullLeft3:
        {
            type = FadeType::kCurtainPullRight3;
            break;
        }
        default:
        {
            // The selected fade does not need to be resolved
            return ResolveFadeReturnValue::RESOLVE_FADE_RETURN_TYPE_DONT_RESOLVE;
        }
    }

    // Resolve the selected fade
    uint32_t color = getColorBlack(0xFF);
    fadeEntry(type, 0, reinterpret_cast<uint8_t *>(&color));

    return ResolveFadeReturnValue::RESOLVE_FADE_RETURN_TYPE_SUCCESS;
}
