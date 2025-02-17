#ifndef JPBATTLEHEAP_H
#define JPBATTLEHEAP_H

#ifdef TTYD_JP
#include "gc/OSAlloc.h"
#include "gc/types.h"

#include <cstdint>

#define BATTLE_HEAP_ORIGINAL_SIZE 0x40000
#define REL_TOTAL_USED_SIZE_ADDRESS 0x80004148

extern "C"
{
    extern HeapInfo *gBattleHeapInfo;

    void *asmAllocFromBattleHeap(uint32_t size);
    void asmFreeToBattleHeap(void *ptr);
}

void battleHeapVarsInit();

#endif
#endif
