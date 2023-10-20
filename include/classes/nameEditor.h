#ifndef CLASSES_NAMEEDITOR_H
#define CLASSES_NAMEEDITOR_H

#include "classes/window.h"
#include "classes/menu.h"

#include <cstdint>

#define NAME_EDITOR_CHARS_TOTAL_ROWS 5
#define NAME_EDITOR_CHARS_TOTAL_ROWS_FLOAT 5.f

typedef void (*NameEditorSetNameFunc)(char *newNamePtr, uint32_t newNameSize);
typedef void (*NameEditorCancelFunc)();

class NameEditor
{
   public:
    NameEditor() {}
    ~NameEditor() {}

    // Sets windowAlpha to 0xFF
    void init(const Window *parentWindow, const char *initialText, char *namePtr, uint32_t nameSize, bool applyNullTerminator);

    void init(const Window *parentWindow,
              const char *initialText,
              char *namePtr,
              uint32_t nameSize,
              bool applyNullTerminator,
              uint8_t windowAlpha);

    bool shouldDraw() const { return this->enabled; }
    void stopDrawing() { this->enabled = false; }

    void startDrawing(NameEditorSetNameFunc setNameFunc, NameEditorCancelFunc cancelFunc)
    {
        this->setNameFunc = setNameFunc;
        this->cancelFunc = cancelFunc;
        this->enabled = true;
    }

    void setCurrentStringIndex(uint32_t index) { this->currentStringIndex = static_cast<uint8_t>(index); }
    void setCurrentIndex(uint32_t index) { this->currentIndex = static_cast<uint8_t>(index); }

    void controls(MenuButtonInput button);
    void draw();

   private:
    void dpadControls(MenuButtonInput button);

    Window window;

    NameEditorSetNameFunc setNameFunc; // Called when the player presses Start to set the new name
    NameEditorCancelFunc cancelFunc;   // Called when the player presses B/Z to cancel setting the new name
    MenuAutoIncrement autoIncrement;

    char *namePtr;    // Pointer to where the new string will be stored
    uint8_t nameSize; // Maximum size of the new string

    uint8_t totalOptions;       // Total amount of characters that can be chosen for the name
    uint8_t charsPerRow;        // Amount of characters per row that can be chosen for the name
    uint8_t currentStringIndex; // Index to be used with the buffer for the current string
    char buffer[32];            // Buffer for where the current string is stored

    bool enabled;             // Whether this window is enabled/drawn or not
    bool applyNullTerminator; // Whether the name will be null terminated upon being written
    uint8_t currentIndex;     // Current cursor position
};

#endif
