#ifndef CLASSES_MEMORYWATCHTYPESELECTOR_H
#define CLASSES_MEMORYWATCHTYPESELECTOR_H

#include "mod.h"
#include "classes/optionSelector.h"
#include "classes/window.h"
#include "classes/menu.h"

#include <cstdint>

#define TOTAL_MEMORY_WATCH_TYPES 12 // Total amount of memory watch types to choose from.

/**
 * Callback function pointer for when the player selects a memory watch type.
 *
 * @param type The memory watch type was selected.
 */
typedef void (*MemoryWatchTypeSelectorSetTypeFunc)(VariableType type);

// Callback function pointer for when the player presses `B` to cancel selecting a memory watch type.
typedef void (*MemoryWatchTypeSelectorCancelFunc)();

// Handles selecting a memory watch type from the `gMemoryWatchTypeStrings` variable. A help window with text is displayed to
// assist in this process.
class MemoryWatchTypeSelector: private OptionSelector
{
   public:
    // Generic constructor.
    MemoryWatchTypeSelector() {}

    // Generic deconstructor.
    ~MemoryWatchTypeSelector() {}

    /**
     * Initializes the memory watch type selector.
     *
     * @param parentWindow The window to place the memory watch type selector's window in.
     *
     * @note This function calls the `init` function that takes `windowAlpha` as a parameter, in which `0xFF` is passed in for
     * that value.
     */
    void init(const Window *parentWindow);

    /**
     * Initializes the memory watch type selector.
     *
     * @param parentWindow The window to place the memory watch type selector's window in.
     * @param windowAlpha The value to set the memory watch type selector's window alpha to.
     *
     * @overload
     */
    void init(const Window *parentWindow, uint8_t windowAlpha);

    /**
     * Checks to see if the memory watch type selector should be drawn this frame.
     *
     * @returns `true` if the memory watch type selector should be drawn this frame, otherwise `false`.
     */
    bool shouldDraw() const { return this->OptionSelector::shouldDraw(); }

    /**
     * Sets the memory watch type selector to be drawn.
     *
     * @param setTypeFunc Callback function for when the player selects a memory watch type.
     * @param cancelFunc Callback function for when the player presses `B` to cancel selecting a memory watch type.
     */
    void startDrawing(MemoryWatchTypeSelectorSetTypeFunc setTypeFunc, MemoryWatchTypeSelectorCancelFunc cancelFunc);

    // Sets the memory watch type selector to not be drawn.
    void stopDrawing();

    /**
     * Updates the current value of the `currentIndex` variable.
     *
     * @param index The value to set the `currentIndex` variable to.
     */
    void setCurrentIndex(uint32_t index) { this->OptionSelector::setCurrentIndex(index); }

    /**
     * Handles the controls for the memory watch type selector.
     *
     * @param button The button that was pressed this frame.
     */
    void controls(MenuButtonInput button) { this->OptionSelector::controls(button); }

    // Draws the memory watch type selector.
    void draw() const { this->OptionSelector::draw(); }

    /**
     * Gets a pointer to the memory watch type selector's `setTypeFunc` variable.
     *
     * @returns A pointer to the `setTypeFunc` variable.
     */
    MemoryWatchTypeSelectorSetTypeFunc getSetTypeFunc() const { return this->setTypeFunc; }

   private:
    // Callback function for when the player selects a memory watch type.
    MemoryWatchTypeSelectorSetTypeFunc setTypeFunc;
};

// Array of available memory watch types to select from.
extern const char *gMemoryWatchTypeStrings[TOTAL_MEMORY_WATCH_TYPES];

#endif
