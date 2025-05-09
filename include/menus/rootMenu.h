#ifndef MENUS_ROOT_MENU_H
#define MENUS_ROOT_MENU_H

#include "classes/menu.h"
#include "classes/window.h"
#include "classes/errorWindow.h"

extern Window *gRootWindow;

class RootMenu
{
   public:
    RootMenu(Menu *menuPtr, const char *battlesErrorMessage)
    {
        this->rootMenu = menuPtr;
        this->initErrorWindow(battlesErrorMessage);
    }

    ~RootMenu() {}

    void initErrorWindow(const char *battlesErrorMessage)
    {
        const Window *rootWindowPtr = gRootWindow;
        ErrorWindow *errorWindowPtr = &this->errorWindow;

        errorWindowPtr->setAlpha(rootWindowPtr->getAlpha());
        errorWindowPtr->setText(battlesErrorMessage);
        errorWindowPtr->placeInWindow(rootWindowPtr, WindowAlignment::MIDDLE_CENTER);
    }

    ErrorWindow *getErrorWindowPtr() { return &this->errorWindow; }
    const Menu *getRootMenuPtr() const { return this->rootMenu; }

   private:
    ErrorWindow errorWindow;
    Menu *rootMenu; // Backup of gMenu when the root menu was initially opened
};

extern RootMenu *gRootMenu;

void rootMenuInit(); // Called when initially entering the root menu

// Called when either trying to enter the battle menu when not in a battle, or while in the Battles menu when a battle has just
// ended
void rootMenuExitBattlesMenu();

#endif
