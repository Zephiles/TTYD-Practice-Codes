#pragma once

#include <cstdint>

namespace gc::OSFont {

struct OSFontHeader
{
	uint16_t fontType;
	uint16_t firstChar;
	uint16_t lastChar;
	uint16_t invalChar;
	uint16_t ascent;
	uint16_t descent;
	uint16_t width;
	uint16_t leading;
	uint16_t cellWidth;
	uint16_t cellHeight;
	uint32_t sheetSize;
	uint16_t sheetFormat;
	uint16_t sheetColumn;
	uint16_t sheetRow;
	uint16_t sheetWidth;
	uint16_t sheetHeight;
	uint16_t widthTable;
	uint32_t sheetImage;
	uint32_t sheetFullSize;
	uint8_t c0;
	uint8_t c1;
	uint8_t c2;
	uint8_t c3;
} __attribute__((__packed__));

static_assert(sizeof(OSFontHeader) == 0x30);

extern "C" {

// GetFontCode
// Decode
uint16_t OSGetFontEncode();
// ReadFont
// OSLoadFont
// ParseStringS
// OSGetFontTexel
// ExpandFontSheet
bool OSInitFont(OSFontHeader *fontData);
char *OSGetFontTexture(const char *string, void **image, int32_t *posX, int32_t *posY, int32_t *width);

}

}