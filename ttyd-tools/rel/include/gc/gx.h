#pragma once

#include <cstdint>

namespace gc::gx {

enum class GXPrimitive
{
	GX_QUADS 			= 128,
	GX_TRIANGLES 		= 144,
	GX_TRIANGLESTRIP 	= 152,
	GX_TRIANGLEFAN 		= 160,
	GX_LINES 			= 168,
	GX_LINESTRIP 		= 176,
	GX_POINTS 			= 184,
};

enum class GXVtxFmt
{
	GX_VTXFMT0 = 0,
	GX_VTXFMT1,
	GX_VTXFMT2,
	GX_VTXFMT3,
	GX_VTXFMT4,
	GX_VTXFMT5,
	GX_VTXFMT6,
	GX_VTXFMT7,
	GX_MAX_VTXFMT,
};

enum class GXChannelID
{
	GX_COLOR0 = 0,
	GX_COLOR1,
	GX_ALPHA0,
	GX_ALPHA1,
	GX_COLOR0A0,
	GX_COLOR1A1,
	GX_COLOR_ZERO,
	GX_ALPHA_BUMP,
	GX_ALPHA_BUMPN,
	GX_COLOR_NULL = 255,
};

extern "C" {

void GXBegin(GXPrimitive type, GXVtxFmt vtxFmt, uint16_t nVerts);
void GXSetChanMatColor(GXChannelID chan, uint8_t matColor[4]);

}

}