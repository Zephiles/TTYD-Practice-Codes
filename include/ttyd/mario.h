#ifndef TTYD_MARIO_H
#define TTYD_MARIO_H

#include "gc/types.h"
#include "ttyd/mario_motion.h"
#include "ttyd/party.h"

#include <cstdint>

enum class PartyMembers : uint8_t;

enum class MarioCharacters : int8_t
{
    kMario = 0,
    kPeach,
    kBowser,
};

// The Player struct was defined using the US version, but has been adjusted to work with the JP version as well
// The Player struct is also 100% identical between US and EU
struct Player
{
    uint32_t flags1;
    uint32_t flags2;
    uint32_t unk_8;
    uint32_t flags3;
    uint32_t flags4;
    uint8_t gap_14[4];
    const char *animName;
    uint32_t unk_1c;
    uint32_t unk_20;
    uint32_t unk_24;
    uint16_t unk_28;
    uint16_t pad_2a;
    uint16_t unk_2c;
    MarioMotion currentMotionId;
    MarioMotion prevMotionId;
    uint16_t pad_32;
    uint32_t wMapTime;
    uint8_t unk_38;
    int8_t wKey;
    uint16_t unk_3a;
    MarioCharacters characterId;
    int8_t colorId;
    uint8_t unk_3e;
    int8_t wWallTimer;
    uint8_t wMotionTimer;
    uint8_t wMotionCounter;
    uint16_t pad_42;
    uint32_t unk_44;
    uint32_t wYawnTimer;
    uint16_t wInvincibleTimer;
    int16_t unk_4e;
    uint16_t wAirTimer;
    uint16_t pad_52;
    uint8_t gap_54[40];
    float wJumpVelocityY;
    float wJumpAccelerationY;
    float unk_84;
    float unk_88;
    Vec3 playerPosition;
    Vec3 wModelPosition;
    Vec3 wAnimPosition;
    uint8_t gap_b0[20];
    float wRotationY;
    Vec3 wSizeVector;
    Vec3 wLastGroundPosition;
    Vec3 unkPosition;
    Vec3 wCameraPosition;
    uint8_t gap_f8[24];
    Vec3 wPlayerPosition2;
    uint8_t gap_11c[16];
    float wJumpApopasis;
    uint32_t unk_130;
    uint32_t wRenderFlags;
    uint8_t gap_138[16];
    float wCamZoom;
    float wCamPan;
    uint8_t gap_150[48];
    float wPlayerBaseSpeed;
    float unk_184;
    float unk_188;
    float wPlayerEffectiveSpeed;

#ifndef TTYD_JP
    float unk_190;
#endif

    float wControlStickSensitivity;
    float wControlStickAngle;
    float unk_19c;
    float wPlayerAngleCurrent;
    float wPlayerAngle;
    float unk_1a8;
    float wPlayerDirectionCurrent;
    float wPlayerDirection;
    uint32_t unk_1b4;
    Vec3 wPlayerCollisionBox;
    Vec3 wPlayerCollisionRelated;
    uint8_t gap_1d0[8];
    void *touchNpcPtr;
    uint8_t gap_1dc[8];
    void *wObjInteract;
    void *wObjStandOn;
    void *wObjJumpFrom;
    void *wUnkObj1;
    void *wUnkObj2;
    void *wUnkObj3;
    void *wUnkObj4;
    void *wUnkObj5;
    void *wObjHammer;
    void *wObjHazardRespawn;
    void *wUnkObj6;
    uint8_t gap_210[24];
    float playerGravity;
    uint8_t gap_22c[24];
    uint8_t unk_244;
    PartyMembers partnerId;
    PartyMembers followerId;
    PartyMembers prevPartnerId;
    PartyMembers prevFollowerId;
    uint8_t unk_249;
    uint16_t wPauseButtonBuffer;
    uint16_t unk_24c;
    uint16_t unk_24e;
    uint16_t unk_250;
    uint8_t wStickDir1;
    uint8_t wStickDir2;
    uint8_t wSubStickDir1;
    uint8_t wSubStickDir2;
    uint8_t wPauseLeftTrigger;
    uint8_t wPauseRightTrigger;
    uint8_t unk_258;
    uint8_t unk_259;
    uint16_t unk_25a;
    uint32_t unk_25c;

#ifdef TTYD_JP
    uint8_t gap_260[68];
#else
    uint8_t gap_260[88];
#endif

