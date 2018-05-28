#include "mod.h"
#include "buttons.h"
#include "maps.h"

#include <ttyd/string.h>
#include <ttyd/system.h>
#include <ttyd/swdrv.h>
#include <ttyd/mariost.h>
#include <ttyd/mario.h>
#include <ttyd/party.h>
#include <ttyd/mario_party.h>
#include <ttyd/fontmgr.h>

#include <cstdio>

extern uint32_t NPCAddressesStart;
extern uint16_t PalaceSkipEnableCombo;
extern char *NextMap;

namespace mod {

void Mod::palaceSkip()
{
  if ((ttyd::string::strcmp(NextMap, reinterpret_cast<char *>(las_25)) == 0) && (ttyd::swdrv::swByteGet(0) < 390))
  {
    uint32_t ButtonInput = ttyd::system::keyGetButton(0);
    
    if ((ButtonInput & PalaceSkipEnableCombo) == PalaceSkipEnableCombo)
    {
      // Hold button to increment counter
      ++mEnabledOrDisabledCounter;
    }
    else
    {
      mEnabledOrDisabledCounter = 0;
    }
    
    if (mEnabledOrDisabledCounter > 360)
    {
      // Enable/Disable the Palace Skip info when button is held for 6 seconds
      mPalaceSkipInfoEnabled = !mPalaceSkipInfoEnabled;
      mEnabledOrDisabledCounter = 0;
    }
    
    if (mPalaceSkipInfoEnabled)
    {
      #ifdef TTYD_US
        uint32_t FieldItemsAddress = 0x803DC294;
      #elif defined TTYD_JP
        uint32_t FieldItemsAddress = 0x803D8714;
      #elif defined TTYD_EU
        uint32_t FieldItemsAddress = 0x803E82F4;
      #endif
      
      uint32_t SystemLevel = ttyd::mariost::marioStGetSystemLevel();
      ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
      
      uint32_t PartnerPointer = reinterpret_cast<uint32_t>(ttyd::party::partyGetPtr(ttyd::mario_party::marioGetPartyId()));
      float partnerCoordinateY;
      
      uint32_t NPCAddresses = *reinterpret_cast<uint32_t *>(NPCAddressesStart);
      uint32_t NPCActive = *reinterpret_cast<uint32_t *>(NPCAddresses + 0x340); // NPC 2
      float phantomEmberCoordinateY;
      
      uint32_t FieldItemsAddressesStart = *reinterpret_cast<uint32_t *>(FieldItemsAddress);
      int32_t ItemTimer;
      bool FoundItem = false;
      
      
      // Palace skip timing code
      if (SystemLevel == 15)
      {
      // Stop upon pausing
      mPalaceSkipTimer.stop();
      mPalaceSkipTimerPaused = true;
      }
      else if (SystemLevel == 0 && mPalaceSkipTimerPaused)
      {
        // Reset and Start when unpausing
        mPalaceSkipTimer.setValue(0);
        mPalaceSkipTimer.start();
        mPalaceSkipTimerPaused = false;
      }
      
      if (ttyd::system::keyGetButtonTrg(0) & PAD_X)
      {
        // Stop when pressing X
        mPalaceSkipTimer.stop();
      }
      
      if (ButtonInput & PAD_Y)
      {
        // Hold button to increment counter
        ++mResetCounter;
      }
      else
      {
        mResetCounter = 0;
      }
      
      if (mResetCounter > 120)
      {
        // Reset timer when button is held for 2 seconds
        mPalaceSkipTimer.stop();
        mPalaceSkipTimer.setValue(0);
        mResetCounter = 0;
      }
      
      // Get Partner Y coordinate
      if (PartnerPointer != 0)
      {
        // A partner is out, so set partnerCoordinateY
        partnerCoordinateY = *reinterpret_cast<float *>(PartnerPointer + 0x58 + 0x4); // Coordinate Y
      }
      else
      {
        // A partner is not out, so set partnerCoordinateY to 0
        partnerCoordinateY = 0;
      }
      
      // Get Phantom Ember Y coordinate
      NPCActive &= (1 << 0); // Check if 0 bit is active
      if (NPCActive)
      {
        // NPC 2 is active, so set phantomEmberCoordinateY
        phantomEmberCoordinateY = *reinterpret_cast<float *>(NPCAddresses + 0x340 + 0x90); // NPC 2 Coordinate Y
      }
      else
      {
        // NPC 2 is not active, so set phantomEmberCoordinateY to 0
        phantomEmberCoordinateY = 0;
      }
      
      // Get Field Item Timer
      for (int i = 0; i < 32; i++)
      {
        uint8_t Coin = 121;
        uint8_t Heart = 123;
        uint8_t Flower = 124;
        
        uint32_t tempItem;
        uint16_t tempItemAction;
        uint16_t tempBit;
        
        tempItem = *reinterpret_cast<uint32_t *>(FieldItemsAddressesStart + 0x4);
        if ((tempItem == 0) || (tempItem == Coin) || (tempItem == Heart) || (tempItem == Flower))
        {
          // Don't want this item, so go to the next one
          FieldItemsAddressesStart += 0x98;
        }
        else
        {
          // Get current item action
          tempItemAction = *reinterpret_cast<uint16_t *>(FieldItemsAddressesStart + 0x24);
          if (tempItemAction == 7)
          {
            // Item is already spawned upon entering the room, so go to the next one
            FieldItemsAddressesStart += 0x98;
          }
          else
          {
            // Check if item is currently spawned
            tempBit = *reinterpret_cast<uint16_t *>(FieldItemsAddressesStart);
            tempBit &= (1 << 0); // Check if 0 bit is active
            if (!tempBit)
            {
              // Item is not spawned, so go to the next one
              FieldItemsAddressesStart += 0x98;
            }
            else
            {
              // Found the item we want, so assign the timer and exit the loop
              ItemTimer = *reinterpret_cast<int32_t *>(FieldItemsAddressesStart + 0x7C);
              FoundItem = true;
              break;
            }
          }
        }
      }
      
      if (!FoundItem)
      {
        // Didn't find an item to use, so set timer to 0
        ItemTimer = 0;
      }
      
      sprintf(mDisplayBuffer,
        "PST: %lu\r\nItemTimer: %ld\r\nPhaEmY: %.2f\r\nParY: %.2f\r\nMarPos: %.2f %.2f %.2f",
        mPalaceSkipTimer.getValue(),
        ItemTimer,
        phantomEmberCoordinateY,
        partnerCoordinateY,
        player->playerPosition[0], player->playerPosition[1], player->playerPosition[2]);
      
      int FontDrawY = -40;
      if (mButtonInputDisplayEnabled)
      {
        FontDrawY += 35;
      }
      
      ttyd::fontmgr::FontDrawStart();
      uint32_t color = 0xFFFFFFFF;
      ttyd::fontmgr::FontDrawColor(reinterpret_cast<uint8_t *>(&color));
      ttyd::fontmgr::FontDrawEdge();
      ttyd::fontmgr::FontDrawMessage(-252, FontDrawY, mDisplayBuffer);
      
      mPalaceSkipTimer.tick();
    }
  }
}

}