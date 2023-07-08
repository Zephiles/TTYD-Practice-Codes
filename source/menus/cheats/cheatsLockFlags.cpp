#include "menuUtils.h"
#include "drawText.h"
#include "cheats.h"
#include "classes/window.h"
#include "menus/cheatsMenu.h"
#include "misc/utils.h"
#include "ttyd/seq_mapchange.h"
#include "ttyd/evtmgr.h"
#include "ttyd/mariost.h"

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cinttypes>

const MenuOption gCheatsMenuLockFlagsOptions[] = {
    "Lock GSWs",
    cheatsMenuLockFlagsLockSeletedFlags,

    "Lock GSWFs",
    cheatsMenuLockFlagsLockSeletedFlags,

    "Lock GWs",
    cheatsMenuLockFlagsLockSeletedFlags,

    "Lock GFs",
    cheatsMenuLockFlagsLockSeletedFlags,

    "Lock LSWs",
    cheatsMenuLockFlagsLockSeletedFlags,

    "Lock LSWFs",
    cheatsMenuLockFlagsLockSeletedFlags,

    "Set New LSW Area",
    cheatsMenuLockFlagsSetNewArea,

    "Set New LSWF Area",
    cheatsMenuLockFlagsSetNewArea,
};

const MenuFunctions gCheatsMenuLockFlagsInitFuncs = {
    gCheatsMenuLockFlagsOptions,
    controlsBasicMenuLayout,
    cheatsMenuLockFlagsDraw,
    nullptr, // Exit function not needed
};

void cheatsMenuLockFlagsInit(Menu *menuPtr)
{
    (void)menuPtr;

    constexpr uint32_t totalOptions = sizeof(gCheatsMenuLockFlagsOptions) / sizeof(MenuOption);
    enterNextMenu(&gCheatsMenuLockFlagsInitFuncs, totalOptions);
}

void CheatsMenu::drawLockFlagsInfo() const
{
    // Get the text position for the top-left of the window
    float tempPosX;
    float tempPosY;
    const float scale = this->scale;
    gRootWindow->getTextPosXY(nullptr, WindowAlignment::TOP_LEFT, scale, &tempPosX, &tempPosY);

    // Get the width that is being used by the main text options
    float textWidth;
    getTextWidthHeight("Lock GSWFs", scale, &textWidth, nullptr);

    // Set the text to be a bit to the right of the main text
    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    const float textIncrement = 20.f * scale;
    const float posXBase = tempPosX;
    const float posYBase = tempPosY;

    float posX = posXBase + textWidth + textIncrement;
    float posY = posYBase;

    // Draw the Yes/No texts for each option
    const float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    Cheats *cheatsPtr = gCheats;
    const char *string;
    uint32_t color;

    for (uint32_t i = 0; i < TOTAL_LOCK_FLAGS_OPTIONS; i++)
    {
        const bool flagIsSet = cheatsPtr->miscFlagIsSet(CheatsMiscFlag::CHEATS_MISC_FLAG_LOCK_GSW + i);
        getYesNoTextAndColor(flagIsSet, &string, &color, 0xFF);
        drawText(string, posX, posY, scale, color);
        posY -= lineDecrement;
    }

    // Get the position two lines under the main text
    const int32_t totalOptions = static_cast<int32_t>(gMenu->getTotalOptions());
    posX = posXBase;
    posY = posYBase - (lineDecrement * intToFloat(totalOptions + 1));

    auto getAreaTextAndColor = [](char *stringOut, uint32_t stringSize, const char *areaLocked, bool flag)
    {
        // Make sure the current area with its flags locked is valid
        if (!flag || (areaLocked[0] == '\0'))
        {
            // Use snprintf to make sure stringSize is not exceeded, and that a null terminator is properly applied
            snprintf(stringOut, stringSize, "None");
            return getColorGrayedOut(0xFF);
        }
        else
        {
            // Check to see if the current area is one that can have its flags locked
            auto areaNames = gCheatsAreaNames;
            bool foundString = false;

            for (uint32_t i = 0; i < CHEATS_TOTAL_AREAS; i++)
            {
                const char **currentArea = areaNames[i];
                if (strcmp(areaLocked, currentArea[0]) == 0)
                {
                    foundString = true;

                    // Use snprintf to make sure stringSize is not exceeded, and that a null terminator is properly applied
                    snprintf(stringOut, stringSize, currentArea[1]);
                    break;
                }
            }

            if (!foundString)
            {
                // The current area cannot have its flags locked, so set the string to whatever the area string is
                // Use snprintf to make sure stringSize is not exceeded, and that a null terminator is properly applied
                snprintf(stringOut, stringSize, areaLocked);
            }
        }

        return getColorWhite(0xFF);
    };

    // Draw the text indicating which area has its LSW flags locked
    drawText("Current Area LSW Flags Locked", posX, posY, scale, getColorWhite(0xFF));
    posY -= lineDecrement;

    // Draw the text for which area has its LSW flags locked
    char buf[64];
    constexpr uint32_t bufSize = sizeof(buf);
    LockFlagsCheat *lockFlagsPtr = cheatsPtr->getLockFlagsCheatPtr();

    color = getAreaTextAndColor(buf,
                                bufSize,
                                lockFlagsPtr->getLswAreaLocked(),
                                cheatsPtr->miscFlagIsSet(CheatsMiscFlag::CHEATS_MISC_FLAG_LOCK_LSW));

    drawText(buf, posX, posY, scale, color);
    posY -= (lineDecrement * 2.f);

    // Draw the text indicating which area has its LSWF flags locked
    drawText("Current Area LSWF Flags Locked", posX, posY, scale, getColorWhite(0xFF));
    posY -= lineDecrement;

    // Draw the text for which area has its flags locked
    color = getAreaTextAndColor(buf,
                                bufSize,
                                lockFlagsPtr->getLswfAreaLocked(),
                                cheatsPtr->miscFlagIsSet(CheatsMiscFlag::CHEATS_MISC_FLAG_LOCK_LSWF));

    drawText(buf, posX, posY, scale, color);
}

