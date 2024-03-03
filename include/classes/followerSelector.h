#ifndef CLASSES_FOLLOWERSELECTOR_h
#define CLASSES_FOLLOWERSELECTOR_h

#include "classes/optionSelector.h"
#include "classes/window.h"
#include "classes/menu.h"
#include "ttyd/party.h"

#include <cstdint>

/**
 * Callback function pointer for when the player selects a follower.
 *
 * @param followerSelected Id of the selected follower.
 */
typedef void (*FollowerSelectorSelectFunc)(PartyMembers followerSelected);

// Callback function pointer for when the player presses `B` to cancel selecting a follower.
typedef void (*FollowerSelectorCancelFunc)();

// Handles selecting a follower from the `gFollowersOptions` variable. A help window with text is displayed to assist in this
// process.
class FollowerSelector: private OptionSelector
{
   public:
    // Generic constructor.
    FollowerSelector() {}

    // Generic deconstructor.
    ~FollowerSelector() {}

    /**
     * Initializes the follower selector.
     *
     * @param parentWindow The window to place the follower selector's window in.
     *
     * @note This function calls the `init` function that takes `windowAlpha` as a parameter, in which `0xFF` is passed in for
     * that value.
     */
    void init(const Window *parentWindow);

    /**
     * Initializes the follower selector.
     *
     * @param parentWindow The window to place the follower selector's window in.
     * @param windowAlpha The value to set the follower selector's window alpha to.
     *
     * @overload
     */
    void init(const Window *parentWindow, uint8_t windowAlpha);

    /**
     * Checks to see if the follower selector should be drawn this frame.
     *
     * @returns `true` if the follower selector should be drawn this frame, otherwise `false`.
     */
    bool shouldDraw() const { return this->OptionSelector::shouldDraw(); }

    /**
     * Sets the follower selector to be drawn.
     *
     * @param selectedFunc Callback function for when the player selects a follower.
     * @param cancelFunc Callback function for when the player presses `B` to cancel selecting a follower.
     */
    void startDrawing(FollowerSelectorSelectFunc selectedFunc, FollowerSelectorCancelFunc cancelFunc);

    // Sets the follower selector to not be drawn.
    void stopDrawing();

    /**
     * Handles the controls for the follower selector.
     *
     * @param button The button that was pressed this frame.
     */
    void controls(MenuButtonInput button) { this->OptionSelector::controls(button); }

    // Draws the follower selector.
    void draw() const { this->OptionSelector::draw(); }

    /**
     * Gets a pointer to the follower selector's `selectFunc` variable.
     *
     * @returns A pointer to the `selectFunc` variable.
     */
    FollowerSelectorSelectFunc getSelectFunc() const { return this->selectFunc; }

   private:
    // Callback function for when the player selects a follower.
    FollowerSelectorSelectFunc selectFunc;
};

// Array of available followers to select from.
extern const char *gFollowersOptions[TOTAL_FOLLOWERS];

// Array of ids of the available followers to select from. Intended to be used alongside `gFollowersOptions`.
extern const PartyMembers gFollowersOptionsId[TOTAL_FOLLOWERS];

#endif