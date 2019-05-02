#include "clearcache.h"

#include <gc/OSCache.h>

#include <cstdint>

namespace clearcache {

void clear_DC_IC_Cache(void *ptr, uint32_t size)
{
	gc::OSCache::DCFlushRange(ptr, size);
	gc::OSCache::ICInvalidateRange(ptr, size);
}

}