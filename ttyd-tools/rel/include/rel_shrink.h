#pragma once

#include <gc/OSModule.h>

#include <cstdint>

namespace mod {

void MakeRelFixed(int32_t heap_id, gc::OSModule::OSModuleInfo *rel);

}