    float wMultiVal1;
    float wYoshiHoverHeight;
    float wCamVal1;
    float wHazardSpeed;
    uint8_t gap_2c8[24];
    uint32_t wbReloadMapOnBottomless;
    Vec3 bottomlessResetPosition;
    uint32_t unk_2f0;
    uint32_t unk_2f4;
} __attribute__((__packed__));

#ifdef TTYD_JP
static_assert(sizeof(Player) == 0x2E0);
#else
static_assert(sizeof(Player) == 0x2F8);
#endif

inline bool peachInXNautForm(const Player *marioPtr)
{
    return (marioPtr->flags4 >> 13) & 1U;
}

extern "C"
{
    // toFrontPose
    // toRearPose
    // marioDisp
    // marioDispBlur
    // marioDispBlurSub
    // marioPreDisp
    // marioRearAnime
    // marioMakeDispDir
    // marioGetScale
    // marioChkInScreen
    // marioGetScreenPos
    // marioPaperLightOff
    // marioPaperOff
    // marioPaperOn
    // marioSetPaperAnimeLocalTime
    // marioChgPaper
    // marioChgPoseTime
    // marioChgPose
    // toDotMarioPose
    // marioChgEvtPose
    // marioAnimeId
    // marioChkPushAnime
    // marioMoveMain
    // marioMove
    // marioCtrlOff2Main
    // marioMain
    // marioReInit
    // marioInit
    // marioPoseInit
    // marioSoundInit
    // marioOfsRotReset
    // marioReset
    // marioResetHitObj
    // marioChkSts
    // marioBgmodeOff
    // marioBgmodeOn
    // marioBgmodeChk
    // marioSetMutekiTime
    int32_t marioKeyOn();
    int32_t marioKeyOff();
    // marioCtrlOn2
    // marioCtrlOff2
    // marioCtrlOn
    // marioCtrlOff
    int32_t marioKeyOffChk();
    // marioCtrlOffChk
    // marioFBattlePost
    // marioFBattlePrepare
    // N_marioChkUseParty
    // marioChkCtrl
    bool marioChkKey();
    // marioCheckMenuDisable
    // marioCaseEventValidChk
    // marioItemGetOk
    // marioItemGetChk
    // marioItemGetDisable
    // marioEntry
    void marioSetSpec();
    // marioSetFamicomMode
    void marioSetCharMode(MarioCharacters mode);
    int32_t marioGetColor();
    Player *marioGetPtr();

    // mario_str_Z_1
    // mario_str_S_1
    // mario_str_W_1
    // mario_str_R_1
    // mario_str_U_1
    // mario_str_J_1
    // mario_str_S_2
    // mario_str_U_2
    // mario_str_J_2
    // mario_str_S_3
    // mario_str_U_3
    // mario_str_J_3
    // mario_str_S_4
    // mario_str_U_4
    // mario_str_J_4
    // mario_dat
    // mario_float_1p2
    // mario_float_2
    // mario_float_1
    // mario_float_neg1
    // mario_float_90
    // mario_float_270
    // mario_float_deg2rad
    // mario_float_180
    // mario_float_0
    // mario_float_0p75
    // mario_float_6p2832
    // mario_float_360
    // mario_float_3p1416
    // mario_float_1p5708
    // mario_float_4p7124
    // mario_float_5000
    // mario_float_10000
    // mario_float_13p333
    // mario_float_30
    // mario_float_14
    // mario_float_6
    // mario_float_0p6
    // mario_float_neg2
    // mario_float_0p1
    // mario_float_290
    // mario_float_70
    // mario_float_0p5
    // mario_float_250
    // mario_float_190
    // mario_float_350
    // mario_float_10
    // mario_float_170
    // mario_float_0p3
    extern float mario_float_0p125;
    extern float mario_float_20;
    extern float mario_float_210;
    // mario_float_200
    // mario_float_0p015
    // mario_float_0p01
    // mario_float_0p25
    // mario_float_0p4
    // mario_float_400
    // mario_float_2p25
    // mario_float_47
    // mario_float_37
    // mario_float_25p9
    // mario_str_T_1
}

#endif
