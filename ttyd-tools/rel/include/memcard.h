#pragma once

#include <gc/card.h>

#include <cstdint>

namespace mod {

int32_t finishAsyncFunction(int32_t returnCode);

int32_t checkForMemoryCard();

int32_t mountCard();

int32_t unmountCard();

int32_t openFileFromCard(const char *fileName, gc::card::CARDFileInfo *fileInfo);

int32_t closeFileFromCard(gc::card::CARDFileInfo *fileInfo);

int32_t createFileOnCard(const char *fileName, 
	uint32_t size, gc::card::CARDFileInfo *fileInfo);

int32_t readFromFileOnCard(gc::card::CARDFileInfo *fileInfo, 
	void *buffer, int32_t length, int32_t offset);

int32_t writeToFileOnCard(gc::card::CARDFileInfo *fileInfo, 
	void *buffer, int32_t length, int32_t offset);

int32_t deleteFileOnCard(const char *fileName);

int32_t getFileStatus(int32_t fileNum, gc::card::CARDStat *stat);

int32_t setFileStatus(int32_t fileNum, gc::card::CARDStat *stat);

int32_t createSettingsFile(gc::card::CARDFileInfo *settingsFileInfo);

int32_t saveSettings();

int32_t loadSettings();

}