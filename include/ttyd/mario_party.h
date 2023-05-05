#ifndef TTYD_MARIO_PARTY_H
#define TTYD_MARIO_PARTY_H

#include "ttyd/party.h"

#include <cstdint>

extern "C"
{
    // partyGetTechLv
    // partyGetHp
    bool partyChkJoin(PartyMembers id);
    void partyLeft(PartyMembers id);
    void partyJoin(PartyMembers id);
    PartySlotId marioGetExtraPartyId();
    PartySlotId marioGetPartyId();
    // marioGetParty
    void marioPartyKill();
    int32_t marioPartyGoodbye();
    PartySlotId marioPartyHello(PartyMembers id);
    PartySlotId marioPartyEntry(PartyMembers id);
    // marioUseParty
}

#endif
