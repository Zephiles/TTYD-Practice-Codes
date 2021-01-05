#include "cxx.h"

#include <ttyd/memory.h>

#include <cstddef>

void *operator new(std::size_t size)
{
    return ttyd::memory::__memAlloc(HEAP_ID, size);
}
void *operator new[](std::size_t size)
{
    return ttyd::memory::__memAlloc(HEAP_ID, size);
}
void operator delete(void *ptr)
{
    return ttyd::memory::__memFree(HEAP_ID, ptr);
}
void operator delete[](void *ptr)
{
    return ttyd::memory::__memFree(HEAP_ID, ptr);
}
void operator delete(void *ptr, std::size_t size)
{
    return ttyd::memory::__memFree(HEAP_ID, ptr);
}
void operator delete[](void *ptr, std::size_t size)
{
    return ttyd::memory::__memFree(HEAP_ID, ptr);
}
