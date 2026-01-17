#include "menuUtils.h"
#include "mod.h"
#include "drawText.h"
#include "classes/window.h"
#include "menus/cheatsMenu.h"
#include "menus/rootMenu.h"
#include "ttyd/mariost.h"
#include "ttyd/evtmgr.h"
#include "ttyd/swdrv.h"
#include "ttyd/camdrv.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

static void draw(CameraId cameraId, void *user);
static void variablesControls(Menu *menuPtr, MenuButtonInput button);
static void flagsControls(Menu *menuPtr, MenuButtonInput button);

static const char *gGlobalWordsOptions[] = {
    "Change Value",
    "Set New Value", // The code assumes that Set New Value will be the last option
};

static const char *gGlobalFlagsOptions[] = {
    "Toggle Value", // The code assumes that Toggle Value will be the last option
};

static void flagsSetValue(const ValueType *valuePtr)
{
    CheatsMenu *cheatsPtr = gCheatsMenu;
    const uint32_t value = valuePtr->u32;

    if (gMenu->getCurrentIndex() == 0)
    {
        cheatsPtr->setFlagVariableToSet(value);
    }
    else
    {
        cheatsPtr->setFlagVariableValue(value);
    }

    // Close the value editor
    cheatsMenuValueEditorCancelSetValue();
}

// Menu for working with flags
static const MenuFunctions gFlagsFuncs = {
    nullptr, // The first line of the options will be different depending on which flag was selected, so all of the options must
             // be drawn manually

    flagsControls,
    draw,
    nullptr, // Exit function not needed
};

static void selectedOptionSetFlags(Menu *menuPtr)
{
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;

    // Back up the current index
    cheatsMenuPtr->setCurrentIndex(menuPtr->getCurrentIndex());

    // Reset flagVariableToSet and flagVariableValue
    cheatsMenuPtr->setFlagVariableToSet(0);
    cheatsMenuPtr->setFlagVariableValue(0);

    constexpr uint32_t totalOptions = (sizeof(gGlobalFlagsOptions) / sizeof(const char *)) + 1;
    enterNextMenu(&gFlagsFuncs, totalOptions);
}

static void flagsControls(Menu *menuPtr, MenuButtonInput button)
{
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;

    // If the value editor is open, then handle the controls for that
    ValueEditor *valueEditorPtr = cheatsMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->controls(button);
        return;
    }

    switch (button)
    {
        case MenuButtonInput::A:
        {
            const uint32_t currentIndex = menuPtr->getCurrentIndex();
            const uint32_t selectedOption = cheatsMenuPtr->getCurrentIndex();
            const uint32_t setNewValueIndex = menuPtr->getTotalOptions() - 1;
            const uint32_t flagVariableToSet = cheatsMenuPtr->getFlagVariableToSet();

            if (currentIndex != setNewValueIndex)
            {
                // Initialize the value editor
                uint32_t maxValue;

                switch (selectedOption)
                {
                    case ManageFlagsOptions::MANAGE_FLAGS_GSWF:
                    {
                        maxValue = (sizeof(GlobalWork::mGSWF) * 8) - 1; // 8 bits/flags per byte
                        break;
                    }
                    case ManageFlagsOptions::MANAGE_FLAGS_GF:
                    {
                        maxValue = (sizeof(EvtWork::gfData) * 8) - 1; // 8 bits/flags per byte
                        break;
                    }
                    case ManageFlagsOptions::MANAGE_FLAGS_LSWF:
                    {
                        maxValue = (sizeof(GlobalWork::mLSWF) * 8) - 1; // 8 bits/flags per byte
                        break;
                    }
                    default:
                    {
                        maxValue = 0;
                        break;
                    }
                }

                const uint32_t currentValue = flagVariableToSet;
                constexpr uint32_t minValue = 0;

                ValueEditor *valueEditorPtr = cheatsMenuPtr->getValueEditorPtr();

                uint32_t flags = 0;
                flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);
                flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Y_SET_MAX);
                flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Z_SET_MIN);

                cheatsMenuInitValueEditor(currentValue, minValue, maxValue, flags, VariableType::u32, true, flagsSetValue);
            }
            else
            {
                switch (selectedOption)
                {
                    case ManageFlagsOptions::MANAGE_FLAGS_GSWF:
                    {
                        if (swGet(flagVariableToSet))
                        {
                            swClear(flagVariableToSet);
                        }
                        else
                        {
                            swSet(flagVariableToSet);
                        }
                        break;
                    }
                    case ManageFlagsOptions::MANAGE_FLAGS_GF:
                    {
                        toggleGF(flagVariableToSet);
                        break;
                    }
                    case ManageFlagsOptions::MANAGE_FLAGS_LSWF:
                    {
                        if (_swGet(flagVariableToSet))
                        {
                            _swClear(flagVariableToSet);
                        }
                        else
                        {
                            _swSet(flagVariableToSet);
                        }
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
            }
            break;
        }
        default:
        {
            // Use the default controls
            basicMenuLayoutControls(menuPtr, button);
            break;
        }
    }
}

