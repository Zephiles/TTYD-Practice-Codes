#include "memcard.h"
#include "global.h"
#include "commonfunctions.h"

#include <gc/card.h>
#include <ttyd/__mem.h>
#include <ttyd/string.h>

#include <cstdio>

namespace mod {

int32_t mountCard(int32_t chn, void *workarea, gc::card::cardcallback detach_cb, 
	gc::card::cardcallback attach_cb)
{
	int32_t ReturnCode = gc::card::CARDMountAsync(chn, workarea, detach_cb, attach_cb);
	if (ReturnCode == CARD_ERROR_READY)
	{
		// Make sure the memory card is done being mounted
		return loopUntilSynced();
	}
	return ReturnCode;
}

int32_t readFromCard(gc::card::card_file *file, void *buffer, int32_t len, 
	int32_t offset, gc::card::cardcallback callback)
{
	int32_t ReturnCode = gc::card::CARDReadAsync(file, buffer, len, offset, callback);
	if (ReturnCode == CARD_ERROR_READY)
	{
		// Make sure the memory card is done being read from
		return loopUntilSynced();
	}
	return ReturnCode;
}

int32_t writeToCard(gc::card::card_file *file, void *buffer, int32_t len, 
	int32_t offset, gc::card::cardcallback callback)
{
	int32_t ReturnCode = gc::card::CARDWriteAsync(file, buffer, len, offset, callback);
	if (ReturnCode == CARD_ERROR_READY)
	{
		// Make sure the memory card is done being written to
		return loopUntilSynced();
	}
	return ReturnCode;
}

int32_t createFile(int32_t chn, char *fileName, uint32_t size, 
	gc::card::card_file *file, gc::card::cardcallback callback)
{
	int32_t ReturnCode = gc::card::CARDCreateAsync(chn, fileName, size, file, callback);
	if (ReturnCode == CARD_ERROR_READY)
	{
		// Make sure the file is done being created
		return loopUntilSynced();
	}
	return ReturnCode;
}

int32_t setFileStatus(int32_t chn, int32_t fileNo, gc::card::card_stat *stats, 
	gc::card::cardcallback callback)
{
	int32_t ReturnCode = gc::card::CARDSetStatusAsync(chn, fileNo, stats, callback);
	if (ReturnCode == CARD_ERROR_READY)
	{
		// Make sure the status is done being set
		return loopUntilSynced();
	}
	return ReturnCode;
}

int32_t loopUntilSynced()
{
	int32_t ReturnCode;
	do
	{
		ReturnCode = gc::card::CARDGetResultCode(CARD_SLOTA);
	}
	while (ReturnCode == CARD_ERROR_BUSY);
	return ReturnCode;
}

int32_t loadSettings(char *fileName, gc::card::card_file *fileInfo, uint8_t *workArea)
{
	// Load the settings file
	int32_t ReturnCode = gc::card::CARDProbeEx(CARD_SLOTA, nullptr, nullptr);
	if (ReturnCode != CARD_ERROR_READY)
	{
		return ReturnCode;
	}
	
	// Mount the memory card in slot A
	ReturnCode = mountCard(CARD_SLOTA, workArea, nullptr, nullptr);
	if (ReturnCode != CARD_ERROR_READY)
	{
		return ReturnCode;
	}
	
	// Open the settings file
	ReturnCode = gc::card::CARDOpen(CARD_SLOTA, fileName, fileInfo);
	if (ReturnCode != CARD_ERROR_READY)
	{
		gc::card::CARDUnmount(CARD_SLOTA);
		return ReturnCode;
	}
	
	// Set up the array to hold the area of the file that contains the size
	char *tempFileData = new char[0x200];
	
	// Get the data from the area that holds the size
	ReturnCode = readFromCard(fileInfo, tempFileData, 0x200, 0x2000, nullptr);
	if (ReturnCode != CARD_ERROR_READY)
	{
		delete[] (tempFileData);
		gc::card::CARDClose(fileInfo);
		gc::card::CARDUnmount(CARD_SLOTA);
		return ReturnCode;
	}
	
	// Get the size of the file
	uint32_t StoredFileSize = *reinterpret_cast<uint32_t *>(&tempFileData[0x40]);
	
	// Delete the data that holds the size, as it's not needed anymore
	delete[] (tempFileData);
	
	// Adjust the file size to be in multiples of 0x2000, rounding up
	uint32_t StoredFileSizeAdjusted = (StoredFileSize + 0x2000 - 1) & ~(0x2000 - 1);
	
	// Make sure the stored file size is at least 0x2000
	if (StoredFileSizeAdjusted < 0x2000)
	{
		StoredFileSizeAdjusted = 0x2000;
	}
	
	// Get the size needed to be read
	uint32_t FileSize = sizeof(struct SettingsStruct) + 0x200;
	
	// Adjust the struct size to be in multiples of 0x2000, rounding up
	uint32_t FileSizeAdjusted = (FileSize + 0x2000 - 1) & ~(0x2000 - 1);
	
	// Set up the memory to be copied from the file
	char *MiscData = new char[FileSizeAdjusted];
	clearMemory(MiscData, FileSizeAdjusted);
	
	// Get the data from the file
	// Must read by the stored size, as the struct size may exceed the size of the file
	ReturnCode = readFromCard(fileInfo, MiscData, StoredFileSizeAdjusted, 0x2000, nullptr);
	
	// Close and unmount the card, as it's no longer needed
	gc::card::CARDClose(fileInfo);
	gc::card::CARDUnmount(CARD_SLOTA);
	
	if (ReturnCode != CARD_ERROR_READY)
	{
		delete[] (MiscData);
		return ReturnCode;
	}
	
	// Get the settings struct from the file
	SettingsStruct *Settings = reinterpret_cast<SettingsStruct *>(&MiscData[0x200]);
	
	// Get the desired variables from the struct
	uint32_t CheatsSize = sizeof(Cheat) / sizeof(Cheat[0]);
	for (uint32_t i = 0; i < CheatsSize; i++)
	{
		uint32_t CurrentIndex 				= CheatsOrder[i];
		Cheat[CurrentIndex].Active 			= Settings->CheatsActive[i];
		Cheat[CurrentIndex].ButtonCombo 	= Settings->CheatButtonCombos[i];
	}
	
	uint32_t DisplaysSize = sizeof(Displays);
	for (uint32_t i = 0; i < DisplaysSize; i++)
	{
		uint32_t CurrentIndex 	= DisplaysOrder[i];
		Displays[CurrentIndex] 	= Settings->DisplaysActive[i];
	}
	
	OnScreenTimer.ButtonCombo[0] = Settings->DisplaysButtonCombos[ONSCREEN_TIMER];
	OnScreenTimer.ButtonCombo[1] = Settings->DisplaysButtonCombos[ONSCREEN_TIMER + 1];
	
	ttyd::__mem::memcpy(MemoryWatch, &Settings->MemoryWatchSettings[0], sizeof(MemoryWatch));
	
	delete[] (MiscData);
	return CARD_ERROR_READY;
}

int32_t writeSettings(char *description, char *fileName, 
	gc::card::card_file *fileInfo, uint8_t *workArea)
{
	// Load the settings file
	int32_t ReturnCode = gc::card::CARDProbeEx(CARD_SLOTA, nullptr, nullptr);
	if (ReturnCode != CARD_ERROR_READY)
	{
		return ReturnCode;
	}
	
	// Mount the memory card in slot A
	ReturnCode = mountCard(CARD_SLOTA, workArea, nullptr, nullptr);
	if (ReturnCode != CARD_ERROR_READY)
	{
		return ReturnCode;
	}
	
	// Get the size thats going to be written
	uint32_t FileSize = sizeof(struct SettingsStruct) + 0x200;
	
	// Adjust the file size to be in multiples of 0x2000, rounding up
	uint32_t FileSizeAdjusted = (FileSize + 0x2000 - 1) & ~(0x2000 - 1);
	
	// Open the settings file if it exists
	ReturnCode = gc::card::CARDOpen(CARD_SLOTA, fileName, fileInfo);
	switch (ReturnCode)
	{
		case CARD_ERROR_READY:
		{
			// Set up the array to hold the area of the file that contains the size
			char *tempFileData = new char[0x200];
			
			// Get the data from the area that holds the size
			ReturnCode = readFromCard(fileInfo, tempFileData, 0x200, 0x2000, nullptr);
			if (ReturnCode != CARD_ERROR_READY)
			{
				delete[] (tempFileData);
				gc::card::CARDClose(fileInfo);
				gc::card::CARDUnmount(CARD_SLOTA);
				return ReturnCode;
			}
			
			// Get the size of the file
			uint32_t StoredFileSize = *reinterpret_cast<uint32_t *>(&tempFileData[0x40]);
			
			// Delete the data that holds the size, as it's not needed anymore
			delete[] (tempFileData);
			
			// Adjust the file size to be in multiples of 0x2000, rounding up
			uint32_t StoredFileSizeAdjusted = (StoredFileSize + 0x2000 - 1) & ~(0x2000 - 1);
			
			// Make sure the size being written does not exceed the current size
			if (FileSizeAdjusted > StoredFileSizeAdjusted)
			{
				// The new size exceeds the current size, so a new file must be created
				// Close the file
				ReturnCode = gc::card::CARDClose(fileInfo);
				if (ReturnCode != CARD_ERROR_READY)
				{
					gc::card::CARDUnmount(CARD_SLOTA);
					return ReturnCode;
				}
				
				// Delete the current file
				ReturnCode = gc::card::CARDDelete(CARD_SLOTA, fileName);
				if (ReturnCode != CARD_ERROR_READY)
				{
					gc::card::CARDUnmount(CARD_SLOTA);
					return ReturnCode;
				}
				
				// Make the new file
				// createSettingsFile keeps the file open, but closes and unmounts if it fails to create the file
				ReturnCode = createSettingsFile(fileName, description, fileInfo);
				if (ReturnCode != CARD_ERROR_READY)
				{
					return ReturnCode;
				}
			}
			break;
		}
		case CARD_ERROR_NOFILE:
		{
			// Settings file does not exist, so create it
			// createSettingsFile keeps the file open, but closes and unmounts if it fails to create the file
			ReturnCode = createSettingsFile(fileName, description, fileInfo);
			if (ReturnCode != CARD_ERROR_READY)
			{
				return ReturnCode;
			}
			break;
		}
		default:
		{
			return ReturnCode;
		}
	}
	
	// Set up the memory to be written to the file
	char *MiscData = new char[FileSizeAdjusted];
	clearMemory(MiscData, FileSizeAdjusted);
	
	// Copy the name, description, and file size into the memory
	ttyd::string::strcpy(MiscData, "Paper Mario");
	ttyd::string::strcpy(&MiscData[0x20], description);
	*reinterpret_cast<uint32_t *>(&MiscData[0x40]) = FileSize;
	
	// Set up the struct to hold the variables to store
	SettingsStruct *Settings = reinterpret_cast<SettingsStruct *>(&MiscData[0x200]);
	
	// Copy the desired variables to the struct
	uint32_t CheatsSize = sizeof(Cheat) / sizeof(Cheat[0]);
	for (uint32_t i = 0; i < CheatsSize; i++)
	{
		uint32_t CurrentIndex 			= CheatsOrder[i];
		Settings->CheatsActive[i] 		= Cheat[CurrentIndex].Active;
		Settings->CheatButtonCombos[i] 	= Cheat[CurrentIndex].ButtonCombo;
	}
	
	uint32_t DisplaysSize = sizeof(Displays);
	for (uint32_t i = 0; i < DisplaysSize; i++)
	{
		uint32_t CurrentIndex 			= DisplaysOrder[i];
		Settings->DisplaysActive[i] 	= Displays[CurrentIndex];
	}
	
	Settings->DisplaysButtonCombos[ONSCREEN_TIMER] 		= OnScreenTimer.ButtonCombo[0];
	Settings->DisplaysButtonCombos[ONSCREEN_TIMER + 1] 	= OnScreenTimer.ButtonCombo[1];
	
	ttyd::__mem::memcpy(&Settings->MemoryWatchSettings[0], MemoryWatch, sizeof(MemoryWatch));
	
	// Write the data to the file
	ReturnCode = writeToCard(fileInfo, MiscData, FileSizeAdjusted, 0x2000, nullptr);
	
	delete[] (MiscData);
	gc::card::CARDClose(fileInfo);
	gc::card::CARDUnmount(CARD_SLOTA);
	return ReturnCode;
}

int32_t createSettingsFile(char *fileName, char *description, gc::card::card_file *fileInfo)
{
	// Get the banner and icon data from the current REL file
	char *relFileName = const_cast<char *>("rel");
	int32_t ReturnCode = gc::card::CARDOpen(CARD_SLOTA, relFileName, fileInfo);
	if (ReturnCode != CARD_ERROR_READY)
	{
		gc::card::CARDUnmount(CARD_SLOTA);
		return ReturnCode;
	}
	
	// Set up the memory for the banner and icon data to go in
	char *BannerIconData = new char[0x2000];
	
	// Get the banner and icon data
	ReturnCode = readFromCard(fileInfo, BannerIconData, 0x2000, 0, nullptr);
	if (ReturnCode != CARD_ERROR_READY)
	{
		delete[] (BannerIconData);
		gc::card::CARDClose(fileInfo);
		gc::card::CARDUnmount(CARD_SLOTA);
		return ReturnCode;
	}
	
	// Close the current REL file
	ReturnCode = gc::card::CARDClose(fileInfo);
	if (ReturnCode != CARD_ERROR_READY)
	{
		delete[] (BannerIconData);
		gc::card::CARDUnmount(CARD_SLOTA);
		return ReturnCode;
	}
	
	// Get the size of the new file
	uint32_t SettingsStructSize = sizeof(struct SettingsStruct);
	uint32_t FileSize = 0x2000 + SettingsStructSize + 0x200;
	
	// Adjust the file size to be in multiples of 0x2000, rounding up
	uint32_t FileSizeAdjusted = (FileSize + 0x2000 - 1) & ~(0x2000 - 1);
	
	// Create the new file
	ReturnCode = createFile(CARD_SLOTA, fileName, FileSizeAdjusted, fileInfo, nullptr);
	if (ReturnCode != CARD_ERROR_READY)
	{
		delete[] (BannerIconData);
		gc::card::CARDUnmount(CARD_SLOTA);
		return ReturnCode;
	}
	
	// Get the stats for the new file
	gc::card::card_stat tempCardStats;
	int32_t FileNumber = fileInfo->fileNum;
	
	ReturnCode = gc::card::CARDGetStatus(CARD_SLOTA, FileNumber, &tempCardStats);
	if (ReturnCode != CARD_ERROR_READY)
	{
		delete[] (BannerIconData);
		gc::card::CARDUnmount(CARD_SLOTA);
		return ReturnCode;
	}
	
	// Modify the stats for the new file
	tempCardStats.banner_fmt 		= 2;
	tempCardStats.icon_addr 		= 0;
	tempCardStats.icon_fmt 			= 2;
	tempCardStats.icon_speed 		= 3;
	tempCardStats.comment_addr 		= 0x2000;
	
	// Set the stats for the new file
	ReturnCode = setFileStatus(CARD_SLOTA, FileNumber, &tempCardStats, nullptr);
	if (ReturnCode != CARD_ERROR_READY)
	{
		delete[] (BannerIconData);
		gc::card::CARDUnmount(CARD_SLOTA);
		return ReturnCode;
	}
	
	// Open the new file
	ReturnCode = gc::card::CARDOpen(CARD_SLOTA, fileName, fileInfo);
	if (ReturnCode != CARD_ERROR_READY)
	{
		delete[] (BannerIconData);
		gc::card::CARDUnmount(CARD_SLOTA);
		return ReturnCode;
	}
	
	// Write the banner and icon data to the new file
	ReturnCode = writeToCard(fileInfo, BannerIconData, 0x2000, 0, nullptr);
	
	// Delete the data for the icon and banner, as they are not needed anymore
	delete[] (BannerIconData);
	
	if (ReturnCode != CARD_ERROR_READY)
	{
		gc::card::CARDClose(fileInfo);
		gc::card::CARDUnmount(CARD_SLOTA);
	}
	
	return ReturnCode;
}

}