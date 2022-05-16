#include <gc/card.h>
#include <gc/OSTime.h>
#include <gc/os.h>

#include <cstring>

namespace gc::card {

int32_t card_rename_async(int32_t channel, const char *oldName, const char *newName, CARDCallback callback)
{
    // Make sure the first char of each string is valid
    char OldNameFirstChar = oldName[0];
    if ((OldNameFirstChar == 0) || (OldNameFirstChar == 0xFF))
    {
        return CARD_RESULT_FATAL_ERROR;
    }
    
    char NewNameFirstChar = newName[0];
    if ((NewNameFirstChar == 0) || (NewNameFirstChar == 0xFF))
    {
        return CARD_RESULT_FATAL_ERROR;
    }
    
    // Make sure the strings are not the same
    if (strncmp(oldName, newName, CARD_FILENAME_MAX) == 0)
    {
        return CARD_RESULT_FATAL_ERROR;
    }
    
    // Make sure each string does not exceed the max length
    if (strlen(oldName) > CARD_FILENAME_MAX)
    {
        return CARD_RESULT_NAMETOOLONG;
    }
    
    if (strlen(newName) > CARD_FILENAME_MAX)
    {
        return CARD_RESULT_NAMETOOLONG;
    }
    
    void *Card;
    int32_t ReturnCode = __CARDGetControlBlock(channel, reinterpret_cast<void **>(&Card));
    if (ReturnCode != CARD_RESULT_READY)
    {
        return ReturnCode;
    }
    
    uint8_t *DefaultGameCodeLocation = *reinterpret_cast<uint8_t **>(reinterpret_cast<uint32_t>(Card) + 0x10C);
    uint32_t DirBlock = reinterpret_cast<uint32_t>(__CARDGetDirBlock(Card));
    int32_t OldNameFileIndex = -1;
    
    // Loop through each entry and check for files that are named oldName and/or newName
    for (int32_t i = 0; i < CARD_MAX_FILE; i++)
    {
        uint8_t *CurrentDirBlock = reinterpret_cast<uint8_t *>(DirBlock + (i * 0x40));
        
        // Make sure the current entry is valid
        if (CurrentDirBlock[0] == 0xFF)
        {
            continue;
        }
        
        // Make sure the current entry's game code matches the default's game code
        if (memcmp(CurrentDirBlock, DefaultGameCodeLocation, 6) != 0)
        {
            continue;
        }
        
        // If a file named oldName is found, keep track of its index
        if (__CARDCompareFileName(CurrentDirBlock, oldName))
        {
            OldNameFileIndex = i;
            continue;
        }
        
        // If a file named newName is found, then the desired name already exists, or the file was already renamed
        if (__CARDCompareFileName(CurrentDirBlock, newName))
        {
            ReturnCode = __CARDPutControlBlock(Card, CARD_RESULT_EXIST);
            return ReturnCode;
        }
    }
    
    // Make sure a file named oldName was found
    if (OldNameFileIndex == -1)
    {
        ReturnCode = __CARDPutControlBlock(Card, CARD_RESULT_NOFILE);
        return ReturnCode;
    }
    
    // Make sure this game has proper access to the file that is being renamed
    uint8_t *CurrentDirBlock = reinterpret_cast<uint8_t *>(DirBlock + (OldNameFileIndex * 0x40));
    
    ReturnCode = __CARDIsWritable(Card, CurrentDirBlock);
    if (ReturnCode != CARD_RESULT_READY)
    {
        __CARDPutControlBlock(Card, ReturnCode);
        return ReturnCode;
    }
    
    // Rename the file
    strncpy(reinterpret_cast<char *>(&CurrentDirBlock[0x8]), newName, CARD_FILENAME_MAX);
    
    // Update the time when the file was last modified
    *reinterpret_cast<uint32_t *>(&CurrentDirBlock[0x28]) = static_cast<uint32_t>(
        gc::OSTime::OSGetTime() / static_cast<int64_t>(gc::os::OSBusClock >> 2));
    
    // Apply the changes
    ReturnCode = __CARDUpdateDir(channel, callback);
    if (ReturnCode == CARD_RESULT_READY)
    {
        return ReturnCode;
    }
    
    __CARDPutControlBlock(Card, ReturnCode);
    return ReturnCode;
}

}