// Menu for working with non-flag variables
static const MenuFunctions gVariableFuncs = {
    nullptr, // The first line of the options will be different depending on which non-flag variable was selected, so all of the
             // options must be drawn manually

    variablesControls,
    draw,
    nullptr, // Exit function not needed
};

static void selectedOptionSetVariables(Menu *menuPtr)
{
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;

    // Back up the current index
    cheatsMenuPtr->setCurrentIndex(menuPtr->getCurrentIndex());

    // Reset flagVariableToSet and flagVariableValue
    cheatsMenuPtr->setFlagVariableToSet(0);
    cheatsMenuPtr->setFlagVariableValue(0);

    constexpr uint32_t totalOptions = (sizeof(gGlobalWordsOptions) / sizeof(const char *)) + 1;
    enterNextMenu(&gVariableFuncs, totalOptions);
}

static void variablesControls(Menu *menuPtr, MenuButtonInput button)
{
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;

    // If the value editor is open, then handle the controls for that
    ValueEditor *valueEditorPtr = cheatsMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->controls(button);
        return;
    }

    switch (button)
    {
        case MenuButtonInput::A:
        {
            const uint32_t currentIndex = menuPtr->getCurrentIndex();
            const uint32_t selectedOption = cheatsMenuPtr->getCurrentIndex();
            const uint32_t setNewValueIndex = menuPtr->getTotalOptions() - 1;
            const uint32_t flagVariableToSet = cheatsMenuPtr->getFlagVariableToSet();

            if (currentIndex != setNewValueIndex)
            {
                // Initialize the value editor
                uint32_t currentValue;

                if (currentIndex == 0)
                {
                    currentValue = flagVariableToSet;
                }
                else
                {
                    currentValue = getGlobalVariableFlagValue(selectedOption, flagVariableToSet);
                }

                ValueEditor *valueEditorPtr = cheatsMenuPtr->getValueEditorPtr();

                uint32_t flags = 0;
                flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);

                constexpr uint32_t minValue = 0;
                bool hasMinAndMax = true;
                uint32_t maxValue = 0;

                switch (selectedOption)
                {
                    case ManageFlagsOptions::MANAGE_FLAGS_GSW:
                    {
                        if (currentIndex == 0)
                        {
                            maxValue = sizeof(GlobalWork::mGSW) - 1;
                        }
                        else if (flagVariableToSet == 0)
                        {
                            // Currently handling the sequence position
                            maxValue = CHEATS_TOTAL_EVENT_NAMES - 1;
                        }
                        else
                        {
                            maxValue = 0xFF;
                        }
                        break;
                    }
                    case ManageFlagsOptions::MANAGE_FLAGS_GW:
                    {
                        if (currentIndex == 0)
                        {
                            maxValue = (sizeof(EvtWork::gwData) / sizeof(uint32_t)) - 1;
                        }
                        else
                        {
                            hasMinAndMax = false;
                            flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::HANDLE_AS_HEX);
                        }
                        break;
                    }
                    case ManageFlagsOptions::MANAGE_FLAGS_LSW:
                    {
                        if (currentIndex == 0)
                        {
                            maxValue = sizeof(GlobalWork::mLSW) - 1;
                        }
                        else
                        {
                            maxValue = 0xFF;
                        }
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }

                if (hasMinAndMax)
                {
                    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Y_SET_MAX);
                    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Z_SET_MIN);
                }

                cheatsMenuInitValueEditor(currentValue,
                                          minValue,
                                          maxValue,
                                          flags,
                                          VariableType::u32,
                                          hasMinAndMax,
                                          flagsSetValue);
            }
            else
            {
                const uint32_t flagVariableValue = cheatsMenuPtr->getFlagVariableValue();
                switch (selectedOption)
                {
                    case ManageFlagsOptions::MANAGE_FLAGS_GSW:
                    {
                        swByteSet(flagVariableToSet, flagVariableValue);
                        break;
                    }
                    case ManageFlagsOptions::MANAGE_FLAGS_GW:
                    {
                        setGW(flagVariableToSet, flagVariableValue);
                        break;
                    }
                    case ManageFlagsOptions::MANAGE_FLAGS_LSW:
                    {
                        _swByteSet(flagVariableToSet, static_cast<uint8_t>(flagVariableValue));
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
            }
            break;
        }
        default:
        {
            // Use the default controls
            basicMenuLayoutControls(menuPtr, button);
            break;
        }
    }
}

