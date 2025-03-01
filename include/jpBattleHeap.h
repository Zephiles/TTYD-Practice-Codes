#ifndef JPBATTLEHEAP_H
#define JPBATTLEHEAP_H

#ifdef TTYD_JP
#include <cstdint>

#define BATTLE_HEAP_ORIGINAL_SIZE 0x40000
#define REL_TOTAL_USED_SIZE_ADDRESS 0x80004148

void battleHeapVarsInit();

#endif
#endif
