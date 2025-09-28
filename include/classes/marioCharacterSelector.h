#ifndef CLASSES_MARIOCHARACTERSELECTOR_H
#define CLASSES_MARIOCHARACTERSELECTOR_H

#include "classes/optionSelector.h"
#include "classes/window.h"
#include "classes/menu.h"

#include <cstdint>

#define TOTAL_MARIO_CHARACTERS 4 // Total amount of characters that Mario can be.

// Ids for each possible character to select from.
enum MarioCharacterSelectorIndex
{
    MARIO_CHARACTER_SELECTOR_INDEX_MARIO = 0, // Mario
    MARIO_CHARACTER_SELECTOR_INDEX_BOWSER,    // Bowser
    MARIO_CHARACTER_SELECTOR_INDEX_PEACH,     // Peach
    MARIO_CHARACTER_SELECTOR_INDEX_X_NAUT,    // X-Naut
};

/**
 * Callback function pointer for when the player selects a character.
 *
 * @param selectedCharacterId Id of the selected character.
 */
typedef void (*MarioCharacterSelectorChangeCharacterFunc)(uint32_t selectedCharacterId);

// Callback function pointer for when the player presses `B` to cancel selecting a character.
typedef void (*MarioCharacterSelectorCancelFunc)();

// Handles selecting a character from the `gMarioCharactersStrings` variable. A help window with text is displayed to assist in
// this process.
class MarioCharacterSelector: private OptionSelector
{
   public:
    // Generic constructor.
    MarioCharacterSelector() {}

    // Generic deconstructor.
    ~MarioCharacterSelector() {}

    /**
     * Initializes the Mario character selector.
     *
     * @param parentWindow The window to place the Mario character selector's window in.
     *
     * @note This function calls the `init` function that takes `windowAlpha` as a parameter, in which `0xFF` is passed in for
     * that value.
     */
    void init(const Window *parentWindow);

    /**
     * Initializes the Mario character selector.
     *
     * @param parentWindow The window to place the Mario character selector's window in.
     * @param windowAlpha The value to set the Mario character selector's window alpha to.
     *
     * @overload
     */
    void init(const Window *parentWindow, uint8_t windowAlpha);

    /**
     * Checks to see if the Mario character selector should be drawn this frame.
     *
     * @returns `true` if the Mario character selector should be drawn this frame, otherwise `false`.
     */
    bool shouldDraw() const { return this->OptionSelector::shouldDraw(); }

    /**
     * Sets the Mario character selector to be drawn.
     *
     * @param changeCharacterFunc Callback function for when the player selects a character.
     * @param cancelFunc Callback function for when the player presses `B` to cancel selecting a character.
     */
    void startDrawing(MarioCharacterSelectorChangeCharacterFunc changeCharacterFunc,
                      MarioCharacterSelectorCancelFunc cancelFunc)
    {
        // The `classPtr` param is not needed for `changeCharacterFunc` nor `cancelFunc`, so they can just be converted to the
        // necessary type to save a bit of memory.
        this->OptionSelector::startDrawing(convertToSelectOptionFunc(changeCharacterFunc), convertToCancelFunc(cancelFunc));
    }

    // Sets the Mario character selector to not be drawn.
    void stopDrawing() { this->OptionSelector::stopDrawing(); }

    /**
     * Updates the current value of the `currentIndex` variable.
     *
     * @param index The value to set the `currentIndex` variable to.
     */
    void setCurrentIndex(uint32_t index) { this->OptionSelector::setCurrentIndex(index); }

    /**
     * Handles the controls for the Mario character selector.
     *
     * @param button The button that was pressed this frame.
     */
    void controls(MenuButtonInput button) { this->OptionSelector::controls(button); }

    // Draws the Mario character selector.
    void draw() const { this->OptionSelector::draw(); }
};

// Array of available characters that Mario can be to select from.
extern const char *gMarioCharactersStrings[TOTAL_MARIO_CHARACTERS];

/**
 * Converts Mario's current character id to a value from the `MarioCharacterSelectorIndex` enum.
 *
 * @returns A value from `MarioCharacterSelectorIndex` enum if the character id is valid, otherwise `-1`.
 */
int32_t marioCharacterIdToIndex();

#endif
