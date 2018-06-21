#include "mod.h"
#include "buttons.h"

#include <ttyd/system.h>
#include <ttyd/mario.h>
#include <ttyd/fontmgr.h>

#include <cstdio>

extern uint16_t JumpStorageDisplayEnableCombo;

namespace mod {

void Mod::jumpStorageDisplay()
{ 
  if ((ttyd::system::keyGetButton(0) & JumpStorageDisplayEnableCombo) == JumpStorageDisplayEnableCombo)
  {
    // Hold button combo to increment counter
    ++jumpStorageDisplayCounter;
  }
  else
  {
    jumpStorageDisplayCounter = 0;
  }
  
  if (jumpStorageDisplayCounter > 360)
  {
    // Enable/Disable the Jump Storage Display when button combo is held for 6 seconds
    jumpStorageDisplayEnabled = !jumpStorageDisplayEnabled;
    jumpStorageDisplayCounter = 0;
  }
  
  if (jumpStorageDisplayEnabled)
  {
    ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
    uint32_t ButtonInputTrg = ttyd::system::keyGetButtonTrg(0);
    
    if (ButtonInputTrg & PAD_Y)
    {
      // Press Y to increment counter
      ++jumpStorageButtonCounter;
    }
    else if (ButtonInputTrg != 0)
    {
      // Reset counter if a different button is pressed
      jumpStorageButtonCounter = 0;
    }
    
    if (jumpStorageButtonCounter == 3)
    {
      player->flags3 |= 1 << 16; // Turn on the 16 bit
      jumpStorageButtonCounter = 0;
    }
    
    sprintf(mDisplayBuffer,
      "JS: %ld\r\nSpdY: %.2f",
      (player->flags3 & (1 << 16)) >> 16, // Get only the 16 bit
      player->wJumpVelocityY);
    
    ttyd::fontmgr::FontDrawStart();
    uint32_t color = 0xFFFFFFFF;
    ttyd::fontmgr::FontDrawColor(reinterpret_cast<uint8_t *>(&color));
    ttyd::fontmgr::FontDrawEdge();
    ttyd::fontmgr::FontDrawMessage(87, 120, mDisplayBuffer);
  }
}

}