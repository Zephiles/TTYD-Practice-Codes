#pragma once

#include <gc/card.h>

#include <cstdint>

namespace mod {

int32_t checkForMemoryCard(int32_t memoryCardSlot);

int32_t finishAsyncFunction(int32_t memoryCardSlot, int32_t returnCode);

int32_t mountCard(int32_t memoryCardSlot);

int32_t createFileOnCard(int32_t memoryCardSlot, const char *fileName, uint32_t size, gc::card::CARDFileInfo *fileInfo);

int32_t readFromFileOnCard(int32_t memoryCardSlot, gc::card::CARDFileInfo *fileInfo, void *buffer, int32_t length, int32_t offset);

int32_t writeToFileOnCard(int32_t memoryCardSlot, gc::card::CARDFileInfo *fileInfo, void *buffer, int32_t length, int32_t offset);

int32_t setFileStatus(int32_t memoryCardSlot, int32_t fileNum, gc::card::CARDStat *stat);

int32_t renameFileOnCard(int32_t memoryCardSlot, const char *oldName, const char *newName);

int32_t createSettingsFile(int32_t memoryCardSlot, gc::card::CARDFileInfo *settingsFileInfo);

int32_t saveSettings(int32_t memoryCardSlot);

int32_t loadSettings(int32_t memoryCardSlot);

int32_t renameSettingsFile(int32_t memoryCardSlot);

int32_t deleteSettingsFile(int32_t memoryCardSlot);

}