#ifndef MEMORY_WATCH_H
#define MEMORY_WATCH_H

#include "mod.h"

#include <cstdint>

#define MAX_MEMORY_WATCHES 100
#define MAX_MEMORY_WATCH_ADDRESS_OFFSETS 20

class MemoryWatchEntry
{
   public:
    MemoryWatchEntry() {}
    ~MemoryWatchEntry() {}

    void init(uint32_t entryId);

    bool addAddressOffset();
    bool removeAddressOffset();

    uint8_t *getAddressPtr() const { return this->address; }
    void setAddress(uint8_t *address) { this->address = address; }

    int32_t *getAddressOffsetsPtr() const { return this->addressOffsets; }

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

    float getPosY() const { return this->posY; }
    float *getPosYPtr() { return &this->posY; }

    float getScale() const { return this->scale; }
    float *getScalePtr() { return &this->scale; }
    void setScale(float scale) { this->scale = scale; }

    uint32_t getTotalAddressOffsets() const { return this->totalAddressOffsets; }

    VariableType getType() const { return this->type; }
    void setType(VariableType type) { this->type = type; }

    bool shouldShowAsHex() const { return this->showAsHex; }

    bool toggleShowAsHex()
    {
        const bool showAsHex = !this->showAsHex;
        this->showAsHex = showAsHex;
        return showAsHex;
    }

    bool shouldDisplay() const { return this->display; }

    bool toggleDisplay()
    {
        const bool display = !this->display;
        this->display = display;
        return display;
    }

    const char *getNamePtrConst() const { return &this->name[0]; }
    char *getNamePtr() { return &this->name[0]; }
    uint32_t getNameSize() const { return sizeof(this->name); }

    void getValueString(char *stringOut, uint32_t stringSize) const;

    bool shouldDraw(uint32_t watchIndex) const;

    void draw(uint32_t watchIndex) const;

   private:
    uint8_t *address;
    int32_t *addressOffsets; // Amount is determined by totalAddressOffsets

    float posX;
    float posY;
    float scale;

    uint8_t totalAddressOffsets;
    VariableType type;
    bool showAsHex;
    bool display;

    char name[16];
};

class MemoryWatch
{
   public:
    MemoryWatch() {}
    ~MemoryWatch() {}

    MemoryWatchEntry *getEntriesPtr() { return this->entries; }

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
bool positioningOrScalingWatch(uint32_t watchIndex);
bool getWatchPosAndScale(float posAndScaleOut[3], uint32_t watchIndex);
uint8_t *getFinalAddressFromPointerPath(uint8_t *address, const int32_t *addressOffsets, uint32_t totalAddressOffsets);
uint8_t *fixBaseAddress(uint8_t *address, VariableType type);

#endif
