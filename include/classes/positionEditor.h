#ifndef CLASSES_POSITIONEDITOR_H
#define CLASSES_POSITIONEDITOR_H

#include "classes/window.h"
#include "classes/menu.h"

#include <cstdint>

typedef bool (*PositionEditorConfirmFunc)(); // Return true to update posXSource and posYSource
typedef void (*PositionEditorCancelFunc)();

class PositionEditor
{
   public:
    PositionEditor() {}
    ~PositionEditor() {}

    // Sets windowAlpha to 0xFF
    void init(const Window *parentWindow, float *posXSource, float *posYSource);

    void init(const Window *parentWindow, float *posXSource, float *posYSource, uint8_t windowAlpha);

    bool shouldDraw() const { return this->enabled; }
    void stopDrawing() { this->enabled = false; }

    void startDrawing(PositionEditorConfirmFunc confirmFunc, PositionEditorCancelFunc cancelFunc)
    {
        this->confirmFunc = confirmFunc;
        this->cancelFunc = cancelFunc;
        this->enabled = true;
    }

    float getPosX() const { return this->posX; }
    float getPosY() const { return this->posY; }

    void controls(MenuButtonInput button);
    void draw() const;

   private:
    bool shouldAutoIncrement();
    void dpadControls(MenuButtonInput button);

    Window window;

    PositionEditorConfirmFunc confirmFunc; // Called when the player presses A to confirm
    PositionEditorCancelFunc cancelFunc;   // Called when the player presses B to cancel

    float posX;        // Current Pos X value
    float posY;        // Current Pos Y value
    float *posXSource; // Pointer to the Pos X value to be updated upon pressing A
    float *posYSource; // Pointer to the Pos Y value to be updated upon pressing A

    uint16_t waitFramesToBegin; // For auto-incrementing
    bool enabled;               // Whether this window is enabled/drawn or not
};

#endif
