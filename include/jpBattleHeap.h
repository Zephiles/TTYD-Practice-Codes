#ifndef JPBATTLEHEAP_H
#define JPBATTLEHEAP_H

#ifdef TTYD_JP
#include "gc/OSAlloc.h"
#include "gc/types.h"

#include <cstdint>

#define BATTLE_HEAP_CUSTOM_HANDLE 0xFF
#define BATTLE_HEAP_ORIGINAL_SIZE 0x40000
#define REL_TOTAL_USED_SIZE_ADDRESS 0x80004148

extern "C"
{
    void asmOSAllocFromHeapGetHeapArrayPtrStart();
    void asmOSAllocFromHeapGetHeapArrayPtrBranchBack();
    void asmOSFreeToHeapGetHeapArrayPtr();

    HeapInfo *OS_getHeapArrayPtr(uint32_t heapArrayPtrRaw, OSHeapHandle handle);
}

extern HeapInfo *gBattleHeapInfo;

void battleHeapVarsInit();

#endif
#endif
