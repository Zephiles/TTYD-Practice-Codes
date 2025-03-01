#ifndef GC_GX_H
#define GC_GX_H

#include "gc/ppc.h"
#include "gc/mtx.h"

#include <cstdint>

enum class GXAttribute
{
    GX_VA_PNMTXIDX = 0,
    GX_VA_TEX0MTXIDX,
    GX_VA_TEX1MTXIDX,
    GX_VA_TEX2MTXIDX,
    GX_VA_TEX3MTXIDX,
    GX_VA_TEX4MTXIDX,
    GX_VA_TEX5MTXIDX,
    GX_VA_TEX6MTXIDX,
    GX_VA_TEX7MTXIDX,
    GX_VA_POS,
    GX_VA_NRM,
    GX_VA_CLR0,
    GX_VA_CLR1,
    GX_VA_TEX0,
    GX_VA_TEX1,
    GX_VA_TEX2,
    GX_VA_TEX3,
    GX_VA_TEX4,
    GX_VA_TEX5,
    GX_VA_TEX6,
    GX_VA_TEX7,
    GX_POS_MTX_ARRAY,
    GX_NRM_MTX_ARRAY,
    GX_TEX_MTX_ARRAY,
    GX_LIGHT_ARRAY,
    GX_VA_NBT,
    GX_VA_MAX_ATTR,
    GX_VA_NULL = 255,
};

