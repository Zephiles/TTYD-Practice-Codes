#include "menuUtils.h"
#include "drawText.h"
#include "displays.h"
#include "mod.h"
#include "classes/window.h"
#include "classes/errorWindow.h"
#include "menus/displaysMenu.h"
#include "menus/rootMenu.h"
#include "ttyd/camdrv.h"
#include "ttyd/evtmgr.h"
#include "misc/utils.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

static void draw(CameraId cameraId, void *user);
static void controls(Menu *menuPtr, MenuButtonInput button);
static void selectedOptionTurnOnOff(Menu *menuPtr);
static void selectedOptionChangeValue(Menu *menuPtr);
static void selectedOptionToggleHandleAsHex(Menu *menuPtr);

static const MenuOption gOptions[] {
    "Turn On/Off",
    selectedOptionTurnOnOff,

    "Change Value",
    selectedOptionChangeValue,

    "Handle As Hex",
    selectedOptionToggleHandleAsHex,

    "Adjust Manual Positioning",
    displaysAdjustManualPositionInit,
};

static const MenuFunctions gFuncs = {
    gOptions,
    controls,
    draw,
    nullptr, // Exit function not needed
};

void displaysEvtIdVariableInit(Menu *menuPtr)
{
    // Backup the selected display
    gDisplaysMenu->setSelectedDisplay(menuPtr->getCurrentIndex());

    constexpr uint32_t totalOptions = sizeof(gOptions) / sizeof(MenuOption);
    menuPtr = enterNextMenu(&gFuncs, totalOptions);

    // This display has an option to be shown as hex, so set the menu flag for it
    menuPtr->setFlag(DisplaysMenuGenericFlags::DISPLAYS_GENERIC_FLAG_HANDLE_AS_HEX);
}

static void controls(Menu *menuPtr, MenuButtonInput button)
{
    DisplaysMenu *displaysMenuPtr = gDisplaysMenu;

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

void drawEvtIdVariableExtraInfo()
{
    // Get the text position for the top-left of the window eight lines under the main text to account for the main info already
    // drawn
    const uint32_t totalOptions = gMenu->getTotalOptions();
    constexpr float scale = MENU_SCALE;

    float tempPosX;
    float tempPosY;
    gRootWindow->getTextPosXYUnderMainText(nullptr, WindowAlignment::TOP_LEFT, totalOptions, 8, scale, &tempPosX, &tempPosY);

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    const float posX = tempPosX;
    float posY = tempPosY;

    // Draw the Current Value text
    drawText("Current Value", posX, posY, scale, getColorWhite(0xFF));
    posY -= lineDecrement;

    // Get the current evtID value
    const char *format;

    if (gDisplays->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_EVT_ID_VARIABLE_HANDLE_AS_HEX))
    {
        format = "0x%" PRIX32;
    }
    else
    {
        format = "%u";
    }

    char buf[32];
    snprintf(buf, sizeof(buf), format, static_cast<uint32_t>(evtID));
    drawText(buf, posX, posY, scale, getColorWhite(0xFF));
}

static void draw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDraw(cameraId, user);

    // Draw the main info for the evtID Variable display
    drawGenericDisplayInfo();

    // Draw the extra info for the evtID Variable display
    drawEvtIdVariableExtraInfo();

    DisplaysMenu *displaysMenuPtr = gDisplaysMenu;

    // Draw the value editor if applicable
    ValueEditor *valueEditorPtr = displaysMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->draw();
    }

    // Draw the error window if applicable
    ErrorWindow *errorWindowPtr = gDisplaysMenu->getErrorWindowPtr();
    if (errorWindowPtr->shouldDraw())
    {
        errorWindowPtr->draw();
    }
}

static void selectedOptionTurnOnOff(Menu *menuPtr)
{
    (void)menuPtr;

    displaysMenuToggleEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_EVT_ID_VARIABLE);
}

static void setEvtIdValue(const ValueType *valuePtr)
{
    // Set the new value in the Mod data, as the actual value will be updated on the next room transition/reload, and then set
    // the appropriate Mod flag for it
    Mod *modPtr = gMod;
    modPtr->setNewEvtIdValue(valuePtr->u32);
    modPtr->setFlag(ModFlag::MOD_FLAG_CHANGE_EVT_ID_VARIABLE);

    // Show the window stating that the change will occur on the next room transition/reload
    const char *text = "The evtID variable's value will\nbe updated on the next room\ntransition/reload.";
    gDisplaysMenu->initErrorWindow(text);

    // Close the value editor
    displaysMenuStopDrawingValueEditor();
}

static void selectedOptionChangeValue(Menu *menuPtr)
{
    (void)menuPtr;

    // Initialize the value editor
    uint32_t flags = 0;
    flags = ValueEditor::setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);
    flags = ValueEditor::setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Y_SET_MAX);
    flags = ValueEditor::setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Z_SET_MIN);

    if (gDisplays->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_EVT_ID_VARIABLE_HANDLE_AS_HEX))
    {
        flags = ValueEditor::setFlag(flags, ValueEditorFlag::HANDLE_AS_HEX);
    }

    uint32_t minValue = 0;
    uint32_t maxValue = 0xFFFFFFFF;
    const Window *rootWindowPtr = gRootWindow;
    uint32_t currentValue = static_cast<uint32_t>(evtID);
    ValueEditor *valueEditorPtr = gDisplaysMenu->getValueEditorPtr();

    valueEditorPtr
        ->init(&currentValue, &minValue, &maxValue, rootWindowPtr, flags, VariableType::u32, rootWindowPtr->getAlpha());

    valueEditorPtr->startDrawing(setEvtIdValue, displaysMenuStopDrawingValueEditor);
}

static void selectedOptionToggleHandleAsHex(Menu *menuPtr)
{
    (void)menuPtr;

    displaysMenuToggleEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_EVT_ID_VARIABLE_HANDLE_AS_HEX);
}
