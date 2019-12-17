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

extern "C" {

// memInit
// memClear
void *__memAlloc(uint32_t heap, uint32_t size);
void __memFree(uint32_t heap, void *ptr);
// _mapAlloc
// _mapAllocTail
// _mapFree
// smartInit
void smartReInit();
void smartAutoFree(uint32_t group);
void smartFree(SmartAllocationData *pAlloc);
SmartAllocationData *smartAlloc(uint32_t size, uint32_t group);
// smartGarbage
// smartTexObj

extern SmartWork *smartWorkPointer;

}

}