#ifndef CLASSES_GLITZPITCONDITIONSELECTOR_H
#define CLASSES_GLITZPITCONDITIONSELECTOR_H

#include "classes/optionSelector.h"
#include "classes/window.h"
#include "classes/menu.h"

#include <cstdint>

// Total amount of conditions to choose from when in the Minor League
#define TOTAL_GLITZ_PIT_CONDITIONS_MINOR_LEAGUE 10

// Total amount of conditions to choose from when either in the Major League or at a post-chapter 3 sequence
#define TOTAL_GLITZ_PIT_CONDITIONS_MAJOR_LEAGUE 14

// Index values for each possible condition to select from.
enum GlitzPitConditionSelectorIndex
{
    // The following conditions are used in both the Minor League and the Major League
    GLITZ_PIT_CONDITION_SELECTOR_INDEX_DONT_USE_JUMP = 0,
    GLITZ_PIT_CONDITION_SELECTOR_INDEX_DONT_USE_HAMMER,
    GLITZ_PIT_CONDITION_SELECTOR_INDEX_DONT_USE_SPECIAL_MOVES,
    GLITZ_PIT_CONDITION_SELECTOR_INDEX_USE_SPECIAL_MOVE_ONCE,
    GLITZ_PIT_CONDITION_SELECTOR_INDEX_TAKE_DAMAGE_X_TIMES,
    GLITZ_PIT_CONDITION_SELECTOR_INDEX_DONT_USE_ITEMS,
    GLITZ_PIT_CONDITION_SELECTOR_INDEX_DONT_SWITCH_PARTNERS,
    GLITZ_PIT_CONDITION_SELECTOR_INDEX_APPEAL_X_TIMES,
    GLITZ_PIT_CONDITION_SELECTOR_INDEX_DONT_USE_FP,
    GLITZ_PIT_CONDITION_SELECTOR_INDEX_WIN_IN_5_TURNS_OR_LESS,

    // The following conditions are only available when using the Major League set of conditions
    GLITZ_PIT_CONDITION_SELECTOR_INDEX_DONT_ATTACK_WITH_MARIO,
    GLITZ_PIT_CONDITION_SELECTOR_INDEX_DONT_ATTACK_WITH_PARTNERS,
    GLITZ_PIT_CONDITION_SELECTOR_INDEX_DONT_ATTACK_FOR_FIRST_3_TURNS,
    GLITZ_PIT_CONDITION_SELECTOR_INDEX_WIN_BEFORE_TAKING_20_DAMAGE,
};

/**
 * Callback function pointer for when the player selects a condition.
 *
 * @param condition The condition that was selected.
 */
typedef void (*GlitzPitConditionSelectorSetConditionFunc)(uint32_t condition);

// Callback function pointer for when the player presses `B` to cancel selecting a condition.
typedef void (*GlitzPitConditionSelectorCancelFunc)();

// Handles selecting a condition from the `gGlitzPitConditions` variable. A help window with text is displayed to assist in this
// process.
class GlitzPitConditionSelector: private OptionSelector
{
   public:
    // Generic constructor.
    GlitzPitConditionSelector() {}

    // Generic deconstructor.
    ~GlitzPitConditionSelector() {}

    /**
     * Initializes the Glitz Pit condition selector.
     *
     * @param parentWindow The window to place the Glitz Pit condition selector's window in.
     *
     * @note This function calls the `init` function that takes `windowAlpha` as a parameter, in which `0xFF` is passed in for
     * that value.
     */
    void init(const Window *parentWindow);

    /**
     * Initializes the Glitz Pit condition selector.
     *
     * @param parentWindow The window to place the Glitz Pit condition selector's window in.
     * @param windowAlpha The value to set the Glitz Pit condition selector's window alpha to.
     *
     * @overload
     */
    void init(const Window *parentWindow, uint8_t windowAlpha);

    /**
     * Converts a value from the `GlitzPitCondition` enum to an index for the `gGlitzPitConditions` variable.
     *
     * @returns An index for the `gGlitzPitConditions` variable if the current condition is valid, otherwise `-1`.
     */
    static int32_t glitzPitConditionToIndex();

    /**
     * Converts an index for the `gGlitzPitConditions` variable to a value from the `GlitzPitCondition` enum.
     *
     * @param index The index to convert to a value from the `GlitzPitCondition` enum.
     *
     * @returns A value from `GlitzPitCondition` enum if the index is valid, otherwise `-1`.
     */
    static int32_t indexToGlitzPitCondition(int32_t index);

    /**
     * Gets the text for the current condition, in which the text is retrieved from the `gGlitzPitConditions` variable. If the
     * text is for either `Take damage X times` or `Appeal at least X times`, then modified versions of those texts will be used
     * instead based on what the game is currently using.
     *
     * @param[out] bufferOut Pointer to the array to write the condition text to.
     * @param[in] bufferSize The size of the array to write the condition text to.
     */
    static void getAdjustedConditionText(char *bufferOut, uint32_t bufferSize);

    /**
     * Checks to see if the Glitz Pit condition selector should be drawn this frame.
     *
     * @returns `true` if the Glitz Pit condition selector should be drawn this frame, otherwise `false`.
     */
    bool shouldDraw() const { return this->OptionSelector::shouldDraw(); }

    /**
     * Sets the Glitz Pit condition selector to be drawn.
     *
     * @param setConditionFunc Callback function for when the player selects a condition.
     * @param cancelFunc Callback function for when the player presses `B` to cancel selecting a condition.
     */
    void startDrawing(GlitzPitConditionSelectorSetConditionFunc setConditionFunc,
                      GlitzPitConditionSelectorCancelFunc cancelFunc)
    {
        this->OptionSelector::startDrawing(setConditionFunc, cancelFunc);
    }

    // Sets the Glitz Pit condition selector to not be drawn.
    void stopDrawing() { this->OptionSelector::stopDrawing(); }

    /**
     * Updates the current value of the `currentIndex` variable.
     *
     * @param index The value to set the `currentIndex` variable to.
     */
    void setCurrentIndex(uint32_t index) { this->OptionSelector::setCurrentIndex(index); }

    /**
     * Handles the controls for the Glitz Pit condition selector.
     *
     * @param button The button that was pressed this frame.
     */
    void controls(MenuButtonInput button) { this->OptionSelector::controls(button); }

    // Draws the Glitz Pit condition selector.
    void draw() const { this->OptionSelector::draw(); }

    // Array of available Glitz Pit conditions to select from.
    static const char *gGlitzPitConditions[TOTAL_GLITZ_PIT_CONDITIONS_MAJOR_LEAGUE];
};

#endif
