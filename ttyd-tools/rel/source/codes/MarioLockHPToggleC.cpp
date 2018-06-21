#include "mod.h"

#include <ttyd/system.h>
#include <ttyd/mariost.h>
#include <ttyd/mario_pouch.h>
#include <ttyd/battle.h>

#include <cstdio>

extern uint32_t BattleAddressesStart;
extern uint16_t LockMarioHPToggleCombo;

namespace mod {

void Mod::lockMarioHPToggle()
{
  uint32_t ButtonInput = ttyd::system::keyGetButton(0);
  uint32_t SystemLevel = ttyd::mariost::marioStGetSystemLevel();
  
  if (!marioHPCurrentlyLocked)
  {
    // HP not currently locked
    if (((ButtonInput & LockMarioHPToggleCombo) == LockMarioHPToggleCombo) && (SystemLevel != 15))
    {
      // Not in pause menu
      if (!lockMarioHPToggleDisable)
      {
        marioHPCurrentlyLocked = true;
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
    uint32_t PouchAddress = reinterpret_cast<uint32_t>(ttyd::mario_pouch::pouchGetPtr());
    
    uint32_t MarioHPAddress = PouchAddress + 0x70;
    int16_t MarioMaxHP = *reinterpret_cast<int16_t *>(PouchAddress + 0x72);
    *reinterpret_cast<int16_t *>(MarioHPAddress) = MarioMaxHP; // Copy Max HP to Current HP
    
    uint32_t BattleAddress = *reinterpret_cast<uint32_t *>(BattleAddressesStart);
    if (BattleAddress != 0)
    {
      // Currently in a battle
      uint32_t *BattleAddressPointer = reinterpret_cast<uint32_t *>(BattleAddress);
      uint32_t MarioBattleAddress = reinterpret_cast<uint32_t>(ttyd::battle::BattleGetMarioPtr(BattleAddressPointer));
      if (MarioBattleAddress != 0)
      {
        // Make sure Mario's info is there
        uint32_t BattleMarioHPAddress = MarioBattleAddress + 0x10C;
        int16_t BattleMarioMaxHP = *reinterpret_cast<int16_t *>(MarioBattleAddress + 0x108);
        *reinterpret_cast<int16_t *>(BattleMarioHPAddress) = BattleMarioMaxHP; // Copy Battle Max HP to Current Battle HP
      }
    }
    
    if (((ButtonInput & LockMarioHPToggleCombo) == LockMarioHPToggleCombo) && (SystemLevel != 15))
    {
      // Not in pause menu
      if (!lockMarioHPToggleDisable)
      {
        marioHPCurrentlyLocked = false;
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