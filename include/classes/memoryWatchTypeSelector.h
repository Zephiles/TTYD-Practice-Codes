#ifndef CLASSES_MEMORYWATCHTYPESELECTOR_H
#define CLASSES_MEMORYWATCHTYPESELECTOR_H

#include "mod.h"
#include "classes/optionSelector.h"
#include "classes/window.h"

#include <cstdint>

#define TOTAL_MEMORY_WATCH_TYPES 12

typedef void (*MemoryWatchTypeSelectorSetTypeFunc)(VariableType type); // Called when the player presses A to select a type
typedef void (*MemoryWatchTypeSelectorCancelFunc)(); // Called when the player presses B to cancel selecting a type

class MemoryWatchTypeSelector: private OptionSelector
{
   public:
    MemoryWatchTypeSelector() {}
    ~MemoryWatchTypeSelector() {}

    // Sets windowAlpha to 0xFF
    void init(const Window *parentWindow);

    void init(const Window *parentWindow, uint8_t windowAlpha);

    bool shouldDraw() const { return this->OptionSelector::shouldDraw(); }
    void stopDrawing();
    void startDrawing(MemoryWatchTypeSelectorSetTypeFunc setTypeFunc, MemoryWatchTypeSelectorCancelFunc cancelFunc);

    void setCurrentIndex(uint32_t index) { this->OptionSelector::setCurrentIndex(index); }

    void controls(MenuButtonInput button) { this->OptionSelector::controls(button); }
    void draw() const { this->OptionSelector::draw(); }

    MemoryWatchTypeSelectorSetTypeFunc getSetTypeFunc() const { return this->setTypeFunc; }

   private:
    MemoryWatchTypeSelectorSetTypeFunc setTypeFunc;
};

extern const char *gMemoryWatchTypeStrings[TOTAL_MEMORY_WATCH_TYPES];

#endif
