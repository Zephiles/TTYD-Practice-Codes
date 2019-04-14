#pragma once

#include <cstdint>

namespace gc::gx {

extern "C" {

enum GXPrimitive
{
	GX_POINTS = 0,
	GX_LINES,
	GX_LINESTRIP,
	GX_TRIANGLES,
	GX_TRIANGLESTRIP,
	GX_TRIANGLEFAN,
	GX_QUADS,
};

enum GXVtxFmt
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

enum GXChannelID
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
	GX_COLOR_NULL,
};

void GXBegin(GXPrimitive type, GXVtxFmt vtxFmt, uint16_t nVerts);
void GXSetChanMatColor(GXChannelID chan, uint8_t matColor[4]);

}

}