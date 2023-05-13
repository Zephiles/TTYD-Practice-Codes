#ifndef CLASSES_ERRORWINDOW_H
#define CLASSES_ERRORWINDOW_H

#include "classes/window.h"

#include <cstdint>

class ErrorWindow
{
   public:
    ErrorWindow(const char *text, float scale, uint8_t alpha)
    {
        this->scale = scale;
        this->alpha = alpha;
        this->setText(text);
    }

    ErrorWindow() {}
    ~ErrorWindow() {}

    // Initializes the text variable, and then calls setWidthHeightFromTextAndInit for the window. The scale and alpha need to
    // be set before calling this function.
    void setText(const char *text);

    void setTimer(uint32_t milliseconds);

    void setScale(float scale) { this->scale = scale; }
    void setAlpha(uint8_t alpha) { this->alpha = alpha; }

    // The draw function sets shouldCheckForInputs to true, so this can be used to unset it to completely avoid checking for
    // button inputs for disabling the window
    void avoidCheckingForInputs() { this->shouldCheckForInputs = false; }

    void placeInWindow(const Window *parent, WindowAlignment alignment)
    {
        this->window.placeInWindow(parent, alignment, this->scale);
    }

    bool shouldDraw();
    void draw();

   private:
    Window window;
    float scale;
    const char *text;
    uint16_t timer;
    uint8_t alpha;
    bool shouldCheckForInputs; // Used to avoid checking for button inputs on the frame that the window starts being drawn
};

#endif
