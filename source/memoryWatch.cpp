#include "memoryWatch.h"
#include "mod.h"
#include "displays.h"
#include "drawText.h"
#include "cxx.h"
#include "memoryEditor.h"
#include "menuUtils.h"
#include "classes/positionEditor.h"
#include "classes/valueEditor.h"
#include "menus/memoryMenu.h"
#include "misc/utils.h"
#include "ttyd/camdrv.h"

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cinttypes>

static bool positioningOrScalingWatch(uint32_t watchIndex);
static bool getWatchPosAndScale(float posAndScaleOut[3], uint32_t watchIndex);
static uint8_t *fixBaseAddress(uint8_t *address, VariableType type);

// The MemoryWatch class is very small, so making it dynamic would waste a lot of memory
MemoryWatch gMemoryWatch;

void MemoryWatchEntry::init(uint32_t entryId)
{
    // Free the memory used by the address offsets
    this->freeAddressOffsets();

    // Make sure the memory for the watch is cleared before doing anything
    clearMemory(this, sizeof(MemoryWatchEntry));

    this->posX = -44.f;
    this->posY = -32.f;
    this->type = VariableType::string;
    this->scale = DISPLAYS_DEFAULT_SCALE;
    this->address = reinterpret_cast<uint8_t *>(0x80000000);
    snprintf(this->name, sizeof(this->name), "Watch %" PRIu32, entryId);
}

bool MemoryWatchEntry::addAddressOffset()
{
    int32_t *offsetsPtr = this->addressOffsets;
    if (!offsetsPtr)
    {
        // No current offsets, so make a new one
        constexpr uint32_t totalOffsets = 1;
        this->totalAddressOffsets = static_cast<uint8_t>(totalOffsets);

        offsetsPtr = new int32_t[totalOffsets];
        this->addressOffsets = offsetsPtr;
        return true;
    }

    // Make sure the limit has not been reached
    const uint32_t totalOffsets = this->totalAddressOffsets;
    if (totalOffsets >= MAX_MEMORY_WATCH_ADDRESS_OFFSETS)
    {
        return false;
    }

    // Add a new offset
    const uint32_t newTotalOffsets = totalOffsets + 1;
    this->totalAddressOffsets = static_cast<uint8_t>(newTotalOffsets);

    // Make a backup of the current offsets
    int32_t offsetsBackup[totalOffsets];
    memcpy(offsetsBackup, offsetsPtr, sizeof(int32_t) * totalOffsets);

    // Reallocate memory for the offsets and restore the current offsets
    delete[] offsetsPtr;
    offsetsPtr = new int32_t[newTotalOffsets];
    this->addressOffsets = offsetsPtr;

    memcpy(offsetsPtr, offsetsBackup, sizeof(int32_t) * totalOffsets);
    return true;
}

bool MemoryWatchEntry::removeAddressOffset()
{
    // Make sure there is at least one offset to delete
    const uint32_t totalOffsets = this->totalAddressOffsets;
    int32_t *offsetsPtr = this->addressOffsets;

    if ((totalOffsets == 0) || !offsetsPtr)
    {
        return false;
    }

    // Remove one offset
    const uint32_t newTotalOffsets = totalOffsets - 1;
    this->totalAddressOffsets = static_cast<uint8_t>(newTotalOffsets);

    // If the total entries are now at 0, then free the memory for the entries
    if (newTotalOffsets == 0)
    {
        delete[] offsetsPtr;
        this->addressOffsets = nullptr;
        return true;
    }

    // Make a backup of the current offsets up to the last one
    int32_t offsetsBackup[newTotalOffsets];
    memcpy(offsetsBackup, offsetsPtr, sizeof(int32_t) * newTotalOffsets);

    // Reallocate memory for the offsets and restore the current offsets up to the last one
    delete[] offsetsPtr;
    offsetsPtr = new int32_t[newTotalOffsets];
    this->addressOffsets = offsetsPtr;

    memcpy(offsetsPtr, offsetsBackup, sizeof(int32_t) * newTotalOffsets);
    return true;
}

