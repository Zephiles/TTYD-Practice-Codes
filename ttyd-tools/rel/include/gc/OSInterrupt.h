#pragma once

#include <cstdint>

namespace gc::OSInterrupt {

extern "C" {

bool OSDisableInterrupts();
// OSEnableInterrupts
bool OSRestoreInterrupts(bool enable);
// __OSSetInterruptHandler
// __OSGetInterruptHandler
// __OSInterruptInit
// SetInterruptMask
// __OSMaskInterrupts
// __OSUnmaskInterrupts
// __OSDispatchInterrupt
// ExternalInterruptHandler

}

}