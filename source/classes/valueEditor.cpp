#include "mod.h"
#include "drawIcon.h"
#include "drawText.h"
#include "menuUtils.h"
#include "classes/valueEditor.h"
#include "classes/window.h"
#include "gc/pad.h"
#include "menus/cheatsMenu.h"
#include "menus/warpsMenu.h"
#include "misc/utils.h"

#include <cstdint>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <cinttypes>

// Helper functions

static uint32_t getMaxDigits(uint32_t value, bool handleAsHex);

static uint32_t getMaxDigits(int32_t value, bool handleAsHex);

static uint32_t getMaxDigits(uint64_t value, bool handleAsHex);

static uint32_t getMaxDigits(int64_t value, bool handleAsHex);

// static uint32_t getMaxDigits(float value, bool handleAsHex);

static uint32_t getMaxDigits(float value);

static uint32_t getMaxDigits(double value);

static uint32_t handleAdjustValue(uint32_t value,
                                  uint32_t currentIndex,
                                  uint32_t totalDigits,
                                  bool handleAsHex,
                                  bool increment);

static int32_t handleAdjustValue(int32_t value, uint32_t currentIndex, uint32_t totalDigits, bool handleAsHex, bool increment);

static uint64_t handleAdjustValue(uint64_t value,
                                  uint32_t currentIndex,
                                  uint32_t totalDigits,
                                  bool handleAsHex,
                                  bool increment);

static int64_t handleAdjustValue(int64_t value, uint32_t currentIndex, uint32_t totalDigits, bool handleAsHex, bool increment);

/*
static float handleAdjustValue(float value,
                               uint32_t currentIndex,
                               uint32_t totalDigits,
                               bool valueIsSigned,
                               bool handleAsHex,
                               bool increment);
*/

static float handleAdjustValue(float value, uint32_t currentIndex, uint32_t totalDigits, bool valueIsSigned, bool increment);

static double handleAdjustValue(double value, uint32_t currentIndex, uint32_t totalDigits, bool valueIsSigned, bool increment);

// static void handleAdjustValueDouble(char *valueString, uint32_t currentIndex, bool handleAsHex, bool increment);

static int32_t floatToString(char *bufOut,
                             uint32_t bufSize,
                             char *formatOut,
                             uint32_t formatSize,
                             uint32_t totalLength,
                             float value);

static int32_t doubleToString(char *bufOut,
                              uint32_t bufSize,
                              char *formatOut,
                              uint32_t formatSize,
                              uint32_t totalLength,
                              double value);

// Main functions

void ValueEditor::init(const void *valuePtr,
                       const void *minValuePtr,
                       const void *maxValuePtr,
                       const Window *parentWindow,
                       uint32_t flags,
                       VariableType type)
{
    this->init(valuePtr, minValuePtr, maxValuePtr, parentWindow, flags, type, 0xFF);
}

