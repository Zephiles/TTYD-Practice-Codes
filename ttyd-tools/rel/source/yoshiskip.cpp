#include "mod.h"

#include <ttyd/system.h>
#include <ttyd/fontmgr.h>
#include <ttyd/dispdrv.h>
#include <ttyd/seqdrv.h>
#include <ttyd/mario.h>
#include <ttyd/e_atan2.h>
#include <ttyd/global.h>

#include <cstdio>
#include <cmath>

namespace mod {

void Mod::updateEarlyYoshiSkip()
{
	ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
	uint32_t MarioControl = player->flags1 & (1 << 0); //Check if first bit is active
	
	// Check for font load
	ttyd::dispdrv::dispEntry(ttyd::dispdrv::DisplayLayer::kDebug3d, 0, [](ttyd::dispdrv::DisplayLayer layerId, void *user)
	{
		reinterpret_cast<Mod *>(user)->drawYoshiSkip();
	}, this);
	
	// Yoshi skip timing code
	if (ttyd::seqdrv::seqGetSeq() == 4) //In battle
	{
	// Pause in battle
	mPalaceSkipTimer.stop();
	mPaused = true;
	}
	else if (MarioControl && mPaused)
	{
		// Reset and Start when leaving battle
		mPalaceSkipTimer.setValue(0);
		mPalaceSkipTimer.start();
		mPaused = false;
	}
	
	if (ttyd::system::keyGetButtonTrg(0) & PAD_A)
	{
		// Stop when pressing A
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

float getStickValue(float Stick)
{
	if (Stick > 127)
	{
		Stick -= 256;
	}
	return Stick;
}

void Mod::drawYoshiSkip()
{
	ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
	float StickX = getStickValue(ttyd::system::keyGetStickX(0));
	float StickY = getStickValue(ttyd::system::keyGetStickY(0));
	
	double stickAngle = (ttyd::e_atan2::__ieee754_atan2(StickX,StickY)) * (180 / M_PI);
	if (stickAngle < 0)
	{
		stickAngle += 360;
	}
	
	sprintf(mDisplayBuffer,
		"YST: %lu\r\nStickAngle: %.2f\r\nMarPos: %.2f %.2f %.2f",
		mPalaceSkipTimer.getValue(),
		stickAngle,
		player->playerPosition[0], player->playerPosition[1], player->playerPosition[2]);
	
	ttyd::fontmgr::FontDrawStart();
	uint32_t color = 0xFFFFFFFF;
	ttyd::fontmgr::FontDrawColor(reinterpret_cast<uint8_t *>(&color));
	ttyd::fontmgr::FontDrawEdge();
	ttyd::fontmgr::FontDrawMessage(-252, -63, mDisplayBuffer);
}

}