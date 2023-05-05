#include "cxx.h"
#include "gc/OSCache.h"
#include "ttyd/memory.h"
#include "misc/heap.h"
#include "misc/utils.h"

#include <cstdint>
#include <cstring>

void *allocateMemoryFromTail(std::size_t size)
{
    void *ptr = allocFromHeapTail(0, size);
    ptr = clearMemory(ptr, size);
    DCFlushRange(ptr, size);
    return ptr;
}

void *allocateMemory(uint32_t size, bool allocFromHead)
{
    if (allocFromHead)
    {
        return __memAlloc(0, size);
    }
    else
    {
        return allocateMemoryFromTail(size);
    }
}

void freeMemory(void *ptr)
{
    return __memFree(0, ptr);
}

void *operator new(std::size_t size)
{
    return allocateMemoryFromTail(size);
}

void *operator new[](std::size_t size)
{
    return allocateMemoryFromTail(size);
}

void *operator new(std::size_t size, bool allocFromHead)
{
    return allocateMemory(size, allocFromHead);
}

void *operator new[](std::size_t size, bool allocFromHead)
{
    return allocateMemory(size, allocFromHead);
}

void operator delete(void *ptr)
{
    return freeMemory(ptr);
}

void operator delete[](void *ptr)
{
    return freeMemory(ptr);
}

void operator delete(void *ptr, std::size_t size)
{
    (void)size;
    return freeMemory(ptr);
}

void operator delete[](void *ptr, std::size_t size)
{
    (void)size;
    return freeMemory(ptr);
}
