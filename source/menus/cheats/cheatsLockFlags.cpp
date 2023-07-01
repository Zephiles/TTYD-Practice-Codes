#include "menus/cheatsMenu.h"
#include "ttyd/evtmgr.h"
#include "ttyd/mariost.h"

#include <cstdint>

void *getLockFlagsRegionPtr(uint32_t region)
{
    EvtWork *eventWorkPtr = evtGetWork();
    GlobalWork *workPtr = globalWorkPtr;

    switch (region)
    {
        case LockFlagsOptions::LOCK_FLAGS_GSW:
        {
            return &workPtr->mGSW[0];
        }
        case LockFlagsOptions::LOCK_FLAGS_GSWF:
        {
            return &workPtr->mGSWF[0];
        }
        case LockFlagsOptions::LOCK_FLAGS_GW:
        {
            return &eventWorkPtr->gwData[0];
        }
        case LockFlagsOptions::LOCK_FLAGS_GF:
        {
            return &eventWorkPtr->gfData[0];
        }
        case LockFlagsOptions::LOCK_FLAGS_LSW:
        {
            return &workPtr->mLSW[0];
        }
        case LockFlagsOptions::LOCK_FLAGS_LSWF:
        {
            return &workPtr->mLSWF[0];
        }
        default:
        {
            return nullptr;
        }
    }
}

uint32_t getLockFlagsRegionSize(uint32_t region)
{
    EvtWork *eventWorkPtr = evtGetWork();
    GlobalWork *workPtr = globalWorkPtr;

    switch (region)
    {
        case LockFlagsOptions::LOCK_FLAGS_GSW:
        {
            return sizeof(workPtr->mGSW);
        }
        case LockFlagsOptions::LOCK_FLAGS_GSWF:
        {
            return sizeof(workPtr->mGSWF);
        }
        case LockFlagsOptions::LOCK_FLAGS_GW:
        {
            return sizeof(eventWorkPtr->gwData);
        }
        case LockFlagsOptions::LOCK_FLAGS_GF:
        {
            return sizeof(eventWorkPtr->gfData);
        }
        case LockFlagsOptions::LOCK_FLAGS_LSW:
        {
            return sizeof(workPtr->mLSW);
        }
        case LockFlagsOptions::LOCK_FLAGS_LSWF:
        {
            return sizeof(workPtr->mLSWF);
        }
        default:
        {
            return 0;
        }
    }
}

void getLockFlagsRegionPtrAndSize(uint32_t region, void **ptr, uint32_t *size)
{
    if (ptr)
    {
        *ptr = getLockFlagsRegionPtr(region);
    }

    if (size)
    {
        *size = getLockFlagsRegionSize(region);
    }
}
