#ifndef CLASSES_ERRORWINDOW_H
#define CLASSES_ERRORWINDOW_H

#include "classes/menu.h"
#include "classes/window.h"

#include <cstdint>

// Handles drawing an arbitrary window with text for an arbitrary amount of time, intended to be used for displaying error text.
// The error window is set up to be drawn for a specified period of time, but will stop being drawn once a button is pressed.
// The functionality of it being hidden once a button is pressed can be avoided using the `avoidCheckingForInputs` class
// function.
class ErrorWindow
{
   public:
    /**
     * Constructor for the error window class.
     *
     * @param text Pointer to the text to draw inside the error window.
     * @param windowAlpha The value to set the `windowAlpha` variable to.
     *
     * @warning The text will be drawn over multiple frames, so it should `not` point to a temporary buffer, such as a string on
     * the stack.
     */
    ErrorWindow(const char *text, uint8_t windowAlpha)
    {
        this->windowAlpha = windowAlpha;
        this->setText(text);
    }

    /**
     * Generic constructor for the error window class.
     *
     * @overload
     */
    ErrorWindow() {}

    // Generic deconstructor.
    ~ErrorWindow() {}

    /**
     * Initializes the `text` variable, and then calls `setWidthHeightFromTextAndInit` to set the width and height (and the
     * window color using the `windowAlpha` variable) of the error window.
     *
     * @param text Pointer to the text to draw inside the error window.
     *
     * @warning The text will be drawn over multiple frames, so it should `not` point to a temporary buffer, such as a string on
     * the stack.
     *
     * @warning The value of the `windowAlpha` variable needs to be set before calling this function.
     */
    void setText(const char *text);

    /**
     * Updates the value of the `timer` variable, which is based in milliseconds.
     *
     * @param milliseconds The value to set the `timer` variable to.
     */
    void setTimer(uint32_t milliseconds);

    /**
     * Updates the value of the `windowAlpha` variable.
     *
     * @param windowAlpha The value to set the `windowAlpha` variable to.
     */
    void setAlpha(uint8_t windowAlpha) { this->windowAlpha = windowAlpha; }

    /**
     * Sets the error window to not check for inputs to stop drawing itself this frame.
     *
     * @note The `draw` function sets the error window to check for inputs to stop drawing itself every frame, so this function
     * must be called every frame before `shouldDraw` is called to avoid inputs from being checked.
     */
    void avoidCheckingForInputs() { this->shouldCheckForInputs = false; }

    /**
     * Places the error window inside of another window.
     *
     * @param parent The window to place the error window in.
     * @param alignment How to align the error window inside of the `parent` window.
     */
    void placeInWindow(const Window *parent, WindowAlignment alignment)
    {
        this->window.placeInWindow(parent, alignment, MENU_SCALE);
    }

    /**
     * Checks to see if the error window should be drawn this frame. Also resets the `timer` variable to `0` if both the error
     * window is currently set to check for button inputs this frame to stop drawing itself and if a button was pressed this
     * frame.
     *
     * @returns `true` if the error window should be drawn this frame, otherwise `false`. Also returns `false` if both the error
     * window is currently set to check for button inputs this frame to stop drawing itself and if a button was pressed this
     * frame.
     */
    bool shouldDraw();

    /**
     * Draws the error window.
     *
     * @note This function sets the error window to check for inputs to stop drawing itself every frame, so
     * `avoidCheckingForInputs` needs to be called every frame before `shouldDraw` is called if this functionality is not
     * wanted.
     */
    void draw();

   private:
    // The window to place all of the text in.
    Window window;

    /**
     * Pointer to the text to draw inside the error window.
     *
     * @warning The text will be drawn over multiple frames, so it should `not` point to a temporary buffer, such as a string on
     * the stack.
     */
    const char *text;

    // How long the error window should be displayed for, based in milliseconds.
    uint16_t timer;

    // The alpha that the error window's alpha will be set to once it is initialized, which occurs in the `setText` function.
    uint8_t windowAlpha;

    // Used to avoid checking for button inputs on the frame that the window starts being drawn.
    bool shouldCheckForInputs;
};

#endif
