#pragma once

#include <cstdint>

namespace ttyd::party {

enum class PartySlotId : int32_t
{
    kNone = -1,
    kParty,
    kFollower,
};

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
// L_partyClearCont
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
// L_allPartyRideOn
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
// N_partyForceChgRunMode
// partyChgRunMode
// N__partyChgRunMode
// partyUsePost
// partyReInit
// partyInit
// partyEntry2Pos
PartySlotId partyEntry2Hello(PartyMembers id);
// partyEntry2
// partyEntryMain
// partyGetHeight
// yoshiSetColor
int32_t partyKill2(PartySlotId id);
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
void *partyGetPtr(PartySlotId id);

}

}