void MemoryWatchEntry::getValueString(char *stringOut, uint32_t stringSize) const
{
    // Get the final address for the value
    uint8_t *address = getFinalAddressFromPointerPath(this->address, this->addressOffsets, this->totalAddressOffsets);

    // Make sure the address is valid
    if (!address)
    {
        // Address is invalid
        // Use snprintf to make sure stringSize is not exceeded, and that a null terminator is properly applied
        snprintf(stringOut, stringSize, "???");
        return;
    }

    // Make sure the address being read does not exceed 0x817FFFFF/0xC17FFFFF
    const VariableType type = this->type;
    address = fixBaseAddress(address, type);

    // Set up the string
    const ValueType *valuePtr = reinterpret_cast<ValueType *>(address);
    const bool showAsHex = this->showAsHex;
    const char *format;

    switch (type)
    {
        case VariableType::s8:
        {
            if (showAsHex)
            {
                format = "0x%02" PRIX8;
            }
            else
            {
                format = "%" PRId8;
            }

            snprintf(stringOut, stringSize, format, valuePtr->s8);
            return;
        }
        case VariableType::s16:
        {
            if (showAsHex)
            {
                format = "0x%04" PRIX16;
            }
            else
            {
                format = "%" PRId16;
            }

            snprintf(stringOut, stringSize, format, valuePtr->s16);
            return;
        }
        case VariableType::s32:
        {
            if (showAsHex)
            {
                format = "0x%08" PRIX32;
            }
            else
            {
                format = "%" PRId32;
            }

            snprintf(stringOut, stringSize, format, valuePtr->s32);
            return;
        }
        case VariableType::s64:
        {
            if (showAsHex)
            {
                format = "0x%016" PRIX64;
            }
            else
            {
                format = "%" PRId64;
            }

            snprintf(stringOut, stringSize, format, valuePtr->s64);
            return;
        }
        case VariableType::u8:
        {
            if (showAsHex)
            {
                format = "0x%02" PRIX8;
            }
            else
            {
                format = "%" PRIu8;
            }

            snprintf(stringOut, stringSize, format, valuePtr->u8);
            return;
        }
        case VariableType::u16:
        {
            if (showAsHex)
            {
                format = "0x%04" PRIX16;
            }
            else
            {
                format = "%" PRIu16;
            }

            snprintf(stringOut, stringSize, format, valuePtr->u16);
            return;
        }
        case VariableType::u32:
        {
            if (showAsHex)
            {
                format = "0x%08" PRIX32;
            }
            else
            {
                format = "%" PRIu32;
            }

            snprintf(stringOut, stringSize, format, valuePtr->u32);
            return;
        }
        case VariableType::u64:
        {
            if (showAsHex)
            {
                format = "0x%016" PRIX64;
            }
            else
            {
                format = "%" PRIu64;
            }

            snprintf(stringOut, stringSize, format, valuePtr->u64);
            return;
        }
        case VariableType::f32:
        {
            if (showAsHex)
            {
                snprintf(stringOut, stringSize, "0x%08" PRIX32, valuePtr->u32);
            }
            else
            {
                snprintf(stringOut, stringSize, "%.6f", valuePtr->f32);
            }
            return;
        }
        case VariableType::f64:
        {
            if (showAsHex)
            {
                snprintf(stringOut, stringSize, "0x%016" PRIX64, valuePtr->u64);
            }
            else
            {
                snprintf(stringOut, stringSize, "%.6f", valuePtr->f64);
            }
            return;
        }
        case VariableType::string:
        {
            // Retrieve each byte while also making sure to not read into invalid memory
            const char *string = reinterpret_cast<const char *>(address);

            for (uint32_t i = 0; i < stringSize; i++, string++)
            {
                if (i == (stringSize - 1))
                {
                    // Reached the end of the string, so make sure it is properly null terminated
                    stringOut[i] = '\0';
                    return;
                }

                if (ptrIsValid(const_cast<char *>(string)))
                {
                    const char currentChar = *string;
                    stringOut[i] = currentChar;

                    if (currentChar == '\0')
                    {
                        // Reached the end of the string
                        return;
                    }
                }
                else
                {
                    // Make sure the string is properly null terminated
                    stringOut[i] = '\0';
                    return;
                }
            }
            return;
        }
        case VariableType::time:
        {
            getTimeString(stringOut, stringSize, valuePtr->s64);
            return;
        }
        default:
        {
            // Type is invalid
            // Use snprintf to make sure stringSize is not exceeded, and that a null terminator is properly applied
            snprintf(stringOut, stringSize, "???");
            return;
        }
    }
}

