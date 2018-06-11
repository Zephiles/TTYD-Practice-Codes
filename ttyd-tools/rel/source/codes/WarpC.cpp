#include "mod.h"
#include "maps.h"

#include <ttyd/system.h>
#include <ttyd/mariost.h>
#include <ttyd/mario_pouch.h>
#include <ttyd/string.h>
#include <ttyd/swdrv.h>

#include <cstdio>

extern uint32_t PauseMenuAddressesStart;
extern uint16_t WarpCombo;
extern char *NextBero;
extern char *NextMap;
extern char *NextArea;

namespace mod {

void Mod::warp()
{
  if ((ttyd::system::keyGetButton(0) & WarpCombo) == WarpCombo)
  {
    uint32_t PauseMenuAddress = *reinterpret_cast<uint32_t *>(PauseMenuAddressesStart);
    uint32_t CurrentTab = *reinterpret_cast<uint32_t *>(PauseMenuAddress + 0x40);
    
    if ((ttyd::mariost::marioStGetSystemLevel() == 15) && (CurrentTab == 0))
    {
      // Currently in the pause menu, and Current Tab is Mario's stats
      uint32_t PouchAddress = reinterpret_cast<uint32_t>(ttyd::mario_pouch::pouchGetPtr());
      int16_t CoinCount = *reinterpret_cast<int16_t *>(PouchAddress + 0x78);
      uint32_t WarpToMap = 0;
      
      switch (CoinCount)
      {
        case 0: WarpToMap = gor_01; break;
        case 1: WarpToMap = tik_00; break;
        case 2: WarpToMap = hei_00; break;
        case 3: WarpToMap = gon_00; break;
        case 4: WarpToMap = win_06; break;
        case 5: WarpToMap = mri_01; break;
        case 6: WarpToMap = tou_02; break;
        case 7: WarpToMap = tou_05; break;
        case 8: WarpToMap = usu_00; break;
        case 9: WarpToMap = jin_00; break;
        case 10: WarpToMap = muj_01; break;
        case 11: WarpToMap = dou_00; break;
        case 12: WarpToMap = rsh_02_a; break;
        case 13: WarpToMap = eki_00; break;
        case 14: WarpToMap = pik_00; break;
        case 15: WarpToMap = bom_00; break;
        case 16: WarpToMap = moo_00; break;
        case 17: WarpToMap = aji_00; break;
        case 18: WarpToMap = aji_13; break;
        case 19: WarpToMap = las_00; break;
        case 20: WarpToMap = las_09; break;
        case 21: WarpToMap = las_27; break;
        case 22:
          #ifdef TTYD_US
            uint32_t minnnanokoe = 0x802ECF74;
          #elif defined TTYD_JP
            uint32_t minnnanokoe = 0x802ECB70;
          #elif defined TTYD_EU
            uint32_t minnnanokoe = 0x802F8BD4;
          #endif
          
          ttyd::string::strcpy(NextBero, reinterpret_cast<char *>(minnnanokoe));
          WarpToMap = las_29;
          ttyd::swdrv::swByteSet(0, 400);
          break;
      }
      
      if (WarpToMap != 0)
      {
        // WarpToMap is set
        ttyd::string::strcpy(NextMap, reinterpret_cast<char *>(WarpToMap));
        ttyd::string::strncpy(NextArea, reinterpret_cast<char *>(WarpToMap), 3);
      }
    }
  }
}

}