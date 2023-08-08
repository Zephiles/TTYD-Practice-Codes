#ifndef MENUS_MEMORY_MENU_H
#define MENUS_MEMORY_MENU_H

#include "classes/buttonComboEditor.h"
#include "classes/menu.h"
#include "ttyd/camdrv.h"

#include <cstdint>

enum MemoryMenuMemoryEditorOptions
{
    MEMORY_MENU_MEMORY_EDITOR_OPTION_CLEAR_CACHE = 0,
    MEMORY_MENU_MEMORY_EDITOR_OPTION_SET_SYSTEM_LEVEL,
    MEMORY_MENU_MEMORY_EDITOR_OPTION_DISABLE_PAUSE_MENU,
    MEMORY_MENU_MEMORY_EDITOR_OPTION_ENABLE_VERTICAL_SEPARATORS,
    MEMORY_MENU_MEMORY_EDITOR_OPTION_ENABLE_HORIZONTAL_SEPARATORS,
};

class MemoryMenu
{
   public:
    MemoryMenu() {}
    ~MemoryMenu() {}

    ButtonComboEditor *getButtonComboEditorPtr() { return &this->buttonComboEditor; }

    void drawMemoryEditorMenuInfo() const;

   private:
    ButtonComboEditor buttonComboEditor;
};

extern MemoryMenu *gMemoryMenu;

// memoryInit
void memoryMenuInit(Menu *menuPtr);
void memoryMenuInitExit();

// memoryEditorMenu
void memoryMenuMemoryEditorMenuInit(Menu *menuPtr);
void memoryMenuMemoryEditorMenuControls(Menu *menuPtr, MenuButtonInput button);
void memoryMenuMemoryEditorMenuDraw(CameraId cameraId, void *user);

void memoryMenuMemoryEditorMenuToggleFlag(Menu *menuPtr);
void memoryMenuMemoryEditorMenuChangeButtonCombo(Menu *menuPtr);

// memoryEditorSettings
void memoryMenuMemoryEditorSettingsInit(Menu *menuPtr);
void memoryMenuMemoryEditorSettingsDraw(CameraId cameraId, void *user);

void memoryMenuMemoryEditorSettingsToggleFlag(Menu *menuPtr);

#endif
