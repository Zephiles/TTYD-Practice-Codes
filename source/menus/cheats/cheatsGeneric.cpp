#include "menuUtils.h"
#include "drawText.h"
#include "cheats.h"
#include "mod.h"
#include "classes/window.h"
#include "menus/cheatsMenu.h"
#include "menus/rootMenu.h"
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

void cheatsMenuGenericControls(Menu *menuPtr, MenuButtonInput button)
{
    // If the button combo editor is open, then handle the controls for that
    if (menuPtr->flagIsSet(CHEATS_MENU_CHANGING_BUTTON_COMBO_FLAG))
    {
        gCheatsMenu->getButtonComboEditor()->controls(button);
        return;
    }

    // Use the default controls
    basicMenuLayoutControls(menuPtr, button);
}

void cheatsMenuGenericDraw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDraw(cameraId, user);

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
    // Get the text position for the top-left of the window two lines under the main text
    Menu *menuPtr = gMenu;
    const float scale = this->scale;
    const uint32_t totalOptions = menuPtr->getTotalOptions();

    float tempPosX;
    float tempPosY;
    gRootWindow->getTextPosXYUnderMainText(nullptr, WindowAlignment::TOP_LEFT, totalOptions, 2, scale, &tempPosX, &tempPosY);

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    const float posX = tempPosX;
    float posY = tempPosY;

    // Draw the text for the current cheat
    const uint32_t selectedCheat = this->selectedCheat;
    drawText(gCheatsMenuInitOptions[selectedCheat].name, posX, posY, scale, getColorWhite(0xFF));

    const float lineDecrement = LINE_HEIGHT_FLOAT * scale;
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

        buttonsToString(cheatsPtr->getCheatButtonCombo(cheatButtonComboFlag), buf, sizeof(buf));
        drawText(buf, posX, posY, scale, getColorWhite(0xFF));
    }
}

void cheatsMenuGenericToggleFlag(Menu *menuPtr)
{
    (void)menuPtr;

    const uint32_t cheatEnabledFlag = indexToCheatEnabledFlag(gCheatsMenu->getSelectedCheat());
    cheatsMenuToggleEnabledFlag(cheatEnabledFlag);
}

void cheatsMenuGenericSetNewButtonCombo(uint32_t buttonCombo)
{
    const uint32_t cheatButtonComboFlag = indexToCheatButtonComboFlag(gCheatsMenu->getSelectedCheat());
    cheatsMenuSetNewButtonCombo(cheatButtonComboFlag, buttonCombo);
}

void cheatsMenuGenericChangeButtonCombo(Menu *menuPtr)
{
    cheatsMenuChangeButtonCombo(menuPtr, cheatsMenuGenericSetNewButtonCombo);
}
