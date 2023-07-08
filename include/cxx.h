#ifndef CXX_H
#define CXX_H

#include <cstdint>
#include <new>

void *allocateMemoryFromTail(std::size_t size);
void *allocMemoryFromHead(std::size_t size);
void freeMemory(void *ptr);

inline void *allocateMemory(uint32_t size, bool allocFromHead)
{
    if (allocFromHead)
    {
        return allocMemoryFromHead(size);
    }
    else
    {
        return allocateMemoryFromTail(size);
    }
}

inline void *operator new(std::size_t size)
{
    return allocateMemoryFromTail(size);
}

inline void *operator new[](std::size_t size)
{
    return allocateMemoryFromTail(size);
}

inline void *operator new(std::size_t size, bool allocFromHead)
{
    return allocateMemory(size, allocFromHead);
}

inline void *operator new[](std::size_t size, bool allocFromHead)
{
    return allocateMemory(size, allocFromHead);
}

inline void operator delete(void *ptr)
{
    return freeMemory(ptr);
}

inline void operator delete[](void *ptr)
{
    return freeMemory(ptr);
}

inline void operator delete(void *ptr, std::size_t size)
{
    (void)size;

    return freeMemory(ptr);
}

inline void operator delete[](void *ptr, std::size_t size)
{
    (void)size;

    return freeMemory(ptr);
}

#endif
