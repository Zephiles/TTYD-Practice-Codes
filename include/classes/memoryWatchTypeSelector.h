#ifndef CLASSES_MEMORYWATCHTYPESELECTOR_H
#define CLASSES_MEMORYWATCHTYPESELECTOR_H

#include "mod.h"
#include "classes/window.h"
#include "classes/menu.h"

#include <cstdint>

#define TOTAL_MEMORY_WATCH_TYPES 12
#define TOTAL_MEMORY_WATCH_TYPES_FLOAT 12.f

typedef void (*MemoryWatchTypeSelectorSetTypeFunc)(VariableType type);
typedef void (*MemoryWatchTypeSelectorCancelFunc)();

class MemoryWatchTypeSelector
{
   public:
    MemoryWatchTypeSelector() {}
    ~MemoryWatchTypeSelector() {}

    // Sets windowAlpha to 0xFF
    void init(const Window *parentWindow);

    void init(const Window *parentWindow, uint8_t windowAlpha);

    bool shouldDraw() const { return this->enabled; }
    void stopDrawing() { this->enabled = false; }

    void startDrawing(MemoryWatchTypeSelectorSetTypeFunc setTypeFunc, MemoryWatchTypeSelectorCancelFunc cancelFunc)
    {
        this->setTypeFunc = setTypeFunc;
        this->cancelFunc = cancelFunc;
        this->enabled = true;
    }

    void setCurrentIndex(uint32_t index) { this->currentIndex = static_cast<uint8_t>(index); }

    void controlsMoveDownOnce();
    void controlsMoveUpOnce();
    void controls(MenuButtonInput button);

    void draw();

   private:
    Window window;

    MemoryWatchTypeSelectorSetTypeFunc setTypeFunc; // Called when the player presses A to select a type
    MemoryWatchTypeSelectorCancelFunc cancelFunc;   // Called when the player presses B to cancel selecting a type
    MenuAutoIncrement autoIncrement;

    bool enabled;         // Whether this window is enabled/drawn or not
    uint8_t currentIndex; // Current cursor position
};

extern const char *gMemoryWatchTypeStrings[TOTAL_MEMORY_WATCH_TYPES];

#endif
