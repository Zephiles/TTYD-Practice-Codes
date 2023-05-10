#include "mod.h"
#include "drawIcon.h"
#include "drawText.h"
#include "menuUtils.h"
#include "classes/valueEditor.h"
#include "classes/window.h"
#include "misc/utils.h"
#include "ttyd/fontmgr.h"

#include <cstdint>
#include <cstring>
#include <cmath>
#include <cstdio>
#include <cinttypes>

void ValueEditor::init(const void *valuePtr,
                       const void *minValuePtr,
                       const void *maxValuePtr,
                       const Window *parentWindow,
                       uint32_t flags,
                       VariableType type,
                       float scale)
{
    this->init(valuePtr, minValuePtr, maxValuePtr, parentWindow, flags, type, 0xFF, scale);
}

void ValueEditor::init(const void *valuePtr,
                       const void *minValuePtr,
                       const void *maxValuePtr,
                       const Window *parentWindow,
                       uint32_t flags,
                       VariableType type,
                       uint8_t alpha,
                       float scale)
{
    this->minValuePtr = minValuePtr;
    this->maxValuePtr = maxValuePtr;
    this->flags = flags;
    this->alpha = alpha;
    this->scale = scale;
    this->setValueFunc = nullptr;
    this->cancelFunc = nullptr;
    this->waitFramesToBegin = 0;
    this->shouldIncrementNow = false;
    this->enabled = false;

    // If the value is not being handled as signed, then change the type to unsigned
    const bool valueIsSigned = this->flagIsSet(ValueEditorFlag::VALUE_IS_SIGNED);
    if (!valueIsSigned)
    {
        switch (type)
        {
            case VariableType::s8:
            {
                type = VariableType::u8;
                break;
            }
            case VariableType::s16:
            {
                type = VariableType::u16;
                break;
            }
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
        case VariableType::s8:
        {
            int32_t value = newValuePtr->s8;

            uint32_t maxDigits;
            if (minAndMaxSet)
            {
                int32_t maxValue = newMaxValuePtr->s8;
                uint32_t digitsMaxValue = getMaxDigits(maxValue, handleAsHex);

                int32_t minValue = newMinValuePtr->s8;
                uint32_t digitsMinValue = getMaxDigits(minValue, handleAsHex);

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
            else if (handleAsHex)
            {
                maxDigits = 2; // 7F
            }
            else
            {
                maxDigits = 3; // 127
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
                tempFormat = "0x%%0%" PRIu32 PRIX8;
            }
            else
            {
                tempFormat = "%%0%" PRIu32 PRId8;
            }

            // Put + or - in front of the value
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
            snprintf(editorValuePtr, editorValueSize, format, static_cast<int8_t>(value));
            break;
        }
        case VariableType::s16:
        {
            int32_t value = newValuePtr->s16;

            uint32_t maxDigits;
            if (minAndMaxSet)
            {
                int32_t maxValue = newMaxValuePtr->s16;
                uint32_t digitsMaxValue = getMaxDigits(maxValue, handleAsHex);

                int32_t minValue = newMinValuePtr->s16;
                uint32_t digitsMinValue = getMaxDigits(minValue, handleAsHex);

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
            else if (handleAsHex)
            {
                maxDigits = 4; // 7FFF
            }
            else
            {
                maxDigits = 5; // 32767
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
                tempFormat = "0x%%0%" PRIu32 PRIX16;
            }
            else
            {
                tempFormat = "%%0%" PRIu32 PRId16;
            }

            // Put + or - in front of the value
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
            snprintf(editorValuePtr, editorValueSize, format, static_cast<int16_t>(value));
            break;
        }
        case VariableType::s32:
        {
            int32_t value = newValuePtr->s32;

            uint32_t maxDigits;
            if (minAndMaxSet)
            {
                int32_t maxValue = newMaxValuePtr->s32;
                uint32_t digitsMaxValue = getMaxDigits(maxValue, handleAsHex);

                int32_t minValue = newMinValuePtr->s32;
                uint32_t digitsMinValue = getMaxDigits(minValue, handleAsHex);

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
            else if (handleAsHex)
            {
                maxDigits = 8; // 7FFFFFFF
            }
            else
            {
                maxDigits = 10; // 2147483647
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
            int64_t value = newValuePtr->s64;

            uint32_t maxDigits;
            if (minAndMaxSet)
            {
                int64_t maxValue = newMaxValuePtr->s64;
                uint32_t digitsMaxValue = getMaxDigits(maxValue, handleAsHex);

                int64_t minValue = newMinValuePtr->s64;
                uint32_t digitsMinValue = getMaxDigits(minValue, handleAsHex);

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
            else if (handleAsHex)
            {
                maxDigits = 16; // 7FFFFFFFFFFFFFFF
            }
            else
            {
                maxDigits = 19; // 9223372036854775807
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
        case VariableType::u8:
        {
            uint32_t maxDigits;
            if (minAndMaxSet)
            {
                uint32_t maxValue = newMaxValuePtr->u8;
                uint32_t digitsMaxValue = getMaxDigits(maxValue, handleAsHex);

                uint32_t minValue = newMinValuePtr->u8;
                uint32_t digitsMinValue = getMaxDigits(minValue, handleAsHex);

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
            else if (handleAsHex)
            {
                maxDigits = 2; // 7F
            }
            else
            {
                maxDigits = 3; // 127
            }

            this->maxDigits = static_cast<uint8_t>(maxDigits);

            // Subtract 1 since neither + nor - are not present
            uint32_t currentIndex = maxDigits;
            currentIndex--;

            if (handleAsHex)
            {
                // Add 2 to account for 0x
                currentIndex += 2;
            }
            this->currentIndex = static_cast<uint8_t>(currentIndex);

            const char *tempFormat;
            if (handleAsHex)
            {
                tempFormat = "0x%%0%" PRIu32 PRIX8;
            }
            else
            {
                tempFormat = "%%0%" PRIu32 PRIu8;
            }

            snprintf(format, sizeof(this->format), tempFormat, maxDigits);
            snprintf(editorValue, sizeof(this->editorValue), format, newValuePtr->u8);
            break;
        }
        case VariableType::u16:
        {
            uint32_t maxDigits;
            if (minAndMaxSet)
            {
                uint32_t maxValue = newMaxValuePtr->u16;
                uint32_t digitsMaxValue = getMaxDigits(maxValue, handleAsHex);

                uint32_t minValue = newMinValuePtr->u16;
                uint32_t digitsMinValue = getMaxDigits(minValue, handleAsHex);

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
            else if (handleAsHex)
            {
                maxDigits = 4; // 7FFF
            }
            else
            {
                maxDigits = 5; // 32767
            }

            this->maxDigits = static_cast<uint8_t>(maxDigits);

            // Subtract 1 since neither + nor - are not present
            uint32_t currentIndex = maxDigits;
            currentIndex--;

            if (handleAsHex)
            {
                // Add 2 to account for 0x
                currentIndex += 2;
            }
            this->currentIndex = static_cast<uint8_t>(currentIndex);

            const char *tempFormat;
            if (handleAsHex)
            {
                tempFormat = "0x%%0%" PRIu32 PRIX16;
            }
            else
            {
                tempFormat = "%%0%" PRIu32 PRIu16;
            }

            snprintf(format, sizeof(this->format), tempFormat, maxDigits);
            snprintf(editorValue, sizeof(this->editorValue), format, newValuePtr->u16);
            break;
        }
        case VariableType::u32:
        {
            uint32_t maxDigits;
            if (minAndMaxSet)
            {
                uint32_t maxValue = newMaxValuePtr->u32;
                uint32_t digitsMaxValue = getMaxDigits(maxValue, handleAsHex);

                uint32_t minValue = newMinValuePtr->u32;
                uint32_t digitsMinValue = getMaxDigits(minValue, handleAsHex);

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
            else if (handleAsHex)
            {
                maxDigits = 8; // 7FFFFFFF
            }
            else
            {
                maxDigits = 10; // 2147483647
            }

            this->maxDigits = static_cast<uint8_t>(maxDigits);

            // Subtract 1 since neither + nor - are not present
            uint32_t currentIndex = maxDigits;
            currentIndex--;

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
            snprintf(editorValue, sizeof(this->editorValue), format, newValuePtr->u32);
            break;
        }
        case VariableType::u64:
        {
            uint32_t maxDigits;
            if (minAndMaxSet)
            {
                uint64_t maxValue = newMaxValuePtr->u64;
                uint32_t digitsMaxValue = getMaxDigits(maxValue, handleAsHex);

                uint64_t minValue = newMinValuePtr->u64;
                uint32_t digitsMinValue = getMaxDigits(minValue, handleAsHex);

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
            else if (handleAsHex)
            {
                maxDigits = 16; // 7FFFFFFFFFFFFFFF
            }
            else
            {
                maxDigits = 19; // 9223372036854775807
            }

            this->maxDigits = static_cast<uint8_t>(maxDigits);

            // Subtract 1 since neither + nor - are not present
            uint32_t currentIndex = maxDigits;
            currentIndex--;

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
            snprintf(editorValue, sizeof(this->editorValue), format, newValuePtr->u64);
            break;
        }
        case VariableType::f32:
        {
            float value = newValuePtr->f32;

            uint32_t maxDigits;
            if (minAndMaxSet)
            {
                float maxValue = newMaxValuePtr->f32;
                // uint32_t digitsMaxValue = getMaxDigits(maxValue, handleAsHex);
                uint32_t digitsMaxValue = getMaxDigits(maxValue);

                float minValue = newMinValuePtr->f32;
                // uint32_t digitsMinValue = getMaxDigits(minValue, handleAsHex);
                uint32_t digitsMinValue = getMaxDigits(minValue);

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
            /*
                else if (handleAsHex)
                {
                    maxDigits = 8; // 7FFFFFFF
                }
            */
            else
            {
                // Subtract 1 to get the max digit
                maxDigits = MAX_DOUBLE_LENGTH - 1;

                if (valueIsSigned)
                {
                    // Subtract 1 to account for the + or -
                    maxDigits--;
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

            /*
                if (handleAsHex)
                {
                    // Add 2 to account for 0x
                    currentIndex += 2;
                }
                else
                {
            */
            // Add 1 to account for the decimal point
            currentIndex++;
            /*
                }
            */

            this->currentIndex = static_cast<uint8_t>(currentIndex);

            // If the value is signed, then put + or - in front of the value
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

            /*
                if (handleAsHex)
                {
                    const char *tempFormat = "0x%%0%" PRIu32 PRIX32;
                    snprintf(format, sizeof(this->format), tempFormat, maxDigits);

                    newValuePtr->f32 = value;
                    snprintf(editorValuePtr, editorValueSize, format, newValuePtr->u32);
                }
                else
                {
            */
            int32_t len = floatToString(editorValuePtr, editorValueSize, format, sizeof(this->format), maxDigits + 1, value);

            // Make sure an error didn't occur
            if (len <= 0)
            {
                float defaultValue;
                if (minAndMaxSet)
                {
                    defaultValue = newMinValuePtr->f32;
                }
                else
                {
                    defaultValue = 0.f;
                }

                floatToString(editorValuePtr, editorValueSize, format, sizeof(this->format), maxDigits + 1, defaultValue);
            }
            /*
                }
            */
            break;
        }
        case VariableType::f64:
        {
            double value = newValuePtr->f64;

            uint32_t maxDigits;
            if (minAndMaxSet)
            {
                double maxValue = newMaxValuePtr->f64;
                uint32_t digitsMaxValue = getMaxDigits(maxValue);

                double minValue = newMinValuePtr->f64;
                uint32_t digitsMinValue = getMaxDigits(minValue);

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
            else
            {
                // Subtract 1 to get the max digit
                maxDigits = MAX_DOUBLE_LENGTH - 1;

                if (valueIsSigned)
                {
                    // Subtract 1 to account for the + or -
                    maxDigits--;
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
                double defaultValue;
                if (minAndMaxSet)
                {
                    defaultValue = newMinValuePtr->f64;
                }
                else
                {
                    defaultValue = 0.0;
                }

                doubleToString(editorValuePtr, editorValueSize, format, sizeof(this->format), maxDigits + 1, defaultValue);
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

    snprintf(&helpText[len], helpTextSize - len, "Press A to confirm\nPress B to cancel");

    // Set up the window
    // Initialize it based on the help text
    Window *windowPtr = &this->window;
    windowPtr->setWidthHeightFromTextAndInit(helpText, scale, SPECIAL_WINDOW_COLOR | this->alpha, 20.f, 20.f);

    // Increase the height to account for the value
    const float valueScale = scale + 0.3f;
    const float padding = windowPtr->getPadding() * scale;
    windowPtr->setHeight(windowPtr->getHeight() + ((LINE_HEIGHT_FLOAT * valueScale) / 2.f) + padding);

    // Increase the height of the window based on flags that are set
    const float lineDecrement = LINE_HEIGHT_FLOAT * scale;

    if (this->flagIsSet(ValueEditorFlag::DRAW_ITEM_ICON_AND_TEXT))
    {
        // Account for the icon and item text
        windowPtr->setHeight(windowPtr->getHeight() + SPACE_USED_PER_ICON(scale) + padding);
    }
    else if (this->flagIsSet(ValueEditorFlag::DRAW_STAGE_AND_EVENT))
    {
        // Account for the stage and event texts
        windowPtr->setHeight(windowPtr->getHeight() + (lineDecrement * 3.f) + padding);
    }
    else if (this->flagIsSet(ValueEditorFlag::DRAW_MAP_STRING))
    {
        // Account for the map text
        windowPtr->setHeight(windowPtr->getHeight() + (lineDecrement * 2.f) + padding);
    }
    else if (this->flagIsSet(ValueEditorFlag::DRAW_WARP_BY_EVENT_DETAILS))
    {
        // Account for all of the extra lines of text
        windowPtr->setHeight(windowPtr->getHeight() + (lineDecrement * 7.f) + padding);
    }

    // If the width of the value text will be larger than the current width, then reset the width to the width of the value text
    const float valueStringLength = intToFloat(static_cast<int32_t>(strlen(editorValue)));
    const float digitLength = 18.8888888f;
    const float valueStringWidth = (valueStringLength * digitLength) * valueScale;

    if (valueStringWidth > windowPtr->getWidth())
    {
        windowPtr->setWidth(valueStringWidth + padding);
    }

    // Place the window inside of the parent window
    windowPtr->placeInWindow(parentWindow, WindowAlignment::MIDDLE_CENTER, scale);
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
            case VariableType::s8:
            {
                const int32_t value = valuePtr->s8;
                valuePtr->s8 = static_cast<int8_t>(-value);
                break;
            }
            case VariableType::s16:
            {
                const int32_t value = valuePtr->s16;
                valuePtr->s16 = static_cast<int16_t>(-value);
                break;
            }
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

bool ValueEditor::handleCheckMinMax(const ValueType *valuePtr, bool valueIsPositive)
{
    // Both a min and a max need to be set to use either of them
    // Handle the min and max as ValueType
    const ValueType *minValuePtr = reinterpret_cast<const ValueType *>(this->minValuePtr);
    const ValueType *maxValuePtr = reinterpret_cast<const ValueType *>(this->maxValuePtr);

    const bool minAndMaxSet = minValuePtr && maxValuePtr;
    if (!minAndMaxSet)
    {
        return false;
    }

    // Make sure the value does not exceed the min or max
    switch (this->type)
    {
        case VariableType::s8:
        {
            int32_t currentValue = valuePtr->s8;

            // If the value was originally negative, then convert it to negative
            if (!valueIsPositive)
            {
                currentValue = -currentValue;
            }

            const int32_t minValue = minValuePtr->s8;
            const int32_t maxValue = maxValuePtr->s8;

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
        case VariableType::s16:
        {
            int32_t currentValue = valuePtr->s16;

            // If the value was originally negative, then convert it to negative
            if (!valueIsPositive)
            {
                currentValue = -currentValue;
            }

            const int32_t minValue = minValuePtr->s16;
            const int32_t maxValue = maxValuePtr->s16;

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
        case VariableType::s32:
        {
            int32_t currentValue = valuePtr->s32;

            // If the value was originally negative, then convert it to negative
            if (!valueIsPositive)
            {
                currentValue = -currentValue;
            }

            const int32_t minValue = minValuePtr->s32;
            const int32_t maxValue = maxValuePtr->s32;

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

            const int64_t minValue = minValuePtr->s64;
            const int64_t maxValue = maxValuePtr->s64;

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
        case VariableType::u8:
        {
            const uint32_t currentValue = valuePtr->u8;
            const uint32_t minValue = minValuePtr->u8;
            const uint32_t maxValue = maxValuePtr->u8;

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
        case VariableType::u16:
        {
            const uint32_t currentValue = valuePtr->u16;
            const uint32_t minValue = minValuePtr->u16;
            const uint32_t maxValue = maxValuePtr->u16;

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
            const uint32_t minValue = minValuePtr->u32;
            const uint32_t maxValue = maxValuePtr->u32;

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
            const uint32_t minValue = minValuePtr->u64;
            const uint32_t maxValue = maxValuePtr->u64;

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

            const float minValue = minValuePtr->f32;
            const float maxValue = maxValuePtr->f32;

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

            const double minValue = minValuePtr->f64;
            const double maxValue = maxValuePtr->f64;

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

void ValueEditor::adjustValue(bool increment)
{
    // Make sure currentIndex is valid
    char *editorValue = this->editorValue;
    uint32_t currentIndex = this->currentIndex;

    if (currentIndex >= strlen(editorValue))
    {
        // Failsafe: Reset currentIndex to 0
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
            this->handleCheckMinMax(&value, false);
            return;
        }
        case '-':
        {
            // Can only be changed to +
            currentDigit[0] = '+';

            // Make sure the value does not exceed the min or max
            this->handleCheckMinMax(&value, true);
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
        case VariableType::s8:
        {
            // Adjust the current digit
            const int32_t currentValue = value.s8;
            int32_t newValue = handleAdjustValue(currentValue, currentIndex, totalDigits, handleAsHex, increment);

            // Make sure the value is in the proper format
            newValue = static_cast<int8_t>(newValue);

            // Make sure the value does not exceed the min or max
            // The value is already converted to negative, so just pass in true
            value.s8 = static_cast<int8_t>(newValue);
            if (this->handleCheckMinMax(&value, true))
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
            snprintf(editorValuePtr, editorValueSize, format, static_cast<int8_t>(newValue));
            break;
        }
        case VariableType::s16:
        {
            // Adjust the current digit
            const int32_t currentValue = value.s16;
            int32_t newValue = handleAdjustValue(currentValue, currentIndex, totalDigits, handleAsHex, increment);

            // Make sure the value is in the proper format
            newValue = static_cast<int16_t>(newValue);

            // Make sure the value does not exceed the min or max
            // The value is already converted to negative, so just pass in true
            value.s16 = static_cast<int16_t>(newValue);
            if (this->handleCheckMinMax(&value, true))
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
            snprintf(editorValuePtr, editorValueSize, format, static_cast<int16_t>(newValue));
            break;
        }
        case VariableType::s32:
        {
            // Adjust the current digit
            const int32_t currentValue = value.s32;
            int32_t newValue = handleAdjustValue(currentValue, currentIndex, totalDigits, handleAsHex, increment);

            // Make sure the value does not exceed the min or max
            // The value is already converted to negative, so just pass in true
            value.s32 = newValue;
            if (this->handleCheckMinMax(&value, true))
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
            if (this->handleCheckMinMax(&value, true))
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
        case VariableType::u8:
        {
            // Adjust the current digit
            const uint32_t currentValue = value.u8;
            uint32_t newValue = handleAdjustValue(currentValue, currentIndex, totalDigits, handleAsHex, increment);

            // Make sure the value is in the proper format
            newValue = static_cast<uint8_t>(newValue);

            // Make sure the value does not exceed the min or max
            // The value can only be positive, so just pass in true
            value.u8 = static_cast<uint8_t>(newValue);
            if (this->handleCheckMinMax(&value, true))
            {
                return;
            }

            // Set the new value
            snprintf(editorValuePtr, editorValueSize, format, static_cast<uint8_t>(newValue));
            break;
        }
        case VariableType::u16:
        {
            // Adjust the current digit
            const uint32_t currentValue = value.u16;
            uint32_t newValue = handleAdjustValue(currentValue, currentIndex, totalDigits, handleAsHex, increment);

            // Make sure the value is in the proper format
            newValue = static_cast<uint16_t>(newValue);

            // Make sure the value does not exceed the min or max
            // The value can only be positive, so just pass in true
            value.u16 = static_cast<uint16_t>(newValue);
            if (this->handleCheckMinMax(&value, true))
            {
                return;
            }

            // Set the new value
            snprintf(editorValuePtr, editorValueSize, format, static_cast<uint16_t>(newValue));
            break;
        }
        case VariableType::u32:
        {
            // Adjust the current digit
            const uint32_t currentValue = value.u32;
            uint32_t newValue = handleAdjustValue(currentValue, currentIndex, totalDigits, handleAsHex, increment);

            // Make sure the value does not exceed the min or max
            // The value can only be positive, so just pass in true
            value.u32 = newValue;
            if (this->handleCheckMinMax(&value, true))
            {
                return;
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

            // Make sure the value does not exceed the min or max
            // The value can only be positive, so just pass in true
            value.u64 = newValue;
            if (this->handleCheckMinMax(&value, true))
            {
                return;
            }

            // Set the new value
            snprintf(editorValuePtr, editorValueSize, format, newValue);
            break;
        }
        case VariableType::f32:
        {
            // Both a min and a max need to be set to use either of them
            const ValueType *minValuePtr = reinterpret_cast<const ValueType *>(this->minValuePtr);
            const bool minAndMaxSet = minValuePtr && this->maxValuePtr;

            // Make sure the current value is one that can be edited easily
            float currentValue = value.f32;
            switch (std::fpclassify(currentValue))
            {
                case FP_ZERO:
                case FP_NORMAL:
                {
                    break;
                }
                default:
                {
                    if (minAndMaxSet)
                    {
                        currentValue = minValuePtr->f32;
                    }
                    else
                    {
                        currentValue = 0.f;
                    }
                }
            }

            float newValue = handleAdjustValue(currentValue, currentIndex, totalDigits, valueIsSigned, increment);

            // Make sure the new value is one that can be edited easily
            switch (std::fpclassify(newValue))
            {
                case FP_ZERO:
                case FP_NORMAL:
                {
                    break;
                }
                default:
                {
                    if (minAndMaxSet)
                    {
                        newValue = minValuePtr->f32;
                    }
                    else
                    {
                        newValue = 0.f;
                    }
                }
            }

            // Make sure the value does not exceed the min or max
            // The value is already converted to negative, so just pass in true
            value.f32 = newValue;
            if (this->handleCheckMinMax(&value, true))
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
            // Both a min and a max need to be set to use either of them
            const ValueType *minValuePtr = reinterpret_cast<const ValueType *>(this->minValuePtr);
            const bool minAndMaxSet = minValuePtr && this->maxValuePtr;

            // Make sure the current value is one that can be edited easily
            double currentValue = value.f64;
            switch (std::fpclassify(currentValue))
            {
                case FP_ZERO:
                case FP_NORMAL:
                {
                    break;
                }
                default:
                {
                    if (minAndMaxSet)
                    {
                        currentValue = minValuePtr->f64;
                    }
                    else
                    {
                        currentValue = 0.0;
                    }
                }
            }

            double newValue = handleAdjustValue(currentValue, currentIndex, totalDigits, valueIsSigned, increment);

            // Make sure the new value is one that can be edited easily
            switch (std::fpclassify(newValue))
            {
                case FP_ZERO:
                case FP_NORMAL:
                {
                    break;
                }
                default:
                {
                    if (minAndMaxSet)
                    {
                        newValue = minValuePtr->f64;
                    }
                    else
                    {
                        newValue = 0.0;
                    }
                }
            }

            // Make sure the value does not exceed the min or max
            // The value is already converted to negative, so just pass in true
            value.f64 = newValue;
            if (this->handleCheckMinMax(&value, true))
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
    // If no min value is set, then do nothing
    const ValueType *minValuePtr = reinterpret_cast<const ValueType *>(this->minValuePtr);
    if (!minValuePtr)
    {
        return;
    }

    const char *format = this->format;
    char *editorValue = this->editorValue;
    uint32_t editorValueSize = sizeof(this->editorValue);
    const bool valueIsSigned = this->flagIsSet(ValueEditorFlag::VALUE_IS_SIGNED);

    switch (this->type)
    {
        case VariableType::s8:
        {
            int8_t minValue = minValuePtr->s8;

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
        case VariableType::s16:
        {
            int16_t minValue = minValuePtr->s16;

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
        case VariableType::s32:
        {
            int32_t minValue = minValuePtr->s32;

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
            int64_t minValue = minValuePtr->s64;

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
        case VariableType::u8:
        {
            const uint8_t minValue = minValuePtr->u8;
            snprintf(editorValue, sizeof(this->editorValue), format, minValue);
            break;
        }
        case VariableType::u16:
        {
            const uint16_t minValue = minValuePtr->u16;
            snprintf(editorValue, sizeof(this->editorValue), format, minValue);
            break;
        }
        case VariableType::u32:
        {
            const uint32_t minValue = minValuePtr->u32;
            snprintf(editorValue, sizeof(this->editorValue), format, minValue);
            break;
        }
        case VariableType::u64:
        {
            const uint64_t minValue = minValuePtr->u64;
            snprintf(editorValue, sizeof(this->editorValue), format, minValue);
            break;
        }
        case VariableType::f32:
        {
            float minValue = minValuePtr->f32;

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
            double minValue = minValuePtr->f64;

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
    // If no max value is set, then do nothing
    const ValueType *maxValuePtr = reinterpret_cast<const ValueType *>(this->maxValuePtr);
    if (!maxValuePtr)
    {
        return;
    }

    const char *format = this->format;
    char *editorValue = this->editorValue;
    uint32_t editorValueSize = sizeof(this->editorValue);
    const bool valueIsSigned = this->flagIsSet(ValueEditorFlag::VALUE_IS_SIGNED);

    switch (this->type)
    {
        case VariableType::s8:
        {
            int8_t maxValue = maxValuePtr->s8;

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
        case VariableType::s16:
        {
            int16_t maxValue = maxValuePtr->s16;

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
        case VariableType::s32:
        {
            int32_t maxValue = maxValuePtr->s32;

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
            int64_t maxValue = maxValuePtr->s64;

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
        case VariableType::u8:
        {
            const uint8_t maxValue = maxValuePtr->u8;
            snprintf(editorValue, sizeof(this->editorValue), format, maxValue);
            break;
        }
        case VariableType::u16:
        {
            const uint16_t maxValue = maxValuePtr->u16;
            snprintf(editorValue, sizeof(this->editorValue), format, maxValue);
            break;
        }
        case VariableType::u32:
        {
            const uint32_t maxValue = maxValuePtr->u32;
            snprintf(editorValue, sizeof(this->editorValue), format, maxValue);
            break;
        }
        case VariableType::u64:
        {
            const uint64_t maxValue = maxValuePtr->u64;
            snprintf(editorValue, sizeof(this->editorValue), format, maxValue);
            break;
        }
        case VariableType::f32:
        {
            float maxValue = maxValuePtr->f32;

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
            double maxValue = maxValuePtr->f64;

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

            // If currentIndex is now invalid, then loop to the last option
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
    const bool autoIncrement = handleMenuAutoIncrementDigit(&this->waitFramesToBegin, &this->shouldIncrementNow);

    const char *editorValue = this->editorValue;
    const uint32_t maxDigit = strlen(editorValue) - 1;

    // Handle held button inputs if auto-incrementing should be done
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

    // Handle the button inputs pressed this frame
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
            const SetValueFunc func = this->setValueFunc;
            if (!func)
            {
                break;
            }

            // Get the new value
            ValueType value;
            if (this->getValueFromString(&value))
            {
                func(&value);
            }

            break;
        }
        case MenuButtonInput::B:
        {
            // Cancel editing the value
            const ValueEditorCancelFunc func = this->cancelFunc;
            if (func)
            {
                func();
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
    // Draw the window
    const Window *windowPtr = &this->window;
    windowPtr->draw();

    // Initialize text drawing
    drawTextInit(false);

    // Draw the help text
    const char *helpText = this->helpText;
    const float scale = this->scale;

    float tempPosX;
    float tempPosY;
    windowPtr->getTextPosXY(helpText, WindowAlignment::TOP_CENTER, scale, &tempPosX, &tempPosY);

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    float posX = tempPosX;
    float posY = tempPosY;
    drawText(helpText, posX, posY, scale, getColorWhite(0xFF));

    // Draw various things based on flags that are set
    const float lineDecrement = LINE_HEIGHT_FLOAT * scale;

    // Set posY to be two lines under the help text
    uint16_t lineCount;
    FontGetMessageWidthLine(this->helpText, &lineCount);
    posY -= (intToFloat(lineCount + 3) * lineDecrement);

    ValueType value;
    if (this->flagIsSet(ValueEditorFlag::DRAW_ITEM_ICON_AND_TEXT))
    {
        // Get the current item id
        if (this->getValueFromString(&value))
        {
            // Several items need to have their icon scales adjusted
            const ItemId item = static_cast<ItemId>(value.s16);
            const float iconScale = adjustItemIconScale(item, scale);

            // Draw the current item with its icon and text
            posX = windowPtr->getIconPosX(WindowAlignment::BOTTOM_LEFT, scale);
            drawItemIconWithText(posX, posY, iconScale, scale, 0.f, item, getColorWhite(0xFF));
        }
    }

    // Get the X and Y coordinates for the value
    const int32_t stringLength = static_cast<int32_t>(strlen(editorValue));
    const float stringLengthFloat = intToFloat(stringLength);

    const float digitLength = 18.8888888f;
    const float valueScale = scale + 0.3f;
    const float valueStringWidth = (stringLengthFloat * digitLength) * valueScale;

    posX = windowPtr->getPosX() + ((windowPtr->getWidth() - valueStringWidth) / 2.f);

    // getTextPosY only uses one scale, while two are required here, so have to calculate posY manually
    float valueHeight;
    getTextWidthHeight(editorValue, valueScale, nullptr, &valueHeight);

    posY = windowPtr->getPosY() - windowPtr->getHeight() + valueHeight + (windowPtr->getPadding() * scale) +
           LINE_HEIGHT_ADJUSTMENT_4(scale);

    // Draw the value
    const uint32_t currentIndex = this->currentIndex;
    const float posXIncrement = digitLength * valueScale;
    uint32_t color;

    // Place the current character into a temporary buffer
    char tempBuf[2];
    tempBuf[1] = '\0';

    for (int32_t i = 0; i < stringLength; i++)
    {
        const char currentCharacter = editorValue[i];
        tempBuf[0] = currentCharacter;

        // If the current character is the decimal point, then move it to the right slightly
        float tempPosX = posX;
        if (currentCharacter == '.')
        {
#ifdef TTYD_JP
            constexpr float baseOffset = 4.5f;
#else
            constexpr float baseOffset = 5.f;
#endif
            tempPosX += (baseOffset / valueScale);
        }

        color = getCurrentOptionColor(static_cast<uint32_t>(i) == currentIndex, 0xFF);
        drawText(tempBuf, tempPosX, posY, valueScale, color);
        posX += posXIncrement;
    }
}

uint32_t getMaxDigits(uint32_t value, bool handleAsHex)
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

uint32_t getMaxDigits(int32_t value, bool handleAsHex)
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

uint32_t getMaxDigits(uint64_t value, bool handleAsHex)
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

uint32_t getMaxDigits(int64_t value, bool handleAsHex)
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

/*
uint32_t getMaxDigits(float value, bool handleAsHex)
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

uint32_t getMaxDigits(float value)
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

uint32_t getMaxDigits(double value)
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

uint32_t handleAdjustValue(uint32_t value, uint32_t currentIndex, uint32_t totalDigits, bool handleAsHex, bool increment)
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

int32_t handleAdjustValue(int32_t value, uint32_t currentIndex, uint32_t totalDigits, bool handleAsHex, bool increment)
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

uint64_t handleAdjustValue(uint64_t value, uint32_t currentIndex, uint32_t totalDigits, bool handleAsHex, bool increment)
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

int64_t handleAdjustValue(int64_t value, uint32_t currentIndex, uint32_t totalDigits, bool handleAsHex, bool increment)
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

/*
float handleAdjustValue(float value,
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

float handleAdjustValue(float value, uint32_t currentIndex, uint32_t totalDigits, bool valueIsSigned, bool increment)
{
    const float base = 10.f;
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

double handleAdjustValue(double value, uint32_t currentIndex, uint32_t totalDigits, bool valueIsSigned, bool increment)
{
    const double base = 10.0;
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

/*
void handleAdjustValueDouble(char *valueString, uint32_t currentIndex, bool handleAsHex, bool increment)
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

int32_t floatToString(char *bufOut, uint32_t bufSize, char *formatOut, uint32_t formatSize, uint32_t totalLength, float value)
{
    // Make sure the number is one that can be edited easily
    switch (std::fpclassify(value))
    {
        case FP_ZERO:
        case FP_NORMAL:
        {
            break;
        }
        default:
        {
            return -1;
        }
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

int32_t doubleToString(char *bufOut, uint32_t bufSize, char *formatOut, uint32_t formatSize, uint32_t totalLength, double value)
{
    // Make sure the number is one that can be edited easily
    switch (std::fpclassify(value))
    {
        case FP_ZERO:
        case FP_NORMAL:
        {
            break;
        }
        default:
        {
            return -1;
        }
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
