#ifndef CLASSES_POSITIONEDITOR_H
#define CLASSES_POSITIONEDITOR_H

#include "classes/window.h"
#include "classes/menu.h"

#include <cstdint>

/**
 * Callback function pointer for when the player presses `A` to confirm setting the new position. Upon confirmation,
 * `posXSource` and `posYSource` will be updated with the new positions.
 *
 * @returns `true` if the new position should be set, otherwise `false`.
 */
typedef bool (*PositionEditorConfirmFunc)();

// Callback function pointer for when the player presses `B` to cancel setting the new position.
typedef void (*PositionEditorCancelFunc)();

// Handles adjusting an arbitrary position for an arbitrary object using `X` and `Y` coordinates. A help window with text is
// displayed to assist in this process.
class PositionEditor
{
   public:
    // Generic constructor.
    PositionEditor() {}

    // Generic deconstructor.
    ~PositionEditor() {}

    /**
     * Initializes the position editor.
     *
     * @param[in] parentWindow The window to place the position editor's window in.
     * @param[out] posXSource The pointer to the `X` coordinate to update upon confirmaing the new position. Also used to get
     * the initial `X` coordinate.
     * @param[out] posYSource The pointer to the `Y` coordinate to update upon confirmaing the new position. Also used to get
     * the initial `Y` coordinate.
     *
     * @note This function calls the `init` function that takes `windowAlpha` as a parameter, in which `0xFF` is passed in for
     * that value.
     */
    void init(const Window *parentWindow, float *posXSource, float *posYSource);

    /**
     * Initializes the position editor.
     *
     * @param[in] parentWindow The window to place the position editor's window in.
     * @param[out] posXSource The pointer to the `X` coordinate to update upon confirmaing the new position. Also used to get
     * the initial `X` coordinate.
     * @param[out] posYSource The pointer to the `Y` coordinate to update upon confirmaing the new position. Also used to get
     * the initial `Y` coordinate.
     * @param[in] windowAlpha The value to set the position editor's window alpha to.
     *
     * @overload
     */
    void init(const Window *parentWindow, float *posXSource, float *posYSource, uint8_t windowAlpha);

    /**
     * Checks to see if the position editor should be drawn this frame.
     *
     * @returns `true` if the position editor should be drawn this frame, otherwise `false`.
     */
    bool shouldDraw() const { return this->enabled; }

    /**
     * Sets the position editor to be drawn.
     *
     * @param selectOptionFunc Callback function for when the player presses `A` to confirm setting the new position. Upon
     * confirmation, `posXSource` and `posYSource` will be updated with the new positions.
     * @param cancelFunc Callback function for when the player presses `B` to cancel setting the new position.
     */
    void startDrawing(PositionEditorConfirmFunc confirmFunc, PositionEditorCancelFunc cancelFunc)
    {
        this->confirmFunc = confirmFunc;
        this->cancelFunc = cancelFunc;
        this->enabled = true;
    }

    // Sets the position editor to not be drawn.
    void stopDrawing() { this->enabled = false; }

    // Gets the current `X` coordinate.
    float getPosX() const { return this->posX; }

    // Gets the current `Y` coordinate.
    float getPosY() const { return this->posY; }

    /**
     * Handles the controls for the position editor.
     *
     * @param button The button that was pressed this frame.
     */
    void controls(MenuButtonInput button);

    // Draws the position editor.
    void draw() const;

   private:
    /**
     * Checks to see if auto-incrementing between options should occur when a `D-Pad` direction is held.
     *
     * @returns `true` if auto-incrementing should occur this frame, otherwise `false`.
     */
    bool shouldAutoIncrement();

    /**
     * Handles the D-Pad controls for the position editor.
     *
     * @param button The button that was pressed this frame.
     */
    void dpadControls(MenuButtonInput button);

    // The window to place all of the position editor's text in.
    Window window;

    // Callback function for when the player presses `A` to confirm setting the new position. Upon confirmation, `posXSource`
    // and `posYSource` will be updated with the new positions.
    PositionEditorConfirmFunc confirmFunc;

    // Callback function for when the player presses `B` to cancel setting the new position.
    PositionEditorCancelFunc cancelFunc;

    // Current `X` coordinate.
    float posX;

    // Current `Y` coordinate.
    float posY;

    // Pointer to the `X` coordinate to update upon confirmaing the new position. Also used to get the initial `X` coordinate.
    float *posXSource;

    // Pointer to the `Y` coordinate to update upon confirmaing the new position. Also used to get the initial `Y` coordinate.
    float *posYSource;

    // Used to figure out long to wait before checking if auto-incrementing should be done.
    uint16_t waitFramesToBegin;

    // Whether the position editor is enabled/drawn or not.
    bool enabled;
};

#endif
