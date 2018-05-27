#include "mod.h"
#include "patch.h"

#include <cstdio>

namespace mod {

extern "C" {
void StartInfiniteItemUsage();
void BranchInfiniteItemUsage();
}

void Mod::infiniteItemUsage()
{
  #ifdef TTYD_US
    uint32_t HookAddress = 0x800D4974;
  #elif defined TTYD_JP
    uint32_t HookAddress = 0x800D0754;
  #elif defined TTYD_EU
    uint32_t HookAddress = 0x800D577C;
  #endif
  
  patch::writeBranch(reinterpret_cast<void *>(HookAddress), reinterpret_cast<void *>(StartInfiniteItemUsage));
  patch::writeBranch(reinterpret_cast<void *>(BranchInfiniteItemUsage), reinterpret_cast<void *>(HookAddress + 0x4));
}

}