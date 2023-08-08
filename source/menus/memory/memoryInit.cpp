#include "menuUtils.h"
#include "cxx.h"
#include "menus/memoryMenu.h"

#include <cstdint>

MemoryMenu *gMemoryMenu = nullptr;

const MenuOption gMemoryMenuInitOptions[] {
    "Memory Watches",
    nullptr,

    "Memory Editor",
    memoryMenuMemoryEditorMenuInit,
};

const MenuFunctions gMemoryMenuInitFuncs = {
    gMemoryMenuInitOptions,
    basicMenuLayoutControls,
    basicMenuLayoutDrawMenuLineHeight,
    memoryMenuInitExit,
};

void memoryMenuInit(Menu *menuPtr)
{
    (void)menuPtr;

    // Failsafe: Make sure memory isn't already allocated for gMemoryMenu
    MemoryMenu *memoryMenuPtr = gMemoryMenu;
    if (memoryMenuPtr)
    {
        delete memoryMenuPtr;
    }

    memoryMenuPtr = new MemoryMenu;
    gMemoryMenu = memoryMenuPtr;

    constexpr uint32_t totalOptions = sizeof(gMemoryMenuInitOptions) / sizeof(MenuOption);
    enterNextMenu(&gMemoryMenuInitFuncs, totalOptions);
}

void memoryMenuInitExit()
{
    delete gMemoryMenu;
    gMemoryMenu = nullptr;
}
