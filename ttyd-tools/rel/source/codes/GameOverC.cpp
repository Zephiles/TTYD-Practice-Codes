#include "mod.h"

#include <ttyd/system.h>
#include <ttyd/seqdrv.h>

#include <cstdio>

extern uint16_t GameOverCombo;

namespace mod {

void Mod::gameOver()
{
  if ((ttyd::system::keyGetButton(0) & GameOverCombo) == (GameOverCombo))
  {
    int32_t Logo = static_cast<uint32_t>(ttyd::seqdrv::SeqIndex::kLogo);
    if (ttyd::seqdrv::seqGetNextSeq() != Logo)
    {
      // The game will crash if the following code runs during the Logo
      ttyd::seqdrv::seqSetSeq(ttyd::seqdrv::SeqIndex::kGameOver, nullptr, nullptr);
    }
  }
}

}