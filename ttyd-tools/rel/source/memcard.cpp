#include "memcard.h"
#include "global.h"
#include "commonfunctions.h"

#include <gc/card.h>

#include <cstdio>

namespace mod {

int32_t finishAsyncFunction(int32_t returnCode)
{
	if (returnCode == CARD_ERROR_READY)
	{
		do
		{
			returnCode = gc::card::CARDGetResultCode(CARD_SLOTA);
		}
		while (returnCode == CARD_ERROR_BUSY);
	}
	return returnCode;
}

int32_t checkForMemoryCard(int32_t channel, int32_t *memSize, int32_t *sectorSize)
{
	return gc::card::CARDProbeEx(channel, memSize, sectorSize);
}

int32_t mountCard(int32_t channel, void *workArea, 
	gc::card::CARDCallback detachCallback, gc::card::CARDCallback attachCallback)
{
	int32_t ReturnCode = gc::card::CARDMountAsync(channel, workArea, detachCallback, attachCallback);
	return finishAsyncFunction(ReturnCode);
}

int32_t unmountCard(int32_t channel)
{
	return gc::card::CARDUnmount(channel);
}

int32_t openFileFromCard(int32_t channel, char *fileName, gc::card::CARDFileInfo *fileInfo)
{
	return gc::card::CARDOpen(channel, fileName, fileInfo);
}

int32_t closeFileFromCard(gc::card::CARDFileInfo *fileInfo)
{
	return gc::card::CARDClose(fileInfo);
}

void closeFileAndUnmountCard(gc::card::CARDFileInfo *fileInfo, int32_t channel)
{
	closeFileFromCard(fileInfo);
	unmountCard(channel);
}

int32_t createFileOnCard(int32_t channel, char *fileName, uint32_t size, 
	gc::card::CARDFileInfo *fileInfo, gc::card::CARDCallback callback)
{
	int32_t ReturnCode = gc::card::CARDCreateAsync(channel, fileName, size, fileInfo, callback);
	return finishAsyncFunction(ReturnCode);
}

int32_t readFromFileOnCard(gc::card::CARDFileInfo *fileInfo, void *buffer, 
	int32_t length, int32_t offset, gc::card::CARDCallback callback)
{
	int32_t ReturnCode = gc::card::CARDReadAsync(fileInfo, buffer, length, offset, callback);
	return finishAsyncFunction(ReturnCode);
}

int32_t writeToFileOnCard(gc::card::CARDFileInfo *fileInfo, void *buffer, 
	int32_t length, int32_t offset, gc::card::CARDCallback callback)
{
	int32_t ReturnCode = gc::card::CARDWriteAsync(fileInfo, buffer, length, offset, callback);
	return finishAsyncFunction(ReturnCode);
}

int32_t deleteFileOnCard(int32_t channel, char *fileName)
{
	return gc::card::CARDDelete(channel, fileName);
}

int32_t getFileStatus(int32_t channel, int32_t fileNum, gc::card::CARDStat *stat)
{
	return gc::card::CARDGetStatus(channel, fileNum, stat);
}

int32_t setFileStatus(int32_t channel, int32_t fileNum, 
	gc::card::CARDStat *stat, gc::card::CARDCallback callback)
{
	int32_t ReturnCode = gc::card::CARDSetStatusAsync(channel, fileNum, stat, callback);
	return finishAsyncFunction(ReturnCode);
}

int32_t loadSettings(char *settingsFileName, void *workArea)
{
	// Load the settings file
	int32_t ReturnCode = checkForMemoryCard(CARD_SLOTA, nullptr, nullptr);
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
	gc::card::CARDFileInfo FileInfo;
	ReturnCode = openFileFromCard(CARD_SLOTA, settingsFileName, &FileInfo);
	
	if (ReturnCode != CARD_ERROR_READY)
	{
		unmountCard(CARD_SLOTA);
		return ReturnCode;
	}
	
	// Set up the array to hold the area of the file that contains the size
	char *FileData = new char[0x200];
	
	// Get the data from the area that holds the size
	ReturnCode = readFromFileOnCard(&FileInfo, FileData, 0x200, 0x2000, nullptr);
	if (ReturnCode != CARD_ERROR_READY)
	{
		delete[] (FileData);
		closeFileAndUnmountCard(&FileInfo, CARD_SLOTA);
		return ReturnCode;
	}
	
	// Get the size of the file
	uint32_t StoredFileSize = *reinterpret_cast<uint32_t *>(&FileData[0x40]);
	
	// Delete the data that holds the size, as it's not needed anymore
	delete[] (FileData);
	
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
	ReturnCode = readFromFileOnCard(&FileInfo, MiscData, StoredFileSizeAdjusted, 0x2000, nullptr);
	
	// Close and unmount the card, as it's no longer needed
	closeFileAndUnmountCard(&FileInfo, CARD_SLOTA);
	
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
	
	copyMemory(MemoryWatch, Settings->MemoryWatchSettings, sizeof(MemoryWatch));
	
	delete[] (MiscData);
	return CARD_ERROR_READY;
}

int32_t writeSettings(char *settingsDescription, 
	char *settingsFileName, char *relFileName, void *workArea)
{
	// Load the settings file
	int32_t ReturnCode = checkForMemoryCard(CARD_SLOTA, nullptr, nullptr);
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
	gc::card::CARDFileInfo FileInfo;
	ReturnCode = openFileFromCard(CARD_SLOTA, settingsFileName, &FileInfo);
	
	switch (ReturnCode)
	{
		case CARD_ERROR_READY:
		{
			// Set up the array to hold the area of the file that contains the size
			char *FileData = new char[0x200];
			
			// Get the data from the area that holds the size
			ReturnCode = readFromFileOnCard(&FileInfo, FileData, 0x200, 0x2000, nullptr);
			if (ReturnCode != CARD_ERROR_READY)
			{
				delete[] (FileData);
				closeFileAndUnmountCard(&FileInfo, CARD_SLOTA);
				return ReturnCode;
			}
			
			// Get the size of the file
			uint32_t StoredFileSize = *reinterpret_cast<uint32_t *>(&FileData[0x40]);
			
			// Delete the data that holds the size, as it's not needed anymore
			delete[] (FileData);
			
			// Adjust the file size to be in multiples of 0x2000, rounding up
			uint32_t StoredFileSizeAdjusted = (StoredFileSize + 0x2000 - 1) & ~(0x2000 - 1);
			
			// Make sure the size being written does not exceed the current size
			if (FileSizeAdjusted > StoredFileSizeAdjusted)
			{
				// The new size exceeds the current size, so a new file must be created
				// Close the file
				ReturnCode = closeFileFromCard(&FileInfo);
				if (ReturnCode != CARD_ERROR_READY)
				{
					unmountCard(CARD_SLOTA);
					return ReturnCode;
				}
				
				// Delete the current file
				ReturnCode = deleteFileOnCard(CARD_SLOTA, settingsFileName);
				if (ReturnCode != CARD_ERROR_READY)
				{
					unmountCard(CARD_SLOTA);
					return ReturnCode;
				}
				
				// Make the new file
				// createSettingsFile keeps the file open, but closes and unmounts if it fails to create the file
				ReturnCode = createSettingsFile(settingsFileName, relFileName, &FileInfo);
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
			ReturnCode = createSettingsFile(settingsFileName, relFileName, &FileInfo);
			if (ReturnCode != CARD_ERROR_READY)
			{
				return ReturnCode;
			}
			break;
		}
		default:
		{
			unmountCard(CARD_SLOTA);
			return ReturnCode;
		}
	}
	
	// Set up the memory to be written to the file
	char *MiscData = new char[FileSizeAdjusted];
	clearMemory(MiscData, FileSizeAdjusted);
	
	// Copy the name, description, and file size into the memory
	copyString(&MiscData[0], "Paper Mario");
	copyString(&MiscData[0x20], settingsDescription);
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
	
	copyMemory(Settings->MemoryWatchSettings, MemoryWatch, sizeof(MemoryWatch));
	
	// Write the data to the file
	ReturnCode = writeToFileOnCard(&FileInfo, MiscData, FileSizeAdjusted, 0x2000, nullptr);
	
	delete[] (MiscData);
	closeFileAndUnmountCard(&FileInfo, CARD_SLOTA);
	return ReturnCode;
}

int32_t createSettingsFile(char *settingsFileName, 
	char *relFileName, gc::card::CARDFileInfo *settingsFileInfo)
{
	// Get the banner and icon data from the desired REL file
	gc::card::CARDFileInfo FileInfo;
	int32_t ReturnCode = openFileFromCard(CARD_SLOTA, relFileName, &FileInfo);
	
	if (ReturnCode != CARD_ERROR_READY)
	{
		unmountCard(CARD_SLOTA);
		return ReturnCode;
	}
	
	// Set up the memory for the banner and icon data to go in
	char *BannerIconData = new char[0x2000];
	
	// Get the banner and icon data
	ReturnCode = readFromFileOnCard(&FileInfo, BannerIconData, 0x2000, 0, nullptr);
	if (ReturnCode != CARD_ERROR_READY)
	{
		delete[] (BannerIconData);
		closeFileAndUnmountCard(&FileInfo, CARD_SLOTA);
		return ReturnCode;
	}
	
	// Close the current REL file
	ReturnCode = closeFileFromCard(&FileInfo);
	if (ReturnCode != CARD_ERROR_READY)
	{
		delete[] (BannerIconData);
		unmountCard(CARD_SLOTA);
		return ReturnCode;
	}
	
	// Get the size of the new file
	uint32_t SettingsStructSize = sizeof(struct SettingsStruct);
	uint32_t FileSize = 0x2000 + SettingsStructSize + 0x200;
	
	// Adjust the file size to be in multiples of 0x2000, rounding up
	uint32_t FileSizeAdjusted = (FileSize + 0x2000 - 1) & ~(0x2000 - 1);
	
	// Create the new file
	ReturnCode = createFileOnCard(CARD_SLOTA, settingsFileName, FileSizeAdjusted, &FileInfo, nullptr);
	if (ReturnCode != CARD_ERROR_READY)
	{
		delete[] (BannerIconData);
		unmountCard(CARD_SLOTA);
		return ReturnCode;
	}
	
	// Get the stats for the new file
	gc::card::CARDStat CardStat;
	int32_t FileNumber = FileInfo.fileNum;
	
	ReturnCode = getFileStatus(CARD_SLOTA, FileNumber, &CardStat);
	if (ReturnCode != CARD_ERROR_READY)
	{
		delete[] (BannerIconData);
		unmountCard(CARD_SLOTA);
		return ReturnCode;
	}
	
	// Modify the stats for the new file
	CardStat.bannerFormat 		= 2;
	CardStat.iconAddress 		= 0;
	CardStat.iconFormat 		= 2;
	CardStat.iconSpeed 			= 3;
	CardStat.commentAddress 	= 0x2000;
	
	// Set the stats for the new file
	ReturnCode = setFileStatus(CARD_SLOTA, FileNumber, &CardStat, nullptr);
	if (ReturnCode != CARD_ERROR_READY)
	{
		delete[] (BannerIconData);
		unmountCard(CARD_SLOTA);
		return ReturnCode;
	}
	
	// Open the new file
	ReturnCode = openFileFromCard(CARD_SLOTA, settingsFileName, settingsFileInfo);
	if (ReturnCode != CARD_ERROR_READY)
	{
		delete[] (BannerIconData);
		unmountCard(CARD_SLOTA);
		return ReturnCode;
	}
	
	// Write the banner and icon data to the new file
	ReturnCode = writeToFileOnCard(settingsFileInfo, BannerIconData, 0x2000, 0, nullptr);
	
	// Delete the data for the icon and banner, as they are not needed anymore
	delete[] (BannerIconData);
	
	if (ReturnCode != CARD_ERROR_READY)
	{
		closeFileAndUnmountCard(settingsFileInfo, CARD_SLOTA);
	}
	
	return ReturnCode;
}

}