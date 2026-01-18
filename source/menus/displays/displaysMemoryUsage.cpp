#include "menuUtils.h"
#include "drawText.h"
#include "displays.h"
#include "menus/displaysMenu.h"
#include "menus/rootMenu.h"
#include "ttyd/camdrv.h"

#include <cstdint>

static void draw(CameraId cameraId, void *user);
static void selectedOptionToggleFlag(Menu *menuPtr);

static const MenuOption gOptions[] = {
    "Main Heap 0",
    selectedOptionToggleFlag,

    "Main Heap 1",
    selectedOptionToggleFlag,

    "Main Heap 2",
    selectedOptionToggleFlag,

    "Main Heap 3",
    selectedOptionToggleFlag,

    "Main Heap 4",
    selectedOptionToggleFlag,

#ifdef TTYD_JP
    "Main Heap 5",
    selectedOptionToggleFlag,
#endif

    "Smart Heap",
    selectedOptionToggleFlag,

    "Map Heap",
    selectedOptionToggleFlag,

#ifndef TTYD_JP
    "Battle Map Heap",
    selectedOptionToggleFlag,
#endif

    "Adjust Manual Positioning",
    displaysAdjustManualPositionInit,
};

static const MenuFunctions gFuncs = {
    gOptions,
    basicMenuLayoutControls,
    draw,
    nullptr, // Exit function not needed
};

void displaysMenuMemoryUsageInit(Menu *menuPtr)
{
    // Backup the selected display
    gDisplaysMenu->setSelectedDisplay(menuPtr->getCurrentIndex());

    constexpr uint32_t totalOptions = sizeof(gOptions) / sizeof(MenuOption);
    enterNextMenu(&gFuncs, totalOptions);
}

static void drawMemoryUsageInfo()
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

    const float width = getTextWidth(longestText, scale);
    const float posX = tempPosX + width + (20.f * scale);
    float posY = tempPosY;

    // Draw the on/off text for each flag
    const uint8_t *memoryUsageFlagsArrayPtr = memoryUsageFlagsArray;
    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    constexpr uint32_t totalOptions = DISPLAYS_TOTAL_HEAPS;
    const Displays *displaysPtr = gDisplays;
    const char *string;
    uint32_t color;

    for (uint32_t i = 0; i < totalOptions; i++)
    {
        getOnOffTextAndColor(displaysPtr->enabledFlagIsSet(memoryUsageFlagsArrayPtr[i]), &string, &color, 0xFF);
        drawText(string, posX, posY, scale, color);
        posY -= lineDecrement;
    }
}

static void draw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDraw(cameraId, user);

    // Draw the info for the Memory Usage display
    drawMemoryUsageInfo();
}

static void selectedOptionToggleFlag(Menu *menuPtr)
{
    // Make sure the current index does not exceed the max index of `memoryUsageFlagsArray`
    constexpr uint32_t memoryUsageFlagsArrayMaxIndex = sizeof(memoryUsageFlagsArray) - 1;
    const uint32_t currentIndex = menuPtr->getCurrentIndex();

    if (currentIndex > memoryUsageFlagsArrayMaxIndex)
    {
        return;
    }

    const bool ret = displaysMenuToggleEnabledFlag(memoryUsageFlagsArray[currentIndex]);
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
