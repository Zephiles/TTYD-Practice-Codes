#include "mod.h"

#include <cstdio>

namespace mod {

void Mod::spawnItemPreventCrash()
{
  #ifdef TTYD_US
    uint32_t itemseq_GetItem_address = 0x800ABCD8;
  #elif defined TTYD_JP
    uint32_t itemseq_GetItem_address = 0x800AA01C;
  #elif defined TTYD_EU
    uint32_t itemseq_GetItem_address = 0x800AD0A8;
  #endif
  
  // This also prevents cutscenes from playing when collecting a boot or hammer upgrade
  *reinterpret_cast<uint32_t *>(itemseq_GetItem_address) = 0x48000030; // b 0x30
}

}