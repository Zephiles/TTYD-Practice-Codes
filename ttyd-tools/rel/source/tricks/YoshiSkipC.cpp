#include "mod.h"
#include "buttons.h"
#include "maps.h"

#include <ttyd/string.h>
#include <ttyd/system.h>
#include <ttyd/mario.h>
#include <ttyd/w_atan2.h>
#include <ttyd/seqdrv.h>
#include <ttyd/fontmgr.h>

#include <cstdio>

extern uint16_t YoshiSkipEnableCombo;
extern char *NextMap;

namespace mod {

double getStickValue(double Stick)
{
  if (Stick > 127)
  {
    Stick -= 256;
  }
  return Stick;
}

void Mod::yoshiSkip()
{
  if (ttyd::string::strcmp(NextMap, reinterpret_cast<char *>(tik_07)) == 0)
  {
    uint32_t ButtonInput = ttyd::system::keyGetButton(0);
    
    if ((ButtonInput & YoshiSkipEnableCombo) == YoshiSkipEnableCombo)
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
      // Enable/Disable the Yoshi Skip info when button is held for 6 seconds
      mYoshiSkipInfoEnabled = !mYoshiSkipInfoEnabled;
      mEnabledOrDisabledCounter = 0;
    }
    
    if (mYoshiSkipInfoEnabled)
    {
      ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
      bool MarioControl = player->flags1 & (1 << 0); // Check if 0 bit is active
      
      double StickX = getStickValue(ttyd::system::keyGetStickX(0));
      double StickY = getStickValue(ttyd::system::keyGetStickY(0));
      double PI = 3.14159265358979323846;
      double stickAngle = (ttyd::w_atan2::atan2(StickX,StickY)) * (180 / PI);
      
      if (stickAngle < 0)
      {
        stickAngle += 360;
      }
      
      // Yoshi skip timing code
      if (ttyd::seqdrv::seqGetSeq() == 4) // In battle
      {
      // Pause in battle
      mYoshiSkipTimer.stop();
      mYoshiSkipTimerPaused = true;
      }
      else if (MarioControl && mYoshiSkipTimerPaused)
      {
        // Reset and Start when leaving battle
        mYoshiSkipTimer.setValue(0);
        mYoshiSkipTimer.start();
        mYoshiSkipTimerPaused = false;
      }
      
      if (ttyd::system::keyGetButtonTrg(0) & PAD_A)
      {
        // Stop when pressing A
        mYoshiSkipTimer.stop();
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
        mYoshiSkipTimer.stop();
        mYoshiSkipTimer.setValue(0);
        mResetCounter = 0;
      }
      
      sprintf(mDisplayBuffer,
        "YST: %lu\r\nStickAngle: %.2f\r\nMarPos: %.2f %.2f %.2f",
        mYoshiSkipTimer.getValue(),
        stickAngle,
        player->playerPosition[0], player->playerPosition[1], player->playerPosition[2]);
      
      int FontDrawY = -98;
      if (mButtonInputDisplayEnabled)
      {
        FontDrawY += 35;
      }
      
      ttyd::fontmgr::FontDrawStart();
      uint32_t color = 0xFFFFFFFF;
      ttyd::fontmgr::FontDrawColor(reinterpret_cast<uint8_t *>(&color));
      ttyd::fontmgr::FontDrawEdge();
      ttyd::fontmgr::FontDrawMessage(-252, FontDrawY, mDisplayBuffer);
      
      mYoshiSkipTimer.tick();
    }
  }
}

}