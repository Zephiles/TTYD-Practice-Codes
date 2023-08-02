#include "drawText.h"
#include "mod.h"
#include "classes/menu.h"
#include "classes/window.h"
#include "menuUtils.h"
#include "menus/rootMenu.h"
#include "misc/utils.h"
#include "ttyd/camdrv.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

void Menu::runControlsFunc(MenuButtonInput button)
{
    const MenuFunctions *menuFuncsPtr = this->funcs;
    if (!menuFuncsPtr)
    {
        return;
    }

    const MenuControlsFunc func = menuFuncsPtr->controls;
    if (!func)
    {
        return;
    }

    func(this, button);
}

void Menu::runSelectedOptionFunc()
{
    // Make sure the current index is valid
    const uint32_t index = this->currentIndex;
    if (index >= this->totalOptions)
    {
        // Failsafe: Reset the current index to the first option on the current page
        this->currentIndex = this->totalOptionsPerPage * this->currentPage;
        return;
    }

    const MenuFunctions *menuFuncsPtr = this->funcs;
    if (!menuFuncsPtr)
    {
        return;
    }

    const MenuOption *optionsPtr = menuFuncsPtr->options;
    if (!optionsPtr)
    {
        return;
    }

    const MenuSelectedOptionFunc func = optionsPtr[index].selectedFunc;
    if (!func)
    {
        return;
    }

    func(this);
}

void Menu::runDrawFunc() const
{
    const MenuFunctions *menuFuncsPtr = this->funcs;
    if (!menuFuncsPtr)
    {
        return;
    }

    const DispCallback func = menuFuncsPtr->draw;
    if (!func)
    {
        return;
    }

    drawOnDebugLayer(func, DRAW_ORDER_MENU);
}

void Menu::runExitFunc() const
{
    const MenuFunctions *menuFuncsPtr = this->funcs;
    if (!menuFuncsPtr)
    {
        return;
    }

    const MenuExitFunc func = menuFuncsPtr->exit;
    if (!func)
    {
        return;
    }

    func();
}

void Menu::setCurrentPage(uint32_t page)
{
    // Make sure there are options on the desired page
    const uint32_t totalOptionsPerPage = this->totalOptionsPerPage;
    if ((page * totalOptionsPerPage) >= this->totalOptions)
    {
        return;
    }

    this->currentPage = static_cast<uint8_t>(page);

    // Set the current index to the top of the current page
    this->currentIndex = static_cast<uint8_t>(page * totalOptionsPerPage);
}

void Menu::setCurrentIndex(uint32_t index)
{
    // Make sure the desired index does not exceed the maximum number of options
    if (index >= this->totalOptions)
    {
        return;
    }

    this->currentIndex = static_cast<uint8_t>(index);

    // Set the current page to where the desired index is
    this->currentPage = static_cast<uint8_t>(index / this->totalOptionsPerPage);
}

void Menu::basicLayoutDraw(float offsetX, float offsetY) const
{
    // Make sure the options are set
    const MenuFunctions *menuFuncsPtr = this->funcs;
    if (!menuFuncsPtr)
    {
        return;
    }

    const MenuOption *optionsPtr = menuFuncsPtr->options;
    if (!optionsPtr)
    {
        return;
    }

    // Initialize text drawing
    drawTextInit(false);

    // Draw the page number at the top-right of the main window if there is more than one page
    const Window *rootWindowPtr = gRootWindow;
    constexpr float scale = MENU_SCALE;
    float tempPosX;
    float tempPosY;

    if (this->totalOptions > this->totalOptionsPerPage)
    {
        char buf[16];

        // Draw the page as an int, to prevent long text if it somehow becomes negative
        snprintf(buf, sizeof(buf), "Page %" PRId32, this->currentPage + 1);

        rootWindowPtr->getTextPosXY(buf, WindowAlignment::TOP_RIGHT, scale, &tempPosX, &tempPosY);
        drawText(buf, tempPosX, tempPosY, scale, getColorWhite(0xFF));
    }

    // Draw the main text
    rootWindowPtr->getTextPosXY(nullptr, WindowAlignment::TOP_LEFT, scale, &tempPosX, &tempPosY);

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    float posX = tempPosX + offsetX;
    float posY = tempPosY - offsetY;
    constexpr float lineDecrement = MENU_LINE_HEIGHT * scale;
    uint32_t color;

    const uint32_t totalOptions = this->totalOptions;
    const uint32_t totalOptionsPerPage = this->totalOptionsPerPage;
    const uint32_t currentIndex = this->currentIndex;
    const uint32_t indexStart = this->currentPage * totalOptionsPerPage;

    for (uint32_t i = indexStart; i < (indexStart + totalOptionsPerPage); i++)
    {
        if (i >= totalOptions)
        {
            // Reached the end of the array, so exit
            return;
        }

        color = getCurrentOptionColor(i == currentIndex, 0xFF);
        const char *currentLine = optionsPtr[i].name;

        drawText(currentLine, posX, posY, scale, color);
        posY -= lineDecrement;
    }
}

void Menu::basicLayoutControls(MenuButtonInput button)
{
    const uint32_t totalOptions = this->totalOptions;
    const uint32_t totalOptionsPerPage = this->totalOptionsPerPage;
    uint8_t *currentPagePtr = nullptr;

    // Check if there are multiple pages
    if (totalOptions > totalOptionsPerPage)
    {
        currentPagePtr = &this->currentPage;
    }

    menuControlsVertical(button, &this->currentIndex, currentPagePtr, totalOptions, totalOptionsPerPage, 1, false);
}
