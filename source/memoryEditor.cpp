#include "memoryEditor.h"
#include "menuUtils.h"
#include "cxx.h"
#include "mod.h"
#include "drawText.h"
#include "cheats.h"
#include "classes/window.h"
#include "gc/pad.h"
#include "menus/memoryMenu.h"
#include "menus/rootMenu.h"
#include "misc/utils.h"
#include "ttyd/win_main.h"
#include "ttyd/windowdrv.h"
#include "ttyd/camdrv.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

MemoryEditor *gMemoryEditor = nullptr;

bool MemoryEditor::enabledFlagIsSet(uint32_t enabledFlag) const
{
    // Make sure the flag is valid
    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
    constexpr uint32_t maxFlags = MEMORY_EDITOR_ENABLED_FLAGS_ARRAY_SIZE * bitsPerWord;

    if (enabledFlag >= maxFlags)
    {
        return false;
    }

    // Make sure the flag does not exceed TOTAL_MEMORY_EDITOR_ENABLED_FLAGS
    if (enabledFlag >= TOTAL_MEMORY_EDITOR_ENABLED_FLAGS)
    {
        return false;
    }

    return (this->enabledFlags[enabledFlag / bitsPerWord] >> (enabledFlag % bitsPerWord)) & 1U;
}

void MemoryEditor::setEnabledFlag(uint32_t enabledFlag)
{
    // Make sure the flag is valid
    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
    constexpr uint32_t maxFlags = MEMORY_EDITOR_ENABLED_FLAGS_ARRAY_SIZE * bitsPerWord;

    if (enabledFlag >= maxFlags)
    {
        return;
    }

    // Make sure the flag does not exceed TOTAL_MEMORY_EDITOR_ENABLED_FLAGS
    if (enabledFlag >= TOTAL_MEMORY_EDITOR_ENABLED_FLAGS)
    {
        return;
    }

    this->enabledFlags[enabledFlag / bitsPerWord] |= (1UL << (enabledFlag % bitsPerWord));
}

void MemoryEditor::clearEnabledFlag(uint32_t enabledFlag)
{
    // Make sure the flag is valid
    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
    constexpr uint32_t maxFlags = MEMORY_EDITOR_ENABLED_FLAGS_ARRAY_SIZE * bitsPerWord;

    if (enabledFlag >= maxFlags)
    {
        return;
    }

    // Make sure the flag does not exceed TOTAL_MEMORY_EDITOR_ENABLED_FLAGS
    if (enabledFlag >= TOTAL_MEMORY_EDITOR_ENABLED_FLAGS)
    {
        return;
    }

    this->enabledFlags[enabledFlag / bitsPerWord] &= ~(1UL << (enabledFlag % bitsPerWord));
}

bool MemoryEditor::toggleEnabledFlag(uint32_t enabledFlag)
{
    // Make sure the flag is valid
    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
    constexpr uint32_t maxFlags = MEMORY_EDITOR_ENABLED_FLAGS_ARRAY_SIZE * bitsPerWord;

    if (enabledFlag >= maxFlags)
    {
        return false;
    }

    // Make sure the flag does not exceed TOTAL_MEMORY_EDITOR_ENABLED_FLAGS
    if (enabledFlag >= TOTAL_MEMORY_EDITOR_ENABLED_FLAGS)
    {
        return false;
    }

    this->enabledFlags[enabledFlag / bitsPerWord] ^= (1UL << (enabledFlag % bitsPerWord));
    return this->enabledFlagIsSet(enabledFlag);
}

static bool verifyNumBytesAndCurrentDigit(uint32_t numBytes, uint32_t currentDigit)
{
    // Make sure numBytes does not exceed the maximum
    if (numBytes > MEMORY_EDITOR_MAX_NUM_BYTES_DISPLAYED)
    {
        return false;
    }

    // Make sure currentDigit does not exceed the maximum
    if (currentDigit >= (numBytes * 2))
    {
        return false;
    }

    return true;
}

static uint8_t *getDigitAddress(uint8_t *address, uint32_t numBytes, uint32_t currentDigit)
{
    // Make sure numBytes and currentDigit are valid
    if (!verifyNumBytesAndCurrentDigit(numBytes, currentDigit))
    {
        return nullptr;
    }

    // Adjust the address based on currentDigit
    address += (currentDigit / 2);

    return address;
}

static int32_t getDigitValue(const uint8_t *address, uint32_t numBytes, uint32_t currentDigit)
{
    // Make sure the address is valid
    if (!ptrIsValid(const_cast<uint8_t *>(address)))
    {
        return -1;
    }

    // Make sure numBytes and currentDigit are valid
    if (!verifyNumBytesAndCurrentDigit(numBytes, currentDigit))
    {
        return -1;
    }

    // Get the value stored in the byte of the desired digit
    const int32_t value = static_cast<int32_t>(address[0]);

    // Get the desired digit
    const int32_t shiftAmount = (1 - (currentDigit % 2)) * 4;
    return (value & (0xF << shiftAmount)) >> shiftAmount;
}

static int32_t getDigitValueFromAddress(uint8_t *address, uint32_t numBytes, uint32_t currentDigit)
{
    // Get the address of the current digit
    address = getDigitAddress(address, numBytes, currentDigit);

    // Get the desired digit
    return getDigitValue(address, numBytes, currentDigit);
}

static uint8_t *getAndVerifyDigitAddress(uint8_t *address, uint32_t numBytes, uint32_t currentDigit)
{
    // Get the address of the current digit
    address = getDigitAddress(address, numBytes, currentDigit);

    // Make sure the address is valid
    if (!ptrIsValid(const_cast<uint8_t *>(address)))
    {
        return nullptr;
    }

    return address;
}

