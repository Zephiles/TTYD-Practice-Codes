#include "mod.h"

#include <ttyd/mariost.h>
#include <ttyd/system.h>
#include <ttyd/mario_pouch.h>
#include <ttyd/win_party.h>

#include <cstdio>

extern uint32_t PauseMenuAddressesStart;
extern uint16_t AddPartnersCombo;
extern uint16_t RemovePartnersCombo;

namespace mod {

void Mod::addOrRemovePartners()
{
  if (ttyd::mariost::marioStGetSystemLevel() == 15)
  {
    // Currently in the pause menu	
    uint32_t PauseMenuAddress  = *reinterpret_cast<uint32_t *>(PauseMenuAddressesStart);
    uint32_t CurrentTab = *reinterpret_cast<uint32_t *>(PauseMenuAddress  + 0x40);
    
    if (CurrentTab == 1)
    {
      // Currently in partners menu
      uint32_t ButtonInput = ttyd::system::keyGetButton(0);
      uint32_t PouchAddress = ttyd::mario_pouch::pouchGetPtr();
      
      if ((ButtonInput & AddPartnersCombo) == AddPartnersCombo)
      {
        if (!addOrRemovePartnersDisable)
        {
          for (int i = 0; i < 7; i++)
          {
            PouchAddress += 0xE;
            *reinterpret_cast<uint8_t *>(PouchAddress + 0x1) = 1;
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
            
            if (PartnerOnCursor == 0)
            {
              // Goombella
              *reinterpret_cast<uint8_t *>(PouchAddress + 0xF) = 0;
            }
            else if (PartnerOnCursor == 1)
            {
              // Koops
              *reinterpret_cast<uint8_t *>(PouchAddress + 0x1D) = 0;
            }
            else if (PartnerOnCursor == 2)
            {
              // Flurrie
              *reinterpret_cast<uint8_t *>(PouchAddress + 0x47) = 0;
            }
            else if (PartnerOnCursor == 3)
            {
              // Yoshi
              *reinterpret_cast<uint8_t *>(PouchAddress + 0x39) = 0;
            }
            else if (PartnerOnCursor == 4)
            {
              // Vivian
              *reinterpret_cast<uint8_t *>(PouchAddress + 0x55) = 0;
            }
            else if (PartnerOnCursor == 5)
            {
              // Bobbery
              *reinterpret_cast<uint8_t *>(PouchAddress + 0x2B) = 0;
            }
            else // if (PartnerOnCursor == 6)
            {
              // Mowz
              *reinterpret_cast<uint8_t *>(PouchAddress + 0x63) = 0;
            }
            
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

}