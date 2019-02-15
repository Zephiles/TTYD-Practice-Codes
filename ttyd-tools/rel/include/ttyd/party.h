#pragma once

#include <cstdint>

namespace ttyd::party {

enum class PartyMembers : uint8_t
{
	Goombella = 1,
	Koops,
	Bobbery,
	Yoshi,
	Flurrie,
	Vivian,
	MsMowz,
	Egg,
	Flavio,
	Punio,
	Frankly,
	Gus,
	GoombellaFollower,
	KoopsFollower,
	BobberyFollower,
	YoshiFollower,
	FlurrieFollower,
	VivianFollower,
	MsMowzFollower,
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
// partyRun
// partyCtrlOn
// partyCtrlOff
// partyChgPaper
// partyChgPose
// partyChgPoseId
// partyPaperLightOff
// partyPaperOff
// partyPaperOn
// anotherPartyGetPtr
void *partyGetPtr(uint32_t partyId);

}

}