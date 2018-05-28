#include "mod.h"

#include <ttyd/system.h>
#include <ttyd/seqdrv.h>
#include <ttyd/mariost.h>
#include <ttyd/mario_pouch.h>
#include <ttyd/string.h>
#include <ttyd/mario.h>
#include <ttyd/itemdrv.h>

#include <cstdio>

extern uint16_t SpawnItemCombo;

namespace mod {

void Mod::spawnItem()
{
  if ((ttyd::system::keyGetButton(0) & SpawnItemCombo) == (SpawnItemCombo))
  {
    if (!spawnItemDisable)
    {
      uint32_t Game = static_cast<uint32_t>(ttyd::seqdrv::SeqIndex::kGame);
      if ((ttyd::seqdrv::seqGetNextSeq() == Game) && (ttyd::mariost::marioStGetSystemLevel() != 15))
      {
        // Not in pause menu
        int16_t CoinCount = *reinterpret_cast<int16_t *>(ttyd::mario_pouch::pouchGetPtr() + 0x78);
        
        if ((CoinCount >= 1) && (CoinCount <= 338))
        {
          // Coin count is a valid item ID
          char *ItemName = new char[8]; // 6 bytes for C_Item, 1 byte for spawnItemNameCounter, 1 byte for NULL
          ttyd::string::strcpy(ItemName, "C_Item");
          ttyd::string::strcat(ItemName, reinterpret_cast<char *>(&spawnItemNameCounter));
          spawnItemNameCounter++;
          
          ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
          float ItemCoordinateX = player->playerPosition[0];
          float ItemCoordinateY = player->playerPosition[1];
          float ItemCoordinateZ = player->playerPosition[2];
          
          // Move item 30 units in front of Mario
          if (player->wPlayerDirection <= 0)
          {
            ItemCoordinateX -= 30;
          }
          else
          {
            ItemCoordinateX += 30;
          }
          
          ttyd::itemdrv::itemEntry(ItemName, CoinCount, 16, -1, 0, ItemCoordinateX, ItemCoordinateY, ItemCoordinateZ);
          delete[] (ItemName);
        }
      }
    }
    spawnItemDisable = true;
  }
  else
  {
    // Reset flag if no button combo is pressed/held
    spawnItemDisable = false;
  }
}

}