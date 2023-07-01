#ifndef GC_VI_H
#define GC_VI_H

#include <cstdint>

extern "C"
{
    // __VIRetraceHandler
    // VISetPreRetraceCallback
    // VISetPostRetraceCallback
    // getTiming
    // __VIInit
    // VIInit
    void VIWaitForRetrace();
    // setFbbRegs
    // setVerticalRegs
    // VIConfigure
    void VIFlush();
    void VISetNextFrameBuffer(void *frameBuffer);
    void VISetBlack(bool black);
    uint32_t VIGetRetraceCount();
    // GetCurrentDisplayPosition
    // getCurrentFieldEvenOdd
    // VIGetNextField
    // VIGetCurrentLine
    // VIGetTvFormat
    // U_VIGetDTVStatus
    // __VIDisplayPositionToXY
    // __VIGetCurrentPosition
}

#endif
