#include "menuUtils.h"
#include "drawText.h"
#include "displays.h"
#include "mod.h"
#include "classes/window.h"
#include "menus/displaysMenu.h"
#include "menus/rootMenu.h"
#include "ttyd/camdrv.h"

#include <cstdint>

static void draw(CameraId cameraId, void *user);
static void selectedOptionTurnOnOff(Menu *menuPtr);
static void selectedOptionChangeButtonCombo(Menu *menuPtr);

// For all selection types except displaysMenuGenericNoButtonComboNoManualPositionInit
static const MenuOption gMainOptions[] {
    "Turn On/Off",
    selectedOptionTurnOnOff,

    "Adjust Manual Positioning",
    displaysAdjustManualPositionInit,

    "Change Button Combo",
    selectedOptionChangeButtonCombo,
};

static const MenuFunctions gMainFuncs = {
    gMainOptions,
    displaysMenuDefaultControlsWithButtonComboEditor,
    draw,
    nullptr, // Exit function not needed
};

// For displaysMenuGenericHasButtonComboNoManualPositionInit
static const MenuOption gHasComboNoPositionOptions[] {
    "Turn On/Off",
    selectedOptionTurnOnOff,

    "Change Button Combo",
    selectedOptionChangeButtonCombo,
};

static const MenuFunctions gHasComboNoPositionFuncs = {
    gHasComboNoPositionOptions,
    displaysMenuDefaultControlsWithButtonComboEditor,
    draw,
    nullptr, // Exit function not needed
};

static void enterDisplaysMenuGeneric(Menu *menuPtr, uint32_t flags)
{
    // Backup the selected display
    gDisplaysMenu->setSelectedDisplay(menuPtr->getCurrentIndex());

    // Get the functions pointer and total options
    const MenuFunctions *funcs;
    uint32_t totalOptions;

    const bool hasButtonComboNoPosition =
        displaysMenuGenericSelectionFlagIsSet(
            flags,
            DisplaysMenuGenericSelectionFlags::DISPLAYS_GENERIC_OPTION_FLAG_HAS_BUTTON_COMBO) &&
        displaysMenuGenericSelectionFlagIsSet(
            flags,
            DisplaysMenuGenericSelectionFlags::DISPLAYS_GENERIC_OPTION_FLAG_NO_MANUAL_POSITION);

    if (hasButtonComboNoPosition)
    {
        funcs = &gHasComboNoPositionFuncs;
        totalOptions = sizeof(gHasComboNoPositionOptions) / sizeof(MenuOption);
    }
    else
    {
        funcs = &gMainFuncs;
        totalOptions = sizeof(gMainOptions) / sizeof(MenuOption);

        if (displaysMenuGenericSelectionFlagIsSet(
                flags,
                DisplaysMenuGenericSelectionFlags::DISPLAYS_GENERIC_OPTION_FLAG_NO_BUTTON_COMBO))
        {
            // Subtract one to exclude the button combo from the options
            // This assumes that the button combo is the last option in gMainOptions
            totalOptions--;
        }

        if (displaysMenuGenericSelectionFlagIsSet(
                flags,
                DisplaysMenuGenericSelectionFlags::DISPLAYS_GENERIC_OPTION_FLAG_NO_MANUAL_POSITION))
        {
            // Subtract one to exclude the manual positioning from the options
            // This assumes that the manual positioning is the semi-last option in gDisplaysMenuGenericOptions
            totalOptions--;
        }
    }

    menuPtr = enterNextMenu(funcs, totalOptions);

    if (displaysMenuGenericSelectionFlagIsSet(flags,
                                              DisplaysMenuGenericSelectionFlags::DISPLAYS_GENERIC_OPTION_FLAG_HAS_BUTTON_COMBO))
    {
        menuPtr->setFlag(DisplaysMenuGenericFlags::DISPLAYS_GENERIC_FLAG_DISPLAY_HAS_BUTTON_COMBO);
    }
}

void displaysMenuGenericHasButtonComboInit(Menu *menuPtr)
{
    uint32_t flags = 0;

    flags =
        setDisplaysMenuGenericSelectionFlag(flags,
                                            DisplaysMenuGenericSelectionFlags::DISPLAYS_GENERIC_OPTION_FLAG_HAS_BUTTON_COMBO);

    enterDisplaysMenuGeneric(menuPtr, flags);
}

void displaysMenuGenericNoButtonComboInit(Menu *menuPtr)
{
    uint32_t flags = 0;

    flags =
        setDisplaysMenuGenericSelectionFlag(flags,
                                            DisplaysMenuGenericSelectionFlags::DISPLAYS_GENERIC_OPTION_FLAG_NO_BUTTON_COMBO);

    enterDisplaysMenuGeneric(menuPtr, flags);
}

