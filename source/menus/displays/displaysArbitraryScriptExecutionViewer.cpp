#include "menuUtils.h"
#include "drawText.h"
#include "displays.h"
#include "mod.h"
#include "classes/window.h"
#include "classes/confirmationWindow.h"
#include "classes/errorWindow.h"
#include "menus/displaysMenu.h"
#include "menus/rootMenu.h"
#include "ttyd/camdrv.h"
#include "misc/utils.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

static void draw(CameraId cameraId, void *user);
static void controls(Menu *menuPtr, MenuButtonInput button);
static void selectedOptionTurnOnOff(Menu *menuPtr);
static void selectedOptionSetArrayIndex(Menu *menuPtr);
static void selectedOptionStoreCurrentValues(Menu *menuPtr);
static void selectedOptionApplyStoredValues(Menu *menuPtr);
static void selectedOptionToggleAdjustValuesColors(Menu *menuPtr);
static void selectedOptionToggleArrayIndexAsHex(Menu *menuPtr);

static const MenuOption gOptions[] {
    "Turn On/Off",
    selectedOptionTurnOnOff,

    "Set Array Index",
    selectedOptionSetArrayIndex,

    "Store Current Values",
    selectedOptionStoreCurrentValues,

    "Apply Stored Values",
    selectedOptionApplyStoredValues,

    "Adjust Values Colors",
    selectedOptionToggleAdjustValuesColors,

    "Handle Array Index As Hex",
    selectedOptionToggleArrayIndexAsHex,

    "Adjust Manual Positioning",
    displaysAdjustManualPositionInit,
};

static const MenuFunctions gFuncs = {
    gOptions,
    controls,
    draw,
    nullptr, // Exit function not needed
};

void displaysArbitraryScriptExecutionViewerInit(Menu *menuPtr)
{
    // Backup the selected display
    gDisplaysMenu->setSelectedDisplay(menuPtr->getCurrentIndex());

    constexpr uint32_t totalOptions = sizeof(gOptions) / sizeof(MenuOption);
    enterNextMenu(&gFuncs, totalOptions);
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

    // If the confirmation window is open, then handle the controls for that
    ConfirmationWindow *confirmationWindowPtr = displaysMenuPtr->getConfirmationWindowPtr();
    if (confirmationWindowPtr->shouldDraw())
    {
        confirmationWindowPtr->controls(button);
        return;
    }

    // Use the default controls
    basicMenuLayoutControls(menuPtr, button);
}

static void drawArbitraryScriptExecutionViewerExtraInfo()
{
    // Get the text position for the top-left of the window five lines under the main text to account for the main info already
    // drawn
    const uint32_t totalOptions = gMenu->getTotalOptions();
    constexpr float scale = MENU_SCALE;

    float tempPosX;
    float tempPosY;
    gRootWindow->getTextPosXYUnderMainText(nullptr, WindowAlignment::TOP_LEFT, totalOptions, 5, scale, &tempPosX, &tempPosY);

    // Get the current array index
    Displays *displaysPtr = gDisplays;
    const ArbitraryScriptExecutionViewer *aseViewerPtr = displaysPtr->getArbitraryScriptExecutionViewerPtr();
    const uint32_t arrayIndex = aseViewerPtr->getIndex();
    const char *valueFormat;

    if (displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_ASE_VIEWER_HANDLE_INDEX_AS_HEX))
    {
        valueFormat = "0x%" PRIX32;
    }
    else
    {
        valueFormat = "%u";
    }

    char format[64];
    snprintf(format, sizeof(format), "Array Index %s (Address 0x%%08" PRIX32 ")", valueFormat);

    char buf[64];
    const EvtEntry *entryPtr = aseViewerPtr->getViewedEvtPtr();
    snprintf(buf, sizeof(buf), format, arrayIndex, reinterpret_cast<uint32_t>(entryPtr));

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    const float posX = tempPosX;
    float posY = tempPosY;

    // Draw the current array index
    drawText(buf, posX, posY, scale, getColorWhite(0xFF));
    posY -= lineDecrement;

    // Draw the Handle Array Index As Hex text
    drawText("Handle Array Index As Hex", posX, posY, scale, getColorWhite(0xFF));

    // Position the text a bit to the right of the Handle Array Index As Hex text
    float width = getTextWidth("Handle Array Index As Hex", scale);
    float valuesPosX = tempPosX + width + (20.f * scale);

    // Draw whether the array index will be handled as hex
    const bool handleIndexAsHex =
        displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_ASE_VIEWER_HANDLE_INDEX_AS_HEX);

    const char *onOffText;
    uint32_t color;

    getYesNoTextAndColor(handleIndexAsHex, &onOffText, &color, 0xFF);
    drawText(onOffText, valuesPosX, posY, scale, color);
    posY -= lineDecrement;

    // Draw the Adjust Values Colors text
    drawText("Adjust Values Colors", posX, posY, scale, getColorWhite(0xFF));

    // Draw whether the values colors will be adjusted
    const bool adjustValuesColors =
        displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_ASE_VIEWER_ADJUST_VALUES_COLORS);

    getYesNoTextAndColor(adjustValuesColors, &onOffText, &color, 0xFF);
    drawText(onOffText, valuesPosX, posY, scale, color);
}

