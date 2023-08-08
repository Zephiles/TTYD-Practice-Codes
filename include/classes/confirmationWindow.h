#ifndef CLASSES_CONFIRMATIONWINDOW_H
#define CLASSES_CONFIRMATIONWINDOW_H

#include "classes/window.h"
#include "classes/menu.h"

#include <cstdint>

typedef void (*ConfirmationWindowSelectedOptionFunc)(bool selectedYes);

class ConfirmationWindow
{
    enum ConfirmationWindowOptions
    {
        Yes = 0,
        No,
    };

   public:
    ConfirmationWindow() {}
    ~ConfirmationWindow() {}

    // Sets windowAlpha to 0xFF
    void init(const Window *parentWindow, const char *helpText);

    void init(const Window *parentWindow, const char *helpText, uint8_t windowAlpha);

    bool shouldDraw() const { return this->enabled; }
    void stopDrawing() { this->enabled = false; }

    void startDrawing(ConfirmationWindowSelectedOptionFunc selectedOptionFunc)
    {
        this->selectedOptionFunc = selectedOptionFunc;
        this->enabled = true;
    }

    void callSelectedOptionFunc(bool selectedYes) const;
    void dpadControls(MenuButtonInput button);
    void controls(MenuButtonInput button);

    void draw() const;

   private:
    Window window;

    ConfirmationWindowSelectedOptionFunc selectedOptionFunc; // Called when the player selects yes/no or presses B to cancel
    MenuAutoIncrement autoIncrement;
    const char *helpText;

    bool enabled; // Whether this window is enabled/drawn or not
    uint8_t currentIndex;
};

#endif
