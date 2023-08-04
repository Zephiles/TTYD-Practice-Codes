#include "misc/printf.h"

#include <cstddef>
#include <cstdint>
#include <cstdarg>

// Place vsnprintf and snprintf in an extern "C" block to allow them to be accessed via cstdio
extern "C"
{
    int32_t vsnprintf(char *buffer, std::size_t maxLength, const char *format, va_list args)
    {
        // Make sure buffer and maxLength are valid before doing anything
        if (!buffer || (static_cast<int32_t>(maxLength) <= 0))
        {
            return 0;
        }

        WriteFormatterFnData data;
        data.buffer = buffer;
        data.maxLength = maxLength;
        data.charsWritten = 0;

        const int32_t ret = __pformatter(__StringWrite, &data, format, args);

        // __pformatter does not write a null terminator, so it must be written manually
        int32_t maxIndex = maxLength - 1;
        if (ret < maxIndex)
        {
            maxIndex = ret;
        }
        buffer[maxIndex] = '\0';

        return ret;
    }

    int32_t snprintf(char *buffer, std::size_t maxLength, const char *format, ...)
    {
        va_list args;
        va_start(args, format);

        const int32_t ret = vsnprintf(buffer, maxLength, format, args);

        va_end(args);
        return ret;
    }
}