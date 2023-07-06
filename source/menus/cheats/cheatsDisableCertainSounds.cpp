#include "menuUtils.h"
#include "drawText.h"
#include "cheats.h"
#include "classes/window.h"
#include "menus/cheatsMenu.h"
#include "menus/rootMenu.h"
#include "misc/utils.h"
#include "ttyd/camdrv.h"
#include "ttyd/pmario_sound.h"

#include <cstdint>

const MenuOption gCheatsMenuDisableCertainSoundsOptions[] = {
    "Pause Menu/Z Menu Sounds",
    cheatsMenuTogglePauseMenuZMenuSounds,

    "Background Music",
    cheatsMenuToggleBackgroundMusic,

    "Environment Sounds",
    cheatsMenuToggleEnvironmentSounds,
};

const MenuFunctions gCheatsDisableCertainSoundsFuncs = {
    gCheatsMenuDisableCertainSoundsOptions,
    controlsBasicMenuLayout,
    cheatsMenuDisableCertainSoundsDraw,
    nullptr, // Exit function not needed
};

void cheatsMenuDisableCertainSoundsInit(Menu *menuPtr)
{
    (void)menuPtr;

    constexpr uint32_t totalOptions = sizeof(gCheatsMenuDisableCertainSoundsOptions) / sizeof(MenuOption);
    enterNextMenu(&gCheatsDisableCertainSoundsFuncs, totalOptions);
}

void CheatsMenu::drawDisableCertainSoundsInfo() const
{
    // Get the text position for the top-left of the window
    float tempPosX;
    float tempPosY;
    gRootWindow->getTextPosXY(nullptr, WindowAlignment::TOP_LEFT, scale, &tempPosX, &tempPosY);

    // Position the text two lines under the main text
    const uint32_t totalOptions = gMenu->getTotalOptions();
    const float scale = this->scale;
    const float lineDecrement = LINE_HEIGHT_FLOAT * scale;

    const float posX = tempPosX;
    float posY = tempPosY - (intToFloat(totalOptions + 1) * lineDecrement);

    // Draw each option with its flag
    const MenuOption *disableCertainSoundsOptionsPtr = gCheatsMenuDisableCertainSoundsOptions;
    Cheats *cheatsPtr = gCheats;
    const char *string;
    uint32_t color;

    for (uint32_t i = 0; i < totalOptions; i++)
    {
        drawText(disableCertainSoundsOptionsPtr[i].name, posX, posY, scale, getColorWhite(0xFF));
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

void cheatsMenuDisableCertainSoundsDraw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    drawBasicMenuLayout(cameraId, user);

    // Draw the info for the sounds
    gCheatsMenu->drawDisableCertainSoundsInfo();
}

void cheatsMenuTogglePauseMenuZMenuSounds(Menu *menuPtr)
{
    (void)menuPtr;

    cheatsMenuToggleEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_DISABLE_SOUNDS_PAUSE_MENU_Z_MENU);
}

void cheatsMenuToggleBackgroundMusic(Menu *menuPtr)
{
    (void)menuPtr;

    if (cheatsMenuToggleEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_DISABLE_SOUNDS_BACKGROUND_MUSIC))
    {
        // Turn off the current background music
        psndBGMOff(0x200);
        psndBGMOff(0x201);
    }
}

void cheatsMenuToggleEnvironmentSounds(Menu *menuPtr)
{
    (void)menuPtr;

    if (cheatsMenuToggleEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_DISABLE_SOUNDS_ENVIRONMENT_SOUNDS))
    {
        // Turn off the current environment sounds
        psndENVOff(0x200);
        psndENVOff(0x201);
    }
}
