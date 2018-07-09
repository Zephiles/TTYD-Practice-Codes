#include "mod.h"

#include <ttyd/system.h>
#include <ttyd/seqdrv.h>
#include <ttyd/string.h>
#include <ttyd/mariost.h>
#include <ttyd/camdrv.h>
#include <ttyd/pmario_sound.h>
#include <ttyd/mario_cam.h>

#include <cstdio>

extern uint16_t ReloadScreenCombo;
extern char *NextMap;
extern char *NextBero;
extern char *NewMap;
extern char *NewBero;

namespace mod {

void Mod::reloadScreen()
{
  if ((ttyd::system::keyGetButton(0) & ReloadScreenCombo) == (ReloadScreenCombo))
  {
    uint32_t NextSeq = ttyd::seqdrv::seqGetNextSeq();
    uint32_t Game = static_cast<uint32_t>(ttyd::seqdrv::SeqIndex::kGame);
    uint32_t MapChange = static_cast<uint32_t>(ttyd::seqdrv::SeqIndex::kMapChange);
    
    if ((NextSeq >= Game) && (NextSeq <= MapChange))
    {
      // Not in the pause menu
      // A separate address for NextBero is needed, as the original value will be cleared during the reloading process
      // The game will crash if NextMap is used directly in seqSetSeq, so a separate address must be used instead
      
      ttyd::string::strcpy(NewBero, NextBero);
      ttyd::string::strcpy(NewMap, NextMap);
      ttyd::seqdrv::seqSetSeq(ttyd::seqdrv::SeqIndex::kMapChange, NewMap, NewBero);
      
      uint32_t SystemLevel = ttyd::mariost::marioStGetSystemLevel();
      if (SystemLevel != 0)
      {
        if (SystemLevel == 15)
        {
          // Currently in pause menu, so re-enable the camera
          ttyd::camdrv::camDispOn(4);
        }
        
        // Enable sound effects, set the default camera id for Mario, and give back control to the player
        ttyd::pmario_sound::psndClearFlag(0x80);
        ttyd::mario_cam::marioSetCamId(4);
        ttyd::mariost::marioStSystemLevel(0);
      }
    }
  }
}

}