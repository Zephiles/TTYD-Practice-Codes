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

const MenuOption gCheatsMenuGenerateLagSpikeOptions[] = {
    "Turn On/Off",
    cheatsMenuGenerateLagSpikeToggleFlag,

    "Change Button Combo",
    cheatsMenuGenerateLagSpikeChangeButtonCombo,

    "Set Duration",
    cheatsMenuGenerateLagSpikeSetDuration,
};

const MenuFunctions gCheatsMenuGenerateLagSpikeFuncs = {
    gCheatsMenuGenerateLagSpikeOptions,
    cheatsMenuGenerateLagSpikeControls,
    cheatsMenuGenerateLagSpikeDraw,
    nullptr, // Exit function not needed
};

void cheatsMenuGenerateLagSpikeInit(Menu *menuPtr)
{
    // Backup the selected cheat
    gCheatsMenu->setSelectedCheat(menuPtr->getCurrentIndex());

    constexpr uint32_t totalOptions = sizeof(gCheatsMenuGenerateLagSpikeOptions) / sizeof(MenuOption);
    enterNextMenu(&gCheatsMenuGenerateLagSpikeFuncs, totalOptions);
}

void cheatsMenuGenerateLagSpikeControls(Menu *menuPtr, MenuButtonInput button)
{
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;

    // If the button combo editor is open, then handle the controls for that
    if (menuPtr->flagIsSet(CHEATS_MENU_CHANGING_BUTTON_COMBO_FLAG))
    {
        cheatsMenuPtr->getButtonComboEditorPtr()->controls(button);
        return;
    }

    // If the value editor is open, then handle the controls for that
    if (menuPtr->flagIsSet(CHEATS_MENU_USING_VALUE_EDITOR_FLAG))
    {
        cheatsMenuPtr->getValueEditorPtr()->controls(button);
        return;
    }

    // Use the default controls
    basicMenuLayoutControls(menuPtr, button);
}

void CheatsMenu::drawGenerateLagSpikeInfo() const
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
    drawText(gCheatsMenuInitOptions[this->selectedCheat].name, posX, posY, scale, getColorWhite(0xFF));

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

void cheatsMenuGenerateLagSpikeDraw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDraw(cameraId, user);

    // Draw the info for the Generate Lag Spike cheat
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;
    cheatsMenuPtr->drawGenerateLagSpikeInfo();

    // Draw the button combo editor if applicable
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

void cheatsMenuGenerateLagSpikeToggleFlag(Menu *menuPtr)
{
    (void)menuPtr;

    cheatsMenuToggleEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_GENERATE_LAG_SPIKE);
}

void cheatsMenuGenerateLagSpikeSetNewButtonCombo(uint32_t buttonCombo)
{
    cheatsMenuSetNewButtonCombo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_GENERATE_LAG_SPIKE, buttonCombo);
}

void cheatsMenuGenerateLagSpikeChangeButtonCombo(Menu *menuPtr)
{
    cheatsMenuChangeButtonCombo(menuPtr, cheatsMenuGenerateLagSpikeSetNewButtonCombo);
}

void cheatsMenuGenerateLagSpikeSetNewDuration(const ValueType *valuePtr)
{
    gCheats->getGenerateLagSpikeCheatPtr()->setDuration(valuePtr->u32);

    // Close the value editor
    cheatsMenuValueEditorCancelSetValue();
}

void cheatsMenuGenerateLagSpikeSetDuration(Menu *menuPtr)
{
    // Initialize the value editor
    constexpr uint32_t minValue = 1;
    constexpr uint32_t maxValue = LAG_SPIKE_MAX_DURATION;
    const uint32_t currentValue = gCheats->getGenerateLagSpikeCheatPtr()->getDuration();

    ValueEditor *valueEditorPtr = gCheatsMenu->getValueEditorPtr();

    uint32_t flags = 0;
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Y_SET_MAX);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Z_SET_MIN);

    cheatsMenuInitValueEditor(menuPtr,
                              currentValue,
                              minValue,
                              maxValue,
                              flags,
                              VariableType::u16,
                              true,
                              cheatsMenuGenerateLagSpikeSetNewDuration);
}
