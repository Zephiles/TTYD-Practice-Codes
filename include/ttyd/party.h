#ifndef TTYD_PARTY_H
#define TTYD_PARTY_H

#include "gc/types.h"
#include "ttyd/mario.h"

#include <cstdint>

#define TOTAL_PARTNERS 7
#define TOTAL_FOLLOWERS 12 // Includes all partners as well

#define TOTAL_PARTNERS_FLOAT 7.f
#define TOTAL_FOLLOWERS_FLOAT 12.f // Includes all partners as well

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

// Taken from NWPlayer123's decomp:
// https://github.com/NWPlayer123/PaperMario2/blob/master/include/party.h
struct PartyEntry
{
    uint32_t flags;
    uint32_t flags2;
    uint32_t field_0x8;
    int32_t field_0xC;
    int32_t field_0x10;
    int32_t field_0x14;
    uint8_t field_0x18[0x2F - 0x18];
    int8_t currentSlotId;
    uint8_t field_0x30;
    PartyMembers currentMemberId;
    uint8_t field_0x32[0x39 - 0x32];
    uint8_t wActionState;
    uint8_t field_0x3A[0x58 - 0x3A];
    Vec3 partnerPosition;
    uint8_t field_0x64[0xFC - 0x64];
    float wPartnerAngleCurrent;
    float wPartnerAngle;
    uint8_t field_0x104[0x10C - 0x104];
    float wPartnerDirectionCurrent;
    float wPartnerDirection;
    uint8_t field_0x114[0x160 - 0x114];
    Player *playerPtr;
    int32_t camId;
    uint8_t field_0x168[0x188 - 0x168];
} __attribute__((__packed__));

static_assert(sizeof(PartyEntry) == 0x188);

extern "C"
{
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
    void partyRun(PartyEntry *partyPtr);
    // partyCtrlOn
    // partyCtrlOff
    // partyChgPaper
    // partyChgPose
    // partyChgPoseId
    // partyPaperLightOff
    // partyPaperOff
    // partyPaperOn
    // anotherPartyGetPtr
    PartyEntry *partyGetPtr(PartySlotId id);
}

#endif