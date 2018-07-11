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
    // Hold button combo to increment counter
    ++buttonInputDisplayCounter;
  }
  else
  {
    buttonInputDisplayCounter = 0;
  }
  
  if (buttonInputDisplayCounter > 360)
  {
    // Enable/Disable the Button Input Display when button combo is held for 6 seconds
    buttonInputDisplayEnabled = !buttonInputDisplayEnabled;
    buttonInputDisplayCounter = 0;
  }
  
  if (buttonInputDisplayEnabled)
  {
    char ButtonArray[] = {'(', ')', 'v', '^', 'Z', 'R', 'L', ' ', 'A', 'B', 'X', 'Y', 'S'};
    int32_t DrawX = -252;
    
    ttyd::fontmgr::FontDrawStart();
    uint32_t color = 0xFFFFFFFF;
    ttyd::fontmgr::FontDrawColor(reinterpret_cast<uint8_t *>(&color));
    ttyd::fontmgr::FontDrawEdge();
    ttyd::fontmgr::FontDrawScale(0.75);
    
    for (int i = 0; i < 13; i++)
    {
      if (i != 7) // Skip unused input value
      {
        if (ButtonInput & (1 << i))
        {
          sprintf(mDisplayBuffer,
            "%c",
            ButtonArray[i]);
          
          ttyd::fontmgr::FontDrawString(DrawX, -160, mDisplayBuffer);
        }
        DrawX += 20;
      }
    }
  }
}

}