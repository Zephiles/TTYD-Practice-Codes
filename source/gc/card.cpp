#include "gc/card.h"
#include "gc/OSTime.h"
#include "gc/os.h"

#include <cstring>

int32_t card_rename_async(int32_t channel, const char *oldName, const char *newName, CARDCallback callback)
{
    // Make sure the first char of each string is valid
    const char oldNameFirstChar = oldName[0];
    if ((oldNameFirstChar == 0) || (oldNameFirstChar == 0xFF))
    {
        return CARD_RESULT_FATAL_ERROR;
    }

    const char newNameFirstChar = newName[0];
    if ((newNameFirstChar == 0) || (newNameFirstChar == 0xFF))
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

    void *card;
    int32_t ret = __CARDGetControlBlock(channel, reinterpret_cast<void **>(&card));
    if (ret != CARD_RESULT_READY)
    {
        return ret;
    }

    const uint8_t *defaultGameCodeLocation = *reinterpret_cast<uint8_t **>(reinterpret_cast<uint32_t>(card) + 0x10C);
    uint32_t dirBlock = reinterpret_cast<uint32_t>(__CARDGetDirBlock(card));
    int32_t oldNameFileIndex = -1;

    // Loop through each entry and check for files that are named oldName and/or newName
    for (int32_t i = 0; i < CARD_MAX_FILE; i++)
    {
        uint8_t *currentDirBlock = reinterpret_cast<uint8_t *>(dirBlock + (i * 0x40));

        // Make sure the current entry is valid
        if (currentDirBlock[0] == 0xFF)
        {
            continue;
        }

        // Make sure the current entry's game code matches the default's game code
        if (memcmp(currentDirBlock, defaultGameCodeLocation, 6) != 0)
        {
            continue;
        }

        // If a file named oldName is found, keep track of its index
        if (__CARDCompareFileName(currentDirBlock, oldName))
        {
            oldNameFileIndex = i;
            continue;
        }

        // If a file named newName is found, then the desired name already exists, or the file was already renamed
        if (__CARDCompareFileName(currentDirBlock, newName))
        {
            ret = __CARDPutControlBlock(card, CARD_RESULT_EXIST);
            return ret;
        }
    }

    // Make sure a file named oldName was found
    if (oldNameFileIndex == -1)
    {
        ret = __CARDPutControlBlock(card, CARD_RESULT_NOFILE);
        return ret;
    }

    // Make sure this game has proper access to the file that is being renamed
    uint8_t *currentDirBlock = reinterpret_cast<uint8_t *>(dirBlock + (oldNameFileIndex * 0x40));

    ret = __CARDIsWritable(card, currentDirBlock);
    if (ret != CARD_RESULT_READY)
    {
        __CARDPutControlBlock(card, ret);
        return ret;
    }

    // Rename the file
    strncpy(reinterpret_cast<char *>(&currentDirBlock[0x8]), newName, CARD_FILENAME_MAX);

    // Update the time when the file was last modified
    *reinterpret_cast<uint32_t *>(&currentDirBlock[0x28]) =
        static_cast<uint32_t>(OSGetTime() / static_cast<int64_t>(OSBusClock >> 2));

    // Apply the changes
    ret = __CARDUpdateDir(channel, callback);
    if (ret == CARD_RESULT_READY)
    {
        return ret;
    }

    __CARDPutControlBlock(card, ret);
    return ret;
}
