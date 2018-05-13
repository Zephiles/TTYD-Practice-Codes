#include "mod.h"

#include <ttyd/system.h>
#include <ttyd/fontmgr.h>

#include <cstdio>

namespace mod {

void Mod::drawButtonsDisplay()
{
	char ButtonArray[] = {'(', ')', 'v', '^', 'Z', 'R', 'L', ' ', 'A', 'B', 'X', 'Y', 'S'};
	int DrawX = -252;
	
	for (int i=0;i<13;i++)
	{
		if (i != 7) //Skip unused input value
		{
			if (ttyd::system::keyGetButton(0) & (1 << i))
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