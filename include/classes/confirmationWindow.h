#ifndef CLASSES_CONFIRMATIONWINDOW_H
#define CLASSES_CONFIRMATIONWINDOW_H

#include "classes/optionSelector.h"
#include "classes/window.h"

#include <cstdint>

#define CONFIRMATION_WINDOW_OPTION_YES 0
#define CONFIRMATION_WINDOW_OPTION_NO (CONFIRMATION_WINDOW_OPTION_YES + 1)

// Called when the player selects yes/no or presses B to cancel
typedef void (*ConfirmationWindowSelectedOptionFunc)(bool selectedYes);

class ConfirmationWindow: private OptionSelector
{
   public:
    ConfirmationWindow() {}
    ~ConfirmationWindow() {}

    // Sets windowAlpha to 0xFF
    void init(const Window *parentWindow, const char *helpText);

    void init(const Window *parentWindow, const char *helpText, uint8_t windowAlpha);

    bool shouldDraw() const { return this->OptionSelector::shouldDraw(); }
    void stopDrawing();
    void startDrawing(ConfirmationWindowSelectedOptionFunc selectedOptionFunc);

    void controls(MenuButtonInput button) { this->OptionSelector::controls(button); }
    void draw() const { this->OptionSelector::draw(); }

    ConfirmationWindowSelectedOptionFunc getSelectedOptionFunc() const { return this->selectedOptionFunc; }

   private:
    ConfirmationWindowSelectedOptionFunc selectedOptionFunc;
};

#endif
