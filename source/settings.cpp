#include "settings.h"
#include "cxx.h"
#include "cheats.h"
#include "displays.h"
#include "memoryWatch.h"
#include "memoryEditor.h"
#include "customState.h"
#include "gc/card.h"
#include "misc/utils.h"
#include "ttyd/cardmgr.h"

#include <cstdint>

const char *gSettingsFileName = "Practice_Codes_Data";

#ifdef TTYD_US
const char *gSettingsDescription = "Settings (US)";
#elif defined TTYD_JP
const char *gSettingsDescription = "Settings (JP)";
#elif defined TTYD_EU
const char *gSettingsDescription = "Settings (EU)";
#endif

int32_t finishAsyncMemoryCardFunc(int32_t ret, int32_t channel)
{
    if (ret == CARD_RESULT_READY)
    {
        // CARDGetResultCode will return CARD_RESULT_BUSY until the current async memory card operation is complete
        do
        {
            ret = CARDGetResultCode(channel);
        } while (ret == CARD_RESULT_BUSY);
    }
    return ret;
}

int32_t checkForMemoryCard(int32_t channel)
{
    // Check if a memory card is inserted
    int32_t ret;

    // To prevent an infinite loop, limit the loop count to 1,000,000
    for (uint32_t i = 0; i < 1000000; i++)
    {
        ret = CARDProbeEx(channel, nullptr, nullptr);
        if (ret != CARD_RESULT_BUSY)
        {
            break;
        }
    }

    return ret;
}

int32_t mountMemoryCard(int32_t channel)
{
    // Make sure the memory card is already unmounted before trying to mount it, as otherwise problems may occur
    CARDUnmount(channel);

    // Check if a memory card is inserted
    int32_t ret = checkForMemoryCard(channel);
    if (ret != CARD_RESULT_READY)
    {
        // Error occured
        return ret;
    }

    // Mount the memory card
    ret = CARDMountAsync(channel, _cardMgrWorkPtr->workArea, nullptr, nullptr);
    ret = finishAsyncMemoryCardFunc(ret, channel);

    // Check if the memory card was successfully mounted, or if there are problems with it
    if ((ret == CARD_RESULT_READY) || (ret == CARD_RESULT_BROKEN))
    {
        // Attempt to repair any possible problems with the memory card
        ret = CARDCheckAsync(channel, nullptr);
        ret = finishAsyncMemoryCardFunc(ret, channel);
    }

    return ret;
}

