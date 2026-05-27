#include "mod.h"
#include "classes/yoshiColorSelector.h"
#include "classes/window.h"
#include "classes/menu.h"

#include <cstdint>

const char *YoshiColorSelector::gYoshiColorsStrings[TOTAL_YOSHI_COLORS] = {
    "Green",
    "Red",
    "Blue",
    "Orange",
    "Pink",
    "Black",
    "White",
};

void YoshiColorSelector::init(const Window *parentWindow)
{
    this->init(parentWindow, 0xFF);
}

void YoshiColorSelector::init(const Window *parentWindow, uint8_t windowAlpha)
{
    this->OptionSelector::init(gHelpTextAConfirmBCancel,
                               this->gYoshiColorsStrings,
                               TOTAL_YOSHI_COLORS,
                               1,
                               parentWindow,
                               windowAlpha,
                               0.f);
}
