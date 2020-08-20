#pragma once

#include <cstdint>
#include <cstdarg>

namespace gc::OSError {

extern "C" {

void OSReport(const char *msg, ...);
// OSPanic
// OSSetErrorHandler
// __OSUnhandledException

}

}