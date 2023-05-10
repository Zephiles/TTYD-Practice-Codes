#ifndef MENUS_ROOT_H
#define MENUS_ROOT_H

#include "menuUtils.h"
#include "classes/window.h"
#include "classes/errorWindow.h"
#include "ttyd/dispdrv.h"
#include "classes/valueEditor.h"

extern Window *gRootWindow;

class Root
{
   public:
    Root(const char *battlesErrorMessage, float scale)
    {
        this->rootMenu = gMenu;
        this->scale = scale;
        this->initErrorWindow(battlesErrorMessage);
    }

    ~Root() {}

    void initErrorWindow(const char *battlesErrorMessage)
    {
        const Window *rootWindowPtr = gRootWindow;
        ErrorWindow *errorWindowPtr = this->getErrorWindow();

        errorWindowPtr->setScale(this->scale);
        errorWindowPtr->setAlpha(rootWindowPtr->getAlpha());
        errorWindowPtr->setText(battlesErrorMessage);
        errorWindowPtr->placeInWindow(rootWindowPtr, WindowAlignment::MIDDLE_CENTER);
    }

    ValueEditor *getValueEditor() { return &this->valueEditor; }
    ErrorWindow *getErrorWindow() { return &this->errorWindow; }
    float getScale() const { return this->scale; }
    Menu *getRootMenu() { return this->rootMenu; }

   private:
    ValueEditor valueEditor;
    ErrorWindow errorWindow;
    float scale;
    Menu *rootMenu; // Backup of gMenu when the root menu was initially opened
};

extern Root *gRoot;

void rootMenuInit();                              // Called when initially entering the root menu
void rootMenuExit();                              // Called when exiting the root menu
void rootMenuDraw(CameraId cameraId, void *user); // Called to handle drawing the root menu

// Called when either trying to enter the battle menu when not in a battle, or while in the Battles menu when a battle has just
// ended
void rootMenuExitBattleMenu();

#endif
