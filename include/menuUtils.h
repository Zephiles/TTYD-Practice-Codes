#ifndef MENUUTILS_H
#define MENUUTILS_H

#include "classes/menu.h"
#include "gc/pad.h"
#include "ttyd/camdrv.h"

#include <cstdint>

#define OPEN_CLOSE_MENU_BUTTON_COMBO (PadInput::PAD_L | PadInput::PAD_START)

extern Menu *gMenu;

// Calls enterNextMenu with totalOptionsPerPage set to totalOptions, which sets gMenu to the next menu pointer
Menu *enterNextMenu(const MenuFunctions *funcs, uint32_t totalOptions);

// Calls enterNextMenu with totalOptionsPerPage set to totalOptions
Menu *enterNextMenu(const MenuFunctions *funcs, uint32_t totalOptions, Menu *menuPtr);

// Calls enterNextMenu with menuPtr set to gMenu, and sets gMenu to the next menu pointer
Menu *enterNextMenu(const MenuFunctions *funcs, uint32_t totalOptions, uint32_t totalOptionsPerPage);

Menu *enterNextMenu(const MenuFunctions *funcs, uint32_t totalOptions, uint32_t totalOptionsPerPage, Menu *menuPtr);

// Calls enterPrevMenu with menuPtr set to gMenu, and sets gMenu to the previous menu pointer
Menu *enterPrevMenu();

Menu *enterPrevMenu(Menu *menuPtr);

// Closes all menus, calls each of their exit functions, and sets gMenu to nullptr
void closeAllMenus();

// Handles auto-incrementing in menus via D-Pad inputs
bool handleMenuAutoIncrement(MenuAutoIncrement *autoIncrement);

// Converts the standard button inputs into the simplified inputs
MenuButtonInput getMenuButtonInput(bool singleFrame);

// Handles basic controls for the current menu
void basicMenuLayoutControls(Menu *menuPtr, MenuButtonInput button);

// Handles drawing the main window for the menu
void drawMainWindow();

// Draws the main window and options for the current menu
// Calls basicMenuLayoutDraw with lineHeight set to LINE_HEIGHT_FLOAT, and offsetX and offsetY set to 0.f
void basicMenuLayoutDraw(CameraId cameraId, void *user);

// Calls basicMenuLayoutDraw with lineHeight set to MENU_LINE_HEIGHT, and offsetX and offsetY set to 0.f
void basicMenuLayoutDrawMenuLineHeight(CameraId cameraId, void *user);

// Draws the main window and options for the current menu
void basicMenuLayoutDraw(CameraId cameraId, void *user, float lineHeight, float offsetX, float offsetY);

// Handles various menu things each frame
void handleMenu();

// Controls vertical D-Pad movements. Allows for multiple pages and columns. If there is only one page, then currentPagePtr
// should be set to nullptr. totalOptionsPerPage should be a multiple of totalOptionsPerRow, unless totalOptionsPerRow is one.
void menuControlsVertical(MenuButtonInput button,
                          uint8_t *currentIndexPtr,
                          uint8_t *currentPagePtr,
                          uint32_t totalOptions,
                          uint32_t totalOptionsPerPage,
                          uint32_t totalOptionsPerRow,
                          bool allowUpwardsSnap);

// Controls horizontal D-Pad movements. Allows for multiple pages and columns. If there is only one page, then currentPagePtr
// should be set to nullptr. totalOptionsPerPage should be a multiple of totalOptionsPerRow, unless totalOptionsPerRow is one.
void menuControlsHorizontal(MenuButtonInput button,
                            uint8_t *currentIndexPtr,
                            uint8_t *currentPagePtr,
                            uint32_t totalOptions,
                            uint32_t totalOptionsPerPage,
                            uint32_t totalOptionsPerRow,
                            bool allowUpwardsSnap);

#endif
