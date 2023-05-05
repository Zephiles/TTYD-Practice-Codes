#ifndef MOD_H
#define MOD_H

#include <cstdint>

#define VERSION_STRING "v4.0-dev"

enum class VariableType : uint8_t
{
    s8 = 0,
    s16,
    s32,
    s64,
    u8,
    u16,
    u32,
    u64,
    f32,
    f64,
    string,
    time,
};

union ValueType
{
    int8_t s8;
    int16_t s16;
    int32_t s32;
    int64_t s64;
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
    float f32;
    double f64;
};

class Mod
{
   public:
    bool systemLevelIsSet;
};

extern Mod *gMod;

#endif
