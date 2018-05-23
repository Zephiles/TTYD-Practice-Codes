#include "mod.h"
#include "patch.h"

#include <cstdio>

namespace mod {

extern "C" {
void StartPhantomEmberDrop();
void BranchBackPhantomEmberDrop();
}

void Mod::forcePhantomEmberDrop()
{
	#ifdef TTYD_US
	uint32_t HookAddress = 0x8004EC10;
	#elif defined TTYD_JP
	uint32_t HookAddress = 0x8004DFB0;
	#elif defined TTYD_EU
	uint32_t HookAddress = 0x8004ECDC;
	#endif
	
	patch::writeBranch(reinterpret_cast<void *>(HookAddress), reinterpret_cast<void *>(StartPhantomEmberDrop));
	patch::writeBranch(reinterpret_cast<void *>(BranchBackPhantomEmberDrop), reinterpret_cast<void *>(HookAddress + 0x4));
}

}