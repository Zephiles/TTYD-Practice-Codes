#ifndef MENUUTILS_H
#define MENUUTILS_H

#include "classes/menu.h"

#include <cstdint>

extern Menu *gMenu;

// Calls enterNextMenu with totalOptionsPerPage set to totalOptions
void enterNextMenu(const MenuOption *options, const MenuFunctions *funcs, uint32_t totalOptions);

void enterNextMenu(const MenuOption *options, const MenuFunctions *funcs, uint32_t totalOptions, uint32_t totalOptionsPerPage);
void enterPrevMenu();

// Closes all menus, calls each of their exit functions, and sets gMenu to nullptr
void closeAllMenus();

bool handleMenuAutoIncrementDigit(uint16_t *waitFramesToBeginPtr, bool *shouldIncrementNowPtr);

// Converts the standard button inputs into the simplified inputs
MenuButtonInput getMenuButtonInput(bool singleFrame);

// Handles basic controls for the current menu
void controlsBasicMenuLayout(Menu *menuPtr, MenuButtonInput button);

// Draws the main window and options for the current menu
void drawBasicMenuLayout(CameraId cameraId, void *user);

// Handles various menu things each frame
void handleMenu();

// Controls vertical D-Pad movements. Allows for multiple pages and columns. If there is only one page, then currentPagePtr
// should be set to nullptr.
void menuControlsVertical(MenuButtonInput button,
                          uint8_t *currentIndexPtr,
                          uint8_t *currentPagePtr,
                          uint32_t totalOptions,
                          uint32_t totalOptionsPerPage,
                          uint32_t totalOptionsPerRow,
                          bool allowUpwardsSnap);

// Controls horizontal D-Pad movements. Allows for multiple pages and columns. If there is only one page, then currentPagePtr
// should be set to nullptr.
void menuControlsHorizontal(MenuButtonInput button,
                            uint8_t *currentIndexPtr,
                            uint8_t *currentPagePtr,
                            uint32_t totalOptions,
                            uint32_t totalOptionsPerPage,
                            uint32_t totalOptionsPerRow,
                            bool allowUpwardsSnap);

#endif
