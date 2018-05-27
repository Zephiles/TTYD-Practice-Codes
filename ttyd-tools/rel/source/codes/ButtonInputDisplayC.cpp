#include "mod.h"

#include <ttyd/system.h>
#include <ttyd/fontmgr.h>

#include <cstdio>

extern uint16_t ButtonInputDisplayEnableCombo;

namespace mod {

void Mod::buttonInputDisplay()
{
  uint32_t ButtonInput = ttyd::system::keyGetButton(0);
  
  if ((ButtonInput & ButtonInputDisplayEnableCombo) == ButtonInputDisplayEnableCombo)
  {
    // Hold button to increment counter
    ++mButtonInputDisplayCounter;
  }
  else
  {
    mButtonInputDisplayCounter = 0;
  }
  
  if (mButtonInputDisplayCounter > 360)
  {
    // Enable/Disable the Button Input Display when button is held for 6 seconds
    mButtonInputDisplayEnabled = !mButtonInputDisplayEnabled;
    mButtonInputDisplayCounter = 0;
  }
  
  if (mButtonInputDisplayEnabled)
  {
    char ButtonArray[] = {'(', ')', 'v', '^', 'Z', 'R', 'L', ' ', 'A', 'B', 'X', 'Y', 'S'};
    int32_t DrawX = -252;
    
    for (int i = 0; i < 13; i++)
    {
      if (i != 7) // Skip unused input value
      {
        if (ButtonInput & (1 << i))
        {
          sprintf(mDisplayBuffer,
            "%c",
            ButtonArray[i]);
          
          ttyd::fontmgr::FontDrawStart();
          uint32_t color = 0xFFFFFFFF;
          ttyd::fontmgr::FontDrawColor(reinterpret_cast<uint8_t *>(&color));
          ttyd::fontmgr::FontDrawEdge();
          ttyd::fontmgr::FontDrawMessage(DrawX, -140, mDisplayBuffer);
        }
        
      DrawX += 20;
      }
    }
  }
}

}