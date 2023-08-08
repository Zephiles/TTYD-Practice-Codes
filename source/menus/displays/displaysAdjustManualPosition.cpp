#include "menuUtils.h"
#include "mod.h"
#include "drawText.h"
#include "displays.h"
#include "classes/window.h"
#include "menus/displaysMenu.h"
#include "menus/rootMenu.h"
#include "misc/utils.h"
#include "ttyd/camdrv.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

const MenuOption gDisplaysMenuAdjustManualPositionOptions[] {
    "Turn On/Off",
    displaysMenuAdjustManualPositionToggleFlag,

    "Change Position",
    displaysMenuAdjustManualPositionStartChangingPosition,

    "Change Scale",
    displaysMenuAdjustManualPositionStartChangingScale,
};

const MenuFunctions gDisplaysMenuAdjustManualPositionFuncs = {
    gDisplaysMenuAdjustManualPositionOptions,
    displaysMenuAdjustManualPositionControls,
    displaysMenuAdjustManualPositionDraw,
    nullptr, // Exit function not needed
};

DisplayManuallyPosition *getSelectedDisplayManuallyPositionPtr()
{
    const uint32_t displayManuallyPositionFlag = indexToDisplayManuallyPositionFlag(gDisplaysMenu->getSelectedDisplay());
    return gDisplays->getDisplayManuallyPositionPtr(displayManuallyPositionFlag);
}

void displaysAdjustManualPositionInit(Menu *menuPtr)
{
    (void)menuPtr;

    constexpr uint32_t totalOptions = sizeof(gDisplaysMenuAdjustManualPositionOptions) / sizeof(MenuOption);
    enterNextMenu(&gDisplaysMenuAdjustManualPositionFuncs, totalOptions);
}

void displaysMenuAdjustManualPositionControls(Menu *menuPtr, MenuButtonInput button)
{
    DisplaysMenu *displaysMenuPtr = gDisplaysMenu;

    // If the position editor is open, then handle the controls for that
    PositionEditor *positionEditorPtr;
    if (positionEditorPtr = displaysMenuPtr->getPositionEditor(), positionEditorPtr->shouldDraw())
    {
        positionEditorPtr->controls(button);
        return;
    }

    // If the value editor is open, then handle the controls for that
    ValueEditor *valueEditorPtr;
    if (valueEditorPtr = displaysMenuPtr->getValueEditor(), valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->controls(button);
        return;
    }

    // Use the default controls
    basicMenuLayoutControls(menuPtr, button);
}

void DisplaysMenu::drawDisplayManualPositionData() const
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

    // Draw the current display with its flag
    const uint32_t selectedDisplay = this->selectedDisplay;
    drawText(gDisplaysMenuInitOptions[selectedDisplay].name, posX, posY, scale, getColorWhite(0xFF));

    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    posY -= lineDecrement;

    const uint32_t displayManuallyPositionFlag = indexToDisplayManuallyPositionFlag(selectedDisplay);
    Displays *displaysPtr = gDisplays;
    const char *string;
    uint32_t color;

    getOnOffTextAndColor(displaysPtr->manuallyPositionFlagIsSet(displayManuallyPositionFlag), &string, &color, 0xFF);
    drawText(string, posX, posY, scale, color);
    posY -= (lineDecrement * 2.f);

    // Draw the current Pos X, Pos Y, and Scale
    const DisplayManuallyPosition *manuallyPositionPtr =
        displaysPtr->getDisplayManuallyPositionPtr(displayManuallyPositionFlag);

    if (!manuallyPositionPtr)
    {
        return;
    }

    const char *mainText = "Pos X\nPos Y\nScale";
    drawText(mainText, posX, posY, scale, getColorWhite(0xFF));

    // Get the text position a bit to the right of the main text
    float width;
    getTextWidthHeight(mainText, scale, &width, nullptr);

    char buf[128];
    snprintf(buf,
             sizeof(buf),
             " = %" PRId32 "\n = %" PRId32 "\n = %.6f",
             floatToInt(manuallyPositionPtr->getPosX()),
             floatToInt(manuallyPositionPtr->getPosY()),
             manuallyPositionPtr->getScale());

    drawText(buf, posX + width, posY, scale, getColorWhite(0xFF));
}

