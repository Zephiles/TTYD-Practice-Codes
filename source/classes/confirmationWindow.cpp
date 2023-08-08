#include "drawText.h"
#include "menuUtils.h"
#include "classes/confirmationWindow.h"
#include "classes/window.h"

#include <cstdint>

const char *gConfirmationWindowYesNoStrings[] = {
    "Yes",
    "No",
};

void ConfirmationWindow::init(const Window *parentWindow, const char *helpText)
{
    this->init(parentWindow, helpText, 0xFF);
}

void ConfirmationWindow::init(const Window *parentWindow, const char *helpText, uint8_t windowAlpha)
{
    this->helpText = helpText;
    this->selectedOptionFunc = nullptr;
    this->autoIncrement.framesBeforeIncrement = 0;
    this->autoIncrement.waitFramesToBegin = 0;
    this->enabled = false;

    // Start on no
    this->currentIndex = ConfirmationWindowOptions::No;

    // Set up the window
    // Initialize it based on the help text
    Window *windowPtr = &this->window;
    constexpr float scale = MENU_SCALE;
    windowPtr->setWidthHeightFromTextAndInit(helpText, scale, SPECIAL_WINDOW_COLOR | windowAlpha, 20.f, 30.f);

    // Increase the height of the window to account for the yes/no text
    constexpr float yesNoScale = scale + 0.1f;
    const float padding = windowPtr->getPadding() * yesNoScale;
    windowPtr->setHeight(windowPtr->getHeight() + ((LINE_HEIGHT_FLOAT * yesNoScale) * 2.f) + padding);

    // Place the window inside of the parent window
    windowPtr->placeInWindow(parentWindow, WindowAlignment::MIDDLE_CENTER, scale);
}

void ConfirmationWindow::callSelectedOptionFunc(bool selectedYes) const
{
    const ConfirmationWindowSelectedOptionFunc func = this->selectedOptionFunc;
    if (func)
    {
        return func(selectedYes);
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

            return this->callSelectedOptionFunc(selectedYes);
        }
        case MenuButtonInput::B:
        {
            return this->callSelectedOptionFunc(false);
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
    constexpr float scale = MENU_SCALE;

    windowPtr->getTextPosXY(nullptr, WindowAlignment::TOP_LEFT, scale, &tempPosX, &tempPosY);
    drawText(this->helpText, tempPosX, tempPosY, scale, getColorWhite(0xFF));

    // Draw Yes and No
    constexpr float yesNoScale = scale + 0.1f;
    const char **yesNoStringsPtr = gConfirmationWindowYesNoStrings;

    // getTextPosY only uses one scale, while two are required here, so have to calculate posY manually
    float yesNoHeight;
    const char *currentString = yesNoStringsPtr[0];
    getTextWidthHeight(currentString, yesNoScale, nullptr, &yesNoHeight);

    const float posX = windowPtr->getTextPosX(currentString, WindowAlignment::BOTTOM_CENTER, yesNoScale);

    float posY = windowPtr->getPosY() - windowPtr->getHeight() + yesNoHeight + (windowPtr->getPadding() * scale) +
                 LINE_HEIGHT_ADJUSTMENT_4(scale);

    const uint32_t currentIndex = this->currentIndex;
    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * yesNoScale;

    // Starting Y coordinate is for No, so draw No first
    for (int32_t i = ConfirmationWindowOptions::No; i >= ConfirmationWindowOptions::Yes; i--)
    {
        const uint32_t color = getCurrentOptionColor(currentIndex == static_cast<uint32_t>(i), 0xFF);
        drawText(yesNoStringsPtr[i], posX, posY, yesNoScale, color);
        posY += lineDecrement;
    }
}
