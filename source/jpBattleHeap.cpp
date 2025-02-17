#ifdef TTYD_JP
#include "jpBattleHeap.h"
#include "patch.h"
#include "gc/OSAlloc.h"
#include "gc/types.h"
#include "misc/heap.h"
#include "misc/functionHooks.h"
#include "ttyd/memory.h"
#include "ttyd/battle.h"

#include <cstdint>

HeapInfo *gBattleHeapInfo;
static ChunkInfo *gBattleHeap = nullptr;

static void (*g___memFree_trampoline)(int32_t heap, void *ptr) = nullptr;
static void *(*g___memAlloc_trampoline)(int32_t heap, uint32_t size) = nullptr;
static void (*g_BattleEnd_trampoline)() = nullptr;
static OSHeapHandle (*g_OSCreateHeap_trampoline)(void *start, void *end) = nullptr;

void battleHeapInit()
{
    // Allocate the memory from the map heap
    uint32_t memoryPtrRaw = reinterpret_cast<uint32_t>(_mapAlloc(BATTLE_HEAP_ORIGINAL_SIZE));

    // Turn the allocated memory into a standard heap
    // Cannot use OSCreateHeap since this heap needs to be handled separately from the other game heaps
    ChunkInfo *chunkPtr = reinterpret_cast<ChunkInfo *>(memoryPtrRaw);
    gBattleHeap = chunkPtr;

    chunkPtr->prev = nullptr;
    chunkPtr->next = nullptr;
    chunkPtr->size = BATTLE_HEAP_ORIGINAL_SIZE;

    HeapInfo *heapInfoPtr = gBattleHeapInfo;
    heapInfoPtr->capacity = BATTLE_HEAP_ORIGINAL_SIZE;
    heapInfoPtr->firstFree = chunkPtr;
    heapInfoPtr->firstUsed = nullptr;
}

void *memAllocHook(int32_t heap, uint32_t size)
{
    if (heap != HeapType::HEAP_BATTLE)
    {
        // Not allocating from the battle heap, so call the original function
        return g___memAlloc_trampoline(heap, size);
    }

    // Return OSAllocFromHeap with the handle set to BATTLE_HEAP_CUSTOM_HANDLE, as this will cause the assembly inject for it to
    // return the proper heap info pointer
    return OSAllocFromHeap(BATTLE_HEAP_CUSTOM_HANDLE, size);
}

void memFreeHook(int32_t heap, void *ptr)
{
    if (heap != HeapType::HEAP_BATTLE)
    {
        // Not freeing from the battle heap, so call the original function
        return g___memFree_trampoline(heap, ptr);
    }

    // Return OSFreeToHeap with the handle set to BATTLE_HEAP_CUSTOM_HANDLE, as this will cause the assembly inject for it to
    // return the proper heap info pointer
    return OSFreeToHeap(BATTLE_HEAP_CUSTOM_HANDLE, ptr);
}

HeapInfo *OS_getHeapArrayPtr(uint32_t heapArrayPtrRaw, OSHeapHandle handle)
{
    // Both instances of this function being used have the handle multipled by 0xC
    if (handle != (BATTLE_HEAP_CUSTOM_HANDLE * 0xC))
    {
        // Not allocating from the battle heap, so return the original value
        return reinterpret_cast<HeapInfo *>(heapArrayPtrRaw + handle);
    }

    // Set the new handle and return gBattleHeapInfo
    return gBattleHeapInfo;
}

void battleEndHook()
{
    // Call the original function immediately, as various code needs to run before the battle heap is freed
    g_BattleEnd_trampoline();

    // Free the battle heap
    _mapFree(gBattleHeap);
    gBattleHeap = nullptr;

    // Reset the variables in gBattleHeapInfo since the battle heap is no longer allocated
    HeapInfo *heapInfoPtr = gBattleHeapInfo;
    heapInfoPtr->capacity = 0;
    heapInfoPtr->firstFree = nullptr;
    heapInfoPtr->firstUsed = nullptr;
}

OSHeapHandle OSCreateHeapHook(void *start, void *end)
{
    // If start and/or end are nullptr, then assume the game was trying to create the battle heap, so exit immediately
    if (!start || !end)
    {
        return HeapType::HEAP_BATTLE;
    }

    // Call the original function
    return g_OSCreateHeap_trampoline(start, end);
}

void battleHeapVarsInit()
{
    gBattleHeapInfo = new (true, true) HeapInfo;

    // For preventing the battle heap portion of the size table from being read when the game is creating its heaps
    applyAssemblyPatch(&size_table[HeapType::HEAP_BATTLE][0], 2);

    // For giving the original battle heap's size to the main heap
    int32_t *relUsedSizePtr = reinterpret_cast<int32_t *>(REL_TOTAL_USED_SIZE_ADDRESS);
    const int32_t relUsedSize = *relUsedSizePtr - BATTLE_HEAP_ORIGINAL_SIZE;
    applyAssemblyPatch(relUsedSizePtr, static_cast<uint32_t>(relUsedSize));

    // For preventing OSCreateHeap from running if either the start or end parameters are nullptr, as this would occur when
    // trying to create the battle heap
    g_OSCreateHeap_trampoline = hookFunctionArena(OSCreateHeap, OSCreateHeapHook);

    // For handling freeing memory to the battle heap
    g___memFree_trampoline = hookFunctionArena(__memFree, memFreeHook);

    // For handling allocating memory from the battle heap
    g___memAlloc_trampoline = hookFunctionArena(__memAlloc, memAllocHook);

    // For freeing the battle heap back to the map heap upon exiting a battle
    g_BattleEnd_trampoline = hookFunctionArena(BattleEnd, battleEndHook);

    // For replacing the memClear call in battle_init to create the battle heap
    writeBranchBL(0x800726B4, battleHeapInit);

    // For preventing memClear from being called when exiting a battle
    applyAssemblyPatch(0x80072528, 0x60000000); // nop

    // For using the correct HeapInfo pointer when allocating from the battle heap
    writeStandardBranches(0x8029049C, asmOSAllocFromHeapGetHeapArrayPtrStart, asmOSAllocFromHeapGetHeapArrayPtrBranchBack);

    // For using the correct HeapInfo pointer when freeing to the battle heap
    writeBranchBL(0x802905B0, asmOSFreeToHeapGetHeapArrayPtr);
}
#endif
