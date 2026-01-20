#include "menuUtils.h"
#include "drawText.h"
#include "displays.h"
#include "mod.h"
#include "classes/window.h"
#include "menus/displaysMenu.h"
#include "menus/rootMenu.h"
#include "ttyd/camdrv.h"

#include <cstdint>

static void draw(CameraId cameraId, void *user);
static void selectedOptionTurnOnOff(Menu *menuPtr);
static void selectedOptionShowAsHex(Menu *menuPtr);

static const MenuOption gOptions[] {
    "Turn On/Off",
    selectedOptionTurnOnOff,

    "Show As Hex",
    selectedOptionShowAsHex,
};

static const MenuFunctions gFuncs = {
    gOptions,
    basicMenuLayoutControls,
    draw,
    nullptr, // Exit function not needed
};

void displaysMenuMariosCoordinatesInit(Menu *menuPtr)
{
    // Backup the selected display
    gDisplaysMenu->setSelectedDisplay(menuPtr->getCurrentIndex());

    // Backup the selected display
    constexpr uint32_t totalOptions = sizeof(gOptions) / sizeof(MenuOption);
    enterNextMenu(&gFuncs, totalOptions);
}

static void drawMariosCoordinatesShowAsHexInfo()
{
    // Get the text position for the top-left of the window five lines under the main text to account for the main info already
    // drawn
    const uint32_t totalOptions = gMenu->getTotalOptions();
    constexpr float scale = MENU_SCALE;

    float tempPosX;
    float tempPosY;
    gRootWindow->getTextPosXYUnderMainText(nullptr, WindowAlignment::TOP_LEFT, totalOptions, 5, scale, &tempPosX, &tempPosY);

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    const float posX = tempPosX;
    float posY = tempPosY;

    // Draw the Show As Hex text
    drawText("Show As Hex", posX, posY, scale, getColorWhite(0xFF));
    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    posY -= lineDecrement;

    // Draw whether the coordinates are shown in hex
    const Displays *displaysPtr = gDisplays;

    const bool showAsHex =
        displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MARIO_COORDINATES_SHOW_AS_HEX);

    uint32_t color;
    const char *onOffText;
    getOnOffTextAndColor(showAsHex, &onOffText, &color, 0xFF);
    drawText(onOffText, posX, posY, scale, color);
}

static void draw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDraw(cameraId, user);

    // Draw the main info for Mario's Coordinates
    drawGenericDisplayInfo();

    // Draw whether or not Mario's Coordinates should be shown in hex
    drawMariosCoordinatesShowAsHexInfo();
}

static void selectedOptionTurnOnOff(Menu *menuPtr)
{
    (void)menuPtr;

    displaysMenuToggleEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MARIO_COORDINATES);
}

static void selectedOptionShowAsHex(Menu *menuPtr)
{
    (void)menuPtr;

    displaysMenuToggleEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MARIO_COORDINATES_SHOW_AS_HEX);
}
