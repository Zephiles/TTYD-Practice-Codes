#pragma once

#include <cstdint>

namespace ttyd::party {

enum class PartyMembers : uint8_t
{
	kNone = 0,
	kGoombella,
	kKoops,
	kBobbery,
	kYoshi,
	kFlurrie,
	kVivian,
	kMsMowz,
	kEgg,
	kFlavio,
	kPunio,
	kFrankly,
	kGus,
	kGoombellaFollower,
	kKoopsFollower,
	kBobberyFollower,
	kYoshiFollower,
	kFlurrieFollower,
	kVivianFollower,
	kMsMowzFollower,
};

extern "C" {

// partySetFamicomMode
// partyShadowOff
// partyUpdateKeyData
// partyClearCont
// partyKeyOff
// partyKeyOn
// partyDisp
// partyPreDisp
// partyMakeDispDir
// partySetCamId
// partyInitCamId
// party_damage_return
// partyRideYoshiMain
// party_ride_yoshi_force_move
// party_force_ride_yoshi
// partyRideMain
// partyRideChk
// allPartyRideOff2
// allPartyRideOff
// allPartyForceRideOn
// getRidePos
// allPartyRideShip
// allPartyRidePlane
// partyDokanEnd
// partyDokanMain
// partyDokanInit
// partyGoodbyeMain
// partyGoodbyeInit
// partyHelloMain
// partyHello
// partyDoWork
// driveParty
// partyMain
// partyChgRunMode
// partyUsePost
// partyReInit
// partyInit
// partyEntry2Pos
int32_t partyEntry2Hello(PartyMembers id);
// partyEntry2
// partyEntryMain
// partyGetHeight
// yoshiSetColor
int32_t partyKill2(int32_t partyId);
// partyKill
// partyGoodbye
// partyEntryPos
// partyEntry
// partyEntryHello
// partySetForceMove
// partyStop
void partyRun(void *partyPtr);
// partyCtrlOn
// partyCtrlOff
// partyChgPaper
// partyChgPose
// partyChgPoseId
// partyPaperLightOff
// partyPaperOff
// partyPaperOn
// anotherPartyGetPtr
void *partyGetPtr(int32_t partyId);

}

}