void ValueEditor::init(const void *valuePtr,
                       const void *minValuePtr,
                       const void *maxValuePtr,
                       const Window *parentWindow,
                       uint32_t flags,
                       VariableType type,
                       uint8_t windowAlpha)
{
    this->flags = flags;
    this->setValueFunc = nullptr;
    this->cancelFunc = nullptr;
    this->autoIncrement.waitFramesToBegin = 0;
    this->autoIncrement.framesBeforeIncrement = 0;
    this->enabled = false;

    // If the value is a float/double and is being handled as hex, then handle them as u32/u64 respectively
    const bool handleAsHex = this->flagIsSet(ValueEditorFlag::HANDLE_AS_HEX);
    if (handleAsHex)
    {
        if (type == VariableType::f32)
        {
            type = VariableType::u32;
        }
        else if (type == VariableType::f64)
        {
            type = VariableType::u64;
        }
    }

    // Set the min and max based on the type, and get the default max digits for the number
    const bool valueIsSigned = this->flagIsSet(ValueEditorFlag::VALUE_IS_SIGNED);
    uint32_t maxDigits = 1;

    switch (type)
    {
        case VariableType::s8:
        {
            if (valueIsSigned)
            {
                this->minValue.s32 = INT8_MIN;
            }
            else
            {
                this->minValue.s32 = 0;
            }

            this->maxValue.s32 = INT8_MAX;

            if (handleAsHex)
            {
                maxDigits = 2; // 7F
            }
            else
            {
                maxDigits = 3; // 127
            }
            break;
        }
        case VariableType::s16:
        {
            if (valueIsSigned)
            {
                this->minValue.s32 = INT16_MIN;
            }
            else
            {
                this->minValue.s32 = 0;
            }

            this->maxValue.s32 = INT16_MAX;

            if (handleAsHex)
            {
                maxDigits = 4; // 7FFF
            }
            else
            {
                maxDigits = 5; // 32767
            }
            break;
        }
        case VariableType::s32:
        {
            if (valueIsSigned)
            {
                this->minValue.s32 = INT32_MIN;
            }
            else
            {
                this->minValue.s32 = 0;
            }

            this->maxValue.s32 = INT32_MAX;

            if (handleAsHex)
            {
                maxDigits = 8; // 7FFFFFFF
            }
            else
            {
                maxDigits = 10; // 2147483647
            }
            break;
        }
        case VariableType::s64:
        {
            if (valueIsSigned)
            {
                this->minValue.s64 = INT64_MIN;
            }
            else
            {
                this->minValue.s64 = 0;
            }

            this->maxValue.s64 = INT64_MAX;

            if (handleAsHex)
            {
                maxDigits = 16; // 7FFFFFFFFFFFFFFF
            }
            else
            {
                maxDigits = 19; // 9223372036854775807
            }
            break;
        }
        case VariableType::u8:
        {
            this->minValue.u32 = 0;
            this->maxValue.u32 = UINT8_MAX;

            if (handleAsHex)
            {
                maxDigits = 2; // FF
            }
            else
            {
                maxDigits = 3; // 255
            }
            break;
        }
        case VariableType::u16:
        {
            this->minValue.u32 = 0;
            this->maxValue.u32 = UINT16_MAX;

            if (handleAsHex)
            {
                maxDigits = 4; // FFFF
            }
            else
            {
                maxDigits = 5; // 65535
            }
            break;
        }
        case VariableType::u32:
        {
            this->minValue.u32 = 0;
            this->maxValue.u32 = UINT32_MAX;

            if (handleAsHex)
            {
                maxDigits = 8; // FFFFFFFF
            }
            else
            {
                maxDigits = 10; // 4294967295
            }
            break;
        }
        case VariableType::u64:
        {
            this->minValue.u64 = 0;
            this->maxValue.u64 = UINT64_MAX;

            if (handleAsHex)
            {
                maxDigits = 16; // FFFFFFFFFFFFFFFF
            }
            else
            {
                maxDigits = 20; // 18446744073709551615
            }
            break;
        }
        case VariableType::f32:
        {
            if (valueIsSigned)
            {
                this->minValue.f32 = MIN_FLOAT_VALUE;
            }
            else
            {
                this->minValue.f32 = 0.f;
            }

            this->maxValue.f32 = MAX_FLOAT_VAlUE;

            // Subtract 1 to get the max digit
            maxDigits = MAX_DOUBLE_LENGTH - 1;

            if (valueIsSigned)
            {
                // Subtract 1 to account for the + or -
                maxDigits--;
            }
            break;
        }
        case VariableType::f64:
        {
            if (valueIsSigned)
            {
                this->minValue.f64 = MIN_DOUBLE_VALUE;
            }
            else
            {
                this->maxValue.f64 = 0.0;
            }

            this->maxValue.f64 = MAX_DOUBLE_VAlUE;

            // Subtract 1 to get the max digit
            maxDigits = MAX_DOUBLE_LENGTH - 1;

            if (valueIsSigned)
            {
                // Subtract 1 to account for the + or -
                maxDigits--;
            }
            break;
        }
        default:
        {
            break;
        }
    }

    // If the value is not being handled as signed, then change the type to unsigned
    if (!valueIsSigned)
    {
        switch (type)
        {
            case VariableType::s8:
            case VariableType::s16:
            case VariableType::s32:
            {
                type = VariableType::u32;
                break;
            }
            case VariableType::s64:
            {
                type = VariableType::u64;
                break;
            }
            default:
            {
                break;
            }
        }
    }

    // Change smaller types to 4 bytes if necessary
    switch (type)
    {
        case VariableType::s8:
        case VariableType::s16:
        {
            type = VariableType::s32;
            break;
        }
        case VariableType::u8:
        case VariableType::u16:
        {
            type = VariableType::u32;
            break;
        }
        default:
        {
            break;
        }
    }

    this->type = type;

    // Set up format, totalDigits, and editorValue
    char *format = this->format;
    char *editorValue = this->editorValue;
    const bool minAndMaxSet = minValuePtr && maxValuePtr;

    char *editorValuePtr = editorValue;
    uint32_t editorValueSize = sizeof(this->editorValue);

    // Handle the values as ValueType
    const ValueType *newValuePtr = reinterpret_cast<const ValueType *>(valuePtr);
    const ValueType *newMinValuePtr = reinterpret_cast<const ValueType *>(minValuePtr);
    const ValueType *newMaxValuePtr = reinterpret_cast<const ValueType *>(maxValuePtr);

    switch (type)
    {
        case VariableType::s32:
        {
            if (minAndMaxSet)
            {
                const int32_t maxValue = newMaxValuePtr->s32;
                this->maxValue.s32 = maxValue;
                const uint32_t digitsMaxValue = getMaxDigits(maxValue, handleAsHex);

                const int32_t minValue = newMinValuePtr->s32;
                this->minValue.s32 = minValue;
                const uint32_t digitsMinValue = getMaxDigits(minValue, handleAsHex);

                if (digitsMaxValue > digitsMinValue)
                {
                    maxDigits = digitsMaxValue;
                }
                else
                {
                    maxDigits = digitsMinValue;
                }

                // Failsafe: Make sure maxDigits is not 0
                if (maxDigits == 0)
                {
                    maxDigits = 1;
                }
            }

            this->maxDigits = static_cast<uint8_t>(maxDigits);

            uint32_t currentIndex = maxDigits;
            if (handleAsHex)
            {
                // Add 2 to account for 0x
                currentIndex += 2;
            }
            this->currentIndex = static_cast<uint8_t>(currentIndex);

            const char *tempFormat;
            if (handleAsHex)
            {
                tempFormat = "0x%%0%" PRIu32 PRIX32;
            }
            else
            {
                tempFormat = "%%0%" PRIu32 PRId32;
            }

            // Put + or - in front of the value
            int32_t value = newValuePtr->s32;
            char *editorValuePtr = this->editorValue;

            if (value >= 0)
            {
                editorValuePtr[0] = '+';
            }
            else
            {
                editorValuePtr[0] = '-';
                value = -value;
            }

            editorValuePtr++;
            editorValueSize--;

            snprintf(format, sizeof(this->format), tempFormat, maxDigits);
            snprintf(editorValuePtr, editorValueSize, format, value);
            break;
        }
        case VariableType::s64:
        {
            if (minAndMaxSet)
            {
                const int64_t maxValue = newMaxValuePtr->s64;
                this->maxValue.s64 = maxValue;
                const uint32_t digitsMaxValue = getMaxDigits(maxValue, handleAsHex);

                const int64_t minValue = newMinValuePtr->s64;
                this->minValue.s64 = minValue;
                const uint32_t digitsMinValue = getMaxDigits(minValue, handleAsHex);

                if (digitsMaxValue > digitsMinValue)
                {
                    maxDigits = digitsMaxValue;
                }
                else
                {
                    maxDigits = digitsMinValue;
                }

                // Failsafe: Make sure maxDigits is not 0
                if (maxDigits == 0)
                {
                    maxDigits = 1;
                }
            }

            this->maxDigits = static_cast<uint8_t>(maxDigits);

            uint32_t currentIndex = maxDigits;
            if (handleAsHex)
            {
                // Add 2 to account for 0x
                currentIndex += 2;
            }
            this->currentIndex = static_cast<uint8_t>(currentIndex);

            const char *tempFormat;
            if (handleAsHex)
            {
                tempFormat = "0x%%0%" PRIu32 PRIX64;
            }
            else
            {
                tempFormat = "%%0%" PRIu32 PRId64;
            }

            // Put + or - in front of the value
            int64_t value = newValuePtr->s64;
            if (value >= 0)
            {
                editorValuePtr[0] = '+';
            }
            else
            {
                editorValuePtr[0] = '-';
                value = -value;
            }

            editorValuePtr++;
            editorValueSize--;

            snprintf(format, sizeof(this->format), tempFormat, maxDigits);
            snprintf(editorValuePtr, editorValueSize, format, value);
            break;
        }
        case VariableType::u32:
        {
            if (minAndMaxSet)
            {
                const uint32_t maxValue = newMaxValuePtr->u32;
                this->maxValue.u32 = maxValue;
                const uint32_t digitsMaxValue = getMaxDigits(maxValue, handleAsHex);

                const int32_t minValue = newMinValuePtr->u32;
                this->minValue.u32 = minValue;
                const uint32_t digitsMinValue = getMaxDigits(minValue, handleAsHex);

                if (digitsMaxValue > digitsMinValue)
                {
                    maxDigits = digitsMaxValue;
                }
                else
                {
                    maxDigits = digitsMinValue;
                }

                // Failsafe: Make sure maxDigits is not 0
                if (maxDigits == 0)
                {
                    maxDigits = 1;
                }
            }

            this->maxDigits = static_cast<uint8_t>(maxDigits);

            // Subtract 1 since neither + nor - are not present
            uint32_t currentIndex = maxDigits - 1;

            if (handleAsHex)
            {
                // Add 2 to account for 0x
                currentIndex += 2;
            }
            this->currentIndex = static_cast<uint8_t>(currentIndex);

            const char *tempFormat;
            if (handleAsHex)
            {
                tempFormat = "0x%%0%" PRIu32 PRIX32;
            }
            else
            {
                tempFormat = "%%0%" PRIu32 PRIu32;
            }

            snprintf(format, sizeof(this->format), tempFormat, maxDigits);
            snprintf(this->editorValue, sizeof(this->editorValue), format, newValuePtr->u32);
            break;
        }
        case VariableType::u64:
        {
            if (minAndMaxSet)
            {
                const uint64_t maxValue = newMaxValuePtr->u64;
                this->maxValue.u64 = maxValue;
                const uint32_t digitsMaxValue = getMaxDigits(maxValue, handleAsHex);

                const uint64_t minValue = newMinValuePtr->u64;
                this->minValue.u64 = minValue;
                const uint32_t digitsMinValue = getMaxDigits(minValue, handleAsHex);

                if (digitsMaxValue > digitsMinValue)
                {
                    maxDigits = digitsMaxValue;
                }
                else
                {
                    maxDigits = digitsMinValue;
                }

                // Failsafe: Make sure maxDigits is not 0
                if (maxDigits == 0)
                {
                    maxDigits = 1;
                }
            }

            this->maxDigits = static_cast<uint8_t>(maxDigits);

            // Subtract 1 since neither + nor - are not present
            uint32_t currentIndex = maxDigits - 1;

            if (handleAsHex)
            {
                // Add 2 to account for 0x
                currentIndex += 2;
            }
            this->currentIndex = static_cast<uint8_t>(currentIndex);

            const char *tempFormat;
            if (handleAsHex)
            {
                tempFormat = "0x%%0%" PRIu32 PRIX64;
            }
            else
            {
                tempFormat = "%%0%" PRIu32 PRIu64;
            }

            snprintf(format, sizeof(this->format), tempFormat, maxDigits);
            snprintf(editorValuePtr, editorValueSize, format, newValuePtr->u64);
            break;
        }
        case VariableType::f32:
        {
            if (minAndMaxSet)
            {
                const float maxValue = newMaxValuePtr->f32;
                this->maxValue.f32 = maxValue;
                // uint32_t digitsMaxValue = getMaxDigits(maxValue, handleAsHex);
                const uint32_t digitsMaxValue = getMaxDigits(maxValue);

                const float minValue = newMinValuePtr->f32;
                this->minValue.f32 = minValue;
                // uint32_t digitsMinValue = getMaxDigits(minValue, handleAsHex);
                const uint32_t digitsMinValue = getMaxDigits(minValue);

                if (digitsMaxValue > digitsMinValue)
                {
                    maxDigits = digitsMaxValue;
                }
                else
                {
                    maxDigits = digitsMinValue;
                }

                // Failsafe: Make sure maxDigits is not 0
                if (maxDigits == 0)
                {
                    maxDigits = 1;
                }
            }

            this->maxDigits = static_cast<uint8_t>(maxDigits);

            // Subtract 1 to get the last digit
            uint32_t currentIndex = maxDigits - 1;

            if (valueIsSigned)
            {
                // Add 1 to account for the + or -
                currentIndex++;
            }

            // Add 1 to account for the decimal point
            currentIndex++;

            this->currentIndex = static_cast<uint8_t>(currentIndex);

            // If the value is signed, then put + or - in front of the value
            float value = newValuePtr->f32;
            if (valueIsSigned)
            {
                if (!std::signbit(value)) // Check if positive; works for checking against +0.0 and -0.0
                {
                    editorValuePtr[0] = '+';
                }
                else
                {
                    editorValuePtr[0] = '-';
                    value = -value;
                }

                editorValuePtr++;
                editorValueSize--;
            }

            int32_t len = floatToString(editorValuePtr, editorValueSize, format, sizeof(this->format), maxDigits + 1, value);

            // Make sure an error didn't occur
            if (len <= 0)
            {
                floatToString(editorValuePtr, editorValueSize, format, sizeof(this->format), maxDigits + 1, minValue.f32);
            }
            break;
        }
        case VariableType::f64:
        {
            if (minAndMaxSet)
            {
                const double maxValue = newMaxValuePtr->f64;
                this->maxValue.f64 = maxValue;
                const uint32_t digitsMaxValue = getMaxDigits(maxValue);

                const double minValue = newMinValuePtr->f64;
                this->maxValue.f64 = minValue;
                const uint32_t digitsMinValue = getMaxDigits(minValue);

                if (digitsMaxValue > digitsMinValue)
                {
                    maxDigits = digitsMaxValue;
                }
                else
                {
                    maxDigits = digitsMinValue;
                }

                // Failsafe: Make sure maxDigits is not 0
                if (maxDigits == 0)
                {
                    maxDigits = 1;
                }
            }

            this->maxDigits = static_cast<uint8_t>(maxDigits);

            // Subtract 1 to get the last digit
            uint32_t currentIndex = maxDigits - 1;

            if (valueIsSigned)
            {
                // Add 1 to account for the + or -
                currentIndex++;
            }

            // Add 1 to account for the decimal point
            currentIndex++;

            this->currentIndex = static_cast<uint8_t>(currentIndex);

            // If the value is signed, then put + or - in front of the value
            double value = newValuePtr->f64;
            if (valueIsSigned)
            {
                if (!std::signbit(value)) // Check if positive; works for checking against +0.0 and -0.0
                {
                    editorValuePtr[0] = '+';
                }
                else
                {
                    editorValuePtr[0] = '-';
                    value = -value;
                }

                editorValuePtr++;
                editorValueSize--;
            }

            int32_t len = doubleToString(editorValuePtr, editorValueSize, format, sizeof(this->format), maxDigits + 1, value);

            // Make sure an error didn't occur
            if (len <= 0)
            {
                doubleToString(editorValuePtr, editorValueSize, format, sizeof(this->format), maxDigits + 1, minValue.f64);
            }
            break;
        }
        default:
        {
            break;
        }
    }

    // Set up the help text
    char *helpText = this->helpText;

    // Use snprintf to make sure a buffer overflow doesn't occur
    constexpr uint32_t helpTextSize = sizeof(this->helpText);
    int32_t len = 0;

    /*
    static const char *buttonStrings[] = {
        "Press D-Pad Up/Down to adjust the value\n",
        "Press D-Pad Left/Right to change digits\n",
        "Press Y to set the value to max\n",
        "Press Z to set the value to min\n",
    };

    for (uint32_t i = 0; i < 4; i++)
    {
        if (this->flagIsSet(ValueEditorFlag::DRAW_DPAD_UP_DOWN + i))
        {
            len += snprintf(&helpText[len], helpTextSize - len, buttonStrings[i]);
        }
    }
    */

    /*
    if (this->flagIsSet(ValueEditorFlag::DRAW_DPAD_UP_DOWN))
    {
        len = snprintf(&helpText[len], helpTextSize - len, "Press D-Pad Up/Down to adjust the value\n");
    }
    */

    len += snprintf(&helpText[len], helpTextSize - len, "Press D-Pad Up/Down to adjust the value\n");

    if (this->flagIsSet(ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT))
    {
        len += snprintf(&helpText[len], helpTextSize - len, "Press D-Pad Left/Right to change digits\n");
    }

    // Both a min and a max need to be set to use either of them
    if (minAndMaxSet)
    {
        if (this->flagIsSet(ValueEditorFlag::DRAW_BUTTON_Y_SET_MAX))
        {
            len += snprintf(&helpText[len], helpTextSize - len, "Press Y to set the value to max\n");
        }

        if (this->flagIsSet(ValueEditorFlag::DRAW_BUTTON_Z_SET_MIN))
        {
            len += snprintf(&helpText[len], helpTextSize - len, "Press Z to set the value to min\n");
        }
    }

    if (this->flagIsSet(ValueEditorFlag::DRAW_BUTTON_X_HIDE))
    {
        len += snprintf(&helpText[len], helpTextSize - len, "Hold X to hide this window\n");
    }

    snprintf(&helpText[len], helpTextSize - len, "Press A to confirm\nPress B to cancel");

    // Set up the window
    // Initialize it based on the help text
    Window *windowPtr = &this->window;
    constexpr float scale = MENU_SCALE;
    windowPtr->setWidthHeightFromTextAndInit(helpText, scale, getSpecialWindowColor(windowAlpha), 20.f, 20.f);

    // Increase the height to account for the value
    constexpr float valueScale = scale + 0.3f;
    const float padding = windowPtr->getPadding();
    const float paddingScaled = padding * scale;
    windowPtr->setHeight(windowPtr->getHeight() + ((LINE_HEIGHT_FLOAT * valueScale) / 2.f) + paddingScaled);

    // Increase the height of the window based on flags that are set
    const float valuePaddingScaled = padding * valueScale;
    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;

    if (this->flagIsSet(ValueEditorFlag::DRAW_ITEM_ICON_AND_TEXT))
    {
        // Account for the icon and item text
        windowPtr->setHeight(windowPtr->getHeight() + SPACE_USED_PER_ICON(scale) + paddingScaled);
    }
    else if (this->flagIsSet(ValueEditorFlag::DRAW_STAGE_AND_EVENT))
    {
        // Account for the stage and event texts
        windowPtr->setHeight(windowPtr->getHeight() + (lineDecrement * 2.f) + valuePaddingScaled);
    }
    else if (this->flagIsSet(ValueEditorFlag::DRAW_MAP_STRING))
    {
        // Account for the map text
        windowPtr->setHeight(windowPtr->getHeight() + lineDecrement + valuePaddingScaled);
    }
    else if (this->flagIsSet(ValueEditorFlag::DRAW_WARP_BY_EVENT_DETAILS))
    {
        // Account for all of the extra lines of text
        windowPtr->setHeight(windowPtr->getHeight() + (lineDecrement * 7.f) + valuePaddingScaled);
    }

    // If the width of the value text will be larger than the current width, then reset the width to the width of the value text
    const float valueStringLength = intToFloat(static_cast<int32_t>(strlen(editorValue)));
    const float valueStringWidth = (valueStringLength * EDITOR_DIGIT_LENGTH) * valueScale;

    if (valueStringWidth > windowPtr->getWidth())
    {
        windowPtr->setWidth(valueStringWidth + paddingScaled);
    }

    // Place the window inside of the parent window
    WindowAlignment alignment;
    if (this->flagIsSet(ValueEditorFlag::WINDOW_POSITION_TOP))
    {
        alignment = WindowAlignment::TOP_CENTER;
    }
    else
    {
        alignment = WindowAlignment::MIDDLE_CENTER;
    }

    windowPtr->placeInWindow(parentWindow, alignment, scale);
}

