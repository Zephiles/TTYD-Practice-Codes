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
    memoryMenuMemoryEditorSettingsControls,
    memoryMenuMemoryEditorSettingsDraw,
    nullptr, // Exit function not needed
};

void memoryMenuMemoryEditorSettingsInit(Menu *menuPtr)
{
    constexpr uint32_t totalOptions = sizeof(gMemoryMenuMemoryEditorSettingsOptions) / sizeof(MenuOption);

    if (gMenu)
    {
        // Normal menu is open, so proceed as normal
        enterNextMenu(&gMemoryMenuMemoryEditorSettingsFuncs, totalOptions);
    }
    else
    {
        // Memory Editor is open
        menuPtr = enterNextMenu(&gMemoryMenuMemoryEditorSettingsFuncs, totalOptions, menuPtr);
        gMemoryEditor->setMenuPtr(menuPtr);
    }
}

// enterPrevMenu is called in basicMenuLayoutControls, which uses gMenu. If the Memory Editor is open, then gMenu will not be
// used, so basicMenuLayoutControls cannot be used. So a custom controls function must be used here, in which the B button is
// handled separately.
void memoryMenuMemoryEditorSettingsControls(Menu *menuPtr, MenuButtonInput button)
{
    switch (button)
    {
        case MenuButtonInput::B:
        {
            if (gMenu)
            {
                // Normal menu is open, so proceed as normal
                enterPrevMenu();
            }
            else
            {
                // Memory Editor is open
                // Failsafe: Loop until nullptr is reached
                do
                {
                    menuPtr = enterPrevMenu(menuPtr);
                } while (menuPtr);

                gMemoryEditor->setMenuPtr(nullptr);
            }
            break;
        }
        default:
        {
            // Use the default controls
            basicMenuLayoutControls(menuPtr, button);
            break;
        }
    }
}

void memoryMenuMemoryEditorSettingsDraw(CameraId cameraId, void *user)
{
    // Cannot use basicMenuLayoutDraw since that uses gMenu
    (void)cameraId;
    (void)user;

    // Draw the main window
    drawMainWindow();

    MemoryEditor *memoryEditorPtr = gMemoryEditor;
    const Menu *menuPtr = gMenu;

    if (!menuPtr)
    {
        // Memory Editor is open
        menuPtr = memoryEditorPtr->getMenuPtr();
    }

    // Failsafe: Make sure menuPtr is properly set
    if (!menuPtr)
    {
        return;
    }

    // Draw the main text
    menuPtr->basicLayoutDraw(LINE_HEIGHT_FLOAT, 0.f, 0.f);

    // Get the text position for the top-left of the window
    float tempPosX;
    float tempPosY;
    constexpr float scale = MENU_SCALE;
    gRootWindow->getTextPosXY(nullptr, WindowAlignment::TOP_LEFT, scale, &tempPosX, &tempPosY);

    // Position the text a bit to the right of the longest text
    const float width = getTextWidth("Enable Horizontal Separaters", scale);
    const float posX = tempPosX + width + (20.f * scale);
    float posY = tempPosY;

    // Draw the on/off text for each flag
    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
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
    if (gMenu)
    {
        // Normal menu is open
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
