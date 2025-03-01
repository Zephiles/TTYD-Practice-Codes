#include "gc/types.h"
#include "gc/OSAlloc.h"
#include "gc/OSModule.h"
#include "misc/heap.h"
#include "misc/utils.h"
#include "ttyd/memory.h"

#include <cstdint>

void *allocFromHeapTail(OSHeapHandle handle, uint32_t size)
{
    // Increase the size to account for extra alignment padding
    size = roundUp(size + HEADER_SIZE, HEAP_ALIGNMENT);

    // Make sure there is at least one free chunk available
    HeapInfo *heapArrayPtr = &HeapArray[handle];
    ChunkInfo *chunk = heapArrayPtr->firstFree;
    if (!chunk)
    {
        return nullptr;
    }

    // Get a pointer to the last free entry in the heap
    while (1)
    {
        ChunkInfo *nextChunk = chunk->next;
        if (!nextChunk)
        {
            // Reached the end of the chunks, so the current chunk should be the last chunk
            break;
        }
        else
        {
            chunk = nextChunk;
        }
    }

    // Search for a chunk that has enough space to hold the requested size
    for (; chunk; chunk = chunk->prev)
    {
        if (chunk->size >= size)
        {
            break;
        }
    }

    // Make sure a chunk was found
    if (!chunk)
    {
        return nullptr;
    }

    // Get the remaining size once the current size is allocated
    const uint32_t remainingSize = chunk->size - size;

    // Check if the remaining size can be made into a chunk
    if (remainingSize >= MIN_CHUNK_SIZE)
    {
        // Update the size for the current chunk
        chunk->size = remainingSize;

        // Create a new chunk to use for the allocated memory
        chunk = reinterpret_cast<ChunkInfo *>(reinterpret_cast<uint32_t>(chunk) + remainingSize);

        // Update the size for the new chunk
        chunk->size = size;
    }
    else
    {
        // The remaining size is too small to be made into a chunk, so just use all of it
        if (chunk->next)
        {
            chunk->next->prev = chunk->prev;
        }

        if (chunk->prev)
        {
            chunk->prev->next = chunk->next;
        }
        else
        {
            heapArrayPtr->firstFree = chunk->next;
        }
    }

    // Add the chunk to the allocated list
    ChunkInfo *firstUsedChunk = heapArrayPtr->firstUsed;
    chunk->next = firstUsedChunk;
    chunk->prev = nullptr;

    if (firstUsedChunk)
    {
        firstUsedChunk->prev = chunk;
    }

    heapArrayPtr->firstUsed = chunk;
    return reinterpret_cast<void *>(reinterpret_cast<uint32_t>(chunk) + HEADER_SIZE);
}

// Origial code made by PistonMiner:
// https://gist.github.com/PistonMiner/3cf955e887bc87f149f5349807a77f8b
static void shrinkAllocation(int32_t heap, OSModuleInfo *rel, int32_t new_size)
{
    // Round to HEAP_ALIGNMENT
    new_size = roundUp(new_size, HEAP_ALIGNMENT);

    ChunkInfo *first_chunk = reinterpret_cast<ChunkInfo *>(reinterpret_cast<uint8_t *>(rel) - HEAP_ALIGNMENT);
    const int32_t old_size = first_chunk->size - HEAP_ALIGNMENT;

    // Check we have sufficient space to split
    if (static_cast<int32_t>((new_size + MIN_CHUNK_SIZE)) > old_size)
    {
        return;
    }

    // Shrink used chunk
    first_chunk->size = new_size + HEAP_ALIGNMENT;

    // Build free chunk
    // Remaining fields will be filled in by DLInsert
    ChunkInfo *second_chunk = reinterpret_cast<ChunkInfo *>(reinterpret_cast<uint8_t *>(rel) + new_size);
    second_chunk->size = old_size - new_size;

    // Insert free space into heap
    HeapInfo *heap_info = &HeapArray[heapHandle[heap]];
    heap_info->firstFree = DLInsert(heap_info->firstFree, second_chunk);
}

void makeRelFixed(int32_t heap, OSModuleInfo *rel)
{
    // Remove imports for the current rel and the dol to avoid invalid memory accesses during unlink.
    // The imports for the current rel and dol should always be at the end of the imp table.
    constexpr int32_t impTableSectionSize = sizeof(OSModuleImpSection);
    const uint32_t imp_size_rounded = roundUp(rel->impSize, impTableSectionSize);
    const int32_t total_sections = imp_size_rounded / impTableSectionSize;

    const OSModuleImpSection *impTable = reinterpret_cast<OSModuleImpSection *>(rel->impOffset);
    const int32_t rel_module_id = rel->id;

    // Loop through each entry until either the dol or rel id is found.
    // If found, then that should be the end of the imp table.
    bool shouldShrinkRel = false;
    for (int32_t i = 0; i < total_sections; i++)
    {
        const int32_t imp_module_id = impTable[i].moduleId;
        if ((imp_module_id == 0) || (imp_module_id == rel_module_id))
        {
            shouldShrinkRel = true;
            rel->impSize = i * impTableSectionSize;
            break;
        }
    }

    // Make sure the rel should be actually be shrunk
    if (!shouldShrinkRel)
    {
        return;
    }

    // Figure out fixed size
    int32_t fixed_size = rel->fixSize - reinterpret_cast<uint32_t>(rel);

    // Free up unused space
    shrinkAllocation(heap, rel, fixed_size);
}
