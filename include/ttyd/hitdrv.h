#ifndef TTYD_HITDRV_H
#define TTYD_HITDRV_H

#include "gc/mtx.h"

#include <cstdint>

enum HitEntryFlags
{
    HitEntryFlags_ShipOnly = 0x80,

    HitEntryFlags_MaterialShallowWater = 0x100,
    HitEntryFlags_WaterHazard = 0x200,
    HitEntryFlags_Unk1Hazard = 0x400,
    HitEntryFlags_Unk2Hazard = 0x800,
    HitEntryFlags_UnkMaterial3_waterRelated = 0x1000,

    HitEntryFlags_UnkMaterial1 = 0x100000,
    HitEntryFlags_UnkMaterial2 = 0x200000,
    HitEntryFlags_MaterialMetal = 0x400000,

    HitEntryFlags_Bero = 0x800000,

    HitEntryFlags_DamageReturnSet = 0x40000000,
};

struct HitCheckQuery
{
    uint32_t bCurrentHitSingleSided;
    uint32_t wUser0;
    uint32_t wUser1;
    Vec3 targetPosition;
    Vec3 targetDirection;
    Vec3 hitPosition;
    Vec3 hitNormal;
    float inOutTargetDistance;
} __attribute__((__packed__));

struct HitEntryTriangleData
{
    Vec3 worldSpaceVertex0Position;
    Vec3 worldSpaceVertex1Position;
    Vec3 worldSpaceVertex2Position;
    Vec3 edgeV0V2;
    Vec3 edgeV1V0;
    Vec3 edgeV2V1;
    Vec3 realNormal;
} __attribute__((__packed__));

struct HitDamageReturn
{
    const char *mapObjName;
    Vec3 returnPosition;
} __attribute__((__packed__));

struct HitEntry
{
    uint16_t flags;

    uint8_t pad_2[0x2];

    uint32_t attributes;
    void *pSourceJoint; // Reference to unimplemented type MapDataFileJoint
    mtx34 localSpaceToWorldSpaceMatrix;
    mtx34 wTransformMtx_2;

    uint8_t gap_6c[0x30];

    Vec3 localSpaceOrigin;
    uint16_t triangleDataCount;
    int16_t mapSubgroupIndex;
    HitEntryTriangleData *pTriangleData;
    HitDamageReturn *pDamageReturn;

    uint8_t gap_b4[0xc];

    Vec3 worldSpaceOrigin;
    float worldSpaceBboxRadius;
    void *pMobj; // Reference to unimplemented type MobjEntry
    HitEntry *pParent;
    HitEntry *pChild;
    HitEntry *pNext;
    HitEntry *pNextActive;
} __attribute__((__packed__));

static_assert(sizeof(HitCheckQuery) == 0x40);
static_assert(sizeof(HitEntryTriangleData) == 0x54);
static_assert(sizeof(HitDamageReturn) == 0x10);
static_assert(sizeof(HitEntry) == 0xE4);

typedef int32_t (*PFN_HitFilterFunction)(HitCheckQuery *query, HitEntry *hit);

extern "C"
{
    void hitInit();
    void hitReInit();

    // local: _hitEnt
    // local: hitEntrySub
    HitEntry *hitEntry(void *pJoint, mtx34 *mat, int supgroupIndex);
    HitEntry *hitEntryMOBJ(void *pJoint, mtx34 *mat);
    void hitDelete(const char *mapobjName);

    void hitMain();

    void hitObjFlagOn(const char *mapobjName, uint16_t flags);
    void hitObjFlagOff(const char *mapobjName, uint16_t flags);
    // local: hitFlgOn
    // local: hitFlgOff
    void hitGrpFlagOn(const char *mapobjName, uint16_t flags);
    void hitGrpFlagOff(const char *mapobjName, uint16_t flags);
    void hitObjAttrOn(const char *mapobjName, uint32_t attr);
    void hitObjAttrOff(const char *mapobjName, uint32_t attr);
    // local: hitAtrOn
    // local: hitAtrOff
    void hitGrpAttrOn(const char *mapobjName, uint32_t attr);
    void hitGrpAttrOff(const char *mapobjName, uint32_t attr);

    void hitReCalcMatrix(HitEntry *hit, mtx34 *mat);
    // local: hitReCalcMatrix2
    // local: hitCalcVtxPosition

    HitEntry *hitCheckVecFilter(HitCheckQuery *pQuery, PFN_HitFilterFunction filterFunction);

    HitEntry *hitCheckFilter(PFN_HitFilterFunction filterFunction,
                             float *pHitX,
                             float *pHitY,
                             float *pHitZ,
                             float *pInOutDistance,
                             float *pHitNX,
                             float *pHitNY,
                             float *pHitNZ,
                             float x,
                             float y,
                             float z,
                             float nx,
                             float ny,
                             float nz);

    // local: checkTriVec_xz
    int hitCheckVecHitObjXZ(HitCheckQuery *pQuery, HitEntry *hit);

    // local: chkFilterAttr
    HitEntry *hitCheckAttr(uint32_t bitMask,
                           float *pOutPositionX,
                           float *pOutPositionY,
                           float *pOutPositionZ,
                           float *pInOutDistance,
                           float *pOutNormalX,
                           float *pOutNormalY,
                           float *pOutNormalZ,
                           float targetPosX,
                           float targetPosY,
                           float targetPosZ,
                           float targetNormX,
                           float targetNormY,
                           float targetNormZ);

    // hitCheckSphereFilter

    HitEntry *hitNameToPtr(const char *mapobjName);

    // local: hitObjGetPosSub
    void hitObjGetPos(const char *mapobjName, Vec3 *pOutPosition);
    void hitObjGetNormal(const char *mapobjName, Vec3 *pOutNormal);
    char *hitGetName(HitEntry *hit);
    uint32_t hitGetAttr(HitEntry *hit);

    // local: hitDamageReturnSet
    void hitGrpDamageReturnSet(const char *mapobjName, HitDamageReturn *pDamageReturn);
    Vec3 *hitGetDamageReturnPos(HitEntry *hit);

    void hitBindMapObj(const char *mapobjName, const char *bindMapObjName);
    void hitBindUpdate(const char *mapobjName);
}

#endif