int32_t loadSettings(int32_t channel)
{
    // Make sure the vanilla code isn't currently doing memory card stuff
    if (cardIsExec())
    {
        // vanilla code is currently doing memory card stuff
        return CARD_RESULT_CARD_IN_USE;
    }

    // Mount the memory card
    int32_t ret = mountMemoryCard(channel);
    if (ret != CARD_RESULT_READY)
    {
        // Error occured
        return ret;
    }

    // Open the settings file
    CARDFileInfo fileInfo;
    ret = CARDOpen(channel, gSettingsFileName, &fileInfo);
    if (ret != CARD_RESULT_READY)
    {
        // Error occured
        CARDUnmount(channel);
        return ret;
    }

    // Set up the array to hold the area of the file that contains the size
    char *fileData = new char[CARD_READ_SIZE];

    // Get the data from the area that holds the size
    ret = readFromFileOnMemoryCard(channel, &fileInfo, fileData, CARD_READ_SIZE, 0x2000);
    if (ret != CARD_RESULT_READY)
    {
        // Error occured
        delete[] fileData;
        CARDClose(&fileInfo);
        CARDUnmount(channel);
        return ret;
    }

    // Make sure the version number of the settings file is supported
    const SettingsHeader *settingsHeaderPtr = getSettingsHeaderPtr(fileData, 0);
    if (settingsHeaderPtr->getVersion() > SETTINGS_VERSION)
    {
        // Invalid settings version
        delete[] fileData;
        CARDClose(&fileInfo);
        CARDUnmount(channel);
        return CARD_RESULT_INVALID_SETTNGS_VERSION;
    }

    // Get the size of the file
    // Adjust the data size to be in multiples of CARD_READ_SIZE, rounding up
    const int32_t dataSizeAdjusted = roundUp(settingsHeaderPtr->getDataSize(), CARD_READ_SIZE);

    // Delete the data that holds the size, as it's not needed anymore
    delete[] fileData;

    // Set up the memory to be copied from the file
    // Allocate to the front of the heap to avoid possible fragmentation later
    fileData = new (true) char[dataSizeAdjusted];

    // Get the data from the file
    ret = readFromFileOnMemoryCard(channel, &fileInfo, fileData, dataSizeAdjusted, 0x2000);

    // Close and unmount the memory card, as it's no longer needed
    CARDClose(&fileInfo);
    CARDUnmount(channel);

    if (ret != CARD_RESULT_READY)
    {
        // Error occured
        delete[] fileData;
        return ret;
    }

    // Reinitialize settingsHeaderPtr
    settingsHeaderPtr = getSettingsHeaderPtr(fileData, 0);

    // Get the misc header data if it is included
    const uint32_t miscHeaderOffset = settingsHeaderPtr->getMiscHeaderOffset();
    if (miscHeaderOffset > 0)
    {
        // Get the misc data if it is included
        const MiscSettingsHeader *miscSettingsHeaderPtr = getMiscSettingsHeaderPtr(fileData, miscHeaderOffset);
        const uint32_t miscDataOffset = miscSettingsHeaderPtr->getDataOffset();

        if (miscDataOffset > 0)
        {
            const MiscSettingsData *miscSettingsDataPtr = getMiscSettingsDataPtr(fileData, miscDataOffset);
            miscSettingsDataPtr->getData();
        }
    }

    // Get the cheats header data if it is included
    const uint32_t cheatsHeaderOffset = settingsHeaderPtr->getCheatsHeaderOffset();
    if (cheatsHeaderOffset > 0)
    {
        // Get the cheats data if it is included
        const CheatsSettingsHeader *cheatsSettingsHeaderPtr = getCheatsSettingsHeaderPtr(fileData, cheatsHeaderOffset);
        const uint32_t cheatsDataOffset = cheatsSettingsHeaderPtr->getDataOffset();

        if (cheatsDataOffset > 0)
        {
            const CheatsSettingsData *cheatsSettingsDataPtr = getCheatsSettingsDataPtr(fileData, cheatsDataOffset);

            cheatsSettingsDataPtr->getData(cheatsSettingsHeaderPtr->getTotalCheats(),
                                           cheatsSettingsHeaderPtr->getTotalButtonCombos());
        }
    }

    // Get the displays header data if it is included
    const uint32_t displaysHeaderOffset = settingsHeaderPtr->getDisplaysHeaderOffset();
    if (displaysHeaderOffset > 0)
    {
        // Get the displays data if it is included
        const DisplaysSettingsHeader *displaysSettingsHeaderPtr = getDisplaysSettingsHeaderPtr(fileData, displaysHeaderOffset);
        const uint32_t displaysDataOffset = displaysSettingsHeaderPtr->getDataOffset();

        if (displaysDataOffset > 0)
        {
            const DisplaysSettingsData *displaysSettingsDataPtr = getDisplaysSettingsDataPtr(fileData, displaysDataOffset);

            displaysSettingsDataPtr->getData(displaysSettingsHeaderPtr->getTotalDisplays(),
                                             displaysSettingsHeaderPtr->getTotalManuallyPositionEntries(),
                                             displaysSettingsHeaderPtr->getTotalButtonCombos());
        }
    }

    // Get the memory watches header data if it is included
    const uint32_t memoryWatchesHeaderOffset = settingsHeaderPtr->getMemoryWatchesHeaderOffset();
    if (memoryWatchesHeaderOffset > 0)
    {
        // Get the memory watches data if it is included
        const MemoryWatchesSettingsHeader *memoryWatchesSettingsHeaderPtr =
            getMemoryWatchesSettingsHeaderPtr(fileData, memoryWatchesHeaderOffset);

        const uint32_t memoryWatchesDataOffset = memoryWatchesSettingsHeaderPtr->getDataOffset();
        if (memoryWatchesDataOffset > 0)
        {
            const MemoryWatchesSettingsData *memoryWatchesSettingsDataPtr =
                getMemoryWatchesSettingsDataPtr(fileData, memoryWatchesDataOffset);

            // Get the main data for each watch
            const uint32_t addressOffsetsOffset =
                memoryWatchesSettingsDataPtr->getData(memoryWatchesSettingsHeaderPtr->getTotalWatches(),
                                                      memoryWatchesSettingsHeaderPtr->getWatchNameLength());

            // Get the data for each watch's address offsets
            memoryWatchesSettingsDataPtr->getAddressOffsetsData(addressOffsetsOffset);
        }
    }

    // Get the memory editor header data if it is included
    const uint32_t memoryEditorHeaderOffset = settingsHeaderPtr->getMemoryEditorHeaderOffset();
    if (memoryEditorHeaderOffset > 0)
    {
        // Get the memory watches data if it is included
        const MemoryEditorSettingsHeader *memoryEditorSettingsHeaderPtr =
            getMemoryEditorSettingsHeaderPtr(fileData, memoryEditorHeaderOffset);

        const uint32_t memoryEditorDataOffset = memoryEditorSettingsHeaderPtr->getDataOffset();
        if (memoryEditorDataOffset > 0)
        {
            const MemoryEditorSettingsData *memoryEditorSettingsDataPtr =
                getMemoryEditorSettingsDataPtr(fileData, memoryEditorDataOffset);

            memoryEditorSettingsDataPtr->getData(memoryEditorSettingsHeaderPtr->getTotalFlags());
        }
    }

    // Get the custom states header data if it is included
    const uint32_t customStatesHeaderOffset = settingsHeaderPtr->getCustomStatesHeaderOffset();
    if (customStatesHeaderOffset > 0)
    {
        // Get the custom states data if it is included
        const CustomStatesSettingsHeader *customStatesSettingsHeaderPtr =
            getCustomStatesSettingsHeaderPtr(fileData, customStatesHeaderOffset);

        const uint32_t customStatesDataOffset = customStatesSettingsHeaderPtr->getDataOffset();
        if (customStatesDataOffset > 0)
        {
            const CustomStatesSettingsData *customStatesSettingsDataPtr =
                getCustomStatesSettingsDataPtr(fileData, customStatesDataOffset);

            customStatesSettingsDataPtr->getData(customStatesSettingsHeaderPtr->getTotalStates(),
                                                 customStatesSettingsHeaderPtr->getStateNameLength());
        }
    }

    // Done
    delete[] fileData;
    return CARD_RESULT_READY;
}