void displaysMenuAdjustManualPositionDraw(CameraId cameraId, void *user)
{
    DisplaysMenu *displaysMenuPtr = gDisplaysMenu;
    if (!gMod.menuIsHidden())
    {
        // Draw the main window and text
        basicMenuLayoutDraw(cameraId, user);

        // Draw the info for the display's manual position data
        displaysMenuPtr->drawDisplayManualPositionData();
    }

    // Draw the position editor if applicable
    PositionEditor *positionEditorPtr = displaysMenuPtr->getPositionEditor();
    if (positionEditorPtr->shouldDraw())
    {
        positionEditorPtr->draw();
    }

    // Draw the value editor if applicable
    ValueEditor *valueEditorPtr = displaysMenuPtr->getValueEditor();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->draw();
    }
}

void displaysMenuAdjustManualPositionToggleFlag(Menu *menuPtr)
{
    (void)menuPtr;

    const uint32_t displayManuallyPositionFlag = indexToDisplayManuallyPositionFlag(gDisplaysMenu->getSelectedDisplay());
    gDisplays->toggleManuallyPositionFlag(displayManuallyPositionFlag);
}

void displaysMenuAdjustManualPositionCancelChangingPosition()
{
    gDisplaysMenu->getPositionEditor()->stopDrawing();
    gMod.showMenu();
}

bool displaysMenuAdjustManualPositionConfirmChangingPosition()
{
    // Close the position editor
    displaysMenuAdjustManualPositionCancelChangingPosition();

    return true;
}

void displaysMenuAdjustManualPositionStartChangingPosition(Menu *menuPtr)
{
    (void)menuPtr;

    // Make sure the pointer to the current display's manually position data is properly retrieved before doing anything
    DisplayManuallyPosition *manuallyPositionPtr = getSelectedDisplayManuallyPositionPtr();
    if (!manuallyPositionPtr)
    {
        return;
    }

    gMod.hideMenu();

    // Initialize the position editor
    DisplaysMenu *displaysMenuPtr = gDisplaysMenu;
    PositionEditor *positionEditorPtr = displaysMenuPtr->getPositionEditor();

    const Window *rootWindowPtr = gRootWindow;

    positionEditorPtr->init(rootWindowPtr,
                            manuallyPositionPtr->getPosXPtr(),
                            manuallyPositionPtr->getPosYPtr(),
                            rootWindowPtr->getAlpha());

    positionEditorPtr->startDrawing(displaysMenuAdjustManualPositionConfirmChangingPosition,
                                    displaysMenuAdjustManualPositionCancelChangingPosition);
}

void displaysMenudjustManualPositionCancelSetNewScale()
{
    gDisplaysMenu->getValueEditor()->stopDrawing();
    gMod.showMenu();
}

void displaysMenuAdjustManualPositionSetNewScale(const ValueType *valuePtr)
{
    DisplayManuallyPosition *manuallyPositionPtr = getSelectedDisplayManuallyPositionPtr();
    if (manuallyPositionPtr)
    {
        manuallyPositionPtr->setScale(valuePtr->f32);
    }

    // Close the value editor
    displaysMenudjustManualPositionCancelSetNewScale();
}

void displaysMenuAdjustManualPositionStartChangingScale(Menu *menuPtr)
{
    (void)menuPtr;

    // Make sure the pointer to the current display's manually position data is properly retrieved before doing anything
    DisplayManuallyPosition *manuallyPositionPtr = getSelectedDisplayManuallyPositionPtr();
    if (!manuallyPositionPtr)
    {
        return;
    }

    gMod.hideMenu();

    // Initialize the value editor
    DisplaysMenu *displaysMenuPtr = gDisplaysMenu;
    ValueEditor *valueEditorPtr = displaysMenuPtr->getValueEditor();

    uint32_t flags = 0;
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::WINDOW_POSITION_TOP);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Y_SET_MAX);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Z_SET_MIN);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_X_HIDE);

    const float minValue = 0.3f;
    const float maxValue = 1.5f;
    const Window *rootWindowPtr = gRootWindow;

    valueEditorPtr->init(manuallyPositionPtr->getScalePtr(),
                         &minValue,
                         &maxValue,
                         rootWindowPtr,
                         flags,
                         VariableType::f32,
                         rootWindowPtr->getAlpha());

    valueEditorPtr->startDrawing(displaysMenuAdjustManualPositionSetNewScale, displaysMenudjustManualPositionCancelSetNewScale);
}