bool ValueEditor::getValueFromString(ValueType *valuePtr) const
{
    // If the string starts with + or -, then skip them
    const char *string = editorValue;
    bool valueIsPositive = true;

    if (string[0] == '+')
    {
        string++;
    }
    else if (string[0] == '-')
    {
        valueIsPositive = false;
        string++;
    }

    // If the value is a float/double and it is not being handled as hex, then each one needs a specific type of format string
    const VariableType type = this->type;
    const char *format;

    switch (type)
    {
        default:
        {
            format = this->format;
            break;
        }
        case VariableType::f32:
        {
            /*
            if (!this->flagIsSet(ValueEditorFlag::HANDLE_AS_HEX))
            {
            */
            format = "%f";
            /*
            }
            */
            break;
        }
        case VariableType::f64:
        {
            format = "%lf";
            break;
        }
    }

    // Make sure the value was successfully retrieved
    if (sscanf(string, format, valuePtr) != 1)
    {
        return false;
    }

    // If the value was originally negative, then make it negative
    if (!valueIsPositive)
    {
        switch (type)
        {
            case VariableType::s32:
            {
                const int32_t value = valuePtr->s32;
                valuePtr->s32 = -value;
                break;
            }
            case VariableType::s64:
            {
                const int64_t value = valuePtr->s64;
                valuePtr->s64 = -value;
                break;
            }
            case VariableType::f32:
            {
                const float value = valuePtr->f32;
                valuePtr->f32 = -value;
                break;
            }
            case VariableType::f64:
            {
                const double value = valuePtr->f64;
                valuePtr->f64 = -value;
                break;
            }
            default:
            {
                // Should never be changed
                return false;
            }
        }
    }

    return true;
}

bool ValueEditor::handleCheckMinMax(const ValueType *valuePtr, bool valueIsPositive, uint32_t specialCase)
{
    // Handle special cases
    switch (specialCase)
    {
        case MinMaxSpecialCases::SPECIAL_CASE_SET_TO_MIN:
        {
            this->setValueToMin();
            return true;
        }
        case MinMaxSpecialCases::SPECIAL_CASE_SET_TO_MAX:
        {
            this->setValueToMax();
            return true;
        }
        default:
        {
            break;
        }
    }

    // Make sure the value does not exceed the min or max
    switch (this->type)
    {
        case VariableType::s32:
        {
            int32_t currentValue = valuePtr->s32;

            // If the value was originally negative, then convert it to negative
            if (!valueIsPositive)
            {
                currentValue = -currentValue;
            }

            const int32_t minValue = this->minValue.s32;
            const int32_t maxValue = this->maxValue.s32;

            if (currentValue < minValue)
            {
                // Loop around to the max
                this->setValueToMax();
                return true;
            }
            else if (currentValue > maxValue)
            {
                // Loop around to the min
                this->setValueToMin();
                return true;
            }
            break;
        }
        case VariableType::s64:
        {
            int64_t currentValue = valuePtr->s64;

            // If the value was originally negative, then convert it to negative
            if (!valueIsPositive)
            {
                currentValue = -currentValue;
            }

            const int64_t minValue = this->minValue.s64;
            const int64_t maxValue = this->maxValue.s64;

            if (currentValue < minValue)
            {
                // Loop around to the max
                this->setValueToMax();
                return true;
            }
            else if (currentValue > maxValue)
            {
                // Loop around to the min
                this->setValueToMin();
                return true;
            }
            break;
        }
        case VariableType::u32:
        {
            const uint32_t currentValue = valuePtr->u32;
            const uint32_t minValue = this->minValue.u32;
            const uint32_t maxValue = this->maxValue.u32;

            if (currentValue < minValue)
            {
                // Loop around to the max
                this->setValueToMax();
                return true;
            }
            else if (currentValue > maxValue)
            {
                // Loop around to the min
                this->setValueToMin();
                return true;
            }
            break;
        }
        case VariableType::u64:
        {
            const uint32_t currentValue = valuePtr->u64;
            const uint32_t minValue = this->minValue.u64;
            const uint32_t maxValue = this->maxValue.u64;

            if (currentValue < minValue)
            {
                // Loop around to the max
                this->setValueToMax();
                return true;
            }
            else if (currentValue > maxValue)
            {
                // Loop around to the min
                this->setValueToMin();
                return true;
            }
            break;
        }
        case VariableType::f32:
        {
            float currentValue = valuePtr->f32;

            // If the value was originally negative, then convert it to negative
            if (!valueIsPositive)
            {
                currentValue = -currentValue;
            }

            const float minValue = this->minValue.f32;
            const float maxValue = this->maxValue.f32;

            if (currentValue < minValue)
            {
                // Loop around to the max
                this->setValueToMax();
                return true;
            }
            else if (currentValue > maxValue)
            {
                // Loop around to the min
                this->setValueToMin();
                return true;
            }
            break;
        }
        case VariableType::f64:
        {
            double currentValue = valuePtr->f64;

            // If the value was originally negative, then convert it to negative
            if (!valueIsPositive)
            {
                currentValue = -currentValue;
            }

            const double minValue = this->minValue.f64;
            const double maxValue = this->maxValue.f64;

            if (currentValue < minValue)
            {
                // Loop around to the max
                this->setValueToMax();
                return true;
            }
            else if (currentValue > maxValue)
            {
                // Loop around to the min
                this->setValueToMin();
                return true;
            }
            break;
        }
        default:
        {
            break;
        }
    }

    return false;
}

/**
 * Used for the `Battles Stats` menu, in which the values for items that do not normally appear when held by NPCs in battles
 * will be skipped.
 *
 * @param value The current item value.
 * @param increment Whether the value was incremented or decremented. If `true`, then the value was incremented.
 *
 * @returns The updated `value` parameter value.
 */
