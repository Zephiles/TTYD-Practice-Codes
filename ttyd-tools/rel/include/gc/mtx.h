#pragma once

#include <cstdint>

namespace gc::mtx {

typedef float mtx34[3][4];
typedef float mtx44[4][4];
typedef float mtxUnk4[][4];

struct Vec3
{
    float x, y, z;
} __attribute__((__packed__));

static_assert(sizeof(Vec3) == 0xC);

extern "C" {

// PSMTXIdentity
// PSMTXCopy
void PSMTXConcat(const mtx34 a, const mtx34 b, mtx34 ab);
// PSMTXInverse
// PSMTXInvXpose
// PSMTXRotRad
// PSMTXRotTrig
// __PSMTXRotAxisRadInternal
// PSMTXRotAxisRad
void PSMTXTrans(mtx34 mtx, float x, float y, float z);
void PSMTXTransApply(const mtx34 source, mtx34 destination, float x, float y, float z);
void PSMTXScale(mtx34 mtx, float xScale, float yScale, float zScale);
// PSMTXScaleApply
// C_MTXLookAt
// C_MTXLightFrustum
// C_MTXLightPerspective
// C_MTXLightOrtho
// PSMTXMultVec
// PSMTXMultVecSR
// C_MTXFrustum
// C_MTXPerspective

void C_MTXOrtho(mtx44 mtx, float topEdge, float bottomEdge, 
    float leftEdge, float rightEdge, float nearClipPlain, float farClipPlain);

// PSMTX44Copy
// PSMTX44Concat
// PSMTX44Trans
// PSMTX44Scale
// PSMTX44MultVec
void PSVECAdd(const Vec3 *a, const Vec3 *b, Vec3 *ab);
// PSVECSubtract
void PSVECScale(const Vec3 *source, Vec3 *destination, float scale);
// PSVECNormalize
// PSVECSquareMag
// PSVECMag
// PSVECDotProduct
// PSVECCrossProduct
// C_VECHalfAngle
// C_VECReflect
// PSVECSquareDistance
// PSVECDistance

}

}