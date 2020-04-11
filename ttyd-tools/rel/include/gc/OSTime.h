#pragma once

#include <cstdint>

namespace gc::OSTime {

extern "C" {

int64_t OSGetTime();
uint32_t OSGetTick();
// __OSGetSystemTime
// GetDates
// OSTicksToCalendarTime

}

}