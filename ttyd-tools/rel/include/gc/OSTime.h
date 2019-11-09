#pragma once

#include <cstdint>

namespace gc::OSTime {

extern "C" {

int64_t OSGetTime();
// OSGetTick
// __OSGetSystemTime
// GetDates
// OSTicksToCalendarTime

}

}