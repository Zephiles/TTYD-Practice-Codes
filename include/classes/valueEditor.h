#ifndef CLASSES_VALUEEDITOR_H
#define CLASSES_VALUEEDITOR_H

#include "mod.h"
#include "classes/window.h"
#include "classes/menu.h"

#include <cstdint>

#ifdef TTYD_JP
#define EDITOR_DIGIT_LENGTH 16.f // The amount of space used by each adjustable digit in the value editor.
#else
#define EDITOR_DIGIT_LENGTH 18.8888888f // The amount of space used by each adjustable digit in the value editor.
#endif

// The maximum amount of digits that can be displayed for floats/doubles. Excludes the + or -, the decimal point, and the
// decimal digits.
#define MAX_DOUBLE_LENGTH 21

// The maximum amount of digits that can be after the decimal point in floats/doubles.
#define MAX_DOUBLE_DIGITS 6

// The maximum amount of digits that can be displayed for floats/doubles, converted to a string. Excludes the + or -, the
// decimal point, and the decimal digits.
#define MAX_DOUBLE_LENGTH_STRING "21"

// The maximum amount of digits that can be after the decimal point in floats/doubles, converted to a string.
#define MAX_DOUBLE_DIGITS_STRING "6"

// The minimum float value that can be displayed in the value editor. Based on MAX_DOUBLE_LENGTH, including the decimal digits.
#define MIN_FLOAT_VALUE -9999999999999.f

// The maximum float value that can be displayed in the value editor. Based on MAX_DOUBLE_LENGTH, including the decimal digits.
#define MAX_FLOAT_VAlUE 9999999999999.f

// The minimum double value that can be displayed in the value editor. Based on MAX_DOUBLE_LENGTH, including the decimal digits.
#define MIN_DOUBLE_VALUE -9999999999999.0

// The maximum double value that can be displayed in the value editor. Based on MAX_DOUBLE_LENGTH, including the decimal digits.
#define MAX_DOUBLE_VAlUE 9999999999999.0

/**
 * Callback function pointer for when the player presses `A` to set the new value.
 *
 * @param valuePtr Pointer to where the new value is stored.
 *
 * @note `valuePtr` assumes that the value is required to be stored using a type that is either 4 or 8 bytes. As an example, if
 * the initial value came from an `int8_t` or `int16_t` variable, then it would be stored as an `int32_t` value at `valuePtr`.
 * Note that the default minimum/maximum values would still be applied for the original type, even if it would be stored as
 * either 4 or 8 bytes.
 */
typedef void (*ValueEditorSetValueFunc)(const ValueType *valuePtr);

// Callback function pointer for when the player presses `B` to cancel adjustimg the value.
typedef void (*ValueEditorCancelFunc)();

// Flags for handling the value in specific ways.
enum ValueEditorFlag
{
    // Assumes the value is signed.
    VALUE_IS_SIGNED = 0,

    // Assumes the value should be displayed/handled as a hexadecimal value.
    HANDLE_AS_HEX,

    // Assumes that the value editor window should be placed at the top of the parent window, rather than in the middle of it.
    WINDOW_POSITION_TOP,

    // Assumes that `D-Pad Up`/`Down` can be used to adjust the value of the current digit. When this flag is set, the value
    // editor will display text indicating so.
    // DRAW_DPAD_UP_DOWN,

    // Assumes that `D-Pad Left`/`Right` can be used to navigate between digits. When this flag is set, the value
    // editor will display text indicating so.
    DRAW_DPAD_LEFT_RIGHT,

    // Assumes that `Y` can be pressed to set the value to the maximum value. When this flag is set, the value
    // editor will display text indicating so.
    DRAW_BUTTON_Y_SET_MAX,

    // Assumes that `Z` can be pressed to set the value to the minimum value. When this flag is set, the value
    // editor will display text indicating so.
    DRAW_BUTTON_Z_SET_MIN,

