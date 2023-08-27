#include "menuUtils.h"
#include "cxx.h"
#include "memoryWatch.h"
#include "menus/memoryMenu.h"
#include "menus/rootMenu.h"

#include <cstdint>

MemoryMenu *gMemoryMenu = nullptr;

const MenuOption gMemoryMenuInitOptions[] {
    "Memory Watches",
    memoryMenuMemoryWatchMenuInit,

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

MemoryWatchEntry *getSelectedMemoryWatchEntryPtr()
{
    // Make sure the pointer for the entries is set
    MemoryWatchEntry *entriesPtr = gMemoryWatch.getEntriesPtr();
    if (!entriesPtr)
    {
        return nullptr;
    }

    return &entriesPtr[gMemoryMenu->getSelectedIndex()];
}

void MemoryMenu::initErrorWindow(const char *text)
{
    ErrorWindow *errorWindowPtr = &this->errorWindow;
    const Window *rootWindowPtr = gRootWindow;

    errorWindowPtr->setAlpha(rootWindowPtr->getAlpha());
    errorWindowPtr->setText(text);
    errorWindowPtr->setTimer(3000);
    errorWindowPtr->placeInWindow(rootWindowPtr, WindowAlignment::MIDDLE_CENTER);
}
