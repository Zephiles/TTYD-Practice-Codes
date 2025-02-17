#ifndef CXX_H
#define CXX_H

#include <cstdint>
#include <new>

void *clearMemoryAndCacheAndReturnPtr(void *ptr, std::size_t size);
void *allocMemoryFromHead(std::size_t size);
void *allocateMemoryFromTail(std::size_t size);
void *allocMemoryFromArenaHead(std::size_t size, uint32_t alignment);
void *allocMemoryFromArenaHead(std::size_t size);
void *allocMemoryFromArenaTail(std::size_t size, uint32_t alignment);
void *allocMemoryFromArenaTail(std::size_t size);
void freeMemory(void *ptr);

// `alignment` is only used when allocating from the arena
inline void *allocateMemory(uint32_t size, bool allocFromHead, bool allocFromArena, uint32_t alignment)
{
    if (allocFromArena)
    {
        if (allocFromHead)
        {
            return allocMemoryFromArenaHead(size, alignment);
        }
        else
        {
            return allocMemoryFromArenaTail(size, alignment);
        }
    }
    else if (allocFromHead)
    {
        return allocMemoryFromHead(size);
    }
    else
    {
        return allocateMemoryFromTail(size);
    }
}

inline void *allocateMemory(uint32_t size, bool allocFromHead, bool allocFromArena)
{
    if (allocFromArena)
    {
        if (allocFromHead)
        {
            return allocMemoryFromArenaHead(size);
        }
        else
        {
            return allocMemoryFromArenaTail(size);
        }
    }
    else if (allocFromHead)
    {
        return allocMemoryFromHead(size);
    }
    else
    {
        return allocateMemoryFromTail(size);
    }
}

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

inline void *operator new(std::size_t size, bool allocFromHead, bool allocFromArena)
{
    return allocateMemory(size, allocFromHead, allocFromArena);
}

inline void *operator new(std::size_t size, bool allocFromHead, bool allocFromArena, uint32_t alignment)
{
    return allocateMemory(size, allocFromHead, allocFromArena, alignment);
}

inline void *operator new[](std::size_t size, bool allocFromHead, bool allocFromArena)
{
    return allocateMemory(size, allocFromHead, allocFromArena);
}

inline void *operator new[](std::size_t size, bool allocFromHead, bool allocFromArena, uint32_t alignment)
{
    return allocateMemory(size, allocFromHead, allocFromArena, alignment);
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
