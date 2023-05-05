#ifndef CXX_H
#define CXX_H

#include <cstdint>
#include <new>

void *operator new(std::size_t size);
void *operator new[](std::size_t size);
void *operator new(std::size_t size, bool allocFromHead);
void *operator new[](std::size_t size, bool allocFromHead);
void operator delete(void *ptr);
void operator delete[](void *ptr);
void operator delete(void *ptr, std::size_t size);
void operator delete[](void *ptr, std::size_t size);

#endif
