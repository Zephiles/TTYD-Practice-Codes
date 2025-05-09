#include "menuUtils.h"
#include "drawText.h"
#include "classes/window.h"
#include "menus/cheatsMenu.h"
#include "menus/rootMenu.h"
#include "ttyd/fadedrv.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

static void draw(CameraId cameraId, void *user);
static void selectedOptionResolveFade(Menu *menuPtr);

static const MenuOption gOptions[] = {
    "Slot 1:",
    selectedOptionResolveFade,

    "Slot 2:",
    selectedOptionResolveFade,

    "Slot 3:",
    selectedOptionResolveFade,

    "Slot 4:",
    selectedOptionResolveFade,

    "Slot 5:",
    selectedOptionResolveFade,
};

static const MenuFunctions gFuncs = {
    gOptions,
    basicMenuLayoutControls,
    draw,
    nullptr, // Exit function not needed
};

void cheatsMenuResolveFadesInit(Menu *menuPtr)
{
    (void)menuPtr;

    constexpr uint32_t totalOptions = sizeof(gOptions) / sizeof(MenuOption);
    enterNextMenu(&gFuncs, totalOptions);
}

static void drawResolveFadesInfo(float offsetY)
{
    // Get the text position for the top-left of the window
    float tempPosX;
    float tempPosY;
    constexpr float scale = MENU_SCALE;
    gRootWindow->getTextPosXY(nullptr, WindowAlignment::TOP_LEFT, scale, &tempPosX, &tempPosY);

    // Draw the help text
    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    const float posXBase = tempPosX;
    const float posYBase = tempPosY;
    drawText("Select a fade to resolve, if applicable.", posXBase, posYBase, scale, getColorWhite(0xFF));

    // Get the width that is being used by the main text options
    const float textWidth = getTextWidth("Slot 4:", scale);

    // Set the text to be a bit to the right of the main text
    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    constexpr float textIncrement = 15.f * scale;
    const float posX = posXBase + textWidth + textIncrement;
    float posY = posYBase - offsetY;

    char buf[16];
    const FadeWork *fadeWorkPtr = _fadeWorkPtr;
    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    constexpr uint32_t totalEntries = sizeof(FadeWork::entry) / sizeof(FadeEntry);

    for (uint32_t i = 0; i < totalEntries; i++)
    {
        // Check if the current fade is active
        const FadeEntry *entry = &fadeWorkPtr->entry[i];

        if (entry->flags & 1U)
        {
            // Fade is active
            snprintf(buf, sizeof(buf), "%" PRId32, static_cast<int32_t>(entry->fadeType));
            drawText(buf, posX, posY, scale, getColorWhite(0xFF));
        }
        else
        {
            // Fade is not active
            drawText("None", posX, posY, scale, getColorGrayedOut(0xFF));
        }

        posY -= lineDecrement;
    }
}

static void draw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    // Help text will be drawn at the top-left of the window, so draw the main text two lines under it
    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * MENU_SCALE;
    constexpr float offsetY = lineDecrement * 2.f;

    basicMenuLayoutDraw(cameraId, user, LINE_HEIGHT_FLOAT, 0.f, offsetY);

    // Draw the info for the fades
    drawResolveFadesInfo(offsetY);

    // Draw the error window if applicable
    ErrorWindow *errorWindowPtr = gCheatsMenu->getErrorWindowPtr();
    if (errorWindowPtr->shouldDraw())
    {
        errorWindowPtr->draw();
    }
}

static void selectedOptionResolveFade(Menu *menuPtr)
{
    const char *errorMessage;

    switch (resolveFade(menuPtr->getCurrentIndex()))
    {
        case ResolveFadeReturnValue::RESOLVE_FADE_RETURN_TYPE_INVALID_INDEX:
        {
            // Index is somehow invalid, so reset it
            menuPtr->setCurrentIndex(0);
            return;
        }
        case ResolveFadeReturnValue::RESOLVE_FADE_RETURN_TYPE_NOT_ACTIVE:
        {
            errorMessage = "The selected fade is not active.";
            break;
        }
        case ResolveFadeReturnValue::RESOLVE_FADE_RETURN_TYPE_DONT_RESOLVE:
        {
            errorMessage = "The selected fade does not need to be resolved.";
            break;
        }
        case ResolveFadeReturnValue::RESOLVE_FADE_RETURN_TYPE_SUCCESS:
        {
            errorMessage = "The selected fade was successfully resolved.";
            break;
        }
        default:
        {
            return;
        }
    }

    // Initialize the error window
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;
    ErrorWindow *errorWindowPtr = cheatsMenuPtr->getErrorWindowPtr();
    const Window *rootWindowPtr = gRootWindow;

    errorWindowPtr->setAlpha(rootWindowPtr->getAlpha());
    errorWindowPtr->setText(errorMessage);

    errorWindowPtr->setTimer(3000);
    errorWindowPtr->placeInWindow(rootWindowPtr, WindowAlignment::MIDDLE_CENTER);
}

ResolveFadeReturnValue resolveFade(uint32_t index)
{
    // Make sure the index is valid
    constexpr uint32_t totalEntries = sizeof(FadeWork::entry) / sizeof(FadeEntry);
    if (index >= totalEntries)
    {
        return ResolveFadeReturnValue::RESOLVE_FADE_RETURN_TYPE_INVALID_INDEX;
    }

    // Check if the current fade is active
    FadeEntry *entryPtr = &_fadeWorkPtr->entry[index];
    if (!(entryPtr->flags & 1U))
    {
        // Fade is not active
        return ResolveFadeReturnValue::RESOLVE_FADE_RETURN_TYPE_NOT_ACTIVE;
    }

    // Check if the selected fade needs to be resolved
    FadeType type = entryPtr->fadeType;

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
