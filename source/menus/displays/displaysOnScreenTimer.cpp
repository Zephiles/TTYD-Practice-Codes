#include "menuUtils.h"
#include "drawText.h"
#include "displays.h"
#include "classes/window.h"
#include "menus/displaysMenu.h"
#include "menus/rootMenu.h"
#include "ttyd/camdrv.h"

#include <cstdint>

// The code assumes that the button combos are last, and that Start/Pause/Resume comes before Reset
const MenuOption gDisplaysMenuOnScreenTimerOptions[] = {
    "Turn On/Off",
    displaysMenuOnScreenTimerToggleFlag,

    "Adjust Manual Positioning",
    displaysAdjustManualPositionInit,

    "Change Start/Pause/Resume Button Combo",
    displaysMenuOnScreeTimerChangeButtonCombo,

    "Change Reset Button Combo",
    displaysMenuOnScreeTimerChangeButtonCombo,
};

const MenuFunctions gDisplaysMenuOnScreenTimerFuncs = {
    gDisplaysMenuOnScreenTimerOptions,
    displaysMenuOnScreenTimerControls,
    displaysMenuOnScreenTimerDraw,
    nullptr, // Exit function not needed
};

void displaysMenuOnScreenTimerInit(Menu *menuPtr)
{
    // Backup the selected display
    gDisplaysMenu->setSelectedDisplay(menuPtr->getCurrentIndex());

    constexpr uint32_t totalOptions = sizeof(gDisplaysMenuOnScreenTimerOptions) / sizeof(MenuOption);
    enterNextMenu(&gDisplaysMenuOnScreenTimerFuncs, totalOptions);
}

void displaysMenuOnScreenTimerControls(Menu *menuPtr, MenuButtonInput button)
{
    // If the button combo editor is open, then handle the controls for that
    if (menuPtr->flagIsSet(DisplaysMenuSpecialFlags::DISPLAYS_MENU_CHANGING_BUTTON_COMBO_FLAG))
    {
        gDisplaysMenu->getButtonComboEditor()->controls(button);
        return;
    }

    // Use the default controls
    basicMenuLayoutControls(menuPtr, button);
}

void DisplaysMenu::drawOnScreenTimerInfo() const
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

    const uint32_t displayEnabledFlag = indexToDisplayEnabledFlag(selectedDisplay);
    Displays *displaysPtr = gDisplays;
    const char *string;
    uint32_t color;

    getOnOffTextAndColor(displaysPtr->enabledFlagIsSet(displayEnabledFlag), &string, &color, 0xFF);
    drawText(string, posX, posY, scale, color);
    posY -= (lineDecrement * 2.f);

    // Draw the start/pause/resume button combo
    drawText("Start/Pause/Resume Button Combo", posX, posY, scale, getColorWhite(0xFF));
    posY -= lineDecrement;

    uint32_t buttonComboFlag;
    if (selectedDisplay == DisplaysMenuOptions::DISPLAYS_MENU_ONSCREEN_TIMER)
    {
        buttonComboFlag = DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_ONSCREEN_TIMER_START_PAUSE_RESUME;
    }
    else
    {
        buttonComboFlag = DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_FRAME_COUNTER_START_PAUSE_RESUME;
    }

    char buf[128];
    constexpr uint32_t bufSize = sizeof(buf);

    buttonsToString(displaysPtr->getDisplayButtonCombo(buttonComboFlag++), buf, bufSize);
    drawText(buf, posX, posY, scale, getColorWhite(0xFF));
    posY -= (lineDecrement * 2.f);

    // Draw the reset button combo
    drawText("Reset Button Combo", posX, posY, scale, getColorWhite(0xFF));
    posY -= lineDecrement;

    buttonsToString(displaysPtr->getDisplayButtonCombo(buttonComboFlag), buf, bufSize);
    drawText(buf, posX, posY, scale, getColorWhite(0xFF));
}

void displaysMenuOnScreenTimerDraw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDraw(cameraId, user);

    // Draw the info for the On-Screen Timer or Frame Counter display
    DisplaysMenu *displaysMenuPtr = gDisplaysMenu;
    displaysMenuPtr->drawOnScreenTimerInfo();

    // Draw the button combo editor if applicable
    ButtonComboEditor *buttonComboEditorPtr = displaysMenuPtr->getButtonComboEditor();
    if (buttonComboEditorPtr->shouldDraw())
    {
        buttonComboEditorPtr->draw();
    }
}

void displaysMenuOnScreenTimerToggleFlag(Menu *menuPtr)
{
    (void)menuPtr;

    const uint32_t displayEnabledFlag = indexToDisplayEnabledFlag(gDisplaysMenu->getSelectedDisplay());
    if (displaysMenuToggleEnabledFlag(displayEnabledFlag))
    {
        Displays *displaysPtr = gDisplays;

        if (displayEnabledFlag == DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_ONSCREEN_TIMER)
        {
            // Reset the On-Screen Timer
            displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_ONSCREEN_TIMER_PAUSED);

            OnScreenTimerDisplay *onScreenTimerPtr = displaysPtr->getOnScreenTimerDisplayPtr();
            onScreenTimerPtr->setMainTimer(0);
            onScreenTimerPtr->setPrevFrameTime(0);
        }
        else
        {
            // Reset the Frame Counter
            displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_FRAME_COUNTER_PAUSED);
            displaysPtr->getFrameCounterDisplayPtr()->setTimer(0);
        }
    }
}

void displaysMenuOnScreenTimerSetNewButtonCombo(uint32_t buttonCombo)
{
    uint32_t currentIndex = gMenu->getCurrentIndex();
    if (currentIndex > 1)
    {
        currentIndex -= 2; // Make the Start/Pause/Resume button combo option index 0
    }

    uint32_t displayButtonComboFlag;

    if (gDisplaysMenu->getSelectedDisplay() == DisplaysMenuOptions::DISPLAYS_MENU_ONSCREEN_TIMER)
    {
        displayButtonComboFlag =
            DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_ONSCREEN_TIMER_START_PAUSE_RESUME + currentIndex;
    }
    else
    {
        displayButtonComboFlag = DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_FRAME_COUNTER_START_PAUSE_RESUME + currentIndex;
    }

    displaysMenuSetNewButtonCombo(displayButtonComboFlag, buttonCombo);
}

void displaysMenuOnScreeTimerChangeButtonCombo(Menu *menuPtr)
{
    displaysMenuChangeButtonCombo(menuPtr, displaysMenuOnScreenTimerSetNewButtonCombo);
}
