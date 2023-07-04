#ifndef MENUS_ROOT_MENU_H
#define MENUS_ROOT_MENU_H

#include "menuUtils.h"
#include "classes/window.h"
#include "classes/errorWindow.h"
#include "ttyd/camdrv.h"

extern Window *gRootWindow;

class RootMenu
{
   public:
    RootMenu(const char *battlesErrorMessage, float scale)
    {
        this->rootMenu = gMenu;
        this->scale = scale;
        this->initErrorWindow(battlesErrorMessage);
    }

    ~RootMenu() {}

    void initErrorWindow(const char *battlesErrorMessage)
    {
        const Window *rootWindowPtr = gRootWindow;
        ErrorWindow *errorWindowPtr = &this->errorWindow;

        errorWindowPtr->setScale(this->scale);
        errorWindowPtr->setAlpha(rootWindowPtr->getAlpha());
        errorWindowPtr->setText(battlesErrorMessage);
        errorWindowPtr->placeInWindow(rootWindowPtr, WindowAlignment::MIDDLE_CENTER);
    }

    ErrorWindow *getErrorWindow() { return &this->errorWindow; }
    float getScale() const { return this->scale; }
    const Menu *getRootMenu() const { return this->rootMenu; }

   private:
    ErrorWindow errorWindow;
    float scale;
    Menu *rootMenu; // Backup of gMenu when the root menu was initially opened
};

extern RootMenu *gRootMenu;

void rootMenuInit();                              // Called when initially entering the root menu
void rootMenuExit();                              // Called when exiting the root menu
void rootMenuDraw(CameraId cameraId, void *user); // Called to handle drawing the root menu

// Called when either trying to enter the battle menu when not in a battle, or while in the Battles menu when a battle has just
// ended
void rootMenuExitBattlesMenu();

#endif
