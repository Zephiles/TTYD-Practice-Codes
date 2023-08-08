#include "menuUtils.h"
#include "drawText.h"
#include "memoryEditor.h"
#include "classes/window.h"
#include "menus/memoryMenu.h"
#include "menus/rootMenu.h"
#include "ttyd/camdrv.h"
#include "ttyd/win_main.h"

#include <cstdint>

// Note that this specific menu will also be used by the Memory Editor, so gMemoryMenu should not be used in this file.

const MenuOption gMemoryMenuMemoryEditorSettingsOptions[] {
    "Clear Cache",
    memoryMenuMemoryEditorSettingsToggleFlag,

    "Set System Level",
    memoryMenuMemoryEditorSettingsToggleFlag,

    "Disable Pause Menu",
    memoryMenuMemoryEditorSettingsToggleFlag,

    "Enable Vertical Separaters",
    memoryMenuMemoryEditorSettingsToggleFlag,

    "Enable Horizontal Separaters",
    memoryMenuMemoryEditorSettingsToggleFlag,
};

const MenuFunctions gMemoryMenuMemoryEditorSettingsFuncs = {
    gMemoryMenuMemoryEditorSettingsOptions,
    basicMenuLayoutControls,
    memoryMenuMemoryEditorSettingsDraw,
    nullptr, // Exit function not needed
};

// menuPtr is assumed to not be used in this function; The memory editor calls this function with menuPtr set to nullptr
void memoryMenuMemoryEditorSettingsInit(Menu *menuPtr)
{
    (void)menuPtr;

    constexpr uint32_t totalOptions = sizeof(gMemoryMenuMemoryEditorSettingsOptions) / sizeof(MenuOption);
    enterNextMenu(&gMemoryMenuMemoryEditorSettingsFuncs, totalOptions);
}

void memoryMenuMemoryEditorSettingsDraw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDraw(cameraId, user);

    // Get the text position for the top-left of the window
    float tempPosX;
    float tempPosY;
    constexpr float scale = MENU_SCALE;
    gRootWindow->getTextPosXY(nullptr, WindowAlignment::TOP_LEFT, scale, &tempPosX, &tempPosY);

    // Position the text a bit to the right of the longest text
    float width;
    getTextWidthHeight("Enable Horizontal Separaters", scale, &width, nullptr);

    const float posX = tempPosX + width + (20.f * scale);
    float posY = tempPosY;

    // Draw the on/off text for each flag
    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    const MemoryEditor *memoryEditorPtr = gMemoryEditor;
    const char *onOffText;
    uint32_t color;

    constexpr uint32_t loopCount = MemoryEditorEnabledFlag::MEMORY_EDITOR_ENABLED_FLAG_ENABLE_HORIZONTAL_LINES -
                                   MemoryEditorEnabledFlag::MEMORY_EDITOR_ENABLED_FLAG_CLEAR_CACHE + 1;

    for (uint32_t i = 0; i < loopCount; i++)
    {
        getOnOffTextAndColor(
            memoryEditorPtr->enabledFlagIsSet(MemoryEditorEnabledFlag::MEMORY_EDITOR_ENABLED_FLAG_CLEAR_CACHE + i),
            &onOffText,
            &color,
            0xFF);

        drawText(onOffText, posX, posY, scale, color);
        posY -= lineDecrement;
    }
}

void memoryMenuMemoryEditorSettingsToggleFlag(Menu *menuPtr)
{
    const uint32_t currentIndex = menuPtr->getCurrentIndex();

    const bool flagSet =
        gMemoryEditor->toggleEnabledFlag(MemoryEditorEnabledFlag::MEMORY_EDITOR_ENABLED_FLAG_CLEAR_CACHE + currentIndex);

    // Certain things must be done if the memory editor is currently open
    if (!memoryEditorIsOpen())
    {
        return;
    }

    switch (currentIndex)
    {
        case MemoryMenuMemoryEditorOptions::MEMORY_MENU_MEMORY_EDITOR_OPTION_SET_SYSTEM_LEVEL:
        {
            if (flagSet)
            {
                // Set the system level
                setSystemLevel(1);
            }
            else
            {
                // Clear the system level
                setSystemLevel(0);
            }
            break;
        }
        case MemoryMenuMemoryEditorOptions::MEMORY_MENU_MEMORY_EDITOR_OPTION_DISABLE_PAUSE_MENU:
        {
            if (flagSet)
            {
                // Disable the pause menu
                winOpenDisable();
            }
            else
            {
                // Enable the pause menu
                winOpenEnable();
            }
            break;
        }
        default:
        {
            break;
        }
    }
}
