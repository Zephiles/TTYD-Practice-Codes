#include "gc/types.h"
#include "gc/OSAlloc.h"
#include "gc/OSModule.h"
#include "misc/heap.h"
#include "misc/utils.h"

#include <cstdint>

void *allocFromHeapTail(OSHeapHandle heapHandle, uint32_t size)
{
    // Make sure HeapArray is set
    HeapInfo *heapArrayPtr = HeapArray;
    if (!heapArrayPtr)
    {
        return nullptr;
    }

    // Make sure the size is valid
    if (size == 0)
    {
        return nullptr;
    }

    // Make sure the heap is valid
    if (heapHandle >= NumHeaps)
    {
        return nullptr;
    }

    // Make sure the current heap has some free space
    heapArrayPtr = &heapArrayPtr[heapHandle];
    if (heapArrayPtr->capacity == 0)
    {
        return nullptr;
    }

    // Increase the size to account for extra alignment padding
    size = roundUp(size + HEADER_SIZE, HEAP_ALIGNMENT);

    // Make sure there is at least one free chunk available
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

    // Make sure the chunk is properly aligned
    if (getOffset(chunk, HEAP_ALIGNMENT) != 0)
    {
        return nullptr;
    }

    // Get the remaining size once the current size is allocated
    uint32_t remainingSize = chunk->size - size;

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
        ChunkInfo *prevChunk = chunk->prev;
        ChunkInfo *nextChunk = chunk->next;

        if (prevChunk)
        {
            prevChunk->next = nextChunk;
        }

        if (nextChunk)
        {
            nextChunk->prev = prevChunk;
        }
    }

    // Add the chunk to the end of the allocated list
    ChunkInfo *nextChunk = heapArrayPtr->firstUsed;
    ChunkInfo *prevChunk = nullptr;

    for (; nextChunk; prevChunk = nextChunk, nextChunk = nextChunk->next)
    {
        if (chunk <= nextChunk)
        {
            break;
        }
    }

    chunk->prev = prevChunk;
    chunk->next = nextChunk;

    if (prevChunk)
    {
        prevChunk->next = chunk;
    }

    if (nextChunk)
    {
        nextChunk->prev = chunk;
    }

    return reinterpret_cast<void *>(reinterpret_cast<uint32_t>(chunk) + HEADER_SIZE);
}

// Origial code made by PistonMiner:
// https://gist.github.com/PistonMiner/3cf955e887bc87f149f5349807a77f8b
void shrinkAllocation(OSHeapHandle heapHandle, OSModuleInfo *rel, int32_t new_size)
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
    // You can just pass the id in if you don't want to derive it
    HeapInfo *heap_info = &HeapArray[heapHandle];
    heap_info->firstFree = DLInsert(heap_info->firstFree, second_chunk);
}

void makeRelFixed(OSHeapHandle heapHandle, OSModuleInfo *rel)
{
    // Remove imports for the current rel and the dol to avoid invalid
    // memory accesses during unlink.

    // The imports for the current rel and dol should always be at the
    // end of the imp table.
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
    shrinkAllocation(heapHandle, rel, fixed_size);
}
