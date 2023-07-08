#ifndef MENUUTILS_H
#define MENUUTILS_H

#include "classes/menu.h"
#include "gc/pad.h"
#include "ttyd/camdrv.h"

#include <cstdint>

#define OPEN_CLOSE_MENU_BUTTON_COMBO (PadInput::PAD_L | PadInput::PAD_START)

extern Menu *gMenu;

// Calls enterNextMenu with totalOptionsPerPage set to totalOptions
Menu *enterNextMenu(const MenuFunctions *funcs, uint32_t totalOptions);

Menu *enterNextMenu(const MenuFunctions *funcs, uint32_t totalOptions, uint32_t totalOptionsPerPage);
Menu *enterPrevMenu();

// Closes all menus, calls each of their exit functions, and sets gMenu to nullptr
void closeAllMenus();

// Handles auto-incrementing in menus via D-Pad inputs
bool handleMenuAutoIncrement(MenuAutoIncrement *autoIncrement);

// Converts the standard button inputs into the simplified inputs
MenuButtonInput getMenuButtonInput(bool singleFrame);

// Handles basic controls for the current menu
void controlsBasicMenuLayout(Menu *menuPtr, MenuButtonInput button);

// Handles drawing the main window for the menu, as it can sometimes be hidden via a flag
void drawMainWindow();

// Draws the main window and options for the current menu
// Calls drawBasicMenuLayout with offsetX and offsetY set to 0.f
void drawBasicMenuLayout(CameraId cameraId, void *user);

// Draws the main window and options for the current menu
void drawBasicMenuLayout(CameraId cameraId, void *user, float offsetX, float offsetY);

// Handles various menu things each frame
void handleMenu();

// Controls vertical D-Pad movements. Allows for multiple pages and columns. If there is only one page, then currentPagePtr
// should be set to nullptr. totalOptionsPerPage should be a multiple of totalOptionsPerRow.
void menuControlsVertical(MenuButtonInput button,
                          uint8_t *currentIndexPtr,
                          uint8_t *currentPagePtr,
                          uint32_t totalOptions,
                          uint32_t totalOptionsPerPage,
                          uint32_t totalOptionsPerRow,
                          bool allowUpwardsSnap);

// Controls horizontal D-Pad movements. Allows for multiple pages and columns. If there is only one page, then currentPagePtr
// should be set to nullptr. totalOptionsPerPage should be a multiple of totalOptionsPerRow.
void menuControlsHorizontal(MenuButtonInput button,
                            uint8_t *currentIndexPtr,
                            uint8_t *currentPagePtr,
                            uint32_t totalOptions,
                            uint32_t totalOptionsPerPage,
                            uint32_t totalOptionsPerRow,
                            bool allowUpwardsSnap);

// Non-inlined functions for calling fpclassify on floats/doubles
int32_t classifyFloat(float value);
int32_t classifyDouble(double value);

#endif
