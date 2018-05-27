#include "mod.h"
#include "patch.h"

#include <cstdio>

namespace mod {

extern "C" {
void StartDisableBattles();
void BranchBackDisableBattles();
}

void Mod::disableBattles()
{
  #ifdef TTYD_US
    uint32_t HookAddress = 0x800448CC;
  #elif defined TTYD_JP
    uint32_t HookAddress = 0x80044228;
  #elif defined TTYD_EU
    uint32_t HookAddress = 0x800449B4;
  #endif
  
  patch::writeBranch(reinterpret_cast<void *>(HookAddress), reinterpret_cast<void *>(StartDisableBattles));
  patch::writeBranch(reinterpret_cast<void *>(BranchBackDisableBattles), reinterpret_cast<void *>(HookAddress + 0x4));
}

}