#include "mod.h"

#include <cstdio>

namespace mod {

void Mod::enableDebugMode()
{
  #ifdef TTYD_US
    uint32_t DebugModeInitialzeAddress = 0x80009B2C;
    uint32_t DebugModeShowBuildDateAddress = 0x80008FE8;
  #elif defined TTYD_JP
    uint32_t DebugModeInitialzeAddress = 0x8000999C;
    uint32_t DebugModeShowBuildDateAddress = 0x80008EB8;
  #elif defined TTYD_EU
    uint32_t DebugModeInitialzeAddress = 0x80009CFC;
    uint32_t DebugModeShowBuildDateAddress = 0x800091B4;
  #endif
  
  *reinterpret_cast<uint32_t *>(DebugModeInitialzeAddress) = 0x3800FFFF; // li r0,-1
  *reinterpret_cast<uint32_t *>(DebugModeShowBuildDateAddress) = 0x60000000; // nop
}

}