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

const char *gCheatsMenuManageFlagsGlobalWordsOptions[] = {
    "Change Value",
    "Set New Value", // The code assumes that Set New Value will be the last option
};

const char *gCheatsMenuManageFlagsGlobalFlagsOptions[] = {
    "Toggle Value", // The code assumes that Toggle Value will be the last option
};

void cheatsMenuManageFlagsSetValue(const ValueType *valuePtr)
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
const MenuFunctions gCheatsMenuManageFlagsFlagsFuncs = {
    nullptr, // The first line of the options will be different depending on which flag was selected, so all of the options must
             // be drawn manually

    cheatsMenuManageFlagsFlagsControls,
    cheatsMenuManageFlagsDraw,
    nullptr, // Exit function not needed
};

void cheatsMenuManageFlagsFlagsInit(Menu *menuPtr)
{
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;

    // Back up the current index
    cheatsMenuPtr->setCurrentIndex(menuPtr->getCurrentIndex());

    // Reset flagVariableToSet and flagVariableValue
    cheatsMenuPtr->setFlagVariableToSet(0);
    cheatsMenuPtr->setFlagVariableValue(0);

    constexpr uint32_t totalOptions = (sizeof(gCheatsMenuManageFlagsGlobalFlagsOptions) / sizeof(const char *)) + 1;
    enterNextMenu(&gCheatsMenuManageFlagsFlagsFuncs, totalOptions);
}

