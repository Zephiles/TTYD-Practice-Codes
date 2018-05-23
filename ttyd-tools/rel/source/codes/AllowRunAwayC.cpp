#include "mod.h"
#include "patch.h"

#include <cstdio>

namespace mod {

void Mod::allowRunAway()
{
	#ifdef TTYD_US
	uint32_t AllowRunAwayBranchAddress = 0x80123CAC;
	#elif defined TTYD_JP
	uint32_t AllowRunAwayBranchAddress = 0x8011E7E4;
	#elif defined TTYD_EU
	uint32_t AllowRunAwayBranchAddress = 0x80124BEC;
	#endif
	
	*reinterpret_cast<uint32_t *>(AllowRunAwayBranchAddress) = 0x48000018; // b 0x18
}

}