#ifndef GC_OSCACHE_H
#define GC_OSCACHE_H

#include <cstdint>

extern "C"
{
    // DCEnable
    // DCInvalidateRange
    void DCFlushRange(void *startAddr, uint32_t nBytes);
    // DCStoreRange
    // DCFlushRangeNoSync
    // DCStoreRangeNoSync
    // DCZeroRange
    void ICInvalidateRange(void *startAddr, uint32_t nBytes);
    // ICFlashInvalidate
    // ICEnable
    // __LCEnable
    // LCEnable
    // LCDisable
    // LCStoreBlocks
    // LCStoreData
    // LCQueueWait
    // L2GlobalInvalidate
    // DMAErrorHandler
    // __OSCacheInit
}

#endif
