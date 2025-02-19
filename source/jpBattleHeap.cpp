#ifdef TTYD_JP
#include "jpBattleHeap.h"
#include "patch.h"
#include "gc/OSAlloc.h"
#include "gc/types.h"
#include "ttyd/memory.h"
#include "ttyd/battle.h"

#include <cstdint>

static ChunkInfo *gBattleHeap = nullptr;

static void (*g_BattleEnd_trampoline)() = nullptr;
static OSHeapHandle (*g_OSCreateHeap_trampoline)(void *start, void *end) = nullptr;

void battleHeapInit()
{
    // Allocate the memory from the map heap
    // Turn the allocated memory into a standard heap
    // Cannot use OSCreateHeap since this heap needs to be handled separately from the other game heaps
    ChunkInfo *chunkPtr = reinterpret_cast<ChunkInfo *>(_mapAlloc(BATTLE_HEAP_ORIGINAL_SIZE));
    gBattleHeap = chunkPtr;

    // Init chunk variables
    chunkPtr->prev = nullptr;
    chunkPtr->next = nullptr;
    chunkPtr->size = BATTLE_HEAP_ORIGINAL_SIZE;

    // Init heap info variables
    HeapInfo *heapInfoPtr = &HeapArray[HeapType::HEAP_BATTLE];
    heapInfoPtr->capacity = BATTLE_HEAP_ORIGINAL_SIZE;
    heapInfoPtr->firstFree = chunkPtr;
    heapInfoPtr->firstUsed = nullptr;
}

void battleEndHook()
{
    // Call the original function immediately, as various code needs to run before the battle heap is freed
    g_BattleEnd_trampoline();

    // Free the battle heap
    _mapFree(gBattleHeap);
    gBattleHeap = nullptr;

    // Reset the heap info variables since the battle heap is no longer allocated
    HeapInfo *heapInfoPtr = &HeapArray[HeapType::HEAP_BATTLE];
    heapInfoPtr->capacity = 0;
    heapInfoPtr->firstFree = nullptr;
    heapInfoPtr->firstUsed = nullptr;
}

OSHeapHandle OSCreateHeapHook(void *start, void *end)
{
    // If start and/or end are nullptr, then the game is trying to create the battle heap in memInit. To prevent this, set the
    // capacity for it to 0 to prevent it from being written to, and return HeapType::HEAP_BATTLE to make sure the proper handle
    // is written to the heapHandle global variable.
    if (!start || !end)
    {
        HeapArray[HeapType::HEAP_BATTLE].capacity = 0;
        return HeapType::HEAP_BATTLE;
    }

    // Call the original function
    return g_OSCreateHeap_trampoline(start, end);
}

void battleHeapVarsInit()
{
    // For preventing the battle heap portion of the size table from being read when the game is creating its heaps
    applyAssemblyPatch(&size_table[HeapType::HEAP_BATTLE][0], 2);

    // For giving the original battle heap's size to the main heap
    int32_t *relUsedSizePtr = reinterpret_cast<int32_t *>(REL_TOTAL_USED_SIZE_ADDRESS);
    const int32_t relUsedSize = *relUsedSizePtr - BATTLE_HEAP_ORIGINAL_SIZE;
    applyAssemblyPatch(relUsedSizePtr, static_cast<uint32_t>(relUsedSize));

    // For replacing the memClear call in battle_init to create the battle heap
    writeBranchBL(0x800726B4, battleHeapInit);

    // For preventing memClear from being called when exiting a battle
    applyAssemblyPatch(0x80072528, 0x60000000); // nop

    // For freeing the battle heap back to the map heap upon exiting a battle
    g_BattleEnd_trampoline = hookFunctionArena(BattleEnd, battleEndHook);

    // For preventing OSCreateHeap from running if either the start or end parameters are nullptr, as this would occur when
    // trying to create the battle heap
    g_OSCreateHeap_trampoline = hookFunctionArena(OSCreateHeap, OSCreateHeapHook);
}
#endif