    // Assumes that `X` can be held to hide the value editor. When this flag is set, the value editor will display text
    // indicating so.
    DRAW_BUTTON_X_HIDE,

    // Flags for other misc things

    // Used for the `Battles Stats` menu. When this flag is set, the values for items that do not normally appear when held by
    // NPCs in battles will be skipped when adjusting the value.
    BATTLES_CHANGE_HELD_ITEM,

    // Used for the `Force NPC Item Drop` cheat. When this flag is set, the values for damaging items that can be thrown from
    // the audience (such as rocks) will be skipped when adjusting the value.
    CHEATS_CHANGE_DROPPED_ITEM,

    // Only one of the following flags can be used at any given time

    // Used for the `Inventory` menu, the `Battles Stats` menu, and for the `Spawn Item` cheat. When this flag is set, the value
    // editor will assume that the value is for an item, and the `icon` and `text` for the item will be drawn above the value.
    DRAW_ITEM_ICON_AND_TEXT,

    // Used for the `Change Sequence` menu. When this flag is set, the value editor will assume that the value is for indicating
    // an event, so the `Stage` and `Event` strings for the event will be drawn above the value.
    DRAW_STAGE_AND_EVENT,

    // Used for the `Warp By Index` menu. When this flag is set, the value editor will assume that the value is for indicating
    // a map string, so the string will be drawn above the value.
    DRAW_MAP_STRING,

    // Used for the `Warp By Event` menu. When this flag is set, the value editor will assume that the value is for selecting an
    // event, so the details for the event will be drawn above the value.
    DRAW_WARP_BY_EVENT_DETAILS,
};

// Handles adjusting an arbitrary value, with various options for how the value should be handled/formatted. A help window with
// text is displayed to assist in this process.
class ValueEditor
{
    // Used for handling instances where the value needs to be set to the minimum/maximum value under special circumstances.
    // Currently only used for when unsigned values go from UINT_MIN to UINT_MAX and vice versa.
    enum MinMaxSpecialCases
    {
        SPECIAL_CASE_NONE = 0,   // No special action(s) needed.
        SPECIAL_CASE_SET_TO_MIN, // Set the value to the minimum value.
        SPECIAL_CASE_SET_TO_MAX, // Set the value to the maximum value.
    };

   public:
    // Generic constructor.
    ValueEditor() {}

    // Generic deconstructor.
    ~ValueEditor() {}

    /**
     * Initializes the value editor.
     *
     * @param valuePtr Pointer to the initial value to be used in the value editor.
     * @param minValuePtr Pointer to the minimum value that the value can be in the value editor. Can be set to `nullptr` if not
     * needed (see note below).
     * @param maxValuePtr Pointer to the maximum value that the value can be in the value editor. Can be set to `nullptr` if not
     * needed (see note below).
     * @param parentWindow The window to place the value editor's window in.
     * @param flags The flags used to specify how the value should be handled, based on `ValueEditorFlag`.
     * @param type The variable type that the initial value came from. As an example, if the initial value came from an `int8_t`
     * variable, then the type would be `s8`. This variable also determines the default minimum/maximum values for the value, as
     * well as how many digits it should have in the value editor.
     *
     * @note The value parameters for this function are required to be stored using a type that is either 4 or 8 bytes. As an
     * example, if the initial value came from an `int8_t` or `int16_t` variable, then it would be stored as an `int32_t` value
     * at the pointer.
     *
     * @note Both `minValuePtr` and `maxValuePtr` are required to be set to pointers to proper values in order to actually be
     * used.
     *
     * @note `type` should not be set to `string` nor `time`, as these formats are not supported in the value editor.
     *
     * @note This function calls the `init` function that takes `windowAlpha` as a parameter, in which `0xFF` is passed in for
     * that value.
     */
    void init(const void *valuePtr,
              const void *minValuePtr,
              const void *maxValuePtr,
              const Window *parentWindow,
              uint32_t flags,
              VariableType type);

