#pragma once

namespace ttyd::party {
	
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
// unk_JP_US_PAL_029_800b7bcc
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
// unk_JP_US_PAL_030_800ba2dc
// unk_JP_US_PAL_031_800ba72c
// unk_JP_US_PAL_032_800ba7a8
// partyChgRunMode
// partyUsePost
// partyReInit
// partyInit
// partyEntry2Pos
// partyEntry2Hello
// partyEntry2
// partyEntryMain
// partyGetHeight
// yoshiSetColor
// partyKill2
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
uint32_t partyGetPtr(uint32_t marioGetPartyIdResult);

}

}