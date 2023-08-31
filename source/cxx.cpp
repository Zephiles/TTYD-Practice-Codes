#include "cxx.h"
#include "gc/OSCache.h"
#include "misc/heap.h"
#include "misc/utils.h"
#include "ttyd/memory.h"

#include <cstdint>
#include <cstring>

void *allocateMemoryFromTail(std::size_t size)
{
    void *ptr = allocFromHeapTail(heapHandle[0], size);
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

void freeMemory(void *ptr)
{
    return __memFree(0, ptr);
}
