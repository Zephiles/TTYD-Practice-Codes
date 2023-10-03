#ifndef TTYD_OSERROR_H
#define TTYD_OSERROR_H

#include <cstdint>
#include <cstdarg>

extern "C"
{
    void OSReport(const char *format, ...);
    // OSPanic
    // OSSetErrorHandler
    // __OSUnhandledException
}

#endif