uint32_t cheatsChangeDroppedItemSkipValues(uint32_t value, bool increment)
{
    // Skip everything after Fresh Juice and before Power Jump
    constexpr uint32_t freshJuiceValue = static_cast<uint32_t>(ItemId::ITEM_FRESH_JUICE);
    constexpr uint32_t powerJumpValue = static_cast<uint32_t>(ItemId::ITEM_POWER_JUMP);

    if ((value > freshJuiceValue) && (value < powerJumpValue))
    {
        if (increment)
        {
            value = powerJumpValue;
        }
        else
        {
            value = freshJuiceValue;
        }
    }

    return value;
}

/**
 * Used for the `Force NPC Item Drop` cheat, in which the values for damaging items that can be thrown from the audience (such
 * as rocks) will be skipped.
 *
 * @param value The current item value.
 * @param increment Whether the value was incremented or decremented. If `true`, then the value was incremented.
 *
 * @returns The updated `value` parameter value.
 */
uint32_t battlesChangeHeldItemSkipValues(uint32_t value, bool increment)
{
    // Skip Trade Off
    if (value == static_cast<uint32_t>(ItemId::ITEM_TRADE_OFF))
    {
        if (increment)
        {
            value++;
        }
        else
        {
            value--;
        }

        return value;
    }

    // Skip everything after Cake and before Power Jump
    constexpr uint32_t cakeValue = static_cast<uint32_t>(ItemId::ITEM_CAKE);
    constexpr uint32_t powerJumpValue = static_cast<uint32_t>(ItemId::ITEM_POWER_JUMP);

    if ((value > cakeValue) && (value < powerJumpValue))
    {
        if (increment)
        {
            value = powerJumpValue;
        }
        else
        {
            value = cakeValue;
        }

        return value;
    }

    return value;
}

void ValueEditor::adjustValue(bool increment)
{
    // Make sure the current index is valid
    char *editorValue = this->editorValue;
    uint32_t currentIndex = this->currentIndex;

    if (currentIndex >= strlen(editorValue))
    {
        // Failsafe: Reset the current index to 0
        this->currentIndex = 0;
        return;
    }

    // Convert the string to its proper value
    // Get the current value
    ValueType value;
    if (!this->getValueFromString(&value))
    {
        return;
    }

    // Handle special cases first for the current digit
    char *currentDigit = &editorValue[currentIndex];
    switch (currentDigit[0])
    {
        case '0':
        {
            // If the current index is 0 and the next character is x, then this is for the 0x for indicating a hex value, so do
            // nothing
            if ((currentIndex == 0) && (editorValue[1] == 'x'))
            {
                return;
            }
            break;
        }
        case '+':
        {
            // Can only be changed to -
            currentDigit[0] = '-';

            // Make sure the value does not exceed the min or max
            this->handleCheckMinMax(&value, false, MinMaxSpecialCases::SPECIAL_CASE_NONE);
            return;
        }
        case '-':
        {
            // Can only be changed to +
            currentDigit[0] = '+';

            // Make sure the value does not exceed the min or max
            this->handleCheckMinMax(&value, true, MinMaxSpecialCases::SPECIAL_CASE_NONE);
            return;
        }
        case '.':
        case 'x':
        {
            // Should never be changed
            return;
        }
        default:
        {
            break;
        }
    }

    // Adjust the value
    const bool handleAsHex = this->flagIsSet(ValueEditorFlag::HANDLE_AS_HEX);
    const bool valueIsSigned = this->flagIsSet(ValueEditorFlag::VALUE_IS_SIGNED);
    const uint32_t totalDigits = this->maxDigits;

    char *editorValuePtr = editorValue;
    uint32_t editorValueSize = sizeof(this->editorValue);

    switch (this->type)
    {
        case VariableType::s32:
        {
            // Adjust the current digit
            const int32_t currentValue = value.s32;
            int32_t newValue = handleAdjustValue(currentValue, currentIndex, totalDigits, handleAsHex, increment);

            // Make sure the value does not exceed the min or max
            // The value is already converted to negative, so just pass in true
            value.s32 = newValue;
            if (this->handleCheckMinMax(&value, true, MinMaxSpecialCases::SPECIAL_CASE_NONE))
            {
                return;
            }

            // If the value is now positive, then make sure valueIsPositive is set to true
            bool valueIsPositive;
            if (newValue >= 0)
            {
                valueIsPositive = true;
            }
            else // The value is negative, so convert it to positive and set valueIsPositive to false
            {
                newValue = -newValue;
                valueIsPositive = false;
            }

            // Put + or - in front of the value
            if (valueIsPositive)
            {
                editorValuePtr[0] = '+';
            }
            else
            {
                editorValuePtr[0] = '-';
            }

            editorValuePtr++;
            editorValueSize--;

            // Set the new value
            snprintf(editorValuePtr, editorValueSize, format, newValue);
            break;
        }
        case VariableType::s64:
        {
            // Adjust the current digit
            const int64_t currentValue = value.s64;
            int64_t newValue = handleAdjustValue(currentValue, currentIndex, totalDigits, handleAsHex, increment);

            // Make sure the value does not exceed the min or max
            // The value is already converted to negative, so just pass in true
            value.s64 = newValue;
            if (this->handleCheckMinMax(&value, true, MinMaxSpecialCases::SPECIAL_CASE_NONE))
            {
                return;
            }

            // If the value is now positive, then make sure valueIsPositive is set to true
            bool valueIsPositive;
            if (newValue >= 0)
            {
                valueIsPositive = true;
            }
            else // The value is negative, so convert it to positive and set valueIsPositive to false
            {
                newValue = -newValue;
                valueIsPositive = false;
            }

            // Put + or - in front of the value
            if (valueIsPositive)
            {
                editorValuePtr[0] = '+';
            }
            else
            {
                editorValuePtr[0] = '-';
            }

            editorValuePtr++;
            editorValueSize--;

            // Set the new value
            snprintf(editorValuePtr, editorValueSize, format, newValue);
            break;
        }
        case VariableType::u32:
        {
            // Adjust the current digit
            const uint32_t currentValue = value.u32;
            uint32_t newValue = handleAdjustValue(currentValue, currentIndex, totalDigits, handleAsHex, increment);

            uint32_t specialCase = MinMaxSpecialCases::SPECIAL_CASE_NONE;

            // If the new value was incremented and is now lower than the original value, then the new value would be negative
            // if it was being handled as a signed value, so set it to the min
            if (increment && (newValue < currentValue))
            {
                specialCase = MinMaxSpecialCases::SPECIAL_CASE_SET_TO_MIN;
            }

            // If the new value was decremented and is now higher than the original value, then the new value would be negative
            // if it was being handled as a signed value, so set it to the max
            else if (!increment && (newValue > currentValue))
            {
                specialCase = MinMaxSpecialCases::SPECIAL_CASE_SET_TO_MAX;
            }

            // Make sure the value does not exceed the min or max
            // The value can only be positive, so just pass in true
            value.u32 = newValue;
            if (this->handleCheckMinMax(&value, true, specialCase))
            {
                return;
            }

            // If changing the held item for battle actors, then check for values to skip
            if (this->flagIsSet(ValueEditorFlag::BATTLES_CHANGE_HELD_ITEM))
            {
                newValue = battlesChangeHeldItemSkipValues(newValue, increment);
            }

            // If changing the dropped item from enemies, then check for values to skip
            else if (this->flagIsSet(ValueEditorFlag::CHEATS_CHANGE_DROPPED_ITEM))
            {
                newValue = cheatsChangeDroppedItemSkipValues(newValue, increment);
            }

            // Set the new value
            snprintf(editorValuePtr, editorValueSize, format, newValue);
            break;
        }
        case VariableType::u64:
        {
            // Adjust the current digit
            const uint64_t currentValue = value.u64;
            uint64_t newValue = handleAdjustValue(currentValue, currentIndex, totalDigits, handleAsHex, increment);

            uint32_t specialCase = MinMaxSpecialCases::SPECIAL_CASE_NONE;

            // If the new value was incremented and is now lower than the original value, then the new value would be negative
            // if it was being handled as a signed value, so set it to the min
            if (increment && (newValue < currentValue))
            {
                specialCase = MinMaxSpecialCases::SPECIAL_CASE_SET_TO_MIN;
            }

            // If the new value was decremented and is now higher than the original value, then the new value would be negative
            // if it was being handled as a signed value, so set it to the max
            else if (!increment && (newValue > currentValue))
            {
                specialCase = MinMaxSpecialCases::SPECIAL_CASE_SET_TO_MAX;
            }

            // Make sure the value does not exceed the min or max
            // The value can only be positive, so just pass in true
            value.u64 = newValue;
            if (this->handleCheckMinMax(&value, true, specialCase))
            {
                return;
            }

            // Set the new value
            snprintf(editorValuePtr, editorValueSize, format, newValue);
            break;
        }
        case VariableType::f32:
        {
            // Make sure the current value is one that can be edited easily
            float currentValue = value.f32;
            if (!floatCanBeWorkedWith(currentValue))
            {
                currentValue = this->minValue.f32;
            }

            float newValue = handleAdjustValue(currentValue, currentIndex, totalDigits, valueIsSigned, increment);

            // Make sure the new value is one that can be edited easily
            if (!floatCanBeWorkedWith(newValue))
            {
                newValue = this->minValue.f32;
            }

            // Make sure the value does not exceed the min or max
            // The value is already converted to negative, so just pass in true
            value.f32 = newValue;
            if (this->handleCheckMinMax(&value, true, MinMaxSpecialCases::SPECIAL_CASE_NONE))
            {
                return;
            }

            // If the value is now positive, then make sure valueIsPositive is set to true
            bool valueIsPositive;
            if (!std::signbit(newValue)) // Check if positive; works for checking against +0.0 and -0.0
            {
                valueIsPositive = true;
            }
            else // The value is negative, so convert it to positive and set valueIsPositive to false
            {
                newValue = -newValue;
                valueIsPositive = false;
            }

            // If the value is signed, then put + or - in front of it
            if (valueIsSigned)
            {
                if (valueIsPositive)
                {
                    editorValuePtr[0] = '+';
                }
                else
                {
                    editorValuePtr[0] = '-';
                }

                editorValuePtr++;
                editorValueSize--;
            }

            /*
                if (handleAsHex)
                {
                    value->f32 = newValue;
                    snprintf(editorValuePtr, editorValueSize, format, value->u32);
                }
                else
                {
            */
            snprintf(editorValuePtr, editorValueSize, format, newValue);
            /*
                }
            */

            /*
            // Adjust the current digit
            handleAdjustValueDouble(editorValue, currentIndex, handleAsHex, increment);

            // Get the current value
            // Make sure the value was successfully retrieved
            if (sscanf(editorValuePtr, format, &value) != 1)
            {
                break;
            }

            // Make sure the value does not exceed the min or max
            // The value is already converted to negative, so just pass in true
            if (this->handleCheckMinMax(&value, true))
            {
                return;
            }

            float newValue = value.f32;

            // If the value is now positive, then make sure valueIsPositive is set to true
            if (!std::signbit(newValue)) // Check if positive; works for checking against +0.0 and -0.0
            {
                valueIsPositive = true;
            }
            else // The value is negative, so convert it to positive and set valueIsPositive to false
            {
                newValue = -newValue;
                valueIsPositive = false;
            }

            // If the value is signed, then put + or - in front of it
            if (valueIsSigned)
            {
                if (valueIsPositive)
                {
                    editorValuePtr[0] = '+';
                }
                else
                {
                    editorValuePtr[0] = '-';
                }

                editorValuePtr++;
                editorValueSize--;
            }

            if (handleAsHex)
            {
                value.f32 = newValue;
                snprintf(editorValuePtr, editorValueSize, format, value.u32);
            }
            else
            {
                snprintf(editorValuePtr, editorValueSize, format, newValue);
            }
            */
            break;
        }
        case VariableType::f64:
        {
            // Make sure the current value is one that can be edited easily
            double currentValue = value.f64;
            if (!doubleCanBeWorkedWith(currentValue))
            {
                currentValue = this->minValue.f64;
            }

            double newValue = handleAdjustValue(currentValue, currentIndex, totalDigits, valueIsSigned, increment);

            // Make sure the new value is one that can be edited easily
            if (!doubleCanBeWorkedWith(newValue))
            {
                newValue = this->minValue.f64;
            }

            // Make sure the value does not exceed the min or max
            // The value is already converted to negative, so just pass in true
            value.f64 = newValue;
            if (this->handleCheckMinMax(&value, true, MinMaxSpecialCases::SPECIAL_CASE_NONE))
            {
                return;
            }

            // If the value is now positive, then make sure valueIsPositive is set to true
            bool valueIsPositive;
            if (!std::signbit(newValue)) // Check if positive; works for checking against +0.0 and -0.0
            {
                valueIsPositive = true;
            }
            else // The value is negative, so convert it to positive and set valueIsPositive to false
            {
                newValue = -newValue;
                valueIsPositive = false;
            }

            // If the value is signed, then put + or - in front of it
            if (valueIsSigned)
            {
                if (valueIsPositive)
                {
                    editorValuePtr[0] = '+';
                }
                else
                {
                    editorValuePtr[0] = '-';
                }

                editorValuePtr++;
                editorValueSize--;
            }

            snprintf(editorValuePtr, editorValueSize, format, newValue);
            break;
        }
        default:
        {
            return;
        }
    }
}

