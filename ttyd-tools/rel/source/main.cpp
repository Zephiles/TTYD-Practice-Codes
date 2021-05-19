#include "main.h"
#include "global.h"
#include "commonfunctions.h"
#include "menufunctions.h"
#include "draw.h"
#include "mod.h"
#include "patch.h"
#include "menu.h"
#include "codes.h"
#include "assembly.h"

#include <gc/OSCache.h>
#include <gc/OSAlloc.h>
#include <ttyd/item_data.h>
#include <ttyd/seq_mapchange.h>
#include <ttyd/seqdrv.h>
#include <ttyd/npcdrv.h>
#include <ttyd/swdrv.h>
#include <ttyd/mario.h>
#include <ttyd/mario_party.h>
#include <ttyd/party.h>
#include <ttyd/memory.h>
#include <ttyd/seq_title.h>
#include <ttyd/mario_pouch.h>
#include <ttyd/sac_scissor.h>

#include <cstdio>
#include <cinttypes>
#include <cstring>

namespace mod {

bool displayMegaJumpBadgeInMenu(uint32_t checkBit)
{
    if (checkIfBadgeEquipped(ttyd::item_data::Item::UpgradedPowerJump))
    {
        return false;
    }
    else
    {
        return (checkBit & (1 << 9)); // Check if the 9 bit is on
    }
}

bool displayMegaHammerBadgesInMenu(uint32_t checkBit)
{
    if (checkIfBadgeEquipped(ttyd::item_data::Item::UpgradedPowerSmash) || 
        checkIfBadgeEquipped(ttyd::item_data::Item::UpgradedQuakeHammer))
    {
        return false;
    }
    else
    {
        return (checkBit & (1 << 10)); // Check if the 10 bit is on
    }
}

uint32_t fixBlooperCrash1(uint32_t unkValue, void *battleUnitPointer)
{
    if (checkIfPointerIsValid(battleUnitPointer))
    {
#ifdef TTYD_US
        uint32_t offset = 0x218;
#elif defined TTYD_JP
        uint32_t offset = 0x214;
#elif defined TTYD_EU
        uint32_t offset = 0x218;
#endif
        
        *reinterpret_cast<uint32_t *>(
            reinterpret_cast<uint32_t>(battleUnitPointer) 
                + offset) = unkValue;
    }
    
    return 2;
}

void preventTextboxOptionSelection(char *currentText, void *storeAddress, int32_t selectedOption)
{
    int32_t NewOption;
    // const int32_t FirstOption = 0;
    const int32_t SecondOption = 1;
    const int32_t ThirdOption = 2;
    
    if (compareStrings(currentText, "stg6_rsh_diary_01_yn"))
    {
        // Prevent the first option from being selected, so that the game does not crash when reading the diary
        // Only needs to run when not on the train
        if (!compareStrings(ttyd::seq_mapchange::NextArea, "rsh"))
        {
            if (selectedOption == ThirdOption)
            {
                ttyd::seqdrv::seqSetSeq(ttyd::seqdrv::SeqIndex::kGameOver, nullptr, nullptr);
            }
            
            NewOption = SecondOption;
        }
        else
        {
            // Store the original value
            NewOption = selectedOption;
        }
    }
    else
    {
        // Store the original value
        NewOption = selectedOption;
    }
    
    *reinterpret_cast<int32_t *>(
        reinterpret_cast<uint32_t>(storeAddress) + 0x9C) = NewOption;
}

void fixRoomProblems() // Gets called in initStageEvents
{
    uint32_t SequencePosition = getSequencePosition();
    
    if (compareStringToNextMap("nok_00"))
    {
        // Prevent the game from crashing if the player enters the intro cutscene after interacting with an NPC that is past slot 10
        // Check if the cutscene is going to play
        if (SequencePosition < 26)
        {
            // Clear the pointer used to check which animation Mario should use when greeting the Koopa
            ttyd::npcdrv::FbatData *fbatPtr = ttyd::npcdrv::fbatGetPointer();
            fbatPtr->wHitNpc = nullptr; // Mario will do no animation when the pointer is not set
        }
    }
    else if (compareStringToNextMap("rsh_05_a"))
    {
        // Prevent the game from crashing if the player enters rsh_05_a with the Sequence past 338
        if (SequencePosition > 338)
        {
            // Set the Sequence to 338 to prevent the crash
            setSequencePosition(338);
        }
    }
    else if (compareStringToNextMap("aji_13"))
    {
        // Prevent the game from crashing if the conveyor belt has not been activated
        // Set GW(11) to 0 upon entering the room to prevent the crash
        setGW(11, 0);
    }
    else if (compareStringToNextMap("las_08"))
    {
        // Prevent the game from crashing if the player entered las_08 with the Sequence as 385 and GSW(1121) at 7
        if (SequencePosition == 385)
        {
            // Check if GSW(1121) is currently at 7
            uint32_t GSW_1121 = ttyd::swdrv::swByteGet(1121);
            if (GSW_1121 == 7)
            {
                // Lower the value to 6 to prevent the game from crashing
                ttyd::swdrv::swByteSet(1121, 6);
            }
        }
    }
}

void *fixEvtMapBlendSetFlagPartnerCrash(void *partnerPtr)
{
    // Bring out a partner if no partner is currently out
    if (!partnerPtr)
    {
        spawnFailsafePartnerOrFollower(true);
        return getPartnerPointer();
    }
    else
    {
        return partnerPtr;
    }
}

void *fixEvtMapBlendSetFlagFollowerCrash(void *followerPtr)
{
    // Bring out a follower if no follower is currently out
    if (!followerPtr)
    {
        spawnFailsafePartnerOrFollower(false);
        return getFollowerPointer();
    }
    else
    {
        return followerPtr;
    }
}

void *preventPouchInitMemoryLeak(uint32_t heap, uint32_t size)
{
    // Check if the memory has already been allocated or not
    void *PouchPtr = ttyd::mario_pouch::pouchGetPtr();
    if (PouchPtr)
    {
        // The memory has already been allocated
        return PouchPtr;
    }
    else
    {
        // The memory has not been allocated, so allocate it
        return ttyd::memory::__memAlloc(heap, size);
    }
}

const char *replaceJumpFallAnim(char *jumpFallString)
{
    if (compareStringsSize(jumpFallString, "M_J_", 4))
    {
        const char *MapCheck = "gor_01";
        if (compareStringToNextMap(MapCheck))
        {
            if (MenuVar.MarioFreeze)
            {
                // Return an arbitrary string
                return MapCheck;
            }
            else
            {
                ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
                
                int32_t MarPosX = static_cast<int32_t>(player->playerPosition[0]);
                if ((MarPosX >= -9) && (MarPosX <= 8))
                {
                    int32_t MarPosZ = static_cast<int32_t>(player->playerPosition[2]);
                    if ((MarPosZ >= 245) && (MarPosZ <= 265))
                    {
                        int32_t MarPosY = static_cast<int32_t>(player->playerPosition[1]);
                        if (MarPosY >= 59)
                        {
                            MenuVar.MarioFreeze = true;
                            
                            // Return an arbitrary string
                            return MapCheck;
                        }
                    }
                }
            }
        }
    }
    return jumpFallString;
}

void *jumpOnWater(void *ptr)
{
    // Allow jumping on water if the Bridge Skip display is active
    if (Displays[BRIDGE_SKIP])
    {
        return nullptr;
    }
    else
    {
        // Return the original value
        return ptr;
    }
}

void displayTitleScreenAndFileSelectScreenInfo()
{
    if (checkForSpecificSeq(ttyd::seqdrv::SeqIndex::kTitle))
    {
        // Check to see if the title screen is ready
        // Check to see if the curtain is fully displayed or not
        uint32_t TitleMainCheck = *reinterpret_cast<uint32_t *>(
            reinterpret_cast<uint32_t>(ttyd::seq_title::seqTitleWorkPointer2) + 0x8);
        
        if ((TitleMainCheck >= 2) && (TitleMainCheck < 12))
        {
            // Curtain is not fully displayed
            // Draw the title screen info
            drawFunctionOn2DLayerWithOrder(drawTitleScreenInfo, -1.f);
        }
    }
    else if (checkForSpecificSeq(ttyd::seqdrv::SeqIndex::kLoad))
    {
        // Check to see if the curtain is down or not
        uint32_t SeqMainCheck = ttyd::seqdrv::seqWork.wFileSelectScreenProgress;
        
        if (SeqMainCheck == 2)
        {
            // Draw the file select screen info
            drawFunctionOn2DLayerWithOrder(drawFileSelectScreenInfo, -1.f);
        }
    }
}

int32_t Mod::pauseMenuPreventUnpause(void *pauseMenuPointer)
{
    // Prevent unpausing if L is being held
    uint32_t OpenMenuCombo = PAD_L | PAD_START;
    
    if (checkButtonComboEveryFrame(OpenMenuCombo))
    {
        // Prevent being able to close the menu if L is being held
        return 0;
    }
    else
    {
        // Call original function
        return mPFN_winRootMain_trampoline(pauseMenuPointer);
    }
}

bool Mod::performPreBattleActions()
{
    // Make sure the Jump and Hammer upgrades have been properly checked
    recheckJumpAndHammerLevels();
    
    // Clear the cache for Mario if his stats were changed manually
    if (ClearCacheForBattles.MarioStatsShouldBeCleared)
    {
        ClearCacheForBattles.MarioStatsShouldBeCleared = false;
        
        uint32_t PouchPtr = reinterpret_cast<uint32_t>(ttyd::mario_pouch::pouchGetPtr());
        void *AddressesToClearStart = reinterpret_cast<void *>(PouchPtr + 0x70);
        gc::OSCache::DCFlushRange(AddressesToClearStart, 0x21);
    }
    
    // Clear the cache for the partners if their stats were changed manually
    if (ClearCacheForBattles.PartnerStatsShouldBeCleared)
    {
        ClearCacheForBattles.PartnerStatsShouldBeCleared = false;
        
        uint32_t PouchPtr = reinterpret_cast<uint32_t>(ttyd::mario_pouch::pouchGetPtr());
        void *AddressesToClearStart = reinterpret_cast<void *>(PouchPtr + 0xE);
        gc::OSCache::DCFlushRange(AddressesToClearStart, 0x61);
    }
    
    // Call original function
    return mPFN_battle_init_trampoline();
}

const char *Mod::getCustomMessage(const char *key)
{
    char *tempNextArea = ttyd::seq_mapchange::NextArea;
    if (compareStrings(key, "stg6_rsh_diary_01"))
    {
        // Change the text asking if you want to read the diary
        // Only needs to change when not on the train
        if (!compareStrings(tempNextArea, "rsh"))
        {
            const char *message = "<system>\n@ me if you think this mod\nsucks, but <shake><col EC2013FF>don't</col></shake><col DCC8DCFF> mention\nwhy you're @ing me.</col>\n<o>";
            
            return message;
        }
    }
    else if (compareStrings(key, "stg6_rsh_diary_01_yn"))
    {
        // Change the yes/no text answers for the diary
        // Only needs to change when not on the train
        if (!compareStrings(tempNextArea, "rsh"))
        {
#ifdef TTYD_JP
            const char *message = "<select 0 0 300 40>No\nAlright\nChoose this option";
#else
            const char *message = "<select 0 0 0 40>No\nAlright\nChoose this option";
#endif
            
            return message;
        }
    }

    // Call original function
    return mPFN_msgSearch_trampoline(key);
}

uint32_t Mod::pauseArtAttackTimer()
{
    if (!Displays[ART_ATTACK_HITBOXES])
    {
        // Call original function
        return mPFN_scissor_timer_main_trampoline();
    }
    
    // Prevent the function from running if Y is being held
    if (checkButtonComboEveryFrame(PAD_Y))
    {
        // Make sure the timer is not at 59 or lower, as pausing it then will cause issues
        void *ArtAttackPointer = ttyd::sac_scissor::GetScissorPtr();
        
        int32_t ArtAttackTimer = *reinterpret_cast<int32_t *>(
            reinterpret_cast<uint32_t>(ArtAttackPointer) + 0x104);
        
        if (ArtAttackTimer > 59)
        {
            return 0;
        }
    }
    
    // Call original function
    return mPFN_scissor_timer_main_trampoline();
}

int32_t Mod::fixMarioKeyOn()
{
    ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
    int8_t MarioKey = player->wKey;
    
    // Make sure the value is greater than 0
    if (MarioKey < 1)
    {
        player->wKey = 1;
    }
    
    // Call original function
    return mPFN_marioKeyOn_trampoline();
}

bool Mod::performRelPatches(gc::OSModule::OSModuleInfo *newModule, void *bss)
{
    // Call the original function immediately, as the REL file should be linked before applying patches
    const bool Result = mPFN_OSLink_trampoline(newModule, bss);
    
    // Make sure the linking process succeeded
    if (!Result)
    {
       return Result;
    }
    
    // Make sure a REL file is currently loaded
    gc::OSModule::OSModuleInfo *CurrentRelModuleInfo = getCurrentRelModuleInfo();
    if (!CurrentRelModuleInfo)
    {
        return Result;
    }
    
#ifdef TTYD_US
    const uint32_t LAS = 0x10;
    const uint32_t SQ_Cutscene_Spawn_Textboxes_Script_Offset = 0x36FBC;
    const uint32_t SQ_Cutscene_Spawn_Textboxes_Script_Loop_Offset = 0x64;
    const uint32_t SQ_Cutscene_Spawn_Textboxes_Script_WaitMS_Offset = 0x114;
#elif defined TTYD_JP
    const uint32_t LAS = 0x11;
    const uint32_t SQ_Cutscene_Spawn_Textboxes_Script_Offset = 0x36FC0;
    const uint32_t SQ_Cutscene_Spawn_Textboxes_Script_Loop_Offset = 0x58;
    const uint32_t SQ_Cutscene_Spawn_Textboxes_Script_WaitMS_Offset = 0xD4;
#elif defined TTYD_EU
    const uint32_t LAS = 0x11;
    const uint32_t SQ_Cutscene_Spawn_Textboxes_Script_Offset = 0x36FBC;
    const uint32_t SQ_Cutscene_Spawn_Textboxes_Script_Loop_Offset = 0x64;
    const uint32_t SQ_Cutscene_Spawn_Textboxes_Script_WaitMS_Offset = 0x114;
#endif
    
    uint32_t CurrentRelModuleInfoRaw = reinterpret_cast<uint32_t>(CurrentRelModuleInfo);
    switch (CurrentRelModuleInfo->id)
    {
        case LAS:
        {
            if (compareStringToNextMap("las_29"))
            {
                // Make changes to the function that spawns the textboxes, to prevent the standard heap from being corrupted
                // Change the loop count from 10 to 5
                *reinterpret_cast<uint32_t *>(
                    CurrentRelModuleInfoRaw + 
                        SQ_Cutscene_Spawn_Textboxes_Script_Offset + 
                            SQ_Cutscene_Spawn_Textboxes_Script_Loop_Offset) = 5;
                
                // Wait for 400ms instead of 200ms at the end of the loop
                *reinterpret_cast<uint32_t *>(
                    CurrentRelModuleInfoRaw + 
                        SQ_Cutscene_Spawn_Textboxes_Script_Offset + 
                            SQ_Cutscene_Spawn_Textboxes_Script_WaitMS_Offset) = 400;
            }
            return Result;
        }
        default:
        {
            return Result;
        }
    }
}

void clearHeapCorruptionBuffer()
{
    clearMemory(HeapInfo.HeapCorruptionBuffer, sizeof(HeapInfo.HeapCorruptionBuffer));
}

void clearMemoryUsageBuffer()
{
    int32_t ArrayCount = HeapInfo.ArrayCount;
    
    // Subtract 1 for not displaying the free portion of the smart heap
    int32_t MemoryUsageArrays = (ArrayCount * 2) - 1;
    clearMemory(HeapInfo.MemoryUsageBuffer, MemoryUsageArrays * MEMORY_USAGE_LINE_BUFFER_SIZE);
}

void addTextToHeapCorruptionBuffer(char *text)
{
    char *tempHeapCorruptionBuffer = HeapInfo.HeapCorruptionBuffer;
    
    // Make sure adding the new text will not result in an overflow
    uint32_t NewTextSize = strlen(text);
    uint32_t CurrentHeapBufferSize = strlen(tempHeapCorruptionBuffer);
    
    uint32_t NewHeapBufferSize = CurrentHeapBufferSize + NewTextSize + 1;
    uint32_t MaxHeapBufferSize = sizeof(HeapInfo.HeapCorruptionBuffer);
    
    if (NewHeapBufferSize > MaxHeapBufferSize)
    {
        // Adding the new text will result in an overflow, so don't add it
        return;
    }
    
    // Add the new text onto the heap
    strcat(tempHeapCorruptionBuffer, text);
}

void *checkIndividualStandardHeap(gc::OSAlloc::ChunkInfo *start)
{
    gc::OSAlloc::ChunkInfo *currentChunk = nullptr;
    gc::OSAlloc::ChunkInfo *prevChunk = nullptr;
    
    for (currentChunk = start; currentChunk; currentChunk = currentChunk->next)
    {
        // Check pointer sanity
        if (!checkIfPointerIsValid(currentChunk))
        {
            return currentChunk;
        }
        
        // Sanity check size
        if (currentChunk->size >= 0x1800000)
        {
            return currentChunk;
        }

        // Check linked list integrity
        if (prevChunk != currentChunk->prev)
        {
            return currentChunk;
        }

        prevChunk = currentChunk;
    }
    
    return nullptr;
}

void *checkIndividualSmartHeap(ttyd::memory::SmartAllocationData *start)
{
    ttyd::memory::SmartAllocationData *currentChunk = nullptr;
    ttyd::memory::SmartAllocationData *prevChunk = nullptr;
    
    for (currentChunk = start; currentChunk; currentChunk = currentChunk->pNext)
    {
        // Check pointer sanity
        if (!checkIfPointerIsValid(currentChunk))
        {
            return currentChunk;
        }
        
        // Sanity check size
        if (currentChunk->usedSize >= 0x1800000)
        {
            return currentChunk;
        }

        // Check linked list integrity
        if (prevChunk != currentChunk->pPrev)
        {
            return currentChunk;
        }
        
        prevChunk = currentChunk;
    }
    
    return nullptr;
}

void handleStandardHeapChunkResults(void *addressWithError, 
    gc::OSAlloc::ChunkInfo *chunk, const gc::OSAlloc::HeapInfo *heap, 
    int32_t heapIndex, uint32_t memoryUsageBufferIndex, bool isUsedPortion)
{
    // Get the used or free text
    const char *String;
    
    if (isUsedPortion)
    {
        String = "used";
    }
    else
    {
        String = "free";
    }
    
    char *tempDisplayBuffer = DisplayBuffer;
    if (addressWithError)
    {
        // Error occured, so add error text to the corruption buffer
        sprintf(tempDisplayBuffer,
            "Main Heap %" PRId32 " (%s) corrupt at 0x%08" PRIX32 "\n",
            heapIndex,
            String,
            reinterpret_cast<uint32_t>(addressWithError));
        
        // Add the text to the heap corruption buffer
        addTextToHeapCorruptionBuffer(tempDisplayBuffer);
    }
    else
    {
        // Add info about the memory usage to the memory usage buffer
        int32_t Usage = 0;
        int32_t Chunks = 0;
        
        for (gc::OSAlloc::ChunkInfo *tempChunk = chunk; 
            tempChunk; tempChunk = tempChunk->next)
        {
            Usage += tempChunk->size;
            Chunks++;
        }
        
        sprintf(tempDisplayBuffer,
            "Main Heap %" PRId32 " (%s): %.2f/%.2fkb, %" PRId32 " cks",
            heapIndex,
            String,
            static_cast<float>(Usage) / 1024.f,
            static_cast<float>(heap->capacity) / 1024.f,
            Chunks);
        
        // Add the text to the memory usage buffer
        strncpy(&HeapInfo.MemoryUsageBuffer[memoryUsageBufferIndex * MEMORY_USAGE_LINE_BUFFER_SIZE], 
            tempDisplayBuffer, MEMORY_USAGE_LINE_BUFFER_SIZE - 1);
    }
}

void handleSmartHeapChunkResults(void *addressWithError, 
    ttyd::memory::SmartAllocationData *chunk, 
    int32_t memoryUsageBufferIndex, bool isUsedPortion)
{
    char *tempDisplayBuffer = DisplayBuffer;
    if (addressWithError)
    {
        // Get the used or free text
        const char *String;
        
        if (isUsedPortion)
        {
            String = "used";
        }
        else
        {
            String = "free";
        }
        
        sprintf(tempDisplayBuffer,
            "Smart Heap (%s) corrupt at 0x%08" PRIX32 "\n",
            String,
            reinterpret_cast<uint32_t>(addressWithError));
        
        // Add the text to the heap corruption buffer
        addTextToHeapCorruptionBuffer(tempDisplayBuffer);
    }
    else if (isUsedPortion) // Don't draw the free portion
    {
        // Add info about the memory usage to the memory usage buffer
        int32_t Usage = 0;
        int32_t Chunks = 0;
        
        for (ttyd::memory::SmartAllocationData *tempChunk = chunk; 
            tempChunk; tempChunk = tempChunk->pNext)
        {
            Usage += tempChunk->usedSize;
            Chunks++;
        }
        
#ifdef TTYD_JP
        uint32_t HeapEnd = reinterpret_cast<uint32_t>(ttyd::memory::heapEnd.pHeap5End);
        uint32_t HeapStart = reinterpret_cast<uint32_t>(ttyd::memory::heapStart.pHeap5Start);
#else
        uint32_t HeapEnd = reinterpret_cast<uint32_t>(ttyd::memory::heapEnd.pHeap4End);
        uint32_t HeapStart = reinterpret_cast<uint32_t>(ttyd::memory::heapStart.pHeap4Start);
#endif
        
        uint32_t TotalSize = HeapEnd - HeapStart - 0x20;
        sprintf(tempDisplayBuffer,
            "Smart Heap (used): %.2f/%.2fkb, %" PRId32 " cks",
            static_cast<float>(Usage) / 1024.f,
            static_cast<float>(TotalSize) / 1024.f,
            Chunks);
        
        // Add the text to the memory usage buffer
        strncpy(&HeapInfo.MemoryUsageBuffer[memoryUsageBufferIndex * MEMORY_USAGE_LINE_BUFFER_SIZE], 
            tempDisplayBuffer, MEMORY_USAGE_LINE_BUFFER_SIZE - 1);
    }
}

void checkHeaps()
{
    // Clear the heap buffers to be safe
    clearHeapCorruptionBuffer();
    clearMemoryUsageBuffer();
    
    void *AddressWithError;
    uint32_t MemoryUsageCounter = 0;
    
    // Check the standard heaps
    gc::OSAlloc::HeapInfo *HeapArray = gc::OSAlloc::HeapArray;
    int32_t NumHeaps = gc::OSAlloc::NumHeaps;
    
    for (int32_t i = 0; i < NumHeaps; i++)
    {
        const gc::OSAlloc::HeapInfo *heap = &HeapArray[i];
        
        // Check the used entries
        gc::OSAlloc::ChunkInfo *tempChunk = heap->firstUsed;
        AddressWithError = checkIndividualStandardHeap(tempChunk);
        
        handleStandardHeapChunkResults(AddressWithError, 
            tempChunk, heap, i, MemoryUsageCounter++, true);
        
        // Check the free entries
        tempChunk = heap->firstFree;
        AddressWithError = checkIndividualStandardHeap(tempChunk);
        
        handleStandardHeapChunkResults(AddressWithError, 
            tempChunk, heap, i, MemoryUsageCounter++, false);
    }
    
    // Check the smart heap
    ttyd::memory::SmartWork *SmartWorkPtr = ttyd::memory::smartWorkPointer;
    
    // Check the used entries
    ttyd::memory::SmartAllocationData *tempChunk = SmartWorkPtr->pFirstUsed;
    AddressWithError = checkIndividualSmartHeap(tempChunk);
    handleSmartHeapChunkResults(AddressWithError, tempChunk, MemoryUsageCounter++, true);
    
    // Check the free entries
    tempChunk = SmartWorkPtr->pFirstFree;
    AddressWithError = checkIndividualSmartHeap(tempChunk);
    handleSmartHeapChunkResults(AddressWithError, tempChunk, MemoryUsageCounter, false);
    
    // Draw any errors that occured
    if (HeapInfo.HeapCorruptionBuffer[0] != '\0')
    {
        drawFunctionOnDebugLayerWithOrder(drawHeapArrayErrors, 100.f);
    }
    
    // Draw the memory usage details
    // Make sure at least one heap is being drawn
    bool *DisplayHeapInfo = HeapInfo.DisplayHeapInfo;
    for (int32_t i = 0; i <= NumHeaps; i++)
    {
        if (DisplayHeapInfo[i])
        {
            drawFunctionOnDebugLayerWithOrder(drawMemoryUsage, 5.f);
            break;
        }
    }
}

ttyd::npcdrv::NpcEntry *Mod::checkForNpcNameToPtrError(const char *name)
{
    // Call the original function immediately
    ttyd::npcdrv::NpcEntry *NPC = mPFN_npcNameToPtr_trampoline(name);
    
    // Check if the returned pointer is valid
    ttyd::npcdrv::NpcWork *NpcWorkPointer = ttyd::npcdrv::npcGetWorkPtr();
    if (NPC == &NpcWorkPointer->entries[NpcWorkPointer->npcMaxCount])
    {
        // Didn't find the correct NPC, so print error text
        NpcNameToPtrError.Timer = secondsToFrames(5);
        NpcNameToPtrError.Counter++;
    }
    
    return NPC;
}

void displayNpcNameToPtrError()
{
    uint32_t Timer = NpcNameToPtrError.Timer;
    if (Timer > 0)
    {
        Timer--;
        NpcNameToPtrError.Timer = Timer;
        drawFunctionOnDebugLayerWithOrder(drawNpcNameToPtrError, 100.f);
    }
    else
    {
        NpcNameToPtrError.Counter = 0;
    }
}

void checkForEnemyEncounters(void *ptr)
{
    if (!Displays[ENEMY_ENCOUNTER_NOTIFIER])
    {
        return;
    }
    
    if (!ptr)
    {
        return;
    }
    
    // ptr is set, so an encounter would normally occur now
    EnemyEncounterNotifier.Timer = secondsToFrames(3);
    EnemyEncounterNotifier.Counter++;
}

void displayEnemyEncounterNotifier()
{
    uint32_t Timer = EnemyEncounterNotifier.Timer;
    if (Timer > 0)
    {
        Timer--;
        EnemyEncounterNotifier.Timer = Timer;
        drawFunctionOnDebugLayer(drawEnemyEncounterNotifier);
    }
    else
    {
        EnemyEncounterNotifier.Counter = 0;
    }
}

ttyd::npcdrv::NpcEntry *Mod::fbatHitCheck_Work(uint32_t flags, void *unk)
{
    // Call the original function immediately
    ttyd::npcdrv::NpcEntry *Result = mPFN_fbatHitCheck_trampoline(flags, unk);
    
    // Check for enemy encounters
    checkForEnemyEncounters(Result);
    
    // Check if battles should be disabled
    if (disableBattles())
    {
        return nullptr;
    }
    else
    {
        return Result;
    }
}

void initAddressOverwrites()
{
#ifdef TTYD_US
    void *AllowRunningFromBattlesAddress            = reinterpret_cast<void *>(0x80123CA4);
    void *ForceNPCItemDropAddress                   = reinterpret_cast<void *>(0x8004EC10);
    void *DebugModeInitialzeAddress                 = reinterpret_cast<void *>(0x80009B2C);
    void *DebugModeShowBuildDateAddress             = reinterpret_cast<void *>(0x80008FE8);
    void *PauseMenuPartnerMenuAddress               = reinterpret_cast<void *>(0x801649A0);
    void *PauseMenuBadgeMenuAddress                 = reinterpret_cast<void *>(0x80164A44);
    void *ReplaceJumpAnimAddress                    = reinterpret_cast<void *>(0x800411D0);
    void *PreventImportantItemCutscenesAddress      = reinterpret_cast<void *>(0x800ABCD8);
    void *msgWindowMrAddress                        = reinterpret_cast<void *>(0x800816F4);
    void *DisplayBattleMenuJumpAddress              = reinterpret_cast<void *>(0x80122BA4);
    void *DisplayBattleMenuHammerAddress            = reinterpret_cast<void *>(0x80122BB8);
    void *FixBlooperCrash1Address                   = reinterpret_cast<void *>(0x8010F810);
    void *FixBlooperCrash2Address                   = reinterpret_cast<void *>(0x8010F888);
    void *PreventTextboxSelectionAddress            = reinterpret_cast<void *>(0x800D214C);
    void *BacktraceScreenFontSizeAddress            = reinterpret_cast<void *>(0x80428BC0);
    void *BacktraceScreenPPCHaltBranchAddress       = reinterpret_cast<void *>(0x8025E4A4);
    void *BacktraceScreenEndBranchAddress           = reinterpret_cast<void *>(0x8025E4A8);
    void *DisableDPadOptionsDisplayAddress          = reinterpret_cast<void *>(0x8013D148);
    void *FixEvtMapBlendSetFlagPartnerCrashAddress  = reinterpret_cast<void *>(0x800389C4);
    void *FixEvtMapBlendSetFlagFollowerCrashAddress = reinterpret_cast<void *>(0x80038A0C);
    void *PreventPouchInitMemoryLeakAddress         = reinterpret_cast<void *>(0x800D59DC);
    void *InitStageEventsAddress                    = reinterpret_cast<void *>(0x800080E4);
    void *FallThroughMostObjectsStandAddress        = reinterpret_cast<void *>(0x8008E9DC);
    void *FallThroughMostObjectsTubeAddress         = reinterpret_cast<void *>(0x8008E1E8);
    void *FallThroughMostObjectsBowserAddress       = reinterpret_cast<void *>(0x8021A30C);
    void *AutoMashThroughText1Address               = reinterpret_cast<void *>(0x80080FCC);
    void *AutoMashThroughText2Address               = reinterpret_cast<void *>(0x80080FF0);
    void *AutoMashThroughText3Address               = reinterpret_cast<void *>(0x80084268);
#elif defined TTYD_JP
    void *AllowRunningFromBattlesAddress            = reinterpret_cast<void *>(0x8011E7DC);
    void *ForceNPCItemDropAddress                   = reinterpret_cast<void *>(0x8004DFB0);
    void *DebugModeInitialzeAddress                 = reinterpret_cast<void *>(0x8000999C);
    void *DebugModeShowBuildDateAddress             = reinterpret_cast<void *>(0x80008EB8);
    void *PauseMenuPartnerMenuAddress               = reinterpret_cast<void *>(0x8015EFBC);
    void *PauseMenuBadgeMenuAddress                 = reinterpret_cast<void *>(0x8015F060);
    void *ReplaceJumpAnimAddress                    = reinterpret_cast<void *>(0x80040B34);
    void *PreventImportantItemCutscenesAddress      = reinterpret_cast<void *>(0x800AA01C);
    void *msgWindowMrAddress                        = reinterpret_cast<void *>(0x80080B6C);
    void *DisplayBattleMenuJumpAddress              = reinterpret_cast<void *>(0x8011D6DC);
    void *DisplayBattleMenuHammerAddress            = reinterpret_cast<void *>(0x8011D6F0);
    void *FixBlooperCrash1Address                   = reinterpret_cast<void *>(0x8010A724);
    void *FixBlooperCrash2Address                   = reinterpret_cast<void *>(0x8010A79C);
    void *PreventTextboxSelectionAddress            = reinterpret_cast<void *>(0x800CE01C);
    void *BacktraceScreenFontSizeAddress            = reinterpret_cast<void *>(0x80422618);
    void *DisableDPadOptionsDisplayAddress          = reinterpret_cast<void *>(0x80137C1C);
    void *FixEvtMapBlendSetFlagPartnerCrashAddress  = reinterpret_cast<void *>(0x80038328);
    void *FixEvtMapBlendSetFlagFollowerCrashAddress = reinterpret_cast<void *>(0x80038370);
    void *PreventPouchInitMemoryLeakAddress         = reinterpret_cast<void *>(0x800D17A8);
    void *InitStageEventsAddress                    = reinterpret_cast<void *>(0x80008054);
    void *FallThroughMostObjectsStandAddress        = reinterpret_cast<void *>(0x8008D428);
    void *FallThroughMostObjectsTubeAddress         = reinterpret_cast<void *>(0x8008CC4C);
    void *FallThroughMostObjectsBowserAddress       = reinterpret_cast<void *>(0x80215668);
    void *AutoMashThroughText1Address               = reinterpret_cast<void *>(0x8008047C);
    void *AutoMashThroughText2Address               = reinterpret_cast<void *>(0x800804A0);
    void *AutoMashThroughText3Address               = reinterpret_cast<void *>(0x80083390);
#elif defined TTYD_EU
    void *AllowRunningFromBattlesAddress            = reinterpret_cast<void *>(0x80124BE4);
    void *ForceNPCItemDropAddress                   = reinterpret_cast<void *>(0x8004ECDC);
    void *DebugModeInitialzeAddress                 = reinterpret_cast<void *>(0x80009CF0);
    void *DebugModeShowBuildDateAddress             = reinterpret_cast<void *>(0x800091B4);
    void *PauseMenuPartnerMenuAddress               = reinterpret_cast<void *>(0x80166490);
    void *PauseMenuBadgeMenuAddress                 = reinterpret_cast<void *>(0x80166534);
    void *ReplaceJumpAnimAddress                    = reinterpret_cast<void *>(0x800412B8);
    void *PreventImportantItemCutscenesAddress      = reinterpret_cast<void *>(0x800AD0A8);
    void *msgWindowMrAddress                        = reinterpret_cast<void *>(0x800829b0);
    void *DisplayBattleMenuJumpAddress              = reinterpret_cast<void *>(0x80123AE4);
    void *DisplayBattleMenuHammerAddress            = reinterpret_cast<void *>(0x80123AF8);
    void *FixBlooperCrash1Address                   = reinterpret_cast<void *>(0x801106E8);
    void *FixBlooperCrash2Address                   = reinterpret_cast<void *>(0x80110760);
    void *PreventTextboxSelectionAddress            = reinterpret_cast<void *>(0x800D2F44);
    void *BacktraceScreenFontSizeAddress            = reinterpret_cast<void *>(0x804356C8);
    void *BacktraceScreenPPCHaltBranchAddress       = reinterpret_cast<void *>(0x8026207C);
    void *BacktraceScreenEndBranchAddress           = reinterpret_cast<void *>(0x80262080);
    void *DisableDPadOptionsDisplayAddress          = reinterpret_cast<void *>(0x8013EC30);
    void *FixEvtMapBlendSetFlagPartnerCrashAddress  = reinterpret_cast<void *>(0x80038AAC);
    void *FixEvtMapBlendSetFlagFollowerCrashAddress = reinterpret_cast<void *>(0x80038AF4);
    void *PreventPouchInitMemoryLeakAddress         = reinterpret_cast<void *>(0x800D67E4);
    void *InitStageEventsAddress                    = reinterpret_cast<void *>(0x800082BC);
    void *FallThroughMostObjectsStandAddress        = reinterpret_cast<void *>(0x8008FD38);
    void *FallThroughMostObjectsTubeAddress         = reinterpret_cast<void *>(0x8008F544);
    void *FallThroughMostObjectsBowserAddress       = reinterpret_cast<void *>(0x8021DD9C);
    void *AutoMashThroughText1Address               = reinterpret_cast<void *>(0x80082288);
    void *AutoMashThroughText2Address               = reinterpret_cast<void *>(0x800822AC);
    void *AutoMashThroughText3Address               = reinterpret_cast<void *>(0x800855BC);
    void *JumpOnWaterAddress                        = reinterpret_cast<void *>(0x80093CF0);
#endif
    
    patch::writeBranchBL_Template(AllowRunningFromBattlesAddress, StartAllowRunningFromBattles);
    
    patch::writeBranchBL_Template(ForceNPCItemDropAddress, StartForceNPCItemDrop);
    
    patch::writeBranchBL_Template(ReplaceJumpAnimAddress, StartReplaceJumpFallAnim);
    
    patch::writeBranchBL_Template(DisplayBattleMenuJumpAddress, StartDisplayMegaJumpBadgeBattleMenu);
    
    patch::writeBranchBL_Template(DisplayBattleMenuHammerAddress, StartDisplayMegaHammerBadgesBattleMenu);
    
    patch::writeBranchBL_Template(FixBlooperCrash1Address, StartFixBlooperCrash1);
    
    patch::writeBranchBL_Template(FixBlooperCrash2Address, StartFixBlooperCrash2);
    
    patch::writeBranchBL_Template(PreventTextboxSelectionAddress, StartPreventTextboxSelection);
    
    patch::writeBranchBL_Template(DisableDPadOptionsDisplayAddress, StartDisableDPadOptionsDisplay);
    
    patch::writeBranchBL_Template(FixEvtMapBlendSetFlagPartnerCrashAddress, StartFixEvtMapBlendSetFlagPartnerCrash);
    
    patch::writeBranchBL_Template(FixEvtMapBlendSetFlagFollowerCrashAddress, StartFixEvtMapBlendSetFlagFollowerCrash);
    
    patch::writeBranchBL_Template(PreventPouchInitMemoryLeakAddress, preventPouchInitMemoryLeak);
    
    patch::writeBranchBL_Template(InitStageEventsAddress, initStageEvents);
    
    patch::writeBranchBL_Template(FallThroughMostObjectsStandAddress, StartFallThroughMostObjectsStandard);
    patch::writeBranchBL_Template(FallThroughMostObjectsTubeAddress, StartFallThroughMostObjectsStandard);
    
    patch::writeBranchBL_Template(FallThroughMostObjectsBowserAddress, StartFallThroughMostObjectsBowser);
    
    patch::writeBranchBL_Template(AutoMashThroughText1Address, autoMashText);
    patch::writeBranchBL_Template(AutoMashThroughText2Address, autoMashText);
    patch::writeBranchBL_Template(AutoMashThroughText3Address, autoMashText);
    
#ifdef TTYD_EU
    patch::writeBranchBL_Template(JumpOnWaterAddress, StartJumpOnWater);
#endif
    
    *reinterpret_cast<uint32_t *>(DebugModeInitialzeAddress)            = 0x3800FFFF; // li r0,-1
    *reinterpret_cast<uint32_t *>(DebugModeShowBuildDateAddress)        = 0x60000000; // nop
    
    *reinterpret_cast<uint32_t *>(PauseMenuPartnerMenuAddress)          = 0x60000000; // nop
    *reinterpret_cast<uint32_t *>(PauseMenuBadgeMenuAddress)            = 0x60000000; // nop
    
    *reinterpret_cast<uint32_t *>(PreventImportantItemCutscenesAddress) = 0x48000030; // b 0x30
    
    *reinterpret_cast<uint32_t *>(msgWindowMrAddress)                   = 0x38830001; // addi r4,r3,1
    
    *reinterpret_cast<float *>(BacktraceScreenFontSizeAddress)          = 0.66f;
    
#ifndef TTYD_JP
    // This part of the backtrace screen does not need to be modified in JP
    *reinterpret_cast<uint32_t *>(BacktraceScreenPPCHaltBranchAddress)  = 0x3B400000; // li r26,0
    *reinterpret_cast<uint32_t *>(BacktraceScreenEndBranchAddress)      = 0x4BFFFDD4; // b -0x22C
#endif
    
    // Set the initial value for the debug mode variable
    *reinterpret_cast<int32_t *>(
        reinterpret_cast<uint32_t>(
            ttyd::seq_title::seqTitleWorkPointer2) + 0x30) = -1;
}

void Mod::run()
{
    // Display the title screen and file select screen info
    displayTitleScreenAndFileSelectScreenInfo();
    
    // Update the On-Screen Timer variables if the display is on
    updateOnScreenTimerVars();
    
    // Check if the menu should be enabled or disabled
    // Prevent checking it if currently in the process of spawning an item
    // Prevent checking it if the memory editor is open
    if (!SpawnItem.InAdjustableValueMenu && 
        !MemoryEditor.EditorCurrentlyDisplayed)
    {
        enableOrDisableMenu();
    }
    
    // Run each cheat function
    // Only run button-based codes if currently not changing button combos
    bool tempChangingCheatButtonCombo = MenuVar.ChangingCheatButtonCombo;
    if (!tempChangingCheatButtonCombo)
    {
        saveMarioAndPartnerPositions();
        loadMarioAndPartnerPositions();
        setTextStorage();
        setTimeStopTextStorage();
        lockMarioHPToMax();
        levitate();
        generateLagSpike();
    }
    
    // Run each cheat function that isn't button-based
    walkThroughMostObjects(); // Needs to always run due to a variable constantly being checked
    saveAnywhere(); // Needs to always run due to the script taking more than one frame
    speedUpMario(); // Needs to always run due to Mario's base speed constantly being set
    reloadRoom(); // Needs to always run due to the extra code that always does some failsafe checking
    bobberyEarly();
    checkIfAreaFlagsShouldBeCleared();
    checkIfSystemLevelShouldBeLowered();
    lockFlags();
    
    // Run each display function
    // Only run if the mod menu isn't currently open
    // Run if currently repositioning watches
    if (!MenuVar.MenuIsDisplayed || MenuVar.HideMenu)
    {
        // Only run button-based displays if currently not changing button combos
        if (!tempChangingCheatButtonCombo)
        {
            displayYoshiSkipDetails();
            displayPalaceSkipDetails();
            displayBridgeSkipDetails();
            displayBlimpTicketSkipDetails();
            displayOnScreenTimer();
            displayFrameCounter();
            displayJumpStorageDetails();
            displayButtonInputs();
        }
        
        // Run each display function that isn't button-based
        displayMarioCoordinates();
        displayMarioSpeedXZ();
        displayStickAngle();
        displayMemoryWatches();
        displayNpcNameToPtrError();
        spawnItem(); // Needs to always run due to the adjustable value menu sometimes being displayed
        displayMemoryEditor(); // Needs to always run due to the adjustable value menu sometimes being displayed
        displaySequenceInPauseMenu();
        displayActionCommandsTiming();
        displayEffsActive();
        displayEvtsActive();
        displayEnemyEncounterNotifier();
        
        // Check the heaps
        checkHeaps();
    }
    
    uint32_t tempFrameCounter = MenuVar.FrameCounter;
    if (tempFrameCounter > 0)
    {
        MenuVar.FrameCounter = tempFrameCounter - 1;
    }
    
    // Call original function
    mPFN_marioStMain_trampoline();
}

}