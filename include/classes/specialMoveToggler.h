#ifndef CLASSES_SPECIALMOVETOGGLER_H
#define CLASSES_SPECIALMOVETOGGLER_H

#include "classes/window.h"
#include "classes/menu.h"
#include "ttyd/icondrv.h"

#include <cstdint>

#define TOTAL_SPECIAL_MOVES 8         // Total amount of special moves.
#define TOTAL_SPECIAL_MOVES_FLOAT 8.f // Total amount of special moves, converted to a float.

/**
 * Callback function pointer for when the player selects a special move to toggle.
 *
 * @param currentIndex The index of the selected special move.
 *
 * @returns `true` if the selected special move should be toggled, otherwise `false`.
 */
typedef bool (*SpecialMoveTogglerToggleFunc)(uint32_t currentIndex);

// Callback function pointer for when the player presses `B` to cancel/stop toggling special moves.
typedef void (*SpecialMoveTogglerCancelFunc)();

// Handles toggling Mario's special moves on/off. A help window with text is displayed to assist in this process.
class SpecialMoveToggler
{
   public:
    // Generic constructor.
    SpecialMoveToggler() {}

    // Generic deconstructor.
    ~SpecialMoveToggler() {}

    /**
     * Initializes the special move toggler.
     *
     * @param parentWindow The window to place the special move toggler's window in.
     *
     * @note This function calls the `init` function that takes `windowAlpha` as a parameter, in which `0xFF` is passed in for
     * that value.
     */
    void init(const Window *parentWindow);

    /**
     * Initializes the special move toggler.
     *
     * @param parentWindow The window to place the special move toggler's window in.
     * @param windowAlpha The value to set the special move toggler's window alpha to.
     *
     * @overload
     */
    void init(const Window *parentWindow, uint8_t windowAlpha);

    /**
     * Checks to see if the special move toggler should be drawn this frame.
     *
     * @returns `true` if the special move toggler should be drawn this frame, otherwise `false`.
     */
    bool shouldDraw() const { return this->enabled; }

    /**
     * Sets the special move toggler to be drawn.
     *
     * @param toggleFunc Callback function for when the player selects a special move to toggle.
     * @param cancelFunc Callback function for when the player presses `B` to cancel/stop toggling special moves.
     */
    void startDrawing(SpecialMoveTogglerToggleFunc toggleFunc, SpecialMoveTogglerCancelFunc cancelFunc)
    {
        this->toggleFunc = toggleFunc;
        this->cancelFunc = cancelFunc;
        this->enabled = true;
    }

    // Sets the special move toggler to not be drawn.
    void stopDrawing() { this->enabled = false; }

    /**
     * Handles the controls for the special move toggler.
     *
     * @param button The button that was pressed this frame.
     */
    void controls(MenuButtonInput button);

    // Draws the special move toggler.
    void draw();

   private:
    // Handles the control for moving `down` once for the special move toggler.
    void controlsMoveDownOnce();

    // Handles the control for moving `up` once for the special move toggler.
    void controlsMoveUpOnce();

    // The window to place all of the special move toggler's text in.
    Window window;

    // Callback function for when the player selects a special move to toggle.
    SpecialMoveTogglerToggleFunc toggleFunc;

    // Callback function for when the player presses `B` to cancel/stop toggling special moves.
    SpecialMoveTogglerCancelFunc cancelFunc;

    // Used to handle automatically moving the cursor when a `D-Pad` direction is held.
    MenuAutoIncrement autoIncrement;

    // Whether the special move toggler is enabled/drawn or not.
    bool enabled;

    // Current cursor position.
    uint8_t currentIndex;
};

// Array of icon ids to be used with the special moves.
extern const IconId gSpecialMoveIcons[TOTAL_SPECIAL_MOVES];

#endif