void ValueEditor::setValueToMin()
{
    const char *format = this->format;
    char *editorValue = this->editorValue;
    uint32_t editorValueSize = sizeof(this->editorValue);
    const bool valueIsSigned = this->flagIsSet(ValueEditorFlag::VALUE_IS_SIGNED);

    switch (this->type)
    {
        case VariableType::s32:
        {
            int32_t minValue = this->minValue.s32;

            // Put + or - in front of the value
            if (minValue >= 0)
            {
                editorValue[0] = '+';
            }
            else
            {
                editorValue[0] = '-';
                minValue = -minValue;
            }

            editorValue++;
            editorValueSize--;

            snprintf(editorValue, editorValueSize, format, minValue);
            break;
        }
        case VariableType::s64:
        {
            int64_t minValue = this->minValue.s64;

            // Put + or - in front of the value
            if (minValue >= 0)
            {
                editorValue[0] = '+';
            }
            else
            {
                editorValue[0] = '-';
                minValue = -minValue;
            }

            editorValue++;
            editorValueSize--;

            snprintf(editorValue, editorValueSize, format, minValue);
            break;
        }
        case VariableType::u32:
        {
            const uint32_t minValue = this->minValue.u32;
            snprintf(editorValue, sizeof(this->editorValue), format, minValue);
            break;
        }
        case VariableType::u64:
        {
            const uint64_t minValue = this->minValue.u64;
            snprintf(editorValue, sizeof(this->editorValue), format, minValue);
            break;
        }
        case VariableType::f32:
        {
            float minValue = this->minValue.f32;

            // If the value is signed, then put + or - in front of the value
            if (valueIsSigned)
            {
                if (!std::signbit(minValue)) // Check if positive; works for checking against +0.0 and -0.0
                {
                    editorValue[0] = '+';
                }
                else
                {
                    editorValue[0] = '-';
                    minValue = -minValue;
                }

                editorValue++;
                editorValueSize--;
            }

            snprintf(editorValue, editorValueSize, format, minValue);
            break;
        }
        case VariableType::f64:
        {
            double minValue = this->minValue.f64;

            // If the value is signed, then put + or - in front of the value
            if (valueIsSigned)
            {
                if (!std::signbit(minValue)) // Check if positive; works for checking against +0.0 and -0.0
                {
                    editorValue[0] = '+';
                }
                else
                {
                    editorValue[0] = '-';
                    minValue = -minValue;
                }

                editorValue++;
                editorValueSize--;
            }

            snprintf(editorValue, editorValueSize, format, minValue);
            break;
        }
        default:
        {
            break;
        }
    }
}

void ValueEditor::setValueToMax()
{
    const char *format = this->format;
    char *editorValue = this->editorValue;
    uint32_t editorValueSize = sizeof(this->editorValue);
    const bool valueIsSigned = this->flagIsSet(ValueEditorFlag::VALUE_IS_SIGNED);

    switch (this->type)
    {
        case VariableType::s32:
        {
            int32_t maxValue = this->maxValue.s32;

            // Put + or - in front of the value
            if (maxValue >= 0)
            {
                editorValue[0] = '+';
            }
            else
            {
                editorValue[0] = '-';
                maxValue = -maxValue;
            }

            editorValue++;
            editorValueSize--;

            snprintf(editorValue, editorValueSize, format, maxValue);
            break;
        }
        case VariableType::s64:
        {
            int64_t maxValue = this->maxValue.s64;

            // Put + or - in front of the value
            if (maxValue >= 0)
            {
                editorValue[0] = '+';
            }
            else
            {
                editorValue[0] = '-';
                maxValue = -maxValue;
            }

            editorValue++;
            editorValueSize--;

            snprintf(editorValue, editorValueSize, format, maxValue);
            break;
        }
        case VariableType::u32:
        {
            const uint32_t maxValue = this->maxValue.u32;
            snprintf(editorValue, sizeof(this->editorValue), format, maxValue);
            break;
        }
        case VariableType::u64:
        {
            const uint64_t maxValue = this->maxValue.u64;
            snprintf(editorValue, sizeof(this->editorValue), format, maxValue);
            break;
        }
        case VariableType::f32:
        {
            float maxValue = this->maxValue.f32;

            // If the value is signed, then put + or - in front of the value
            if (valueIsSigned)
            {
                if (!std::signbit(maxValue)) // Check if positive; works for checking against +0.0 and -0.0
                {
                    editorValue[0] = '+';
                }
                else
                {
                    editorValue[0] = '-';
                    maxValue = -maxValue;
                }

                editorValue++;
                editorValueSize--;
            }

            snprintf(editorValue, editorValueSize, format, maxValue);
            break;
        }
        case VariableType::f64:
        {
            double maxValue = this->maxValue.f64;

            // If the value is signed, then put + or - in front of the value
            if (valueIsSigned)
            {
                if (!std::signbit(maxValue)) // Check if positive; works for checking against +0.0 and -0.0
                {
                    editorValue[0] = '+';
                }
                else
                {
                    editorValue[0] = '-';
                    maxValue = -maxValue;
                }

                editorValue++;
                editorValueSize--;
            }

            snprintf(editorValue, editorValueSize, format, maxValue);
            break;
        }
        default:
        {
            break;
        }
    }
}

void ValueEditor::controlsMoveLeftOnce(uint32_t maxDigit)
{
    int32_t currentIndex = static_cast<int32_t>(this->currentIndex);
    if (currentIndex == 0)
    {
        // Loop to the last option
        this->setCurrentIndex(maxDigit);
        return;
    }

    // Move left once
    currentIndex--;

    // Handle special actions for certain characters
    const char *editorValue = this->editorValue;
    char currentDigit = editorValue[currentIndex];

    switch (currentDigit)
    {
        case '0':
        {
            // If the next character is x, then loop to the last option
            if (editorValue[currentIndex + 1] == 'x')
            {
                currentIndex = static_cast<int32_t>(maxDigit);
            }
            break;
        }
        case '.':
        {
            // Move left again
            currentIndex--;
            break;
        }
        case 'x':
        {
            // Move left twice
            currentIndex -= 2;

            // If the current index is now invalid, then loop to the last option
            if (currentIndex < 0)
            {
                currentIndex = static_cast<int32_t>(maxDigit);
            }
            break;
        }
        default:
        {
            break;
        }
    }

    this->setCurrentIndex(static_cast<uint32_t>(currentIndex));
}

