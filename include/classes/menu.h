#ifndef CLASSES_MENU_H
#define CLASSES_MENU_H

#include "mod.h"
#include "ttyd/dispdrv.h"

#include <cstdint>

// Custom line height to use in the menus, which assures that no additional empty space will be at the bottom of the window if
// the options reach it.
#define MENU_LINE_HEIGHT (20.f / 0.6f)

// Default scale to use in the menus.
#define MENU_SCALE 0.6f

// Button inputs converted to simple values.
enum class MenuButtonInput : uint8_t
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

// Total amount of valid button inputs for the menu.
#define TOTAL_MENU_INPUT_BUTTONS (static_cast<uint32_t>(MenuButtonInput::START) + 1)

// Used to handle automatically moving the cursor when a `D-Pad` direction is held.
struct MenuAutoIncrement
{
    uint16_t waitFramesToBegin;     // Used to figure out long to wait before checking if auto-incrementing should be done.
    uint16_t framesBeforeIncrement; // Used to limit how fast auto-incrementing should occur.
} __attribute__((__packed__));

static_assert(sizeof(MenuAutoIncrement) == 0x4);

class Menu;
/**
 * Callback function pointer for when a menu option is selected.
 *
 * @param menuPtr The pointer to the current menu.
 */
typedef void (*MenuSelectedOptionFunc)(Menu *menuPtr);

// Callback function pointer for leaving the current menu.
typedef void (*MenuExitFunc)();

/**
 * Callback function pointer for handling the controls for the current menu.
 *
 * @param menuPtr The pointer to the current menu.
 * @param button The button that was pressed this frame.
 */
typedef void (*MenuControlsFunc)(Menu *menuPtr, MenuButtonInput button);

// Used to store the name and `MenuSelectedOptionFunc` function pointer for an option.
struct MenuOption
{
    // Name of the menu option.
    const char *name;

    // Function to be called when the menu option is selected via pressing `A`. Can be set to `nullptr` if not needed for
    // whatever reason.
    MenuSelectedOptionFunc selectedFunc;
};

// Used to store all of the options and function pointers for the current menu.
struct MenuFunctions
{
    // Pointer to all of the options within the current menu. The amount of options is handled via `totalOptions` when the menu
    // is intitialized.
    const MenuOption *options;

    // Function to be called to handle the controls for the current menu.
    MenuControlsFunc controls;

    // Function to be called to draw the current menu.
    DispCallback draw;

    // Function to be called when exiting the current menu. Can be set to `nullptr` if not needed for whatever reason.
    MenuExitFunc exit;
};

// Used to handle the current menu.
class Menu
{
   public:
    /**
     * Constructor for the menu class.
     *
     * @param funcs Pointer to the options and function pointers to use for the menu.
     * @param totalOptions The total amount of options to choose from in the menu.
     * @param totalOptionsPerPage The total amount of options to choose from on any given page in the menu.
     */
    Menu(const MenuFunctions *funcs, uint32_t totalOptions, uint32_t totalOptionsPerPage)
    {
        this->funcs = funcs;
        this->totalOptions = static_cast<uint8_t>(totalOptions);
        this->totalOptionsPerPage = static_cast<uint8_t>(totalOptionsPerPage);
    }

    // Generic deconstructor.
    ~Menu() {}

    /**
     * Updates the current value of the `prev` variable.
     *
     * @param prev The value to set the `prev` variable to.
     */
    void setPrevMenu(Menu *prev) { this->prev = prev; }

    /**
     * Gets a pointer to the menu's `prev` variable.
     *
     * @returns A pointer to the `prev` variable.
     */
    Menu *getPrevMenuPtr() const { return this->prev; }

    /**
     * Calls the function pointer for handling the menu's controls.
     *
     * @param button The button that was pressed this frame.
     *
     * @note If the function pointer for the controls is `nullptr`, then this function will do nothing.
     */
    void runControlsFunc(MenuButtonInput button);

    /**
     * Calls the function pointer for the option that was selected in the menu.
     *
     * @note If the function pointer for the option is `nullptr`, then this function will do nothing.
     *
     * @note If either of the values for the `currentPage` or `currentIndex` variables are somehow invalid, then this function
     * will fix them.
     */
    void runSelectedOptionFunc();

    /**
     * Calls the function pointer for drawing the menu.
     *
     * @note If the function pointer for the draw function is `nullptr`, then this function will do nothing.
     */
    void runDrawFunc() const;

    /**
     * Calls the function pointer for exiting the menu.
     *
     * @note If the function pointer for the exit function is `nullptr`, then this function will do nothing.
     */
    void runExitFunc() const;

    /**
     * Checks to see if any of the menu's flags are set.
     *
     * @returns `true` if any of the menu's flags are set, otherwise `false`.
     */
    bool anyFlagIsSet() const { return this->flags != 0; }

    // Clears all of the menu's flags, settings the value of the `flags` variable to `0`.
    void clearAllFlags() { this->flags = 0; }

    // Flag functions take values 0-31. The functions return if the flag parameter has a higher value than 31.

