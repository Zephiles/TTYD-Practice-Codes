#include "mod.h"

#include <ttyd/system.h>
#include <ttyd/mariost.h>
#include <ttyd/mario_pouch.h>

#include <cstdio>

extern uint32_t BattleAddressesStart;
extern uint16_t LockMarioHPToggleCombo;

namespace mod {

void Mod::lockMarioHPToggle()
{
	uint32_t ButtonInput = ttyd::system::keyGetButton(0);
	uint32_t SystemLevel = ttyd::mariost::marioStGetSystemLevel();
	
	if (!MarioHPCurrentlyLocked)
	{
		// HP not currently locked
		if (((ButtonInput & LockMarioHPToggleCombo) == LockMarioHPToggleCombo) && (SystemLevel != 15))
		{
			// Not in pause menu
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
		uint32_t BattleAddress = *reinterpret_cast<uint32_t *>(BattleAddressesStart);
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
		
		if (((ButtonInput & LockMarioHPToggleCombo) == LockMarioHPToggleCombo) && (SystemLevel != 15))
		{
			// Not in pause menu
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