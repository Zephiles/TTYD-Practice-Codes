#include "menuUtils.h"
#include "drawText.h"
#include "cheats.h"
#include "mod.h"
#include "menus/cheatsMenu.h"
#include "menus/rootMenu.h"
#include "misc/utils.h"
#include "ttyd/camdrv.h"

#include <cstdint>

const MenuOption gCheatsMenuGenericOptions[] = {
    "Turn On/Off",
    cheatsMenuGenericToggleFlag,

    "Change Button Combo",
    cheatsMenuGenericChangeButtonCombo,
};

const MenuFunctions gCheatsMenuInitFuncs = {
    gCheatsMenuGenericOptions,
    cheatsMenuGenericControls,
    cheatsMenuGenericDraw,
    nullptr, // Exit function not needed
};

void enterCheatsMenuGeneric(Menu *menuPtr, bool hasButtonCombo)
{
    // Backup the selected cheat
    gCheatsMenu->setSelectedCheat(menuPtr->getCurrentIndex());

    uint32_t totalOptions = sizeof(gCheatsMenuGenericOptions) / sizeof(MenuOption);
    if (!hasButtonCombo)
    {
        // Subtract one to exclude the button combo from the options
        // This assumes that the button combo is the last option in gCheatsMenuGenericOptions
        totalOptions--;
    }

    menuPtr = enterNextMenu(&gCheatsMenuInitFuncs, totalOptions);

    if (hasButtonCombo)
    {
        menuPtr->setFlag(CheatsMenuGenericFlags::CHEATS_GENERIC_FLAG_CHEAT_HAS_BUTTON_COMBO);
    }
}

void cheatsMenuGenericNoButtonComboInit(Menu *menuPtr)
{
    enterCheatsMenuGeneric(menuPtr, false);
}

void cheatsMenuGenericHasButtonComboInit(Menu *menuPtr)
{
    enterCheatsMenuGeneric(menuPtr, true);
}

void cheatsMenuGenericDPadControls(MenuButtonInput button, Menu *menuPtr)
{
    const uint32_t totalOptions = menuPtr->getTotalOptions();

    // If there's only one option, then don't bother handling inputs
    if (totalOptions <= 1)
    {
        return;
    }

    menuControlsVertical(button,
                         menuPtr->getCurrentIndexPtr(),
                         menuPtr->getCurrentPagePtr(),
                         totalOptions,
                         MAX_CHEATS_PER_PAGE,
                         1,
                         false);
}

