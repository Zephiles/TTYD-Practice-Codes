#include "mod.h"
#include "maps.h"

#include <ttyd/string.h>

#include <cstdio>

extern uint32_t r13;
extern char *NextMap;

namespace mod {

void Mod::bobberyEarly()
{
	// Turn off GSWF(3137)
	uint32_t GSWFAddressesStart = *reinterpret_cast<uint32_t *>(r13 - 0x6F50);
	*reinterpret_cast<uint32_t *>(GSWFAddressesStart + 0x300) &= ~(1 << 1); // Turn off the 1 bit
	
	if (ttyd::string::strcmp(NextMap, reinterpret_cast<char *>(muj_05)) == 0)
	{
		#ifdef TTYD_US
		uint32_t NPCAddress = r13 + 0x19A0;
		#elif defined TTYD_JP
		uint32_t NPCAddress = r13 + 0x1448;
		#elif defined TTYD_EU
		uint32_t NPCAddress = r13 + 0x1A80;
		#endif
		
		NPCAddress = *reinterpret_cast<uint32_t *>(NPCAddress);
		NPCAddress += 0x1040; // NPC 6
		
		// Allow the Ember to be refought
		*reinterpret_cast<uint8_t *>(NPCAddress + 0x1D7) = 0;
	}
}

}