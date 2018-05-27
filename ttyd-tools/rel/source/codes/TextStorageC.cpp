#include "mod.h"

#include <ttyd/system.h>
#include <ttyd/party.h>
#include <ttyd/mario_party.h>

#include <cstdio>

extern uint16_t TextStorageCombo;
extern uint16_t TimeStopTextStorageCombo;

namespace mod {

void Mod::textStorage()
{
  uint32_t ButtonInput = ttyd::system::keyGetButton(0);
  uint32_t PartnerPointer = ttyd::party::partyGetPtr(ttyd::mario_party::marioGetPartyId());
  
  if ((ButtonInput & TextStorageCombo) == (TextStorageCombo))
  {
    if (PartnerPointer != 0)
    {
      // A Partner is currently out
      *reinterpret_cast<uint8_t *>(PartnerPointer + 0x39) = 0;
    }
  }
  else if ((ButtonInput & TimeStopTextStorageCombo) == (TimeStopTextStorageCombo))
  {
    if (PartnerPointer != 0)
    {
      // A Partner is currently out
      *reinterpret_cast<uint8_t *>(PartnerPointer + 0x39) = 21;
    }
  }
}

}