bool MemoryWatchEntry::shouldDraw(uint32_t watchIndex) const
{
    return this->shouldDisplay() || positioningOrScalingWatch(watchIndex);
}

void MemoryWatchEntry::draw(uint32_t watchIndex) const
{
    // Get the position and scale for the watch
    float posAndScale[3];
    getWatchPosAndScale(posAndScale, watchIndex);

    // Draw the watch
    char buf[128];
    this->getValueString(buf, sizeof(buf));
    drawText(buf, posAndScale[0], posAndScale[1], posAndScale[2], getColorWhite(0xFF));
}

MemoryWatchEntry *MemoryWatch::addWatch()
{
    MemoryWatchEntry *entriesPtr = this->entries;
    if (!entriesPtr)
    {
        // No current entries, so make a new one
        constexpr uint32_t totalEntries = 1;
        this->totalEntries = static_cast<uint8_t>(totalEntries);

        entriesPtr = new MemoryWatchEntry[totalEntries];
        this->entries = entriesPtr;

        entriesPtr->init(totalEntries);
        return entriesPtr;
    }

    // Make sure the limit has not been reached
    const uint32_t totalEntries = this->totalEntries;
    if (totalEntries >= MAX_MEMORY_WATCHES)
    {
        return nullptr;
    }

    // Add a new entry
    const uint32_t newTotalEntries = totalEntries + 1;
    this->totalEntries = static_cast<uint8_t>(newTotalEntries);

    // Make a backup of the current entries
    // Allocate to the front of the heap to avoid fragmentation
    MemoryWatchEntry *entriesBackupPtr = new (true) MemoryWatchEntry[totalEntries];
    constexpr uint32_t watchSize = sizeof(MemoryWatchEntry);
    memcpy(entriesBackupPtr, entriesPtr, watchSize * totalEntries);

    // Reallocate memory for the watches and restore the current entries
    delete[] entriesPtr;
    entriesPtr = new MemoryWatchEntry[newTotalEntries];
    this->entries = entriesPtr;

    memcpy(entriesPtr, entriesBackupPtr, watchSize * totalEntries);
    delete[] entriesBackupPtr;

    // Initialize the new entry
    MemoryWatchEntry *currentEntry = &entriesPtr[totalEntries];
    currentEntry->init(newTotalEntries);
    return currentEntry;
}

bool MemoryWatch::duplicateWatch(uint32_t index)
{
    // Make sure the limit has not been reached
    const uint32_t totalEntries = this->totalEntries;
    if (totalEntries >= MAX_MEMORY_WATCHES)
    {
        return false;
    }

    // Make sure the index is valid
    if (index >= totalEntries)
    {
        return false;
    }

    // Make sure the entries pointer is set
    MemoryWatchEntry *entriesPtr = this->entries;
    if (!entriesPtr)
    {
        return false;
    }

    // Add a new entry
    const uint32_t newTotalEntries = totalEntries + 1;
    this->totalEntries = static_cast<uint8_t>(newTotalEntries);

    // Make a backup of the current entries
    // Allocate to the front of the heap to avoid fragmentation
    MemoryWatchEntry *entriesBackupPtr = new (true) MemoryWatchEntry[totalEntries];
    constexpr uint32_t watchSize = sizeof(MemoryWatchEntry);
    memcpy(entriesBackupPtr, entriesPtr, watchSize * totalEntries);

    // Reallocate memory for the watches and restore the current entries
    delete[] entriesPtr;
    entriesPtr = new MemoryWatchEntry[newTotalEntries];
    this->entries = entriesPtr;

    memcpy(entriesPtr, entriesBackupPtr, watchSize * totalEntries);
    delete[] entriesBackupPtr;

    // Copy the selected watch to the new watch
    MemoryWatchEntry *newEntryPtr = &entriesPtr[totalEntries];
    memcpy(newEntryPtr, &entriesPtr[index], watchSize);

    // The new watch's addressOffsets pointer is currently set to the original's, so set up new memory for it if it has at least
    // one offset
    const uint32_t totalOffsets = newEntryPtr->getTotalAddressOffsets();
    const int32_t *offsetsPtr = newEntryPtr->getAddressOffsetsPtr();
    if ((totalOffsets > 0) && offsetsPtr)
    {
        int32_t *newOffsets = new int32_t[totalOffsets];
        newEntryPtr->setAddressOffsetsPtr(newOffsets);
        memcpy(newOffsets, offsetsPtr, totalOffsets * sizeof(int32_t));
    }

    return true;
}

