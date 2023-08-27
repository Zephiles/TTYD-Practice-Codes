#ifndef MISC_HEAP_H
#define MISC_HEAP_H

#include "gc/types.h"
#include "gc/OSAlloc.h"
#include "gc/OSModule.h"
#include "misc/utils.h"

#include <cstdint>

#define HEAP_ALIGNMENT 32
#define HEADER_SIZE roundUp(sizeof(ChunkInfo), HEAP_ALIGNMENT)
#define MIN_CHUNK_SIZE (HEADER_SIZE + HEAP_ALIGNMENT)

void *allocFromHeapTail(OSHeapHandle handle, uint32_t size);
void makeRelFixed(int32_t heap, OSModuleInfo *rel);

#endif
