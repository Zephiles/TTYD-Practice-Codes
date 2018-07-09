#pragma once

namespace ttyd::mario_party {

extern "C" {

// partyGetTechLv
// partyGetHp
// partyChkJoin
void partyLeft(uint32_t id);
void partyJoin(uint32_t id);
// marioGetExtraPartyId
uint32_t marioGetPartyId();
// marioGetParty
// marioPartyKill
// marioPartyGoodbye
// marioPartyHello
// marioPartyEntry
// unk_JP_US_PAL_062
// marioUseParty

}

}