    /**
     * Initializes the value editor.
     *
     * @param valuePtr Pointer to the initial value to be used in the value editor.
     * @param minValuePtr Pointer to the minimum value that the value can be in the value editor. Can be set to `nullptr` if not
     * needed (see note below).
     * @param maxValuePtr Pointer to the maximum value that the value can be in the value editor. Can be set to `nullptr` if not
     * needed (see note below).
     * @param parentWindow The window to place the value editor's window in.
     * @param flags The flags used to specify how the value should be handled, based on `ValueEditorFlag`.
     * @param type The variable type that the initial value came from. As an example, if the initial value came from an `int8_t`
     * variable, then the type would be `s8`. This variable also determines the default minimum/maximum values for the value, as
     * well as how many digits it should have in the value editor.
     * @param windowAlpha The value to set the value editor's window alpha to.
     *
     * @note The value parameters for this function are required to be stored using a type that is either 4 or 8 bytes. As an
     * example, if the initial value came from an `int8_t` or `int16_t` variable, then it would be stored as an `int32_t` value
     * at the pointer.
     *
     * @note `type` should not be set to `string` nor `time`, as these formats are not supported in the value editor.
     *
     * @note Both `minValuePtr` and `maxValuePtr` are required to be set to pointers to proper values in order to actually be
     * used.
     *
     * @overload
     */
    void init(const void *valuePtr,
              const void *minValuePtr,
              const void *maxValuePtr,
              const Window *parentWindow,
              uint32_t flags,
              VariableType type,
              uint8_t windowAlpha);

    // Flag functions take values 0-31. The functions return if the flag parameter has a higher value than 31.

    /**
     * Checks if a specific flag is set in the value editor.
     *
     * @param flag The flag to check for.
     *
     * @returns `true` if the flag is set, otherwise `false`.
     */
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

    /**
     * Sets a specified flag in the value editor.
     *
     * @param flags All of the current flags that are set in the value editor.
     * @param flag The new flag to set.
     *
     * @returns The updated `flags` parameter value.
     */
    uint32_t setFlag(uint32_t flags, uint32_t flag)
    {
        // Make sure the flag is valid
        constexpr uint32_t maxFlags = sizeof(flags) * 8;
        if (flag >= maxFlags)
        {
            return flags;
        }

        return flags |= (1UL << flag);
    }

    /**
     * Updates the current value of the `currentIndex` variable.
     *
     * @param index The value to set the `currentIndex` variable to.
     */
    void setCurrentIndex(uint32_t index) { this->currentIndex = static_cast<uint8_t>(index); }

    /**
     * Checks to see if the value editor should be drawn this frame.
     *
     * @returns `true` if the value editor should be drawn this frame, otherwise `false`.
     */
    bool shouldDraw() const { return this->enabled; }

    /**
     * Sets the value editor to be drawn.
     *
     * @param setValueFunc Callback function for when the player presses `A` to set the new value.
     * @param cancelFunc Callback function for when the player presses `B` to cancel adjustimg the value.
     */
    void startDrawing(ValueEditorSetValueFunc setValueFunc, ValueEditorCancelFunc cancelFunc)
    {
        this->setValueFunc = setValueFunc;
        this->cancelFunc = cancelFunc;
        this->enabled = true;
    }

    // Sets the value editor to not be drawn.
    void stopDrawing() { this->enabled = false; }

    /**
     * Updates the current value of the value editor's window alpha.
     *
     * @param alpha The value to set the value editor's window alpha to.
     */
    void setWindowAlpha(uint8_t alpha) { this->window.setAlpha(alpha); }

    /**
     * Retrieves whatever value is currently in the `editorValue` variable.
     *
     * @param[out] valuePtr Pointer to where the value will be stored.
     *
     * @returns `true` if the value was successfully retrieved, otherwise `false`.
     *
     * @note `valuePtr` assumes that the value is required to be stored using a type that is either 4 or 8 bytes. As an example,
     * if the initial value came from an `int8_t` or `int16_t` variable, then it will be stored as an `int32_t` value at
     * `valuePtr`. Note that the default minimum/maximum values will still be applied for the original type, even if it will be
     * stored as either 4 or 8 bytes.
     */
    bool getValueFromString(ValueType *valuePtr) const;

