#ifndef CLASSES_VALUEEDITOR_H
#define CLASSES_VALUEEDITOR_H

#include "mod.h"
#include "classes/menu.h"
#include "classes/window.h"

#include <cstdint>

#define MAX_DOUBLE_LENGTH 21 // Excludes the + or -, the decimal point, and the decimal digits
#define MAX_DOUBLE_DIGITS 6

#define MAX_DOUBLE_LENGTH_STRING "21" // Excludes the + or -, the decimal point, and the decimal digits
#define MAX_DOUBLE_DIGITS_STRING "6"

enum ValueEditorFlag
{
    VALUE_IS_SIGNED = 0,
    HANDLE_AS_HEX,
    // DRAW_DPAD_UP_DOWN,
    DRAW_DPAD_LEFT_RIGHT,
    DRAW_BUTTON_Y_SET_MAX,
    DRAW_BUTTON_Z_SET_MIN,

    // Only one of the following can be used at any given time
    DRAW_ITEM_ICON_AND_TEXT,    // Inventory menu/Spawn Item cheat
    DRAW_STAGE_AND_EVENT,       // Change Sequence menu
    DRAW_MAP_STRING,            // Warp By Index menu
    DRAW_WARP_BY_EVENT_DETAILS, // Warp By Event menu
};

typedef void (*ValueEditorSetValueFunc)(const ValueType *valuePtr);
typedef void (*ValueEditorCancelFunc)();

class ValueEditor
{
    // Currently only used for when unsigned values go from UINT_MIN to UINT_MAX and vice versa
    enum MinMaxSpecialCases
    {
        SPECIAL_CASE_NONE = 0,
        SPECIAL_CASE_SET_TO_MIN,
        SPECIAL_CASE_SET_TO_MAX,
    };

   public:
    ValueEditor() {}
    ~ValueEditor() {}

    // Sets the alpha to 0xFF. The value is required to be stored using a type that is either 4 or 8 bytes. In this case, you'd
    // still use the original type for the type variable, as that's used to figure out the min/max of the value as well as how
    // many digits the value should have.
    void init(const void *valuePtr, // Initial value
              const void *minValuePtr,
              const void *maxValuePtr,
              const Window *parentWindow,
              uint32_t flags,
              VariableType type,
              float scale);

    // The value is required to be stored using a type that is either 4 or 8 bytes. In this case, you'd still use the original
    // type for the type variable, as that's used to figure out the min/max of the value as well as how many digits the value
    // should have
    void init(const void *valuePtr, // Initial value
              const void *minValuePtr,
              const void *maxValuePtr,
              const Window *parentWindow,
              uint32_t flags,
              VariableType type,
              uint8_t alpha,
              float scale);

    void initHandleS32(const ValueType *valuePtr,
                       const ValueType *minValuePtr,
                       const ValueType *maxValuePtr,
                       uint32_t maxDigitsDefault,
                       bool minAndMaxSet,
                       bool handleAsHex);

    void initHandleU32(const ValueType *valuePtr,
                       const ValueType *minValuePtr,
                       const ValueType *maxValuePtr,
                       uint32_t maxDigitsDefault,
                       bool minAndMaxSet,
                       bool handleAsHex);

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

    void setCurrentIndex(uint32_t index) { this->currentIndex = static_cast<uint8_t>(index); }

    bool shouldDraw() const { return this->enabled; }
    void stopDrawing() { this->enabled = false; }

    void startDrawing(ValueEditorSetValueFunc setValueFunc, ValueEditorCancelFunc cancelFunc)
    {
        this->setValueFunc = setValueFunc;
        this->cancelFunc = cancelFunc;
        this->enabled = true;
    }

    // Retrieves whatever value is currently in editorValue
    bool getValueFromString(ValueType *valuePtr) const;

    // Must use const void instead of ValueType for valuePtr, as the value is stored in a fixed size (int32_t, uint32_t,
    // etc.)
    bool handleCheckMinMax(const ValueType *valuePtr, bool valueIsPositive, uint32_t specialCase);

    void adjustValue(bool increment);
    void setValueToMin();
    void setValueToMax();

    void controlsMoveLeftOnce(uint32_t maxDigit);
    void controlsMoveRightOnce(uint32_t maxDigit);
    void controls(MenuButtonInput button);

    void draw();

   private:
    Window window;
    float scale; // Based on the help text. The editor value scale will be increased based on this.

    ValueType minValue;
    ValueType maxValue;

    ValueEditorSetValueFunc setValueFunc; // Called when the player presses A to set the new value
    ValueEditorCancelFunc cancelFunc;     // Called when the player presses B to cancel adjustimg the value

    uint32_t flags;                       // Flags for what should be drawn in the window alongside the value

    MenuAutoIncrement autoIncrement;
    uint8_t currentIndex; // Current cursor position
    uint8_t maxDigits;
    uint8_t alpha;

    VariableType type;    // Type of variable being modified
    bool enabled;         // Whether this window is enabled/drawn or not
    char format[16];      // Format string for the value, to be used with snprintf and sscanf
    char editorValue[32]; // Current value in string format. This is what is being modified while the editor is displayed.
    char helpText[192];   // Text that shows what buttons can be pressed to handle certain things in the window
};

uint32_t getMaxDigits(uint32_t value, bool handleAsHex);
uint32_t getMaxDigits(int32_t value, bool handleAsHex);
uint32_t getMaxDigits(uint64_t value, bool handleAsHex);
uint32_t getMaxDigits(int64_t value, bool handleAsHex);

// uint32_t getMaxDigits(float value, bool handleAsHex);
uint32_t getMaxDigits(float value);
uint32_t getMaxDigits(double value);

uint32_t handleAdjustValue(uint32_t value, uint32_t currentIndex, uint32_t totalDigits, bool handleAsHex, bool increment);
int32_t handleAdjustValue(int32_t value, uint32_t currentIndex, uint32_t totalDigits, bool handleAsHex, bool increment);
uint64_t handleAdjustValue(uint64_t value, uint32_t currentIndex, uint32_t totalDigits, bool handleAsHex, bool increment);
int64_t handleAdjustValue(int64_t value, uint32_t currentIndex, uint32_t totalDigits, bool handleAsHex, bool increment);

/*
float handleAdjustValue(float value,
                        uint32_t currentIndex,
                        uint32_t totalDigits,
                        bool valueIsSigned,
                        bool handleAsHex,
                        bool increment);
*/

float handleAdjustValue(float value, uint32_t currentIndex, uint32_t totalDigits, bool valueIsSigned, bool increment);
double handleAdjustValue(double value, uint32_t currentIndex, uint32_t totalDigits, bool valueIsSigned, bool increment);

// void handleAdjustValueDouble(char *valueString, uint32_t currentIndex, bool handleAsHex, bool increment);

int32_t floatToString(char *bufOut, uint32_t bufSize, char *formatOut, uint32_t formatSize, uint32_t totalLength, float value);

int32_t doubleToString(char *bufOut,
                       uint32_t bufSize,
                       char *formatOut,
                       uint32_t formatSize,
                       uint32_t totalLength,
                       double value);

#endif
