#pragma once

#include <cstdint>

namespace ttyd::mario_motion {

enum MarioMotions : uint16_t
{
	kNone 						= 0,
	kWalkSlow 					= 1,
	kRun 						= 2,
	kJumpRising 				= 3,
	kJumpLandOnEnemyStillInAir 	= 4,
	kBounceOffSwitch 			= 5,
	kBounceOffSpring 			= 6,
	kEnterOrExitArea 			= 8,
	kFall 						= 10,
	kWalkOffPlatform 			= 11,
	kClipOntoPlatform 			= 12,
	kTalkToNpcDirect 			= 14,
	kGetItem 					= 15,
	kSpinJumpInAir 				= 16,
	kSpinJumpHitGround 			= 17,
	kHammer 					= 18,
	kHammerSpin 				= 19,
	kSpringJump 				= 20,
	kPaperMode 					= 21,
	kTubeMode 					= 22,
	kPlaneMode 					= 24,
	kBoatMode 					= 25,
	kUseYoshi 					= 26,
	kUseFlurrie 				= 27,
	kUseVivian 					= 28,
	kEnterPipe 					= 29,
	kFallInWater 				= 31,
	kTouchHazard 				= 32,
	kRespawnInBackground 		= 33,
	kRespawnNormal 				= 34,
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