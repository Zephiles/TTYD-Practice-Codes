#pragma once

#include <gc/card.h>

#include <cstdint>

namespace mod {

int32_t mountCard(int32_t chn, void *workarea, gc::card::cardcallback detach_cb, 
	gc::card::cardcallback attach_cb);

int32_t readFromCard(gc::card::card_file *file, void *buffer, int32_t len, 
	int32_t offset, gc::card::cardcallback callback);

int32_t writeToCard(gc::card::card_file *file, void *buffer, int32_t len, 
	int32_t offset, gc::card::cardcallback callback);

int32_t createFile(int32_t chn, const char *fileName, uint32_t size, 
	gc::card::card_file *file, gc::card::cardcallback callback);

int32_t setFileStatus(int32_t chn, int32_t fileNo, gc::card::card_stat *stats, 
	gc::card::cardcallback callback);

int32_t loopUntilSynced();
int32_t openFile(const char *description, gc::card::card_file *fileInfo, uint8_t *workArea);
bool loadSettings(gc::card::card_file *fileInfo);
bool writeSettings(const char *description, gc::card::card_file *fileInfo);
bool createSettingsFile(const char *fileName, const char *description, gc::card::card_file *fileInfo);

}