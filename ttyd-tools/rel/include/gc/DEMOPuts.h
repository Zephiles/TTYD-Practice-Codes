#pragma once

#include <cstdint>
#include <cstdarg>

namespace gc::DEMOPuts {

enum class FontType : int32_t
{
	kOpaque = 0, // White on black
	kReverse, // Black on white
	kTranslucent, // White on transparent
};

extern "C" {

void DEMOInitCaption(FontType type, int32_t screenWidth, int32_t screenHeight);
void DEMOPuts(int16_t posX, int16_t posY, int16_t posZ, const char *string);
void DEMOPrintf(int16_t posX, int16_t posY, int16_t posZ, const char *format, ...);

}

}