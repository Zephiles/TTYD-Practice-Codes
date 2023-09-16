#include "menuUtils.h"
#include "drawText.h"
#include "displays.h"
#include "menus/displaysMenu.h"
#include "menus/rootMenu.h"
#include "ttyd/camdrv.h"

#include <cstdint>

const MenuOption gDisplaysMenuMemoryUsageOptions[] = {
    "Adjust Manual Positioning",
    displaysAdjustManualPositionInit,

    "Main Heap 0",
    displaysMenuMemoryUsageToggleFlag,

    "Main Heap 1",
    displaysMenuMemoryUsageToggleFlag,

    "Main Heap 2",
    displaysMenuMemoryUsageToggleFlag,

    "Main Heap 3",
    displaysMenuMemoryUsageToggleFlag,

    "Main Heap 4",
    displaysMenuMemoryUsageToggleFlag,

#ifdef TTYD_JP
    "Main Heap 5",
    displaysMenuMemoryUsageToggleFlag,
#endif

    "Smart Heap",
    displaysMenuMemoryUsageToggleFlag,

    "Map Heap",
    displaysMenuMemoryUsageToggleFlag,

#ifndef TTYD_JP
    "Battle Map Heap",
    displaysMenuMemoryUsageToggleFlag,
#endif
};

const MenuFunctions gDisplaysMenuMemoryUsageFuncs = {
    gDisplaysMenuMemoryUsageOptions,
    basicMenuLayoutControls,
    displaysMenuMemoryUsageDraw,
    nullptr, // Exit function not needed
};

void displaysMenuMemoryUsageInit(Menu *menuPtr)
{
    // Backup the selected display
    gDisplaysMenu->setSelectedDisplay(menuPtr->getCurrentIndex());

    constexpr uint32_t totalOptions = sizeof(gDisplaysMenuMemoryUsageOptions) / sizeof(MenuOption);
    enterNextMenu(&gDisplaysMenuMemoryUsageFuncs, totalOptions);
}

void DisplaysMenu::drawMemoryUsageInfo() const
{
    // Get the text position for the top-left of the window
    float tempPosX;
    float tempPosY;
    constexpr float scale = MENU_SCALE;
    gRootWindow->getTextPosXY(nullptr, WindowAlignment::TOP_LEFT, scale, &tempPosX, &tempPosY);

    // Position the text a bit to the right of the longest text
#ifdef TTYD_JP
    const char *longestText = "Main Heap 0";
#else
    const char *longestText = "Battle Map Heap";
#endif

    float width;
    getTextWidthHeight(longestText, scale, &width, nullptr);

    const float posX = tempPosX + width + (20.f * scale);
    float posY = tempPosY;

    // Draw the on/off text for each flag
    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    const uint32_t totalOptions = DISPLAYS_TOTAL_HEAPS;
    const Displays *displaysPtr = gDisplays;
    const char *string;
    uint32_t color;

    for (uint32_t i = 0; i < totalOptions; i++)
    {
        getOnOffTextAndColor(displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_HEAP_0 + i),
                             &string,
                             &color,
                             0xFF);

        posY -= lineDecrement;
        drawText(string, posX, posY, scale, color);
    }
}

void displaysMenuMemoryUsageDraw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDraw(cameraId, user);

    // Draw the info for the Memory Usage display
    DisplaysMenu *displaysMenuPtr = gDisplaysMenu;
    displaysMenuPtr->drawMemoryUsageInfo();
}

void displaysMenuMemoryUsageToggleFlag(Menu *menuPtr)
{
    const bool ret = displaysMenuToggleEnabledFlag(menuPtr->getCurrentIndex() - 1 +
                                                   DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_HEAP_0);

    if (!ret)
    {
        // If none of the flags are enabled, then free the memory used by the memory usage buffer
        Displays *displaysPtr = gDisplays;
        if (!displaysPtr->anyHeapDisplayIsEnabled())
        {
            displaysPtr->getMemoryUsageDisplayPtr()->freeMemoryUsageBuffer();
        }
    }
}
