#ifndef MISC_PRINTF_H
#define MISC_PRINTF_H

#include <cstddef>
#include <cstdint>
#include <cstdarg>

struct WriteFormatterFnData
{
    char *buffer;
    std::size_t maxLength;
    std::size_t bytesWritten;
} __attribute__((__packed__));

static_assert(sizeof(WriteFormatterFnData) == 0xC);

typedef void *(*WriteFormatterFn)(WriteFormatterFnData *data, const char *string, std::size_t length);

extern "C"
{
    void *__StringWrite(WriteFormatterFnData *data, const char *string, std::size_t length);
    int32_t __pformatter(WriteFormatterFn writeFunc, WriteFormatterFnData *data, const char *format, va_list args);
}

#endif
