#include "classes/confirmationWindow.h"
#include "classes/window.h"
#include "classes/menu.h"

#include <cstdint>

// `Yes`/`No` options to select in the confirmation window.
static const char *gYesNoStrings[] = {
    "Yes",
    "No",
};

void ConfirmationWindow::init(const Window *parentWindow, const char *helpText)
{
    this->init(parentWindow, helpText, 0xFF);
}

void ConfirmationWindow::init(const Window *parentWindow, const char *helpText, uint8_t windowAlpha)
{
    constexpr uint32_t totalOptions = sizeof(gYesNoStrings) / sizeof(const char *);
    this->OptionSelector::init(helpText, gYesNoStrings, totalOptions, 1, parentWindow, windowAlpha, 20.f);

    // Start on no
    this->OptionSelector::setCurrentIndex(CONFIRMATION_WINDOW_OPTION_NO);
}

void ConfirmationWindow::stopDrawing()
{
    this->OptionSelector::setUser(nullptr);
    this->OptionSelector::stopDrawing();
}

/**
 * Callback function for when an option is selected. Calls the `selectedOptionFunc` variable function from the confirmation
 * window.
 *
 * @param currentIndex The index of the option that was selected.
 * @param user Pointer to the current confirmation window.
 *
 * @relatesalso ConfirmationWindow
 */
static void confirmationWindowSelectedOption(uint32_t currentIndex, void *user)
{
    ConfirmationWindow *confirmationWindowPtr = reinterpret_cast<ConfirmationWindow *>(user);

    // Make sure confirmationWindowPtr is set
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

/**
 * Callback function for when `B` is pressed to cancel selecting an option. Calls the `selectedOptionFunc` variable function
 * from the confirmation window (with the `currentIndex` parameter set to `CONFIRMATION_WINDOW_OPTION_NO`).
 *
 * @param user Pointer to the current confirmation window.
 *
 * @relatesalso ConfirmationWindow
 */
static void confirmationWindowSelectedNo(void *user)
{
    confirmationWindowSelectedOption(CONFIRMATION_WINDOW_OPTION_NO, user);
}

void ConfirmationWindow::startDrawing(ConfirmationWindowSelectedOptionFunc selectedOptionFunc)
{
    this->OptionSelector::setUser(this);
    this->selectedOptionFunc = selectedOptionFunc;

    this->OptionSelector::startDrawing(confirmationWindowSelectedOption, confirmationWindowSelectedNo);
}
