#include "mod.h"

#include <cstdio>

namespace mod {

void Mod::pauseMenuDisplayAllMenus()
{
  #ifdef TTYD_US
    uint32_t PauseMenuPartnerMenuAddress = 0x801649A0;
    uint32_t PauseMenuBadgeMenuAddress = 0x80164A44;
  #elif defined TTYD_JP
    uint32_t PauseMenuPartnerMenuAddress = 0x8015EFBC;
    uint32_t PauseMenuBadgeMenuAddress = 0x8015F060;
  #elif defined TTYD_EU
    uint32_t PauseMenuPartnerMenuAddress = 0x80166490;
    uint32_t PauseMenuBadgeMenuAddress = 0x80166534;
  #endif
  
  *reinterpret_cast<uint32_t *>(PauseMenuPartnerMenuAddress) = 0x60000000; // nop
  *reinterpret_cast<uint32_t *>(PauseMenuBadgeMenuAddress) = 0x60000000; // nop
}

}