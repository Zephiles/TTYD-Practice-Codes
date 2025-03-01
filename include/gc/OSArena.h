#ifndef GC_OSARENA_H
#define GC_OSARENA_H

#include <cstdint>

extern "C"
{
    extern void *__OSArenaLo;
    extern void *__OSArenaHi;

    void *OSAllocFromArenaLo(uint32_t size, uint32_t alignment);
}

#endif
