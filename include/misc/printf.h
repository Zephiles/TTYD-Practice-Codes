#ifndef MISC_PRINTF_H
#define MISC_PRINTF_H

#include <cstddef>
#include <cstdint>
#include <cstdarg>

struct WriteFormatterFnData
{
    char *buffer;
    std::size_t maxLength;
    std::size_t charsWritten;
} __attribute__((__packed__));

static_assert(sizeof(WriteFormatterFnData) == 0xC);

// The return type seems to differ depending on which function is used, and currently the function does not need to be called
// directly, so just set it to void pointer for now
typedef void *(*WriteFormatterFn)(WriteFormatterFnData *data, const char *string, std::size_t length);

extern "C"
{
    void *__StringWrite(WriteFormatterFnData *data, const char *string, std::size_t length);
    int32_t __pformatter(WriteFormatterFn writeFunc, WriteFormatterFnData *data, const char *format, va_list args);
}

#endif