enum class GXAttributeType
{
    GX_NONE = 0,
    GX_DIRECT,
    GX_INDEX8,
    GX_INDEX16,
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

enum class GXComponentContents
{
    GX_POS_XY = 0,
    GX_POS_XYZ,
    GX_NRM_XYZ = 0,
    GX_NRM_NBT,
    GX_NRM_NBT3,
    GX_CLR_RGB = 0,
    GX_CLR_RGBA,
    GX_TEX_S = 0,
    GX_TEX_ST,
};

enum class GXComponentType
{
    GX_U8 = 0,
    GX_S8,
    GX_U16,
    GX_S16,
    GX_F32,
    GX_RGB565 = 0,
    GX_RGB8,
    GX_RGBX8,
    GX_RGBA4,
    GX_RGBA6,
    GX_RGBA8,
};

enum class GXTexCoordID
{
    GX_TEXCOORD0 = 0,
    GX_TEXCOORD1,
    GX_TEXCOORD2,
    GX_TEXCOORD3,
    GX_TEXCOORD4,
    GX_TEXCOORD5,
    GX_TEXCOORD6,
    GX_TEXCOORD7,
    GX_MAX_TEXCOORD,
    GX_TEXCOORD_NULL = 255,
};

enum class GXTexGenType
{
    GX_TG_MTX3x4 = 0,
    GX_TG_MTX2x4,
    GX_TG_BUMP0,
    GX_TG_BUMP1,
    GX_TG_BUMP2,
    GX_TG_BUMP3,
    GX_TG_BUMP4,
    GX_TG_BUMP5,
    GX_TG_BUMP6,
    GX_TG_BUMP7,
    GX_TG_SRTG,
};

enum GXTexMtx
{
    GX_TEXMTX0 = 30,
    GX_TEXMTX1 = 33,
    GX_TEXMTX2 = 36,
    GX_TEXMTX3 = 39,
    GX_TEXMTX4 = 42,
    GX_TEXMTX5 = 45,
    GX_TEXMTX6 = 48,
    GX_TEXMTX7 = 51,
    GX_TEXMTX8 = 54,
    GX_TEXMTX9 = 57,
    GX_IDENTITY = 60,
};

enum class GXTexGenSrc
{
    GX_TG_POS = 0,
    GX_TG_NRM,
    GX_TG_BINRM,
    GX_TG_TANGENT,
    GX_TG_TEX0,
    GX_TG_TEX1,
    GX_TG_TEX2,
    GX_TG_TEX3,
    GX_TG_TEX4,
    GX_TG_TEX5,
    GX_TG_TEX6,
    GX_TG_TEX7,
    GX_TG_TEXCOORD0,
    GX_TG_TEXCOORD1,
    GX_TG_TEXCOORD2,
    GX_TG_TEXCOORD3,
    GX_TG_TEXCOORD4,
    GX_TG_TEXCOORD5,
    GX_TG_TEXCOORD6,
    GX_TG_COLOR0,
    GX_TG_COLOR1,
};

enum class GXPrimitive
{
    GX_QUADS = 128,
    GX_TRIANGLES = 144,
    GX_TRIANGLESTRIP = 152,
    GX_TRIANGLEFAN = 160,
    GX_LINES = 168,
    GX_LINESTRIP = 176,
    GX_POINTS = 184,
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

enum class GXTexFmt
{
    GX_TF_I4 = 0,
    GX_TF_I8,
    GX_TF_IA4,
    GX_TF_IA8,
    GX_TF_RGB565,
    GX_TF_RGB5A3,
    GX_TF_RGBA8,
    GX_TF_CMPR = 14,
    GX_CTF_R4 = 32,
    GX_CTF_RA4 = 34,
    GX_CTF_RA8,
    GX_CTF_YUVA8 = 38,
    GX_CTF_A8,
    GX_CTF_R8,
    GX_CTF_G8,
    GX_CTF_B8,
    GX_CTF_RG8,
    GX_CTF_GB8,
    GX_TF_Z8 = 17,
    GX_TF_Z16 = 19,
    GX_TF_Z24X8 = 22,
    GX_CTF_Z4 = 48,
    GX_CTF_Z8M = 57,
    GX_CTF_Z8L,
    GX_CTF_Z16L = 60,
    GX_TF_A8 = GX_CTF_A8,
};

enum class GXTexWrapMode
{
    GX_CLAMP = 0,
    GX_REPEAT,
    GX_MIRROR,
    GX_MAX_TEXWRAPMODE,
};

enum class GXTexMapID
{
    GX_TEXMAP0 = 0,
    GX_TEXMAP1,
    GX_TEXMAP2,
    GX_TEXMAP3,
    GX_TEXMAP4,
    GX_TEXMAP5,
    GX_TEXMAP6,
    GX_TEXMAP7,
    GX_MAX_TEXMAP,
    GX_TEXMAP_NULL = 255,
    GX_TEX_DISABLE,
};

enum class GXTevStageID
{
    GX_TEVSTAGE0 = 0,
    GX_TEVSTAGE1,
    GX_TEVSTAGE2,
    GX_TEVSTAGE3,
    GX_TEVSTAGE4,
    GX_TEVSTAGE5,
    GX_TEVSTAGE6,
    GX_TEVSTAGE7,
    GX_TEVSTAGE8,
    GX_TEVSTAGE9,
    GX_TEVSTAGE10,
    GX_TEVSTAGE11,
    GX_TEVSTAGE12,
    GX_TEVSTAGE13,
    GX_TEVSTAGE14,
    GX_TEVSTAGE15,
    GX_MAX_TEVSTAGE,
};

enum class GXTevMode
{
    GX_MODULATE = 0,
    GX_DECAL,
    GX_BLEND,
    GX_REPLACE,
    GX_PASSCLR,
};

enum class GXCompare
{
    GX_NEVER = 0,
    GX_LESS,
    GX_EQUAL,
    GX_LEQUAL,
    GX_GREATER,
    GX_NEQUAL,
    GX_GEQUAL,
    GX_ALWAYS,
};

enum class GXAlphaOperation
{
    GX_AOP_AND = 0,
    GX_AOP_OR,
    GX_AOP_XOR,
    GX_AOP_XNOR,
    GX_MAX_ALPHAOP,
};

enum class GXBlendMode
{
    GX_BM_NONE = 0,
    GX_BM_BLEND,
    GX_BM_LOGIC,
    GX_BM_SUBTRACT,
    GX_MAX_BLENDMODE,
};

enum class GXBlendFactor
{
    GX_BL_ZERO = 0,
    GX_BL_ONE,
    GX_BL_SRCCLR,
    GX_BL_INVSRCCLR,
    GX_BL_SRCALPHA,
    GX_BL_INVSRCALPHA,
    GX_BL_DSTALPHA,
    GX_BL_INVDSTALPHA,
    GX_BL_DSTCLR = GX_BL_SRCCLR,
    GX_BL_INVDSTCLR = GX_BL_INVSRCCLR,
};

enum class GXLogicOperation
{
    GX_LO_CLEAR = 0,
    GX_LO_AND,
    GX_LO_REVAND,
    GX_LO_COPY,
    GX_LO_INVAND,
    GX_LO_NOOP,
    GX_LO_XOR,
    GX_LO_OR,
    GX_LO_NOR,
    GX_LO_EQUIV,
    GX_LO_INV,
    GX_LO_REVOR,
    GX_LO_INVCOPY,
    GX_LO_INVOR,
    GX_LO_NAND,
    GX_LO_SET,
};

enum class GXProjectionType
{
    GX_PERSPECTIVE = 0,
    GX_ORTHOGRAPHIC,
};

enum GXPosNormMtx
{
    GX_PNMTX0 = 0,
    GX_PNMTX1 = 3,
    GX_PNMTX2 = 6,
    GX_PNMTX3 = 9,
    GX_PNMTX4 = 12,
    GX_PNMTX5 = 15,
    GX_PNMTX6 = 18,
    GX_PNMTX7 = 21,
    GX_PNMTX8 = 24,
    GX_PNMTX9 = 27,
};

enum class GXTexMtxType
{
    GX_MTX3x4 = 0,
    GX_MTX2x4,
};

enum class GXCullMode
{
    GX_CULL_NONE = 0,
    GX_CULL_FRONT,
    GX_CULL_BACK,
    GX_CULL_ALL,
};

enum class GXColorSrc
{
    GX_SRC_REG = 0,
    GX_SRC_VTX,
};

enum class GXLightID
{
    GX_LIGHT_NULL = 0,
    GX_LIGHT0,
    GX_LIGHT1,
    GX_LIGHT2 = 4,
    GX_LIGHT3 = 8,
    GX_LIGHT4 = 16,
    GX_LIGHT5 = 32,
    GX_LIGHT6 = 64,
    GX_LIGHT7 = 128,
    GX_MAX_LIGHT = 256,
};

enum class GXDiffuseFn
{
    GX_DF_NONE = 0,
    GX_DF_SIGN,
    GX_DF_CLAMP,
};

enum class GXAttnFn
{
    GX_AF_SPEC = 0,
    GX_AF_SPOT,
    GX_AF_NONE,
};

enum class GXTexOffset
{
    GX_TO_ZERO = 0,
    GX_TO_SIXTEENTH,
    GX_TO_EIGHTH,
    GX_TO_FOURTH,
    GX_TO_HALF,
    GX_TO_ONE,
    GX_MAX_TEXOFFSET,
};

struct GXTexObj
{
    uint32_t padding[8];
} __attribute__((__packed__));

static_assert(sizeof(GXTexObj) == 0x20);

// Inlined GX functions
static inline void GXPosition1x16(const uint16_t x)
{
    writeGatherPipe.u16 = x;
}

static inline void GXPosition3f32(const float x, const float y, const float z)
{
    writeGatherPipe.f32 = x;
    writeGatherPipe.f32 = y;
    writeGatherPipe.f32 = z;
}

extern "C"
{
    void GXSetVtxDesc(GXAttribute attribute, GXAttributeType type);
    void GXClearVtxDesc();

    void GXSetVtxAttrFmt(GXVtxFmt vtxfmt,
                         GXAttribute attribute,
                         GXComponentContents contents,
                         GXComponentType type,
                         uint8_t fracBits);

    void GXSetTexCoordGen2(GXTexCoordID dstCoord,
                           GXTexGenType func,
                           GXTexGenSrc srcParam,
                           uint32_t mtx,
                           bool normalize,
                           uint32_t postMtx);

    void GXSetNumTexGens(uint8_t nTexGens);
    void GXBegin(GXPrimitive type, GXVtxFmt vtxFmt, uint16_t nVerts);
    void GXSetLineWidth(uint8_t width, GXTexOffset texOffsets);
    void GXSetCullMode(GXCullMode mode);
    void GXSetCopyClear(uint8_t clearColor[4], uint32_t clearZ);
    void GXCopyDisp(void *dest, bool clear);
    void GXSetChanMatColor(GXChannelID chan, uint8_t matColor[4]);
    void GXSetNumChans(uint8_t nChans);

    void GXSetChanCtrl(GXChannelID channel,
                       bool enable,
                       GXColorSrc ambientColor,
                       GXColorSrc materialColor,
                       GXLightID lightMask,
                       GXDiffuseFn diffuseFn,
                       GXAttnFn attnFn);

    void GXInitTexObj(GXTexObj *obj,
                      void *image,
                      uint16_t width,
                      uint16_t height,
                      GXTexFmt format,
                      GXTexWrapMode wrapS,
                      GXTexWrapMode wrapT,
                      bool mipMap);

    void GXLoadTexObj(const GXTexObj *obj, GXTexMapID id);
    void GXSetTevOp(GXTevStageID id, GXTevMode mode);

    void GXSetAlphaCompare(GXCompare func0,
                           uint8_t reference0,
                           GXAlphaOperation operation,
                           GXCompare func1,
                           uint8_t reference1);

    void GXSetTevOrder(GXTevStageID stage, GXTexCoordID coord, GXTexMapID map, GXChannelID color);
    void GXSetNumTevStages(uint8_t nStages);

    void GXSetBlendMode(GXBlendMode type, GXBlendFactor srcFactor, GXBlendFactor dstFactor, GXLogicOperation operation);

    void GXSetColorUpdate(bool updateEnable);
    void GXSetAlphaUpdate(bool updateEnable);

    void GXSetZMode(bool compareEnable, GXCompare func, bool updateEnable);
    void GXSetProjection(const mtx44 mtx, GXProjectionType type);
    void GXLoadPosMtxImm(const mtx34 mtx, GXTexMtxType id);
    void GXSetCurrentMtx(uint32_t id);
    void GXLoadTexMtxImm(const mtxUnk4 mtx, uint32_t id, GXTexMtxType type);

    void GXSetViewport(float posX, float posY, float width, float height, float nearDepth, float farDepth);

    void GXSetScissor(uint32_t posX, uint32_t posY, uint32_t width, uint32_t height);
}

#endif
