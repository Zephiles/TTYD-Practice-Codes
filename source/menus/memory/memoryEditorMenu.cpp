#include "menuUtils.h"
#include "drawText.h"
#include "memoryEditor.h"
#include "mod.h"
#include "classes/window.h"
#include "menus/memoryMenu.h"
#include "menus/rootMenu.h"
#include "ttyd/camdrv.h"

#include <cstdint>

const MenuOption gMemoryMenuMemoryEditorMenuOptions[] {
    "Enable Memory Editor",
    memoryMenuMemoryEditorMenuToggleFlag,

    "Change Button Combo",
    memoryMenuMemoryEditorMenuChangeButtonCombo,

    "Open Settings Menu",
    memoryMenuMemoryEditorSettingsInit,
};

const MenuFunctions gMemoryMenuMemoryEditorMenuFuncs = {
    gMemoryMenuMemoryEditorMenuOptions,
    memoryMenuMemoryEditorMenuControls,
    memoryMenuMemoryEditorMenuDraw,
    nullptr, // Exit function not needed
};

void memoryMenuMemoryEditorMenuInit(Menu *menuPtr)
{
    (void)menuPtr;

    constexpr uint32_t totalOptions = sizeof(gMemoryMenuMemoryEditorMenuOptions) / sizeof(MenuOption);
    enterNextMenu(&gMemoryMenuMemoryEditorMenuFuncs, totalOptions);
}

void memoryMenuMemoryEditorMenuControls(Menu *menuPtr, MenuButtonInput button)
{
    // If the button combo editor is open, then handle the controls for that
    ButtonComboEditor *buttonComboEditorPtr;
    if (buttonComboEditorPtr = gMemoryMenu->getButtonComboEditorPtr(), buttonComboEditorPtr->shouldDraw())
    {
        buttonComboEditorPtr->controls(button);
        return;
    }

    // Use the default controls
    basicMenuLayoutControls(menuPtr, button);
}

void MemoryMenu::drawMemoryEditorMenuInfo() const
{
    // Get the text position for the top-left of the window two lines under the main text
    Menu *menuPtr = gMenu;
    constexpr float scale = MENU_SCALE;
    const uint32_t totalOptions = menuPtr->getTotalOptions();

    float tempPosX;
    float tempPosY;
    gRootWindow->getTextPosXYUnderMainText(nullptr, WindowAlignment::TOP_LEFT, totalOptions, 2, scale, &tempPosX, &tempPosY);

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    const float posX = tempPosX;
    float posY = tempPosY;

    // Draw the text for the memory editor
    drawText("Memory Editor", posX, posY, scale, getColorWhite(0xFF));

    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    posY -= lineDecrement;

    // Draw the on/off text for the memory editor
    const MemoryEditor *memoryEditorPtr = gMemoryEditor;
    const char *onOffText;
    uint32_t color;

    getOnOffTextAndColor(
        memoryEditorPtr->enabledFlagIsSet(MemoryEditorEnabledFlag::MEMORY_EDITOR_ENABLED_FLAG_MEMORY_EDITOR_ENABLED),
        &onOffText,
        &color,
        0xFF);

    drawText(onOffText, posX, posY, scale, color);
    posY -= (lineDecrement * 2.f);

    // Draw the help text for the button combo
    drawText(gHelpTextButtonCombo, posX, posY, scale, getColorWhite(0xFF));
    posY -= lineDecrement;

    // Draw the button combo
    char buf[128];
    buttonsToString(memoryEditorPtr->getButtonCombo(), buf, sizeof(buf));
    drawText(buf, posX, posY, scale, getColorWhite(0xFF));
}

void memoryMenuMemoryEditorMenuDraw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDraw(cameraId, user);

    // Draw the info for the memory editor
    MemoryMenu *memoryMenuPtr = gMemoryMenu;
    memoryMenuPtr->drawMemoryEditorMenuInfo();

    // Draw the button combo editor if applicable
    ButtonComboEditor *buttonComboEditorPtr = memoryMenuPtr->getButtonComboEditorPtr();
    if (buttonComboEditorPtr->shouldDraw())
    {
        buttonComboEditorPtr->draw();
    }
}

void memoryMenuMemoryEditorMenuToggleFlag(Menu *menuPtr)
{
    (void)menuPtr;

    gMemoryEditor->toggleEnabledFlag(MemoryEditorEnabledFlag::MEMORY_EDITOR_ENABLED_FLAG_MEMORY_EDITOR_ENABLED);
}

void memoryMenuMemoryEditorMenuCancelSetNewButtonCombo()
{
    gMemoryMenu->getButtonComboEditorPtr()->stopDrawing();
    gMod->clearFlag(ModFlag::MOD_FLAG_CHANGING_BUTTON_COMBO);
}

void memoryMenuMemoryEditorMenuSetNewButtonCombo(uint32_t buttonCombo)
{
    gMemoryEditor->setButtonCombo(buttonCombo);

    // Close the button combo editor
    memoryMenuMemoryEditorMenuCancelSetNewButtonCombo();
}

void memoryMenuMemoryEditorMenuChangeButtonCombo(Menu *menuPtr)
{
    (void)menuPtr;

    gMod->setFlag(ModFlag::MOD_FLAG_CHANGING_BUTTON_COMBO);

    // Initialize the button combo editor
    ButtonComboEditor *buttonComboEditorPtr = gMemoryMenu->getButtonComboEditorPtr();

    const Window *rootWindowPtr = gRootWindow;
    buttonComboEditorPtr->init(rootWindowPtr, rootWindowPtr->getAlpha());

    buttonComboEditorPtr->startDrawing(memoryMenuMemoryEditorMenuSetNewButtonCombo,
                                       memoryMenuMemoryEditorMenuCancelSetNewButtonCombo);
}
