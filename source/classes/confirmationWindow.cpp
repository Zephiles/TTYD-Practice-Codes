#include "classes/confirmationWindow.h"
#include "classes/window.h"
#include "classes/menu.h"

#include <cstdint>

static const char *gYesNoStrings[] = {
    "Yes",
    "No",
};

static ConfirmationWindow *gConfirmationWindow = nullptr;

void ConfirmationWindow::init(const Window *parentWindow, const char *helpText)
{
    this->init(parentWindow, helpText, 0xFF);
}

void ConfirmationWindow::init(const Window *parentWindow, const char *helpText, uint8_t windowAlpha)
{
    constexpr uint32_t totalOptions = sizeof(gYesNoStrings) / sizeof(const char *);
    this->OptionSelector::init(helpText, gYesNoStrings, totalOptions, 1, parentWindow, windowAlpha, 20.f, MENU_SCALE);

    // Start on no
    this->OptionSelector::setCurrentIndex(CONFIRMATION_WINDOW_OPTION_NO);
}

void ConfirmationWindow::stopDrawing()
{
    gConfirmationWindow = nullptr;
    this->OptionSelector::stopDrawing();
}

static void confirmationWindowSelectedOption(uint32_t currentIndex)
{
    // Make sure gConfirmationWindow is set
    const ConfirmationWindow *confirmationWindowPtr = gConfirmationWindow;
    if (!confirmationWindowPtr)
    {
        return;
    }

    // Run the function for when an option is selected
    const ConfirmationWindowSelectedOptionFunc func = confirmationWindowPtr->getSelectedOptionFunc();
    if (func)
    {
        bool selectedYes = false;
        if (currentIndex == CONFIRMATION_WINDOW_OPTION_YES)
        {
            selectedYes = true;
        }

        return func(selectedYes);
    }
}

static void confirmationWindowSelectedNo()
{
    confirmationWindowSelectedOption(CONFIRMATION_WINDOW_OPTION_NO);
}

void ConfirmationWindow::startDrawing(ConfirmationWindowSelectedOptionFunc selectedOptionFunc)
{
    gConfirmationWindow = this;
    this->selectedOptionFunc = selectedOptionFunc;

    this->OptionSelector::startDrawing(confirmationWindowSelectedOption, confirmationWindowSelectedNo);
}
