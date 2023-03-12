#include "rel_shrink.h"

#include <gc/OSModule.h>
#include <gc/OSAlloc.h>

// Origial code made by PistonMiner:
// https://gist.github.com/PistonMiner/3cf955e887bc87f149f5349807a77f8b

void ShrinkAllocation(int32_t heap_id, gc::OSModule::OSModuleInfo *rel, int32_t new_size)
{
    constexpr int32_t kGranularity = 32;
    
    // Round to heap granularity
    new_size = RelShrinkRoundUp(new_size, kGranularity);
    
    gc::OSAlloc::ChunkInfo *first_chunk = reinterpret_cast<gc::OSAlloc::ChunkInfo *>(
        reinterpret_cast<uint8_t *>(rel) - kGranularity);
    
    const int32_t old_size = first_chunk->size - kGranularity;
    
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
    // Remove imports for the current rel and the dol to avoid invalid
    // memory accesses during unlink.
    
    // The imports for the current rel and dol should always be at the
    // end of the imp table.
    constexpr int32_t kImpTableSectionSize = sizeof(gc::OSModule::OSModuleImpSection);
    const uint32_t imp_size_rounded = RelShrinkRoundUp(rel->impSize, kImpTableSectionSize);
    const int32_t total_sections = imp_size_rounded / kImpTableSectionSize;
    
    const gc::OSModule::OSModuleImpSection *impTable = 
        reinterpret_cast<gc::OSModule::OSModuleImpSection *>(rel->impOffset);
    
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
            rel->impSize = i * kImpTableSectionSize;
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
    ShrinkAllocation(heap_id, rel, fixed_size);
}
