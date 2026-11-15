#ifndef GC_OSTHREAD_H
#define GC_OSTHREAD_H

#include <cstdint>

extern "C"
{
    // DefaultSwitchThreadCallback
    // __OSThreadInit
    // OSInitThreadQueue
    // OSGetCurrentThread
    // OSDisableScheduler
    // OSEnableScheduler
    // UnsetRun
    // __OSGetEffectivePriority
    // SetEffectivePriority
    // SelectThread
    // __OSReschedule
    void OSYieldThread();
    // OSCreateThread
    // OSExitThread
    // OSCancelThread
    // OSResumeThread
    // OSSuspendThread
    // OSSleepThread
    // OSWakeupThread
    // OSClearStack
}

#endif