// Menu for determining which set of variables/flags to work with
static const MenuOption gInitOptions[] = {
    "Set GSWs",
    selectedOptionSetVariables,

    "Set GSWFs",
    selectedOptionSetFlags,

    "Set GWs",
    selectedOptionSetVariables,

    "Set GFs",
    selectedOptionSetFlags,

    "Set LSWs",
    selectedOptionSetVariables,

    "Set LSWFs",
    selectedOptionSetFlags,
};

static const MenuFunctions gInitFuncs = {
    gInitOptions,
    basicMenuLayoutControls,
    basicMenuLayoutDraw,
    nullptr, // Exit function not needed
};

void cheatsMenuManageFlagsInit(Menu *menuPtr)
{
    (void)menuPtr;

    constexpr uint32_t totalOptions = sizeof(gInitOptions) / sizeof(MenuOption);
    enterNextMenu(&gInitFuncs, totalOptions);
}

static void drawManageFlagsInfo()
{
    // Initialize text drawing
    drawTextInit(false);

    // Get the text position for the top-left of the window
    float tempPosX;
    float tempPosY;
    constexpr float scale = MENU_SCALE;
    gRootWindow->getTextPosXY(nullptr, WindowAlignment::TOP_LEFT, scale, &tempPosX, &tempPosY);

    // Determine if working with variables or flags
    const CheatsMenu *cheatsMenuPtr = gCheatsMenu;
    const uint32_t selectedOption = cheatsMenuPtr->getCurrentIndex();
    const char **linesPtr;
    bool changingWord;

    switch (selectedOption)
    {
        case ManageFlagsOptions::MANAGE_FLAGS_GSW:
        case ManageFlagsOptions::MANAGE_FLAGS_GW:
        case ManageFlagsOptions::MANAGE_FLAGS_LSW:
        {
            linesPtr = gGlobalWordsOptions;
            changingWord = true;
            break;
        }
        case ManageFlagsOptions::MANAGE_FLAGS_GSWF:
        case ManageFlagsOptions::MANAGE_FLAGS_GF:
        case ManageFlagsOptions::MANAGE_FLAGS_LSWF:
        {
            linesPtr = gGlobalFlagsOptions;
            changingWord = false;
            break;
        }
        default:
        {
            // Shouldn't reach this
            return;
        }
    }

    const char *flagVariableText;
    switch (selectedOption)
    {
        case ManageFlagsOptions::MANAGE_FLAGS_GSW:
        {
            flagVariableText = "GSW";
            break;
        }
        case ManageFlagsOptions::MANAGE_FLAGS_GSWF:
        {
            flagVariableText = "GSWF";
            break;
        }
        case ManageFlagsOptions::MANAGE_FLAGS_GW:
        {
            flagVariableText = "GW";
            break;
        }
        case ManageFlagsOptions::MANAGE_FLAGS_GF:
        {
            flagVariableText = "GF";
            break;
        }
        case ManageFlagsOptions::MANAGE_FLAGS_LSW:
        {
            flagVariableText = "LSW";
            break;
        }
        case ManageFlagsOptions::MANAGE_FLAGS_LSWF:
        {
            flagVariableText = "LSWF";
            break;
        }
        default:
        {
            // Shouldn't reach this
            return;
        }
    }

    // Draw the main text
    char buf[64];
    constexpr uint32_t bufSize = sizeof(buf);

    const Menu *menuPtr = gMenu;
    const uint32_t totalOptions = menuPtr->getTotalOptions();
    const uint32_t currentIndex = menuPtr->getCurrentIndex();
    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    float posX = tempPosX;
    float posY = tempPosY;

    for (uint32_t i = 0, lineIndex = 0; i < totalOptions; i++)
    {
        const uint32_t color = getCurrentOptionColor(currentIndex == i, 0xFF);

        if (i == 0)
        {
            snprintf(buf, bufSize, "Change %s", flagVariableText);
            drawText(buf, posX, posY, scale, color);
        }
        else
        {
            drawText(linesPtr[lineIndex++], posX, posY, scale, color);
        }

        posY -= lineDecrement;
    }

    // Draw the text for the values
    posY -= lineDecrement;
    const uint32_t flagVariableToSet = cheatsMenuPtr->getFlagVariableToSet();
    const uint32_t currentFlagVariableValue = getGlobalVariableFlagValue(selectedOption, flagVariableToSet);

    // Get the text position a bit to the right of the longest text
    const float width = getTextWidth("Current Value", scale);
    const float widthAdjustment = width + (40.f * scale);

    if (changingWord)
    {
        // Draw the text descriptions
        snprintf(buf, bufSize, "%s To Set\nCurrent Value\nNew Value", flagVariableText);
        drawText(buf, posX, posY, scale, getColorWhite(0xFF));

        // Draw the values
        const char *format;
        if (selectedOption == ManageFlagsOptions::MANAGE_FLAGS_GW)
        {
            // Draw the values as hex
            format = "%" PRIu32 "\n0x%08" PRIX32 "\n0x%08" PRIX32;
        }
        else
        {
            format = "%" PRIu32 "\n%" PRIu32 "\n%" PRIu32;
        }

        snprintf(buf, bufSize, format, flagVariableToSet, currentFlagVariableValue, cheatsMenuPtr->getFlagVariableValue());
        drawText(buf, posX + widthAdjustment, posY, scale, getColorWhite(0xFF));
    }
    else
    {
        // Draw the text descriptions
        snprintf(buf, bufSize, "%s To Set\nCurrent Value", flagVariableText);
        drawText(buf, posX, posY, scale, getColorWhite(0xFF));

        // Draw the values
        const char *string;
        uint32_t color;
        getOnOffTextAndColor(currentFlagVariableValue, &string, &color, 0xFF);

        snprintf(buf, bufSize, "%" PRIu32 "\n<col %" PRIx32 ">%s", flagVariableToSet, color, string);
        drawText(buf, posX + widthAdjustment, posY, scale, getColorWhite(0xFF));
    }
}

