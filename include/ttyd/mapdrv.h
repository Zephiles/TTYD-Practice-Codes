#ifndef TTYD_MAPDRV_H
#define TTYD_MAPDRV_H

#include "gc/types.h"
#include "ttyd/hitdrv.h"

#include <cstdint>

// Taken from NWPlayer123's decomp:
// https://github.com/NWPlayer123/PaperMario2/blob/master/include/drv/mapdrv.h
struct MapJointPart
{
    void *material;
    void *mesh;
} __attribute__((__packed__));

struct MapJointDrawMode
{
    uint8_t unk0[0x1 - 0x0];
    uint8_t cullMode;
    uint8_t unk2[0x10 - 0x2];
} __attribute__((__packed__));

struct MapJoint
{
    const char *name;
    const char *type;
    MapJoint *parent;
    MapJoint *child;
    MapJoint *next;
    MapJoint *prev;
    Vec3 scale;
    Vec3 rotation;
    Vec3 translation;
    Vec3 bboxMin;
    Vec3 bboxMax;
    uint8_t field_0x54[0x58 - 0x54];
    MapJointDrawMode *drawMode;
    int32_t partCount;
    MapJointPart parts[];
} __attribute__((__packed__));

struct MapFileHeader
{
    int32_t fileSize;
    int32_t dataSize;
    int32_t relCount;   // TODO: rename?
    int32_t chunkCount; // TODO: rename?
    int32_t unused;
    uint8_t reserved[0xC];
} __attribute__((__packed__));

struct MapFileChunk
{
    uint32_t offset; // relative offset into data
    uint32_t string; // relative offset into strings
} __attribute__((__packed__));

struct MapHeader
{
    MapFileHeader file;
    void *data; // TODO re-type?
    void *rel;  // TODO: re-type?
    MapFileChunk *chunks;
    void *unused;
    const char *strings;
    uint8_t field_0x34[0x3C - 0x34];
    int32_t field_0x3C; // dispFlags something?
    uint8_t field_0x40[0x44 - 0x40];
} __attribute__((__packed__));

struct MapObject
{
    uint8_t field_0x0[0x8 - 0x0];
    MapJoint *joints;
    uint8_t field_0xC[0x1C - 0xC];
    mtx34 modelWorldMtx;
    uint8_t field_0x4C[0xAC - 0x4C];
    mtx34 unkAC;
    uint8_t field_0xDC[0xE0 - 0xDC];
    MapObject *parent;
    MapObject *child;
    MapObject *next;
    uint8_t field_0xEC[0x134 - 0xEC];
} __attribute__((__packed__));

struct MapEntryAnimData
{
    uint8_t field_0x0[0x20 - 0x0];
} __attribute__((__packed__));

struct MapEntry
{
    int32_t count;
    uint16_t flags;
    char name[16];
    uint8_t field_0x16[0x38 - 0x16];
    MapHeader header;
    void *dat; // ./map/%s/map.dat
    uint32_t datSize;
    void *tpl; // ./map/%s/texture.tpl
    uint32_t tplSize;
    uint8_t field_0x8C[0xA8 - 0x8C];
    MapObject *rootMapObj;
    HitEntry *rootHitObj;
    uint8_t field_0xB0[0x150 - 0xB0];
    int32_t numJoints;  // TODO: rename? see: hitNumJoints
    MapObject *objects; // TODO: rename? see: hitNumJoints
    int32_t hitNumJoints;
    HitEntry *hitObjects;
    uint8_t field_0x160[0x164 - 0x160];
    MapEntryAnimData *animData;
    uint8_t field_0x168[0x178 - 0x168];
} __attribute__((__packed__));

struct MapWork
{
    MapEntry entries[2];
    int32_t unk_2F0;
} __attribute__((__packed__));

static_assert(sizeof(MapJointPart) == 0x8);
static_assert(sizeof(MapJointDrawMode) == 0x10);
static_assert(sizeof(MapFileHeader) == 0x20);
static_assert(sizeof(MapFileChunk) == 0x8);
static_assert(sizeof(MapHeader) == 0x44);
static_assert(sizeof(MapObject) == 0x134);
static_assert(sizeof(MapEntryAnimData) == 0x20);
static_assert(sizeof(MapEntry) == 0x178);
static_assert(sizeof(MapWork) == 0x2F4);

extern "C"
{
    // mapObjGetFlushColor
    // mapGrpSetFlushColor
    // mapObjSetFlushColor
    // mapGrpFlushOff
    // mapGrpFlushOn
    // mapObjFlushOff
    // mapObjFlushOn
    // mapSetTevCallback
    // mapTestXLU
    // spline_getvalue
    // spline_maketable
    // mapSetProjMtx
    // mapSetProjTexObj
    // mapGrpSetColor
    // setColor
    // mapObjSetColor
    // mapBlendOff2
    // mapBlendOff
    // mapGetBlend2
    // mapGetBlend
    // mapSetBlend2
    // mapSetBlend
    // mapFogOff
    // mapFogOn
    // mapGetFog
    // mapSetFog
    // mapObjGetPos
    // mapObjGetPosSub
    // mapObjScale
    // mapObjTranslate
    // mapObjRotate
    // mapGrpClearOffScreen
    // mapObjClearOffScreen
    // mapGrpSetOffScreen
    // mapObjSetOffScreen
    // _setOffScrnId
    // mapGrpFlagOff
    // mapGrpFlagOn
    // mapFlgOff
    // mapFlgOn
    // mapObjFlagOff
    // mapObjFlagOn
    // mapGetMapObj
    // mapSearchDmdJoint
    // mapSearchDmdJointSub2
    // mapSearchDmdJointSub
    // mapSetPolygonVtxDesc
    // mapSetPolygon
    // mapSetMaterialLight
    // mapResetPaperAmbColor
    // mapSetPaperAmbColor
    // mapSetMaterial
    // mapSetTextureMatrix
    // mapSetMaterialFog
    // mapSetMaterialLastStageBlend
    // mapSetMaterialTev
    // mapSetLight
    // mapDisp
    // mapDispMapGrp_bbox
    // mapDispMapObj_bbox
    // _mapDispMapGrp_NoMaterial
    // _mapDispMapObj_NoMaterial
    // test_kururing_mapdisp
    // mapDispMapGrp_off
    // mapDispMapObj_off
    // mapDispMapGrp
    // mapDispMapObj
    // _mapDispMapGrp
    // _mapDispMapObj
    // mapMain
    // mapReCalcMatrix
    // mapCalcAnimMatrix
    // bmapLoad
    // bmapUnLoad
    // mapLoad
    // mapPreLoad
    // mapUnLoad
    // _mapLoad
    // mapSetPlayRate
    // mapReStartAnimationAll
    // mapPauseAnimationAll
    // mapReStartAnimation
    // mapPauseAnimation
    // mapPlayAnimationLv
    // mapCheckAnimation
    // mapSearchAnmObj
    // makeDisplayList
    // mapEntry
    // mapEntrySub
    // _mapEnt
    // mapBuildTexture
    // mapInit
    // mapGetBoundingBox
    // mapGetJoints
    // mapGetJointsSub
    // mapGetActiveGroup
    MapWork *mapGetWork();
    // N_mapDispOn
    // N_mapDispOff
    // getMapDataDvdRoot
    // mapErrorEntry
}

#endif