void ValueEditor::controlsMoveRightOnce(uint32_t maxDigit)
{
    uint32_t currentIndex = this->currentIndex;
    const char *editorValue = this->editorValue;

    if (currentIndex == maxDigit)
    {
        // Loop to the first option
        currentIndex = 0;

        // If the first character is 0 and the second character is x, then this is for the 0x for indicating a hex value, so
        // move right twice
        // x will always be the second character if 0x is present in this scenario
        if (editorValue[1] == 'x')
        {
            currentIndex += 2;
        }

        this->setCurrentIndex(currentIndex);
        return;
    }

    // Move right once
    currentIndex++;

    // Handle special actions for certain characters
    char currentDigit = editorValue[currentIndex];

    switch (currentDigit)
    {
        case '0':
        {
            // If the next character is x, then this is for the 0x for indicating a hex value, so move right twice
            if (editorValue[currentIndex + 1] == 'x')
            {
                currentIndex += 2;
            }
            break;
        }
        case '.':
        case 'x':
        {
            // Move right again
            currentIndex++;
            break;
        }
        default:
        {
            break;
        }
    }

    this->setCurrentIndex(currentIndex);
}

void ValueEditor::controls(MenuButtonInput button)
{
    // The function for checking for auto-incrementing needs to run every frame to be handled correctly
    const bool autoIncrement = handleMenuAutoIncrement(&this->autoIncrement);

    const char *editorValue = this->editorValue;
    const uint32_t maxDigit = strlen(editorValue) - 1;

    // Handle held button input if auto-incrementing should be done
    if (autoIncrement)
    {
        switch (getMenuButtonInput(false))
        {
            case MenuButtonInput::DPAD_LEFT:
            {
                this->controlsMoveLeftOnce(maxDigit);
                break;
            }
            case MenuButtonInput::DPAD_RIGHT:
            {
                this->controlsMoveRightOnce(maxDigit);
                break;
            }
            case MenuButtonInput::DPAD_DOWN:
            {
                this->adjustValue(false);
                break;
            }
            case MenuButtonInput::DPAD_UP:
            {
                this->adjustValue(true);
            }
            default:
            {
                break;
            }
        }
    }

    // Handle the button input pressed this frame
    switch (button)
    {
        case MenuButtonInput::DPAD_LEFT:
        {
            this->controlsMoveLeftOnce(maxDigit);
            break;
        }
        case MenuButtonInput::DPAD_RIGHT:
        {
            this->controlsMoveRightOnce(maxDigit);
            break;
        }
        case MenuButtonInput::DPAD_DOWN:
        {
            this->adjustValue(false);
            break;
        }
        case MenuButtonInput::DPAD_UP:
        {
            this->adjustValue(true);
            break;
        }
        case MenuButtonInput::Z:
        {
            // Set the value to the min if the flag for it is set
            if (this->flagIsSet(ValueEditorFlag::DRAW_BUTTON_Z_SET_MIN))
            {
                this->setValueToMin();
            }
            break;
        }
        case MenuButtonInput::A:
        {
            const ValueEditorSetValueFunc func = this->setValueFunc;
            if (!func)
            {
                break;
            }

            // Get the new value
            ValueType value;
            if (this->getValueFromString(&value))
            {
                return func(&value);
            }

            break;
        }
        case MenuButtonInput::B:
        {
            // Cancel editing the value
            const ValueEditorCancelFunc func = this->cancelFunc;
            if (func)
            {
                return func();
            }
            break;
        }
        case MenuButtonInput::Y:
        {
            // Set the value to the max if the flag for it is set
            if (this->flagIsSet(ValueEditorFlag::DRAW_BUTTON_Y_SET_MAX))
            {
                this->setValueToMax();
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

void ValueEditor::draw()
{
    // If the flag for X is set and X is held, then do not draw anything
    if (this->flagIsSet(ValueEditorFlag::DRAW_BUTTON_X_HIDE) && checkButtonComboEveryFrame(PadInput::PAD_X))
    {
        return;
    }

    // Draw the window
    const Window *windowPtr = &this->window;
    windowPtr->draw();

    // Initialize text drawing
    drawTextInit(false);

    // Draw the help text
    const char *helpText = this->helpText;
    constexpr float scale = MENU_SCALE;

    float tempPosX;
    float tempPosY;
    windowPtr->getTextPosXY(helpText, WindowAlignment::TOP_CENTER, scale, &tempPosX, &tempPosY);

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    float posX = tempPosX;
    float posY = tempPosY;
    drawText(helpText, posX, posY, scale, getColorWhite(0xFF));

    // Draw various things based on flags that are set
    // Set posY to be two lines under the help text
    const float height = getTextHeight(helpText, scale);
    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    posY -= (height + lineDecrement);

    ValueType value;
    if (this->getValueFromString(&value))
    {
        if (this->flagIsSet(ValueEditorFlag::DRAW_ITEM_ICON_AND_TEXT))
        {
            // Several items need to have their icon scales adjusted
            const ItemId item = static_cast<ItemId>(value.s32);
            const float iconScale = adjustItemIconScale(item, scale);

            // Draw the current item with its icon and text
            posX = windowPtr->getIconPosX(WindowAlignment::BOTTOM_LEFT, scale);
            posY -= (lineDecrement * 1.45f);
            drawItemIconWithText(posX, posY, iconScale, scale, 0.f, item, getColorWhite(0xFF));
        }
        else if (this->flagIsSet(ValueEditorFlag::DRAW_STAGE_AND_EVENT))
        {
            // Draw the stage and event names for the current sequence position
            drawStageAndEvent(value.u32, posX, posY);
        }
        else if (this->flagIsSet(ValueEditorFlag::DRAW_MAP_STRING))
        {
            // Draw the current map
            char buf[32];
            snprintf(buf, sizeof(buf), "Map: %s", getMapFromIndex(value.u32));
            drawText(buf, posX, posY, scale, getColorWhite(0xFF));
        }
        else if (this->flagIsSet(ValueEditorFlag::DRAW_WARP_BY_EVENT_DETAILS))
        {
            // Draw the details for the current event
            drawEventDetails(value.u32, posX, posY);
        }
    }

    // Get the X and Y coordinates for the value
    const int32_t stringLength = static_cast<int32_t>(strlen(editorValue));
    const float stringLengthFloat = intToFloat(stringLength);
    constexpr float valueScale = scale + 0.3f;

    // getTextPosY only uses one scale, while two are required here, so have to calculate posY manually
    const float valueHeight = getTextHeight(editorValue, valueScale);

    posY = windowPtr->getPosY() - windowPtr->getHeight() + valueHeight + (windowPtr->getPadding() * scale) +
           LINE_HEIGHT_ADJUSTMENT_4(scale);

    const float valueStringWidth = (stringLengthFloat * EDITOR_DIGIT_LENGTH) * valueScale;
    posX = windowPtr->getPosX() + ((windowPtr->getWidth() - valueStringWidth) / 2.f);

    // Draw the value
    const uint32_t currentIndex = this->currentIndex;
    constexpr float posXIncrement = EDITOR_DIGIT_LENGTH * valueScale;
    uint32_t color;

    // Place the current character into a temporary buffer
    char tempBuf[2];
    tempBuf[1] = '\0';

    for (int32_t i = 0; i < stringLength; i++)
    {
        const char currentCharacter = editorValue[i];
        tempBuf[0] = currentCharacter;

        color = getCurrentOptionColor(static_cast<uint32_t>(i) == currentIndex, 0xFF);
        drawText(tempBuf, posX, posY, valueScale, posXIncrement, color, TextAlignment::CENTER);
        posX += posXIncrement;
    }
}

/**
 * Gets the amount of digits that are used to represent a `uint32_t` value in the value editor.
 *
 * @param value The value to get the amount of digits for.
 * @param handleAsHex Whether the value is being handled as a hexadecimal value or not. If `true`, then the value is being
 * handled as a hexadecimal value.
 *
 * @returns The amount of digits that are used to represent the value of the `value` parameter.
 *
 * @relatesalso ValueEditor
 */
static uint32_t getMaxDigits(uint32_t value, bool handleAsHex)
{
    uint32_t divideAmount;
    if (handleAsHex)
    {
        divideAmount = 16;
    }
    else
    {
        divideAmount = 10;
    }

    uint32_t count = 0;
    while (value)
    {
        value /= divideAmount;
        count++;
    }

    return count;
}

/**
 * Gets the amount of digits that are used to represent an `int32_t` value in the value editor.
 *
 * @param value The value to get the amount of digits for.
 * @param handleAsHex Whether the value is being handled as a hexadecimal value or not. If `true`, then the value is being
 * handled as a hexadecimal value.
 *
 * @returns The amount of digits that are used to represent the value of the `value` parameter.
 *
 * @relatesalso ValueEditor
 *
 * @overload
 */
static uint32_t getMaxDigits(int32_t value, bool handleAsHex)
{
    int32_t divideAmount;
    if (handleAsHex)
    {
        divideAmount = 16;
    }
    else
    {
        divideAmount = 10;
    }

    uint32_t count = 0;
    while (value)
    {
        value /= divideAmount;
        count++;
    }

    return count;
}

/**
 * Gets the amount of digits that are used to represent a `uint64_t` value in the value editor.
 *
 * @param value The value to get the amount of digits for.
 * @param handleAsHex Whether the value is being handled as a hexadecimal value or not. If `true`, then the value is being
 * handled as a hexadecimal value.
 *
 * @returns The amount of digits that are used to represent the value of the `value` parameter.
 *
 * @relatesalso ValueEditor
 *
 * @overload
 */
static uint32_t getMaxDigits(uint64_t value, bool handleAsHex)
{
    uint32_t divideAmount;
    if (handleAsHex)
    {
        divideAmount = 16;
    }
    else
    {
        divideAmount = 10;
    }

    uint32_t count = 0;
    while (value)
    {
        value /= divideAmount;
        count++;
    }

    return count;
}

/**
 * Gets the amount of digits that are used to represent an `int64_t` value in the value editor.
 *
 * @param value The value to get the amount of digits for.
 * @param handleAsHex Whether the value is being handled as a hexadecimal value or not. If `true`, then the value is being
 * handled as a hexadecimal value.
 *
 * @returns The amount of digits that are used to represent the value of the `value` parameter.
 *
 * @relatesalso ValueEditor
 *
 * @overload
 */
static uint32_t getMaxDigits(int64_t value, bool handleAsHex)
{
    int32_t divideAmount;
    if (handleAsHex)
    {
        divideAmount = 16;
    }
    else
    {
        divideAmount = 10;
    }

    uint32_t count = 0;
    while (value)
    {
        value /= divideAmount;
        count++;
    }

    return count;
}

/**
 * Gets the amount of digits that are used to represent a `float` value in the value editor.
 *
 * @param value The value to get the amount of digits for.
 * @param handleAsHex Whether the value is being handled as a hexadecimal value or not. If `true`, then the value is being
 * handled as a hexadecimal value.
 *
 * @returns The amount of digits that are used to represent the value of the `value` parameter.
 *
 * @relatesalso ValueEditor
 *
 * @overload
 */
/*
static uint32_t getMaxDigits(float value, bool handleAsHex)
{
    // For simplicity, convert the value to a string, and then count the characters in it
    char buf[64];
    int32_t len;

    if (handleAsHex)
    {
        void *valuePtr = &value;
        const char *format = "%" PRIX32;
        len = snprintf(buf, sizeof(buf), format, *reinterpret_cast<uint32_t *>(valuePtr));
    }
    else
    {
        // If the value is negative, then convert it to positive, as the - should not be included in the size
        if (std::signbit(value)) // Check if negative; works for checking against +0.0 and -0.0
        {
            value = -value;
        }

        const char *format = "%." MAX_DOUBLE_DIGITS_STRING "f";
        len = snprintf(buf, sizeof(buf), format, value);
    }

    // Make sure a proper value was returned
    if (len > 0)
    {
        // If not handling as hex, subtract 1 to account for the decimal point
        if (!handleAsHex)
        {
            len--;
        }
    }
    else
    {
        return 0;
    }

    return static_cast<uint32_t>(len);
}
*/

/**
 * Gets the amount of digits that are used to represent a `float` value in the value editor.
 *
 * @param value The value to get the amount of digits for.
 *
 * @returns The amount of digits that are used to represent the value of the `value` parameter.
 *
 * @relatesalso ValueEditor
 *
 * @overload
 */
static uint32_t getMaxDigits(float value)
{
    // For simplicity, convert the value to a string, and then count the characters in it
    char buf[32];
    int32_t len;

    // If the value is negative, then convert it to positive, as the - should not be included in the size
    if (std::signbit(value)) // Check if negative; works for checking against +0.0 and -0.0
    {
        value = -value;
    }

    const char *format = "%." MAX_DOUBLE_DIGITS_STRING "f";
    len = snprintf(buf, sizeof(buf), format, value);

    // Make sure a proper value was returned
    if (len > 0)
    {
        // Subtract 1 to account for the decimal point
        len--;
    }
    else
    {
        return 0;
    }

    return static_cast<uint32_t>(len);
}

/**
 * Gets the amount of digits that are used to represent a `double` value in the value editor.
 *
 * @param value The value to get the amount of digits for.
 *
 * @returns The amount of digits that are used to represent the value of the `value` parameter.
 *
 * @relatesalso ValueEditor
 *
 * @overload
 */
static uint32_t getMaxDigits(double value)
{
    // For simplicity, convert the value to a string, and then count the characters in it
    char buf[32];
    int32_t len;

    // If the value is negative, then convert it to positive, as the - should not be included in the size
    if (std::signbit(value)) // Check if negative; works for checking against +0.0 and -0.0
    {
        value = -value;
    }

    const char *format = "%." MAX_DOUBLE_DIGITS_STRING "f";
    len = snprintf(buf, sizeof(buf), format, value);

    // Make sure a proper value was returned
    if (len > 0)
    {
        // Subtract 1 to account for the decimal point
        len--;
    }
    else
    {
        return 0;
    }

    return static_cast<uint32_t>(len);
}

/**
 * Handles adjusting the current value in the value editor, assuming the value is in `uint32_t` format.
 *
 * @param value The current value to be adjusted.
 * @param currentIndex The index for the digit of the current value being adjusted.
 * @param totalDigits The total amount of digits being used to represent the value.
 * @param handleAsHex Whether the value is being handled as a hexadecimal value or not. If `true`, then the value is being
 * handled as a hexadecimal value.
 * @param increment Whether the value should be incremented or decremented. If `true`, then the value should be incremented.
 *
 * @returns The updated `value` parameter value.
 *
 * @relatesalso ValueEditor
 */
static uint32_t handleAdjustValue(uint32_t value, uint32_t currentIndex, uint32_t totalDigits, bool handleAsHex, bool increment)
{
    // Get the amount to increment/decrement by
    int32_t base;
    if (handleAsHex)
    {
        base = 0x10;
    }
    else
    {
        base = 10;
    }

    int32_t adjustmentValue = 1;
    const uint32_t loopCount = totalDigits - 1;

    if (handleAsHex)
    {
        // Subtract 2 to account for 0x
        currentIndex -= 2;
    }

    for (uint32_t i = currentIndex; i < loopCount; i++)
    {
        adjustmentValue *= base;
    }

    // If decrementing, make adjustmentValue negative
    if (!increment)
    {
        adjustmentValue = -adjustmentValue;
    }

    return value += adjustmentValue;
}

/**
 * Handles adjusting the current value in the value editor, assuming the value is in `int32_t` format.
 *
 * @param value The current value to be adjusted.
 * @param currentIndex The index for the digit of the current value being adjusted.
 * @param totalDigits The total amount of digits being used to represent the value.
 * @param handleAsHex Whether the value is being handled as a hexadecimal value or not. If `true`, then the value is being
 * handled as a hexadecimal value.
 * @param increment Whether the value should be incremented or decremented. If `true`, then the value should be incremented.
 *
 * @returns The updated `value` parameter value.
 *
 * @relatesalso ValueEditor
 *
 * @overload
 */
static int32_t handleAdjustValue(int32_t value, uint32_t currentIndex, uint32_t totalDigits, bool handleAsHex, bool increment)
{
    // Get the amount to increment/decrement by
    int32_t base;
    if (handleAsHex)
    {
        base = 0x10;
    }
    else
    {
        base = 10;
    }

    int32_t adjustmentValue = 1;
    const uint32_t loopCount = totalDigits - 1;

    // Subtract 1 to account for + or -
    currentIndex--;

    if (handleAsHex)
    {
        // Subtract 2 to account for 0x
        currentIndex -= 2;
    }

    for (uint32_t i = currentIndex; i < loopCount; i++)
    {
        adjustmentValue *= base;
    }

    // If decrementing, make adjustmentValue negative
    if (!increment)
    {
        adjustmentValue = -adjustmentValue;
    }

    return value += adjustmentValue;
}

/**
 * Handles adjusting the current value in the value editor, assuming the value is in `uint64_t` format.
 *
 * @param value The current value to be adjusted.
 * @param currentIndex The index for the digit of the current value being adjusted.
 * @param totalDigits The total amount of digits being used to represent the value.
 * @param handleAsHex Whether the value is being handled as a hexadecimal value or not. If `true`, then the value is being
 * handled as a hexadecimal value.
 * @param increment Whether the value should be incremented or decremented. If `true`, then the value should be incremented.
 *
 * @returns The updated `value` parameter value.
 *
 * @relatesalso ValueEditor
 *
 * @overload
 */
static uint64_t handleAdjustValue(uint64_t value, uint32_t currentIndex, uint32_t totalDigits, bool handleAsHex, bool increment)
{
    // Get the amount to increment/decrement by
    int32_t base;
    if (handleAsHex)
    {
        base = 0x10;
    }
    else
    {
        base = 10;
    }

    int64_t adjustmentValue = 1;
    const uint32_t loopCount = totalDigits - 1;

    if (handleAsHex)
    {
        // Subtract 2 to account for 0x
        currentIndex -= 2;
    }

    for (uint32_t i = currentIndex; i < loopCount; i++)
    {
        adjustmentValue *= base;
    }

    // If decrementing, make adjustmentValue negative
    if (!increment)
    {
        adjustmentValue = -adjustmentValue;
    }

    return value += adjustmentValue;
}

/**
 * Handles adjusting the current value in the value editor, assuming the value is in `int64_t` format.
 *
 * @param value The current value to be adjusted.
 * @param currentIndex The index for the digit of the current value being adjusted.
 * @param totalDigits The total amount of digits being used to represent the value.
 * @param handleAsHex Whether the value is being handled as a hexadecimal value or not. If `true`, then the value is being
 * handled as a hexadecimal value.
 * @param increment Whether the value should be incremented or decremented. If `true`, then the value should be incremented.
 *
 * @returns The updated `value` parameter value.
 *
 * @relatesalso ValueEditor
 *
 * @overload
 */
static int64_t handleAdjustValue(int64_t value, uint32_t currentIndex, uint32_t totalDigits, bool handleAsHex, bool increment)
{
    // Get the amount to increment/decrement by
    int32_t base;
    if (handleAsHex)
    {
        base = 0x10;
    }
    else
    {
        base = 10;
    }

    int64_t adjustmentValue = 1;
    const uint32_t loopCount = totalDigits - 1;

    // Subtract 1 to account for + or -
    currentIndex--;

    if (handleAsHex)
    {
        // Subtract 2 to account for 0x
        currentIndex -= 2;
    }

    for (uint32_t i = currentIndex; i < loopCount; i++)
    {
        adjustmentValue *= base;
    }

    // If decrementing, make adjustmentValue negative
    if (!increment)
    {
        adjustmentValue = -adjustmentValue;
    }

    return value += adjustmentValue;
}

/**
 * Handles adjusting the current value in the value editor, assuming the value is in `float` format.
 *
 * @param value The current value to be adjusted.
 * @param currentIndex The index for the digit of the current value being adjusted.
 * @param totalDigits The total amount of digits being used to represent the value.
 * @param valueIsSigned Whether the value is considered signed or unsigned. If `true`, then the value is considered to be
 * signed.
 * @param handleAsHex Whether the value is being handled as a hexadecimal value or not. If `true`, then the value is being
 * handled as a hexadecimal value.
 * @param increment Whether the value should be incremented or decremented. If `true`, then the value should be incremented.
 *
 * @returns The updated `value` parameter value.
 *
 * @relatesalso ValueEditor
 *
 * @overload
 */
/*
static float handleAdjustValue(float value,
                               uint32_t currentIndex,
                               uint32_t totalDigits,
                               bool valueIsSigned,
                               bool handleAsHex,
                               bool increment)
{
    float base;
    if (handleAsHex)
    {
        base = 16.f;
    }
    else
    {
        base = 10.f;
    }

    float adjustmentValue = 0.000001f; // Based on MAX_DOUBLE_DIGITS
    const uint32_t loopCount = totalDigits - 1;

    if (valueIsSigned)
    {
        // Subtract 1 to account for + or -
        currentIndex--;
    }

    if (handleAsHex)
    {
        // Subtract 2 to account for 0x
        currentIndex -= 2;
    }
    else
    {
        // If the cursor is to the right of the decimal point, then subtract 1
        if (currentIndex >= (totalDigits - MAX_DOUBLE_DIGITS))
        {
            currentIndex--;
        }
    }

    for (uint32_t i = currentIndex; i < loopCount; i++)
    {
        adjustmentValue *= base;
    }

    // If decrementing, make adjustmentValue negative
    if (!increment)
    {
        adjustmentValue = -adjustmentValue;
    }

    return value += adjustmentValue;
}
*/

/**
 * Handles adjusting the current value in the value editor, assuming the value is in `float` format.
 *
 * @param value The current value to be adjusted.
 * @param currentIndex The index for the digit of the current value being adjusted.
 * @param totalDigits The total amount of digits being used to represent the value.
 * @param valueIsSigned Whether the value is considered signed or unsigned. If `true`, then the value is considered to be
 * signed.
 * @param increment Whether the value should be incremented or decremented. If `true`, then the value should be incremented.
 *
 * @returns The updated `value` parameter value.
 *
 * @relatesalso ValueEditor
 *
 * @overload
 */
static float handleAdjustValue(float value, uint32_t currentIndex, uint32_t totalDigits, bool valueIsSigned, bool increment)
{
    constexpr float base = 10.f;
    float adjustmentValue = 0.000001f; // Based on MAX_DOUBLE_DIGITS
    const uint32_t loopCount = totalDigits - 1;

    if (valueIsSigned)
    {
        // Subtract 1 to account for + or -
        currentIndex--;
    }

    // If the cursor is to the right of the decimal point, then subtract 1
    if (currentIndex >= (totalDigits - MAX_DOUBLE_DIGITS))
    {
        currentIndex--;
    }

    for (uint32_t i = currentIndex; i < loopCount; i++)
    {
        adjustmentValue *= base;
    }

    // If decrementing, make adjustmentValue negative
    if (!increment)
    {
        adjustmentValue = -adjustmentValue;
    }

    return value += adjustmentValue;
}

/**
 * Handles adjusting the current value in the value editor, assuming the value is in `double` format.
 *
 * @param value The current value to be adjusted.
 * @param currentIndex The index for the digit of the current value being adjusted.
 * @param totalDigits The total amount of digits being used to represent the value.
 * @param valueIsSigned Whether the value is considered signed or unsigned. If `true`, then the value is considered to be
 * signed.
 * @param increment Whether the value should be incremented or decremented. If `true`, then the value should be incremented.
 *
 * @returns The updated `value` parameter value.
 *
 * @relatesalso ValueEditor
 *
 * @overload
 */
static double handleAdjustValue(double value, uint32_t currentIndex, uint32_t totalDigits, bool valueIsSigned, bool increment)
{
    constexpr double base = 10.0;
    double adjustmentValue = 0.000001; // Based on MAX_DOUBLE_DIGITS
    const uint32_t loopCount = totalDigits - 1;

    if (valueIsSigned)
    {
        // Subtract 1 to account for + or -
        currentIndex--;
    }

    // If the cursor is to the right of the decimal point, then subtract 1
    if (currentIndex >= (totalDigits - MAX_DOUBLE_DIGITS))
    {
        currentIndex--;
    }

    for (uint32_t i = currentIndex; i < loopCount; i++)
    {
        adjustmentValue *= base;
    }

    // If decrementing, make adjustmentValue negative
    if (!increment)
    {
        adjustmentValue = -adjustmentValue;
    }

    return value += adjustmentValue;
}

/**
 * Handles adjusting the current value in the value editor, assuming the value is in `double` format.
 *
 * @param[out] valueString Pointer to the current value to be adjusted in string format.
 * @param[in] currentIndex The index for the digit of the current value being adjusted.
 * @param[in] handleAsHex Whether the value is being handled as a hexadecimal value or not. If `true`, then the value is being
 * handled as a hexadecimal value.
 * @param[in] increment Whether the value should be incremented or decremented. If `true`, then the value should be incremented.
 *
 * @relatesalso ValueEditor
 */
/*
static void handleAdjustValueDouble(char *valueString, uint32_t currentIndex, bool handleAsHex, bool increment)
{
    // Handle special cases first for the current digit
    char currentDigit = valueString[currentIndex];
    switch (currentDigit)
    {
        case '0':
        {
            // If the current index is 0 and the next character is x, then this is for the 0x for indicating a hex value, so do
            // nothing
            if ((currentIndex == 0) && (valueString[1] == 'x'))
            {
                return;
            }
            break;
        }
        case '+':
        case '-':
        case '.':
        case 'x':
        {
            // These wll be handled elsewhere, so do nothing
            return;
        }
        default:
        {
            break;
        }
    }

    // Adjust the current digit
    bool adjustedDigit = false;
    switch (currentDigit)
    {
        case '0':
        {
            if (!increment)
            {
                if (handleAsHex)
                {
                    currentDigit = 'F';
                }
                else
                {
                    currentDigit = '9';
                }

                adjustedDigit = true;
            }
            break;
        }
        case '9':
        {
            if (increment)
            {
                if (handleAsHex)
                {
                    currentDigit = 'A';
                }
                else
                {
                    currentDigit = '0';
                }

                adjustedDigit = true;
            }
            break;
        }
        case 'A':
        {
            if (!increment)
            {
                currentDigit = '9';
                adjustedDigit = true;
            }
            break;
        }
        case 'F':
        {
            if (increment)
            {
                currentDigit = '0';
                adjustedDigit = true;
            }
            break;
        }
        default:
        {
            break;
        }
    }

    if (!adjustedDigit)
    {
        char adjustmentValue = 1;

        // If decrementing, make adjustmentValue negative
        if (!increment)
        {
            adjustmentValue = -adjustmentValue;
        }

        currentDigit += adjustmentValue;
    }

    valueString[currentIndex] = currentDigit;
}
*/

/**
 * Handles converting a `float` value to a string.
 *
 * @param[out] bufOut Pointer to where the string should be written to.
 * @param[in] bufSize The total size of the `bufOut` parameter.
 * @param[out] formatOut Pointer to where the format string used to convert the `float` value to a string should be written to.
 * @param[in] formatSize The total size of the `formatOut` parameter.
 * @param[in] totalLength The total amount of digits in the value, limited to `MAX_DOUBLE_LENGTH`.
 * @param[in] value The value being converted to a string.
 *
 * @returns The value returned from `snprintf` creating the string to be written to `bufOut`. If the `value` parameter is not
 * one that can be edited easily (specified by `floatCanBeWorkedWith`), then `-1` will be returned instead.
 *
 * @relatesalso ValueEditor
 */
static int32_t floatToString(char *bufOut,
                             uint32_t bufSize,
                             char *formatOut,
                             uint32_t formatSize,
                             uint32_t totalLength,
                             float value)
{
    // Make sure the number is one that can be edited easily
    if (!floatCanBeWorkedWith(value))
    {
        return -1;
    }

    // Make sure the length is valid
    if ((static_cast<int32_t>(totalLength) <= 0) || (totalLength > MAX_DOUBLE_LENGTH))
    {
        totalLength = MAX_DOUBLE_LENGTH;
    }

    // Create the format string
    snprintf(formatOut, formatSize, "%%0%" PRIu32 "." MAX_DOUBLE_DIGITS_STRING "f", totalLength);

    // Create the actual string
    return snprintf(bufOut, bufSize, formatOut, value);
}

/**
 * Handles converting a `double` value to a string.
 *
 * @param[out] bufOut Pointer to where the string should be written to.
 * @param[in] bufSize The total size of the `bufOut` parameter.
 * @param[out] formatOut Pointer to where the format string used to convert the `double` value to a string should be written to.
 * @param[in] formatSize The total size of the `formatOut` parameter.
 * @param[in] totalLength The total amount of digits in the value, limited to `MAX_DOUBLE_LENGTH`.
 * @param[in] value The value being converted to a string.
 *
 * @returns The value returned from `snprintf` creating the string to be written to `bufOut`. If the `value` parameter is not
 * one that can be edited easily (specified by `doubleCanBeWorkedWith`), then `-1` will be returned instead.
 *
 * @relatesalso ValueEditor
 */
static int32_t doubleToString(char *bufOut,
                              uint32_t bufSize,
                              char *formatOut,
                              uint32_t formatSize,
                              uint32_t totalLength,
                              double value)
{
    // Make sure the number is one that can be edited easily
    if (!doubleCanBeWorkedWith(value))
    {
        return -1;
    }

    // Make sure the length is valid
    if ((static_cast<int32_t>(totalLength) <= 0) || (totalLength > MAX_DOUBLE_LENGTH))
    {
        totalLength = MAX_DOUBLE_LENGTH;
    }

    // Create the format string
    snprintf(formatOut, formatSize, "%%0%" PRIu32 "." MAX_DOUBLE_DIGITS_STRING "f", totalLength);

    // Create the actual string
    return snprintf(bufOut, bufSize, formatOut, value);
}
