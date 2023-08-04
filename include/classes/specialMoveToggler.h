#ifndef CLASSES_SPECIALMOVETOGGLER_H
#define CLASSES_SPECIALMOVETOGGLER_H

#include "classes/window.h"
#include "classes/menu.h"
#include "ttyd/icondrv.h"

#include <cstdint>

#define TOTAL_SPECIAL_MOVES 8
#define TOTAL_SPECIAL_MOVES_FLOAT 8.f

typedef bool (*SpecialMoveTogglerToggleFunc)(uint32_t currentIndex); // Return true to toggle the selected special move
typedef void (*SpecialMoveTogglerCancelFunc)();

class SpecialMoveToggler
{
   public:
    SpecialMoveToggler() {}
    ~SpecialMoveToggler() {}

    // Sets the alpha to 0xFF
    void init(const Window *parentWindow);

    void init(const Window *parentWindow, uint8_t alpha);

    bool shouldDraw() const { return this->enabled; }
    void stopDrawing() { this->enabled = false; }

    void startDrawing(SpecialMoveTogglerToggleFunc toggleFunc, SpecialMoveTogglerCancelFunc cancelFunc)
    {
        this->toggleFunc = toggleFunc;
        this->cancelFunc = cancelFunc;
        this->enabled = true;
    }

    void controlsMoveDownOnce();
    void controlsMoveUpOnce();
    void controls(MenuButtonInput button);

    void draw();

   private:
    Window window;

    SpecialMoveTogglerToggleFunc toggleFunc; // Called when the player presses A to toggle the selected special move
    SpecialMoveTogglerCancelFunc cancelFunc; // Called when the player presses B to toggling special moves
    MenuAutoIncrement autoIncrement;

    bool enabled;         // Whether this window is enabled/drawn or not
    uint8_t currentIndex; // Current cursor position
    uint8_t alpha;
};

extern const IconId gSpecialMoveIcons[TOTAL_SPECIAL_MOVES];

#endif