void cheatsMenuGenericControls(Menu *menuPtr, MenuButtonInput button)
{
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;

    // If the button combo editor is open, then handle the controls for that
    if (menuPtr->flagIsSet(CheatsMenuGenericFlags::CHEATS_GENERIC_FLAG_CHANGING_BUTTON_COMBO))
    {
        cheatsMenuPtr->getButtonComboEditor()->controls(button);
        return;
    }

    // The function for checking for auto-incrementing needs to run every frame to be handled correctly
    const bool autoIncrement = handleMenuAutoIncrement(cheatsMenuPtr->getAutoIncrementPtr());

    // Handle held button inputs if auto-incrementing should be done
    if (autoIncrement)
    {
        const MenuButtonInput buttonHeld = getMenuButtonInput(false);
        switch (buttonHeld)
        {
            case MenuButtonInput::DPAD_DOWN:
            case MenuButtonInput::DPAD_UP:
            {
                cheatsMenuGenericDPadControls(buttonHeld, menuPtr);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    switch (button)
    {
        case MenuButtonInput::DPAD_DOWN:
        case MenuButtonInput::DPAD_UP:
        {
            cheatsMenuGenericDPadControls(button, menuPtr);
            break;
        }
        default:
        {
            // Use the default controls
            controlsBasicMenuLayout(menuPtr, button);
            break;
        }
    }
}

void cheatsMenuGenericDraw(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Draw the main window
    drawBasicMenuLayout(cameraId, user);

    // Draw the info for the current cheat
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;
    cheatsMenuPtr->drawGenericCheatInfo();

    // Draw the button combo editor if applicable
    ButtonComboEditor *buttonComboEditorPtr = cheatsMenuPtr->getButtonComboEditor();
    if (buttonComboEditorPtr->shouldDraw())
    {
        buttonComboEditorPtr->draw();
    }
}

void CheatsMenu::drawGenericCheatInfo() const
{
    // Get the text position for the top-left of the window
    float tempPosX;
    float tempPosY;
    gRootWindow->getTextPosXY(nullptr, WindowAlignment::TOP_LEFT, scale, &tempPosX, &tempPosY);

    // Position the text two lines under the options
    Menu *menuPtr = gMenu;
    const uint32_t totalOptions = menuPtr->getTotalOptions();

    const float scale = this->scale;
    const float lineDecrement = LINE_HEIGHT_FLOAT * scale;

    const float posX = tempPosX;
    float posY = tempPosY - (intToFloat(totalOptions + 1) * lineDecrement);

    // Draw the text for the current cheat
    const uint32_t selectedCheat = this->selectedCheat;
    drawText(gCheatsMenuInitOptions[selectedCheat].name, posX, posY, scale, getColorWhite(0xFF));
    posY -= lineDecrement;

    // Draw the on/off text for the current cheat
    const uint32_t cheatEnabledFlag = indexToCheatEnabledFlag(selectedCheat);
    const char *onOffText;
    uint32_t color;

    Cheats *cheatsPtr = gCheats;
    getOnOffTextAndColor(cheatsPtr->enabledFlagIsSet(cheatEnabledFlag), &onOffText, &color, 0xFF);
    drawText(onOffText, posX, posY, scale, color);

    if (menuPtr->flagIsSet(CheatsMenuGenericFlags::CHEATS_GENERIC_FLAG_CHEAT_HAS_BUTTON_COMBO))
    {
        posY -= (lineDecrement * 2.f);

        // Draw the help text for the button combo
        drawText(gHelpTextButtonCombo, posX, posY, scale, getColorWhite(0xFF));
        posY -= lineDecrement;

        // Draw the button combo for the cheat
        const uint32_t cheatButtonComboFlag = indexToCheatButtonComboFlag(selectedCheat);
        char buf[128];

        buttonsToString(cheatsPtr->getButtonCombo(cheatButtonComboFlag), buf, sizeof(buf));
        drawText(buf, posX, posY, scale, getColorWhite(0xFF));
    }
}

void cheatsMenuGenericToggleFlag(Menu *menuPtr)
{
    (void)menuPtr;

    const uint32_t cheatEnabledFlag = indexToCheatEnabledFlag(gCheatsMenu->getSelectedCheat());
    gCheats->toggleEnabledFlag(cheatEnabledFlag);
}

void cheatsMenuGenericCancelSetNewButtonCombo()
{
    gCheatsMenu->getButtonComboEditor()->stopDrawing();
    gMenu->clearFlag(CheatsMenuGenericFlags::CHEATS_GENERIC_FLAG_CHANGING_BUTTON_COMBO);
    gMod.stopChangingButtonCombos();
}

void cheatsMenuGenericSetNewButtonCombo(uint32_t buttonCombo)
{
    const uint32_t cheatButtonComboFlag = indexToCheatButtonComboFlag(gCheatsMenu->getSelectedCheat());
    gCheats->setButtonCombo(cheatButtonComboFlag, buttonCombo);

    // Close the button combo editor
    cheatsMenuGenericCancelSetNewButtonCombo();
}

void cheatsMenuGenericChangeButtonCombo(Menu *menuPtr)
{
    gMod.startChangingButtonCombos();

    // Bring up the window for changing button combos
    menuPtr->setFlag(CheatsMenuGenericFlags::CHEATS_GENERIC_FLAG_CHANGING_BUTTON_COMBO);

    // Initialize the button combo editor
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;
    ButtonComboEditor *buttonComboEditorPtr = cheatsMenuPtr->getButtonComboEditor();

    const Window *rootWindowPtr = gRootWindow;
    buttonComboEditorPtr->init(rootWindowPtr, cheatsMenuPtr->getScale(), rootWindowPtr->getAlpha());
    buttonComboEditorPtr->startDrawing(cheatsMenuGenericSetNewButtonCombo, cheatsMenuGenericCancelSetNewButtonCombo);
}
