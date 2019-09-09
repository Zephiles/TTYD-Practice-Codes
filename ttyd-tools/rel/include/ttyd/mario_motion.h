#pragma once

#include <cstdint>

namespace ttyd::mario_motion {

extern "C" {

// mot_forceReset
// mot_bottomless
// mot_dummy
// mot_partyUse
// mot_shadow
// mot_kaze
// marioChgMotJump2
// marioRollChgChk
// marioChkRub
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
// marioChgMot
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

}