bool MemoryWatch::swapWatches(uint32_t watchOneIndex, uint32_t watchTwoIndex)
{
    // Make sure both indexes are valid
    const uint32_t totalEntries = this->totalEntries;
    if (watchOneIndex >= totalEntries)
    {
        return false;
    }

    if (watchTwoIndex >= totalEntries)
    {
        return false;
    }

    // Make sure the entries pointer is set
    MemoryWatchEntry *entriesPtr = this->entries;
    if (!entriesPtr)
    {
        return false;
    }

    // If both indexes are the same, then don't bother doing anything
    if (watchOneIndex == watchTwoIndex)
    {
        return true;
    }

    // Make a temporary backup for the first watch
    MemoryWatchEntry *firstWatchPtr = &entriesPtr[watchOneIndex];
    constexpr uint32_t watchSize = sizeof(MemoryWatchEntry);

    MemoryWatchEntry tempEntry;
    memcpy(&tempEntry, firstWatchPtr, watchSize);

    // Move the second watch to the first watch
    MemoryWatchEntry *secondWatchPtr = &entriesPtr[watchTwoIndex];
    memcpy(firstWatchPtr, secondWatchPtr, watchSize);

    // Move the first watch to the second watch
    memcpy(secondWatchPtr, &tempEntry, watchSize);
    return true;
}

bool MemoryWatch::moveWatch(uint32_t destinationIndex, uint32_t sourceIndex)
{
    // Make sure both indexes are valid
    const uint32_t totalEntries = this->totalEntries;
    if (destinationIndex >= totalEntries)
    {
        return false;
    }

    if (sourceIndex >= totalEntries)
    {
        return false;
    }

    // Make sure the entries pointer is set
    MemoryWatchEntry *entriesPtr = this->entries;
    if (!entriesPtr)
    {
        return false;
    }

    // If both indexes are the same, then don't bother doing anything
    if (destinationIndex == sourceIndex)
    {
        return true;
    }

    // If the indexes are right next to each other, then just swap them
    const bool movingWatchDown = destinationIndex > sourceIndex;
    int32_t indexDistance;

    if (movingWatchDown)
    {
        indexDistance = destinationIndex - sourceIndex;
    }
    else
    {
        indexDistance = sourceIndex - destinationIndex;
    }

    if (indexDistance < 2)
    {
        return this->swapWatches(destinationIndex, sourceIndex);
    }

    // Make a temporary backup for the source watch
    constexpr uint32_t watchSize = sizeof(MemoryWatchEntry);
    MemoryWatchEntry tempEntry;

    MemoryWatchEntry *sourceWatchPtr = &entriesPtr[sourceIndex];
    memcpy(&tempEntry, sourceWatchPtr, watchSize);

    MemoryWatchEntry *destinationWatchPtr = &entriesPtr[destinationIndex];

    if (movingWatchDown)
    {
        // Move the watches from the destination to the source up by one
        memmove(sourceWatchPtr, &sourceWatchPtr[1], watchSize * indexDistance);
    }
    else
    {
        // Move the watches from the destination to the source down by one
        memmove(&destinationWatchPtr[1], destinationWatchPtr, watchSize * indexDistance);
    }

    // Move the source watch to the destination watch
    memcpy(destinationWatchPtr, &tempEntry, watchSize);
    return true;
}

