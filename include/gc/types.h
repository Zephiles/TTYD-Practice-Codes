#ifndef GC_TYPES_H
#define GC_TYPES_H

#include <cstdint>

typedef int32_t BOOL;
typedef int32_t OSHeapHandle;

typedef int64_t OSTime;
typedef uint32_t OSTick;

typedef float mtx34[3][4];
typedef float mtx44[4][4];
typedef float mtxUnk4[][4];

struct Vec3
{
    float x, y, z;
} __attribute__((__packed__));

static_assert(sizeof(Vec3) == 0xC);

#endif
