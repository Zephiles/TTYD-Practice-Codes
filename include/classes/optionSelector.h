#ifndef CLASSES_OPTION_SELECTOR_H
#define CLASSES_OPTION_SELECTOR_H

#include "classes/window.h"
#include "classes/menu.h"

#include <cstdint>

#define OPTION_SELECTOR_COLUMN_PADDING(scale) (30.f * (scale))

typedef void (*OptionSelectorSelectOptionFunc)(uint32_t currentIndex);
typedef void (*OptionSelectorCancelFunc)();

class OptionSelector
{
   public:
    OptionSelector() {}
    ~OptionSelector() {}

    // Sets windowAlpha to 0xFF
    void init(const char *helpText,
              const char **options,
              uint32_t totalOptions,
              uint32_t totalColumns,
              const Window *parentWindow,
              float spaceBetweenHelpTextAndOptions);

    void init(const char *helpText,
              const char **options,
              uint32_t totalOptions,
              uint32_t totalColumns,
              const Window *parentWindow,
              uint8_t windowAlpha,
              float spaceBetweenHelpTextAndOptions);

    void setCurrentIndex(uint32_t index) { this->currentIndex = static_cast<uint8_t>(index); }

    bool shouldDraw() const { return this->enabled; }
    void stopDrawing() { this->enabled = false; }

    void startDrawing(OptionSelectorSelectOptionFunc selectOptionFunc, OptionSelectorCancelFunc cancelFunc)
    {
        this->selectOptionFunc = selectOptionFunc;
        this->cancelFunc = cancelFunc;
        this->enabled = true;
    }

    void controls(MenuButtonInput button);
    void draw() const;

   private:
    void dpadControls(MenuButtonInput button);

    Window window;
    OptionSelectorSelectOptionFunc selectOptionFunc; // Called when the player presses A to select an option
    OptionSelectorCancelFunc cancelFunc;             // Called when the player presses B to cancel selecting an option

    MenuAutoIncrement autoIncrement;
    const char *helpText; // Help text to be drawn at the top-center of the window
    const char **options; // Text options to choose from; will be drawn vertically

    float totalWidth; // Total width of the options, including padding and space between each column
    uint8_t totalOptions;
    uint8_t totalColumns;
    uint8_t currentIndex;

    bool enabled; // Whether this window is enabled/drawn or not
};

#endif
