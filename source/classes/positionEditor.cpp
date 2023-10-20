#include "menuUtils.h"
#include "drawText.h"
#include "classes/positionEditor.h"
#include "classes/window.h"
#include "gc/pad.h"
#include "misc/utils.h"
#include "ttyd/system.h"

#include <cstdint>

static const char *gHelpText =
    "Press/Hold the D-Pad directions\nto adjust the position\n\nHold Y to hide this window\n\nPress A to confirm\n\nPress B to "
    "cancel";

void PositionEditor::init(const Window *parentWindow, float *posXSource, float *posYSource)
{
    this->init(parentWindow, posXSource, posYSource, 0xFF);
}

void PositionEditor::init(const Window *parentWindow, float *posXSource, float *posYSource, uint8_t windowAlpha)
{
    this->confirmFunc = nullptr;
    this->cancelFunc = nullptr;
    this->posX = *posXSource;
    this->posY = *posYSource;
    this->posXSource = posXSource;
    this->posYSource = posYSource;
    this->enabled = false;

    // Set up the window
    // Initialize it based on the help text
    Window *windowPtr = &this->window;
    constexpr float scale = MENU_SCALE;
    windowPtr->setWidthHeightFromTextAndInit(gHelpText, scale, SPECIAL_WINDOW_COLOR | windowAlpha, 20.f, 20.f);

    // Place the window inside of the parent window
    windowPtr->placeInWindow(parentWindow, WindowAlignment::TOP_CENTER, scale);
}

bool PositionEditor::shouldAutoIncrement()
{
    // Check to see if any of the D-Pad buttons are held
    constexpr uint32_t buttons =
        PadInput::PAD_DPAD_LEFT | PadInput::PAD_DPAD_RIGHT | PadInput::PAD_DPAD_DOWN | PadInput::PAD_DPAD_UP;

    if (!(keyGetButton(PadId::CONTROLLER_ONE) & buttons))
    {
        // Reset the counter
        this->waitFramesToBegin = 0;
        return false;
    }

    // Check to see if the value should begin to auto-increment
    // Require holding a direction for 500ms before auto-incrementing starts
    uint32_t waitFramesToBegin = this->waitFramesToBegin;
    if (waitFramesToBegin < sysMsec2Frame(500))
    {
        this->waitFramesToBegin = static_cast<uint16_t>(++waitFramesToBegin);
        return false;
    }

    // Auto-increment the value
    return true;
}

void PositionEditor::dpadControls(MenuButtonInput button)
{
    constexpr float adjustmentValue = 1.f;

    switch (button)
    {
        case MenuButtonInput::DPAD_LEFT:
        {
            this->posX -= adjustmentValue;
            break;
        }
        case MenuButtonInput::DPAD_RIGHT:
        {
            this->posX += adjustmentValue;
            break;
        }
        case MenuButtonInput::DPAD_DOWN:
        {
            this->posY -= adjustmentValue;
            break;
        }
        case MenuButtonInput::DPAD_UP:
        {
            this->posY += adjustmentValue;
            break;
        }
        default:
        {
            break;
        }
    }
}

void PositionEditor::controls(MenuButtonInput button)
{
    // The function for checking for auto-incrementing needs to run every frame to be handled correctly
    const bool autoIncrement = this->shouldAutoIncrement();

    // Handle held button inputs if auto-incrementing should be done
    if (autoIncrement)
    {
        const MenuButtonInput buttonHeld = getMenuButtonInput(false);
        switch (buttonHeld)
        {
            case MenuButtonInput::DPAD_LEFT:
            case MenuButtonInput::DPAD_RIGHT:
            case MenuButtonInput::DPAD_DOWN:
            case MenuButtonInput::DPAD_UP:
            {
                this->dpadControls(buttonHeld);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    // Handle the button inputs pressed this frame
    switch (button)
    {
        case MenuButtonInput::DPAD_LEFT:
        case MenuButtonInput::DPAD_RIGHT:
        case MenuButtonInput::DPAD_DOWN:
        case MenuButtonInput::DPAD_UP:
        {
            this->dpadControls(button);
            break;
        }
        case MenuButtonInput::A:
        {
            // Run the function for when confirming to set the new position
            const PositionEditorConfirmFunc func = this->confirmFunc;
            if (func && !func())
            {
                return;
            }

            *this->posXSource = this->posX;
            *this->posYSource = this->posY;
            return;
        }
        case MenuButtonInput::B:
        {
            const PositionEditorCancelFunc func = this->cancelFunc;
            if (func)
            {
                return func();
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

void PositionEditor::draw() const
{
    // If Y is held, then do not draw anything
    if (checkButtonComboEveryFrame(PadInput::PAD_Y))
    {
        return;
    }

    // Draw the main window
    const Window *windowPtr = &this->window;
    windowPtr->draw();

    // Initialize text drawing
    drawTextInit(false);

    // Draw the help text
    float posX;
    float posY;
    constexpr float scale = MENU_SCALE;

    windowPtr->getTextPosXY(nullptr, WindowAlignment::TOP_LEFT, scale, &posX, &posY);
    drawText(gHelpText, posX, posY, scale, getColorWhite(0xFF));
}
