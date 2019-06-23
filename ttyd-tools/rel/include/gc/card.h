#pragma once

#include <cstdint>

namespace gc::card {

#define CARD_SLOT_A 				0		// Memory card slot A
#define CARD_SLOT_B 				1		// Memory card slot B

#define CARD_FILENAME_MAX 			32		// Maximum filename length, excluding null terminator
#define CARD_MAX_ICONS 				8

#define CARD_RESULT_UNLOCKED 		1		// Card is being unlocked or is already unlocked
#define CARD_RESULT_READY			0		// Card is ready
#define CARD_RESULT_BUSY 			-1		// Card is busy
#define CARD_RESULT_WRONGDEVICE 	-2		// Wrong device connected in slot
#define CARD_RESULT_NOCARD 			-3		// No memory card in slot
#define CARD_RESULT_NOFILE 			-4		// Specified file not found
#define CARD_RESULT_IOERROR 		-5		// Internal EXI I/O error
#define CARD_RESULT_BROKEN 			-6		// Directory structure or file entry broken
#define CARD_RESULT_EXIST 			-7		// File already exists with the specified parameters
#define CARD_RESULT_NOENT 			-8		// Found no empty block to create the file
#define CARD_RESULT_INSSPACE 		-9		// Not enough space to write the file to the memory card
#define CARD_RESULT_NOPERM 			-10		// Not enough permissions to operate on the file
#define CARD_RESULT_LIMIT 			-11		// Card size limit reached
#define CARD_RESULT_NAMETOOLONG 	-12		// Filename too long
#define CARD_RESULT_ENCODING 		-13		// Font encoding PAL/SJIS mismatch
#define CARD_RESULT_CANCELED 		-14		// Card operation canceled
#define CARD_RESULT_FATAL_ERROR 	-128	// Fatal error, non-recoverable

typedef struct CARDFileInfo {
	int32_t 	channel;
	int32_t 	fileNum;
	int32_t 	offset;
	int32_t 	length;
	uint16_t 	iBlock;
	uint16_t 	padding;
} __attribute__((__packed__)) CARDFileInfo;

typedef struct CARDStat {
	char 		fileName[CARD_FILENAME_MAX];
	uint32_t	length;
	uint32_t	time;
	uint8_t 	gameName[4];
	uint8_t 	company[2];
	uint8_t 	bannerFormat;
	uint8_t 	padding;
	uint32_t 	iconAddress;
	uint16_t 	iconFormat;
	uint16_t 	iconSpeed;
	uint32_t 	commentAddress;
	uint32_t 	offsetBanner;
	uint32_t 	offsetBannerTlut;
	uint32_t 	offsetIcon[CARD_MAX_ICONS];
	uint32_t 	offsetIconTlut;
	uint32_t 	offsetData;
} __attribute__((__packed__)) CARDStat;

typedef void (*CARDCallback)(int32_t channel, int32_t result);

extern "C" {

int32_t CARDGetResultCode(int32_t channel);
int32_t CARDProbeEx(int32_t channel, int32_t *memSize, int32_t *sectorSize);
int32_t CARDMountAsync(int32_t channel, void *workArea, CARDCallback detachCallback, CARDCallback attachCallback);
int32_t CARDUnmount(int32_t channel);
int32_t CARDOpen(int32_t channel, const char *fileName, CARDFileInfo *fileInfo);
int32_t CARDClose(CARDFileInfo *fileInfo);
int32_t CARDCreateAsync(int32_t channel, const char *fileName, uint32_t size, CARDFileInfo *fileInfo, CARDCallback callback);
int32_t CARDReadAsync(CARDFileInfo *fileInfo, void *buffer, int32_t length, int32_t offset, CARDCallback callback);
int32_t CARDWriteAsync(CARDFileInfo *fileInfo, void *buffer, int32_t length, int32_t offset, CARDCallback callback);
int32_t CARDDelete(int32_t channel, const char *fileName);
int32_t CARDGetStatus(int32_t channel, int32_t fileNum, CARDStat *stat);
int32_t CARDSetStatusAsync(int32_t channel, int32_t fileNum, CARDStat *stat, CARDCallback callback);

}

}