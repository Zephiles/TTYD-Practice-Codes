#include "mod.h"
#include "classes/memoryWatchTypeSelector.h"
#include "classes/optionSelector.h"
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
                               0.f);
}

void MemoryWatchTypeSelector::stopDrawing()
{
    this->OptionSelector::setClassPtr(nullptr);
    this->OptionSelector::stopDrawing();
}

/**
 * Callback function for when an option is selected. Calls the `setTypeFunc` variable function from the follower selector.
 *
 * @param currentIndex The index of the option that was selected.
 * @param classPtr Pointer to the current memory watch type selector.
 *
 * @relatesalso MemoryWatchTypeSelector
 */
static void memoryWatchTypeSelectorSelectedType(uint32_t currentIndex, void *classPtr)
{
    const MemoryWatchTypeSelector *memoryWatchTypeSelectorPtr = convertToMemoryWatchTypeSelectorPtr(classPtr);

    // Make sure memoryWatchTypeSelectorPtr is set
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

void MemoryWatchTypeSelector::startDrawing(MemoryWatchTypeSelectorSetTypeFunc setTypeFunc,
                                           MemoryWatchTypeSelectorCancelFunc cancelFunc)
{
    this->OptionSelector::setClassPtr(this);
    this->setTypeFunc = setTypeFunc;

    this->OptionSelector::startDrawing(memoryWatchTypeSelectorSelectedType, cancelFunc);
}
