#ifndef MENUS_MEMORY_MENU_H
#define MENUS_MEMORY_MENU_H

#include "memoryWatch.h"
#include "classes/valueEditor.h"
#include "classes/nameEditor.h"
#include "classes/positionEditor.h"
#include "classes/memoryWatchTypeSelector.h"
#include "classes/buttonComboEditor.h"
#include "classes/errorWindow.h"
#include "classes/menu.h"
#include "ttyd/camdrv.h"

#include <cstdint>

#define MAX_MEMORY_WATCHES_PER_PAGE 10
#define MAX_MEMORY_WATCH_OFFSETS_PER_PAGE 12 // Includes the initial address option

enum MemoryMenuMemoryWatchModifyOptions
{
    MEMORY_MENU_MEMORY_WATCH_OPTION_NAME = 0,
    MEMORY_MENU_MEMORY_WATCH_OPTION_ADDRESS,
    MEMORY_MENU_MEMORY_WATCH_OPTION_TYPE,
    MEMORY_MENU_MEMORY_WATCH_OPTION_HEX,
    MEMORY_MENU_MEMORY_WATCH_OPTION_POSITION,
    MEMORY_MENU_MEMORY_WATCH_OPTION_SCALE,
    MEMORY_MENU_MEMORY_WATCH_OPTION_DISPLAY,
};

enum MemoryMenuMemoryEditorOptions
{
    MEMORY_MENU_MEMORY_EDITOR_OPTION_CLEAR_CACHE = 0,
    MEMORY_MENU_MEMORY_EDITOR_OPTION_SET_SYSTEM_LEVEL,
    MEMORY_MENU_MEMORY_EDITOR_OPTION_DISABLE_PAUSE_MENU,
    MEMORY_MENU_MEMORY_EDITOR_OPTION_ENABLE_VERTICAL_SEPARATORS,
    MEMORY_MENU_MEMORY_EDITOR_OPTION_ENABLE_HORIZONTAL_SEPARATORS,
};

enum MemoryMenuMemoryWatchFlag
{
    MEMORY_MENU_MEMORY_WATCH_FLAG_MODIFY = 0,
    MEMORY_MENU_MEMORY_WATCH_FLAG_DUPLICATE,
    MEMORY_MENU_MEMORY_WATCH_FLAG_SWAP_MOVE_INIT,
    MEMORY_MENU_MEMORY_WATCH_FLAG_SWAP_MOVE_SELECTED_WATCH_TO_SWAP,
    MEMORY_MENU_MEMORY_WATCH_FLAG_DELETE,
};

enum MemoryMenuMemoryWatchChangeAddressFlag
{
    MEMORY_MENU_MEMORY_WATCH_CHANGE_ADDRESS_FLAG_CHANGE_VALUES = 0,
};

class MemoryMenu
{
   public:
    MemoryMenu() {}
    ~MemoryMenu() {}

    ValueEditor *getValueEditorPtr() { return &this->valueEditor; }
    NameEditor *getNameEditorPtr() { return &this->nameEditor; }
    PositionEditor *getPositionEditorPtr() { return &this->positionEditor; }
    MemoryWatchTypeSelector *getMemoryWatchTypeSelectorPtr() { return &this->typeSelector; }
    ButtonComboEditor *getButtonComboEditorPtr() { return &this->buttonComboEditor; }
    ErrorWindow *getErrorWindowPtr() { return &this->errorWindow; }

    MenuAutoIncrement *getAutoIncrementPtr() { return &this->autoIncrement; }

    uint32_t getSelectedIndex() const { return this->selectedIndex; }
    void setSelectedIndex(uint32_t index) { this->selectedIndex = static_cast<uint8_t>(index); }

    uint32_t getCurrentIndex() const { return this->currentIndex; }
    void setCurrentIndex(uint32_t index) { this->currentIndex = static_cast<uint8_t>(index); }

    uint32_t getChangeAddressCurrentIndex() const { return this->changeAddressCurrentIndex; }
    void setChangeAddressCurrentIndex(uint32_t index) { this->changeAddressCurrentIndex = static_cast<uint8_t>(index); }

