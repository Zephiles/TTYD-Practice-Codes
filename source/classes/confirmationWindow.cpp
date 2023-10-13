#include "classes/confirmationWindow.h"
#include "classes/window.h"
#include "classes/menu.h"

#include <cstdint>

const char *gConfirmationWindowYesNoStrings[] = {
    "Yes",
    "No",
};

ConfirmationWindow *gConfirmationWindow = nullptr;

void ConfirmationWindow::init(const Window *parentWindow, const char *helpText)
{
    this->init(parentWindow, helpText, 0xFF);
}

void ConfirmationWindow::init(const Window *parentWindow, const char *helpText, uint8_t windowAlpha)
{
    constexpr uint32_t totalOptions = sizeof(gConfirmationWindowYesNoStrings) / sizeof(const char *);

    this->OptionSelector::init(helpText,
                               gConfirmationWindowYesNoStrings,
                               totalOptions,
                               1,
                               parentWindow,
                               windowAlpha,
                               20.f,
                               MENU_SCALE);

    // Start on no
    this->OptionSelector::setCurrentIndex(CONFIRMATION_WINDOW_OPTION_NO);
}

void confirmationWindowSelectedOption(uint32_t currentIndex)
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

void confirmationWindowSelectedNo()
{
    confirmationWindowSelectedOption(CONFIRMATION_WINDOW_OPTION_NO);
}
