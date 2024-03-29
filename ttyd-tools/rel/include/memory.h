#pragma once

#include <cstdint>

namespace mod {

// Memory Watch stuff
int32_t getEmptyWatchSlot();
const char *getAddressStringNoLetterP(int32_t slot);
const char *getAddressString(int32_t slot);
const char *getAddressStringFromOffsets(int32_t slot, uint32_t maxOffset);
const char *getValueString(int32_t slot);
void addMemoryWatch(int32_t slot);
void deleteWatch(int32_t slot);
void duplicateWatch(int32_t currentSlot, int32_t emptySlot);
uint32_t adjustWatchValueControls(int32_t slot);
void adjustWatchTempValueAndBounds(/* int32_t slot, */ uint32_t highestDigit, int32_t valueChangedBy);
void *fixBaseAddress(int32_t slot, void *address);


// Memory Editor stuff
bool checkIfOtherHeaderOptionsSelected(uint32_t option);
uint8_t *getDigitAddress(uint8_t *address, uint32_t numBytes, uint32_t currentDigit); // This function can return an invalid address
uint8_t *getAndVerifyDigitAddress(uint8_t *address, uint32_t numBytes, uint32_t currentDigit);
int32_t getDigitValue(uint8_t *address, uint32_t numBytes, uint32_t currentDigit);
int32_t getDigitValueFromRawAddress(uint8_t *address, uint32_t numBytes, uint32_t currentDigit);
void adjustDigitValue(uint8_t *address, uint32_t numBytes, uint32_t currentDigit, int32_t value);
void adjustDigitValueFromRawAddress(uint32_t currentDigit, int32_t value);
bool checkIfAddressInHoverRange(uint8_t *address, uint32_t currentDigit);
uint32_t memoryEditorButtonControls();

}