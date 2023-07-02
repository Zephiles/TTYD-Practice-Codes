#include "drawText.h"
#include "menuUtils.h"
#include "classes/buttonComboEditor.h"
#include "classes/window.h"
#include "gc/pad.h"
#include "misc/utils.h"
#include "ttyd/mariost.h"
#include "ttyd/system.h"

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cinttypes>

const char *gButtonComboEditorHelpText =
    "Hold the button(s) for 3 seconds to set the\nnew button combo\n\nPress B three times in succession to cancel";

void ButtonComboEditor::init(const Window *parentWindow, float scale)
{
    this->init(parentWindow, scale, 0xFF);
}

void ButtonComboEditor::init(const Window *parentWindow, float scale, uint8_t alpha)
{
    this->scale = scale;
    this->alpha = alpha;
    this->setComboFunc = nullptr;
    this->cancelFunc = nullptr;
    this->timer = 0;
    this->bButtonCounter = 0;
    this->enabled = false;

    clearMemory(this->buttonsHeldPrev, sizeof(this->buttonsHeldPrev));
    clearMemory(this->buttonsHeldCurrent, sizeof(this->buttonsHeldCurrent));

    // Set up the window
    // Initialize it based on the help text
    Window *windowPtr = &this->window;
    windowPtr->setWidthHeightFromTextAndInit(gButtonComboEditorHelpText, scale, SPECIAL_WINDOW_COLOR | alpha, 20.f, 30.f);

    // Increase the height of the window to account for the rest of the text, which is assumed to use six more lines
    const float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    const float padding = windowPtr->getPadding() * scale;
    windowPtr->setHeight(windowPtr->getHeight() + (lineDecrement * 5.f) + (padding / 2.f));

    // Place the window inside of the parent window
    windowPtr->placeInWindow(parentWindow, WindowAlignment::MIDDLE_CENTER, scale);
}

bool ButtonComboEditor::checkIfBPressedThreeTimes(uint32_t buttonsHeld, MenuButtonInput button)
{
    uint32_t bButtonCounter = this->bButtonCounter;
    if (button == MenuButtonInput::B)
    {
        // Make sure no other buttons are currently being held
        if (buttonsHeld == PadInput::PAD_B)
        {
            // Increment the B Button counter
            bButtonCounter++;
            this->setBButtonCounter(bButtonCounter);
        }
        else
        {
            return false;
        }
    }
    else if (button != MenuButtonInput::BUTTON_NONE)
    {
        // Another button has been pressed, so reset the B Button counter
        bButtonCounter = 0;
        this->setBButtonCounter(bButtonCounter);
        return false;
    }

    // Close this window if the B button has been pressed 3 times in succession
    if (bButtonCounter >= 3)
    {
        bButtonCounter = 0;
        this->setBButtonCounter(bButtonCounter);
        return true;
    }
    else
    {
        return false;
    }
}

bool ButtonComboEditor::decrementTimerAndCheckIfZero(uint32_t buttonsHeld)
{
    // L + Start are reserved for opening/closing the menu, so don't allow both to be used at the same time for a button combo
    if (checkButtonComboEveryFrame(OPEN_CLOSE_MENU_BUTTON_COMBO))
    {
        // L + Start are currently held, so reset the timer
        this->setTimer(sysMsec2Frame(3000));
        return false;
    }
    else if (buttonsHeld)
    {
        uint32_t i = 0;
        bool foundDifference = false;
        MenuButtonInput *buttonsHeldPrevPtr = this->buttonsHeldPrev;
        const MenuButtonInput *buttonsHeldCurrentPtr = this->buttonsHeldCurrent;

        for (; i < TOTAL_MENU_INPUT_BUTTONS; i++)
        {
            const MenuButtonInput currentButton = buttonsHeldCurrentPtr[i];

            if (currentButton == MenuButtonInput::BUTTON_NONE)
            {
                // Reached the end of the inputs, so exit
                break;
            }

            if (buttonsHeldPrevPtr[i] != currentButton)
            {
                foundDifference = true;
                break;
            }
        }

        if (buttonsHeldPrevPtr[i] != MenuButtonInput::BUTTON_NONE)
        {
            // Button(s) were released
            foundDifference = true;
        }

        if (foundDifference)
        {
            // New button(s) were pressed, so reset the timer
            this->setTimer(sysMsec2Frame(3000));

            // Copy the values from the current buttons held to the previous buttons held
            memcpy(buttonsHeldPrevPtr, buttonsHeldCurrentPtr, TOTAL_MENU_INPUT_BUTTONS);
            return false;
        }
        else
        {
            // Decrement the timer
            const uint32_t timer = this->timer - 1;
            this->setTimer(timer);
            return timer == 0;
        }
    }
    else
    {
        // No buttons are currently held, so reset the timer
        this->setTimer(sysMsec2Frame(3000));
        return false;
    }
}

void ButtonComboEditor::controls(MenuButtonInput button)
{
    const uint32_t buttonsHeld = keyGetButton(PadId::CONTROLLER_ONE);

    // Close this window if the B button has been pressed 3 times in succession
    if (this->checkIfBPressedThreeTimes(buttonsHeld, button))
    {
        const ButtonComboEditorCancelFunc func = this->cancelFunc;
        if (func)
        {
            func();
        }

        return;
    }

    // Get the buttons pressed this frame
    getButtonsPressed(this->buttonsHeldCurrent, buttonsHeld);

    // Decrement the timer and check if it's at 0
    if (this->decrementTimerAndCheckIfZero(buttonsHeld))
    {
        // The timer reached 0
        const ButtonComboEditorSetComboFunc func = this->setComboFunc;
        if (func)
        {
            func(buttonsHeld);
        }

        return;
    }
}

