#include "mod.h"
#include "patch.h"

#include <cstdio>

namespace mod {

extern "C" {
void StartActionCommands();
void BranchBackActionCommands();
}

void Mod::autoActionCommands()
{
  #ifdef TTYD_US
    uint32_t HookAddress = 0x800F78F0;
  #elif defined TTYD_JP
    uint32_t HookAddress = 0x800F29A4;
  #elif defined TTYD_EU
    uint32_t HookAddress = 0x800F875C;
  #endif
  
  patch::writeBranch(reinterpret_cast<void *>(HookAddress), reinterpret_cast<void *>(StartActionCommands));
  patch::writeBranch(reinterpret_cast<void *>(BranchBackActionCommands), reinterpret_cast<void *>(HookAddress + 0x4));
}

}