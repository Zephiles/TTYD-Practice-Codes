#pragma once

#include <cstdint>

namespace ttyd::swdrv {

extern "C" {

uint8_t _swByteGet(uint32_t lsw);
void _swByteSet(uint32_t lsw, uint8_t value);
void _swClear(uint32_t lswf);
bool _swGet(uint32_t lswf);
void _swSet(uint32_t lswf);
uint32_t swByteGet(uint32_t gsw);
void swByteSet(uint32_t gsw, uint32_t value); // The value for GSW(0) can be uint32_t; the rest are uint8_t
void swClear(uint32_t gswf);
bool swGet(uint32_t gswf);
void swSet(uint32_t gswf);
// swReInit
void swInit();

}

}