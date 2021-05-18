#include "memory.h"
#include "global.h"
#include "commonfunctions.h"
#include "menufunctions.h"
#include "patch.h"

#include <ttyd/system.h>
#include <ttyd/win_main.h>

#include <cstdio>
#include <cinttypes>
#include <cstring>

namespace mod {

// Memory Watch stuff
int32_t getEmptyWatchSlot()
{
    int32_t Size = sizeof(MemoryWatch) / sizeof(MemoryWatch[0]);
    for (int32_t i = 0; i < Size; i++)
    {
        if (!MemoryWatch[i].Address)
        {
            return i;
        }
    }
    return NO_SLOTS_LEFT;
}

const char *getAddressStringNoLetterP(int32_t slot)
{
    char *tempDisplayBuffer = DisplayBuffer;
    sprintf(tempDisplayBuffer,
        "0x%08" PRIX32,
        MemoryWatch[slot].Address);
    
    return tempDisplayBuffer;
}

const char *getAddressString(int32_t slot)
{
    // Check if the current address uses pointers or not
    const char *tempPointerText;
    if (MemoryWatch[slot].AddressOffsetAmount > 0)
    {
#ifdef TTYD_JP
        tempPointerText = PointerText;
#else
        tempPointerText = "P=";
#endif
    }
    else
    {
        tempPointerText = "";
    }
    
    char *tempDisplayBuffer = DisplayBuffer;
    sprintf(tempDisplayBuffer,
        "%s0x%08" PRIX32,
        tempPointerText,
        MemoryWatch[slot].Address);
    
    return tempDisplayBuffer;
}

const char *getAddressStringFromOffsets(int32_t slot, uint32_t maxOffset)
{
    int32_t OffsetArray[maxOffset];
    for (uint32_t i = 0; i < maxOffset; i++)
    {
        OffsetArray[i] = MemoryWatch[slot].AddressOffset[i];
    }
    
    uint32_t Address = reinterpret_cast<uint32_t>(
        getLastPointerFromPath(reinterpret_cast<void *>(
            MemoryWatch[slot].Address), OffsetArray, maxOffset));
    
    // Make sure the address is valid
    if (!checkIfPointerIsValid(reinterpret_cast<void *>(Address)))
    {
        // Address is invalid
        return "???";
    }
    
    // Make sure the address being read does not exceed 0x817FFFFF
    Address = reinterpret_cast<uint32_t>(fixBaseAddress(
        slot, reinterpret_cast<void *>(Address)));
    
    char *tempDisplayBuffer = DisplayBuffer;
    sprintf(tempDisplayBuffer,
        "0x%08" PRIX32,
        Address);
    
    return tempDisplayBuffer;
}

const char *getValueString(int32_t slot)
{
    uint32_t Address = reinterpret_cast<uint32_t>(
        getLastPointerFromPath(reinterpret_cast<void *>(MemoryWatch[slot].Address), 
            MemoryWatch[slot].AddressOffset, MemoryWatch[slot].AddressOffsetAmount));
    
    // Make sure the address is valid
    if (!checkIfPointerIsValid(reinterpret_cast<void *>(Address)))
    {
        // Address is invalid
        return "???";
    }
    
    // Make sure the address being read does not exceed 0x817FFFFF
    Address = reinterpret_cast<uint32_t>(fixBaseAddress(
        slot, reinterpret_cast<void *>(Address)));
    
    bool ShowAsHex = MemoryWatch[slot].ShowAsHex;
    char *tempDisplayBuffer = DisplayBuffer;
    
    const char *Format;
    switch (MemoryWatch[slot].Type)
    {
        case string:
        {
            return reinterpret_cast<const char *>(Address);
        }
        case time:
        {
            int64_t CurrentTime = *reinterpret_cast<int64_t *>(Address);
            return getTimeString(tempDisplayBuffer, CurrentTime);
        }
        case s8:
        {
            int8_t Value = *reinterpret_cast<int8_t *>(Address);
            if (ShowAsHex)
            {
                Format = "0x%02" PRIX8;
            }
            else
            {
                Format = "%" PRId8;
            }
            
            sprintf(tempDisplayBuffer,
                Format,
                Value);
            
            return tempDisplayBuffer;
        }
        case s16:
        {
            int16_t Value = *reinterpret_cast<int16_t *>(Address);
            if (ShowAsHex)
            {
                Format = "0x%04" PRIX16;
            }
            else
            {
                Format = "%" PRId16;
            }
            
            sprintf(tempDisplayBuffer,
                Format,
                Value);
            
            return tempDisplayBuffer;
        }
        case s32:
        {
            int32_t Value = *reinterpret_cast<int32_t *>(Address);
            if (ShowAsHex)
            {
                Format = "0x%08" PRIX32;
            }
            else
            {
                Format = "%" PRId32;
            }
            
            sprintf(tempDisplayBuffer,
                Format,
                Value);
            
            return tempDisplayBuffer;
        }
        case s64:
        {
            int64_t Value = *reinterpret_cast<int64_t *>(Address);
            if (ShowAsHex)
            {
                Format = "0x%016" PRIX64;
            }
            else
            {
                Format = "%" PRId64;
            }
            
            sprintf(tempDisplayBuffer,
                Format,
                Value);
            
            return tempDisplayBuffer;
        }
        case u8:
        {
            uint8_t Value = *reinterpret_cast<uint8_t *>(Address);
            if (ShowAsHex)
            {
                Format = "0x%02" PRIX8;
            }
            else
            {
                Format = "%" PRIu8;
            }
            
            sprintf(tempDisplayBuffer,
                Format,
                Value);
            
            return tempDisplayBuffer;
        }
        case u16:
        {
            uint16_t Value = *reinterpret_cast<uint16_t *>(Address);
            if (ShowAsHex)
            {
                Format = "0x%04" PRIX16;
            }
            else
            {
                Format = "%" PRIu16;
            }
            
            sprintf(tempDisplayBuffer,
                Format,
                Value);
            
            return tempDisplayBuffer;
        }
        case u32:
        {
            uint32_t Value = *reinterpret_cast<uint32_t *>(Address);
            if (ShowAsHex)
            {
                Format = "0x%08" PRIX32;
            }
            else
            {
                Format = "%" PRIu32;
            }
            
            sprintf(tempDisplayBuffer,
                Format,
                Value);
            
            return tempDisplayBuffer;
        }
        case u64:
        {
            uint64_t Value = *reinterpret_cast<uint64_t *>(Address);
            if (ShowAsHex)
            {
                Format = "0x%016" PRIX64;
            }
            else
            {
                Format = "%" PRIu64;
            }
            
            sprintf(tempDisplayBuffer,
                Format,
                Value);
            
            return tempDisplayBuffer;
        }
        case f32:
        {
            if (ShowAsHex)
            {
                sprintf(tempDisplayBuffer,
                    "0x%08" PRIX32,
                    *reinterpret_cast<uint32_t *>(Address));
            }
            else
            {
                sprintf(tempDisplayBuffer,
                    "%.2f",
                    *reinterpret_cast<float *>(Address));
            }
            return tempDisplayBuffer;
        }
        case f64:
        {
            if (ShowAsHex)
            {
                sprintf(tempDisplayBuffer,
                    "0x%016" PRIX64,
                    *reinterpret_cast<uint64_t *>(Address));
            }
            else
            {
                sprintf(tempDisplayBuffer,
                    "%.2f",
                    *reinterpret_cast<double *>(Address));
            }
            return tempDisplayBuffer;
        }
        default:
        {
            return "";
        }
    }
}

void addMemoryWatch(int32_t slot)
{
    MemoryWatch[slot].Address   = 0x80000000;
    MemoryWatch[slot].Type      = string;
    MemoryWatch[slot].ShowAsHex = false;
    MemoryWatch[slot].Display   = false;
    MemoryWatch[slot].PosX      = -44;
    MemoryWatch[slot].PosY      = -32;
}

void deleteWatch(int32_t slot)
{
    uint32_t TotalOptionsPerPage = 10;
    int32_t TotalSlots = sizeof(MemoryWatch) / sizeof(MemoryWatch[0]);
    int32_t TotalMenuOptions = 0;
    
    for (int32_t i = 0; i < TotalSlots; i++)
    {
        if (!MemoryWatch[i].Address)
        {
            break;
        }
        TotalMenuOptions++;
    }
    
    // Adjust for 0 index
    TotalMenuOptions--;
    
    if (slot < TotalMenuOptions)
    {
        uint32_t Size = sizeof(MemoryWatch[0]) * (TotalMenuOptions - slot);
        memcpy(&MemoryWatch[slot], &MemoryWatch[slot + 1], Size);
    }
    
    clearMemory(&MemoryWatch[TotalMenuOptions], sizeof(MemoryWatch[0]));
    TotalMenuOptions--;
    
    // Check if there are any watches left
    if (TotalMenuOptions < 0)
    {
        closeSecondaryMenu();
        return;
    }
    
    // Make sure the currently selected option is valid
    uint32_t tempCurrentMenuOption = MenuVar.CurrentMenuOption;
    if (tempCurrentMenuOption > static_cast<uint32_t>(TotalMenuOptions))
    {
        MenuVar.CurrentMenuOption = tempCurrentMenuOption - 1;
    }
    
    // Make sure the current page is valid
    uint32_t tempCurrentPage = MenuVar.CurrentPage;
    if ((tempCurrentPage * TotalOptionsPerPage) > static_cast<uint32_t>(TotalMenuOptions))
    {
        MenuVar.CurrentPage = tempCurrentPage - 1;
    }
}

void duplicateWatch(int32_t currentSlot, int32_t emptySlot)
{
    uint32_t Size = sizeof(MemoryWatch[0]);
    memcpy(&MemoryWatch[emptySlot], &MemoryWatch[currentSlot], Size);
}

uint32_t adjustWatchValueControls(int32_t slot)
{
    uint32_t tempCurrentMenu       = MenuVar.CurrentMenu;
    uint32_t tempCurrentMenuOption = MenuVar.CurrentMenuOption;
    
    // Get the amount of numbers to draw
    bool NoNumbersToDisplay = false;
    uint32_t HighestDigit = 0;
    uint32_t Button = 0;
    
    switch (tempCurrentMenu)
    {
        case MEMORY_WATCH_CHANGE_ADDRESS:
        {
            HighestDigit = 8;
            break;
        }
        default:
        {
            NoNumbersToDisplay = true;
            Button = B;
            break;
        }
    }
    
    // Make sure there is at least 1 number to display
    if (!NoNumbersToDisplay)
    {
        // Check to see if D-Pad Up or D-Pad Down are being held
        if (checkButtonComboEveryFrame(PAD_DPAD_UP))
        {
            Button = DPADUP;
        }
        else if (checkButtonComboEveryFrame(PAD_DPAD_DOWN))
        {
            Button = DPADDOWN;
        }
        
        if (Button != 0)
        {
            // Check to see if the value should begin to auto-increment
            uint32_t WaitFramesToBeginIncrement = MemoryWatchAdjustableValueMenu.WaitFramesToBeginIncrement;
            if (WaitFramesToBeginIncrement >= ttyd::system::sysMsec2Frame(500))
            {
                // Check to see if the number should increment or not
                uint32_t WaitFramesToPerformIncrement = MemoryWatchAdjustableValueMenu.WaitFramesToPerformIncrement;
                if (WaitFramesToPerformIncrement >= 1)
                {
                    // Auto-increment the value
                    int32_t IncrementAmount;
                    if (Button == DPADUP)
                    {
                        IncrementAmount = 1;
                    }
                    else // (Button == DPADDOWN)
                    {
                        IncrementAmount = -1;
                    }
                    
                    adjustWatchTempValueAndBounds(slot, HighestDigit, IncrementAmount);
                    MemoryWatchAdjustableValueMenu.WaitFramesToPerformIncrement = 0;
                    return Button;
                }
                else
                {
                    WaitFramesToPerformIncrement++;
                    MemoryWatchAdjustableValueMenu.WaitFramesToPerformIncrement = WaitFramesToPerformIncrement;
                }
            }
            else
            {
                WaitFramesToBeginIncrement++;
                MemoryWatchAdjustableValueMenu.WaitFramesToBeginIncrement = WaitFramesToBeginIncrement;
            }
        }
        else
        {
            // Reset the counters
            MemoryWatchAdjustableValueMenu.WaitFramesToBeginIncrement = 0;
            MemoryWatchAdjustableValueMenu.WaitFramesToPerformIncrement = 0;
        }
        
        Button = checkButtonSingleFrame();
    }
    
    switch (Button)
    {
        case DPADLEFT:
        {
            uint32_t tempSecondaryMenuOption = MenuVar.SecondaryMenuOption;
            if (tempSecondaryMenuOption == 0)
            {
                // Loop to the last option
                MenuVar.SecondaryMenuOption = HighestDigit - 1;
            }
            else
            {
                // Move left once
                MenuVar.SecondaryMenuOption = tempSecondaryMenuOption - 1;
            }
            
            MenuVar.FrameCounter = 1;
            return Button;
        }
        case DPADRIGHT:
        {
            uint32_t tempSecondaryMenuOption = MenuVar.SecondaryMenuOption;
            if (tempSecondaryMenuOption == (HighestDigit - 1))
            {
                // Loop to the first option
                MenuVar.SecondaryMenuOption = 0;
            }
            else
            {
                // Move right once
                MenuVar.SecondaryMenuOption = tempSecondaryMenuOption + 1;
            }
            
            MenuVar.FrameCounter = 1;
            return Button;
        }
        case DPADDOWN:
        {
            // Decrement the current value for the current slot
            adjustWatchTempValueAndBounds(slot, HighestDigit, -1);
            
            MenuVar.FrameCounter = 1;
            return Button;
        }
        case DPADUP:
        {
            // Increment the current value for the current slot
            adjustWatchTempValueAndBounds(slot, HighestDigit, 1);
            
            MenuVar.FrameCounter = 1;
            return Button;
        }
        case A:
        {
            switch (tempCurrentMenu)
            {
                case MEMORY_WATCH_CHANGE_ADDRESS:
                {
                    switch (tempCurrentMenuOption)
                    {
                        case 0:
                        {
                            // Modifying the address
                            // Make sure the address being read does not exceed 0x817FFFFF
                            MemoryWatch[slot].Address = reinterpret_cast<uint32_t>(
                                fixBaseAddress(slot, reinterpret_cast<void *>(MenuVar.MenuSecondaryValueUnsigned)));
                            
                            MenuVar.MenuSelectionStates = 0;
                            
                            MenuVar.FrameCounter = 1;
                            return Button;
                        }
                        default:
                        {
                            // Modifying the pointer offsets
                            MemoryWatch[slot].AddressOffset[tempCurrentMenuOption - 1] = 
                                static_cast<int32_t>(MenuVar.MenuSecondaryValueUnsigned);
                            MenuVar.MenuSelectionStates = 0;
                            
                            MenuVar.FrameCounter = 1;
                            return Button;
                        }
                    }
                    break;
                }
                default:
                {
                    return 0;
                }
            }
            break;
        }
        case B:
        {
            switch (tempCurrentMenu)
            {
                case MEMORY_WATCH_CHANGE_ADDRESS:
                {
                    MenuVar.MenuSelectionStates = 0;
                    
                    MenuVar.FrameCounter = 1;
                    
                    if (!NoNumbersToDisplay)
                    {
                        return Button;
                    }
                    else
                    {
                        return NO_NUMBERS_TO_DISPLAY;
                    }
                }
                default:
                {
                    return 0;
                }
            }
            break;
        }
        default:
        {
            return 0;
        }
    }
}

void adjustWatchTempValueAndBounds(int32_t slot, uint32_t highestDigit, int32_t valueChangedBy)
{
    for (uint32_t i = 0; i < (highestDigit - MenuVar.SecondaryMenuOption - 1); i++)
    {
        valueChangedBy *= 0x10;
    }
    
    uint32_t tempMenuSecondaryValueUnsigned = MenuVar.MenuSecondaryValueUnsigned + valueChangedBy;
    MenuVar.MenuSecondaryValueUnsigned = tempMenuSecondaryValueUnsigned;
    
    switch (MenuVar.CurrentMenu)
    {
        case MEMORY_WATCH_CHANGE_ADDRESS:
        {
            uint32_t UpperBoundUnsigned;
            uint32_t LowerBoundUnsigned;
            int32_t UpperBoundSigned;
            int32_t LowerBoundSigned;
            
            switch (MenuVar.CurrentMenuOption)
            {
                case 0:
                {
                    // Modifying the address
                    // Make sure the upper bound does not exceed 0x817FFFFF
                    UpperBoundUnsigned = reinterpret_cast<uint32_t>(
                        fixBaseAddress(slot, reinterpret_cast<void *>(0x817FFFFF)));
                    
                    LowerBoundUnsigned = 0x80000000;
                    
                    if (tempMenuSecondaryValueUnsigned > UpperBoundUnsigned)
                    {
                        // Loop to the beginning
                        MenuVar.MenuSecondaryValueUnsigned = LowerBoundUnsigned;
                    }
                    else if (tempMenuSecondaryValueUnsigned < LowerBoundUnsigned)
                    {
                        // Loop to the end
                        MenuVar.MenuSecondaryValueUnsigned = UpperBoundUnsigned;
                    }
                    break;
                }
                default:
                {
                    // Modifying the pointer offsets
                    UpperBoundSigned = 0x7FFFFFFF;
                    LowerBoundSigned = -0x80000000;
                    
                    int32_t tempMenuSecondaryValue = static_cast<int32_t>(tempMenuSecondaryValueUnsigned);
                    if (tempMenuSecondaryValue > UpperBoundSigned)
                    {
                        // Loop to the beginning
                        MenuVar.MenuSecondaryValueUnsigned = static_cast<uint32_t>(LowerBoundSigned);
                    }
                    else if (tempMenuSecondaryValue < LowerBoundSigned)
                    {
                        // Loop to the end
                        MenuVar.MenuSecondaryValueUnsigned = static_cast<uint32_t>(UpperBoundSigned);
                    }
                    break;
                }
            }
            break;
        }
        default:
        {
            return;
        }
    }
}

void *fixBaseAddress(int32_t slot, void *address)
{
    uint32_t CurrentTypeSize;   
    switch (MemoryWatch[slot].Type)
    {
        case time:
        case s64:
        case u64:
        case f64:
        {
            CurrentTypeSize = 8;
            break;
        }
        case s32:
        case u32:
        case f32:
        {
            CurrentTypeSize = 4;
            break;
        }
        case s16:
        case u16:
        {
            CurrentTypeSize = 2;
            break;
        }
        case s8:
        case u8:
        default:
        {
            CurrentTypeSize = 1;
            break;
        }
    }
    
    // Make sure the address does not exceed 0x817FFFFF
    uint32_t tempAddress = reinterpret_cast<uint32_t>(address);
    while ((tempAddress + CurrentTypeSize - 1) >= 0x81800000)
    {
        tempAddress--;
    }
    
    return reinterpret_cast<void *>(tempAddress);
}


// Memory Editor stuff
bool checkIfOtherHeaderOptionsSelected(uint32_t option)
{
    uint32_t CurrentSelectionStatus = MemoryEditor.CurrentSelectionStatus;
    switch (option)
    {
        case EDITOR_HEADER_CHANGE_ADDRESS:
        {
            if (CurrentSelectionStatus & EDITOR_SELECT_CHANGE_BYTES)
            {
                return true;
            }
            
            if (CurrentSelectionStatus & EDITOR_SELECT_GOTO_EDITOR)
            {
                return true;
            }
            break;
        }
        case EDITOR_HEADER_CHANGE_BYTE_SIZE:
        {
            if (CurrentSelectionStatus & EDITOR_SELECT_CHANGE_ADDRESS)
            {
                return true;
            }
            
            if (CurrentSelectionStatus & EDITOR_SELECT_GOTO_EDITOR)
            {
                return true;
            }
            break;
        }
        case EDITOR_HEADER_GOTO_EDITOR:
        {
            if (CurrentSelectionStatus & EDITOR_SELECT_CHANGE_ADDRESS)
            {
                return true;
            }
            
            if (CurrentSelectionStatus & EDITOR_SELECT_CHANGE_BYTES)
            {
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

bool verifyNumBytesAndCurrentDigit(uint32_t numBytes, uint32_t currentDigit)
{
    // Make sure numBytes does not exceed the maximum
    if (numBytes > EDITOR_MAX_NUM_BYTES_DISPLAYED)
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

uint8_t *getDigitAddress(uint8_t *address, uint32_t numBytes, uint32_t currentDigit)
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

uint8_t *getAndVerifyDigitAddress(uint8_t *address, uint32_t numBytes, uint32_t currentDigit)
{
    // Get the address of the current digit
    address = getDigitAddress(address, numBytes, currentDigit);
    
    // Make sure the address is valid
    if (!checkIfPointerIsValid(address))
    {
        return nullptr;
    }
    
    return address;
}

int32_t getDigitValue(uint8_t *address, uint32_t numBytes, uint32_t currentDigit)
{
    // Make sure the address is valid
    if (!checkIfPointerIsValid(address))
    {
        return -1;
    }
    
    // Make sure numBytes and currentDigit are valid
    if (!verifyNumBytesAndCurrentDigit(numBytes, currentDigit))
    {
        return -1;
    }
    
    // Get the value stored in the byte of the desired digit
    int32_t Value = static_cast<int32_t>(*address);
    
    // Get the desired digit
    int32_t ShiftAmount = (1 - (currentDigit % 2)) * 4;
    return (Value & (0xF << ShiftAmount)) >> ShiftAmount;
}

int32_t getDigitValueFromRawAddress(uint8_t *address, uint32_t numBytes, uint32_t currentDigit)
{
    // Get the address of the current digit
    address = getDigitAddress(address, numBytes, currentDigit);
    
    // Get the desired digit
    return getDigitValue(address, numBytes, currentDigit);
}

void adjustDigitValue(uint8_t *address, uint32_t numBytes, uint32_t currentDigit, int32_t value)
{
    // Make sure the address is valid
    if (!checkIfPointerIsValid(address))
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
    int32_t CurrentValue;
    int32_t OtherDigit;
    
    if ((currentDigit % 2) == 0)
    {
        // Currently changing the first digit, so get the second
        OtherDigit = getDigitValue(address, numBytes, currentDigit + 1);
        
        // Combine the values
        CurrentValue = OtherDigit + (value * 0x10);
    }
    else
    {
        // Currently changing the second digit, so get the first
        OtherDigit = getDigitValue(address, numBytes, currentDigit - 1);
        
        // Combine the values
        CurrentValue = (OtherDigit * 0x10) + value;
    }
    
    // Store the new value
    *address = static_cast<uint8_t>(CurrentValue);
}

void adjustDigitValueFromRawAddress(uint32_t currentDigit, int32_t adjustAmount)
{
    // Only adjust the current digit if it's at a valid address
    uint32_t NumBytesBeingEdited = MemoryEditor.NumBytesBeingEdited;
    
    if (!getAndVerifyDigitAddress(MemoryEditor.CurrentAddress + 
            MemoryEditor.CurrentEditorMenuOption, NumBytesBeingEdited, currentDigit))
    {
        return;
    }
    
    // Get the current digit address
    uint8_t *DigitAddress = getDigitAddress(
        MemoryEditor.BytesBeingEdited, 
        NumBytesBeingEdited, 
        currentDigit);
    
    // Get the current digit value
    int32_t CurrentDigitValue = getDigitValue(
        DigitAddress, 
        NumBytesBeingEdited, 
        currentDigit);
    
    // Make sure CurrentDigitValue is valid
    if ((CurrentDigitValue < 0) || (CurrentDigitValue > 0xF))
    {
        return;
    }
    
    // Adjust CurrentDigitValue
    CurrentDigitValue += adjustAmount;
    
    if (CurrentDigitValue < 0)
    {
        CurrentDigitValue = 0xF;
    }
    else if (CurrentDigitValue > 0xF)
    {
        CurrentDigitValue = 0;
    }
    
    // Set the new digit value
    adjustDigitValue(DigitAddress, NumBytesBeingEdited, currentDigit, CurrentDigitValue);
}

bool checkIfAddressInHoverRange(uint8_t *address, uint32_t currentDigit)
{
    // Get the amount of bytes that are being hovered over
    uint32_t NumBytesSelected = MemoryEditor.NumBytesBeingEdited;
    
    // Make sure NumBytesSelected and currentDigit are valid
    if (!verifyNumBytesAndCurrentDigit(NumBytesSelected, currentDigit))
    {
        return false;
    }
    
    // Make sure the current editor menu option is valid
    uint32_t CurrentEditorMenuOption = MemoryEditor.CurrentEditorMenuOption;
    if (CurrentEditorMenuOption >= EDITOR_MAX_NUM_BYTES_DISPLAYED)
    {
        return false;
    }
    
    // Adjust CurrentAddress based on the current editor menu option
    uint8_t *HoverAddress = MemoryEditor.CurrentAddress + CurrentEditorMenuOption;
    
    // Get HoverAddress's current digit address
    HoverAddress = getDigitAddress(HoverAddress, NumBytesSelected, currentDigit);
    
    // Check if the addresses are the same
    return HoverAddress == address;
}

uint32_t memoryEditorButtonControls()
{
    uint32_t CurrentSelectionStatus = MemoryEditor.CurrentSelectionStatus;
    
    // Check to see if the D-Pad directions are being held
    bool AutoRun = false;
    uint32_t Button = 0;
    
    // Only run when selecting or editing bytes
    if (CurrentSelectionStatus & EDITOR_SELECT_GOTO_EDITOR)
    {
        for (uint32_t i = 0; i < 4; i++)
        {
            // Values start at PAD_DPAD_LEFT
            if (checkButtonComboEveryFrame(PAD_DPAD_LEFT << i))
            {
                Button = i + 1;
                break;
            }
        }
        
        if (Button != 0)
        {
            // Check to see if the value should begin to auto-increment
            uint32_t WaitFramesToBeginIncrement = MemoryEditor.AdjustEachFrame.WaitFramesToBeginIncrement;
            if (WaitFramesToBeginIncrement >= ttyd::system::sysMsec2Frame(500))
            {
                // Check to see if the number should auto-increment or not
                uint32_t WaitFramesToPerformIncrement = MemoryEditor.AdjustEachFrame.WaitFramesToPerformIncrement;
                if (WaitFramesToPerformIncrement >= 1)
                {
                    AutoRun = true;
                    MemoryEditor.AdjustEachFrame.WaitFramesToPerformIncrement = 0;
                }
                else
                {
                    WaitFramesToPerformIncrement++;
                    MemoryEditor.AdjustEachFrame.WaitFramesToPerformIncrement = WaitFramesToPerformIncrement;
                }
            }
            else
            {
                WaitFramesToBeginIncrement++;
                MemoryEditor.AdjustEachFrame.WaitFramesToBeginIncrement = WaitFramesToBeginIncrement;
            }
        }
        else
        {
            // Reset the counters
            MemoryEditor.AdjustEachFrame.WaitFramesToBeginIncrement = 0;
            MemoryEditor.AdjustEachFrame.WaitFramesToPerformIncrement = 0;
        }
    }
    
    int32_t CurrentEditorSelectedMenuOption = static_cast<int32_t>(MemoryEditor.CurrentEditorSelectedMenuOption);
    int32_t CurrentEditorMenuOption = static_cast<int32_t>(MemoryEditor.CurrentEditorMenuOption);
    uint32_t CurrentSettingsMenuOption = MemoryEditor.CurrentSettingsMenuOption;
    
    uint8_t *CurrentAddress = MemoryEditor.CurrentAddress;
    uint8_t *BytesBeingEdited = MemoryEditor.BytesBeingEdited;
    uint32_t NumBytesBeingEdited = MemoryEditor.NumBytesBeingEdited;
    int32_t NumDigitsBeingEdited = NumBytesBeingEdited * 2;
    
    uint32_t StartingHoverDigit = (CurrentEditorMenuOption * 2) % EDITOR_DIGITS_PER_ROW;
    // uint32_t StartingHoverByte = CurrentEditorMenuOption % EDITOR_BYTES_PER_ROW;
    
    if (!AutoRun)
    {
        Button = checkButtonSingleFrame();
    }
    
    switch (Button)
    {
        case DPADLEFT:
        {
            if (CurrentSelectionStatus & EDITOR_OPEN_SETTINGS)
            {
                // D-Pad Left is not currently used in the settings menu, so do nothing
            }
            else if (CurrentSelectionStatus & EDITOR_SELECTED_BYTES_TO_EDIT) // This handles EDITOR_EDITING_BYTES as well
            {
                // Only move left if not on the furthest left byte of the current row
                bool FurthestLeftCheck = ((CurrentEditorSelectedMenuOption + 
                    StartingHoverDigit) % EDITOR_DIGITS_PER_ROW) != 0;
                
                // Only move left if not on the first digit
                bool FirstDigitCheck = CurrentEditorSelectedMenuOption > 0;
                if (FurthestLeftCheck && FirstDigitCheck)
                {
                    // Move one to the left
                    CurrentEditorSelectedMenuOption--;
                }
                else
                {
                    // Move to the furthest right digit of the current row
                    // Check to see if on the first row
                    if (!FirstDigitCheck)
                    {
                        // If there's only enough bytes to make a single row, then go to the last valid digit
                        if (NumBytesBeingEdited <= EDITOR_BYTES_PER_ROW)
                        {
                            CurrentEditorSelectedMenuOption = NumDigitsBeingEdited - 1;
                        }
                        else
                        {
                            // Go to the last digit on the first row
                            CurrentEditorSelectedMenuOption = EDITOR_DIGITS_PER_ROW - StartingHoverDigit - 1;
                        }
                    }
                    else
                    {
                        // If there's only enough bytes to make a single row, then go to to the previous digit
                        if (NumBytesBeingEdited <= EDITOR_BYTES_PER_ROW)
                        {
                            CurrentEditorSelectedMenuOption--;
                        }
                        else
                        {
                            // Go to the last digit on the current row
                            CurrentEditorSelectedMenuOption += EDITOR_DIGITS_PER_ROW - 1;
                        }
                    }
                    
                    // Make sure the current digit is valid
                    bool LastDigitCheck = CurrentEditorSelectedMenuOption > (NumDigitsBeingEdited - 1);
                    if (LastDigitCheck)
                    {
                        CurrentEditorSelectedMenuOption = NumDigitsBeingEdited - 1;
                    }
                }
                
                MemoryEditor.CurrentEditorSelectedMenuOption = 
                    static_cast<uint32_t>(CurrentEditorSelectedMenuOption);
            }
            else if (CurrentSelectionStatus & EDITOR_SELECT_GOTO_EDITOR)
            {
                // Check if currently on the furthest left byte of the current row
                if ((CurrentEditorMenuOption % EDITOR_BYTES_PER_ROW) == 0)
                {
                    // Only move if not on the top row
                    if (CurrentEditorMenuOption > 0)
                    {
                        // Move to the last byte in the previous row
                        CurrentEditorMenuOption--;
                        
                        MemoryEditor.CurrentEditorMenuOption = 
                            static_cast<uint32_t>(CurrentEditorMenuOption);
                    }
                }
                else
                {
                    // Move one to the left
                    CurrentEditorMenuOption--;
                    
                    MemoryEditor.CurrentEditorMenuOption = 
                        static_cast<uint32_t>(CurrentEditorMenuOption);
                }
            }
            else // Haven't selected an option yet
            {
                // Only move left if not on the furthest left option
                if (CurrentEditorMenuOption > 0)
                {
                    // Move one to the left
                    CurrentEditorMenuOption--;
                }
                else
                {
                    // Move to the furthest right option
                    CurrentEditorMenuOption = EDITOR_HEADER_TOTAL_OPTIONS - 1;
                }
                
                MemoryEditor.CurrentEditorMenuOption = 
                    static_cast<uint32_t>(CurrentEditorMenuOption);
            }
            
            MenuVar.FrameCounter = 1;
            return Button;
        }
        case DPADRIGHT:
        {
            if (CurrentSelectionStatus & EDITOR_OPEN_SETTINGS)
            {
                // D-Pad Right is not currently used in the settings menu, so do nothing
            }
            else if (CurrentSelectionStatus & EDITOR_SELECTED_BYTES_TO_EDIT) // This handles EDITOR_EDITING_BYTES as well
            {
                CurrentEditorSelectedMenuOption++;
                
                // Only move right if not on the furthest right byte of the current row
                bool FurthestRightByteCheck = ((CurrentEditorSelectedMenuOption + 
                    StartingHoverDigit) % EDITOR_DIGITS_PER_ROW) != 0;
                
                // Only move right if not on the last digit
                bool LastDigitCheck = CurrentEditorSelectedMenuOption < NumDigitsBeingEdited;
                if (!FurthestRightByteCheck || !LastDigitCheck)
                {
                    // Check to see if on the first row
                    int32_t RowCheck = CurrentEditorSelectedMenuOption + StartingHoverDigit;
                    if (RowCheck <= EDITOR_DIGITS_PER_ROW)
                    {
                        // Go to the first digit if the amount of bytes exceeds a single row
                        // Also go to the first digit if the next digit exceeds the maximum
                        if ((NumBytesBeingEdited > EDITOR_BYTES_PER_ROW) || 
                            (CurrentEditorSelectedMenuOption > (NumDigitsBeingEdited - 1)))
                        {
                            CurrentEditorSelectedMenuOption = 0;
                        }
                    }
                    else
                    {
                        // If there's only enough bytes to make a single row, then go to the first digit
                        if (NumBytesBeingEdited <= EDITOR_BYTES_PER_ROW)
                        {
                            CurrentEditorSelectedMenuOption = 0;
                        }
                        else
                        {
                            // Move to the first digit of the current row
                            // Check to see if on the furthest right digit of the current row
                            if (!FurthestRightByteCheck)
                            {
                                CurrentEditorSelectedMenuOption -= EDITOR_DIGITS_PER_ROW;
                            }
                            else
                            {
                                CurrentEditorSelectedMenuOption -= ((CurrentEditorSelectedMenuOption + 
                                    StartingHoverDigit) % EDITOR_DIGITS_PER_ROW);
                            }
                        }
                    }
                }
                
                MemoryEditor.CurrentEditorSelectedMenuOption = 
                    static_cast<uint32_t>(CurrentEditorSelectedMenuOption);
            }
            else if (CurrentSelectionStatus & EDITOR_SELECT_GOTO_EDITOR)
            {
                CurrentEditorMenuOption++;
                
                // Only move if not on the bottom row
                int32_t RowCheck = (CurrentEditorMenuOption + 
                    NumBytesBeingEdited) - EDITOR_MAX_NUM_BYTES_DISPLAYED;
                
                if (RowCheck <= 0)
                {
                    // Move one to the right
                    MemoryEditor.CurrentEditorMenuOption = 
                        static_cast<uint32_t>(CurrentEditorMenuOption);
                }
            }
            else // Haven't selected an option yet
            {
                // Only move right if not on the furthest right option
                if (CurrentEditorMenuOption < (EDITOR_HEADER_TOTAL_OPTIONS - 1))
                {
                    // Move one to the right
                    CurrentEditorMenuOption++;
                }
                else
                {
                    // Move to the furthest left option
                    CurrentEditorMenuOption = 0;
                }
                
                MemoryEditor.CurrentEditorMenuOption = 
                    static_cast<uint32_t>(CurrentEditorMenuOption);
            }
            
            MenuVar.FrameCounter = 1;
            return Button;
        }
        case DPADDOWN:
        {
            if (CurrentSelectionStatus & EDITOR_OPEN_SETTINGS)
            {
                // Only move down if not on the furthest bottom option
                int32_t OptionCheck = (MemoryEditorSettingsLinesSize - 1) - CurrentSettingsMenuOption;
                if (OptionCheck > 0)
                {
                    CurrentSettingsMenuOption++;
                    MemoryEditor.CurrentSettingsMenuOption = CurrentSettingsMenuOption;
                }
                else
                {
                    // Go to the first option
                    MemoryEditor.CurrentSettingsMenuOption = 0;
                }
            }
            else if (CurrentSelectionStatus & EDITOR_EDITING_BYTES)
            {
                // Decrement the current digit value
                adjustDigitValueFromRawAddress(CurrentEditorSelectedMenuOption, -1);
            }
            else if (CurrentSelectionStatus & EDITOR_SELECTED_BYTES_TO_EDIT)
            {
                // Move down one row
                CurrentEditorSelectedMenuOption += EDITOR_DIGITS_PER_ROW;
                
                // Check if there's a row under the current one
                int32_t RowCheck = NumDigitsBeingEdited - CurrentEditorSelectedMenuOption;
                if (RowCheck <= 0)
                {
                    // Loop backwards through the rows until the first invalid digit is reached
                    do
                    {
                        CurrentEditorSelectedMenuOption -= EDITOR_DIGITS_PER_ROW;
                    }
                    while (CurrentEditorSelectedMenuOption >= 0);
                    
                    // Go back to the first valid digit
                    CurrentEditorSelectedMenuOption += EDITOR_DIGITS_PER_ROW;
                }
                
                MemoryEditor.CurrentEditorSelectedMenuOption = 
                    static_cast<uint32_t>(CurrentEditorSelectedMenuOption);
            }
            else if (CurrentSelectionStatus & EDITOR_SELECT_GOTO_EDITOR)
            {
                CurrentEditorMenuOption += EDITOR_BYTES_PER_ROW;
                
                // Check to see if moving down goes to the next row
                bool RowCheck = (CurrentEditorMenuOption + 
                    NumBytesBeingEdited) > EDITOR_MAX_NUM_BYTES_DISPLAYED;
                
                if (RowCheck)
                {
                    // Only move down if there's at least one valid byte under the last row
                    if (checkIfPointerIsValid(CurrentAddress + EDITOR_MAX_NUM_BYTES_DISPLAYED))
                    {
                        // Move down one row
                        // Don't set CurrentEditorMenuOption, as it's already correct
                        MemoryEditor.CurrentAddress = CurrentAddress + EDITOR_BYTES_PER_ROW;
                    }
                }
                else
                {
                    // Move down one row
                    MemoryEditor.CurrentEditorMenuOption = 
                        static_cast<uint32_t>(CurrentEditorMenuOption);
                }
            }
            
            MenuVar.FrameCounter = 1;
            return Button;
        }
        case DPADUP:
        {
            if (CurrentSelectionStatus & EDITOR_OPEN_SETTINGS)
            {
                // Only move up if not on the first option
                if (CurrentSettingsMenuOption > 0)
                {
                    CurrentSettingsMenuOption--;
                    MemoryEditor.CurrentSettingsMenuOption = CurrentSettingsMenuOption;
                }
                else
                {
                    // Go to the last option
                    MemoryEditor.CurrentSettingsMenuOption = MemoryEditorSettingsLinesSize - 1;
                }
            }
            else if (CurrentSelectionStatus & EDITOR_EDITING_BYTES)
            {
                // Increment the current digit value
                adjustDigitValueFromRawAddress(CurrentEditorSelectedMenuOption, 1);
            }
            else if (CurrentSelectionStatus & EDITOR_SELECTED_BYTES_TO_EDIT)
            {
                // Move up one row
                CurrentEditorSelectedMenuOption -= EDITOR_DIGITS_PER_ROW;
                
                // Check if there's a row above the current one
                if (CurrentEditorSelectedMenuOption < 0)
                {
                    // Loop forwards through the rows until the first invalid digit is reached
                    do
                    {
                        CurrentEditorSelectedMenuOption += EDITOR_DIGITS_PER_ROW;
                    }
                    while (CurrentEditorSelectedMenuOption <= (NumDigitsBeingEdited - 1));
                    
                    // Go back to the first valid digit
                    CurrentEditorSelectedMenuOption -= EDITOR_DIGITS_PER_ROW;
                }
                
                MemoryEditor.CurrentEditorSelectedMenuOption = 
                    static_cast<uint32_t>(CurrentEditorSelectedMenuOption);
            }
            else if (CurrentSelectionStatus & EDITOR_SELECT_GOTO_EDITOR)
            {
                CurrentEditorMenuOption -= EDITOR_BYTES_PER_ROW;
                
                // Check to see if moving up goes to the previous row
                if (CurrentEditorMenuOption < 0)
                {
                    // Only move up if there's at least one valid byte above the first row
                    if (checkIfPointerIsValid(CurrentAddress - 0x1))
                    {
                        // Move up one row
                        // Don't set CurrentEditorMenuOption, as it's already correct
                        MemoryEditor.CurrentAddress = CurrentAddress - EDITOR_BYTES_PER_ROW;
                    }
                }
                else
                {
                    // Move up one row
                    MemoryEditor.CurrentEditorMenuOption = 
                        static_cast<uint32_t>(CurrentEditorMenuOption);
                }
            }
            
            MenuVar.FrameCounter = 1;
            return Button;
        }
        case A:
        {
            if (CurrentSelectionStatus & EDITOR_OPEN_SETTINGS)
            {
                switch (CurrentSettingsMenuOption)
                {
                    case EDITOR_SETTINGS_CLEAR_CACHE:
                    {
                        // Toggle the bit for the current option
                        CurrentSelectionStatus ^= EDITOR_CLEAR_CACHE;
                        MemoryEditor.CurrentSelectionStatus = CurrentSelectionStatus;
                        break;
                    }
                    case EDITOR_SETTINGS_SYSTEM_LEVEL:
                    {
                        // Toggle the bit for the current option
                        CurrentSelectionStatus ^= EDITOR_RAISE_SYSTEM_LEVEL;
                        MemoryEditor.CurrentSelectionStatus = CurrentSelectionStatus;
                        
                        // Raise or lower the System Level based on the new value
                        if (CurrentSelectionStatus & EDITOR_RAISE_SYSTEM_LEVEL)
                        {
                            raiseSystemLevel(1);
                        }
                        else
                        {
                            lowerSystemLevel();
                        }
                        break;
                    }
                    case EDITOR_SETTINGS_PAUSE_MENU:
                    {
                        // Toggle the bit for the current option
                        CurrentSelectionStatus ^= EDITOR_DISABLE_PAUSE_MENU;
                        MemoryEditor.CurrentSelectionStatus = CurrentSelectionStatus;
                        
                        // Enable or disable the pause menu based on the new value
                        if (CurrentSelectionStatus & EDITOR_DISABLE_PAUSE_MENU)
                        {
                            ttyd::win_main::winOpenDisable();
                        }
                        else
                        {
                            ttyd::win_main::winOpenEnable();
                        }
                        break;
                    }
                    case EDITOR_SETTINGS_VERTICAL_LINES:
                    {
                        // Toggle the bit for the current option
                        CurrentSelectionStatus ^= EDITOR_VERTICAL_LINES;
                        MemoryEditor.CurrentSelectionStatus = CurrentSelectionStatus;
                        break;
                    }
                    case EDITOR_SETTINGS_HORIZONTAL_LINES:
                    {
                        // Toggle the bit for the current option
                        CurrentSelectionStatus ^= EDITOR_HORIZONTAL_LINES;
                        MemoryEditor.CurrentSelectionStatus = CurrentSelectionStatus;
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
            }
            else if (CurrentSelectionStatus & EDITOR_EDITING_BYTES)
            {
                /* Copy the bytes from the amount 
                    of bytes to change into memory */
                
                // Only copy bytes that are at a valid address
                uint8_t *tempAddress = CurrentAddress + CurrentEditorMenuOption;
                
                for (uint32_t i = 0; i < NumBytesBeingEdited; i++)
                {
                    uint8_t *address = &tempAddress[i];
                    if (checkIfPointerIsValid(address))
                    {
                         *address = BytesBeingEdited[i];
                    }
                }
                
                // Clear the cache of the memory if the flag is set
                if (CurrentSelectionStatus & EDITOR_CLEAR_CACHE)
                {
                    // Only clear the cache of valid memory
                    uint32_t Start = reinterpret_cast<uint32_t>(tempAddress);
                    uint32_t End = Start + NumBytesBeingEdited;
                    
                    if (Start < 0x80000000)
                    {
                        Start = 0x80000000;
                    }
                    
                    if (End > 0x81800000)
                    {
                        End = 0x81800000;
                    }
                    
                    uint32_t Size = End - Start;
                    patch::clear_DC_IC_Cache(reinterpret_cast<uint8_t *>(Start), Size);
                }
                
                // Go back to selecting a range of bytes to edit
                MemoryEditor.CurrentSelectionStatus = CurrentSelectionStatus & 
                    ~(EDITOR_EDITING_BYTES | EDITOR_SELECTED_BYTES_TO_EDIT);
            }
            else if (CurrentSelectionStatus & EDITOR_SELECTED_BYTES_TO_EDIT)
            {
                // Start editing bytes
                MemoryEditor.CurrentSelectionStatus = 
                    CurrentSelectionStatus | EDITOR_EDITING_BYTES;
            }
            else if (CurrentSelectionStatus & EDITOR_SELECT_GOTO_EDITOR)
            {
                // Clear the array for the amount of bytes to change
                clearMemory(BytesBeingEdited, NumBytesBeingEdited);
                
                /* Copy the bytes from memory into the array 
                    for the amount of bytes to change */
                
                // Make sure there's at least one valid byte to copy
                uint8_t *tempAddress = CurrentAddress + CurrentEditorMenuOption;
                bool ValidBytesCopied = false;
                
                for (uint32_t i = 0; i < NumBytesBeingEdited; i++)
                {
                    uint8_t *address = &tempAddress[i];
                    if (checkIfPointerIsValid(address))
                    {
                        BytesBeingEdited[i] = *address;
                        ValidBytesCopied = true;
                    }
                }
                
                // If at least one valid byte was copied, then allow selecting digits to edit
                if (ValidBytesCopied)
                {
                    MemoryEditor.CurrentEditorSelectedMenuOption = 0;
                    
                    MemoryEditor.CurrentSelectionStatus = 
                        CurrentSelectionStatus | EDITOR_SELECTED_BYTES_TO_EDIT;
                }
            }
            else // Haven't selected an option yet
            {
                switch (CurrentEditorMenuOption)
                {
                    case EDITOR_HEADER_CHANGE_ADDRESS:
                    {
                        // Set the values for the adjustable value menu
                        MenuVar.SecondaryMenuOption = 7;
                        
                        MenuVar.MenuSecondaryValueUnsigned = 
                            reinterpret_cast<uint32_t>(CurrentAddress);
                        
                        // Open the window for changing the address
                        MemoryEditor.CurrentSelectionStatus = 
                            CurrentSelectionStatus | EDITOR_SELECT_CHANGE_ADDRESS;
                        
                        // Don't display the adjustable value menu on frame it is enabled
                        MenuVar.FrameCounter = 1;
                        return MEMORY_EDITOR_ADJUSTABLE_VALUE;
                    }
                    case EDITOR_HEADER_CHANGE_BYTE_SIZE:
                    {
                        // Set the values for the adjustable value menu
                        MenuVar.SecondaryMenuOption = 
                            getHighestAdjustableValueDigit(MEMORY_EDITOR_MENU) - 1;
                        
                        MenuVar.MenuSecondaryValue = static_cast<int32_t>(NumBytesBeingEdited);
                        
                        // Open the window for changing the amount of bytes to change
                        MemoryEditor.CurrentSelectionStatus = 
                            CurrentSelectionStatus | EDITOR_SELECT_CHANGE_BYTES;
                        
                        // Don't display the adjustable value menu on frame it is enabled
                        MenuVar.FrameCounter = 1;
                        return MEMORY_EDITOR_ADJUSTABLE_VALUE;
                    }
                    case EDITOR_HEADER_GOTO_EDITOR:
                    {
                        /* If memory was previously allocated for the bytes 
                            that are being changed, then free it */
                        if (BytesBeingEdited)
                        {
                            delete[] (BytesBeingEdited);
                        }
                        
                        // Allocate memory for the bytes that are being changed
                        MemoryEditor.BytesBeingEdited = new uint8_t[NumBytesBeingEdited];
                        
                        MemoryEditor.CurrentEditorMenuOption = 0;
                        
                        // Open the editor
                        MemoryEditor.CurrentSelectionStatus = 
                            CurrentSelectionStatus | EDITOR_SELECT_GOTO_EDITOR;
                        
                        break;
                    }
                    default:
                    {
                        return 0;
                    }
                }
            }
            
            MenuVar.FrameCounter = 1;
            return Button;
        }
        case B:
        {
            if (CurrentSelectionStatus & EDITOR_OPEN_SETTINGS)
            {
                // Go back to the main editor menu
                MemoryEditor.CurrentSelectionStatus = 
                    CurrentSelectionStatus & ~EDITOR_OPEN_SETTINGS;
            }
            else if (CurrentSelectionStatus & EDITOR_EDITING_BYTES)
            {
                // Go back to navigating through the selected bytes
                MemoryEditor.CurrentSelectionStatus = 
                    CurrentSelectionStatus & ~EDITOR_EDITING_BYTES;
            }
            else if (CurrentSelectionStatus & EDITOR_SELECTED_BYTES_TO_EDIT)
            {
                // Clear the memory for the amount of bytes to change
                clearMemory(BytesBeingEdited, NumBytesBeingEdited);
                
                // Go back to selecting a range to modify
                MemoryEditor.CurrentSelectionStatus = 
                    CurrentSelectionStatus & ~EDITOR_SELECTED_BYTES_TO_EDIT;
            }
            else if (CurrentSelectionStatus & EDITOR_SELECT_GOTO_EDITOR)
            {
                // Clear the memory for the amount of bytes to change
                if (BytesBeingEdited)
                {
                    delete[] (BytesBeingEdited);
                    MemoryEditor.BytesBeingEdited = nullptr;
                }
                
                MemoryEditor.CurrentEditorMenuOption = EDITOR_HEADER_GOTO_EDITOR;
                
                // Go back to selecting a header option
                MemoryEditor.CurrentSelectionStatus = 
                    CurrentSelectionStatus & ~EDITOR_SELECT_GOTO_EDITOR;
            }
            else // Haven't selected an option yet
            {
                // Lower the System Level if it was previously raised
                if (CurrentSelectionStatus & EDITOR_RAISE_SYSTEM_LEVEL)
                {
                    lowerSystemLevel();
                }
                
                // Enable the pause menu if it was disabled
                if (CurrentSelectionStatus & EDITOR_DISABLE_PAUSE_MENU)
                {
                    ttyd::win_main::winOpenEnable();
                }
                
                // Close the editor
                MemoryEditor.CurrentEditorMenuOption = 0;
                MemoryEditor.EditorCurrentlyDisplayed = false;
            }
            
            MenuVar.FrameCounter = 1;
            return Button;
        }
        case Y:
        {
            // Open the settings menu
            MemoryEditor.CurrentSelectionStatus = 
                CurrentSelectionStatus | EDITOR_OPEN_SETTINGS;
            
            MenuVar.FrameCounter = 1;
            return Button;
        }
        default:
        {
            return 0;
        }
    }
}

}