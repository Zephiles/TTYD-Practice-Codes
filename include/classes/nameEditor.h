#ifndef CLASSES_NAMEEDITOR_H
#define CLASSES_NAMEEDITOR_H

#include "classes/window.h"
#include "classes/menu.h"

#include <cstdint>

// Total amount of rows containing characters to choose from in the name editor.
#define NAME_EDITOR_CHARS_TOTAL_ROWS 5

// Total amount of rows containing characters to choose from in the name editor, converted to a `float`.
#define NAME_EDITOR_CHARS_TOTAL_ROWS_FLOAT 5.f

/**
 * Callback function pointer for when the player presses `Start` to set the new name.
 *
 * @param newNamePtr Pointer to the new name that was set.
 * @param newNameSize The total size of `newNamePtr`, including the null terminator.
 *
 * @note The new name has already been written before this function pointer is called. The only purpose of this function pointer
 * is to allow being able to do additional things once the name has been set.
 */
typedef void (*NameEditorSetNameFunc)(char *newNamePtr, uint32_t newNameSize);

// Callback function pointer for when player presses `B`/`Z` to cancel setting the new name.
typedef void (*NameEditorCancelFunc)();

// Handles selecting characters to set a new name for an arbitrary string. A help window with text is displayed to assist in
// this process.
class NameEditor
{
   public:
    // Generic constructor.
    NameEditor() {}

    // Generic deconstructor.
    ~NameEditor() {}

    /**
     * Initializes the name editor.
     *
     * @param[in] parentWindow The window to place the name editor's window in.
     * @param[in] initialText The initial text to set the name editor's current text value to. If this is `nullptr` (or if the
     * `Japanese` version of the game is being played and the text is `Japanese` text), then the name editor's current text
     * value will be empty.
     * @param[out] namePtr The pointer to the buffer to place the name.
     * @param[in] nameSize The total size of `namePtr`, including the null terminator.
     * @param[in] applyNullTerminator If `true`, then a null terminator will be applied to the end of the name.
     *
     * @note This function calls the `init` function that takes `windowAlpha` as a parameter, in which `0xFF` is passed in for
     * that value.
     */
    void init(const Window *parentWindow, const char *initialText, char *namePtr, uint32_t nameSize, bool applyNullTerminator);

    /**
     * Initializes the name editor.
     *
     * @param[in] parentWindow The window to place the name editor's window in.
     * @param[in] initialText The initial text to set the name editor's current text value to. If this is `nullptr` (or if the
     * `Japanese` version of the game is being played and the text is `Japanese` text), then the name editor's current text
     * value will be empty.
     * @param[out] namePtr The pointer to the buffer to place the name.
     * @param[in] nameSize The total size of `namePtr`, including the null terminator.
     * @param[in] applyNullTerminator If `true`, then a null terminator will be applied to the end of the name.
     * @param[in] windowAlpha The value to set the name editor's window alpha to.
     *
     * @overload
     */
    void init(const Window *parentWindow,
              const char *initialText,
              char *namePtr,
              uint32_t nameSize,
              bool applyNullTerminator,
              uint8_t windowAlpha);

    /**
     * Checks to see if the name editor should be drawn this frame.
     *
     * @returns `true` if the name editor should be drawn this frame, otherwise `false`.
     */
    bool shouldDraw() const { return this->enabled; }

    /**
     * Sets the name editor to be drawn.
     *
     * @param setNameFunc Callback function for when the player presses `Start` to set the new name.
     * @param cancelFunc Callback function for when player presses `B`/`Z` to cancel setting the new name.
     */
    void startDrawing(NameEditorSetNameFunc setNameFunc, NameEditorCancelFunc cancelFunc)
    {
        this->setNameFunc = setNameFunc;
        this->cancelFunc = cancelFunc;
        this->enabled = true;
    }

    // Sets the name editor to not be drawn.
    void stopDrawing() { this->enabled = false; }

    /**
     * Updates the current value of the `currentStringIndex` variable.
     *
     * @param index The value to set the `currentStringIndex` variable to.
     */
    void setCurrentStringIndex(uint32_t index) { this->currentStringIndex = static_cast<uint8_t>(index); }

    /**
     * Updates the current value of the `currentIndex` variable.
     *
     * @param index The value to set the `currentIndex` variable to.
     */
    void setCurrentIndex(uint32_t index) { this->currentIndex = static_cast<uint8_t>(index); }

    /**
     * Handles the controls for the name editor.
     *
     * @param button The button that was pressed this frame.
     */
    void controls(MenuButtonInput button);

    // Draws the name editor.
    void draw();

   private:
    /**
     * Handles the D-Pad controls for the name editor.
     *
     * @param button The button that was pressed this frame.
     */
    void dpadControls(MenuButtonInput button);

    // The window to place all of the name editor's text in.
    Window window;

    /**
     * Callback function for when the player presses `Start` to set the new name.
     *
     * @note The new name has already been written before this function is called. The only purpose of this function is to allow
     * being able to do additional things once the name has been set.
     */
    NameEditorSetNameFunc setNameFunc;

    // Callback function for when the player presses `B`/`Z` to cancel setting the new name.
    NameEditorCancelFunc cancelFunc;

    // Used to handle automatically moving the cursor when a `D-Pad` direction is held.
    MenuAutoIncrement autoIncrement;

    // Pointer to the buffer to place the new name.
    char *namePtr;

    // The total size of `namePtr`. May or may not include the null terminator, depending on the `applyNullTerminator` parameter
    // in the `init` functions.
    uint8_t nameSize;

    // Total amount of characters that can be chosen for the name, based on the `gNameEditorCharacterOptions` global variable.
    uint8_t totalOptions;

    // Amount of characters per row that can be chosen for the name.
    uint8_t charsPerRow;

    // Index to be used with the buffer for the current string.
    uint8_t currentStringIndex;

    // Buffer for where the current string is stored.
    char buffer[32];

    // Whether the name editor is enabled/drawn or not.
    bool enabled;

    // If `true`, then a null terminator will be applied to the end of the name upon being written.
    bool applyNullTerminator;

    // Current cursor position.
    uint8_t currentIndex;
};

#endif
