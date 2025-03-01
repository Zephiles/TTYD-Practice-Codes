#ifndef CLASSES_OPTION_SELECTOR_H
#define CLASSES_OPTION_SELECTOR_H

#include "classes/window.h"
#include "classes/menu.h"

#include <cstdint>

#define OPTION_SELECTOR_COLUMN_PADDING(scale) (30.f * (scale)) // Padding between each column of options

/**
 * Callback function pointer for when the player selects an option.
 *
 * @param currentIndex The index of the option that was selected.
 */
typedef void (*OptionSelectorSelectOptionFunc)(uint32_t currentIndex);

// Callback function pointer for when the player presses `B` to cancel selecting an option.
typedef void (*OptionSelectorCancelFunc)();

// Handles selecting an arbitrary option from an arbitrary list of options, with an arbitrary amount of columns for the options.
// A help window with text is displayed to assist in this process.
class OptionSelector
{
   public:
    // Generic constructor.
    OptionSelector() {}

    // Generic deconstructor.
    ~OptionSelector() {}

    /**
     * Initializes the option selector.
     *
     * @param helpText The help text that will be placed at the `top-center` of the option selector's window.
     * @param options The options that will be drawn in the option selector's window.
     * @param totalOptions The total amount of options to choose from.
     * @param totalColumns The total amount of columns that will contain the options.
     * @param parentWindow The window to place the option selector's window in.
     * @param spaceBetweenHelpTextAndOptions The amount of vertical space between the help text and the columns. This value will
     * be multiplied by `MENU_SCALE`.
     *
     * @note This function calls the `init` function that takes `windowAlpha` as a parameter, in which `0xFF` is passed in for
     * that value.
     */
    void init(const char *helpText,
              const char **options,
              uint32_t totalOptions,
              uint32_t totalColumns,
              const Window *parentWindow,
              float spaceBetweenHelpTextAndOptions);

    /**
     * Initializes the option selector.
     *
     * @param helpText The help text that will be placed at the `top-center` of the option selector's window.
     * @param options The options that will be drawn in the option selector's window.
     * @param totalOptions The total amount of options to choose from.
     * @param totalColumns The total amount of columns that will contain the options.
     * @param parentWindow The window to place the option selector's window in.
     * @param windowAlpha The value to set the option selector's window alpha to.
     * @param spaceBetweenHelpTextAndOptions The amount of vertical space between the help text and the columns. This value will
     * be multiplied by `MENU_SCALE`.
     *
     * @overload
     */
    void init(const char *helpText,
              const char **options,
              uint32_t totalOptions,
              uint32_t totalColumns,
              const Window *parentWindow,
              uint8_t windowAlpha,
              float spaceBetweenHelpTextAndOptions);

    /**
     * Updates the current value of the `currentIndex` variable.
     *
     * @param index The value to set the `currentIndex` variable to.
     */
    void setCurrentIndex(uint32_t index) { this->currentIndex = static_cast<uint8_t>(index); }

    /**
     * Checks to see if the option selector should be drawn this frame.
     *
     * @returns `true` if the option selector should be drawn this frame, otherwise `false`.
     */
    bool shouldDraw() const { return this->enabled; }

    /**
     * Sets the option selector to be drawn.
     *
     * @param selectOptionFunc Callback function for when the player selects an option.
     * @param cancelFunc Callback function for when the player presses `B` to cancel selecting an option.
     */
    void startDrawing(OptionSelectorSelectOptionFunc selectOptionFunc, OptionSelectorCancelFunc cancelFunc)
    {
        this->selectOptionFunc = selectOptionFunc;
        this->cancelFunc = cancelFunc;
        this->enabled = true;
    }

    // Sets the option selector to not be drawn.
    void stopDrawing() { this->enabled = false; }

    /**
     * Handles the controls for the option selector.
     *
     * @param button The button that was pressed this frame.
     */
    void controls(MenuButtonInput button);

    // Draws the option selector.
    void draw() const;

   private:
    /**
     * Handles the D-Pad controls for the option selector.
     *
     * @param button The button that was pressed this frame.
     */
    void dpadControls(MenuButtonInput button);

    // The window to place all of the option selector's text in.
    Window window;

    // Callback function for when the player selects an option.
    OptionSelectorSelectOptionFunc selectOptionFunc;

    // Callback function for when the player presses `B` to cancel selecting an option.
    OptionSelectorCancelFunc cancelFunc;

    // Used to handle automatically moving the cursor when a `D-Pad` direction is held.
    MenuAutoIncrement autoIncrement;

    // help text that will be placed at the `top-center` of the option selector's window.
    const char *helpText;

    // Options that will be drawn in the option selector's window.
    const char **options;

    // Total width of the options, including padding and the space between each column.
    float totalWidth;

    // Total amount of options to choose from.
    uint8_t totalOptions;

    // Total amount of columns that will contain the options.
    uint8_t totalColumns;

    // Current cursor position.
    uint8_t currentIndex;

    // Whether the option selector is enabled/drawn or not.
    bool enabled;
};

#endif
