#pragma once

#include <cstdint>

#include <ttyd/party.h>

namespace ttyd::mario_party {

extern "C" {

// partyGetTechLv
// partyGetHp
// partyChkJoin
void partyLeft(ttyd::party::PartyMembers id);
void partyJoin(ttyd::party::PartyMembers id);
int32_t marioGetExtraPartyId();
int32_t marioGetPartyId();
// marioGetParty
void marioPartyKill();
int32_t marioPartyGoodbye();
int32_t marioPartyHello(ttyd::party::PartyMembers id);
int32_t marioPartyEntry(ttyd::party::PartyMembers id);
// marioUseParty

}

}