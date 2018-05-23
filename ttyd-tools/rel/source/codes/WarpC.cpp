#include "mod.h"
#include "maps.h"

#include <ttyd/system.h>
#include <ttyd/mariost.h>
#include <ttyd/mario_pouch.h>
#include <ttyd/string.h>
#include <ttyd/swdrv.h>

#include <cstdio>

extern uint32_t r13;
extern uint16_t WarpCombo;
extern char *NextBero;
extern char *NextMap;
extern char *NextArea;

namespace mod {

void writeMap(uint32_t Source)
{
	ttyd::string::strcpy(NextMap, reinterpret_cast<char *>(Source));
	ttyd::string::strncpy(NextArea, reinterpret_cast<char *>(Source), 3);
}

void Mod::warp()
{
	if ((ttyd::system::keyGetButton(0) & WarpCombo) == WarpCombo)
	{
		if (ttyd::mariost::marioStGetSystemLevel() == 15)
		{
			// Currently in the pause menu
			#ifdef TTYD_US
			uint32_t PauseMenuAddress = r13 + 0x1D10;
			char *minnnanokoe = reinterpret_cast<char *>(0x802ECF74);
			#elif defined TTYD_JP
			uint32_t PauseMenuAddress = r13 + 0x17B0;
			char *minnnanokoe = reinterpret_cast<char *>(0x802ECB70);
			#elif defined TTYD_EU
			uint32_t PauseMenuAddress = r13 + 0x1DF0;
			char *minnnanokoe = reinterpret_cast<char *>(0x802F8BD4);
			#endif
			
			PauseMenuAddress = *reinterpret_cast<uint32_t *>(PauseMenuAddress);
			uint32_t CurrentTab = *reinterpret_cast<uint32_t *>(PauseMenuAddress + 0x40);
			int16_t CoinCount = *reinterpret_cast<int16_t *>(ttyd::mario_pouch::pouchGetPtr() + 0x78);
			
			if (CurrentTab == 0)
			{
				// Current Tab is Mario's stats
				if (CoinCount == 0)
				{
					writeMap(gor_01);
				}
				else if (CoinCount == 1)
				{
					writeMap(tik_00);
				}
				else if (CoinCount == 2)
				{
					writeMap(hei_00);
				}
				else if (CoinCount == 3)
				{
					writeMap(gon_00);
				}
				else if (CoinCount == 4)
				{
					writeMap(win_06);
				}
				else if (CoinCount == 5)
				{
					writeMap(mri_01);
				}
				else if (CoinCount == 6)
				{
					writeMap(tou_02);
				}
				else if (CoinCount == 7)
				{
					writeMap(tou_05);
				}
				else if (CoinCount == 8)
				{
					writeMap(usu_00);
				}
				else if (CoinCount == 9)
				{
					writeMap(jin_00);
				}
				else if (CoinCount == 10)
				{
					writeMap(muj_01);
				}
				else if (CoinCount == 11)
				{
					writeMap(dou_00);
				}
				else if (CoinCount == 12)
				{
					writeMap(rsh_02_a);
				}
				else if (CoinCount == 13)
				{
					writeMap(eki_00);
				}
				else if (CoinCount == 14)
				{
					writeMap(pik_00);
				}
				else if (CoinCount == 15)
				{
					writeMap(bom_00);
				}
				else if (CoinCount == 16)
				{
					writeMap(moo_00);
				}
				else if (CoinCount == 17)
				{
					writeMap(aji_00);
				}
				else if (CoinCount == 18)
				{
					writeMap(aji_13);
				}
				else if (CoinCount == 19)
				{
					writeMap(las_00);
				}
				else if (CoinCount == 20)
				{
					writeMap(las_09);
				}
				else if (CoinCount == 21)
				{
					writeMap(las_27);
				}
				else if (CoinCount == 22)
				{
					ttyd::string::strcpy(NextBero, minnnanokoe);
					writeMap(las_29);
					ttyd::swdrv::swByteSet(0, 400);
				}
			}
		}
	}
}

}