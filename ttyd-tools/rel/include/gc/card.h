#pragma once

#include <cstdint>

namespace gc::card {

extern "C" {

#define CARD_SLOTA					0			/*!< memory card slot A */
#define CARD_SLOTB					1			/*!< memory card slot B */

#define CARD_FILENAMELEN			32			/*!< maximum filename length */	
#define CARD_MAXICONS				8

#define CARD_ERROR_UNLOCKED			1			/*!< card beeing unlocked or allready unlocked. */
#define CARD_ERROR_READY            0			/*!< card is ready. */
#define CARD_ERROR_BUSY            -1			/*!< card is busy. */
#define CARD_ERROR_WRONGDEVICE     -2			/*!< wrong device connected in slot */
#define CARD_ERROR_NOCARD          -3			/*!< no memory card in slot */
#define CARD_ERROR_NOFILE          -4			/*!< specified file not found */
#define CARD_ERROR_IOERROR         -5			/*!< internal EXI I/O error */
#define CARD_ERROR_BROKEN          -6			/*!< directory structure or file entry broken */
#define CARD_ERROR_EXIST           -7			/*!< file allready exists with the specified parameters */
#define CARD_ERROR_NOENT           -8			/*!< found no empty block to create the file */
#define CARD_ERROR_INSSPACE        -9			/*!< not enough space to write file to memory card */
#define CARD_ERROR_NOPERM          -10			/*!< not enough permissions to operate on the file */
#define CARD_ERROR_LIMIT           -11			/*!< card size limit reached */
#define CARD_ERROR_NAMETOOLONG     -12			/*!< fileName too long */
#define CARD_ERROR_ENCODING        -13			/*!< font encoding PAL/SJIS mismatch*/
#define CARD_ERROR_CANCELED        -14			/*!< card operation canceled */
#define CARD_ERROR_FATAL_ERROR     -128			/*!< fatal error, non recoverable */

typedef struct _card_file {
	int32_t chn;
	int32_t fileNum;
	int32_t offset;
	int32_t len;
	uint16_t iBlock;
} card_file;

typedef struct _card_dir { 
	int32_t chn; 
	uint32_t fileNo; 
	uint32_t fileLen;
	uint8_t permissions;
	uint8_t fileName[CARD_FILENAMELEN]; 
	uint8_t gameCode[4]; 
	uint8_t company[2];
	bool showAll;
} card_dir; 

typedef struct _card_stat {
	char fileName[CARD_FILENAMELEN]; 
	uint32_t  len; 
	uint32_t  time;
	uint8_t   gameCode[4]; 
	uint8_t   company[2]; 
	uint8_t   banner_fmt; 
	uint32_t  icon_addr;
	uint16_t  icon_fmt; 
	uint16_t  icon_speed; 
	uint32_t  comment_addr;
	uint32_t  offset_banner; 
	uint32_t  offset_banner_tlut; 
	uint32_t  offset_icon[CARD_MAXICONS]; 
	uint32_t  offset_icon_tlut; 
	uint32_t  offset_data; 
} card_stat;

typedef void (*cardcallback)(int32_t chan, int32_t result);

int32_t CARDGetResultCode(int32_t chn);
int32_t CARDProbeEx(int32_t chn, int32_t *mem_size, int32_t *sect_size);
int32_t CARDMountAsync(int32_t chn, void *workarea, cardcallback detach_cb, cardcallback attach_cb);
int32_t CARDUnmount(int32_t chn);
int32_t CARDOpen(int32_t chn, const char *fileName, card_file *file);
int32_t CARDClose(card_file *file);
int32_t CARDCreateAsync(int32_t chn, const char *fileName, uint32_t size, card_file *file, cardcallback callback);
int32_t CARDReadAsync(card_file *file, void *buffer, int32_t len, int32_t offset, cardcallback callback);
int32_t CARDWriteAsync(card_file *file, void *buffer, int32_t len, int32_t offset, cardcallback callback);
int32_t CARDGetStatus(int32_t chn, int32_t fileNo, card_stat *stats);
int32_t CARDSetStatusAsync(int32_t chn, int32_t fileNo, card_stat *stats, cardcallback callback);

}

}