#include "mod.h"
#include "classes/memoryWatchTypeSelector.h"
#include "classes/window.h"
#include "classes/menu.h"

#include <cstdint>

const char *gMemoryWatchTypeStrings[TOTAL_MEMORY_WATCH_TYPES] = {
    "int8_t",
    "int16_t",
    "int32_t",
    "int64_t",
    "uint8_t",
    "uint16_t",
    "uint32_t",
    "uint64_t",
    "float",
    "double",
    "string",
    "time",
};

MemoryWatchTypeSelector *gMemoryWatchTypeSelector = nullptr;

void MemoryWatchTypeSelector::init(const Window *parentWindow)
{
    this->init(parentWindow, 0xFF);
}

void MemoryWatchTypeSelector::init(const Window *parentWindow, uint8_t windowAlpha)
{
    this->OptionSelector::init(gHelpTextAConfirmBCancel,
                               gMemoryWatchTypeStrings,
                               TOTAL_MEMORY_WATCH_TYPES,
                               1,
                               parentWindow,
                               windowAlpha,
                               0.f,
                               MENU_SCALE);
}

void memoryWatchTypeSelectorSelectedType(uint32_t currentIndex)
{
    // Make sure MemoryWatchTypeSelector is set
    const MemoryWatchTypeSelector *memoryWatchTypeSelectorPtr = gMemoryWatchTypeSelector;
    if (!memoryWatchTypeSelectorPtr)
    {
        return;
    }

    // Run the function for when a type is selected
    const MemoryWatchTypeSelectorSetTypeFunc func = memoryWatchTypeSelectorPtr->getSetTypeFunc();
    if (func)
    {
        return func(static_cast<VariableType>(currentIndex));
    }
}