void cheatsMenuLockFlagsDraw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    drawBasicMenuLayout(cameraId, user);

    // Draw the info for the flags
    gCheatsMenu->drawLockFlagsInfo();
}

void lockFlagsSetAreaString(LockFlagsCheat *lockFlagsPtr, uint32_t currentIndex)
{
    // Update the area for the flags to be locked
    char *area;
    if (currentIndex == LockFlagsOptions::LOCK_FLAGS_LSW)
    {
        area = lockFlagsPtr->getLswAreaLocked();
    }
    else
    {
        area = lockFlagsPtr->getLswfAreaLocked();
    }

    strncpy(area, _next_area, 3);
}

void cheatsMenuLockFlagsLockSeletedFlags(Menu *menuPtr)
{
    // Flip the bool for the selected flags
    Cheats *cheatsPtr = gCheats;
    LockFlagsCheat *lockFlagsPtr = cheatsPtr->getLockFlagsCheatPtr();
    const uint32_t currentIndex = menuPtr->getCurrentIndex();

    if (cheatsPtr->toggleMiscFlag(CheatsMiscFlag::CHEATS_MISC_FLAG_LOCK_GSW + currentIndex))
    {
        // Lock the flags
        // Get the pointer and size for the selected flags
        void *sourceMemory;
        uint32_t sourceSize;
        getLockFlagsRegionPtrAndSize(currentIndex, &sourceMemory, &sourceSize);

        // Allocate memory for the flags
        uint8_t *prevMemoryPtr = lockFlagsPtr->allocMemoryForPrevMemory(currentIndex, sourceSize);
        if (!prevMemoryPtr)
        {
            // Something went wrong
            return;
        }

        // Back up the memory for the flags
        memcpy(prevMemoryPtr, sourceMemory, sourceSize);

        // Do other stuff depending on the current index
        switch (currentIndex)
        {
            case LockFlagsOptions::LOCK_FLAGS_GSW:
            {
                // Back up the sequence position
                lockFlagsPtr->setSequencePosition(getSequencePosition());
                break;
            }
            case LockFlagsOptions::LOCK_FLAGS_LSW:
            case LockFlagsOptions::LOCK_FLAGS_LSWF:
            {
                // Update the area for the flags to be locked
                lockFlagsSetAreaString(lockFlagsPtr, currentIndex);
                break;
            }
            default:
            {
                break;
            }
        }
    }
    else
    {
        // Free the memory for the selected flags
        lockFlagsPtr->freePrevMemory(currentIndex);
    }
}

