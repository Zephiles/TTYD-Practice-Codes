#pragma once

#include <cstdint>

namespace ttyd::string {

extern "C" {

// strstr
// strchr
int32_t strncmp(const char *str1, const char *str2, uint32_t amountOfBytesToCompare);
int32_t strcmp(const char *str1, const char *str2);
char *strcat(char *dest, const char *source);
char *strncpy(char *dest, const char *source, uint32_t amountOfBytesToCopy);
char *strcpy(char *dest, const char *source);
int32_t strlen(const char *str);

}

}