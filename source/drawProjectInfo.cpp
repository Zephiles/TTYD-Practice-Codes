#include "mod.h"
#include "drawText.h"
#include "classes/window.h"
#include "misc/functionHooks.h"
#include "misc/utils.h"
#include "ttyd/camdrv.h"
#include "ttyd/seqdrv.h"
#include "ttyd/seq_title.h"

#include <cstdint>

static void drawTitleScreenInfo(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Initialize the window based on the text
    const char *text = "Practice Codes " VERSION_STRING "\nCreated by Zephiles";
    constexpr float scale = 0.75f;
    Window window;

    window.setWidthHeightFromTextAndInit(text, scale, getColorBlack(0xE5), 10.f, 20.f);

    // Set the proper posX, posY, and width
    float windowPosY = -20.5f;

#ifdef TTYD_JP
    windowPosY += 30.f;
#endif

    window.setPosX(-225.f);
    window.setPosY(windowPosY);
    window.setWidth(450.f);

    // Draw the window
    window.draw();

    // Initialize text drawing
    drawTextInit(false);

    // Draw the text
    float posX;
    float posY;
    window.getTextPosXY(text, WindowAlignment::MIDDLE_CENTER, scale, &posX, &posY);
    drawText(text, posX, posY, scale, getColorWhite(0xFF));
}

static void drawFileSelectScreenInfo(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Initialize the window based on the text
    const char *text =
        "Press L + Start to open the menu\n\nFor more info, go to the main "
        "page:\nhttps://github.com/Zephiles/TTYD-Practice-Codes";

    constexpr float scale = 0.6f;
    Window window;

    window.setWidthHeightFromTextAndInit(text, scale, getColorBlack(0xE5), 10.f, 20.f);

    // Set the proper posX, posY, and width
    window.setPosX(-225.f);
    window.setPosY(-30.f);
    window.setWidth(450.f);

    // Draw the window
    window.draw();

    // Initialize text drawing
    drawTextInit(false);

    // Draw the text
    float posX;
    float posY;
    window.getTextPosXY(text, WindowAlignment::MIDDLE_CENTER, scale, &posX, &posY);
    drawText(text, posX, posY, scale, getColorWhite(0xFF));
}

void displayTitleScreenInfo(SeqWork *seqWork)
{
    // Check to see if the title screen is ready
    // Check to see if the curtain is fully displayed or not
    const int32_t state = seqTitleWorkPtr2->field_0x8;
    if ((state >= 2) && (state < 12))
    {
        drawOn2DLayer(drawTitleScreenInfo, DRAW_ORDER_PROJECT_INFO);
    }

    // Call the original function
    return g_titleMain_trampoline(seqWork);
}

void displayFileSelectScreenInfo(SeqWork *seqWork)
{
    // Check to see if the curtain is down or not
    if (seqWork->state == 2)
    {
        drawOn2DLayer(drawFileSelectScreenInfo, DRAW_ORDER_PROJECT_INFO);
    }

    // Call the original function
    return g_seq_loadMain_trampoline(seqWork);
}
