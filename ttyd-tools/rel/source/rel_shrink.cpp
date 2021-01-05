#include "rel_shrink.h"

#include <gc/OSModule.h>
#include <gc/OSAlloc.h>

// Origial code made by PistonMiner:
// https://gist.github.com/PistonMiner/3cf955e887bc87f149f5349807a77f8b

namespace mod {

int32_t RoundUp(int32_t val, int32_t align)
{
    return (val + align - 1) & ~(align - 1);
}

void ShrinkAllocation(int32_t heap_id, gc::OSModule::OSModuleInfo *rel, int32_t new_size)
{
    constexpr int32_t kGranularity = 32;
    
    // Round to heap granularity
    new_size = RoundUp(new_size, kGranularity);
    
    gc::OSAlloc::ChunkInfo *first_chunk = reinterpret_cast<gc::OSAlloc::ChunkInfo *>(
        reinterpret_cast<uint8_t *>(rel) - kGranularity);
    
    int32_t old_size = first_chunk->size - kGranularity;
    
    // Check we have sufficient space to split
    if ((new_size + (2 * kGranularity)) > old_size)
    {
        return;
    }
    
    // Shrink used chunk
    first_chunk->size = new_size + kGranularity;
    
    // Build free chunk
    // Remaining fields will be filled in by DLInsert
    gc::OSAlloc::ChunkInfo *second_chunk = reinterpret_cast<gc::OSAlloc::ChunkInfo *>(
        reinterpret_cast<uint8_t *>(rel) + new_size);
    
    second_chunk->size = old_size - new_size;
    
    // Insert free space into heap
    // You can just pass the ID in if you don't want to derive it
    gc::OSAlloc::HeapInfo *heap_info = &gc::OSAlloc::HeapArray[heap_id];
    heap_info->firstFree = gc::OSAlloc::DLInsert(heap_info->firstFree, second_chunk);
}

void MakeRelFixed(int32_t heap_id, gc::OSModule::OSModuleInfo *rel)
{
    // Clear imports to avoid invalid memory accesses during unlink
    // 
    // NB if you need to support RELs with relocations against other RELs, you
    // must trim off only the self-relocations and relocations against the DOL.
    // These will always be the last imports in the list if present, so it's
    // sufficient to scan for them and reduce the size field appropriately.
    rel->impSize = 0;
    
    // Figure out fixed size
    int32_t fixed_size = rel->fixSize - reinterpret_cast<uint32_t>(rel);
    
    // Free up unused space
    ShrinkAllocation(heap_id, rel, fixed_size);
}

}