#include "memcard.h"
#include "global.h"
#include "commonfunctions.h"

#include <gc/card.h>
#include <ttyd/string.h>
#include <ttyd/__mem.h>

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

int32_t createFile(int32_t chn, const char *fileName, uint32_t size, 
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

int32_t openFile(const char *fileName, gc::card::card_file *fileInfo, uint8_t *workArea)
{
	// Make sure a memory card is present in slot A
	int32_t ReturnCode = gc::card::CARDProbeEx(CARD_SLOTA, nullptr, nullptr);
	if (ReturnCode == CARD_ERROR_READY)
	{
		// Mount the memory card in slot A
		ReturnCode = mountCard(CARD_SLOTA, workArea, nullptr, nullptr);
		if (ReturnCode == CARD_ERROR_READY)
		{
			// Open the desired file
			ReturnCode = gc::card::CARDOpen(CARD_SLOTA, fileName, fileInfo);
			if (ReturnCode != CARD_ERROR_READY)
			{
				// Failed to open the file, so close the card
				gc::card::CARDClose(fileInfo);
			}
		}
	}
	return ReturnCode;
}

bool loadSettings(gc::card::card_file *fileInfo)
{
	int32_t ReturnCode;
	
	// Get the size of the file
	uint32_t SettingsStructSize = sizeof(struct SettingsStruct);
	uint32_t FileSize = 0x2000 + SettingsStructSize + 0x200;
	
	// Adjust the file size to be in multiples of 0x2000, rounding up
	uint32_t FileSizeAdjusted = (FileSize + 0x2000 - 1) & ~(0x2000 - 1);
	
	// Set up the memory to be copied from the file
	uint32_t MiscDataSize = FileSizeAdjusted - 0x2000; // Remove the extra 0x2000 from the banner and icon
	char *MiscData = new char[MiscDataSize];
	clearMemory(MiscData, MiscDataSize);
	
	// Get the data from the file
	ReturnCode = readFromCard(fileInfo, MiscData, MiscDataSize, 0x2000, nullptr);
	
	// Close and unmount the card, as it's no longer needed
	gc::card::CARDClose(fileInfo);
	gc::card::CARDUnmount(CARD_SLOTA);
	
	if (ReturnCode != CARD_ERROR_READY)
	{
		delete[] (MiscData);
		return false;
	}
	
	// Get the settings struct from the file
	SettingsStruct *Settings = reinterpret_cast<SettingsStruct *>(&MiscData[0x200]);
	
	// Get the desired variables from the struct
	uint32_t CheatsSize = sizeof(Cheat) / sizeof(Cheat[0]);
	for (uint32_t i = 0; i < CheatsSize; i++)
	{
		Cheat[CheatsOrder[i]].Active 		= Settings->CheatsActive[i];
		Cheat[CheatsOrder[i]].ButtonCombo 	= Settings->CheatButtonCombos[i];
	}
	
	uint32_t DisplaysSize = sizeof(Displays);
	for (uint32_t i = 0; i < DisplaysSize; i++)
	{
		Displays[i] = Settings->DisplaysActive[i];
	}
	
	OnScreenTimer.ButtonCombo[0] = Settings->DisplaysButtonCombos[ONSCREEN_TIMER];
	OnScreenTimer.ButtonCombo[1] = Settings->DisplaysButtonCombos[ONSCREEN_TIMER + 1];
	
	delete[] (MiscData);
	return true;
	
}

bool writeSettings(const char *description, gc::card::card_file *fileInfo)
{
	int32_t ReturnCode;
	
	// Get the size of the file
	uint32_t SettingsStructSize = sizeof(struct SettingsStruct);
	uint32_t FileSize = 0x2000 + SettingsStructSize + 0x200;
	
	// Adjust the file size to be in multiples of 0x2000, rounding up
	uint32_t FileSizeAdjusted = (FileSize + 0x2000 - 1) & ~(0x2000 - 1);
	
	// Set up the memory to be written to the file
	uint32_t MiscDataSize = FileSizeAdjusted - 0x2000; // Remove the extra 0x2000 from the banner and icon
	char *MiscData = new char[MiscDataSize];
	clearMemory(MiscData, MiscDataSize);
	
	// Copy the name and description to display into the memory
	ttyd::string::strcpy(MiscData, "Paper Mario");
	ttyd::string::strcpy(&MiscData[0x20], description);
	
	// Set up the struct to hold the variables to store
	SettingsStruct *Settings = new SettingsStruct;
	clearMemory(Settings, SettingsStructSize);
	
	// Copy the desired variables to the struct
	uint32_t CheatsSize = sizeof(Cheat) / sizeof(Cheat[0]);
	for (uint32_t i = 0; i < CheatsSize; i++)
	{
		Settings->CheatsActive[i] 		= Cheat[CheatsOrder[i]].Active;
		Settings->CheatButtonCombos[i] 	= Cheat[CheatsOrder[i]].ButtonCombo;
	}
	
	uint32_t DisplaysSize = sizeof(Displays);
	for (uint32_t i = 0; i < DisplaysSize; i++)
	{
		Settings->DisplaysActive[i] = Displays[i];
	}
	
	Settings->DisplaysButtonCombos[ONSCREEN_TIMER] 		= OnScreenTimer.ButtonCombo[0];
	Settings->DisplaysButtonCombos[ONSCREEN_TIMER + 1] 	= OnScreenTimer.ButtonCombo[1];
	
	// Copy the struct contents into the memory
	ttyd::__mem::memcpy(&MiscData[0x200], Settings, SettingsStructSize);
	
	// Delete the struct holding the variables, as they're no longer needed
	delete[] (Settings);
	
	// Write the data to the file
	ReturnCode = writeToCard(fileInfo, MiscData, MiscDataSize, 0x2000, nullptr);
	
	delete[] (MiscData);
	gc::card::CARDClose(fileInfo);
	gc::card::CARDUnmount(CARD_SLOTA);
	
	if (ReturnCode == CARD_ERROR_READY)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool createSettingsFile(const char *fileName, const char *description, gc::card::card_file *fileInfo)
{
	int32_t ReturnCode;
	
	// Get the banner and icon data from the current REL file
	ReturnCode = gc::card::CARDOpen(CARD_SLOTA, "rel", fileInfo);
	if (ReturnCode != CARD_ERROR_READY)
	{
		gc::card::CARDUnmount(CARD_SLOTA);
		return false;
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
		return false;
	}
	
	// Close the current REL file
	ReturnCode = gc::card::CARDClose(fileInfo);
	if (ReturnCode != CARD_ERROR_READY)
	{
		delete[] (BannerIconData);
		gc::card::CARDUnmount(CARD_SLOTA);
		return false;
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
		return false;
	}
	
	// Get the stats for the new file
	gc::card::card_stat tempCardStats;
	int32_t FileNumber = fileInfo->fileNum;
	
	ReturnCode = gc::card::CARDGetStatus(CARD_SLOTA, FileNumber, &tempCardStats);
	if (ReturnCode != CARD_ERROR_READY)
	{
		delete[] (BannerIconData);
		gc::card::CARDUnmount(CARD_SLOTA);
		return false;
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
		return false;
	}
	
	// Open the new file
	ReturnCode = gc::card::CARDOpen(CARD_SLOTA, fileName, fileInfo);
	if (ReturnCode != CARD_ERROR_READY)
	{
		delete[] (BannerIconData);
		gc::card::CARDUnmount(CARD_SLOTA);
		return false;
	}
	
	// Write the banner and icon data to the new file
	ReturnCode = writeToCard(fileInfo, BannerIconData, 0x2000, 0, nullptr);
	
	// Delete the data for the icon and banner, as they are not needed anymore
	delete[] (BannerIconData);
	
	if (ReturnCode != CARD_ERROR_READY)
	{
		gc::card::CARDClose(fileInfo);
		gc::card::CARDUnmount(CARD_SLOTA);
		return false;
	}
	
	// Write the rest of the data to the new file
	return writeSettings(description, fileInfo);
}

}