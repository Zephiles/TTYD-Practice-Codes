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
  uint32_t PartnerPointer = reinterpret_cast<uint32_t>(ttyd::party::partyGetPtr(ttyd::mario_party::marioGetPartyId()));
  
  if ((ButtonInput & TextStorageCombo) == (TextStorageCombo))
  {
    if ((!textStorageDisable) && (PartnerPointer != 0))
    {
      // A Partner is currently out
      *reinterpret_cast<uint8_t *>(PartnerPointer + 0x39) = 0;
    }
    textStorageDisable = true;
  }
  else if ((ButtonInput & TimeStopTextStorageCombo) == (TimeStopTextStorageCombo))
  {
    if ((!textStorageDisable) && (PartnerPointer != 0))
    {
      // A Partner is currently out
      *reinterpret_cast<uint8_t *>(PartnerPointer + 0x39) = 21;
    }
    textStorageDisable = true;
  }
  else
  {
    // Reset flag if no button combo is pressed/held
    textStorageDisable = false;
  }
}

}