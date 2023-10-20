#ifndef MEMORY_WATCH_H
#define MEMORY_WATCH_H

#include "mod.h"

#include <cstdint>

#define MAX_MEMORY_WATCHES 100
#define MAX_MEMORY_WATCH_ADDRESS_OFFSETS 20
#define MEMORY_WATCH_NAME_SIZE 16

class MemoryWatchEntry
{
   public:
    MemoryWatchEntry() {}
    ~MemoryWatchEntry() {}

    void init(uint32_t entryId);

    bool addAddressOffset();
    bool removeAddressOffset();

    int32_t *getAddressOffsetsPtr() const { return this->addressOffsets; }
    void setAddressOffsetsPtr(int32_t *offsetsPtr) { this->addressOffsets = offsetsPtr; }

    uint8_t *getAddressPtr() const { return this->address; }
    void setAddress(uint8_t *address) { this->address = address; }

    void freeAddressOffsets()
    {
        int32_t *addressOffsetsPtr = this->addressOffsets;
        if (addressOffsetsPtr)
        {
            delete[] addressOffsetsPtr;
            this->addressOffsets = nullptr;
        }
    }

    uint32_t initAddressOffsetsPtr(uint32_t totalOffsets)
    {
        // Make sure totalOffsets is valid
        if (totalOffsets > MAX_MEMORY_WATCH_ADDRESS_OFFSETS)
        {
            totalOffsets = MAX_MEMORY_WATCH_ADDRESS_OFFSETS;
        }

        // If memory is already allocated for the entries, then free it
        this->freeAddressOffsets();

        // Set the new total
        this->totalAddressOffsets = totalOffsets;

        // Allocate memory for the entries
        this->addressOffsets = new int32_t[totalOffsets];
        return totalOffsets;
    }

    int32_t getAddressOffset(uint32_t index)
    {
        // Make sure the index is valid
        if (index >= MAX_MEMORY_WATCH_ADDRESS_OFFSETS)
        {
            return 0;
        }

        return this->addressOffsets[index];
    }

    float getPosX() const { return this->posX; }
    float *getPosXPtr() { return &this->posX; }
    void setPosX(float pos) { this->posX = pos; }

    float getPosY() const { return this->posY; }
    float *getPosYPtr() { return &this->posY; }
    void setPosY(float pos) { this->posY = pos; }

    float getScale() const { return this->scale; }
    float *getScalePtr() { return &this->scale; }
    void setScale(float scale) { this->scale = scale; }

    uint32_t getTotalAddressOffsets() const { return this->totalAddressOffsets; }

    void setTotalAddressOffsets(uint32_t totalAddressOffsets)
    {
        this->totalAddressOffsets = static_cast<uint8_t>(totalAddressOffsets);
    }

    VariableType getType() const { return this->type; }
    void setType(VariableType type) { this->type = type; }

    bool shouldShowAsHex() const { return this->showAsHex; }
    void setShowAsHex(bool showAsHex) { this->showAsHex = showAsHex; }

    bool toggleShowAsHex()
    {
        const bool showAsHex = !this->showAsHex;
        this->showAsHex = showAsHex;
        return showAsHex;
    }

    bool shouldDisplay() const { return this->display; }
    void setDisplay(bool display) { this->display = display; }

    bool toggleDisplay()
    {
        const bool display = !this->display;
        this->display = display;
        return display;
    }

    const char *getNamePtrConst() const { return &this->name[0]; }
    char *getNamePtr() { return &this->name[0]; }

    void getValueString(char *stringOut, uint32_t stringSize) const;

    bool shouldDraw(uint32_t watchIndex) const;

    void draw(uint32_t watchIndex) const;

   private:
    int32_t *addressOffsets; // Amount is determined by totalAddressOffsets
    uint8_t *address;

    float posX;
    float posY;
    float scale;

    uint8_t totalAddressOffsets;
    VariableType type;
    bool showAsHex;
    bool display;

    char name[MEMORY_WATCH_NAME_SIZE];
};

class MemoryWatch
{
   public:
    MemoryWatch() {}
    ~MemoryWatch() {}

    MemoryWatchEntry *getEntriesPtr() { return this->entries; }

    MemoryWatchEntry *reinitEntries(uint32_t totalWatches)
    {
        // If memory is already allocated for the entries, then free it
        const uint32_t totalEntries = this->totalEntries;
        MemoryWatchEntry *entriesPtr = this->entries;
        if ((totalEntries > 0) && entriesPtr)
        {
            // Free the memory used by the address offsets of each watch
            for (uint32_t i = 0; i < totalEntries; i++)
            {
                entriesPtr[i].freeAddressOffsets();
            }

            // Free the main memory
            delete[] entriesPtr;
        }

        // Make sure totalWatches is valid
        if (totalWatches > MAX_MEMORY_WATCHES)
        {
            totalWatches = MAX_MEMORY_WATCHES;
        }

        // Set the new total
        this->totalEntries = static_cast<uint8_t>(totalWatches);

        // Allocate memory for the entries
        entriesPtr = new MemoryWatchEntry[totalWatches];
        this->entries = entriesPtr;
        return entriesPtr;
    }

    uint32_t getTotalEntries() const { return this->totalEntries; }
    bool limitHasBeenReached() const { return this->totalEntries >= MAX_MEMORY_WATCHES; }

    MemoryWatchEntry *addWatch();
    bool duplicateWatch(uint32_t index);
    bool swapWatches(uint32_t watchOneIndex, uint32_t watchTwoIndex);
    bool moveWatch(uint32_t destinationIndex, uint32_t sourceIndex);
    bool deleteWatch(uint32_t index);

   private:
    MemoryWatchEntry *entries; // Amount is determined by totalEntries
    uint8_t totalEntries;
};

// The MemoryWatch class is very small, so making it dynamic would waste a lot of memory
extern MemoryWatch gMemoryWatch;

void handleMemoryWatches();
uint8_t *getFinalAddressFromPointerPath(uint8_t *address, const int32_t *addressOffsets, uint32_t totalAddressOffsets);

#endif
