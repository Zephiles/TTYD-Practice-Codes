#pragma once

#include <gc/card.h>

#include <cstdint>

namespace mod {

int32_t mountCard(int32_t channel, void *workArea, 
	gc::card::CARDCallback detachCallback, gc::card::CARDCallback attachCallback);

int32_t readFromCard(gc::card::CARDFileInfo *fileInfo, void *buffer, 
	int32_t length, int32_t offset, gc::card::CARDCallback callback);

int32_t writeToCard(gc::card::CARDFileInfo *fileInfo, void *buffer, 
	int32_t length, int32_t offset, gc::card::CARDCallback callback);

int32_t createFile(int32_t channel, char *fileName, uint32_t size, 
	gc::card::CARDFileInfo *fileInfo, gc::card::CARDCallback callback);

int32_t setFileStatus(int32_t channel, int32_t fileNum, 
	gc::card::CARDStat *stat, gc::card::CARDCallback callback);

int32_t finishAsyncFunction(int32_t returnCode);

int32_t loadSettings(char *settingsFileName, void *workArea);

int32_t writeSettings(char *settingsDescription, 
	char *settingsFileName, char *relFileName, void *workArea);

int32_t createSettingsFile(char *settingsFileName, 
	char *relFileName, gc::card::CARDFileInfo *settingsFileInfo);

}