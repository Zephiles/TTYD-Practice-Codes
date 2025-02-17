#include "cxx.h"
#include "gc/OSCache.h"
#include "gc/OSArena.h"
#include "misc/heap.h"
#include "misc/utils.h"
#include "ttyd/memory.h"

#include <cstdint>
#include <cstring>

void *clearMemoryAndCacheAndReturnPtr(void *ptr, std::size_t size)
{
    if (ptr)
    {
        clearMemory(ptr, size);
        DCFlushRange(ptr, size);
    }
    return ptr;
}

void *allocMemoryFromHead(std::size_t size)
{
    return __memAlloc(0, size);
}

void *allocateMemoryFromTail(std::size_t size)
{
    void *ptr = allocFromHeapTail(heapHandle[0], size);
    return clearMemoryAndCacheAndReturnPtr(ptr, size);
}

void *allocMemoryFromArenaHead(std::size_t size, uint32_t alignment)
{
    void *ptr = OSAllocFromArenaLo(size, alignment);
    return clearMemoryAndCacheAndReturnPtr(ptr, size);
}

void *allocMemoryFromArenaHead(std::size_t size)
{
    return allocMemoryFromArenaHead(size, 0x4);
}

void *allocMemoryFromArenaTail(std::size_t size, uint32_t alignment)
{
    uint32_t arenaHiRaw = reinterpret_cast<uint32_t>(__OSArenaHi);

    arenaHiRaw &= ~((alignment)-1);
    arenaHiRaw -= size;
    arenaHiRaw &= ~((alignment)-1);

    void *arenaHi = reinterpret_cast<void *>(arenaHiRaw);
    __OSArenaHi = arenaHi;

    return clearMemoryAndCacheAndReturnPtr(arenaHi, size);
}

void *allocMemoryFromArenaTail(std::size_t size)
{
    return allocMemoryFromArenaTail(size, 0x4);
}

void freeMemory(void *ptr)
{
    return __memFree(0, ptr);
}
