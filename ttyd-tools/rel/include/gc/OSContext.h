#pragma once

#include <cstdint>

namespace gc::OSContext {

struct OSContext
{
    uint32_t gpr[32];
    uint32_t cr;
    uint32_t lr;
    uint32_t ctr;
    uint32_t xer;
    double fpr[32];
    uint32_t fpscrPad;
    uint32_t fpscr;
    uint32_t srr0;
    uint32_t srr1;
    uint16_t mode;
    uint16_t state;
    uint32_t gqr[8];
    uint32_t psfPad;
    double psf[32];
} __attribute__((__packed__));

static_assert(sizeof(OSContext) == 0x2C8);

extern "C" {

// __OSLoadFPUContext
// __OSSaveFPUContext
// OSSaveFPUContext
// OSSetCurrentContext
// OSGetCurrentContext
// OSSaveContext
// OSLoadContext
// OSGetStackPointer
// OSClearContext
// OSInitContext
// OSDumpContext
// OSSwitchFPUContext
// __OSContextInit
void L_OSFillFPUContext(OSContext *context);

}

}