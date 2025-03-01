#include "menuUtils.h"
#include "drawText.h"
#include "cheats.h"
#include "classes/window.h"
#include "menus/cheatsMenu.h"
#include "menus/rootMenu.h"
#include "ttyd/camdrv.h"

#include <cstdint>

static void draw(CameraId cameraId, void *user);
static void selectedOptionTurnOnOff(Menu *menuPtr);
static void selectedOptionChangeButtonCombo(Menu *menuPtr);

static const MenuOption gOptions[] = {
    "Turn On/Off",
    selectedOptionTurnOnOff,

    "Change Frame Advance Button Combo",
    selectedOptionChangeButtonCombo,

    "Change Pause/Resume Button Combo",
    selectedOptionChangeButtonCombo,
};

static const MenuFunctions gFuncs = {
    gOptions,
    cheatsMenuDefaultControlsWithButtonComboEditor,
    draw,
    nullptr, // Exit function not needed
};

void cheatsMenuFrameAdvanceInit(Menu *menuPtr)
{
    // Backup the selected cheat
    gCheatsMenu->setSelectedCheat(menuPtr->getCurrentIndex());

    constexpr uint32_t totalOptions = sizeof(gOptions) / sizeof(MenuOption);
    enterNextMenu(&gFuncs, totalOptions);
}

static void drawFrameAdvanceInfo()
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

    getOnOffTextAndColor(cheatsPtr->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_FRAME_ADVANCE),
                         &string,
                         &color,
                         0xFF);

    drawText(string, posX, posY, scale, color);
    posY -= (lineDecrement * 2.f);

    // Draw the Frame Advance button combo
    drawText("Frame Advance Button Combo", posX, posY, scale, getColorWhite(0xFF));
    posY -= lineDecrement;

    char buf[128];
    constexpr uint32_t bufSize = sizeof(buf);

    buttonsToString(cheatsPtr->getCheatButtonCombo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_FRAME_ADVANCE_ADVANCE_FRAME),
                    buf,
                    bufSize);

    drawText(buf, posX, posY, scale, getColorWhite(0xFF));
    posY -= (lineDecrement * 2.f);

    // Draw the pause/resume button combo
    drawText("Pause/Resume Button Combo", posX, posY, scale, getColorWhite(0xFF));
    posY -= lineDecrement;

    buttonsToString(cheatsPtr->getCheatButtonCombo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_FRAME_ADVANCE_PAUSE_RESUME),
                    buf,
                    bufSize);

    drawText(buf, posX, posY, scale, getColorWhite(0xFF));
}

static void draw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDraw(cameraId, user);

    // Draw the info for the Frame Advance cheat
    drawFrameAdvanceInfo();

    // Draw the button combo editor if applicable
    ButtonComboEditor *buttonComboEditorPtr = gCheatsMenu->getButtonComboEditorPtr();
    if (buttonComboEditorPtr->shouldDraw())
    {
        buttonComboEditorPtr->draw();
    }
}

static void selectedOptionTurnOnOff(Menu *menuPtr)
{
    (void)menuPtr;

    Cheats *cheatsPtr = gCheats;
    FrameAdvanceCheat *frameAdvancePtr = cheatsPtr->getFrameAdvanceCheatPtr();

    if (cheatsMenuToggleEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_FRAME_ADVANCE))
    {
        // Allocate memory for the backup inputs
        frameAdvancePtr->allocMemoryForPrevInputs();

        // Clear any existing backup inputs to be safe

        // This is needed because memory could have somehow already been allocated for the backup inputs before
        // allocMemoryForPrevInputs was called
        frameAdvancePtr->clearAllInputs();
    }
    else
    {
        // Free the memory for the backup inputs
        frameAdvancePtr->freePrevInputs();
    }

    // Clear both of the Frame Advance flags to be safe
    cheatsPtr->clearMiscFlag(CheatsMiscFlag::CHEATS_MISC_FLAG_FRAME_ADVANCE_ADVANCE_FRAME);
    cheatsPtr->clearMiscFlag(CheatsMiscFlag::CHEATS_MISC_FLAG_FRAME_ADVANCE_GAME_IS_PAUSED);
}

static void setButtonCombo(uint32_t buttonCombo)
{
    uint32_t cheatButtonComboFlag;
    if (gMenu->getCurrentIndex() == CheatsMenuFrameAdvanceOptions::CHEATS_MENU_FRAME_ADVANCE_CHANGE_FRAME_ADVANCE_COMBO)
    {
        cheatButtonComboFlag = CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_FRAME_ADVANCE_ADVANCE_FRAME;
    }
    else
    {
        cheatButtonComboFlag = CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_FRAME_ADVANCE_PAUSE_RESUME;
    }

    cheatsMenuSetNewButtonCombo(cheatButtonComboFlag, buttonCombo);
}

static void selectedOptionChangeButtonCombo(Menu *menuPtr)
{
    (void)menuPtr;

    cheatsMenuChangeButtonCombo(setButtonCombo);
}
