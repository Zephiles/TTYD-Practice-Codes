#ifndef MENUS_SETTINGS_MENU_H
#define MENUS_SETTINGS_MENU_H

#include "menuUtils.h"
#include "gc/card.h"
#include "classes/valueEditor.h"
#include "classes/confirmationWindow.h"
#include "classes/errorWindow.h"
#include "ttyd/camdrv.h"

#include <cstdint>

class SettingsMenu
{
   public:
    SettingsMenu() {}
    ~SettingsMenu() {}

    ValueEditor *getValueEditorPtr() { return &this->valueEditor; }
    ConfirmationWindow *getConfirmationWindowPtr() { return &this->confirmationWindow; }
    ErrorWindow *getErrorWindowPtr() { return &this->errorWindow; }

    uint32_t getRootWindowBackupColor() const { return this->rootWindowBackupColor; }
    void setRootWindowBackupColor(uint32_t color) { this->rootWindowBackupColor = color; }

    int32_t getMemoryCardSlot() const { return static_cast<int32_t>(this->memoryCardSlot); }

    void toggleMemoryCardSlot()
    {
        if (this->memoryCardSlot == CARD_SLOT_A)
        {
            this->memoryCardSlot = CARD_SLOT_B;
        }
        else
        {
            this->memoryCardSlot = CARD_SLOT_A;
        }
    }

    void drawSettingsMenuInfo() const;
    void initErrorWindow(const char *text);

   private:
    ValueEditor valueEditor;
    ConfirmationWindow confirmationWindow;
    ErrorWindow errorWindow;

    uint32_t rootWindowBackupColor;
    uint8_t memoryCardSlot;
};

extern SettingsMenu *gSettingsMenu;

void settingsMenuInit(Menu *menuPtr);
void settingsMenuInitControls(Menu *menuPtr, MenuButtonInput button);
void settingsMenuInitDraw(CameraId cameraId, void *user);
void settingsMenuInitExit();

void settingsMenuToggleMemoryCardSlot(Menu *menuPtr);
void settingsMenuSelectedDeleteSettingsFile(Menu *menuPtr);
void settingsMenuSelectedLoadSettingsFile(Menu *menuPtr);
void settingsMenuSelectedSaveSettingsFile(Menu *menuPtr);
void settingsMenuSelectedChangeWindowColor(Menu *menuPtr);

#endif
