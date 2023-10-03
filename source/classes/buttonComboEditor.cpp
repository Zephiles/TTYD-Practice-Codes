#include "drawText.h"
#include "menuUtils.h"
#include "classes/buttonComboEditor.h"
#include "classes/window.h"
#include "gc/pad.h"
#include "misc/utils.h"
#include "ttyd/mariost.h"
#include "ttyd/system.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

const char *gButtonComboEditorHelpText =
    "Hold the button(s) for 3 seconds to set the\nnew button combo\n\nPress B three times in succession to cancel";

void ButtonComboEditor::init(const Window *parentWindow)
{
    this->init(parentWindow, 0xFF);
}

void ButtonComboEditor::init(const Window *parentWindow, uint8_t windowAlpha)
{
    this->setComboFunc = nullptr;
    this->cancelFunc = nullptr;
    this->buttonsPrevFrame = 0;
    this->timer = 0;
    this->bButtonCounter = 0;
    this->enabled = false;

    // Set up the window
    // Initialize it based on the help text
    Window *windowPtr = &this->window;
    constexpr float scale = MENU_SCALE;
    windowPtr->setWidthHeightFromTextAndInit(gButtonComboEditorHelpText, scale, SPECIAL_WINDOW_COLOR | windowAlpha, 20.f, 30.f);

    // Increase the height of the window to account for the rest of the text, which is assumed to use six more lines
    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
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
            this->setBButtonCounter(++bButtonCounter);
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
    // L + Start is reserved for opening/closing the menu, so don't allow both to be used at the same time for a button combo
    if (checkButtonComboEveryFrame(OPEN_CLOSE_MENU_BUTTON_COMBO))
    {
        // L + Start is currently held, so reset the timer
        this->setTimer(sysMsec2Frame(3000));
        return false;
    }
    else if (buttonsHeld)
    {
        if (this->buttonsPrevFrame != buttonsHeld)
        {
            // Either new button(s) were pressed or some button(s) were released, so copy the current buttons to the previous
            // buttons
            this->setButtonsPrevFrame(buttonsHeld);

            // Reset the timer
            this->setTimer(sysMsec2Frame(3000));
            return false;
        }
        else
        {
            // Decrement the timer
            // Do not decrement if it is somehow already at 0
            uint32_t timer = this->timer;
            if (timer > 0)
            {
                this->setTimer(--timer);
            }

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
    // Get the buttons held this frame
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
    constexpr float scale = MENU_SCALE;
    windowPtr->getTextPosXY(nullptr, WindowAlignment::TOP_LEFT, scale, &tempPosX, &tempPosY);

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    const float posX = tempPosX;
    float posY = tempPosY;
    const char *mainHelpText = gButtonComboEditorHelpText;
    drawText(mainHelpText, posX, posY, scale, getColorWhite(0xFF));

    // Get the position three lines under the help text
    float height;
    getTextWidthHeight(mainHelpText, scale, nullptr, &height);

    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    posY -= (height + (lineDecrement * 2.f));

    // Draw the amount of time left
    const uint32_t currentTime = this->timer;
    const uint32_t currentFps = static_cast<uint32_t>(_globalWorkPtr->framerate);
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
    // Draw the default value if L + Start are held, as those are reserved for opening/closing the menu
    uint32_t heldButtons = 0;
    if (!checkButtonComboEveryFrame(OPEN_CLOSE_MENU_BUTTON_COMBO))
    {
        heldButtons = keyGetButton(PadId::CONTROLLER_ONE);
    }

    buttonsToString(heldButtons, buf, bufSize);
    drawText(buf, posX, posY, scale, getColorWhite(0xFF));
}

void buttonsToString(uint32_t buttons, char *stringOut, uint32_t stringSize)
{
    if (!buttons)
    {
        // No buttons are pressed, so set the default string to None
        // Use snprintf to make sure stringSize is not exceeded, and that a null terminator is properly applied
        snprintf(stringOut, stringSize, "None");
        return;
    }

    // Get the text for each button
    const char *buttonText;
    int32_t textLength = 0;
    bool firstStringWritten = false;

    for (uint32_t i = 0, counter = 1; i < TOTAL_MENU_INPUT_BUTTONS; i++, counter++)
    {
        if (i == 7)
        {
            // Skip unused value
            i++;
        }

        if (!((buttons >> i) & 1U))
        {
            // The current button is not held
            continue;
        }

        switch (static_cast<MenuButtonInput>(counter))
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
                // Unknown button
                continue;
            }
        }

        if (!firstStringWritten)
        {
            firstStringWritten = true;

            // Set the initial button pressed
            textLength = snprintf(stringOut, stringSize, buttonText);
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