    /**
     * Handles the controls for the value editor.
     *
     * @param button The button that was pressed this frame.
     */
    void controls(MenuButtonInput button);

    // Draws the value editor.
    void draw();

   private:
    /**
     * Handles checking if the adjusted value now exceeds the minimum/maximum value, and adjusting it if so.
     *
     * @param valuePtr Pointer to where the value is stored.
     * @param valueIsPositive Whether the value is now considered positive or negative. If `true`, then the value is considered
     * to be positive.
     * @param specialCase Special cases that need to be applied to the value, based on `MinMaxSpecialCases`.
     *
     * @returns `true` if the value had to be set to the minimum/maximum value, otherwise `false`.
     *
     * @note `valuePtr` assumes that the value is required to be stored using a type that is either 4 or 8 bytes. As an example,
     * if the initial value came from an `int8_t` or `int16_t` variable, then it would be stored as an `int32_t` value at
     * `valuePtr`. Note that the default minimum/maximum values would still be applied for the original type, even if it would
     * be stored as either 4 or 8 bytes.
     */
    bool handleCheckMinMax(const ValueType *valuePtr, bool valueIsPositive, uint32_t specialCase);

    /**
     * Handles adjusting the current digit of the value in the value editor.
     *
     * @param increment Whether the current digit is being incremented or decremented. If `true`, then the value is being
     * incremented.
     */
    void adjustValue(bool increment);

    // Sets the current value in the value editor to the minimum value.
    void setValueToMin();

    // Sets the current value in the value editor to the maximum value.
    void setValueToMax();

    /**
     * Handles moving left once to the previous digit in the value editor. If currently at the furthest-left digit, then it will
     * move to the furthest-right digit.
     *
     * @param maxDigit The index for the furthest-right digit.
     */
    void controlsMoveLeftOnce(uint32_t maxDigit);

    /**
     * Handles moving right once to the next digit in the value editor. If currently at the furthest-right digit, then it will
     * move to the furthest-left digit.
     *
     * @param maxDigit The index for the furthest-right digit.
     */
    void controlsMoveRightOnce(uint32_t maxDigit);

    // The window to place all of the value editor's text and other stuff in.
    Window window;

    // The minimum value that the value in the value editor can be set to.
    ValueType minValue;

    // The maximum value that the value in the value editor can be set to.
    ValueType maxValue;

    // Callback function for when the player presses `A` to set the new value.
    ValueEditorSetValueFunc setValueFunc;

    // Callback function for when the player presses `B` to cancel adjustimg the value.
    ValueEditorCancelFunc cancelFunc;

    // The flags used to specify how the value should be handled, based on `ValueEditorFlag`.
    uint32_t flags;

    // Used to handle automatically adjusting the current digit of the value in the value editor when a `D-Pad` direction is
    // held.
    MenuAutoIncrement autoIncrement;

    // Current cursor/digit position.
    uint8_t currentIndex;

    // The maximum amount of digits that can be displayed for the value in the value editor.
    uint8_t maxDigits;

    /**
     * The variable type that the initial value came from. As an example, if the initial value came from an `int8_t` variable,
     * then the type would be `s8`. This variable also determines the default minimum/maximum values for the value, as well as
     * how many digits it should have in the value editor.
     *
     * @note This should not be set to `string` nor `time`, as these formats are not supported in the value editor.
     */
    VariableType type;

    // Whether the value editor is enabled/drawn or not.
    bool enabled;

    // The format string for the value, to be used with snprintf and sscanf.
    char format[16];

    // The current value in the value editor in string format. This is what is being modified while the value editor is
    // displayed.
    char editorValue[32];

    // Text that shows what buttons can be pressed to handle certain things in the value editor.
    char helpText[224];
};

#endif