// This function keeps the current memory card file open, but closes it and unmounts the memory card if it fails to create the
// settings file
int32_t createSettingsFile(int32_t channel, CARDFileInfo *settingsFileInfoPtr, uint32_t dataSizeAdjusted)
{
    int32_t ret;

    // If creating the file on slot B, unmount the current slot and mount slot A
    if (channel != CARD_SLOT_A)
    {
        ret = CARDUnmount(channel);
        if (ret != CARD_RESULT_READY)
        {
            // Error occured
            return ret;
        }

        ret = mountMemoryCard(CARD_SLOT_A);
        if (ret != CARD_RESULT_READY)
        {
            // Error occured
            return ret;
        }
    }

    // Get the banner and icon data from the Practice Codes gci
    CARDFileInfo fileInfo;
    ret = CARDOpen(CARD_SLOT_A, "rel", &fileInfo);
    if (ret != CARD_RESULT_READY)
    {
        // Error occured
        CARDUnmount(CARD_SLOT_A);
        return ret;
    }

    // Set up the memory for the banner and icon data to go in
    char *bannerIconData = new char[0x2000];

    // Get the banner and icon data
    ret = readFromFileOnMemoryCard(CARD_SLOT_A, &fileInfo, bannerIconData, 0x2000, 0);
    if (ret != CARD_RESULT_READY)
    {
        // Error occured
        delete[] bannerIconData;
        CARDClose(&fileInfo);
        CARDUnmount(CARD_SLOT_A);
        return ret;
    }

    // The Practice Codes gci is no longer needed, so close it
    ret = CARDClose(&fileInfo);
    if (ret != CARD_RESULT_READY)
    {
        // Error occured
        delete[] bannerIconData;
        CARDUnmount(CARD_SLOT_A);
        return ret;
    }

    // If creating the file on slot B, unmount the current slot and mount slot B
    if (channel != CARD_SLOT_A)
    {
        ret = CARDUnmount(CARD_SLOT_A);
        if (ret != CARD_RESULT_READY)
        {
            // Error occured
            return ret;
        }

        ret = mountMemoryCard(channel);
        if (ret != CARD_RESULT_READY)
        {
            // Error occured
            return ret;
        }
    }

    // Create the new file
    // The new file should be opened automatically via CARDCreateAsync
    ret = createFileOnMemoryCard(channel, gSettingsFileName, dataSizeAdjusted, settingsFileInfoPtr);
    if (ret != CARD_RESULT_READY)
    {
        // Error occured
        delete[] bannerIconData;
        CARDUnmount(channel);
        return ret;
    }

    // Get the stats for the new file
    const int32_t fileNum = settingsFileInfoPtr->fileNum;
    CARDStat stat;

    ret = CARDGetStatus(channel, fileNum, &stat);
    if (ret != CARD_RESULT_READY)
    {
        // Error occured
        delete[] bannerIconData;
        CARDClose(settingsFileInfoPtr);
        CARDUnmount(channel);
        return ret;
    }

    // Set the stats for the new file
    stat.bannerFormat = 2;
    stat.iconAddress = 0;
    stat.iconFormat = 2;
    stat.iconSpeed = 3;
    stat.commentAddress = 0x2000;

    // Set the stats for the new file
    ret = setMemoryCardFileStatus(channel, fileNum, &stat);
    if (ret != CARD_RESULT_READY)
    {
        // Error occured
        delete[] bannerIconData;
        CARDClose(settingsFileInfoPtr);
        CARDUnmount(channel);
        return ret;
    }

    // Write the banner and icon data to the new file
    ret = writeToFileOnMemoryCard(channel, settingsFileInfoPtr, bannerIconData, 0x2000, 0);

    // Done
    delete[] bannerIconData;

    if (ret != CARD_RESULT_READY)
    {
        // Error occured
        CARDClose(settingsFileInfoPtr);
        CARDUnmount(channel);
    }

    return ret;
}

