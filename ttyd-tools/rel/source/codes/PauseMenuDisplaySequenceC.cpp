#include "mod.h"

#include <ttyd/mariost.h>
#include <ttyd/swdrv.h>
#include <ttyd/fontmgr.h>

#include <cstdio>

extern uint32_t PauseMenuAddressesStart;

namespace mod {

void Mod::pauseMenuDisplaySequence()
{
  uint32_t PauseMenuAddress = *reinterpret_cast<uint32_t *>(PauseMenuAddressesStart);
  uint32_t CurrentTab = *reinterpret_cast<uint32_t *>(PauseMenuAddress + 0x40);
  uint32_t CurrentMenu = *reinterpret_cast<uint32_t *>(PauseMenuAddress + 0x24);
  
  if ((ttyd::mariost::marioStGetSystemLevel() == 15) && (CurrentTab == 0) && ((CurrentMenu == 10) || ((CurrentMenu == 11))))
  {
    // Currently in the pause menu and Current Tab is Mario's stats
    uint32_t SequencePosition = ttyd::swdrv::swByteGet(0);
    
    sprintf(mDisplayBuffer,
      "Seq: %lu",
      SequencePosition);
    
    ttyd::fontmgr::FontDrawStart();
    uint32_t color = 0xFFFFFFFF;
    ttyd::fontmgr::FontDrawColor(reinterpret_cast<uint8_t *>(&color));
    ttyd::fontmgr::FontDrawEdge();
    ttyd::fontmgr::FontDrawMessage(80, 50, mDisplayBuffer);
  }
}

}