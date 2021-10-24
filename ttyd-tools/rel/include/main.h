#pragma once

#include <cstdint>

namespace mod {

void fixRoomProblems();
void clearHeapCorruptionBuffer();
void clearMemoryUsageBuffer();
void setOSTime(int64_t time);

}