void cheatsMenuLockFlagsSetNewArea(Menu *menuPtr)
{
    // Get the proper index to use for the selected flags
    // This assumes that both the LSW and LSWF options are at the end of the options
    const uint32_t selectedFlagIndex = menuPtr->getCurrentIndex() - 2;

    // If the flag for the selected flags is not set, then do nothing
    Cheats *cheatsPtr = gCheats;
    if (!cheatsPtr->miscFlagIsSet(CheatsMiscFlag::CHEATS_MISC_FLAG_LOCK_GSW + selectedFlagIndex))
    {
        return;
    }

    // If memory for the selected flags is not allocated, then do nothing
    LockFlagsCheat *lockFlagsPtr = cheatsPtr->getLockFlagsCheatPtr();
    uint8_t *prevMemoryPtr = lockFlagsPtr->getPrevMemoryPtr(selectedFlagIndex);
    if (!prevMemoryPtr)
    {
        return;
    }

    // Back up the memory for the flags
    void *sourceMemory;
    uint32_t sourceSize;
    getLockFlagsRegionPtrAndSize(selectedFlagIndex, &sourceMemory, &sourceSize);
    memcpy(prevMemoryPtr, sourceMemory, sourceSize);

    // Update the area for the flags to be locked
    lockFlagsSetAreaString(lockFlagsPtr, selectedFlagIndex);
}

void *getLockFlagsRegionPtr(uint32_t region)
{
    EvtWork *eventWorkPtr = evtGetWork();
    GlobalWork *tempGlobalWorkPtr = globalWorkPtr;

    switch (region)
    {
        case LockFlagsOptions::LOCK_FLAGS_GSW:
        {
            return &tempGlobalWorkPtr->mGSW[0];
        }
        case LockFlagsOptions::LOCK_FLAGS_GSWF:
        {
            return &tempGlobalWorkPtr->mGSWF[0];
        }
        case LockFlagsOptions::LOCK_FLAGS_GW:
        {
            return &eventWorkPtr->gwData[0];
        }
        case LockFlagsOptions::LOCK_FLAGS_GF:
        {
            return &eventWorkPtr->gfData[0];
        }
        case LockFlagsOptions::LOCK_FLAGS_LSW:
        {
            return &tempGlobalWorkPtr->mLSW[0];
        }
        case LockFlagsOptions::LOCK_FLAGS_LSWF:
        {
            return &tempGlobalWorkPtr->mLSWF[0];
        }
        default:
        {
            return nullptr;
        }
    }
}

uint32_t getLockFlagsRegionSize(uint32_t region)
{
    switch (region)
    {
        case LockFlagsOptions::LOCK_FLAGS_GSW:
        {
            return sizeof(GlobalWork::mGSW);
        }
        case LockFlagsOptions::LOCK_FLAGS_GSWF:
        {
            return sizeof(GlobalWork::mGSWF);
        }
        case LockFlagsOptions::LOCK_FLAGS_GW:
        {
            return sizeof(EvtWork::gwData);
        }
        case LockFlagsOptions::LOCK_FLAGS_GF:
        {
            return sizeof(EvtWork::gfData);
        }
        case LockFlagsOptions::LOCK_FLAGS_LSW:
        {
            return sizeof(GlobalWork::mLSW);
        }
        case LockFlagsOptions::LOCK_FLAGS_LSWF:
        {
            return sizeof(GlobalWork::mLSWF);
        }
        default:
        {
            return 0;
        }
    }
}

void getLockFlagsRegionPtrAndSize(uint32_t region, void **ptr, uint32_t *size)
{
    if (ptr)
    {
        *ptr = getLockFlagsRegionPtr(region);
    }

    if (size)
    {
        *size = getLockFlagsRegionSize(region);
    }
}
