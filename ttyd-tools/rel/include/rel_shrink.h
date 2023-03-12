#pragma once

#include <gc/OSModule.h>

#include <cstdint>

#define RelShrinkRoundUp(val, align) \
    (val + align - 1) & ~(align - 1)

void MakeRelFixed(int32_t heap_id, gc::OSModule::OSModuleInfo *rel);