void displaysMenuGenericHasButtonComboNoManualPositionInit(Menu *menuPtr)
{
    uint32_t flags = 0;

    flags =
        setDisplaysMenuGenericSelectionFlag(flags,
                                            DisplaysMenuGenericSelectionFlags::DISPLAYS_GENERIC_OPTION_FLAG_HAS_BUTTON_COMBO);

    flags =
        setDisplaysMenuGenericSelectionFlag(flags,
                                            DisplaysMenuGenericSelectionFlags::DISPLAYS_GENERIC_OPTION_FLAG_NO_MANUAL_POSITION);

    enterDisplaysMenuGeneric(menuPtr, flags);
}

void displaysMenuGenericNoButtonComboNoManualPositionInit(Menu *menuPtr)
{
    uint32_t flags = 0;

    flags =
        setDisplaysMenuGenericSelectionFlag(flags,
                                            DisplaysMenuGenericSelectionFlags::DISPLAYS_GENERIC_OPTION_FLAG_NO_BUTTON_COMBO);

    flags =
        setDisplaysMenuGenericSelectionFlag(flags,
                                            DisplaysMenuGenericSelectionFlags::DISPLAYS_GENERIC_OPTION_FLAG_NO_MANUAL_POSITION);

    enterDisplaysMenuGeneric(menuPtr, flags);
}

void drawGenericDisplayInfo()
{
    // Get the text position for the top-left of the window two lines under the main text
    Menu *menuPtr = gMenu;
    constexpr float scale = MENU_SCALE;
    const uint32_t totalOptions = menuPtr->getTotalOptions();

    float tempPosX;
    float tempPosY;
    gRootWindow->getTextPosXYUnderMainText(nullptr, WindowAlignment::TOP_LEFT, totalOptions, 2, scale, &tempPosX, &tempPosY);

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    const float posX = tempPosX;
    float posY = tempPosY;

    // Draw the text for the current display
    const uint32_t selectedDisplay = gDisplaysMenu->getSelectedDisplay();
    drawText(gDisplaysMenuInitOptions[selectedDisplay].name, posX, posY, scale, getColorWhite(0xFF));

    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    posY -= lineDecrement;

    // Draw the on/off text for the current display
    const uint32_t displayEnabledFlag = indexToDisplayEnabledFlag(selectedDisplay);
    const char *onOffText;
    uint32_t color;

    Displays *displaysPtr = gDisplays;
    getOnOffTextAndColor(displaysPtr->enabledFlagIsSet(displayEnabledFlag), &onOffText, &color, 0xFF);
    drawText(onOffText, posX, posY, scale, color);

    if (menuPtr->flagIsSet(DisplaysMenuGenericFlags::DISPLAYS_GENERIC_FLAG_DISPLAY_HAS_BUTTON_COMBO))
    {
        posY -= (lineDecrement * 2.f);

        // Draw the help text for the button combo
        drawText(gHelpTextButtonCombo, posX, posY, scale, getColorWhite(0xFF));
        posY -= lineDecrement;

        // Draw the button combo for the display
        const uint32_t displayButtonComboFlag = indexToDisplayButtonComboFlag(selectedDisplay);
        char buf[128];

        buttonsToString(displaysPtr->getDisplayButtonCombo(displayButtonComboFlag), buf, sizeof(buf));
        drawText(buf, posX, posY, scale, getColorWhite(0xFF));
    }
}

static void draw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDraw(cameraId, user);

    // Draw the info for the current display
    drawGenericDisplayInfo();

    // Draw the button combo editor if applicable
    ButtonComboEditor *buttonComboEditorPtr = gDisplaysMenu->getButtonComboEditorPtr();
    if (buttonComboEditorPtr->shouldDraw())
    {
        buttonComboEditorPtr->draw();
    }
}

static void selectedOptionTurnOnOff(Menu *menuPtr)
{
    (void)menuPtr;

    const uint32_t displayEnabledFlag = indexToDisplayEnabledFlag(gDisplaysMenu->getSelectedDisplay());
    displaysMenuToggleEnabledFlag(displayEnabledFlag);
}

static void setButtonCombo(uint32_t buttonCombo)
{
    const uint32_t displayButtonComboFlag = indexToDisplayButtonComboFlag(gDisplaysMenu->getSelectedDisplay());
    displaysMenuSetNewButtonCombo(displayButtonComboFlag, buttonCombo);
}

static void selectedOptionChangeButtonCombo(Menu *menuPtr)
{
    (void)menuPtr;

    displaysMenuChangeButtonCombo(setButtonCombo);
}