bool MemoryWatch::deleteWatch(uint32_t index)
{
    // Make sure the index is valid
    const uint32_t totalEntries = this->totalEntries;
    if (index >= totalEntries)
    {
        return false;
    }

    // Make sure there is at least one watch to delete
    MemoryWatchEntry *entriesPtr = this->entries;
    if ((totalEntries == 0) || !entriesPtr)
    {
        return false;
    }

    // Free the memory used by the address offsets of the selected watch
    entriesPtr[index].freeAddressOffsets();

    // Remove one entry
    const uint32_t newTotalEntries = totalEntries - 1;
    this->totalEntries = static_cast<uint8_t>(newTotalEntries);

    // If the total entries are now at 0, then free the memory for the entries
    if (newTotalEntries == 0)
    {
        delete[] entriesPtr;
        this->entries = nullptr;
        return true;
    }

    // Make a backup of the current entries
    // Allocate to the front of the heap to avoid fragmentation
    MemoryWatchEntry *entriesBackupPtr = new (true) MemoryWatchEntry[totalEntries];
    constexpr uint32_t watchSize = sizeof(MemoryWatchEntry);
    memcpy(entriesBackupPtr, entriesPtr, watchSize * totalEntries);

    // Reallocate memory for the watches
    delete[] entriesPtr;
    entriesPtr = new MemoryWatchEntry[newTotalEntries];
    this->entries = entriesPtr;

    // Restore all of the watches up to the current one
    if (index > 0)
    {
        memcpy(entriesPtr, entriesBackupPtr, watchSize * index);
    }

    // Restore the rest of the watches if there are any after the current one
    if (index < newTotalEntries)
    {
        memcpy(&entriesPtr[index], &entriesBackupPtr[index + 1], watchSize * (totalEntries - index - 1));
    }

    delete[] entriesBackupPtr;
    return true;
}

static void drawMemoryWatches(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Initialize text drawing
    drawTextInit(true);

    // Draw each watch
    MemoryWatch *memoryWatchPtr = &gMemoryWatch;
    MemoryWatchEntry *entriesPtr = memoryWatchPtr->getEntriesPtr();
    const uint32_t totalEntries = memoryWatchPtr->getTotalEntries();

    for (uint32_t i = 0; i < totalEntries; i++)
    {
        const MemoryWatchEntry *currentEntry = &entriesPtr[i];
        if (currentEntry->shouldDraw(i))
        {
            currentEntry->draw(i);
        }
    }
}

void handleMemoryWatches()
{
    // Do not do anything if the memory editor is currently open
    if (memoryEditorIsOpen())
    {
        return;
    }

    // Draw each watch
    // Only draw if the menu isn't currently open
    // Draw if currently repositioning/scaling watches
    if (gMenu && !gMod->flagIsSet(ModFlag::MOD_FLAG_MENU_IS_HIDDEN))
    {
        return;
    }

    // Make sure the pointer for the entries is set
    MemoryWatch *memoryWatchPtr = &gMemoryWatch;
    MemoryWatchEntry *entriesPtr = memoryWatchPtr->getEntriesPtr();
    if (!entriesPtr)
    {
        return;
    }

    // Make sure there is at least one watch to draw
    const uint32_t totalEntries = memoryWatchPtr->getTotalEntries();
    bool shouldDraw = false;

    for (uint32_t i = 0; i < totalEntries; i++)
    {
        if (entriesPtr[i].shouldDraw(i))
        {
            shouldDraw = true;
            break;
        }
    }

    if (!shouldDraw)
    {
        return;
    }

    // Draw the memory watches
    drawOnDebugLayer(drawMemoryWatches, DRAW_ORDER_DISPLAYS);
}

static bool positioningOrScalingWatch(uint32_t watchIndex)
{
    return getWatchPosAndScale(nullptr, watchIndex);
}

