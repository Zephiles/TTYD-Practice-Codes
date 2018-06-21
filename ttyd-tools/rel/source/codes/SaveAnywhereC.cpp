#include "mod.h"

#include <ttyd/system.h>
#include <ttyd/seqdrv.h>
#include <ttyd/mariost.h>
#include <ttyd/mario.h>
#include <ttyd/evtmgr.h>

#include <cstdio>

extern uint16_t SaveAnywhereCombo;

namespace mod {

void Mod::saveAnywhere()
{
  if (!saveAnywhereScriptRunning)
  {
    // Save Script is not running
    if ((ttyd::system::keyGetButton(0) & SaveAnywhereCombo) == (SaveAnywhereCombo))
    {
      uint32_t Game = static_cast<uint32_t>(ttyd::seqdrv::SeqIndex::kGame);
      if ((ttyd::seqdrv::seqGetNextSeq() == Game) && (ttyd::mariost::marioStGetSystemLevel() != 15))
      {
        // Not in the Pause Menu
        #ifdef TTYD_US
          uint32_t *SaveScript = reinterpret_cast<uint32_t *>(0x803BAC3C);
        #elif defined TTYD_JP
          uint32_t *SaveScript = reinterpret_cast<uint32_t *>(0x803B68BC);
        #elif defined TTYD_EU
          uint32_t *SaveScript = reinterpret_cast<uint32_t *>(0x803C6C4C);
        #endif
        
        // Take away control from the player and start the Save script
        ttyd::mario::marioKeyOff();
        ttyd::mariost::marioStSystemLevel(1);
        saveAnywhereThreadID = *reinterpret_cast<uint32_t *>(ttyd::evtmgr::evtEntryType(SaveScript, 0, 0, 0) + 0x15C);
        saveAnywhereScriptRunning = true;
      }
    }
  }
  else if (!ttyd::evtmgr::evtCheckID(saveAnywhereThreadID))
  {
    // Save Script is no longer running, so give back control to the player
    saveAnywhereScriptRunning = false;
    ttyd::mariost::marioStSystemLevel(0);
    ttyd::mario::marioKeyOn();
  }
}

}