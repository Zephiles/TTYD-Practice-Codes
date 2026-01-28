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
static void selectedOptionAdjustZCoordinateColor(Menu *menuPtr);
static void selectedOptionAdjustXNautPosition(Menu *menuPtr);

static const MenuOption gOptions[] {
    "Turn On/Off",
    selectedOptionTurnOnOff,

    "Adjust Z Coordinate Text Color",
    selectedOptionAdjustZCoordinateColor,

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

void displaysMenuAMWSpinJumpGreatTreeInit(Menu *menuPtr)
{
    // Backup the selected display
    gDisplaysMenu->setSelectedDisplay(menuPtr->getCurrentIndex());

    constexpr uint32_t totalOptions = sizeof(gOptions) / sizeof(MenuOption);
    menuPtr = enterNextMenu(&gFuncs, totalOptions);

    // This display has a button combo, so set the menu flag for it
    menuPtr->setFlag(DisplaysMenuGenericFlags::DISPLAYS_GENERIC_FLAG_DISPLAY_HAS_BUTTON_COMBO);
}

static void drawAMWSpinJumpGreatTreeAdjustXNautPositionInfo()
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

    // Draw the Adjust Z Coordinate Color text
    drawText("Adjust Z Coordinate Text Color", posX, posY, scale, getColorWhite(0xFF));
    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    posY -= lineDecrement;

    // Draw whether the Z Coordinate's color should be adjusted
    const Displays *displaysPtr = gDisplays;

    const bool changeZCoordinateColor =
        displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_AMW_SPIN_JUMP_ADJUST_Z_COORDINATE_COLOR);

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
        displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_AMW_SPIN_JUMP_ADJUST_XNAUT_POSITION);

    getYesNoTextAndColor(adjustXNautPosition, &onOffText, &color, 0xFF);
    drawText(onOffText, posX, posY, scale, color);
}

static void draw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDraw(cameraId, user);

    // Draw the main info for the AMW Spin Jump display
    drawGenericDisplayInfo();

    // Draw whether or not the X-Naut's position should be adjusted
    drawAMWSpinJumpGreatTreeAdjustXNautPositionInfo();

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

    displaysMenuToggleEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_AMW_SPIN_JUMP);
}

static void selectedOptionAdjustZCoordinateColor(Menu *menuPtr)
{
    (void)menuPtr;

    displaysMenuToggleEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_AMW_SPIN_JUMP_ADJUST_Z_COORDINATE_COLOR);
}

static void selectedOptionAdjustXNautPosition(Menu *menuPtr)
{
    (void)menuPtr;

    displaysMenuToggleEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_AMW_SPIN_JUMP_ADJUST_XNAUT_POSITION);
}

#endif
