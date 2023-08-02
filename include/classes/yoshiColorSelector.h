#ifndef CLASSES_YOSHICOLORSELECTOR_H
#define CLASSES_YOSHICOLORSELECTOR_H

#include "classes/window.h"
#include "classes/menu.h"

#include <cstdint>

#define TOTAL_YOSHI_COLORS 7
#define TOTAL_YOSHI_COLORS_FLOAT 7.f

typedef void (*YoshiColorSelectorSetColorFunc)(uint32_t selectedColorId);
typedef void (*YoshiColorSelectorCancelFunc)();

class YoshiColorSelector
{
   public:
    YoshiColorSelector() {}
    ~YoshiColorSelector() {}

    // Sets the alpha to 0xFF
    void init(const Window *parentWindow);

    void init(const Window *parentWindow, uint8_t alpha);

    bool shouldDraw() const { return this->enabled; }
    void stopDrawing() { this->enabled = false; }

    void startDrawing(YoshiColorSelectorSetColorFunc setColorFunc, YoshiColorSelectorCancelFunc cancelFunc)
    {
        this->setColorFunc = setColorFunc;
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

    YoshiColorSelectorSetColorFunc setColorFunc; // Called when the player presses A to select a color
    YoshiColorSelectorCancelFunc cancelFunc;     // Called when the player presses B to cancel selecting a color
    MenuAutoIncrement autoIncrement;

    bool enabled;         // Whether this window is enabled/drawn or not
    uint8_t currentIndex; // Current cursor position
    uint8_t alpha;
};

extern const char *gYoshiColorsStrings[TOTAL_YOSHI_COLORS];

#endif
