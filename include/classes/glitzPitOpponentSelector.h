#ifndef CLASSES_GLITZPITOPPONENTSELECTOR_H
#define CLASSES_GLITZPITOPPONENTSELECTOR_H

#include "classes/optionSelector.h"
#include "classes/window.h"
#include "classes/menu.h"

#include <cstdint>

#define TOTAL_GLITZ_PIT_OPPONENTS 22 // Total amount of opponents to choose from (which restrictions based on sequence).

// Index values for each possible opponent to select from.
enum GlitzPitOpponentSelectorIndex
{
    GLITZ_PIT_OPPONENT_SELECTOR_INDEX_RAWK_HAWK = 0,
    GLITZ_PIT_OPPONENT_SELECTOR_INDEX_KOOPINATOR,
    GLITZ_PIT_OPPONENT_SELECTOR_INDEX_CHOMP_COUNTRY,
    GLITZ_PIT_OPPONENT_SELECTOR_INDEX_HAMMA_BAMMA_FLARE,
    GLITZ_PIT_OPPONENT_SELECTOR_INDEX_CRAW_DADDY,
    GLITZ_PIT_OPPONENT_SELECTOR_INDEX_MAGIKOOPA_MASTERS,
    GLITZ_PIT_OPPONENT_SELECTOR_INDEX_FUZZ,
    GLITZ_PIT_OPPONENT_SELECTOR_INDEX_SHELLSHOCKERS,
    GLITZ_PIT_OPPONENT_SELECTOR_INDEX_POKER_FACES,
    GLITZ_PIT_OPPONENT_SELECTOR_INDEX_TINY_SPINIES,
    GLITZ_PIT_OPPONENT_SELECTOR_INDEX_ARMORED_HARRIERS,
    GLITZ_PIT_OPPONENT_SELECTOR_INDEX_HAND_IT_OVERS,
    GLITZ_PIT_OPPONENT_SELECTOR_INDEX_BOB_OMB_SQUAD,
    GLITZ_PIT_OPPONENT_SELECTOR_INDEX_PUNK_ROCKS,
    GLITZ_PIT_OPPONENT_SELECTOR_INDEX_MIND_BOGGLERS,
    GLITZ_PIT_OPPONENT_SELECTOR_INDEX_SPIKE_STORM,
    GLITZ_PIT_OPPONENT_SELECTOR_INDEX_DEAD_BONES,
    GLITZ_PIT_OPPONENT_SELECTOR_INDEX_POKEY_TRIPLETS,
    GLITZ_PIT_OPPONENT_SELECTOR_INDEX_KP_KOOPAS,
    GLITZ_PIT_OPPONENT_SELECTOR_INDEX_GOOMBA_BROS,
    GLITZ_PIT_OPPONENT_SELECTOR_INDEX_WINGS_OF_NIGHT,
    GLITZ_PIT_OPPONENT_SELECTOR_INDEX_DESTRUCTORS,
};

/**
 * Callback function pointer for when the player selects an opponent.
 *
 * @param opponent The opponent that was selected.
 */
typedef void (*GlitzPitOpponentSelectorSetOpponentFunc)(uint32_t opponent);

// Callback function pointer for when the player presses `B` to cancel selecting an opponent.
typedef void (*GlitzPitOpponentSelectorCancelFunc)();

// Handles selecting an opponent from the `gGlitzPitOpponents` variable. A help window with text is displayed to assist in this
// process.
class GlitzPitOpponentSelector: private OptionSelector
{
   public:
    // Generic constructor.
    GlitzPitOpponentSelector() {}

    // Generic deconstructor.
    ~GlitzPitOpponentSelector() {}

    /**
     * Initializes the Glitz Pit opponent selector.
     *
     * @param parentWindow The window to place the Glitz Pit opponent selector's window in.
     *
     * @note This function calls the `init` function that takes `windowAlpha` as a parameter, in which `0xFF` is passed in for
     * that value.
     */
    void init(const Window *parentWindow);

    /**
     * Initializes the Glitz Pit opponent selector.
     *
     * @param parentWindow The window to place the Glitz Pit opponent selector's window in.
     * @param windowAlpha The value to set the Glitz Pit opponent selector's window alpha to.
     *
     * @overload
     */
    void init(const Window *parentWindow, uint8_t windowAlpha);

    /**
     * Gets the total amount of fighters currently in the Glitz Pit.
     *
     * @returns The total amount of fighters currently in the Glitz Pit. If the sequence is past `172`, then The Destructors and
     * Wings of Night are automatically excluded, so the returned value will be `21`. Otherwise, the returned value will be
     * `23`.
     */
    static uint32_t getGlitzPitTotalFighters();

    /**
     * Converts a value from the `GlitzPitFighterId` enum to an index for the `gGlitzPitOpponents` variable.
     *
     * @returns An index for the `gGlitzPitOpponents` variable if the current opponent is valid, otherwise `-1`.
     */
    static int32_t glitzPitOpponentToIndex();

    /**
     * Converts an index for the `gGlitzPitOpponents` variable to a value from the `GlitzPitFighterId` enum.
     *
     * @param index The index to convert to a value from the `GlitzPitFighterId` enum.
     *
     * @returns A value from `GlitzPitFighterId` enum if the index is valid, otherwise `-1`.
     */
    static int32_t indexToGlitzPitOpponent(int32_t index);

    /**
     * Checks to see if the Glitz Pit opponent selector should be drawn this frame.
     *
     * @returns `true` if the Glitz Pit opponent selector should be drawn this frame, otherwise `false`.
     */
    bool shouldDraw() const { return this->OptionSelector::shouldDraw(); }

    /**
     * Sets the Glitz Pit opponent selector to be drawn.
     *
     * @param setOpponentFunc Callback function for when the player selects an opponent.
     * @param cancelFunc Callback function for when the player presses `B` to cancel selecting an opponent.
     */
    void startDrawing(GlitzPitOpponentSelectorSetOpponentFunc setOpponentFunc, GlitzPitOpponentSelectorCancelFunc cancelFunc)
    {
        this->OptionSelector::startDrawing(setOpponentFunc, cancelFunc);
    }

    // Sets the Glitz Pit opponent selector to not be drawn.
    void stopDrawing() { this->OptionSelector::stopDrawing(); }

    /**
     * Updates the current value of the `currentIndex` variable.
     *
     * @param index The value to set the `currentIndex` variable to.
     */
    void setCurrentIndex(uint32_t index) { this->OptionSelector::setCurrentIndex(index); }

    /**
     * Handles the controls for the Glitz Pit opponent selector.
     *
     * @param button The button that was pressed this frame.
     */
    void controls(MenuButtonInput button) { this->OptionSelector::controls(button); }

    // Draws the Glitz Pit opponent selector.
    void draw() const { this->OptionSelector::draw(); }

    // Array of available Glitz Pit opponents to select from.
    static const char *gGlitzPitOpponents[TOTAL_GLITZ_PIT_OPPONENTS];
};

#endif
