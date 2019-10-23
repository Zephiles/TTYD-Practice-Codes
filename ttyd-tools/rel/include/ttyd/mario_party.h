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
ttyd::party::PartySlotId marioGetExtraPartyId();
ttyd::party::PartySlotId marioGetPartyId();
// marioGetParty
void marioPartyKill();
int32_t marioPartyGoodbye();
ttyd::party::PartySlotId marioPartyHello(ttyd::party::PartyMembers id);
ttyd::party::PartySlotId marioPartyEntry(ttyd::party::PartyMembers id);
// marioUseParty

}

}