int32_t saveSettings(int32_t channel)
{
    // Make sure the vanilla code isn't currently doing memory card stuff
    if (cardIsExec())
    {
        // vanilla code is currently doing memory card stuff
        return CARD_RESULT_CARD_IN_USE;
    }

    // Mount the memory card
    int32_t ret = mountMemoryCard(channel);
    if (ret != CARD_RESULT_READY)
    {
        // Error occured
        return ret;
    }

    // Get the size thats going to be written
    const uint32_t dataSize = getWrittenSettingsSize();

    // Adjust the data size to be in multiples of CARD_WRITE_SIZE, rounding up
    const uint32_t dataSizeAdjusted = roundUp(dataSize, CARD_WRITE_SIZE);

    // Open the settings file if it exists
    const char *settingsFileName = gSettingsFileName;
    CARDFileInfo fileInfo;

    ret = CARDOpen(channel, settingsFileName, &fileInfo);
    switch (ret)
    {
        case CARD_RESULT_READY:
        {
            // Set up the array to hold the area of the file that contains the size
            char *fileData = new char[CARD_READ_SIZE];

            // Get the data from the area that holds the size
            ret = readFromFileOnMemoryCard(channel, &fileInfo, fileData, CARD_READ_SIZE, 0x2000);
            if (ret != CARD_RESULT_READY)
            {
                // Error occured
                delete[] fileData;
                CARDClose(&fileInfo);
                CARDUnmount(channel);
                return ret;
            }

            // Get the size of the file
            // Adjust the file size to be in multiples of CARD_WRITE_SIZE, rounding up
            const SettingsHeader *settingsHeaderPtr = getSettingsHeaderPtr(fileData, 0);
            const uint32_t storedDataSizeAdjusted = roundUp(settingsHeaderPtr->getDataSize(), CARD_WRITE_SIZE);

            // Delete the data that holds the size, as it's not needed anymore
            delete[] fileData;

            // Make sure the size being written matches the current size
            if (dataSizeAdjusted != storedDataSizeAdjusted)
            {
                // The new size differs from the current size, so a new file must be created
                // Close the file
                ret = CARDClose(&fileInfo);
                if (ret != CARD_RESULT_READY)
                {
                    // Error occured
                    CARDUnmount(channel);
                    return ret;
                }

                // Delete the file
                ret = CARDDelete(channel, settingsFileName);
                if (ret != CARD_RESULT_READY)
                {
                    // Error occured
                    CARDUnmount(channel);
                    return ret;
                }

                // Make the new file
                // createSettingsFile keeps the file open, but closes and unmounts if it fails to create the file
                // Add 0x2000 to account for the banner and icon data
                ret = createSettingsFile(channel, &fileInfo, dataSizeAdjusted + 0x2000);
                if (ret != CARD_RESULT_READY)
                {
                    // Error occured
                    return ret;
                }
            }
            break;
        }
        case CARD_RESULT_NOFILE:
        {
            // Settings file does not exist, so create it
            // createSettingsFile keeps the file open, but closes and unmounts if it fails to create the file
            // Add 0x2000 to account for the banner and icon data
            ret = createSettingsFile(channel, &fileInfo, dataSizeAdjusted + 0x2000);
            if (ret != CARD_RESULT_READY)
            {
                // Error occured
                return ret;
            }
            break;
        }
        default:
        {
            // Unhandled error
            CARDUnmount(channel);
            return ret;
        }
    }

    // Set up the memory to be written to the file
    char *fileData = new char[dataSizeAdjusted];

    // Start writing the data
    // Write the title and description
    PracticeCodesDataHeader *practiceCodesDataHeaderPtr = getPracticeCodesDataHeaderPtr(fileData, 0);
    practiceCodesDataHeaderPtr->setData("Paper Mario", gSettingsDescription);

    // Write the intiial settings header data
    uint32_t offset = 0;
    SettingsHeader *settingsHeaderPtr = getSettingsHeaderPtr(fileData, offset);
    offset += settingsHeaderPtr->setData(SETTINGS_VERSION, dataSize);

    // Write the misc header data
    MiscSettingsHeader *miscSettingsHeaderPtr = getMiscSettingsHeaderPtr(fileData, offset);
    offset += miscSettingsHeaderPtr->setData(settingsHeaderPtr, offset);

    // Write the misc data
    MiscSettingsData *miscSettingsDataPtr = getMiscSettingsDataPtr(fileData, offset);
    offset += miscSettingsDataPtr->setData();

    // Write the cheats header data
    CheatsSettingsHeader *cheatsSettingsHeaderPtr = getCheatsSettingsHeaderPtr(fileData, offset);
    offset += cheatsSettingsHeaderPtr->setData(settingsHeaderPtr, offset);

    // Write the cheats data
    CheatsSettingsData *cheatsSettingsDataPtr = getCheatsSettingsDataPtr(fileData, offset);
    offset += cheatsSettingsDataPtr->setData(gCheats);

    // Write the displays header data
    DisplaysSettingsHeader *displaysSettingsHeaderPtr = getDisplaysSettingsHeaderPtr(fileData, offset);
    offset += displaysSettingsHeaderPtr->setData(settingsHeaderPtr, offset);

    // Write the displays data
    DisplaysSettingsData *displaysSettingsDataPtr = getDisplaysSettingsDataPtr(fileData, offset);
    offset += displaysSettingsDataPtr->setData(gDisplays);

    // Write the memory watches header data
    MemoryWatch *memoryWatchPtr = &gMemoryWatch;
    MemoryWatchesSettingsHeader *memoryWatchesSettingsHeaderPtr = getMemoryWatchesSettingsHeaderPtr(fileData, offset);
    const uint32_t memoryWatchesDataOffset = memoryWatchesSettingsHeaderPtr->setData(settingsHeaderPtr, memoryWatchPtr, offset);

    // Make sure there is at least one watch to write
    if (memoryWatchesDataOffset > 0)
    {
        offset += memoryWatchesDataOffset;

        // Write the main data for each watch
        const uint32_t totalEntries = memoryWatchPtr->getTotalEntries();
        const MemoryWatchEntry *entriesPtr = memoryWatchPtr->getEntriesPtr();
        MemoryWatchesSettingsData *memoryWatchesSettingsDataPtr = getMemoryWatchesSettingsDataPtr(fileData, offset);

        for (uint32_t i = 0; i < totalEntries; i++)
        {
            offset += memoryWatchesSettingsDataPtr->setData(&entriesPtr[i]);

            // Update the pointer to get the next entry
            memoryWatchesSettingsDataPtr = getMemoryWatchesSettingsDataPtr(fileData, offset);
        }

        // Write the data for each watch's address offsets
        for (uint32_t i = 0; i < totalEntries; i++)
        {
            offset += memoryWatchesSettingsDataPtr->setAddressOffsetsData(&entriesPtr[i]);

            // Update the pointer to get the next entry
            memoryWatchesSettingsDataPtr = getMemoryWatchesSettingsDataPtr(fileData, offset);
        }
    }

    // Write the memory editor header data
    MemoryEditorSettingsHeader *memoryEditorSettingsHeaderPtr = getMemoryEditorSettingsHeaderPtr(fileData, offset);
    offset += memoryEditorSettingsHeaderPtr->setData(settingsHeaderPtr, offset);

    // Write the memory editor data
    MemoryEditorSettingsData *memoryEditorSettingsDataPtr = getMemoryEditorSettingsDataPtr(fileData, offset);
    offset += memoryEditorSettingsDataPtr->setData(gMemoryEditor);

    // Write the custom states header data
    CustomState *customStatePtr = &gCustomState;
    CustomStatesSettingsHeader *customStatesSettingsHeaderPtr = getCustomStatesSettingsHeaderPtr(fileData, offset);
    const uint32_t customStatesDataOffset = customStatesSettingsHeaderPtr->setData(settingsHeaderPtr, customStatePtr, offset);

    // Make sure there is at least one watch to write
    if (customStatesDataOffset > 0)
    {
        offset += customStatesDataOffset;

        // Write the custom states data
        CustomStatesSettingsData *customSattesSettingsDataPtr = getCustomStatesSettingsDataPtr(fileData, offset);
        offset += customSattesSettingsDataPtr->setData(&gCustomState);
    }

    // Write the data to the file
    ret = writeToFileOnMemoryCard(channel, &fileInfo, fileData, dataSizeAdjusted, 0x2000);

    // Done
    delete[] fileData;
    CARDClose(&fileInfo);
    CARDUnmount(channel);
    return ret;
}

int32_t deleteSettingsFile(int32_t channel)
{
    // Make sure the vanilla code isn't currently doing memory card stuff
    if (cardIsExec())
    {
        // vanilla code is currently doing memory card stuff
        return CARD_RESULT_CARD_IN_USE;
    }

    // Mount the memory card
    int32_t ret = mountMemoryCard(channel);
    if (ret != CARD_RESULT_READY)
    {
        // Error occured
        return ret;
    }

    // Delete the settings file
    ret = CARDDelete(channel, gSettingsFileName);

    // Done
    CARDUnmount(channel);
    return ret;
}
