#include "mod.h"

#include <ttyd/system.h>
#include <ttyd/mario_pouch.h>

#include <cstdio>

extern uint32_t r13;
extern uint16_t LockMarioHPToggleCombo;

namespace mod {

void Mod::lockMarioHPToggle()
{
	uint32_t ButtonInput = ttyd::system::keyGetButton(0);
	
	if (!MarioHPCurrentlyLocked)
	{
		// HP not currently locked
		if ((ButtonInput & LockMarioHPToggleCombo) == LockMarioHPToggleCombo)
		{
			if (!lockMarioHPToggleDisable)
			{
				MarioHPCurrentlyLocked = true;
			}
			lockMarioHPToggleDisable = true;
		}
		else
		{
			// Reset counter if no button combo is pressed/held
			lockMarioHPToggleDisable = false;
		}
	}
	else
	{
		// Lock HP
		uint32_t PouchPointer = ttyd::mario_pouch::pouchGetPtr();
		*reinterpret_cast<int16_t *>(PouchPointer + 0x70) = *reinterpret_cast<int16_t *>(PouchPointer + 0x72); // Copy Max HP to Current HP
		
		#ifdef TTYD_US
		uint32_t BattleAddress  = r13 + 0x1BE0;
		#elif defined TTYD_JP
		uint32_t BattleAddress  = r13 + 0x1710;
		#elif defined TTYD_EU
		uint32_t BattleAddress  = r13 + 0x1CC0;
		#endif
		
		BattleAddress = *reinterpret_cast<uint32_t *>(BattleAddress);
		if (BattleAddress != 0)
		{
			// Currently in a battle
			BattleAddress = *reinterpret_cast<uint32_t *>(BattleAddress + 0x24);
			if (BattleAddress != 0)
			{
				// Make sure enemy info is there
				*reinterpret_cast<int16_t *>(BattleAddress + 0x10C) = *reinterpret_cast<int16_t *>(BattleAddress + 0x108); // Copy Battle Max HP to Current Battle HP
			}
		}
		
		if ((ButtonInput & LockMarioHPToggleCombo) == LockMarioHPToggleCombo)
		{
			if (!lockMarioHPToggleDisable)
			{
				MarioHPCurrentlyLocked = false;
			}
			lockMarioHPToggleDisable = true;
		}
		else
		{
			// Reset counter if no button combo is pressed/held
			lockMarioHPToggleDisable = false;
		}
	}
}

}