#ifndef CLASSES_YOSHICOLORSELECTOR_H
#define CLASSES_YOSHICOLORSELECTOR_H

#include "classes/optionSelector.h"
#include "classes/window.h"

#include <cstdint>

#define TOTAL_YOSHI_COLORS 7

typedef void (*YoshiColorSelectorSetColorFunc)(uint32_t selectedColorId); // Called when the player presses A to select a color
typedef void (*YoshiColorSelectorCancelFunc)(); // Called when the player presses B to cancel selecting a color

class YoshiColorSelector: private OptionSelector
{
   public:
    YoshiColorSelector() {}
    ~YoshiColorSelector() {}

    // Sets windowAlpha to 0xFF
    void init(const Window *parentWindow);

    void init(const Window *parentWindow, uint8_t windowAlpha);

    bool shouldDraw() const { return this->OptionSelector::shouldDraw(); }
    void stopDrawing() { this->OptionSelector::stopDrawing(); }

    void startDrawing(YoshiColorSelectorSetColorFunc setColorFunc, YoshiColorSelectorCancelFunc cancelFunc)
    {
        this->OptionSelector::startDrawing(setColorFunc, cancelFunc);
    }

    void setCurrentIndex(uint32_t index) { this->OptionSelector::setCurrentIndex(index); }

    void controls(MenuButtonInput button) { this->OptionSelector::controls(button); }
    void draw() const { this->OptionSelector::draw(); }
};

extern const char *gYoshiColorsStrings[TOTAL_YOSHI_COLORS];

#endif
