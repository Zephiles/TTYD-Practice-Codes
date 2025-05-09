#include "menuUtils.h"
#include "drawText.h"
#include "cheats.h"
#include "mod.h"
#include "classes/window.h"
#include "menus/cheatsMenu.h"
#include "menus/rootMenu.h"
#include "ttyd/camdrv.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

static void controls(Menu *menuPtr, MenuButtonInput button);
static void draw(CameraId cameraId, void *user);
static void selectedOptionTurnOnOff(Menu *menuPtr);
static void selectedOptionChangeButtonCombo(Menu *menuPtr);
static void selectedOptionSetDuration(Menu *menuPtr);

static const MenuOption gOptions[] = {
    "Turn On/Off",
    selectedOptionTurnOnOff,

    "Change Button Combo",
    selectedOptionChangeButtonCombo,

    "Set Duration",
    selectedOptionSetDuration,
};

static const MenuFunctions gFuncs = {
    gOptions,
    controls,
    draw,
    nullptr, // Exit function not needed
};

void cheatsMenuGenerateLagSpikeInit(Menu *menuPtr)
{
    // Backup the selected cheat
    gCheatsMenu->setSelectedCheat(menuPtr->getCurrentIndex());

    constexpr uint32_t totalOptions = sizeof(gOptions) / sizeof(MenuOption);
    enterNextMenu(&gFuncs, totalOptions);
}

static void controls(Menu *menuPtr, MenuButtonInput button)
{
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;

    // If the button combo editor is open, then handle the controls for that
    ButtonComboEditor *buttonComboEditorPtr = cheatsMenuPtr->getButtonComboEditorPtr();
    if (buttonComboEditorPtr->shouldDraw())
    {
        buttonComboEditorPtr->controls(button);
        return;
    }

    // If the value editor is open, then handle the controls for that
    ValueEditor *valueEditorPtr = cheatsMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->controls(button);
        return;
    }

    // Use the default controls
    basicMenuLayoutControls(menuPtr, button);
}

static void drawGenerateLagSpikeInfo()
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

    // Draw the current cheat with its flag
    const uint32_t selectedCheat = gCheatsMenu->getSelectedCheat();
    drawText(gCheatsMenuInitOptions[selectedCheat].name, posX, posY, scale, getColorWhite(0xFF));

    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    posY -= lineDecrement;

    Cheats *cheatsPtr = gCheats;
    const char *string;
    uint32_t color;

    getOnOffTextAndColor(cheatsPtr->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_GENERATE_LAG_SPIKE),
                         &string,
                         &color,
                         0xFF);

    drawText(string, posX, posY, scale, color);
    posY -= (lineDecrement * 2.f);

    // Draw the button combo
    drawText("Button Combo (Can be pressed in any order)", posX, posY, scale, getColorWhite(0xFF));
    posY -= lineDecrement;

    char buf[128];
    constexpr uint32_t bufSize = sizeof(buf);

    buttonsToString(cheatsPtr->getCheatButtonCombo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_GENERATE_LAG_SPIKE),
                    buf,
                    bufSize);

    drawText(buf, posX, posY, scale, getColorWhite(0xFF));
    posY -= (lineDecrement * 2.f);

    // Draw the duration
    drawText("Duration (In milliseconds)", posX, posY, scale, getColorWhite(0xFF));
    posY -= lineDecrement;

    snprintf(buf, sizeof(buf), "%" PRIu32, cheatsPtr->getGenerateLagSpikeCheatPtr()->getDuration());
    drawText(buf, posX, posY, scale, getColorWhite(0xFF));
}

static void draw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDraw(cameraId, user);

    // Draw the info for the Generate Lag Spike cheat
    drawGenerateLagSpikeInfo();

    // Draw the button combo editor if applicable
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;
    ButtonComboEditor *buttonComboEditorPtr = cheatsMenuPtr->getButtonComboEditorPtr();
    if (buttonComboEditorPtr->shouldDraw())
    {
        buttonComboEditorPtr->draw();
    }

    // Draw the value editor if applicable
    ValueEditor *valueEditorPtr = cheatsMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->draw();
    }
}

static void selectedOptionTurnOnOff(Menu *menuPtr)
{
    (void)menuPtr;

    cheatsMenuToggleEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_GENERATE_LAG_SPIKE);
}

static void setButtonCombo(uint32_t buttonCombo)
{
    cheatsMenuSetNewButtonCombo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_GENERATE_LAG_SPIKE, buttonCombo);
}

static void selectedOptionChangeButtonCombo(Menu *menuPtr)
{
    (void)menuPtr;

    cheatsMenuChangeButtonCombo(setButtonCombo);
}

static void setNewDuration(const ValueType *valuePtr)
{
    gCheats->getGenerateLagSpikeCheatPtr()->setDuration(valuePtr->u32);

    // Close the value editor
    cheatsMenuValueEditorCancelSetValue();
}

static void selectedOptionSetDuration(Menu *menuPtr)
{
    (void)menuPtr;

    // Initialize the value editor
    constexpr uint32_t minValue = 1;
    constexpr uint32_t maxValue = LAG_SPIKE_MAX_DURATION;
    const uint32_t currentValue = gCheats->getGenerateLagSpikeCheatPtr()->getDuration();

    ValueEditor *valueEditorPtr = gCheatsMenu->getValueEditorPtr();

    uint32_t flags = 0;
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Y_SET_MAX);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Z_SET_MIN);

    cheatsMenuInitValueEditor(currentValue, minValue, maxValue, flags, VariableType::u16, true, setNewDuration);
}
