#include "drawText.h"
#include "classes/menu.h"
#include "classes/window.h"
#include "menuUtils.h"
#include "menus/root.h"
#include "misc/utils.h"
#include "ttyd/camdrv.h"

#include <cstdint>

void Menu::runControlsFunc(MenuButtonInput button)
{
    const MenuFunctions *menuFuncs = this->funcs;
    if (!menuFuncs)
    {
        return;
    }

    const MenuControlsFunc func = menuFuncs->controls;
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

    const MenuFunctions *menuFuncs = this->funcs;
    if (!menuFuncs)
    {
        return;
    }

    const MenuSelectedOptionFunc func = this->options[index].selectedFunc;
    if (!func)
    {
        return;
    }

    func(this);
}

void Menu::runDrawFunc() const
{
    const MenuFunctions *menuFuncs = this->funcs;
    if (!menuFuncs)
    {
        return;
    }

    const DispCallback func = menuFuncs->draw;
    if (!func)
    {
        return;
    }

    drawOnDebugLayer(func, 0.f);
}

void Menu::runExitFunc() const
{
    const MenuFunctions *menuFuncs = this->funcs;
    if (!menuFuncs)
    {
        return;
    }

    const MenuExitFunc func = menuFuncs->exit;
    if (!func)
    {
        return;
    }

    func();
}

void Menu::setCurrentIndex(uint32_t index)
{
    // Make sure the index does not exceed the maximum number of options
    if (index >= this->totalOptions)
    {
        return;
    }

    this->currentIndex = static_cast<uint8_t>(index);
}

void Menu::basicLayoutDraw(float scale) const
{
    this->basicLayoutDraw(scale, 0.f, 0.f);
}

void Menu::basicLayoutDraw(float scale, float offsetX, float offsetY) const
{
    // Make sure the options are set
    const MenuOption *optionsPtr = this->options;
    if (!options)
    {
        return;
    }

    // Initialize text drawing
    drawTextInit(false);

    const Window *rootWindowPtr = gRootWindow;

    float tempPosX;
    float tempPosY;
    rootWindowPtr->getTextPosXY(nullptr, WindowAlignment::TOP_LEFT, scale, &tempPosX, &tempPosY);

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    float posX = tempPosX + offsetX;
    float posY = tempPosY - offsetY;
    const float posYDecrement = LINE_HEIGHT_FLOAT * scale;
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
        posY -= posYDecrement;
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
