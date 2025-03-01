#ifndef TTYD_MARIO_MOTION_H
#define TTYD_MARIO_MOTION_H

#include <cstdint>

enum class MarioMotion : uint16_t
{
    kStay = 0,
    kWalk,
    kDash,
    kJump,
    kJumpNPC,
    kJumpSw,
    kJumpStand,
    kJump2,
    kJump3,
    kJumpSmall,
    kFall,
    kFall2,
    kUpstairs,
    kLand,
    kTalk,
    kGetItem,
    kHip,
    kHip2,
    kHammer,
    kHammer2,
    kJabara,
    kSlit,
    kRoll,
    kKaze,
    kPlane,
    kShip,
    kYoshi,
    kCloud,
    kVivian,
    kDokan,
    kGrasp,
    kDamage,
    kDamageToge,
    kBottomless,
    kForceReset,
    kShadow,
    kPartyUse, // Dummied out
    kKpaSwim,
    kKpaPowUp,
    kKpaPowDown,
    kDummy, // Dummied out
};

extern "C"
{
    // mot_forceReset
    // mot_bottomless
    // mot_dummy
    // mot_partyUse
    // mot_shadow
    // mot_kaze
    // marioChgMotJump2
    // marioRollChgChk
    // L_marioChkRub
    // marioChkItemMotion
    // marioChkTransform
    // marioChkJump
    // mot_getItem
    // mot_talk
    // marioLandOn
    // marioFall
    // marioJump
    // marioMakeJumpPara
    // marioGetFallSpd
    // marioSetFallPara
    // marioSetJumpPara
    // marioClearJumpPara
    // marioChgMot2
    // marioChgMotSub
    void marioChgMot(MarioMotion motionId);
    // marioMotion
    // marioBoots
    // marioChkTalkable
    // marioChkItemGetMotion
    // marioChgSmallJumpMotion
    // marioChgJumpStandMotion
    // marioChgRollMotion
    void marioChgShipMotion();
    // marioChgGetItemMotion
    // marioChgTalkMotion
    void marioChgStayMotion();
    // marioChgMoveMotion
}

#endif
