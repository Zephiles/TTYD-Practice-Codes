#pragma once

#include <cstdint>

namespace ttyd::memory {

struct SmartAllocationData
{
    void *pMemory;
    uint32_t usedSize;
    void *pFileInfo; // FileEntryData *pFileInfo;
    uint16_t bUsed;
    uint8_t group;
    uint8_t pad_f;
    uint32_t unusedSize;
    SmartAllocationData *pNext;
    SmartAllocationData *pPrev;
} __attribute__((__packed__));

struct SmartWork
{
    void *pArena;
    SmartAllocationData allocations[2048];
    uint32_t unallocatedArenaStartSize;
    SmartAllocationData *pFirstUsed;
    SmartAllocationData *pLastUsed;
    SmartAllocationData *pFirstFree;
    SmartAllocationData *pLastFree;
    uint32_t lastFrameFreeCount;
} __attribute__((__packed__));

struct HeapEnd
{
    void *pHeap0End;
    void *pHeap1End;
    void *pHeap2End;
    void *pHeap3End;
    void *pHeap4End;
#ifdef TTYD_JP
    void *pHeap5End;
#endif
} __attribute__((__packed__));

struct HeapStart
{
    void *pHeap0Start;
    void *pHeap1Start;
    void *pHeap2Start;
    void *pHeap3Start;
    void *pHeap4Start;
#ifdef TTYD_JP
    void *pHeap5Start;
#endif
} __attribute__((__packed__));

enum SmartAllocationGroup
{
    kNone = 0,
    kArea,
    kBattle,
    kSingleFrame,
    kDoubleFrame,
};

static_assert(sizeof(SmartAllocationData) == 0x1C);
static_assert(sizeof(SmartWork) == 0xE01C);

#ifdef TTYD_JP
static_assert(sizeof(HeapEnd) == 0x18);
static_assert(sizeof(HeapStart) == 0x18);
#else
static_assert(sizeof(HeapEnd) == 0x14);
static_assert(sizeof(HeapStart) == 0x14);
#endif

extern "C" {

// memInit
// memClear
void *__memAlloc(uint32_t heap, uint32_t size);
void __memFree(uint32_t heap, void *ptr);
// _mapAlloc
// _mapAllocTail
// _mapFree
// smartInit
void L_smartReInit();
void smartAutoFree(uint32_t group);
void smartFree(SmartAllocationData *pAlloc);
SmartAllocationData *smartAlloc(uint32_t size, uint32_t group);
// smartGarbage
// smartTexObj

extern SmartWork *smartWorkPointer;
extern HeapEnd heapEnd;
extern HeapStart heapStart;

}

}