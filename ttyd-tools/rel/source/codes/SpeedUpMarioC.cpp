#include "mod.h"

#include <ttyd/mario.h>
#include <ttyd/system.h>

#include <cstdio>

extern uint16_t SpeedUpMarioCombo;

namespace mod {

void Mod::speedUpMario()
{
  ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
  
  if ((ttyd::system::keyGetButton(0) & SpeedUpMarioCombo) == (SpeedUpMarioCombo))
  {
    player->wPlayerBaseSpeed = 16;
    player->unk_184 = 16;
    player->unk_188 = 16;
  }
  else
  {
    player->unk_184 = 1;
    player->unk_188 = 2.25;
  }
}

}