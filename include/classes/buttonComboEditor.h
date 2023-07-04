#ifndef CLASSES_SETBUTTONCOMBO_H
#define CLASSES_SETBUTTONCOMBO_H

#include "classes/window.h"
#include "classes/menu.h"

#include <cstdint>

typedef void (*ButtonComboEditorSetComboFunc)(uint32_t buttonCombo);
typedef void (*ButtonComboEditorCancelFunc)();

class ButtonComboEditor
{
   public:
    ButtonComboEditor() {}
    ~ButtonComboEditor() {}

    // Sets the alpha to 0xFF
    void init(const Window *parentWindow, float scale);

    void init(const Window *parentWindow, float scale, uint8_t alpha);

    bool shouldDraw() const { return this->enabled; }
    void stopDrawing() { this->enabled = false; }

    void startDrawing(ButtonComboEditorSetComboFunc setComboFunc, ButtonComboEditorCancelFunc cancelFunc)
    {
        this->setComboFunc = setComboFunc;
        this->cancelFunc = cancelFunc;
        this->enabled = true;
    }

    void setButtonsPrevFrame(uint32_t buttons) { this->buttonsPrevFrame = static_cast<uint16_t>(buttons); }
    void setBButtonCounter(uint32_t count) { this->bButtonCounter = static_cast<uint8_t>(count); }
    void setTimer(uint32_t timer) { this->timer = static_cast<uint8_t>(timer); }

    bool checkIfBPressedThreeTimes(uint32_t buttonsHeld, MenuButtonInput button);
    bool decrementTimerAndCheckIfZero(uint32_t buttonsHeld);
    void controls(MenuButtonInput button);

    void draw() const;

   private:
    Window window;
    float scale;

    // Called when the player has held any arbitrary buttons for 3 seconds
    ButtonComboEditorSetComboFunc setComboFunc;

    // Called when the player presses B three times consecutively to cancel setting a button combo
    ButtonComboEditorCancelFunc cancelFunc;

    uint16_t buttonsPrevFrame; // The buttons held on the previous frame
    uint8_t bButtonCounter;    // Keeps track of consecutive B presses for canceling setting a button combo
    uint8_t timer;             // For keeping track of the 3 seconds for setting a button combo

    bool enabled;              // Whether this window is enabled/drawn or not
    uint8_t alpha;
};

void buttonsToString(uint32_t buttonsHeld, char *stringOut, uint32_t stringSize);

#endif
