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
        this->alpha = 0xFF;
        this->initErrorWindow(battlesErrorMessage);
    }

    Root(const char *battlesErrorMessage, float scale, uint8_t alpha)
    {
        this->rootMenu = gMenu;
        this->scale = scale;
        this->alpha = alpha;
        this->initErrorWindow(battlesErrorMessage);
    }

    ~Root() {}

    void initErrorWindow(const char *battlesErrorMessage)
    {
        ErrorWindow *errorWindowPtr = this->getErrorWindow();
        errorWindowPtr->setScale(this->scale);
        errorWindowPtr->setAlpha(this->alpha);
        errorWindowPtr->setText(battlesErrorMessage);
        errorWindowPtr->placeInWindow(gRootWindow, WindowAlignment::BOTTOM_CENTER);
    }

    Menu *getRootMenu() { return this->rootMenu; }
    ValueEditor *getValueEditor() { return &this->valueEditor; }
    ErrorWindow *getErrorWindow() { return &this->errorWindow; }
    float getScale() const { return this->scale; }
    uint8_t getAlpha() const { return this->alpha; }

   private:
    ValueEditor valueEditor;
    ErrorWindow errorWindow;
    Menu *rootMenu; // Backup of gMenu when the root menu was initially opened
    float scale;
    uint8_t alpha;
};

extern Root *gRoot;

void rootMenuInit();                              // Called when initially entering the root menu
void rootMenuExit();                              // Called when exiting the root menu
void rootMenuDraw(CameraId cameraId, void *user); // Called to handle drawing the root menu

// Called when either trying to enter the battle menu when not in a battle, or while in the Battles menu when a battle has just
// ended
void rootMenuExitBattleMenu();

#endif
