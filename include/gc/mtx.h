#ifndef GC_MTX_H
#define GC_MTX_H

#include "gc/types.h"

#include <cstdint>

extern "C"
{
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
    void PSMTXTransApply(const mtx34 src, mtx34 dst, float x, float y, float z);
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

    void C_MTXOrtho(mtx44 mtx,
                    float topEdge,
                    float bottomEdge,
                    float leftEdge,
                    float rightEdge,
                    float nearClipPlain,
                    float farClipPlain);

    // PSMTX44Copy
    // PSMTX44Concat
    // PSMTX44Trans
    // PSMTX44Scale
    // PSMTX44MultVec
    void PSVECAdd(const Vec3 *a, const Vec3 *b, Vec3 *ab);
    // PSVECSubtract
    void PSVECScale(const Vec3 *src, Vec3 *dst, float scale);
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

#endif