static void adjustDigitValue(uint8_t *address, uint32_t numBytes, uint32_t currentDigit, int32_t value)
{
    // Make sure the address is valid
    if (!ptrIsValid(address))
    {
        return;
    }

    // Make sure numBytes and currentDigit are valid
    if (!verifyNumBytesAndCurrentDigit(numBytes, currentDigit))
    {
        return;
    }

    // Get the value of the current byte, excluding the digit being modified
    // After getting the value, combine it with the changed value
    int32_t currentValue;
    int32_t otherDigit;

    if ((currentDigit % 2) == 0)
    {
        // Currently changing the first digit, so get the second
        otherDigit = getDigitValue(address, numBytes, currentDigit + 1);

        // Combine the values
        currentValue = otherDigit + (value * 0x10);
    }
    else
    {
        // Currently changing the second digit, so get the first
        otherDigit = getDigitValue(address, numBytes, currentDigit - 1);

        // Combine the values
        currentValue = (otherDigit * 0x10) + value;
    }

    // Store the new value
    *address = static_cast<uint8_t>(currentValue);
}

bool MemoryEditor::checkIfAddressInHoverRange(const uint8_t *address, uint32_t currentDigit) const
{
    // Get the amount of bytes that are being hovered over
    const uint32_t numBytesSelected = this->numBytesBeingEdited;

    // Make sure numBytesSelected and currentDigit are valid
    if (!verifyNumBytesAndCurrentDigit(numBytesSelected, currentDigit))
    {
        return false;
    }

    // Make sure editorIndex is valid
    const uint32_t editorIndex = this->editorIndex;
    if (editorIndex >= MEMORY_EDITOR_MAX_NUM_BYTES_DISPLAYED)
    {
        return false;
    }

    // Get the address that the editor is currently over
    uint8_t *hoverAddress = this->currentAddress + editorIndex;

    // Get hoverAddress's current digit address
    hoverAddress = getDigitAddress(hoverAddress, numBytesSelected, currentDigit);

    // Check if the addresses are the same
    return hoverAddress == address;
}

void MemoryEditor::adjustDigitValueFromAddress(uint32_t currentDigit, int32_t adjustAmount)
{
    // Only adjust the current digit if it's at a valid address
    const uint32_t numBytesBeingEdited = this->numBytesBeingEdited;

    if (!getAndVerifyDigitAddress(this->currentAddress + this->editorIndex, numBytesBeingEdited, currentDigit))
    {
        return;
    }

    // Get the current digit address
    uint8_t *digitAddress = getDigitAddress(this->bytesBeingEdited, numBytesBeingEdited, currentDigit);

    // Get the current digit value
    int32_t currentDigitValue = getDigitValue(digitAddress, numBytesBeingEdited, currentDigit);

    // Make sure currentDigitValue is valid
    if ((currentDigitValue < 0) || (currentDigitValue > 0xF))
    {
        return;
    }

    // Adjust currentDigitValue
    currentDigitValue += adjustAmount;

    if (currentDigitValue < 0)
    {
        currentDigitValue = 0xF;
    }
    else if (currentDigitValue > 0xF)
    {
        currentDigitValue = 0;
    }

    // Set the new digit value
    adjustDigitValue(digitAddress, numBytesBeingEdited, currentDigit, currentDigitValue);
}

static void cancelAdjustValueEditorValue()
{
    gMemoryEditor->freeValueEditor();
}

static void adjustValueEditorValue(const ValueType *valuePtr)
{
    MemoryEditor *memoryEditorPtr = gMemoryEditor;
    const uint32_t value = valuePtr->u32;

    if (memoryEditorPtr->currentlyChangingAddress())
    {
        memoryEditorPtr->setCurrentAddress(value);
    }
    else
    {
        memoryEditorPtr->setNumBytesBeingEdited(value);
    }

    // Close the value editor
    cancelAdjustValueEditorValue();
}