    uint32_t getCurrentPage() const { return this->currentPage; }
    void setCurrentPage(uint32_t page) { this->currentPage = static_cast<uint8_t>(page); }

    uint32_t getChangeAddressCurrentPage() const { return this->changeAddressCurrentPage; }
    void setChangeAddressCurrentPage(uint32_t page) { this->changeAddressCurrentPage = static_cast<uint8_t>(page); }

    bool initSwapMoveWatches(Menu *menuPtr);
    void initErrorWindow(const char *text);
    void memoryWatchFlagSetControls(MenuButtonInput button);
    void memoryWatchChangeAddressFlagSetControls(MenuButtonInput button);

    void drawMemoryWatchMenuInfo() const;
    void drawMemoryWatchModifyInfo() const;
    void drawMemoryWatchChangeAddressInfo() const;
    void drawMemoryEditorMenuInfo() const;

   private:
    ValueEditor valueEditor;              // Memory Watches - Modify menu, Change Address menu
    NameEditor nameEditor;                // Memory Watches - Modify menu
    PositionEditor positionEditor;        // Memory Watches - Modify menu
    MemoryWatchTypeSelector typeSelector; // Memory Watches - Modify menu
    ButtonComboEditor buttonComboEditor;  // Memory Editor
    ErrorWindow errorWindow;              // Memory Watches, Change Address menu

    MenuAutoIncrement autoIncrement; // Memory Watches, Change Address menu

    uint8_t selectedIndex; // Memory Watches, Modify menu

    uint8_t currentIndex; // Memory Watches, excluding Change Address menu
    uint8_t currentPage;  // Memory Watches, excluding Change Address menu

    uint8_t changeAddressCurrentIndex; // Memory Watches - Change Address menu
    uint8_t changeAddressCurrentPage;  // Memory Watches - Change Address menu
};

extern MemoryMenu *gMemoryMenu;

// memoryInit
void memoryMenuInit(Menu *menuPtr);
void memoryMenuInitExit();

MemoryWatchEntry *getSelectedMemoryWatchEntryPtr();

// memoryWatchMenu
void memoryMenuMemoryWatchMenuInit(Menu *menuPtr);
void memoryMenuMemoryWatchMenuControls(Menu *menuPtr, MenuButtonInput button);
void memoryMenuMemoryWatchMenuDraw(CameraId cameraId, void *user);

void memoryMenuMemoryWatchAddWatch(Menu *menuPtr);
void memoryMenuMemoryWatchModifyWatch(Menu *menuPtr);
void memoryMenuMemoryWatchDuplicateWatch(Menu *menuPtr);
void memoryMenuMemoryWatchSwapWatches(Menu *menuPtr);
void memoryMenuMemoryWatchMoveWatch(Menu *menuPtr);
void memoryMenuMemoryWatchDeleteWatch(Menu *menuPtr);

// memoryWatchModify
void memoryMenuMemoryWatchModifyInit(Menu *menuPtr);
void memoryMenuMemoryWatchModifyControls(Menu *menuPtr, MenuButtonInput button);
void memoryMenuMemoryWatchModifyDraw(CameraId cameraId, void *user);

void memoryMenuMemoryWatchModifyToggleFlag(Menu *menuPtr);
void memoryMenuMemoryWatchModifyStartChangingName(Menu *menuPtr);
void memoryMenuMemoryWatchModifyStartChangingType(Menu *menuPtr);
void memoryMenuMemoryWatchModifyStartChangingPosition(Menu *menuPtr);
void memoryMenuMemoryWatchModifyStartChangingScale(Menu *menuPtr);

// memoryWatchChangeAddress
void memoryMenuMemoryWatchChangeAddressInit(Menu *menuPtr);
void memoryMenuMemoryWatchChangeAddressControls(Menu *menuPtr, MenuButtonInput button);
void memoryMenuMemoryWatchChangeAddressDraw(CameraId cameraId, void *user);

void memoryMenuMemoryWatchChangeAddressChangeValues(Menu *menuPtr);
void memoryMenuMemoryWatchChangeAddressAddPointerLevel(Menu *menuPtr);
void memoryMenuMemoryWatchChangeAddressRemovePointerLevel(Menu *menuPtr);

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
