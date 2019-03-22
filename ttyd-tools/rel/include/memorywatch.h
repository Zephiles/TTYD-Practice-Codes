#pragma once

#include <cstdint>

namespace mod {

int32_t getEmptyWatchSlot();
const char *getAddressStringNoLetterP(int32_t slot);
const char *getAddressString(int32_t slot);
const char *getAddressStringFromOffsets(int32_t slot, uint32_t maxOffset);
const char *getValueString(int32_t slot);
void addMemoryWatch(int32_t slot);
void deleteWatch(int32_t slot);
void duplicateWatch(int32_t currentSlot, int32_t emptySlot);
uint32_t adjustWatchValueControls(int32_t slot);
void adjustWatchTempValueAndBounds(int32_t slot, uint32_t highestDigit, int32_t valueChangedBy);
void *fixBaseAddress(int32_t slot, void *address);

}