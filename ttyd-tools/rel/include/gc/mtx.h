#pragma once

#include <cstdint>

namespace gc::mtx {

typedef float mtx34[3][4];
typedef float mtx44[4][4];
typedef float mtxUnk4[][4];

extern "C" {

// PSMTXIdentity
// PSMTXCopy
// PSMTXConcat
// PSMTXInverse
// PSMTXInvXpose
// PSMTXRotRad
// PSMTXRotTrig
// __PSMTXRotAxisRadInternal
// PSMTXRotAxisRad
// PSMTXTrans
// PSMTXTransApply
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
// PSVECAdd
// PSVECSubtract
// PSVECScale
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