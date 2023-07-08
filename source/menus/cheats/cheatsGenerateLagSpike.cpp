#include "menuUtils.h"
#include "drawText.h"
#include "cheats.h"
#include "mod.h"
#include "menus/cheatsMenu.h"
#include "menus/rootMenu.h"
#include "misc/utils.h"
#include "ttyd/camdrv.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

const MenuOption gCheatsMenuGenerateLagSpikeOptions[] = {
    gCheatsMenuTextTurnOnOff,
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
        cheatsMenuPtr->getButtonComboEditor()->controls(button);
        return;
    }

    // If the value editor is open, then handle the controls for that
    if (menuPtr->flagIsSet(CheatsMenuGenerateLagSpike::CHEATS_GENERATE_LAG_SPIKE_FLAG_CURRENTLY_SELECTING_ID))
    {
        cheatsMenuPtr->getValueEditor()->controls(button);
        return;
    }

    // Use the default controls
    controlsBasicMenuLayout(menuPtr, button);
}

void CheatsMenu::drawGenerateLagSpikeInfo() const
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

    // Draw the current cheat with its flag
    drawText(gCheatsMenuInitOptions[this->selectedCheat].name, posX, posY, scale, getColorWhite(0xFF));
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
    drawBasicMenuLayout(cameraId, user);

    // Draw the info for the Generate Lag Spike cheat
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;
    cheatsMenuPtr->drawGenerateLagSpikeInfo();

    // Draw the button combo editor if applicable
    ButtonComboEditor *buttonComboEditorPtr = cheatsMenuPtr->getButtonComboEditor();
    if (buttonComboEditorPtr->shouldDraw())
    {
        buttonComboEditorPtr->draw();
    }

    // Draw the value editor if applicable
    ValueEditor *valueEditorPtr = cheatsMenuPtr->getValueEditor();
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

void cheatsMenuCancelGenerateLagSpikeSetNewDuration()
{
    gCheatsMenu->getValueEditor()->stopDrawing();
    gMenu->clearFlag(CheatsMenuGenerateLagSpike::CHEATS_GENERATE_LAG_SPIKE_FLAG_CURRENTLY_SELECTING_ID);
}

void cheatsMenuGenerateLagSpikeSetNewDuration(const ValueType *valuePtr)
{
    gCheats->getGenerateLagSpikeCheatPtr()->setDuration(valuePtr->u32);

    // Close the value editor
    cheatsMenuCancelGenerateLagSpikeSetNewDuration();
}

void cheatsMenuGenerateLagSpikeSetDuration(Menu *menuPtr)
{
    // Bring up the window for selecting an id
    menuPtr->setFlag(CheatsMenuGenerateLagSpike::CHEATS_GENERATE_LAG_SPIKE_FLAG_CURRENTLY_SELECTING_ID);

    // Initialize the value editor
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;
    ValueEditor *valueEditorPtr = cheatsMenuPtr->getValueEditor();

    uint32_t flags = 0;
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Y_SET_MAX);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Z_SET_MIN);

    const uint32_t minValue = 1;
    const uint32_t maxValue = LAG_SPIKE_MAX_DURATION;
    const uint32_t currentValue = gCheats->getGenerateLagSpikeCheatPtr()->getDuration();

    const Window *rootWindowPtr = gRootWindow;

    valueEditorPtr->init(&currentValue,
                         &minValue,
                         &maxValue,
                         rootWindowPtr,
                         flags,
                         VariableType::u16,
                         rootWindowPtr->getAlpha(),
                         cheatsMenuPtr->getScale());

    valueEditorPtr->startDrawing(cheatsMenuGenerateLagSpikeSetNewDuration, cheatsMenuCancelGenerateLagSpikeSetNewDuration);
}
