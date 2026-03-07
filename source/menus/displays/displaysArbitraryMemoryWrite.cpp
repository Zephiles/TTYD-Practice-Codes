#ifdef TTYD_JP

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
static void selectedOptionAdjustXAndZCoordinatesColor(Menu *menuPtr);
static void selectedOptionAdjustXNautPosition(Menu *menuPtr);

static const MenuOption gOptions[] {
    "Turn On/Off",
    selectedOptionTurnOnOff,

    "Adjust X and Z Coordinates' Text Color",
    selectedOptionAdjustXAndZCoordinatesColor,

    "Adjust X-Naut's Position",
    selectedOptionAdjustXNautPosition,

    "Adjust Manual Positioning",
    displaysAdjustManualPositionInit,

    "Change Button Combo",
    displaysGenericChangeButtonCombo,
};

static const MenuFunctions gFuncs = {
    gOptions,
    displaysMenuDefaultControlsWithButtonComboEditor,
    draw,
    nullptr, // Exit function not needed
};

void displaysArbitraryMemoryWriteInit(Menu *menuPtr)
{
    // Backup the selected display
    gDisplaysMenu->setSelectedDisplay(menuPtr->getCurrentIndex());

    constexpr uint32_t totalOptions = sizeof(gOptions) / sizeof(MenuOption);
    menuPtr = enterNextMenu(&gFuncs, totalOptions);

    // This display has a button combo, so set the menu flag for it
    menuPtr->setFlag(DisplaysMenuGenericFlags::DISPLAYS_GENERIC_FLAG_DISPLAY_HAS_BUTTON_COMBO);
}

static void drawArbitraryMemoryWriteExtraInfo()
{
    // Get the text position for the top-left of the window eight lines under the main text to account for the main info already
    // drawn
    const uint32_t totalOptions = gMenu->getTotalOptions();
    constexpr float scale = MENU_SCALE;

    float tempPosX;
    float tempPosY;
    gRootWindow->getTextPosXYUnderMainText(nullptr, WindowAlignment::TOP_LEFT, totalOptions, 8, scale, &tempPosX, &tempPosY);

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    const float posX = tempPosX;
    float posY = tempPosY;

    // Draw the Adjust X and Z Coordinates' Color text
    drawText("Adjust X and Z Coordinates' Text Color", posX, posY, scale, getColorWhite(0xFF));
    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    posY -= lineDecrement;

    // Draw whether the X and Z Coordinates' color should be adjusted
    const Displays *displaysPtr = gDisplays;

    const bool changeZCoordinateColor = displaysPtr->enabledFlagIsSet(
        DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_ABITRARY_MEMORY_WRITE_ADJUST_X_AND_Z_COORDINATES_COLOR);

    uint32_t color;
    const char *onOffText;

    getYesNoTextAndColor(changeZCoordinateColor, &onOffText, &color, 0xFF);
    drawText(onOffText, posX, posY, scale, color);
    posY -= (lineDecrement * 2.f);

    // Draw the Adjust X-Naut's Position text
    drawText("Adjust X-Naut's Position", posX, posY, scale, getColorWhite(0xFF));
    posY -= lineDecrement;

    // Draw whether the X-Naut's position should be adjusted
    const bool adjustXNautPosition =
        displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_ABITRARY_MEMORY_WRITE_ADJUST_XNAUT_POSITION);

    getYesNoTextAndColor(adjustXNautPosition, &onOffText, &color, 0xFF);
    drawText(onOffText, posX, posY, scale, color);
}

static void draw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDraw(cameraId, user);

    // Draw the main info for the Arbitrary Memory Write display
    drawGenericDisplayInfo();

    // Draw the extra info for the Arbitrary Memory Write display
    drawArbitraryMemoryWriteExtraInfo();

    // Draw the button combo editor if applicable
    ButtonComboEditor *buttonComboEditorPtr = gDisplaysMenu->getButtonComboEditorPtr();
    if (buttonComboEditorPtr->shouldDraw())
    {
        buttonComboEditorPtr->draw();
    }
}

static void selectedOptionTurnOnOff(Menu *menuPtr)
{
    (void)menuPtr;

    displaysMenuToggleEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_ABITRARY_MEMORY_WRITE);
}

static void selectedOptionAdjustXAndZCoordinatesColor(Menu *menuPtr)
{
    (void)menuPtr;

    displaysMenuToggleEnabledFlag(
        DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_ABITRARY_MEMORY_WRITE_ADJUST_X_AND_Z_COORDINATES_COLOR);
}

static void selectedOptionAdjustXNautPosition(Menu *menuPtr)
{
    (void)menuPtr;

    displaysMenuToggleEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_ABITRARY_MEMORY_WRITE_ADJUST_XNAUT_POSITION);
}

#endif