void MemoryEditor::dpadControls(MenuButtonInput button)
{
    int32_t editorSelectedIndex = static_cast<int32_t>(this->editorSelectedIndex);
    int32_t editorIndex = static_cast<int32_t>(this->editorIndex);
    uint32_t headerIndex = this->headerIndex;

    const uint32_t numBytesBeingEdited = this->numBytesBeingEdited;
    const MemoryEditorState currentState = this->state;
    uint8_t *currentAddress = this->currentAddress;

    const uint32_t startingHoverDigit = (editorIndex * 2) % MEMORY_EDITOR_DIGITS_PER_ROW;
    const int32_t numDigitsBeingEdited = numBytesBeingEdited * 2;

    switch (button)
    {
        case MenuButtonInput::DPAD_LEFT:
        {
            switch (currentState)
            {
                case MemoryEditorState::MEMORY_EDITOR_STATE_EDITING_BYTES:
                case MemoryEditorState::MEMORY_EDITOR_STATE_SELECTED_BYTES_TO_EDIT:
                {
                    // Only move left if not on the furthest left byte of the current row
                    const bool furthestLeftCheck =
                        ((editorSelectedIndex + startingHoverDigit) % MEMORY_EDITOR_DIGITS_PER_ROW) != 0;

                    // Only move left if not on the first digit
                    const bool firstDigitCheck = editorSelectedIndex > 0;

                    if (furthestLeftCheck && firstDigitCheck)
                    {
                        // Move one to the left
                        editorSelectedIndex--;
                    }
                    else
                    {
                        // Move to the furthest right digit of the current row
                        // Check to see if on the first row
                        if (!firstDigitCheck)
                        {
                            // If there's only enough bytes to make a single row, then go to the last valid digit
                            if (numBytesBeingEdited <= MEMORY_EDITOR_BYTES_PER_ROW)
                            {
                                editorSelectedIndex = numDigitsBeingEdited - 1;
                            }
                            else
                            {
                                // Go to the last digit on the first row
                                editorSelectedIndex = MEMORY_EDITOR_DIGITS_PER_ROW - startingHoverDigit - 1;
                            }
                        }
                        else
                        {
                            // If there's only enough bytes to make a single row, then go to to the previous digit
                            if (numBytesBeingEdited <= MEMORY_EDITOR_BYTES_PER_ROW)
                            {
                                editorSelectedIndex--;
                            }
                            else
                            {
                                // Go to the last digit on the current row
                                editorSelectedIndex += MEMORY_EDITOR_DIGITS_PER_ROW - 1;
                            }
                        }

                        // Make sure the current digit is valid
                        const bool lastDigitCheck = editorSelectedIndex > (numDigitsBeingEdited - 1);
                        if (lastDigitCheck)
                        {
                            editorSelectedIndex = numDigitsBeingEdited - 1;
                        }
                    }

                    this->editorSelectedIndex = static_cast<uint16_t>(editorSelectedIndex);
                    break;
                }
                case MemoryEditorState::MEMORY_EDITOR_STATE_ENTERED_EDITOR:
                {
                    // Check if currently on the furthest left byte of the current row
                    if ((editorIndex % MEMORY_EDITOR_BYTES_PER_ROW) == 0)
                    {
                        // Only move if not on the top row
                        if (editorIndex > 0)
                        {
                            // Move to the last byte in the previous row
                            editorIndex--;
                            this->editorIndex = static_cast<uint8_t>(editorIndex);
                        }
                    }
                    else
                    {
                        // Move one to the left
                        editorIndex--;
                        this->editorIndex = static_cast<uint8_t>(editorIndex);
                    }
                    break;
                }
                case MemoryEditorState::MEMORY_EDITOR_STATE_SELECTING_HEADER:
                {
                    // Only move left if not on the furthest left option
                    if (headerIndex > 0)
                    {
                        // Move one to the left
                        headerIndex--;
                    }
                    else
                    {
                        // Move to the furthest right option
                        headerIndex = MEMORY_EDITOR_HEADER_TOTAL_OPTIONS - 1;
                    }

                    this->headerIndex = static_cast<uint8_t>(headerIndex);
                    break;
                }
                default:
                {
                    break;
                }
            }
            break;
        }
        case MenuButtonInput::DPAD_RIGHT:
        {
            switch (currentState)
            {
                case MemoryEditorState::MEMORY_EDITOR_STATE_EDITING_BYTES:
                case MemoryEditorState::MEMORY_EDITOR_STATE_SELECTED_BYTES_TO_EDIT:
                {
                    editorSelectedIndex++;

                    // Only move right if not on the furthest right byte of the current row
                    const bool furthestRightByteCheck =
                        ((editorSelectedIndex + startingHoverDigit) % MEMORY_EDITOR_DIGITS_PER_ROW) != 0;

                    // Only move right if not on the last digit
                    const bool lastDigitCheck = editorSelectedIndex < numDigitsBeingEdited;

                    if (!furthestRightByteCheck || !lastDigitCheck)
                    {
                        // Check to see if on the first row
                        const int32_t rowCheck = editorSelectedIndex + startingHoverDigit;

                        if (rowCheck <= MEMORY_EDITOR_DIGITS_PER_ROW)
                        {
                            // Go to the first digit if the amount of bytes exceeds a single row
                            // Also go to the first digit if the next digit exceeds the maximum
                            if ((numBytesBeingEdited > MEMORY_EDITOR_BYTES_PER_ROW) ||
                                (editorSelectedIndex > (numDigitsBeingEdited - 1)))
                            {
                                editorSelectedIndex = 0;
                            }
                        }
                        else
                        {
                            // If there's only enough bytes to make a single row, then go to the first digit
                            if (numBytesBeingEdited <= MEMORY_EDITOR_BYTES_PER_ROW)
                            {
                                editorSelectedIndex = 0;
                            }
                            else
                            {
                                // Move to the first digit of the current row
                                // Check to see if on the furthest right digit of the current row
                                if (!furthestRightByteCheck)
                                {
                                    editorSelectedIndex -= MEMORY_EDITOR_DIGITS_PER_ROW;
                                }
                                else
                                {
                                    editorSelectedIndex -=
                                        ((editorSelectedIndex + startingHoverDigit) % MEMORY_EDITOR_DIGITS_PER_ROW);
                                }
                            }
                        }
                    }

                    this->editorSelectedIndex = static_cast<uint16_t>(editorSelectedIndex);
                    break;
                }
                case MemoryEditorState::MEMORY_EDITOR_STATE_ENTERED_EDITOR:
                {
                    editorIndex++;

                    // Only move if not on the bottom row
                    const int32_t rowCheck = (editorIndex + numBytesBeingEdited) - MEMORY_EDITOR_MAX_NUM_BYTES_DISPLAYED;

                    if (rowCheck <= 0)
                    {
                        // Move one to the right
                        this->editorIndex = static_cast<uint8_t>(editorIndex);
                    }
                    break;
                }
                case MemoryEditorState::MEMORY_EDITOR_STATE_SELECTING_HEADER:
                {
                    // Only move right if not on the furthest right option
                    if (headerIndex < (MEMORY_EDITOR_HEADER_TOTAL_OPTIONS - 1))
                    {
                        // Move one to the right
                        headerIndex++;
                    }
                    else
                    {
                        // Move to the furthest left option
                        headerIndex = 0;
                    }

                    this->headerIndex = static_cast<uint8_t>(headerIndex);
                    break;
                }
                default:
                {
                    break;
                }
            }
            break;
        }
        case MenuButtonInput::DPAD_DOWN:
        {
            switch (currentState)
            {
                case MemoryEditorState::MEMORY_EDITOR_STATE_EDITING_BYTES:
                {
                    // Decrement the current digit value
                    adjustDigitValueFromAddress(editorSelectedIndex, -1);
                    break;
                }
                case MemoryEditorState::MEMORY_EDITOR_STATE_SELECTED_BYTES_TO_EDIT:
                {
                    // Move down one row
                    editorSelectedIndex += MEMORY_EDITOR_DIGITS_PER_ROW;

                    // Check if there's a row under the current one
                    const int32_t rowCheck = numDigitsBeingEdited - editorSelectedIndex;
                    if (rowCheck <= 0)
                    {
                        // Loop backwards through the rows until the first invalid digit is reached
                        do
                        {
                            editorSelectedIndex -= MEMORY_EDITOR_DIGITS_PER_ROW;
                        } while (editorSelectedIndex >= 0);

                        // Go back to the first valid digit
                        editorSelectedIndex += MEMORY_EDITOR_DIGITS_PER_ROW;
                    }

                    this->editorSelectedIndex = static_cast<uint16_t>(editorSelectedIndex);
                    break;
                }
                case MemoryEditorState::MEMORY_EDITOR_STATE_ENTERED_EDITOR:
                {
                    editorIndex += MEMORY_EDITOR_BYTES_PER_ROW;

                    // Check to see if moving down goes to the next row
                    const bool rowCheck = (editorIndex + numBytesBeingEdited) > MEMORY_EDITOR_MAX_NUM_BYTES_DISPLAYED;

                    if (rowCheck)
                    {
                        // Move down one row
                        // Don't set editorIndex, as it's already correct
                        this->currentAddress = currentAddress + MEMORY_EDITOR_BYTES_PER_ROW;
                    }
                    else
                    {
                        // Move down one row
                        this->editorIndex = static_cast<uint8_t>(editorIndex);
                    }
                    break;
                }
                case MemoryEditorState::MEMORY_EDITOR_STATE_SELECTING_HEADER:
                default:
                {
                    break;
                }
            }
            break;
        }
        case MenuButtonInput::DPAD_UP:
        {
            switch (currentState)
            {
                case MemoryEditorState::MEMORY_EDITOR_STATE_EDITING_BYTES:
                {
                    // Decrement the current digit value
                    adjustDigitValueFromAddress(editorSelectedIndex, 1);
                    break;
                }
                case MemoryEditorState::MEMORY_EDITOR_STATE_SELECTED_BYTES_TO_EDIT:
                {
                    // Move up one row
                    editorSelectedIndex -= MEMORY_EDITOR_DIGITS_PER_ROW;

                    // Check if there's a row above the current one
                    if (editorSelectedIndex < 0)
                    {
                        // Loop forwards through the rows until the first invalid digit is reached
                        do
                        {
                            editorSelectedIndex += MEMORY_EDITOR_DIGITS_PER_ROW;
                        } while (editorSelectedIndex <= (numDigitsBeingEdited - 1));

                        // Go back to the first valid digit
                        editorSelectedIndex -= MEMORY_EDITOR_DIGITS_PER_ROW;
                    }

                    this->editorSelectedIndex = static_cast<uint16_t>(editorSelectedIndex);
                    break;
                }
                case MemoryEditorState::MEMORY_EDITOR_STATE_ENTERED_EDITOR:
                {
                    editorIndex -= MEMORY_EDITOR_BYTES_PER_ROW;

                    // Check to see if moving up goes to the previous row
                    if (editorIndex < 0)
                    {
                        // Move up one row
                        // Don't set editorIndex, as it's already correct
                        this->currentAddress = currentAddress - MEMORY_EDITOR_BYTES_PER_ROW;
                    }
                    else
                    {
                        // Move up one row
                        this->editorIndex = static_cast<uint8_t>(editorIndex);
                    }
                    break;
                }
                case MemoryEditorState::MEMORY_EDITOR_STATE_SELECTING_HEADER:
                default:
                {
                    break;
                }
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

void MemoryEditor::controls(MenuButtonInput button)
{
    // If the value editor is open, then handle the controls for that
    ValueEditor *valueEditorPtr = this->valueEditor;
    if (valueEditor)
    {
        valueEditorPtr->controls(button);
        return;
    }

    // The function for checking for auto-incrementing needs to run every frame to be handled correctly
    const bool autoIncrement = handleMenuAutoIncrement(&this->autoIncrement);

    // Handle held button input if auto-incrementing should be done
    if (autoIncrement)
    {
        const MenuButtonInput buttonHeld = getMenuButtonInput(false);
        switch (buttonHeld)
        {
            case MenuButtonInput::DPAD_LEFT:
            case MenuButtonInput::DPAD_RIGHT:
            case MenuButtonInput::DPAD_DOWN:
            case MenuButtonInput::DPAD_UP:
            {
                this->dpadControls(buttonHeld);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    // Handle the button input pressed this frame
    const uint32_t editorIndex = this->editorIndex;
    const MemoryEditorState currentState = this->state;
    const uint32_t numBytesBeingEdited = this->numBytesBeingEdited;
    uint8_t *currentAddress = this->currentAddress;
    uint8_t *bytesBeingEdited = this->bytesBeingEdited;

    switch (button)
    {
        case MenuButtonInput::DPAD_LEFT:
        case MenuButtonInput::DPAD_RIGHT:
        case MenuButtonInput::DPAD_DOWN:
        case MenuButtonInput::DPAD_UP:
        {
            this->dpadControls(button);
            break;
        }
        case MenuButtonInput::A:
        {
            switch (currentState)
            {
                case MemoryEditorState::MEMORY_EDITOR_STATE_EDITING_BYTES:
                {
                    // Copy the bytes from the amount of bytes to change into memory
                    // Only copy bytes that are at a valid address
                    uint8_t *tempAddress = currentAddress + editorIndex;

                    for (uint32_t i = 0; i < numBytesBeingEdited; i++)
                    {
                        uint8_t *address = &tempAddress[i];

                        if (ptrIsValid(address))
                        {
                            *address = bytesBeingEdited[i];
                        }
                    }

                    // Clear the cache of the memory if the flag is set
                    if (this->enabledFlagIsSet(MemoryEditorEnabledFlag::MEMORY_EDITOR_ENABLED_FLAG_CLEAR_CACHE))
                    {
                        // Only clear the cache of valid memory
                        uint32_t startAddressRaw = reinterpret_cast<uint32_t>(tempAddress);
                        uint32_t endAddressRaw = startAddressRaw + numBytesBeingEdited;

                        // Arbitrary check for cached memory
                        if (ptrIsValid(startAddressRaw) == PointerVerificationType::PTR_UNCACHED)
                        {
                            // Only need to clear cached memory, so adjust the start and end to be in cached memory
                            startAddressRaw -= 0x40000000;
                            endAddressRaw -= 0x40000000;
                        }

                        if (startAddressRaw < 0x80000000)
                        {
                            startAddressRaw = 0x80000000;
                        }

                        if (endAddressRaw > 0x81800000)
                        {
                            endAddressRaw = 0x81800000;
                        }

                        const uint32_t size = endAddressRaw - startAddressRaw;
                        clear_DC_IC_Cache(startAddressRaw, size);
                    }

                    // Go back to selecting a range of bytes to edit
                    this->state = MemoryEditorState::MEMORY_EDITOR_STATE_ENTERED_EDITOR;
                    break;
                }
                case MemoryEditorState::MEMORY_EDITOR_STATE_SELECTED_BYTES_TO_EDIT:
                {
                    // Start editing bytes
                    this->state = MemoryEditorState::MEMORY_EDITOR_STATE_EDITING_BYTES;
                    break;
                }
                case MemoryEditorState::MEMORY_EDITOR_STATE_ENTERED_EDITOR:
                {
                    // Clear the array for the amount of bytes to change
                    clearMemory(bytesBeingEdited, numBytesBeingEdited);

                    // Copy the bytes from memory into the array for the amount of bytes to change
                    // Make sure there's at least one valid byte to copy
                    uint8_t *tempAddress = currentAddress + editorIndex;
                    bool validBytesCopied = false;

                    for (uint32_t i = 0; i < numBytesBeingEdited; i++)
                    {
                        uint8_t *address = &tempAddress[i];

                        if (ptrIsValid(address))
                        {
                            bytesBeingEdited[i] = *address;
                            validBytesCopied = true;
                        }
                    }

                    // If at least one valid byte was copied, then allow selecting digits to edit
                    if (validBytesCopied)
                    {
                        this->editorSelectedIndex = 0;
                        this->state = MemoryEditorState::MEMORY_EDITOR_STATE_SELECTED_BYTES_TO_EDIT;
                    }
                    break;
                }
                case MemoryEditorState::MEMORY_EDITOR_STATE_SELECTING_HEADER:
                {
                    const uint32_t headerIndex = this->headerIndex;
                    switch (headerIndex)
                    {
                        case MemoryEditorOptions::MEMORY_EDITOR_OPTION_CHANGE_ADDRESS:
                        case MemoryEditorOptions::MEMORY_EDITOR_OPTION_CHANGE_BYTES_SIZE:
                        {
                            // Initialize the value editor
                            valueEditorPtr = new ValueEditor;
                            this->valueEditor = valueEditorPtr;

                            constexpr uint32_t minValue = 1;
                            constexpr uint32_t maxValue = MEMORY_EDITOR_MAX_NUM_BYTES_DISPLAYED;

                            uint32_t currentValue;
                            const uint32_t *minValuePtr;
                            const uint32_t *maxValuePtr;
                            VariableType type;

                            uint32_t flags = 0;
                            flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);

                            if (headerIndex == MemoryEditorOptions::MEMORY_EDITOR_OPTION_CHANGE_ADDRESS)
                            {
                                flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::HANDLE_AS_HEX);

                                currentValue = reinterpret_cast<uint32_t>(currentAddress);
                                minValuePtr = nullptr;
                                maxValuePtr = nullptr;
                                type = VariableType::u32;
                            }
                            else
                            {
                                flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Y_SET_MAX);
                                flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Z_SET_MIN);

                                currentValue = numBytesBeingEdited;
                                minValuePtr = &minValue;
                                maxValuePtr = &maxValue;
                                type = VariableType::u8;
                            }

                            const Window *rootWindowPtr = gRootWindow;

                            valueEditorPtr->init(&currentValue,
                                                 minValuePtr,
                                                 maxValuePtr,
                                                 rootWindowPtr,
                                                 flags,
                                                 type,
                                                 rootWindowPtr->getAlpha());

                            valueEditorPtr->startDrawing(adjustValueEditorValue, cancelAdjustValueEditorValue);
                            break;
                        }
                        case MemoryEditorOptions::MEMORY_EDITOR_OPTION_ENTER_EDITOR:
                        {
                            // If memory was previously allocated for the bytes that are being changed, then free it
                            if (bytesBeingEdited)
                            {
                                delete[] bytesBeingEdited;
                            }

                            // Allocate memory for the bytes that are being changed
                            this->bytesBeingEdited = new uint8_t[numBytesBeingEdited];
                            this->editorIndex = 0;

                            // Open the editor
                            this->state = MemoryEditorState::MEMORY_EDITOR_STATE_ENTERED_EDITOR;
                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }
                    break;
                }
                default:
                {
                    break;
                }
            }
            break;
        }
        case MenuButtonInput::B:
        {
            switch (currentState)
            {
                case MemoryEditorState::MEMORY_EDITOR_STATE_EDITING_BYTES:
                {
                    // Go back to navigating through the selected bytes
                    this->state = MemoryEditorState::MEMORY_EDITOR_STATE_SELECTED_BYTES_TO_EDIT;
                    break;
                }
                case MemoryEditorState::MEMORY_EDITOR_STATE_SELECTED_BYTES_TO_EDIT:
                {
                    // Clear the memory for the amount of bytes to change
                    clearMemory(bytesBeingEdited, numBytesBeingEdited);

                    // Go back to selecting a range to modify
                    this->state = MemoryEditorState::MEMORY_EDITOR_STATE_ENTERED_EDITOR;
                    break;
                }
                case MemoryEditorState::MEMORY_EDITOR_STATE_ENTERED_EDITOR:
                {
                    // Clear the memory for the amount of bytes to change
                    if (bytesBeingEdited)
                    {
                        delete[] bytesBeingEdited;
                        this->bytesBeingEdited = nullptr;
                    }

                    // Go back to selecting a header option
                    this->state = MemoryEditorState::MEMORY_EDITOR_STATE_SELECTING_HEADER;
                    break;
                }
                case MemoryEditorState::MEMORY_EDITOR_STATE_SELECTING_HEADER:
                {
                    // Reset the System Level if it was previously set
                    if (this->enabledFlagIsSet(MemoryEditorEnabledFlag::MEMORY_EDITOR_ENABLED_FLAG_SET_SYSTEM_LEVEL))
                    {
                        setSystemLevel(0);
                    }

                    // Enable the pause menu if it was disabled
                    if (this->enabledFlagIsSet(MemoryEditorEnabledFlag::MEMORY_EDITOR_ENABLED_FLAG_DISABLE_PAUSE_MENU))
                    {
                        winOpenEnable();
                    }

                    // Close the memory editor
                    this->headerIndex = 0;
                    this->hideMemoryEditor();
                    break;
                }
                default:
                {
                    break;
                }
            }
            break;
        }
        case MenuButtonInput::Y:
        {
            // Open the Settings menu
            return memoryMenuMemoryEditorSettingsInit(this->menuPtr);
        }
        default:
        {
            break;
        }
    }
}

void MemoryEditor::draw() const
{
    // Get the text position for the top-left of the window
    const Window *windowPtr = gRootWindow;
    float scale = MENU_SCALE;

#ifdef TTYD_JP
    scale += 0.05f;
#endif

    float initialPosX;
    float initialPosY;
    windowPtr->getTextPosXY(nullptr, WindowAlignment::TOP_LEFT, scale, &initialPosX, &initialPosY);

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    float posX = initialPosX;
    float posY = initialPosY;

    constexpr float lineDecrement = 20.f;
    const float mainEditorTextPosX = posX + 103.f;
    const float mainEditorTextPosY = posY - (lineDecrement * 4.f);

    constexpr float digitSpace = 12.f;
    constexpr float twoDigitSpace = digitSpace * 2.f;
    constexpr float spaceBetweenEachNum = digitSpace / 3.f; // Each number is made using 2 digits

    // Draw the vertical and/or horizontal lines
    const uint32_t windowColor = getColorWhite(0xFF);
    constexpr float windowPosXAdjustment = twoDigitSpace + spaceBetweenEachNum;

    if (this->enabledFlagIsSet(MemoryEditorEnabledFlag::MEMORY_EDITOR_ENABLED_FLAG_ENABLE_VERITCAL_LINES))
    {
        // Draw a line between each byte
        float windowPosX = mainEditorTextPosX + twoDigitSpace + 0.8f;

        for (uint32_t i = 0; i < (MEMORY_EDITOR_BYTES_PER_ROW - 1); i++, windowPosX += windowPosXAdjustment)
        {
            windowDispGX_Waku_col(0,
                                  reinterpret_cast<const uint8_t *>(&windowColor),
                                  windowPosX,
                                  mainEditorTextPosY,
                                  2.f,
                                  275.f,
                                  0.f);
        }
    }

    if (this->enabledFlagIsSet(MemoryEditorEnabledFlag::MEMORY_EDITOR_ENABLED_FLAG_ENABLE_HORIZONTAL_LINES))
    {
        // Draw a line between each row
        float windowPosX = posX - spaceBetweenEachNum;
        float windowPosY = mainEditorTextPosY - ((lineDecrement * 2.f) - 4.f);
        float windowWidth = 470.f;

#ifdef TTYD_JP
        windowPosX -= 1.f;
        windowWidth += 1.f;
#endif
        for (uint32_t i = 0; i < (MEMORY_EDITOR_BYTES_PER_ROW - 1); i++, windowPosY -= lineDecrement)
        {
            windowDispGX_Waku_col(0,
                                  reinterpret_cast<const uint8_t *>(&windowColor),
                                  windowPosX,
                                  windowPosY,
                                  windowWidth,
                                  2.f,
                                  0.f);
        }
    }

    // Initialize text drawing
    drawTextInit(false);

    // Draw the text stating to press Y to open the options
    const char *text = "Press Y to open the settings menu";
    posX = windowPtr->getTextPosX(text, WindowAlignment::TOP_CENTER, scale);
    drawText(text, posX, posY, scale, getColorWhite(0xFF));
    posY -= lineDecrement;

    // Draw the text for the options
    char buf[32];
    constexpr uint32_t bufSize = sizeof(buf);

    uint8_t *currentAddress = this->currentAddress;
    const uint32_t currentAddressRaw = reinterpret_cast<uint32_t>(currentAddress);
    const uint32_t headerIndex = this->headerIndex;
    const char *string;
    uint32_t color;

    for (uint32_t i = 0; i < MEMORY_EDITOR_HEADER_TOTAL_OPTIONS; i++)
    {
        switch (i)
        {
            case MemoryEditorOptions::MEMORY_EDITOR_OPTION_CHANGE_ADDRESS:
            {
                // Draw the value for the header
                snprintf(buf, bufSize, "0x%08" PRIX32, currentAddressRaw);
                posX = windowPtr->getTextPosX(nullptr, WindowAlignment::TOP_LEFT, scale);
                drawText(buf, posX, posY - lineDecrement, scale, getColorWhite(0xFF));
                string = "Address";
                break;
            }
            case MemoryEditorOptions::MEMORY_EDITOR_OPTION_CHANGE_BYTES_SIZE:
            {
                // Draw the value for the header
                snprintf(buf, bufSize, "%" PRIu32, this->numBytesBeingEdited);
                posX = windowPtr->getTextPosX(buf, WindowAlignment::TOP_CENTER, scale);
                drawText(buf, posX, posY - lineDecrement, scale, getColorWhite(0xFF));

                // Get the proper position for the main text
                string = "Bytes Size";
                posX = windowPtr->getTextPosX(string, WindowAlignment::TOP_CENTER, scale);
                break;
            }
            case MemoryEditorOptions::MEMORY_EDITOR_OPTION_ENTER_EDITOR:
            {
                // Get the proper position for the main text
                string = "Edit Bytes";
                posX = windowPtr->getTextPosX(string, WindowAlignment::TOP_RIGHT, scale);
                break;
            }
            default:
            {
                break;
            }
        }

        // Draw the main text for the header
        color = getCurrentOptionColor(headerIndex == i, 0xFF);
        drawText(string, posX, posY, scale, color);
    }

    // Draw the current addresses displayed in the editor
    posX = initialPosX;
    posY = mainEditorTextPosY - lineDecrement;

    for (uint32_t i = 0; i < MEMORY_EDITOR_TOTAL_ROWS; i++)
    {
        snprintf(buf, bufSize, "%08" PRIX32, currentAddressRaw + (MEMORY_EDITOR_BYTES_PER_ROW * i));
        drawText(buf, posX, posY, scale, getColorWhite(0xFF));
        posY -= lineDecrement;
    }

    // Draw the headers numbers indicating each byte in the current line in the editor
    posX = mainEditorTextPosX;
    posY = mainEditorTextPosY;

    // Start at the last digit of the current address
    uint32_t headerNum = currentAddressRaw & 0xF;
    constexpr float headerNumPosXAdjustment = twoDigitSpace + spaceBetweenEachNum;

    for (uint32_t i = 0; i < MEMORY_EDITOR_BYTES_PER_ROW; i++)
    {
        snprintf(buf, bufSize, "%" PRIX32, headerNum);
        drawText(buf, posX, posY, scale, twoDigitSpace, getColorWhite(0xFF), TextAlignment::CENTER);
        posX += headerNumPosXAdjustment;

        // Increment and handle looping
        if (++headerNum > 0xF)
        {
            headerNum = 0;
        }
    }

    // Draw the bytes displayed in the editor
    const MemoryEditorState currentState = this->state;
    uint32_t selectedBytesCurrentDigit = 0;

    for (uint32_t i = 0; i < MEMORY_EDITOR_MAX_NUM_DIGITS_DISPLAYED; i++)
    {
        if ((i % MEMORY_EDITOR_DIGITS_PER_ROW) == 0)
        {
            // Go to the next line
            // This also handles setting the first line
            posX = mainEditorTextPosX;
            posY -= lineDecrement;
        }

        // Set the default color to white
        uint32_t color = getColorWhite(0xFF);

        // Get the address of the current digit
        uint8_t *digitAddress = getDigitAddress(currentAddress, MEMORY_EDITOR_MAX_NUM_BYTES_DISPLAYED, i);

        // Check which digit to get
        int32_t currentDigit;

        // Check to see if currently hovering over addresses
        if (currentState >= MemoryEditorState::MEMORY_EDITOR_STATE_ENTERED_EDITOR)
        {
            // Check if the current address is part of the hover address
            if (this->checkIfAddressInHoverRange(digitAddress, selectedBytesCurrentDigit))
            {
                // Check if the current digit address is valid
                const bool digitAddressIsValid = ptrIsValid(const_cast<uint8_t *>(digitAddress));

                // Check to see if an address has been selected
                if (currentState >= MemoryEditorState::MEMORY_EDITOR_STATE_SELECTED_BYTES_TO_EDIT)
                {
                    // Check if the current digit is being hovered over
                    if (this->editorSelectedIndex == selectedBytesCurrentDigit)
                    {
                        // Draw a window under the current digit
                        const uint32_t windowColor = 0x1C4F57FF;

                        windowDispGX_Waku_col(0,
                                              reinterpret_cast<const uint8_t *>(&windowColor),
                                              posX,
                                              posY,
                                              digitSpace,
                                              digitSpace + 3.f,
                                              0.f);

                        // Re-initialize text drawing
                        drawTextInit(false);

                        // Set the text color
                        if (currentState >= MemoryEditorState::MEMORY_EDITOR_STATE_EDITING_BYTES)
                        {
                            // Make the text orange
                            color = 0xFFA31AFF;

                            if (!digitAddressIsValid)
                            {
                                // Digit address is invalid, so set the digit to an invalid number
                                currentDigit = -1;
                            }
                            else
                            {
                                // Get the current digit of the edited bytes
                                currentDigit = getDigitValueFromAddress(this->bytesBeingEdited,
                                                                        this->numBytesBeingEdited,
                                                                        selectedBytesCurrentDigit);
                            }
                        }
                        else if (!digitAddressIsValid)
                        {
                            // Digit address is invalid, so make the text green
                            color = 0x2EE866FF;

                            // Set the digit to an invalid number
                            currentDigit = -1;
                        }
                        else
                        {
                            // Make the text green
                            color = 0x2EE866FF;

                            // Get the current digit of the edited bytes
                            currentDigit = getDigitValueFromAddress(this->bytesBeingEdited,
                                                                    this->numBytesBeingEdited,
                                                                    selectedBytesCurrentDigit);
                        }
                    }
                    else if (!digitAddressIsValid)
                    {
                        // Digit address is invalid, so make the text red
                        color = getColorRed(0xFF);

                        // Set the digit to an invalid number
                        currentDigit = -1;
                    }
                    else
                    {
                        // Make the text blue
                        color = getColorBlue(0xFF);

                        // Get the current digit of the edited bytes
                        currentDigit = getDigitValueFromAddress(this->bytesBeingEdited,
                                                                this->numBytesBeingEdited,
                                                                selectedBytesCurrentDigit);
                    }
                }
                else
                {
                    // Set the text color
                    if (!digitAddressIsValid)
                    {
                        // Digit address is invalid, so make the text red
                        color = getColorRed(0xFF);

                        // Set the digit to an invalid number
                        currentDigit = -1;
                    }
                    else
                    {
                        // Make the text blue
                        color = getColorBlue(0xFF);

                        // Get the current digit of the current address
                        currentDigit = getDigitValue(digitAddress, MEMORY_EDITOR_MAX_NUM_BYTES_DISPLAYED, i);
                    }
                }

                selectedBytesCurrentDigit++;
            }
            else
            {
                // Get the current digit of the current address
                currentDigit = getDigitValue(digitAddress, MEMORY_EDITOR_MAX_NUM_BYTES_DISPLAYED, i);
            }
        }
        else
        {
            // Get the current digit of the current address
            currentDigit = getDigitValue(digitAddress, MEMORY_EDITOR_MAX_NUM_BYTES_DISPLAYED, i);
        }

        // Draw the current digit
        if ((currentDigit < 0) || (currentDigit > 0xF))
        {
            // Digit is invalid, so draw a ?
            drawText("?", posX, posY, scale, digitSpace, color, TextAlignment::CENTER);
        }
        else
        {
            // Digit is valid
            snprintf(buf, bufSize, "%" PRIX32, currentDigit);
            drawText(buf, posX, posY, scale, digitSpace, color, TextAlignment::CENTER);
        }

        // Add extra space between every 2 digits
        if ((i % 2) == 1)
        {
            posX += spaceBetweenEachNum;
        }

        posX += digitSpace;
    }
}

MemoryEditor::MemoryEditor()
{
    this->currentAddress = reinterpret_cast<uint8_t *>(0x80000000);
    this->buttonCombo = PadInput::PAD_R | PadInput::PAD_Z;
    this->numBytesBeingEdited = 1;

    this->setEnabledFlag(MemoryEditorEnabledFlag::MEMORY_EDITOR_ENABLED_FLAG_CLEAR_CACHE);
    this->setEnabledFlag(MemoryEditorEnabledFlag::MEMORY_EDITOR_ENABLED_FLAG_SET_SYSTEM_LEVEL);
    this->setEnabledFlag(MemoryEditorEnabledFlag::MEMORY_EDITOR_ENABLED_FLAG_DISABLE_PAUSE_MENU);
}

static void draw(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Draw the main window
    drawMainWindow();

    // Draw the main stuff for the memory editor
    MemoryEditor *memoryEditorPtr = gMemoryEditor;
    memoryEditorPtr->draw();

    // Draw the valie editor if applicable
    ValueEditor *valueEditorPtr = memoryEditorPtr->getValueEditorPtr();
    if (valueEditorPtr)
    {
        valueEditorPtr->draw();
    }
}

bool memoryEditorIsOpen()
{
    // Make sure the memory editor is enabled
    const MemoryEditor *memoryEditorPtr = gMemoryEditor;
    if (!memoryEditorPtr->enabledFlagIsSet(MemoryEditorEnabledFlag::MEMORY_EDITOR_ENABLED_FLAG_MEMORY_EDITOR_ENABLED))
    {
        return false;
    }

    return memoryEditorPtr->memoryEditorIsDisplayed();
}

void handleMemoryEditor()
{
    // Make sure the memory editor is enabled
    MemoryEditor *memoryEditorPtr = gMemoryEditor;
    if (!memoryEditorPtr->enabledFlagIsSet(MemoryEditorEnabledFlag::MEMORY_EDITOR_ENABLED_FLAG_MEMORY_EDITOR_ENABLED))
    {
        return;
    }

    // Do not run if the menu is open
    // Do not run if changing button combos
    // Do not run if currently in the process of spawning an item
    if (gMenu || gMod->flagIsSet(ModFlag::MOD_FLAG_CHANGING_BUTTON_COMBO) ||
        gCheats->getSpawnItemCheatPtr()->getValueEditorPtr())
    {
        return;
    }

    // If the memory editor is not currently open, then check for and handle the button combo for opening it
    if (!memoryEditorPtr->memoryEditorIsDisplayed())
    {
        if (checkButtonCombo(memoryEditorPtr->getButtonCombo()))
        {
            // Open the memory editor
            memoryEditorPtr->displayMemoryEditor();

            // Set the System Level if it should be set
            if (memoryEditorPtr->enabledFlagIsSet(MemoryEditorEnabledFlag::MEMORY_EDITOR_ENABLED_FLAG_SET_SYSTEM_LEVEL))
            {
                setSystemLevel(1);
            }

            // Disable the pause menu if it should be disabled
            if (memoryEditorPtr->enabledFlagIsSet(MemoryEditorEnabledFlag::MEMORY_EDITOR_ENABLED_FLAG_DISABLE_PAUSE_MENU))
            {
                winOpenDisable();
            }
        }

        // Do not accept inputs on the frame that the memory editor was opened
        return;
    }

    const MenuButtonInput button = getMenuButtonInput(true);

    // If the Settings menu is open, then handle the controls for that
    Menu *settingsMenuPtr = memoryEditorPtr->getMenuPtr();
    if (settingsMenuPtr)
    {
        // Handle the controls for the Settings menu
        settingsMenuPtr->runControlsFunc(button);

        // Draw the Settings menu
        // The menu pointer may have been modified, so have to re-get it
        settingsMenuPtr = memoryEditorPtr->getMenuPtr();
        if (settingsMenuPtr)
        {
            settingsMenuPtr->runDrawFunc();
        }
        else
        {
            // Draw the memory editor
            drawOnDebugLayer(draw, DRAW_ORDER_MENU);
        }
    }
    else
    {
        // Handle the controls for the memory editor
        memoryEditorPtr->controls(button);

        // Draw the memory editor
        drawOnDebugLayer(draw, DRAW_ORDER_MENU);
    }
}