void cheatsMenuManageFlagsFlagsControls(Menu *menuPtr, MenuButtonInput button)
{
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;

    // If the value editor is open, then handle the controls for that
    if (menuPtr->flagIsSet(CHEATS_MENU_USING_VALUE_EDITOR_FLAG))
    {
        cheatsMenuPtr->getValueEditorPtr()->controls(button);
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

                cheatsMenuInitValueEditor(menuPtr,
                                          currentValue,
                                          minValue,
                                          maxValue,
                                          flags,
                                          VariableType::u32,
                                          true,
                                          cheatsMenuManageFlagsSetValue);
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
const MenuFunctions gCheatsMenuManageFlagsVariableFuncs = {
    nullptr, // The first line of the options will be different depending on which non-flag variable was selected, so all of the
             // options must be drawn manually

    cheatsMenuManageFlagsVariablesControls,
    cheatsMenuManageFlagsDraw,
    nullptr, // Exit function not needed
};

void cheatsMenuManageFlagsVariablesInit(Menu *menuPtr)
{
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;

    // Back up the current index
    cheatsMenuPtr->setCurrentIndex(menuPtr->getCurrentIndex());

    // Reset flagVariableToSet and flagVariableValue
    cheatsMenuPtr->setFlagVariableToSet(0);
    cheatsMenuPtr->setFlagVariableValue(0);

    constexpr uint32_t totalOptions = (sizeof(gCheatsMenuManageFlagsGlobalWordsOptions) / sizeof(const char *)) + 1;
    enterNextMenu(&gCheatsMenuManageFlagsVariableFuncs, totalOptions);
}

void cheatsMenuManageFlagsVariablesControls(Menu *menuPtr, MenuButtonInput button)
{
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;

    // If the value editor is open, then handle the controls for that
    if (menuPtr->flagIsSet(CHEATS_MENU_USING_VALUE_EDITOR_FLAG))
    {
        cheatsMenuPtr->getValueEditorPtr()->controls(button);
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

                cheatsMenuInitValueEditor(menuPtr,
                                          currentValue,
                                          minValue,
                                          maxValue,
                                          flags,
                                          VariableType::u32,
                                          hasMinAndMax,
                                          cheatsMenuManageFlagsSetValue);
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
const MenuOption gCheatsMenuManageFlagsInitOptions[] = {
    "Set GSWs",
    cheatsMenuManageFlagsVariablesInit,

    "Set GSWFs",
    cheatsMenuManageFlagsFlagsInit,

    "Set GWs",
    cheatsMenuManageFlagsVariablesInit,

    "Set GFs",
    cheatsMenuManageFlagsFlagsInit,

    "Set LSWs",
    cheatsMenuManageFlagsVariablesInit,

    "Set LSWFs",
    cheatsMenuManageFlagsFlagsInit,
};

const MenuFunctions gCheatsMenuManageFlagsInitFuncs = {
    gCheatsMenuManageFlagsInitOptions,
    basicMenuLayoutControls,
    basicMenuLayoutDraw,
    nullptr, // Exit function not needed
};

void cheatsMenuManageFlagsInit(Menu *menuPtr)
{
    (void)menuPtr;

    constexpr uint32_t totalOptions = sizeof(gCheatsMenuManageFlagsInitOptions) / sizeof(MenuOption);
    enterNextMenu(&gCheatsMenuManageFlagsInitFuncs, totalOptions);
}

void CheatsMenu::drawManageFlagsInfo() const
{
    // Initialize text drawing
    drawTextInit(false);

    // Get the text position for the top-left of the window
    float tempPosX;
    float tempPosY;
    constexpr float scale = MENU_SCALE;
    gRootWindow->getTextPosXY(nullptr, WindowAlignment::TOP_LEFT, scale, &tempPosX, &tempPosY);

    // Determine if working with variables or flags
    bool changingWord;
    const char **lines;

    const uint32_t selectedOption = this->currentIndex;
    switch (selectedOption)
    {
        case ManageFlagsOptions::MANAGE_FLAGS_GSW:
        case ManageFlagsOptions::MANAGE_FLAGS_GW:
        case ManageFlagsOptions::MANAGE_FLAGS_LSW:
        {
            changingWord = true;
            lines = gCheatsMenuManageFlagsGlobalWordsOptions;
            break;
        }
        case ManageFlagsOptions::MANAGE_FLAGS_GSWF:
        case ManageFlagsOptions::MANAGE_FLAGS_GF:
        case ManageFlagsOptions::MANAGE_FLAGS_LSWF:
        {
            changingWord = false;
            lines = gCheatsMenuManageFlagsGlobalFlagsOptions;
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
            drawText(lines[lineIndex++], posX, posY, scale, color);
        }

        posY -= lineDecrement;
    }

    // Draw the text for the values
    // Get the text position a bit to the right of the longest text
    float width;
    getTextWidthHeight("Current Value", scale, &width, nullptr);
    const float widthAdjustment = width + (40.f * scale);

    posY -= lineDecrement;
    const uint32_t flagVariableToSet = this->flagVariableToSet;
    const uint32_t currentFlagVariableValue = getGlobalVariableFlagValue(selectedOption, flagVariableToSet);

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

        snprintf(buf, bufSize, format, flagVariableToSet, currentFlagVariableValue, this->flagVariableValue);
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

void cheatsMenuManageFlagsDraw(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Draw the main window
    drawMainWindow();

    // Draw the info for the flags/variables
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;
    cheatsMenuPtr->drawManageFlagsInfo();

    // Draw the value editor if applicable
    ValueEditor *valueEditorPtr = cheatsMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->draw();
    }
}

uint32_t getGW(uint32_t gw)
{
    EvtWork *eventWork = evtGetWork();
    return eventWork->gwData[gw];
}

void setGW(uint32_t gw, uint32_t value)
{
    EvtWork *eventWork = evtGetWork();
    eventWork->gwData[gw] = value;
}

bool getGF(uint32_t gf)
{
    EvtWork *eventWork = evtGetWork();
    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
    return (eventWork->gfData[gf / bitsPerWord] >> (gf % bitsPerWord)) & 1U;
}

void toggleGF(uint32_t gf)
{
    EvtWork *eventWork = evtGetWork();
    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
    eventWork->gfData[gf / bitsPerWord] ^= (1UL << (gf % bitsPerWord));
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
