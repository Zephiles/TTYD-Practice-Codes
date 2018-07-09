#include "mod.h"

#include <ttyd/mariost.h>
#include <ttyd/system.h>
#include <ttyd/mario_party.h>
#include <ttyd/win_party.h>

#include <cstdio>

extern uint32_t PauseMenuAddressesStart;
extern uint16_t AddPartnersCombo;
extern uint16_t RemovePartnersCombo;

namespace mod {

void Mod::addOrRemovePartners()
{
  uint32_t PauseMenuAddress  = *reinterpret_cast<uint32_t *>(PauseMenuAddressesStart);
  uint32_t CurrentTab = *reinterpret_cast<uint32_t *>(PauseMenuAddress + 0x40);
  
  if ((ttyd::mariost::marioStGetSystemLevel() == 15) && (CurrentTab == 1))
  {
    // Currently in the partners menu
    uint32_t ButtonInput = ttyd::system::keyGetButton(0);
    
    if ((ButtonInput & AddPartnersCombo) == AddPartnersCombo)
    {
      if (!addOrRemovePartnersDisable)
      {
        
        // Add all partners
        for (int i = 1; i <= 7; i++)
        {
          ttyd::mario_party::partyJoin(i);
        }
        
        // Reload party menu
        uint32_t *PauseMenuPointer = reinterpret_cast<uint32_t *>(PauseMenuAddress);
        ttyd::win_party::winPartyExit(PauseMenuPointer);
        ttyd::win_party::winPartyInit(PauseMenuPointer);
      }
      addOrRemovePartnersDisable = true;
    }
    else if ((ButtonInput & RemovePartnersCombo) == RemovePartnersCombo)
    {
      if (!addOrRemovePartnersDisable)
      {
        uint32_t CurrentMenuInside = *reinterpret_cast<uint32_t *>(PauseMenuAddress  + 0x24);
        if (CurrentMenuInside == 12)
        {
          // Currently inside partners menu
          uint32_t PartnerOnCursor = *reinterpret_cast<uint32_t *>(PauseMenuAddress  + 0x1D8);
          uint8_t PartnerToRemove;
          
          // Get proper partner value
          switch (PartnerOnCursor)
          {
            case 0: // Goombella
              PartnerToRemove = 1; break;
            case 1: // Koops
              PartnerToRemove = 2; break;
            case 2: // Flurrie
              PartnerToRemove = 5; break;
            case 3: // Yoshi
              PartnerToRemove = 4; break;
            case 4: // Vivian
              PartnerToRemove = 6; break;
            case 5: // Bobbery
              PartnerToRemove = 3; break;
            default: // case 6: Mowz
              PartnerToRemove = 7; break;
          }
          
          // Remove partner
          ttyd::mario_party::partyLeft(PartnerToRemove);
          
          // Reload party menu
          uint32_t *PauseMenuPointer = reinterpret_cast<uint32_t *>(PauseMenuAddress);
          ttyd::win_party::winPartyExit(PauseMenuPointer);
          ttyd::win_party::winPartyInit(PauseMenuPointer);
        }
      }
      addOrRemovePartnersDisable = true;
    }
    else
    {
      // Reset flag if no button combo is pressed/held
      addOrRemovePartnersDisable = false;
    }
  }
}

}