static bool getWatchPosAndScale(float posAndScaleOut[3], uint32_t watchIndex)
{
    // Make sure the pointer for the entries is set
    MemoryWatchEntry *entriesPtr = gMemoryWatch.getEntriesPtr();
    if (!entriesPtr)
    {
        if (posAndScaleOut)
        {
            posAndScaleOut[0] = 0.f;
            posAndScaleOut[1] = 0.f;
            posAndScaleOut[2] = 0.f;
        }
        return false;
    }

    // Set the initial position and scale to the current values
    if (posAndScaleOut)
    {
        const MemoryWatchEntry *currentEntry = &entriesPtr[watchIndex];
        posAndScaleOut[0] = currentEntry->getPosX();
        posAndScaleOut[1] = currentEntry->getPosY();
        posAndScaleOut[2] = currentEntry->getScale();
    }

    // Check if the player is currently manually positioning or scaling the watch
    // Make sure the Memory menu is open
    MemoryMenu *memoryMenuPtr = gMemoryMenu;
    if (!memoryMenuPtr)
    {
        return false;
    }

    // If the selected watch in the menu is not for the current watch, then assume that the player is not currently manually
    // positioning or scaling
    if (memoryMenuPtr->getSelectedIndex() != watchIndex)
    {
        return false;
    }

    // Check if the player is currently manually positioning the display
    const PositionEditor *positonEditorPtr = memoryMenuPtr->getPositionEditorPtr();
    if (positonEditorPtr->shouldDraw())
    {
        if (posAndScaleOut)
        {
            posAndScaleOut[0] = positonEditorPtr->getPosX();
            posAndScaleOut[1] = positonEditorPtr->getPosY();
        }
        return true;
    }

    // Check if the player is currently adjusting the scale
    const ValueEditor *valueEditorPtr = memoryMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        // Try to get the current value from the value editor
        ValueType value;
        if (valueEditorPtr->getValueFromString(&value))
        {
            if (posAndScaleOut)
            {
                posAndScaleOut[2] = value.f32;
            }
            return true;
        }
    }

    return false;
}

uint8_t *getFinalAddressFromPointerPath(uint8_t *address, const int32_t *addressOffsets, uint32_t totalAddressOffsets)
{
    // Make sure the address is valid
    if (!ptrIsValid(address))
    {
        return nullptr;
    }

    // Make sure totalAddressOffsets is valid
    if (totalAddressOffsets > MAX_MEMORY_WATCH_ADDRESS_OFFSETS)
    {
        return nullptr;
    }

    // If there are no additonal offsets, then just return the current address
    if ((totalAddressOffsets == 0) || !addressOffsets)
    {
        return address;
    }

    // Loop through the offsets and make sure each one results in a proper address
    for (uint32_t i = 0; i < totalAddressOffsets; i++)
    {
        address = *reinterpret_cast<uint8_t **>(address);
        if (!ptrIsValid(address))
        {
            return nullptr;
        }

        address += addressOffsets[i];
        if (!ptrIsValid(address))
        {
            return nullptr;
        }
    }

    return address;
}

static uint8_t *fixBaseAddress(uint8_t *address, VariableType type)
{
    uint32_t typeSize;
    switch (type)
    {
        case VariableType::time:
        case VariableType::s64:
        case VariableType::u64:
        case VariableType::f64:
        {
            typeSize = 8;
            break;
        }
        case VariableType::s32:
        case VariableType::u32:
        case VariableType::f32:
        {
            typeSize = 4;
            break;
        }
        case VariableType::s16:
        case VariableType::u16:
        {
            typeSize = 2;
            break;
        }
        case VariableType::s8:
        case VariableType::u8:
        default:
        {
            typeSize = 1;
        }
    }

    // Make sure the address does not exceed 0x817FFFFF/0xC17FFFFF
    // Arbitrary check for cached memory
    uint8_t *addressCap;
    if (address < reinterpret_cast<uint8_t *>(0x90000000))
    {
        addressCap = reinterpret_cast<uint8_t *>(0x81800000);
    }
    else
    {
        addressCap = reinterpret_cast<uint8_t *>(0xC1800000);
    }

    while ((address + typeSize - 1) >= addressCap)
    {
        address--;
    }

    return address;
}
