#include "mod.h"

#include <ttyd/mariost.h>
#include <ttyd/mario_pouch.h>
#include <ttyd/system.h>
#include <ttyd/swdrv.h>

#include <cstdio>

extern uint32_t PauseMenuAddressesStart;
extern uint16_t CoinsSetToZeroCombo;
extern uint16_t CoinsSetSequenceCombo;
extern uint16_t CoinsIncrement1Combo;
extern uint16_t CoinsIncrement10Combo;
extern uint16_t CoinsIncrement100Combo;
extern uint16_t CoinsDecrement1Combo;
extern uint16_t CoinsDecrement10Combo;
extern uint16_t CoinsDecrement100Combo;

namespace mod {

void adjustCoinCount(uint32_t tempCoinCountAddress, int16_t tempCoinCount, int16_t Value)
{
  tempCoinCount += Value;
  *reinterpret_cast<int16_t *>(tempCoinCountAddress) = tempCoinCount;
}

void Mod::adjustCoinCountSetSequence()
{
  if (ttyd::mariost::marioStGetSystemLevel() == 15)
  {
    // Currently in the pause menu	
    uint32_t PauseMenuAddress = *reinterpret_cast<uint32_t *>(PauseMenuAddressesStart);
    uint32_t CurrentTab = *reinterpret_cast<uint32_t *>(PauseMenuAddress + 0x40);
    
    uint32_t CoinCountAddress = ttyd::mario_pouch::pouchGetPtr() + 0x78;
    int16_t CoinCount = *reinterpret_cast<int16_t *>(CoinCountAddress);
    
    uint32_t ButtonInput = ttyd::system::keyGetButton(0);
    uint16_t ButtonHoldFrames = 45; // 0.75 Seconds
    
    if (CurrentTab == 0)
    {
      // Current Tab is Mario's stats
      if ((ButtonInput & CoinsSetToZeroCombo) == CoinsSetToZeroCombo)
      {
        // Set Coin Count to 0
        *reinterpret_cast<int16_t *>(CoinCountAddress) = 0;
      }
      else if ((ButtonInput & CoinsSetSequenceCombo) == CoinsSetSequenceCombo)
      {
        // Set Sequence Position to Coin Count
        ttyd::swdrv::swByteSet(0, CoinCount);
      }
      else if ((ButtonInput & CoinsIncrement1Combo) == CoinsIncrement1Combo)
      {
        if (CoinCountButtonHoldCounter == 0)
        {
          // Increment Coin Count by 1
          adjustCoinCount(CoinCountAddress, CoinCount, 1);
        }
        CoinCountButtonHoldCounter++;
      }
      else if ((ButtonInput & CoinsIncrement10Combo) == CoinsIncrement10Combo)
      {
        if (CoinCountButtonHoldCounter == 0)
        {
          // Increment Coin Count by 10
          adjustCoinCount(CoinCountAddress, CoinCount, 10);
        }
        CoinCountButtonHoldCounter++;
      }
      else if ((ButtonInput & CoinsIncrement100Combo) == CoinsIncrement100Combo)
      {
        if (CoinCountButtonHoldCounter == 0)
        {
          // Increment Coin Count by 100
          adjustCoinCount(CoinCountAddress, CoinCount, 100);
        }
        CoinCountButtonHoldCounter++;
      }
      else if ((ButtonInput & CoinsDecrement1Combo) == CoinsDecrement1Combo)
      {
        if (CoinCountButtonHoldCounter == 0)
        {
          // Decrement Coin Count by 1
          adjustCoinCount(CoinCountAddress, CoinCount, -1);
        }
        CoinCountButtonHoldCounter++;
      }
      else if ((ButtonInput & CoinsDecrement10Combo) == CoinsDecrement10Combo)
      {
        if (CoinCountButtonHoldCounter == 0)
        {
          // Decrement Coin Count by 10
          adjustCoinCount(CoinCountAddress, CoinCount, -10);
        }
        CoinCountButtonHoldCounter++;
      }
      else if ((ButtonInput & CoinsDecrement100Combo) == CoinsDecrement100Combo)
      {
        if (CoinCountButtonHoldCounter == 0)
        {
          // Decrement Coin Count by 100
          adjustCoinCount(CoinCountAddress, CoinCount, -100);
        }
        CoinCountButtonHoldCounter++;
      }
      else
      {
        // Reset counter if no button combo is pressed/held
        CoinCountButtonHoldCounter = 0;
      }
      
      if (CoinCountButtonHoldCounter > ButtonHoldFrames)
      {
        // Reset if counter exceeds ButtonHoldFrames
        CoinCountButtonHoldCounter = 0;
      }
    }
  }
}

}