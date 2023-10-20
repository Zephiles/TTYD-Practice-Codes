#include "menuUtils.h"
#include "drawText.h"
#include "cheats.h"
#include "classes/window.h"
#include "menus/cheatsMenu.h"
#include "menus/rootMenu.h"
#include "ttyd/camdrv.h"
#include "ttyd/pmario_sound.h"

#include <cstdint>

static void draw(CameraId cameraId, void *user);
static void selectedOptionPauseMenuZMenuSounds(Menu *menuPtr);
static void selectedOptionBackgroundMusic(Menu *menuPtr);
static void selectedOptionEnvironmentSounds(Menu *menuPtr);

static const MenuOption gOptions[] = {
    "Pause Menu/Z Menu Sounds",
    selectedOptionPauseMenuZMenuSounds,

    "Background Music",
    selectedOptionBackgroundMusic,

    "Environment Sounds",
    selectedOptionEnvironmentSounds,
};

static const MenuFunctions gFuncs = {
    gOptions,
    basicMenuLayoutControls,
    draw,
    nullptr, // Exit function not needed
};

void cheatsMenuDisableCertainSoundsInit(Menu *menuPtr)
{
    (void)menuPtr;

    constexpr uint32_t totalOptions = sizeof(gOptions) / sizeof(MenuOption);
    enterNextMenu(&gFuncs, totalOptions);
}

void CheatsMenu::drawDisableCertainSoundsInfo() const
{
    // Get the text position for the top-left of the window two lines under the main text
    const uint32_t totalOptions = gMenu->getTotalOptions();
    constexpr float scale = MENU_SCALE;

    float tempPosX;
    float tempPosY;
    gRootWindow->getTextPosXYUnderMainText(nullptr, WindowAlignment::TOP_LEFT, totalOptions, 2, scale, &tempPosX, &tempPosY);

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    const float posX = tempPosX;
    float posY = tempPosY;

    // Draw each option with its flag
    const MenuOption *optionsPtr = gOptions;
    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    Cheats *cheatsPtr = gCheats;

    const char *string;
    uint32_t color;

    for (uint32_t i = 0; i < totalOptions; i++)
    {
        drawText(optionsPtr[i].name, posX, posY, scale, getColorWhite(0xFF));
        posY -= lineDecrement;

        getOnOffTextAndColor(
            cheatsPtr->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_DISABLE_SOUNDS_PAUSE_MENU_Z_MENU + i),
            &string,
            &color,
            0xFF);

        drawText(string, posX, posY, scale, color);
        posY -= (lineDecrement * 2.f);
    }
}

static void draw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDraw(cameraId, user);

    // Draw the info for the sounds
    gCheatsMenu->drawDisableCertainSoundsInfo();
}

static void selectedOptionPauseMenuZMenuSounds(Menu *menuPtr)
{
    (void)menuPtr;

    cheatsMenuToggleEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_DISABLE_SOUNDS_PAUSE_MENU_Z_MENU);
}

static void selectedOptionBackgroundMusic(Menu *menuPtr)
{
    (void)menuPtr;

    if (cheatsMenuToggleEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_DISABLE_SOUNDS_BACKGROUND_MUSIC))
    {
        // Turn off the current background music
        psndBGMOff(0x200);
        psndBGMOff(0x201);
    }
}

static void selectedOptionEnvironmentSounds(Menu *menuPtr)
{
    (void)menuPtr;

    if (cheatsMenuToggleEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_DISABLE_SOUNDS_ENVIRONMENT_SOUNDS))
    {
        // Turn off the current environment sounds
        psndENVOff(0x200);
        psndENVOff(0x201);
    }
}
