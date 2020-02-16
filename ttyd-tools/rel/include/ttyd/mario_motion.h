#pragma once

#include <cstdint>

namespace ttyd::mario_motion {

enum MarioMotions : uint16_t
{
	kStay 			= 0,
	kWalk 			= 1,
	kDash 			= 2,
	kJump 			= 3,
	kJumpNPC 		= 4,
	kJumpSw 		= 5,
	kJumpStand 		= 6,
	kJump2 			= 7,
	kJump3 			= 8,
	kJumpSmall 		= 9,
	kFall 			= 10,
	kFall2 			= 11,
	kUpstairs 		= 12,
	kLand 			= 13,
	kTalk 			= 14,
	kGetItem 		= 15,
	kHip 			= 16,
	kHip2 			= 17,
	kHammer 		= 18,
	kHammer2 		= 19,
	kJabara 		= 20,
	kSlit 			= 21,
	kRoll 			= 22,
	kKaze 			= 23,
	kPlane 			= 24,
	kShip 			= 25,
	kYoshi 			= 26,
	kCloud 			= 27,
	kVivian 		= 28,
	kDokan 			= 29,
	kGrasp 			= 30,
	kDamage 		= 31,
	kDamageToge 	= 32,
	kBottomless 	= 33,
	kForceReset 	= 34,
	kShadow 		= 35,
	kPartyUse 		= 36, // Dummied out
	kKpaSwim 		= 37,
	kKpaPowUp 		= 38,
	kKpaPowDown 	= 39,
	kDummy 			= 40, // Dummied out
};

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
void marioChgMot(uint16_t motionId);
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