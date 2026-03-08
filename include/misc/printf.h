#ifndef MISC_PRINTF_H
#define MISC_PRINTF_H

#include <cstddef>
#include <cstdint>
#include <cstdarg>

struct PrintfWriteData
{
    char *buffer;
    size_t maxLength;
    size_t charsWritten;
} __attribute__((__packed__));

static_assert(sizeof(PrintfWriteData) == 0xC);

typedef bool (*PrintfWriteFunc)(PrintfWriteData *data, const char *string, size_t length);

extern "C"
{
    bool __StringWrite(PrintfWriteData *data, const char *string, size_t length);
    int32_t __pformatter(PrintfWriteFunc writeFunc, PrintfWriteData *data, const char *format, va_list args);
}

// Set up an inlined copy of `vsnprintf` to be used for both `snprintf` and `vsnprintf`, as if only one of these is being used
// in the project, then it's more memory-efficient to use the inlined copy rather than having `snprintf` call `vsnprintf`.
__attribute__((always_inline)) inline int __vsnprintf(char *buffer, size_t maxLength, const char *format, va_list args)
{
    // Make sure buffer and maxLength are valid before doing anything
    if (!buffer || (static_cast<int32_t>(maxLength) <= 0))
    {
        return -1;
    }

    PrintfWriteData data;
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

#endif
