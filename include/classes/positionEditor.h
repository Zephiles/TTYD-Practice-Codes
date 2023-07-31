#ifndef CLASSES_POSITIONEDITOR_H
#define CLASSES_POSITIONEDITOR_H

#include "classes/window.h"
#include "classes/menu.h"

#include <cstdint>

typedef void (*PositionEditorCancelFunc)();

class PositionEditor
{
   public:
    PositionEditor() {}
    ~PositionEditor() {}

    // Sets the alpha to 0xFF
    void init(const Window *parentWindow, float *posXSource, float *posYSource, float scale);

    void init(const Window *parentWindow, float *posXSource, float *posYSource, float scale, uint8_t alpha);

    bool shouldDraw() const { return this->enabled; }
    void stopDrawing() { this->enabled = false; }

    void startDrawing(PositionEditorCancelFunc cancelFunc)
    {
        this->cancelFunc = cancelFunc;
        this->enabled = true;
    }

    float getPosX() const { return this->posX; }
    float getPosY() const { return this->posY; }

    bool checkAutoIncrement();
    void dpadControls(MenuButtonInput button);
    void controls(MenuButtonInput button);

    void draw() const;

   private:
    Window window;
    float scale;

    PositionEditorCancelFunc cancelFunc; // Called when the player presses A to confirm or B to cancel

    float posX;        // Current Pos X value
    float posY;        // Current Pos Y value
    float *posXSource; // Pointer to the Pos X value to be updated upon pressing A
    float *posYSource; // Pointer to the Pos Y value to be updated upon pressing A

    uint16_t waitFramesToBegin; // For auto-incrementing
    bool enabled;               // Whether this window is enabled/drawn or not
    uint8_t alpha;
};

#endif
