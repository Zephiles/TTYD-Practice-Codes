#ifndef CLASSES_YOSHICOLORSELECTOR_H
#define CLASSES_YOSHICOLORSELECTOR_H

#include "classes/optionSelector.h"
#include "classes/window.h"
#include "classes/menu.h"

#include <cstdint>

#define TOTAL_YOSHI_COLORS 7 // Total amount of Yoshi colors.

/**
 * Callback function pointer for when the player selects a color.
 *
 * @param selectedColorId Id of the selected color.
 */
typedef void (*YoshiColorSelectorSetColorFunc)(uint32_t selectedColorId);

// Callback function pointer for when the player presses `B` to cancel selecting a color.
typedef void (*YoshiColorSelectorCancelFunc)();

// Handles selecting a color from the `gYoshiColorsStrings` variable. A help window with text is displayed to assist in this
// process.
class YoshiColorSelector: private OptionSelector
{
   public:
    // Generic constructor.
    YoshiColorSelector() {}

    // Generic deconstructor.
    ~YoshiColorSelector() {}

    /**
     * Initializes the Yoshi color selector.
     *
     * @param parentWindow The window to place the Yoshi color selector's window in.
     *
     * @note This function calls the `init` function that takes `windowAlpha` as a parameter, in which `0xFF` is passed in for
     * that value.
     */
    void init(const Window *parentWindow);

    /**
     * Initializes the Yoshi color selector.
     *
     * @param parentWindow The window to place the Yoshi color selector's window in.
     * @param windowAlpha The value to set the Yoshi color selector's window alpha to.
     *
     * @overload
     */
    void init(const Window *parentWindow, uint8_t windowAlpha);

    /**
     * Checks to see if the Yoshi color selector should be drawn this frame.
     *
     * @returns `true` if the Yoshi color selector should be drawn this frame, otherwise `false`.
     */
    bool shouldDraw() const { return this->OptionSelector::shouldDraw(); }

    /**
     * Sets the Yoshi color selector to be drawn.
     *
     * @param setColorFunc Callback function for when the player selects a color.
     * @param cancelFunc Callback function for when the player presses `B` to cancel selecting a color.
     */
    void startDrawing(YoshiColorSelectorSetColorFunc setColorFunc, YoshiColorSelectorCancelFunc cancelFunc)
    {
        // The `classPtr` param is not needed for `changeCharacterFunc` nor `cancelFunc`, so they can just be converted to the
        // necessary type to save a bit of memory.
        this->OptionSelector::startDrawing(convertToSelectOptionFunc(setColorFunc), convertToCancelFunc(cancelFunc));
    }

    // Sets the Yoshi color selector to not be drawn.
    void stopDrawing() { this->OptionSelector::stopDrawing(); }

    /**
     * Updates the current value of the `currentIndex` variable.
     *
     * @param index The value to set the `currentIndex` variable to.
     */
    void setCurrentIndex(uint32_t index) { this->OptionSelector::setCurrentIndex(index); }

    /**
     * Handles the controls for the Yoshi color selector.
     *
     * @param button The button that was pressed this frame.
     */
    void controls(MenuButtonInput button) { this->OptionSelector::controls(button); }

    // Draws the Yoshi color selector.
    void draw() const { this->OptionSelector::draw(); }
};

// Array of available Yoshi colors to select from.
extern const char *gYoshiColorsStrings[TOTAL_YOSHI_COLORS];

#endif
