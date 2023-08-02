#include "drawText.h"
#include "classes/errorWindow.h"
#include "classes/window.h"
#include "classes/menu.h"
#include "gc/pad.h"
#include "ttyd/system.h"

#include <cstdint>

void ErrorWindow::setText(const char *text)
{
    this->text = text;
    this->window.setWidthHeightFromTextAndInit(text, MENU_SCALE, SPECIAL_WINDOW_COLOR | this->alpha, 20.f, 20.f);
}

void ErrorWindow::setTimer(uint32_t milliseconds)
{
    this->timer = static_cast<uint16_t>(sysMsec2Frame(milliseconds));
}

bool ErrorWindow::shouldDraw()
{
    // if the timer is at 0, then the error message does not need to be drawn
    uint32_t timer = this->timer;
    if (timer == 0)
    {
        return false;
    }

    // If any buttons are pressed, then stop drawing the error message
    if (this->shouldCheckForInputs && keyGetButtonTrg(PadId::CONTROLLER_ONE))
    {
        this->timer = 0;
        return false;
    }

    // Decrement the timer, and if it reaches 0, then stop drawing the error message
    this->timer = static_cast<uint16_t>(--timer);
    if (timer == 0)
    {
        return false;
    }

    return true;
}

void ErrorWindow::draw()
{
    // Allow button inputs to be checked for disabling this window
    this->shouldCheckForInputs = true;

    // Draw the window
    const Window *windowPtr = &this->window;
    windowPtr->draw();

    // Initialize text drawing
    drawTextInit(false);

    // Draw the text
    float posX;
    float posY;
    constexpr float scale = MENU_SCALE;
    windowPtr->getTextPosXY(nullptr, WindowAlignment::TOP_LEFT, scale, &posX, &posY);

    drawText(this->text, posX, posY, scale, getColorWhite(0xFF));
}