    /**
     * Checks to see if a specific flag in the menu is set, in which there are a maximum of `32` flags to check for.
     *
     * @param flag The flag to check for.
     *
     * @returns `true` if the specified flag is set, otherwise `false.` Also returns `false` if the specified flag reaches or
     * exceeds the maximum amount of flags in the menu.
     */
    bool flagIsSet(uint32_t flag) const
    {
        // Make sure the flag is valid
        constexpr uint32_t maxFlags = sizeof(this->flags) * 8;
        if (flag >= maxFlags)
        {
            return false;
        }

        bool ret;
        FLAG_IS_SET(&this->flags, flag, ret);
        return ret;
    }

    /**
     * Sets a specific flag in the menu, in which there are a maximum of `32` flags that can be set.
     *
     * @param flag The flag to set.
     *
     * @note If `flag` reaches or exceeds the maximum amount of flags in the menu, then this function will do nothing.
     */
    void setFlag(uint32_t flag)
    {
        // Make sure the flag is valid
        constexpr uint32_t maxFlags = sizeof(this->flags) * 8;
        if (flag >= maxFlags)
        {
            return;
        }

        SET_FLAG(&this->flags, flag);
    }

    /**
     * Clears a specific flag in the menu, in which there are a maximum of `32` flags that can be cleared.
     *
     * @param flag The flag to clear.
     *
     * @note If `flag` reaches or exceeds the maximum amount of flags in the menu, then this function will do nothing.
     */
    void clearFlag(uint32_t flag)
    {
        // Make sure the flag is valid
        constexpr uint32_t maxFlags = sizeof(this->flags) * 8;
        if (flag >= maxFlags)
        {
            return;
        }

        CLEAR_FLAG(&this->flags, flag);
    }

    /**
     * Toggles a specific flag in the menu, in which there are a maximum of `32` flags that can be toggled.
     *
     * @param flag The flag to toggle.
     *
     * @returns `true` if the specified flag is now set, otherwise `false`. Also returns `false` if the specified flag reaches
     * or exceeds the maximum amount of flags in the menu.
     *
     * @note If `flag` reaches or exceeds the maximum amount of flags in the menu, then this function will do nothing.
     */
    bool toggleFlag(uint32_t flag)
    {
        // Make sure the flag is valid
        constexpr uint32_t maxFlags = sizeof(this->flags) * 8;
        if (flag >= maxFlags)
        {
            return false;
        }

        TOGGLE_FLAG(&this->flags, flag);
        return this->flagIsSet(flag);
    }

    // Gets the total amount of options to choose from in the menu.
    uint32_t getTotalOptions() const { return this->totalOptions; }

    // Gets the current page that is being used/displayed in the menu.
    uint32_t getCurrentPage() const { return this->currentPage; }

    /**
     * Updates the current value of the `currentPage` variable.
     *
     * @param page The value to set the `currentPage` variable to.
     *
     * @note If there are no options on the specified page, then this function will do nothing.
     *
     * @note Upon updating the page, the value of the `currentIndex` variable will be updated to place it at the start of the
     * specified page.
     */
    void setCurrentPage(uint32_t page);

    // Gets the current cursor position in the menu.
    uint32_t getCurrentIndex() const { return this->currentIndex; }

    /**
     * Updates the current value of the `currentIndex` variable.
     *
     * @param index The value to set the `currentIndex` variable to.
     *
     * @note If the specified index reaches or exceeds the maximum amount of options, then this function will do nothing.
     *
     * @note Upon updating `currentIndex`, the value of the `currentPage` variable will be updated to be on the page that
     * contains the specified index.
     */
    void setCurrentIndex(uint32_t index);

    // Gets a pointer to the `currentPage` variable.
    uint8_t *getCurrentPagePtr() { return &this->currentPage; }

    // Gets a pointer to the `currentIndex` variable.
    uint8_t *getCurrentIndexPtr() { return &this->currentIndex; }

    // Generic functions for handling the current menu

    /**
     * Generic drawing function for the menu, which assumes that the menu only has one column of options to choose from.
     *
     * @param lineHeight The line height to use for each option.
     * @param offsetX Offset on the `X` axis to start drawing the options, based on the `left` of the menu's window.
     * @param offsetY Offset on the `Y` axis to start drawing the options, based on the `top` of the menu's window.
     *
     * @note If the menu only has one page of options, then the page number for it will not be drawn. If the page number is
     * drawn, then it will be drawn at the `top-right` of the window.
     */
    void basicLayoutDraw(float lineHeight, float offsetX, float offsetY) const;

    /**
     * Generic controls function for the menu, which assumes that the menu only has one column of options to choose from.
     *
     * @param button The button that was pressed this frame.
     */
    void basicLayoutControls(MenuButtonInput button);

   private:
    Menu *prev;                  // Pointer to the previous menu.
    const MenuFunctions *funcs;  // Functions for handling the menu.
    uint32_t flags;              // Arbitrary flags to set/clear/toggle within the menu.
    uint8_t totalOptions;        // Total amount of options to choose from in the menu.
    uint8_t totalOptionsPerPage; // Total amount of options to choose from on any given page in the menu.
    uint8_t currentPage;         // Current page that is being used/displayed in the menu.
    uint8_t currentIndex;        // Current cursor position in the menu.
};

#endif