void ButtonComboEditor::draw() const
{
    // Draw the main window
    const Window *windowPtr = &this->window;
    windowPtr->draw();

    // Initialize text drawing
    drawTextInit(false);

    // Draw the help text
    float tempPosX;
    float tempPosY;
    const float scale = this->scale;
    windowPtr->getTextPosXY(nullptr, WindowAlignment::TOP_LEFT, scale, &tempPosX, &tempPosY);

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    const float posX = tempPosX;
    float posY = tempPosY;
    const char *mainHelpText = gButtonComboEditorHelpText;
    drawText(mainHelpText, posX, posY, scale, getColorWhite(0xFF));

    // Get the position three lines under the help text
    float height;
    getTextWidthHeight(mainHelpText, scale, nullptr, &height);

    const float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    posY -= (height + (lineDecrement * 2.f));

    // Draw the amount of time left
    const uint32_t currentTime = this->timer;
    const uint32_t currentFps = static_cast<uint32_t>(globalWorkPtr->framerate);
    const uint32_t seconds = (currentTime / currentFps) % 60;
    const uint32_t frames = currentTime % currentFps;

    char buf[128];
    constexpr uint32_t bufSize = sizeof(buf);

    snprintf(buf, bufSize, "Time Left: %" PRIu32 ".%02" PRIu32, seconds, frames);
    drawText(buf, posX, posY, scale, getColorWhite(0xFF));
    posY -= (lineDecrement * 2.f);

    // Draw the text for telling what buttons are currently held
    drawText("Currently held buttons", posX, posY, scale, getColorWhite(0xFF));
    posY -= lineDecrement;

    // Draw the currently held buttons
    // Do not draw if L + Start are held, as those are reserved for opening/closing the menu
    if (!checkButtonComboEveryFrame(OPEN_CLOSE_MENU_BUTTON_COMBO))
    {
        buttonsToString(keyGetButton(PadId::CONTROLLER_ONE), buf, bufSize);
        drawText(buf, posX, posY, scale, getColorWhite(0xFF));
    }
}

void getButtonsPressed(MenuButtonInput *buttonsOut, uint32_t buttons)
{
    // Make sure buttonsOut is cleared out before getting the buttons
    clearMemory(buttonsOut, TOTAL_MENU_INPUT_BUTTONS);

    uint32_t counter = 1;
    uint32_t index = 0;

    for (uint32_t i = 0; i < TOTAL_MENU_INPUT_BUTTONS; i++, counter++)
    {
        if (i == 7)
        {
            // Skip unused value
            i++;
        }

        if ((buttons >> i) & 1U)
        {
            buttonsOut[index] = static_cast<MenuButtonInput>(counter);
            index++;
        }
    }
}

void buttonsToString(uint32_t buttons, char *stringOut, uint32_t stringSize)
{
    // Clear the first byte in stringOut, as it will not be modified if no buttons are pressed
    stringOut[0] = '\0';

    // Get the text for each button
    MenuButtonInput buttonsArray[TOTAL_MENU_INPUT_BUTTONS];
    getButtonsPressed(buttonsArray, buttons);

    int32_t textLength = 0;
    const char *buttonText;

    for (uint32_t i = 0; i < TOTAL_MENU_INPUT_BUTTONS; i++)
    {
        const MenuButtonInput currentButton = buttonsArray[i];

        if (currentButton == MenuButtonInput::BUTTON_NONE)
        {
            // Reached the end of the inputs, so exit
            break;
        }

        switch (currentButton)
        {
            case MenuButtonInput::DPAD_LEFT:
            {
                buttonText = "D-Pad Left";
                break;
            }
            case MenuButtonInput::DPAD_RIGHT:
            {
                buttonText = "D-Pad Right";
                break;
            }
            case MenuButtonInput::DPAD_DOWN:
            {
                buttonText = "D-Pad Down";
                break;
            }
            case MenuButtonInput::DPAD_UP:
            {
                buttonText = "D-Pad Up";
                break;
            }
            case MenuButtonInput::Z:
            {
                buttonText = "Z";
                break;
            }
            case MenuButtonInput::R:
            {
                buttonText = "R";
                break;
            }
            case MenuButtonInput::L:
            {
                buttonText = "L";
                break;
            }
            case MenuButtonInput::A:
            {
                buttonText = "A";
                break;
            }
            case MenuButtonInput::B:
            {
                buttonText = "B";
                break;
            }
            case MenuButtonInput::X:
            {
                buttonText = "X";
                break;
            }
            case MenuButtonInput::Y:
            {
                buttonText = "Y";
                break;
            }
            case MenuButtonInput::START:
            {
                buttonText = "Start";
                break;
            }
            default:
            {
                continue;
            }
        }

        if (i == 0)
        {
            // Set the initial button pressed
            textLength = snprintf(stringOut, stringSize, "%s", buttonText);
        }
        else
        {
            // Add the next button pressed
            textLength += snprintf(&stringOut[textLength], stringSize - textLength, " + %s", buttonText);

            // Exit if textLength somehow reaches stringSize
            if (static_cast<uint32_t>(textLength) >= stringSize)
            {
                break;
            }
        }
    }
}
