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

static void controls(Menu *menuPtr, MenuButtonInput button);
static void draw(CameraId cameraId, void *user);
static void selectedOptionTurnOnOff(Menu *menuPtr);
static void selectedOptionChangePosition(Menu *menuPtr);
static void selectedOptionChangeScale(Menu *menuPtr);

static const MenuOption gOptions[] {
    "Turn On/Off",
    selectedOptionTurnOnOff,

    "Change Position",
    selectedOptionChangePosition,

    "Change Scale",
    selectedOptionChangeScale,
};

static const MenuFunctions gFuncs = {
    gOptions,
    controls,
    draw,
    nullptr, // Exit function not needed
};

static DisplayManuallyPosition *getSelectedDisplayManuallyPositionPtr()
{
    const uint32_t displayManuallyPositionFlag = indexToDisplayManuallyPositionFlag(gDisplaysMenu->getSelectedDisplay());
    return gDisplays->getDisplayManuallyPositionPtr(displayManuallyPositionFlag);
}

void displaysAdjustManualPositionInit(Menu *menuPtr)
{
    (void)menuPtr;

    constexpr uint32_t totalOptions = sizeof(gOptions) / sizeof(MenuOption);
    enterNextMenu(&gFuncs, totalOptions);
}

static void controls(Menu *menuPtr, MenuButtonInput button)
{
    DisplaysMenu *displaysMenuPtr = gDisplaysMenu;

    // If the position editor is open, then handle the controls for that
    PositionEditor *positionEditorPtr = displaysMenuPtr->getPositionEditorPtr();
    if (positionEditorPtr->shouldDraw())
    {
        positionEditorPtr->controls(button);
        return;
    }

    // If the value editor is open, then handle the controls for that
    ValueEditor *valueEditorPtr = displaysMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->controls(button);
        return;
    }

    // Use the default controls
    basicMenuLayoutControls(menuPtr, button);
}

static void drawDisplayManualPositionData()
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
    const uint32_t selectedDisplay = gDisplaysMenu->getSelectedDisplay();
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

    char buf[128];
    snprintf(buf,
             sizeof(buf),
             " = %" PRId32 "\n = %" PRId32 "\n = %.6f",
             floatToInt(manuallyPositionPtr->getPosX()),
             floatToInt(manuallyPositionPtr->getPosY()),
             manuallyPositionPtr->getScale());

    // Get the text position a bit to the right of the main text
    const float width = getTextWidth(mainText, scale);

    drawText(buf, posX + width, posY, scale, getColorWhite(0xFF));
}

static void draw(CameraId cameraId, void *user)
{
    if (!gMod->flagIsSet(ModFlag::MOD_FLAG_MENU_IS_HIDDEN))
    {
        // Draw the main window and text
        basicMenuLayoutDraw(cameraId, user);

        // Draw the info for the display's manual position data
        drawDisplayManualPositionData();
    }

    // Draw the position editor if applicable
    DisplaysMenu *displaysMenuPtr = gDisplaysMenu;
    PositionEditor *positionEditorPtr = displaysMenuPtr->getPositionEditorPtr();
    if (positionEditorPtr->shouldDraw())
    {
        positionEditorPtr->draw();
    }

    // Draw the value editor if applicable
    ValueEditor *valueEditorPtr = displaysMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->draw();
    }
}

static void selectedOptionTurnOnOff(Menu *menuPtr)
{
    (void)menuPtr;

    const uint32_t displayManuallyPositionFlag = indexToDisplayManuallyPositionFlag(gDisplaysMenu->getSelectedDisplay());
    gDisplays->toggleManuallyPositionFlag(displayManuallyPositionFlag);
}

static void cancelChangePosition()
{
    gDisplaysMenu->getPositionEditorPtr()->stopDrawing();
    gMod->clearFlag(ModFlag::MOD_FLAG_MENU_IS_HIDDEN);
}

static bool confirmChangePosition()
{
    // Close the position editor
    cancelChangePosition();

    return true;
}

static void selectedOptionChangePosition(Menu *menuPtr)
{
    (void)menuPtr;

    // Make sure the pointer to the current display's manually position data is properly retrieved before doing anything
    DisplayManuallyPosition *manuallyPositionPtr = getSelectedDisplayManuallyPositionPtr();
    if (!manuallyPositionPtr)
    {
        return;
    }

    gMod->setFlag(ModFlag::MOD_FLAG_MENU_IS_HIDDEN);

    // Initialize the position editor
    DisplaysMenu *displaysMenuPtr = gDisplaysMenu;
    PositionEditor *positionEditorPtr = displaysMenuPtr->getPositionEditorPtr();

    const Window *rootWindowPtr = gRootWindow;

    positionEditorPtr->init(rootWindowPtr,
                            manuallyPositionPtr->getPosXPtr(),
                            manuallyPositionPtr->getPosYPtr(),
                            rootWindowPtr->getAlpha());

    positionEditorPtr->startDrawing(confirmChangePosition, cancelChangePosition);
}

static void cancelSetScale()
{
    gDisplaysMenu->getValueEditorPtr()->stopDrawing();
    gMod->clearFlag(ModFlag::MOD_FLAG_MENU_IS_HIDDEN);
}

static void setScale(const ValueType *valuePtr)
{
    DisplayManuallyPosition *manuallyPositionPtr = getSelectedDisplayManuallyPositionPtr();
    if (manuallyPositionPtr)
    {
        manuallyPositionPtr->setScale(valuePtr->f32);
    }

    // Close the value editor
    cancelSetScale();
}

static void selectedOptionChangeScale(Menu *menuPtr)
{
    (void)menuPtr;

    // Make sure the pointer to the current display's manually position data is properly retrieved before doing anything
    DisplayManuallyPosition *manuallyPositionPtr = getSelectedDisplayManuallyPositionPtr();
    if (!manuallyPositionPtr)
    {
        return;
    }

    gMod->setFlag(ModFlag::MOD_FLAG_MENU_IS_HIDDEN);

    // Initialize the value editor
    DisplaysMenu *displaysMenuPtr = gDisplaysMenu;
    ValueEditor *valueEditorPtr = displaysMenuPtr->getValueEditorPtr();

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

    valueEditorPtr->startDrawing(setScale, cancelSetScale);
}
