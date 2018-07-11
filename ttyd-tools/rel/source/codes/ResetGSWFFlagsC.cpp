#include "mod.h"

#include <ttyd/system.h>
#include <ttyd/__mem.h>

#include <cstdio>

extern uint32_t GSWAddressesStart;
extern uint16_t ResetGSWFFlagsCombo;

namespace mod {

void Mod::resetGSWFFlags()
{
  if ((ttyd::system::keyGetButton(0) & ResetGSWFFlagsCombo) == ResetGSWFFlagsCombo)
  {
    uint32_t GSWFFlagsAddressesStart = *reinterpret_cast<uint32_t *>(GSWAddressesStart);
    GSWFFlagsAddressesStart += 0x178;
    
    ttyd::__mem::memset(reinterpret_cast<uint32_t *>(GSWFFlagsAddressesStart), 0, 0x2C8);
  }
}

}