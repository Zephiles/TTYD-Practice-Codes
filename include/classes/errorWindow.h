#ifndef CLASSES_ERRORWINDOW_H
#define CLASSES_ERRORWINDOW_H

#include "classes/menu.h"
#include "classes/window.h"

#include <cstdint>

class ErrorWindow
{
   public:
    ErrorWindow(const char *text, uint8_t windowAlpha)
    {
        this->windowAlpha = windowAlpha;
        this->setText(text);
    }

    ErrorWindow() {}
    ~ErrorWindow() {}

    // Initializes the text variable, and then calls setWidthHeightFromTextAndInit for the window. The window alpha needs to be
    // set before calling this function.
    void setText(const char *text);

    void setTimer(uint32_t milliseconds);

    void setAlpha(uint8_t windowAlpha) { this->windowAlpha = windowAlpha; }

    // The draw function sets shouldCheckForInputs to true, so this can be used to unset it to completely avoid checking for
    // button inputs for disabling the window
    void avoidCheckingForInputs() { this->shouldCheckForInputs = false; }

    void placeInWindow(const Window *parent, WindowAlignment alignment)
    {
        this->window.placeInWindow(parent, alignment, MENU_SCALE);
    }

    bool shouldDraw();
    void draw();

   private:
    Window window;
    const char *text;
    uint16_t timer;
    uint8_t windowAlpha;
    bool shouldCheckForInputs; // Used to avoid checking for button inputs on the frame that the window starts being drawn
};

#endif
