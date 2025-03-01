#ifndef CLASSES_SETBUTTONCOMBO_H
#define CLASSES_SETBUTTONCOMBO_H

#include "classes/window.h"
#include "classes/menu.h"

#include <cstdint>

/**
 * Callback function pointer for when the player has held any arbitrary button(s) for three seconds.
 *
 * @param buttonCombo The button combination that was held.
 */
typedef void (*ButtonComboEditorSetComboFunc)(uint32_t buttonCombo);

// Callback function pointer for when the player presses `B` three times consecutively to cancel setting a button combo.
typedef void (*ButtonComboEditorCancelFunc)();

// Handles setting/modifying an arbitrary button combo. A help window with text is displayed to assist in this process.
class ButtonComboEditor
{
   public:
    // Generic constructor.
    ButtonComboEditor() {}

    // Generic deconstructor.
    ~ButtonComboEditor() {}

    /**
     * Initializes the button combo editor.
     *
     * @param parentWindow The window to place the button combo editor's window in.
     *
     * @note This function calls the `init` function that takes `windowAlpha` as a parameter, in which `0xFF` is passed in for
     * that value.
     */
    void init(const Window *parentWindow);

    /**
     * Initializes the button combo editor.
     *
     * @param parentWindow The window to place the button combo editor's window in.
     * @param windowAlpha The value to set the button combo editor's window alpha to.
     *
     * @overload
     */
    void init(const Window *parentWindow, uint8_t windowAlpha);

    /**
     * Checks to see if the button combo editor should be drawn this frame.
     *
     * @returns `true` if the button combo editor should be drawn this frame, otherwise `false`.
     */
    bool shouldDraw() const { return this->enabled; }

    /**
     * Sets the button combo editor to be drawn.
     *
     * @param setComboFunc Callback function for when the player has held any arbitrary button(s) for three seconds.
     * @param cancelFunc Callback function for when the player presses `B` three times consecutively to cancel setting a button
     * combo.
     */
    void startDrawing(ButtonComboEditorSetComboFunc setComboFunc, ButtonComboEditorCancelFunc cancelFunc)
    {
        this->setComboFunc = setComboFunc;
        this->cancelFunc = cancelFunc;
        this->enabled = true;
    }

    // Sets the button combo editor to not be drawn.
    void stopDrawing() { this->enabled = false; }

    /**
     * Handles the controls for the button combo editor.
     *
     * @param button The button that was pressed this frame.
     */
    void controls(MenuButtonInput button);

    // Draws the button combo editor.
    void draw() const;

   private:
    /**
     * Updates the button(s) that were pressed on the previous frame.
     *
     * @param buttons The button(s) that were pressed.
     */
    void setButtonsPrevFrame(uint32_t buttons) { this->buttonsPrevFrame = static_cast<uint16_t>(buttons); }

    /**
     * Updates the current value of the `bButtonCounter` variable.
     *
     * @param count The value to set the `bButtonCounter` variable to.
     */
    void setBButtonCounter(uint32_t count) { this->bButtonCounter = static_cast<uint8_t>(count); }

    /**
     * Updates the current value of the `timer` variable.
     *
     * @param timer The value to set the `timer` variable to.
     */
    void setTimer(uint32_t timer) { this->timer = static_cast<uint8_t>(timer); }

    /**
     * Checks to see if `B` was pressed three times consecutively or not.
     *
     * @param buttonsHeld The button(s) that are currently held.
     * @param button The button that was pressed this frame.
     *
     * @returns `true` if `B` was pressed three or more times consecutively, otherwise `false`.
     *
     * @note If `B` was pressed this frame and no other buttons are held, then this function will increment `bButtonCounter`.
     * Otherwise, if `B` was not pressed this frame, `bButtonCounter` will be reset to `0`.
     */
    bool checkIfBPressedThreeTimes(uint32_t buttonsHeld, MenuButtonInput button);

    /**
     * Handles decrementing the `timer` variable when button(s) are held, and resetting it when no buttons are held.
     *
     * @param buttonsHeld The button(s) that are currently held.
     *
     * @returns `true` if the `timer` variable reaches `0`, otherwise `false`.
     *
     * @note `L + Start` is reserved for opening/closing the menu, so the `timer` variable will be reset when both of these
     * buttons are held at the same time, regardless of what other button(s) are held.
     */
    bool decrementTimerAndCheckIfZero(uint32_t buttonsHeld);

    // The window to place all of the button combo editor's text in.
    Window window;

    // Callback function for when the player has held any arbitrary button(s) for three seconds.
    ButtonComboEditorSetComboFunc setComboFunc;

    // Callback function for when the player presses `B` three times consecutively to cancel setting a button combo.
    ButtonComboEditorCancelFunc cancelFunc;

    // The button(s) that were held on the previous frame.
    uint16_t buttonsPrevFrame;

    // Keeps track of consecutive `B` presses for canceling setting a button combo.
    uint8_t bButtonCounter;

    // Handles keeping track of the three seconds for setting a button combo.
    uint8_t timer;

    // Whether the button combo editor is enabled/drawn or not.
    bool enabled;
};

/**
 * Converts the current button(s) held to a string.
 *
 * @param[in] buttonsHeld The button(s) that are currently held.
 * @param[out] stringOut The pointer to the buffer to place the string.
 * @param[in] stringSize The total size of `stringOut`, including the null terminator.
 */
void buttonsToString(uint32_t buttonsHeld, char *stringOut, uint32_t stringSize);

#endif
