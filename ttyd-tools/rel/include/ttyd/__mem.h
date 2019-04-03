#pragma once

#include <cstdint>

namespace ttyd::__mem {

extern "C" {

void *memset(void *destination, uint32_t value, uint32_t amountOfBytesToSet);
// __fill_mem
void *memcpy(void *destination, const void *source, uint32_t amountOfBytesToCopy);

}

}
