#include "mod.h"

#include <ttyd/system.h>
#include <ttyd/mario.h>

#include <cstdio>

extern uint16_t LevitateCombo;

namespace mod {

void Mod::levitate()
{
  if ((ttyd::system::keyGetButton(0) & LevitateCombo) == (LevitateCombo))
  {
    ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
    player->wJumpVelocityY = 4.540000915527344; // This ends up writing the exact same value as the Gecko code
  }
}

}