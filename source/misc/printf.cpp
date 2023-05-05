#include "misc/printf.h"

#include <cstddef>
#include <cstdint>
#include <cstdarg>

extern "C"
{
    int32_t vsnprintf(char *buffer, std::size_t maxLength, const char *format, va_list args)
    {
        WriteFormatterFnData data;
        data.buffer = buffer;
        data.maxLength = maxLength;
        data.bytesWritten = 0;

        int32_t ret = __pformatter(__StringWrite, &data, format, args);
        if (buffer)
        {
            int32_t maxIndex = maxLength - 1;
            if (ret < maxIndex)
            {
                maxIndex = ret;
            }
            buffer[maxIndex] = '\0';
        }

        return ret;
    }

    int32_t snprintf(char *buffer, std::size_t maxLength, const char *format, ...)
    {
        va_list args;
        va_start(args, format);

        int32_t ret = vsnprintf(buffer, maxLength, format, args);

        va_end(args);
        return ret;
    }
}