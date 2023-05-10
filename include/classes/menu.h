#ifndef CLASSES_MENU_H
#define CLASSES_MENU_H

#include "ttyd/dispdrv.h"

#include <cstdint>

// Button inputs converted to simple values
enum class MenuButtonInput
{
    BUTTON_NONE = 0,
    DPAD_LEFT,
    DPAD_RIGHT,
    DPAD_DOWN,
    DPAD_UP,
    Z,
    R,
    L,
    A,
    B,
    X,
    Y,
    START,
};

class Menu;
typedef void (*MenuSelectedOptionFunc)(Menu *menuPtr);
typedef void (*MenuExitFunc)();
typedef void (*MenuControlsFunc)(Menu *menuPtr, MenuButtonInput button);

struct MenuOption
{
    // Name of the menu option
    const char *name;

    // Function to be called when the menu option is selected via pressing A. Can be set to nullptr if not needed for
    // whatever reason.
    MenuSelectedOptionFunc selectedFunc;
};

struct MenuFunctions
{
    // Function to be called to handle what button inputs do inside the current menu
    MenuControlsFunc controls;

    // Function to be called to draw the current menu
    DispCallback draw;

    // Function to be called when exiting the current menu. Can be set to nullptr if not needed.
    MenuExitFunc exit;
};

class Menu
{
   public:
    Menu(const MenuOption *options, const MenuFunctions *funcs, uint32_t totalOptions, uint32_t totalOptionsPerPage)
    {
        this->options = options;
        this->funcs = funcs;
        this->totalOptions = static_cast<uint8_t>(totalOptions);
        this->totalOptionsPerPage = static_cast<uint8_t>(totalOptionsPerPage);
    }

    ~Menu() {}

    void setPrevMenu(Menu *prev) { this->prev = prev; }
    Menu *getPrevMenu() const { return this->prev; }

    void runControlsFunc(MenuButtonInput button);
    void runSelectedOptionFunc();
    void runDrawFunc() const;
    void runExitFunc() const;

    bool anyFlagIsSet() const { return this->flags != 0; }
    void clearAllFlags() { this->flags = 0; }

    // Flag functions take values 0-31. The functions return if the flag parameter has a higher value than 31.
    bool flagIsSet(uint32_t flag) const
    {
        // Make sure the flag is valid
        constexpr uint32_t maxFlags = sizeof(this->flags) * 8;
        if (flag >= maxFlags)
        {
            return false;
        }

        return (this->flags >> flag) & 1U;
    }

    void setFlag(uint32_t flag)
    {
        // Make sure the flag is valid
        constexpr uint32_t maxFlags = sizeof(this->flags) * 8;
        if (flag >= maxFlags)
        {
            return;
        }

        this->flags |= (1UL << flag);
    }

    void clearFlag(uint32_t flag)
    {
        // Make sure the flag is valid
        constexpr uint32_t maxFlags = sizeof(this->flags) * 8;
        if (flag >= maxFlags)
        {
            return;
        }

        this->flags &= ~(1UL << flag);
    }

    bool toggleFlag(uint32_t flag)
    {
        // Make sure the flag is valid
        constexpr uint32_t maxFlags = sizeof(this->flags) * 8;
        if (flag >= maxFlags)
        {
            return false;
        }

        uint32_t currentFlags = this->flags ^= (1UL << flag);
        this->flags = currentFlags;
        return (currentFlags >> flag) & 1U;
    }

    uint32_t getTotalOptions() const { return this->totalOptions; }

    uint32_t getCurrentIndex() const { return this->currentIndex; }
    void setCurrentIndex(uint32_t index);

    // Generic functions for handling the current menu
    void basicLayoutDraw(float scale) const; // Calls basicLayoutDraw with offsetX and offsetY set to 0.f
    void basicLayoutDraw(float scale, float offsetX, float offsetY) const;
    void basicLayoutControls(MenuButtonInput button);

   private:
    Menu *prev;                 // Previous menu
    const MenuOption *options;  // All options within the current menu
    const MenuFunctions *funcs; // Functions for handling the current menu
    uint32_t flags;             // Arbitrary flags within the current menu
    uint8_t totalOptions;
    uint8_t totalOptionsPerPage;
    uint8_t currentPage;
    uint8_t currentIndex; // Current cursor position
};

#endif