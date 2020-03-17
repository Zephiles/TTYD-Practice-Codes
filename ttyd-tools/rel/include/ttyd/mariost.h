#pragma once

#include <gc/OSContext.h>

#include <cstdint>

namespace ttyd::mariost {

extern "C" {

void marioStInit();
void marioStMain();
void marioStDisp();
void marioStSystemLevel(uint32_t level);
uint32_t marioStGetSystemLevel();
void viPostCallback(uint32_t retraceCount);
void gcDvdCheckThread();
void gcRumbleCheck();
// L_gcResetCheck
void N_systemErrorHandler(uint16_t error, gc::OSContext::OSContext *context, uint32_t dsisr, uint32_t dar);
// N_setupErrorHandler

extern void *globalWorkPointer;

}

}