static void draw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDraw(cameraId, user);

    // Draw the main info for the Arbitrary Script Execution Viewer display
    drawGenericDisplayInfo();

    // Draw the extra info for the Arbitrary Script Execution Viewer display
    drawArbitraryScriptExecutionViewerExtraInfo();

    DisplaysMenu *displaysMenuPtr = gDisplaysMenu;

    // Draw the value editor if applicable
    ValueEditor *valueEditorPtr = displaysMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->draw();
    }

    // Draw the confirmation window if applicable
    ConfirmationWindow *confirmationWindowPtr = displaysMenuPtr->getConfirmationWindowPtr();
    if (confirmationWindowPtr->shouldDraw())
    {
        confirmationWindowPtr->draw();
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

    displaysMenuToggleEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_ASE_VIEWER);
}

static void setArrayIndex(const ValueType *valuePtr)
{
    // Set the new index
    gDisplays->getArbitraryScriptExecutionViewerPtr()->setIndex(valuePtr->u32);

    // Close the value editor
    displaysMenuStopDrawingValueEditor();
}

static void selectedOptionSetArrayIndex(Menu *menuPtr)
{
    (void)menuPtr;

    Displays *displaysPtr = gDisplays;

    // Initialize the value editor
    uint32_t flags = 0;
    flags = ValueEditor::setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);
    flags = ValueEditor::setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Y_SET_MAX);
    flags = ValueEditor::setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Z_SET_MIN);

    if (displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_ASE_VIEWER_HANDLE_INDEX_AS_HEX))
    {
        flags = ValueEditor::setFlag(flags, ValueEditorFlag::HANDLE_AS_HEX);
    }

    uint32_t minValue = 0;
    uint32_t maxValue = 0xFFFFFFFF;
    const Window *rootWindowPtr = gRootWindow;
    uint32_t currentValue = displaysPtr->getArbitraryScriptExecutionViewerPtr()->getIndex();
    ValueEditor *valueEditorPtr = gDisplaysMenu->getValueEditorPtr();

    valueEditorPtr
        ->init(&currentValue, &minValue, &maxValue, rootWindowPtr, flags, VariableType::u32, rootWindowPtr->getAlpha());

    valueEditorPtr->startDrawing(setArrayIndex, displaysMenuStopDrawingValueEditor);
}

static void closeConfirmationWindow()
{
    gDisplaysMenu->getConfirmationWindowPtr()->stopDrawing();
}

static void initConfirmationWindow(const char *helpTextPtr, ConfirmationWindowSelectedOptionFunc selectedOptionFunc)
{
    // Initialize the confirmation window
    ConfirmationWindow *confirmationWindowPtr = gDisplaysMenu->getConfirmationWindowPtr();
    const Window *rootWindowPtr = gRootWindow;

    confirmationWindowPtr->init(rootWindowPtr, helpTextPtr, rootWindowPtr->getAlpha());
    confirmationWindowPtr->startDrawing(selectedOptionFunc);
}

static void handleConfirmationWindowSelection(bool storeValues)
{
    // Do not apply if no values were previously stored
    // Do this check first to draw the error text for it before the error text for the array index
    Displays *displaysPtr = gDisplays;
    const char *errorText;

    const bool valuesAreCurrentlyStored =
        displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_ASE_VIEWER_VALUES_STORED);

    if (!storeValues && !valuesAreCurrentlyStored)
    {
        errorText = "Error: No values were previously stored.";
        gDisplaysMenu->initErrorWindow(errorText);
        return;
    }

    // Do not store/apply the values if the index being used ends up pointing to an invalid memory region
    ArbitraryScriptExecutionViewer *aseViewerPtr = displaysPtr->getArbitraryScriptExecutionViewerPtr();
    const EvtEntry *entryPtr = aseViewerPtr->getViewedEvtPtr();

    if (ptrIsValid(entryPtr))
    {
        if (storeValues)
        {
            aseViewerPtr->storeCurrentValues(displaysPtr, entryPtr);
        }
        else
        {
            aseViewerPtr->applyStoredValues();
        }
    }
    else
    {
        if (storeValues)
        {
            errorText =
                "Error: The values could not be stored, as\nthe index being used causes the address\nof the evt to be invalid.";
        }
        else
        {
            errorText =
                "Error: The stored values could not be applied,\nas the index being used causes the address\nof the evt to be "
                "invalid.";
        }

        gDisplaysMenu->initErrorWindow(errorText);
    }
}

static void storeCurrentValues(bool selectedYes)
{
    if (selectedYes)
    {
        handleConfirmationWindowSelection(true);
    }

    // Close the confirmation window
    closeConfirmationWindow();
}

static void applyStoredValues(bool selectedYes)
{
    if (selectedYes)
    {
        handleConfirmationWindowSelection(false);
    }

    // Close the confirmation window
    closeConfirmationWindow();
}

static void selectedOptionStoreCurrentValues(Menu *menuPtr)
{
    (void)menuPtr;

    const char *helpText =
        "Are you sure you want to store the\ncurrent evt values? This will overwrite\nany previously-stored values.";

    initConfirmationWindow(helpText, storeCurrentValues);
}

static void selectedOptionApplyStoredValues(Menu *menuPtr)
{
    (void)menuPtr;

    const char *helpText =
        "Are you sure you want to apply the stored\nevt values? This will overwrite the current\nvalues for the evt specified "
        "by the current\narray index.";

    initConfirmationWindow(helpText, applyStoredValues);
}

static void selectedOptionToggleAdjustValuesColors(Menu *menuPtr)
{
    (void)menuPtr;

    displaysMenuToggleEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_ASE_VIEWER_ADJUST_VALUES_COLORS);
}

static void selectedOptionToggleArrayIndexAsHex(Menu *menuPtr)
{
    (void)menuPtr;

    displaysMenuToggleEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_ASE_VIEWER_HANDLE_INDEX_AS_HEX);
}
