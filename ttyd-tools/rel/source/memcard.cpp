#include "memcard.h"
#include "global.h"
#include "commonfunctions.h"

#include <gc/card.h>
#include <ttyd/cardmgr.h>

#include <cstring>

namespace mod {

int32_t checkForMemoryCard(int32_t memoryCardSlot)
{
    int32_t ReturnCode;
    for (uint32_t i = 0; i < 1000000; i++)
    {
        ReturnCode = gc::card::CARDProbeEx(memoryCardSlot, nullptr, nullptr);
        if (ReturnCode != CARD_RESULT_BUSY)
        {
            break;
        }
    }
    return ReturnCode;
}

int32_t finishAsyncFunction(int32_t memoryCardSlot, int32_t returnCode)
{
    if (returnCode == CARD_RESULT_READY)
    {
        do
        {
            returnCode = gc::card::CARDGetResultCode(memoryCardSlot);
        }
        while (returnCode == CARD_RESULT_BUSY);
    }
    return returnCode;
}

int32_t mountCard(int32_t memoryCardSlot)
{
    void *WorkArea = ttyd::cardmgr::cardMgrWorkPointer->workArea;
    int32_t ReturnCode = gc::card::CARDMountAsync(memoryCardSlot, WorkArea, nullptr, nullptr);
    return finishAsyncFunction(memoryCardSlot, ReturnCode);
}

int32_t createFileOnCard(int32_t memoryCardSlot, const char *fileName, uint32_t size, gc::card::CARDFileInfo *fileInfo)
{
    int32_t ReturnCode = gc::card::CARDCreateAsync(memoryCardSlot, fileName, size, fileInfo, nullptr);
    return finishAsyncFunction(memoryCardSlot, ReturnCode);
}

int32_t readFromFileOnCard(int32_t memoryCardSlot, gc::card::CARDFileInfo *fileInfo, void *buffer, int32_t length, int32_t offset)
{
    int32_t ReturnCode = gc::card::CARDReadAsync(fileInfo, buffer, length, offset, nullptr);
    return finishAsyncFunction(memoryCardSlot, ReturnCode);
}

int32_t writeToFileOnCard(int32_t memoryCardSlot, gc::card::CARDFileInfo *fileInfo, void *buffer, int32_t length, int32_t offset)
{
    int32_t ReturnCode = gc::card::CARDWriteAsync(fileInfo, buffer, length, offset, nullptr);
    return finishAsyncFunction(memoryCardSlot, ReturnCode);
}

int32_t setFileStatus(int32_t memoryCardSlot, int32_t fileNum, gc::card::CARDStat *stat)
{
    int32_t ReturnCode = gc::card::CARDSetStatusAsync(memoryCardSlot, fileNum, stat, nullptr);
    return finishAsyncFunction(memoryCardSlot, ReturnCode);
}

int32_t renameFileOnCard(int32_t memoryCardSlot, const char *oldName, const char *newName)
{
    int32_t ReturnCode = gc::card::card_rename_async(memoryCardSlot, oldName, newName, nullptr);
    return finishAsyncFunction(memoryCardSlot, ReturnCode);
}

int32_t createSettingsFile(int32_t memoryCardSlot, gc::card::CARDFileInfo *settingsFileInfo)
{
    // If creating the file on slot B, unmount the current slot and mount slot A
    int32_t ReturnCode;
    if (memoryCardSlot != CARD_SLOT_A)
    {
        ReturnCode = gc::card::CARDUnmount(memoryCardSlot);
        if (ReturnCode != CARD_RESULT_READY)
        {
            return ReturnCode;
        }
        
        ReturnCode = mountCard(CARD_SLOT_A);
        if (ReturnCode != CARD_RESULT_READY)
        {
            return ReturnCode;
        }
    }
    
    // Get the banner and icon data from the current REL file
    gc::card::CARDFileInfo FileInfo;
    ReturnCode = gc::card::CARDOpen(CARD_SLOT_A, MenuSettings.RelFileName, &FileInfo);
    
    if (ReturnCode != CARD_RESULT_READY)
    {
        gc::card::CARDUnmount(CARD_SLOT_A);
        return ReturnCode;
    }
    
    // Set up the memory for the banner and icon data to go in
    char *BannerIconData = new char[0x2000];
    
    // Get the banner and icon data
    ReturnCode = readFromFileOnCard(CARD_SLOT_A, &FileInfo, BannerIconData, 0x2000, 0);
    if (ReturnCode != CARD_RESULT_READY)
    {
        delete[] (BannerIconData);
        gc::card::CARDClose(&FileInfo);
        gc::card::CARDUnmount(CARD_SLOT_A);
        return ReturnCode;
    }
    
    // Close the current REL file
    ReturnCode = gc::card::CARDClose(&FileInfo);
    if (ReturnCode != CARD_RESULT_READY)
    {
        delete[] (BannerIconData);
        gc::card::CARDUnmount(CARD_SLOT_A);
        return ReturnCode;
    }
    
    // If creating the file on slot B, unmount the current slot and mount slot B
    if (memoryCardSlot != CARD_SLOT_A)
    {
        ReturnCode = gc::card::CARDUnmount(CARD_SLOT_A);
        if (ReturnCode != CARD_RESULT_READY)
        {
            return ReturnCode;
        }
        
        ReturnCode = mountCard(memoryCardSlot);
        if (ReturnCode != CARD_RESULT_READY)
        {
            return ReturnCode;
        }
    }
    
    // Get the size of the new file
    uint32_t FileSize = CARD_WRITE_SIZE + sizeof(struct SettingsStruct) + CARD_READ_SIZE;
    
    // Add on the total size of all of the custom states
    uint32_t tempTotalCustomStates = CustomState.TotalEntries;
    if (tempTotalCustomStates > 0)
    {
        if (tempTotalCustomStates > CUSTOM_STATES_MAX_COUNT)
        {
            tempTotalCustomStates = CUSTOM_STATES_MAX_COUNT;
        }
        
        FileSize += sizeof(CustomStateStruct) * tempTotalCustomStates;
    }
    
    // Adjust the file size to be in multiples of CARD_WRITE_SIZE, rounding up
    uint32_t FileSizeAdjusted = (FileSize + CARD_WRITE_SIZE - 1) & ~(CARD_WRITE_SIZE - 1);
    
    // Create the new file
    // The new file should be opened automatically via CARDCreateAsync
    const char *SettingsFileName = MenuSettings.SettingsFileName;
    ReturnCode = createFileOnCard(memoryCardSlot, SettingsFileName, FileSizeAdjusted, settingsFileInfo);
    
    if (ReturnCode != CARD_RESULT_READY)
    {
        delete[] (BannerIconData);
        gc::card::CARDUnmount(memoryCardSlot);
        return ReturnCode;
    }
    
    // Get the stats for the new file
    int32_t FileNumber = settingsFileInfo->fileNum;
    gc::card::CARDStat CardStat;
    
    ReturnCode = gc::card::CARDGetStatus(memoryCardSlot, FileNumber, &CardStat);
    if (ReturnCode != CARD_RESULT_READY)
    {
        delete[] (BannerIconData);
        gc::card::CARDClose(settingsFileInfo);
        gc::card::CARDUnmount(memoryCardSlot);
        return ReturnCode;
    }
    
    // Modify the stats for the new file
    CardStat.bannerFormat   = 2;
    CardStat.iconAddress    = 0;
    CardStat.iconFormat     = 2;
    CardStat.iconSpeed      = 3;
    CardStat.commentAddress = 0x2000;
    
    // Set the stats for the new file
    ReturnCode = setFileStatus(memoryCardSlot, FileNumber, &CardStat);
    if (ReturnCode != CARD_RESULT_READY)
    {
        delete[] (BannerIconData);
        gc::card::CARDClose(settingsFileInfo);
        gc::card::CARDUnmount(memoryCardSlot);
        return ReturnCode;
    }
    
    // Write the banner and icon data to the new file
    ReturnCode = writeToFileOnCard(memoryCardSlot, settingsFileInfo, BannerIconData, 0x2000, 0);
    
    // Delete the data for the icon and banner, as they are not needed anymore
    delete[] (BannerIconData);
    
    if (ReturnCode != CARD_RESULT_READY)
    {
        gc::card::CARDClose(settingsFileInfo);
        gc::card::CARDUnmount(memoryCardSlot);
    }
    
    return ReturnCode;
}

int32_t saveSettings(int32_t memoryCardSlot)
{
    // Make sure the vanilla code isn't currently doing memory card stuff
    if (ttyd::cardmgr::cardIsExec())
    {
        return MEMCARD_IN_USE;
    }
    
    // Make sure a memory card is inserted into the selected memory card slot
    int32_t ReturnCode = checkForMemoryCard(memoryCardSlot);
    if (ReturnCode != CARD_RESULT_READY)
    {
        return ReturnCode;
    }
    
    // Mount the memory card
    ReturnCode = mountCard(memoryCardSlot);
    if (ReturnCode != CARD_RESULT_READY)
    {
        return ReturnCode;
    }
    
    // Get the size thats going to be written
    uint32_t FileSize = sizeof(struct SettingsStruct) + CARD_READ_SIZE;
    
    // Add on the total size of all of the custom states
    uint32_t tempTotalCustomStates = CustomState.TotalEntries;
    if (tempTotalCustomStates > 0)
    {
        if (tempTotalCustomStates > CUSTOM_STATES_MAX_COUNT)
        {
            tempTotalCustomStates = CUSTOM_STATES_MAX_COUNT;
        }
        
        FileSize += sizeof(CustomStateStruct) * tempTotalCustomStates;
    }
    
    // Adjust the file size to be in multiples of CARD_WRITE_SIZE, rounding up
    uint32_t FileSizeAdjusted = (FileSize + CARD_WRITE_SIZE - 1) & ~(CARD_WRITE_SIZE - 1);
    
    // Open the settings file if it exists
    gc::card::CARDFileInfo FileInfo;
    const char *SettingsFileName = MenuSettings.SettingsFileName;
    
    ReturnCode = gc::card::CARDOpen(memoryCardSlot, SettingsFileName, &FileInfo);
    switch (ReturnCode)
    {
        case CARD_RESULT_READY:
        {
            // Set up the array to hold the area of the file that contains the size
            char *FileData = new char[CARD_READ_SIZE];
            
            // Get the data from the area that holds the size
            ReturnCode = readFromFileOnCard(memoryCardSlot, &FileInfo, FileData, CARD_READ_SIZE, 0x2000);
            if (ReturnCode != CARD_RESULT_READY)
            {
                delete[] (FileData);
                gc::card::CARDClose(&FileInfo);
                gc::card::CARDUnmount(memoryCardSlot);
                return ReturnCode;
            }
            
            // Get the size of the file
            uint32_t StoredFileSize = *reinterpret_cast<uint32_t *>(&FileData[0x40]);
            
            // Delete the data that holds the size, as it's not needed anymore
            delete[] (FileData);
            
            // Adjust the file size to be in multiples of CARD_WRITE_SIZE, rounding up
            uint32_t StoredFileSizeAdjusted = (StoredFileSize + CARD_WRITE_SIZE - 1) & ~(CARD_WRITE_SIZE - 1);
            
            // Make sure the size being written matches the current size
            if (FileSizeAdjusted != StoredFileSizeAdjusted)
            {
                // The new size differs from the current size, so a new file must be created
                // Close the file
                ReturnCode = gc::card::CARDClose(&FileInfo);
                if (ReturnCode != CARD_RESULT_READY)
                {
                    gc::card::CARDUnmount(memoryCardSlot);
                    return ReturnCode;
                }
                
                // Delete the current file
                ReturnCode = gc::card::CARDDelete(memoryCardSlot, SettingsFileName);
                if (ReturnCode != CARD_RESULT_READY)
                {
                    gc::card::CARDUnmount(memoryCardSlot);
                    return ReturnCode;
                }
                
                // Make the new file
                // createSettingsFile keeps the file open, but closes and unmounts if it fails to create the file
                ReturnCode = createSettingsFile(memoryCardSlot, &FileInfo);
                if (ReturnCode != CARD_RESULT_READY)
                {
                    return ReturnCode;
                }
            }
            break;
        }
        case CARD_RESULT_NOFILE:
        {
            // Settings file does not exist, so create it
            // createSettingsFile keeps the file open, but closes and unmounts if it fails to create the file
            ReturnCode = createSettingsFile(memoryCardSlot, &FileInfo);
            if (ReturnCode != CARD_RESULT_READY)
            {
                return ReturnCode;
            }
            break;
        }
        default:
        {
            gc::card::CARDUnmount(memoryCardSlot);
            return ReturnCode;
        }
    }
    
    // Set up the memory to be written to the file
    char *MiscData = new char[FileSizeAdjusted];
    clearMemory(MiscData, FileSizeAdjusted);
    
    // Set up the struct to hold the name, description, and file size to store
    SaveFileDecriptionInfo *SaveFileInfo = reinterpret_cast<SaveFileDecriptionInfo *>(&MiscData[0]);
    
    // Copy the name, description, and file size
    strcpy(SaveFileInfo->Description1, "Paper Mario");
    strcpy(SaveFileInfo->Description2, MenuSettings.SettingsDescription);
    SaveFileInfo->FileSize = FileSize;
    
    // Set up the struct to hold the variables to store
    SettingsStruct *Settings = reinterpret_cast<SettingsStruct *>(&MiscData[0x200]);
    
    // Copy the desired variables to the struct
    // Copy the Cheats bools and button combos
    uint32_t CheatsSize = sizeof(Cheat) / sizeof(Cheat[0]);
    for (uint32_t i = 0; i < CheatsSize; i++)
    {
        uint32_t CurrentIndex          = CheatsOrder[i];
        Settings->CheatsActive[i]      = Cheat[CurrentIndex].Active;
        Settings->CheatButtonCombos[i] = Cheat[CurrentIndex].ButtonCombo;
    }
    
    // Copy the Displays bools
    uint32_t DisplaysSize = sizeof(Displays);
    for (uint32_t i = 0; i < DisplaysSize; i++)
    {
        uint32_t CurrentIndex       = DisplaysOrder[i];
        Settings->DisplaysActive[i] = Displays[CurrentIndex];
    }
    
    // Copy the On-Screen Timer button combos
    uint32_t OnScreenTimerSize = sizeof(OnScreenTimer.ButtonCombo) / sizeof(OnScreenTimer.ButtonCombo[0]);
    for (uint32_t i = 0; i < OnScreenTimerSize; i++)
    {
        Settings->DisplaysButtonCombos[ONSCREEN_TIMER + i] = OnScreenTimer.ButtonCombo[i];
    }
    
    // Copy the Frame Counter button combos
    uint32_t FrameCounterSize = sizeof(FrameCounter.ButtonCombo) / sizeof(FrameCounter.ButtonCombo[0]);
    for (uint32_t i = 0; i < FrameCounterSize; i++)
    {
        Settings->DisplaysButtonCombos[OnScreenTimerSize + ONSCREEN_TIMER + i] = FrameCounter.ButtonCombo[i];
    }
    
    // Copy the Memory Watches
    memcpy(Settings->MemoryWatchSettings, MemoryWatch, sizeof(MemoryWatch));
    
    // Copy the Memory Editor data
    Settings->MemoryEditorSave.CurrentAddress           = MemoryEditor.CurrentAddress;
    Settings->MemoryEditorSave.ButtonCombo              = MemoryEditor.ButtonCombo;
    Settings->MemoryEditorSave.CurrentSelectionStatus   = MemoryEditor.CurrentSelectionStatus;
    Settings->MemoryEditorSave.NumBytesBeingEdited      = MemoryEditor.NumBytesBeingEdited;
    Settings->MemoryEditorSave.EditorCurrentlyDisplayed = MemoryEditor.EditorCurrentlyDisplayed;
    
    // Copy the frame advance button combos
    Settings->FrameAdvanceButtonCombos.AdvanceFrameButtonCombo = FrameAdvance.FrameAdvanceButtonCombos.AdvanceFrameButtonCombo;
    Settings->FrameAdvanceButtonCombos.PauseButtonCombo = FrameAdvance.FrameAdvanceButtonCombos.PauseButtonCombo;
    
    // Copy the custom states to the very end of the struct
    Settings->CustomStateSettings.CustomStateCount = tempTotalCustomStates;
    if (tempTotalCustomStates > 0)
    {
        uint32_t OffsetToCustomStates = 
            (reinterpret_cast<uint32_t>(&Settings->CustomStateSettings) - 
            reinterpret_cast<uint32_t>(Settings)) + 
            sizeof(Settings->CustomStateSettings);
        
        Settings->CustomStateSettings.OffsetToCustomStates = OffsetToCustomStates;
        
        CustomStateStruct *CustomStatesSettings = reinterpret_cast<CustomStateStruct *>(
            reinterpret_cast<uint32_t>(Settings) + OffsetToCustomStates);
        
        memcpy(CustomStatesSettings, CustomState.State, sizeof(CustomStateStruct) * tempTotalCustomStates);
    }
    
    // Copy the hit check visualization data
    Settings->HitCheckSettings.DrawHits = HitCheck.Settings.DrawHits;
    Settings->HitCheckSettings.DrawMisses = HitCheck.Settings.DrawMisses;
    
    // Only copy the colors if their alphas are not 0
    uint32_t HitsColor = HitCheck.Settings.HitsColor;
    if ((HitsColor & 0xFF) != 0)
    {
        Settings->HitCheckSettings.HitsColor = HitsColor;
    }
    
    uint32_t MissesColor = HitCheck.Settings.MissesColor;
    if ((MissesColor & 0xFF) != 0)
    {
        Settings->HitCheckSettings.MissesColor = MissesColor;
    }
    
    // Write the data to the file
    ReturnCode = writeToFileOnCard(memoryCardSlot, &FileInfo, MiscData, FileSizeAdjusted, 0x2000);
    
    delete[] (MiscData);
    gc::card::CARDClose(&FileInfo);
    gc::card::CARDUnmount(memoryCardSlot);
    return ReturnCode;
}

int32_t loadSettings(int32_t memoryCardSlot)
{
    // Make sure the vanilla code isn't currently doing memory card stuff
    if (ttyd::cardmgr::cardIsExec())
    {
        return MEMCARD_IN_USE;
    }
    
    // Make sure a memory card is inserted into the selected memory card slot
    int32_t ReturnCode = checkForMemoryCard(memoryCardSlot);
    if (ReturnCode != CARD_RESULT_READY)
    {
        return ReturnCode;
    }
    
    // Mount the memory card
    ReturnCode = mountCard(memoryCardSlot);
    if (ReturnCode != CARD_RESULT_READY)
    {
        return ReturnCode;
    }
    
    // Open the settings file
    gc::card::CARDFileInfo FileInfo;
    ReturnCode = gc::card::CARDOpen(memoryCardSlot, MenuSettings.SettingsFileName, &FileInfo);
    
    if (ReturnCode != CARD_RESULT_READY)
    {
        gc::card::CARDUnmount(memoryCardSlot);
        return ReturnCode;
    }
    
    // Set up the array to hold the area of the file that contains the size
    char *FileData = new char[CARD_READ_SIZE];
    
    // Get the data from the area that holds the size
    ReturnCode = readFromFileOnCard(memoryCardSlot, &FileInfo, FileData, CARD_READ_SIZE, 0x2000);
    if (ReturnCode != CARD_RESULT_READY)
    {
        delete[] (FileData);
        gc::card::CARDClose(&FileInfo);
        gc::card::CARDUnmount(memoryCardSlot);
        return ReturnCode;
    }
    
    // Get the size of the file
    uint32_t StoredFileSize = *reinterpret_cast<uint32_t *>(&FileData[0x40]);
    
    // Delete the data that holds the size, as it's not needed anymore
    delete[] (FileData);
    
    // Adjust the file size to be in multiples of CARD_READ_SIZE, rounding up
    uint32_t StoredFileSizeAdjusted = (StoredFileSize + CARD_READ_SIZE - 1) & ~(CARD_READ_SIZE - 1);
    
    // Make sure the stored file size is at least CARD_READ_SIZE
    if (StoredFileSizeAdjusted < CARD_READ_SIZE)
    {
        StoredFileSizeAdjusted = CARD_READ_SIZE;
    }
    
    // Get the size needed to be read
    uint32_t FileSize = sizeof(struct SettingsStruct) + CARD_READ_SIZE;
    
    // Adjust the struct size to be in multiples of CARD_READ_SIZE, rounding up
    uint32_t FileSizeAdjusted = (FileSize + CARD_READ_SIZE - 1) & ~(CARD_READ_SIZE - 1);
    
    // The stored size may exceed the struct size
    if (FileSizeAdjusted < StoredFileSizeAdjusted)
    {
        FileSizeAdjusted = StoredFileSizeAdjusted;
    }
    
    // Set up the memory to be copied from the file
    char *MiscData = new char[FileSizeAdjusted];
    
    // Get the data from the file
    // Must read by the stored size, as the struct size may exceed the size of the file
    ReturnCode = readFromFileOnCard(memoryCardSlot, &FileInfo, MiscData, StoredFileSizeAdjusted - CARD_READ_SIZE, 0x2200);
    
    // Close and unmount the card, as it's no longer needed
    gc::card::CARDClose(&FileInfo);
    gc::card::CARDUnmount(memoryCardSlot);
    
    if (ReturnCode != CARD_RESULT_READY)
    {
        delete[] (MiscData);
        return ReturnCode;
    }
    
    // Get the settings struct from the file
    SettingsStruct *Settings = reinterpret_cast<SettingsStruct *>(&MiscData[0]);
    
    // Get the desired variables from the struct
    // Get the Cheats bools and button combos
    uint32_t CheatsSize = sizeof(Cheat) / sizeof(Cheat[0]);
    for (uint32_t i = 0; i < CheatsSize; i++)
    {
        uint32_t CurrentIndex      = CheatsOrder[i];
        Cheat[CurrentIndex].Active = Settings->CheatsActive[i];
        
        // Make sure the button combo is valid, so that new button combos are not overwritten with 0
        uint16_t ButtonCombo = Settings->CheatButtonCombos[i];
        if (ButtonCombo)
        {
            Cheat[CurrentIndex].ButtonCombo = ButtonCombo;
        }
    }
    
    // Get the Displays bools
    uint32_t DisplaysSize = sizeof(Displays);
    for (uint32_t i = 0; i < DisplaysSize; i++)
    {
        uint32_t CurrentIndex  = DisplaysOrder[i];
        Displays[CurrentIndex] = Settings->DisplaysActive[i];
    }
    
    // Get the On-Screen Timer button combos
    uint32_t OnScreenTimerSize = sizeof(OnScreenTimer.ButtonCombo) / sizeof(OnScreenTimer.ButtonCombo[0]);
    for (uint32_t i = 0; i < OnScreenTimerSize; i++)
    {
        uint16_t ButtonCombo = Settings->DisplaysButtonCombos[ONSCREEN_TIMER + i];
        
        // Make sure the button combo is valid, so that new button combos are not overwritten with 0
        if (ButtonCombo)
        {
            OnScreenTimer.ButtonCombo[i] = ButtonCombo;
        }
    }
    
    // Get the Frame Counter button combos
    uint32_t FrameCounterSize = sizeof(FrameCounter.ButtonCombo) / sizeof(FrameCounter.ButtonCombo[0]);
    for (uint32_t i = 0; i < FrameCounterSize; i++)
    {
        uint16_t ButtonCombo = Settings->DisplaysButtonCombos[OnScreenTimerSize + ONSCREEN_TIMER + i];
        
        // Make sure the button combo is valid, so that new button combos are not overwritten with 0
        if (ButtonCombo)
        {
            FrameCounter.ButtonCombo[i] = ButtonCombo;
        }
    }
    
    // Get the Memory Watches
    memcpy(MemoryWatch, Settings->MemoryWatchSettings, sizeof(MemoryWatch));
    
    // Get the Memory Editor data
    MemoryEditor.CurrentSelectionStatus = Settings->MemoryEditorSave.CurrentSelectionStatus;
    MemoryEditor.EditorCurrentlyDisplayed = Settings->MemoryEditorSave.EditorCurrentlyDisplayed;
    
    // Don't get CurrentAddress, ButtonCombo, or NumBytesBeingEdited if they are 0
    uint8_t *CurrentAddress = Settings->MemoryEditorSave.CurrentAddress;
    if (CurrentAddress)
    {
        MemoryEditor.CurrentAddress = CurrentAddress;
    }
    
    uint16_t ButtonCombo = Settings->MemoryEditorSave.ButtonCombo;
    if (ButtonCombo)
    {
        MemoryEditor.ButtonCombo = ButtonCombo;
    }
    
    uint8_t NumBytesBeingEdited = Settings->MemoryEditorSave.NumBytesBeingEdited;
    if (NumBytesBeingEdited)
    {
        MemoryEditor.NumBytesBeingEdited = NumBytesBeingEdited;
    }
    
    // Get the frame advance button combos
    // Don't get if they are 0
    ButtonCombo = Settings->FrameAdvanceButtonCombos.AdvanceFrameButtonCombo;
    if (ButtonCombo)
    {
        FrameAdvance.FrameAdvanceButtonCombos.AdvanceFrameButtonCombo = ButtonCombo;
    }
    
    ButtonCombo = Settings->FrameAdvanceButtonCombos.PauseButtonCombo;
    if (ButtonCombo)
    {
        FrameAdvance.FrameAdvanceButtonCombos.PauseButtonCombo = ButtonCombo;
    }
    
    // Get the custom states
    uint32_t tempTotalCustomStates = Settings->CustomStateSettings.CustomStateCount;
    if (tempTotalCustomStates > 0)
    {
        if (tempTotalCustomStates > CUSTOM_STATES_MAX_COUNT)
        {
            tempTotalCustomStates = CUSTOM_STATES_MAX_COUNT;
        }
        
        CustomState.TotalEntries = tempTotalCustomStates;
        CustomState.StateWasSelected = false;
        
        CustomStateStruct *tempState = CustomState.State;
        if (tempState)
        {
            delete[] (tempState);
        }
        
        tempState = new CustomStateStruct[tempTotalCustomStates];
        CustomState.State = tempState;
        
        CustomStateStruct *CustomStatesSettings = reinterpret_cast<CustomStateStruct *>(
            reinterpret_cast<uint32_t>(Settings) + Settings->CustomStateSettings.OffsetToCustomStates);
        
        memcpy(tempState, CustomStatesSettings, sizeof(CustomStateStruct) * tempTotalCustomStates);
    }
    
    // Get the hit check visualization data
    HitCheck.Settings.DrawHits = Settings->HitCheckSettings.DrawHits;
    HitCheck.Settings.DrawMisses = Settings->HitCheckSettings.DrawMisses;
    
    // Only get the colors if their alphas are not 0
    uint32_t HitsColor = Settings->HitCheckSettings.HitsColor;
    if ((HitsColor & 0xFF) != 0)
    {
        HitCheck.Settings.HitsColor = HitsColor;
    }
    
    uint32_t MissesColor = Settings->HitCheckSettings.MissesColor;
    if ((MissesColor & 0xFF) != 0)
    {
        HitCheck.Settings.MissesColor = MissesColor;
    }
    
    delete[] (MiscData);
    return CARD_RESULT_READY;
}

int32_t renameSettingsFile(int32_t memoryCardSlot)
{
    // Make sure the vanilla code isn't currently doing memory card stuff
    if (ttyd::cardmgr::cardIsExec())
    {
        return MEMCARD_IN_USE;
    }
    
    // Make sure a memory card is inserted into the selected memory card slot
    int32_t ReturnCode = checkForMemoryCard(memoryCardSlot);
    if (ReturnCode != CARD_RESULT_READY)
    {
        return ReturnCode;
    }
    
    // Mount the memory card
    ReturnCode = mountCard(memoryCardSlot);
    if (ReturnCode != CARD_RESULT_READY)
    {
        return ReturnCode;
    }
    
    // Rename the settings file
    ReturnCode = renameFileOnCard(memoryCardSlot, "rel_settings", MenuSettings.SettingsFileName);
    
    // Unmount the memory card
    gc::card::CARDUnmount(memoryCardSlot);
    return ReturnCode;
}

int32_t deleteSettingsFile(int32_t memoryCardSlot)
{
    // Make sure the vanilla code isn't currently doing memory card stuff
    if (ttyd::cardmgr::cardIsExec())
    {
        return MEMCARD_IN_USE;
    }
    
    // Make sure a memory card is inserted into the selected memory card slot
    int32_t ReturnCode = checkForMemoryCard(memoryCardSlot);
    if (ReturnCode != CARD_RESULT_READY)
    {
        return ReturnCode;
    }
    
    // Mount the memory card
    ReturnCode = mountCard(memoryCardSlot);
    if (ReturnCode != CARD_RESULT_READY)
    {
        return ReturnCode;
    }
    
    // Delete the settings file
    ReturnCode = gc::card::CARDDelete(memoryCardSlot, MenuSettings.SettingsFileName);
    
    // Unmount the memory card
    gc::card::CARDUnmount(memoryCardSlot);
    return ReturnCode;
}

}