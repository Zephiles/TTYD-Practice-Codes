#include "drawText.h"
#include "menuUtils.h"
#include "classes/confirmationWindow.h"
#include "classes/window.h"
#include "classes/menu.h"

#include <cstdint>

const char *gConfirmationWindowYesNoStrings[] = {
    "Yes",
    "No",
};

void ConfirmationWindow::init(const Window *parentWindow, const char *helpText, float scale)
{
    this->init(parentWindow, helpText, scale, 0xFF);
}

void ConfirmationWindow::init(const Window *parentWindow, const char *helpText, float scale, uint8_t alpha)
{
    this->scale = scale;
    this->helpText = helpText;
    this->alpha = alpha;
    this->selectedOptionFunc = nullptr;
    this->autoIncrement.framesBeforeIncrement = 0;
    this->autoIncrement.waitFramesToBegin = 0;
    this->enabled = false;

    // Start on no
    this->currentIndex = ConfirmationWindowOptions::No;

    // Set up the window
    // Initialize it based on the help text
    Window *windowPtr = &this->window;
    windowPtr->setWidthHeightFromTextAndInit(helpText, scale, SPECIAL_WINDOW_COLOR | alpha, 20.f, 30.f);

    // Increase the height of the window to account for the yes/no text
    const float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    const float padding = windowPtr->getPadding() * scale;
    windowPtr->setHeight(windowPtr->getHeight() + (lineDecrement * 3.f) + (padding / 2.f));

    // Place the window inside of the parent window
    windowPtr->placeInWindow(parentWindow, WindowAlignment::MIDDLE_CENTER, scale);
}

void ConfirmationWindow::callSelectedOptionFunc(bool selectedYes) const
{
    const ConfirmationWindowSelectedOptionFunc func = this->selectedOptionFunc;
    if (func)
    {
        func(selectedYes);
    }
}

void ConfirmationWindow::dpadControls(MenuButtonInput button)
{
    switch (button)
    {
        case MenuButtonInput::DPAD_DOWN:
        case MenuButtonInput::DPAD_UP:
        {
            if (this->currentIndex == ConfirmationWindowOptions::Yes)
            {
                this->currentIndex = ConfirmationWindowOptions::No;
            }
            else
            {
                this->currentIndex = ConfirmationWindowOptions::Yes;
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

void ConfirmationWindow::controls(MenuButtonInput button)
{
    // The function for checking for auto-incrementing needs to run every frame to be handled correctly
    const bool autoIncrement = handleMenuAutoIncrement(&this->autoIncrement);

    // Handle held button inputs if auto-incrementing should be done
    if (autoIncrement)
    {
        const MenuButtonInput buttonHeld = getMenuButtonInput(false);
        switch (buttonHeld)
        {
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
        case MenuButtonInput::DPAD_DOWN:
        case MenuButtonInput::DPAD_UP:
        {
            this->dpadControls(button);
            break;
        }
        case MenuButtonInput::A:
        {
            bool selectedYes;
            if (this->currentIndex == ConfirmationWindowOptions::Yes)
            {
                selectedYes = true;
            }
            else
            {
                selectedYes = false;
            }

            this->callSelectedOptionFunc(selectedYes);
            break;
        }
        case MenuButtonInput::B:
        {
            this->callSelectedOptionFunc(false);
            break;
        }
        default:
        {
            break;
        }
    }
}

void ConfirmationWindow::draw() const
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
    drawText(this->helpText, tempPosX, tempPosY, scale, getColorWhite(0xFF));

    // Draw Yes and No
    const char **yesNoStringsPtr = gConfirmationWindowYesNoStrings;
    windowPtr->getTextPosXY(yesNoStringsPtr[0], WindowAlignment::BOTTOM_CENTER, scale, &tempPosX, &tempPosY);

    const uint32_t currentIndex = this->currentIndex;
    const float lineDecrement = LINE_HEIGHT_FLOAT * scale;

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    const float posX = tempPosX;
    float posY = tempPosY;

    for (uint32_t i = ConfirmationWindowOptions::Yes; i <= ConfirmationWindowOptions::No; i++)
    {
        // Starting Y coordinate is for No, so draw No first
        const uint32_t yesNoIndex = ConfirmationWindowOptions::No - i;
        const uint32_t color = getCurrentOptionColor(currentIndex == yesNoIndex, 0xFF);

        drawText(yesNoStringsPtr[yesNoIndex], posX, posY, scale, color);
        posY += lineDecrement;
    }
}
