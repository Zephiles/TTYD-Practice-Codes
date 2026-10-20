#ifndef GC_TYPES_H
#define GC_TYPES_H

#include <cstdint>

using BOOL = int32_t;
using OSHeapHandle = int32_t;

using OSTime = int64_t;
using OSTick = uint32_t;

using mtx34 = float[3][4];
using mtx44 = float[4][4];
using mtxUnk4 = float[][4];

struct Vec3
{
    float x, y, z;
} __attribute__((__packed__));

static_assert(sizeof(Vec3) == 0xC);

#endif
