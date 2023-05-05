#ifndef GC_OSALLOC_H
#define GC_OSALLOC_H

#include <cstdint>

struct ChunkInfo
{
    ChunkInfo *prev;
    ChunkInfo *next;
    uint32_t size;
} __attribute__((__packed__));

struct HeapInfo
{
    uint32_t capacity;
    ChunkInfo *firstFree;
    ChunkInfo *firstUsed;
} __attribute__((__packed__));

static_assert(sizeof(ChunkInfo) == 0xC);
static_assert(sizeof(HeapInfo) == 0xC);

extern "C"
{
    extern HeapInfo *HeapArray;
    extern int32_t NumHeaps;

    ChunkInfo *DLInsert(ChunkInfo *list, ChunkInfo *chunk);
    // OSAllocFromHeap
    // OSFreeToHeap
    // OSSetCurrentHeap
    // OSInitAlloc
    // OSCreateHeap
    // OSDestroyHeap
}

#endif