static void draw(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Draw the main window
    drawMainWindow();

    // Draw the info for the flags/variables
    drawManageFlagsInfo();

    // Draw the value editor if applicable
    ValueEditor *valueEditorPtr = gCheatsMenu->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->draw();
    }
}

uint32_t getGW(uint32_t gw)
{
    return evtGetWork()->gwData[gw];
}

void setGW(uint32_t gw, uint32_t value)
{
    evtGetWork()->gwData[gw] = value;
}

bool getGF(uint32_t gf)
{
    constexpr uint32_t totalFlags = sizeof(EvtWork::gfData) * 8;
    return _flagIsSet(evtGetWork()->gfData, gf, totalFlags, totalFlags);
}

void toggleGF(uint32_t gf)
{
    constexpr uint32_t totalFlags = sizeof(EvtWork::gfData) * 8;
    _toggleFlag(evtGetWork()->gfData, gf, totalFlags, totalFlags);
}

uint32_t getGlobalVariableFlagValue(uint32_t selectedOption, uint32_t variableFlag)
{
    switch (selectedOption)
    {
        case ManageFlagsOptions::MANAGE_FLAGS_GSW:
        {
            return swByteGet(variableFlag);
        }
        case ManageFlagsOptions::MANAGE_FLAGS_GSWF:
        {
            return static_cast<uint32_t>(swGet(variableFlag));
        }
        case ManageFlagsOptions::MANAGE_FLAGS_GW:
        {
            return getGW(variableFlag);
        }
        case ManageFlagsOptions::MANAGE_FLAGS_GF:
        {
            return static_cast<uint32_t>(getGF(variableFlag));
        }
        case ManageFlagsOptions::MANAGE_FLAGS_LSW:
        {
            return _swByteGet(variableFlag);
        }
        case ManageFlagsOptions::MANAGE_FLAGS_LSWF:
        {
            return static_cast<uint32_t>(_swGet(variableFlag));
        }
        default:
        {
            return 0;
        }
    }
}
