#ifndef CLASSES_CONFIRMATIONWINDOW_H
#define CLASSES_CONFIRMATIONWINDOW_H

#include "classes/optionSelector.h"
#include "classes/window.h"
#include "classes/menu.h"

#include <cstdint>

#define CONFIRMATION_WINDOW_OPTION_YES 0                                   // Index for the `Yes` option.
#define CONFIRMATION_WINDOW_OPTION_NO (CONFIRMATION_WINDOW_OPTION_YES + 1) // Index for the `No` option.

/**
 * Callback function pointer for when the player selects `Yes`/`No` or presses `B` to cancel.
 *
 * @param selectedYes `true` if `Yes` was selected, otherwise `false`. Also `false` if `B` was pressed to cancel.
 */
typedef void (*ConfirmationWindowSelectedOptionFunc)(bool selectedYes);

// Handles setting up an arbitrary prompt for selecting `Yes`/`No`. A help window with text is displayed to assist in this
// process.
class ConfirmationWindow: private OptionSelector
{
   public:
    // Generic constructor.
    ConfirmationWindow() {}

    // Generic deconstructor.
    ~ConfirmationWindow() {}

    /**
     * Initializes the confirmation window.
     *
     * @param parentWindow The window to place the confirmation window in.
     * @param helpText The help text that will be placed at the `top-center` of the confirmation window.
     *
     * @note This function calls the `init` function that takes `windowAlpha` as a parameter, in which `0xFF` is passed in for
     * that value.
     */
    void init(const Window *parentWindow, const char *helpText);

    /**
     * Initializes the confirmation window.
     *
     * @param parentWindow The window to place the confirmation window in.
     * @param helpText The help text that will be placed at the `top-center` of the confirmation window.
     * @param windowAlpha The value to set the confirmation window's alpha to.
     *
     * @overload
     */
    void init(const Window *parentWindow, const char *helpText, uint8_t windowAlpha);

    /**
     * Checks to see if the confirmation window should be drawn this frame.
     *
     * @returns `true` if the confirmation window should be drawn this frame, otherwise `false`.
     */
    bool shouldDraw() const { return this->OptionSelector::shouldDraw(); }

    /**
     * Sets the confirmation window to be drawn.
     *
     * @param selectedOptionFunc Callback function for when the player selects `Yes`/`No` or presses `B` to cancel.
     */
    void startDrawing(ConfirmationWindowSelectedOptionFunc selectedOptionFunc);

    // Sets the confirmation window to not be drawn.
    void stopDrawing();

    /**
     * Handles the controls for the confirmation window.
     *
     * @param button The button that was pressed this frame.
     */
    void controls(MenuButtonInput button) { this->OptionSelector::controls(button); }

    // Draws the confirmation window.
    void draw() const { this->OptionSelector::draw(); }

    /**
     * Gets a pointer to the confirmation window's `selectedOptionFunc` variable.
     *
     * @returns A pointer to the `selectedOptionFunc` variable.
     */
    ConfirmationWindowSelectedOptionFunc getSelectedOptionFunc() const { return this->selectedOptionFunc; }

   private:
    // Callback function for when the player selects `Yes`/`No` or presses `B` to cancel.
    ConfirmationWindowSelectedOptionFunc selectedOptionFunc;
};

#endif
