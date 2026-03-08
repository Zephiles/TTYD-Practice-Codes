#include "misc/printf.h"

#include <cstddef>
#include <cstdint>
#include <cstdarg>

// Place vsnprintf and snprintf in an extern "C" block to allow them to be accessed via cstdio
extern "C"
{
    int vsnprintf(char *buffer, size_t maxLength, const char *format, va_list args)
    {
        return __vsnprintf(buffer, maxLength, format, args);
    }

    int snprintf(char *buffer, size_t maxLength, const char *format, ...)
    {
        va_list args;
        va_start(args, format);

        const int ret = __vsnprintf(buffer, maxLength, format, args);

        va_end(args);
        return ret;
    }
}