#include "mod.h"

#include <ttyd/system.h>
#include <ttyd/mariost.h>
#include <ttyd/fontmgr.h>
#include <ttyd/dispdrv.h>
#include <ttyd/mario.h>
#include <ttyd/mario_party.h>
#include <ttyd/party.h>
#include <ttyd/global.h>

#include <cstdio>

namespace mod {

void Mod::updateEarlyPalaceSkip()
{
	// Check for font load
	ttyd::dispdrv::dispEntry(ttyd::dispdrv::DisplayLayer::kDebug3d, 0, [](ttyd::dispdrv::DisplayLayer layerId, void *user)
	{
		reinterpret_cast<Mod *>(user)->drawPalaceSkip();
	}, this);
	
	// Palace skip timing code
	if (ttyd::mariost::marioStGetSystemLevel() == 0xF)
	{
	// Stop upon pausing
	mPalaceSkipTimer.stop();
	//mPalaceSkipTimer.setValue(0);
	mPaused = true;
	}
	else if (ttyd::mariost::marioStGetSystemLevel() == 0 && mPaused)
	{
		// Reset and Start when unpausing
		mPalaceSkipTimer.setValue(0);
		mPalaceSkipTimer.start();
		mPaused = false;
	}
	
	if (ttyd::system::keyGetButtonTrg(0) & PAD_X)
	{
		// Stop when pressing X
		mPalaceSkipTimer.stop();
	}
	
	if (ttyd::system::keyGetButton(0) & PAD_Y)
	{
		++mResetCounter;
	}
	else
	{
		mResetCounter = 0;
	}
	
	if (mResetCounter > 120)
	{
		mPalaceSkipTimer.stop();
		mPalaceSkipTimer.setValue(0);
		mResetCounter = 0;
	}
	
	mPalaceSkipTimer.tick();
}

void Mod::drawPalaceSkip()
{
	#ifdef TTYD_US
	float *phantomEmberPos = *reinterpret_cast<float **>(r13 + 0x19A0) + 243;
	uint32_t *ItemArrayPointer = *reinterpret_cast<uint32_t **>(0x803DC294);
	#endif
	
	#ifdef TTYD_JP
	float *phantomEmberPos = *reinterpret_cast<float **>(r13 + 0x1448) + 243;
	uint32_t *ItemArrayPointer = *reinterpret_cast<uint32_t **>(0x803D8714);
	#endif
	
	#ifdef TTYD_EU
	float *phantomEmberPos = *reinterpret_cast<float **>(r13 + 0x1A80) + 243;
	uint32_t *ItemArrayPointer = *reinterpret_cast<uint32_t **>(0x803E82F4);
	#endif
	
	//float *marioPos = *reinterpret_cast<float **>(r13 + 0x19E0) + 35;
	float *marioPos = (float *) ttyd::mario::marioGetPtr() + 35;
	uint32_t PartnerPointer = ttyd::party::partyGetPtr(ttyd::mario_party::marioGetPartyId());
	float *partnerPos = &NoPointer;
	//float *phantomEmberPos = *reinterpret_cast<float **>(r13 + 0x19A0) + 243;
	//uint32_t *ItemArrayPointer = *reinterpret_cast<uint32_t **>(0x803DC294);
	uint32_t *Item = *reinterpret_cast<uint32_t **>(ItemArrayPointer + 1);
	bool FoundItem = false;
	
	if (PartnerPointer != 0)
	{
		partnerPos = (float *) (PartnerPointer + 0x58 + 0x4); //Y Coordinate
	}
	
	for (int i=0; i<32; i++)
	{
		uint32_t tempitem = (uint32_t) Item;
		uint32_t tempoffset;
		uint16_t tempitemaction;
		uint16_t tempbit;
		
		if ((tempitem == 0) || (tempitem == 121) || (tempitem == 123) || (tempitem == 124))
		{
			tempoffset = (i + 1) * 0x26;
			Item = *reinterpret_cast<uint32_t **>(ItemArrayPointer + tempoffset + 1);
		}
		else
		{
			tempoffset = i * 0x26;
			tempitemaction = *reinterpret_cast<uint16_t *>(ItemArrayPointer + tempoffset + 9);
			if (tempitemaction == 7) //Item is already spawned upon entering the room
			{
				tempoffset = (i + 1) * 0x26;
				Item = *reinterpret_cast<uint32_t **>(ItemArrayPointer + tempoffset + 1);
			}
			else
			{
				tempbit = *reinterpret_cast<uint16_t *>(ItemArrayPointer + tempoffset);
				tempbit = tempbit & (1 << 0); //Check if first bit is active
				if (tempbit) //First bit is active
				{
					Item = *reinterpret_cast<uint32_t **>(ItemArrayPointer + tempoffset + 31);
					FoundItem = true;
					break;
				}
				else
				{
					tempoffset = (i + 1) * 0x26;
					Item = *reinterpret_cast<uint32_t **>(ItemArrayPointer + tempoffset + 1);
				}
			}
		}
	}
	
	if (FoundItem == false)
	{
		Item = (uint32_t *) 0;
	}
	
	sprintf(mDisplayBuffer,
		"PST: %lu\r\nItemTimer: %d\r\nPhaEmY: %.2f\r\nParY: %.2f\r\nMarPos: %.2f %.2f %.2f",
		mPalaceSkipTimer.getValue(),
		Item,
		phantomEmberPos[1],
		partnerPos[0],
		marioPos[0], marioPos[1], marioPos[2]);
	
	ttyd::fontmgr::FontDrawStart();
	uint32_t color = 0xFFFFFFFF;
	ttyd::fontmgr::FontDrawColor(reinterpret_cast<uint8_t *>(&color));
	ttyd::fontmgr::FontDrawEdge();
	ttyd::fontmgr::FontDrawMessage(-252, -5, mDisplayBuffer);
}

}