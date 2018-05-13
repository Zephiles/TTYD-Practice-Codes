#pragma once

#include "timer.h"

#include <cstdint>

namespace mod {

class Mod
{
public:
	Mod();
	void init();
	
private:
	void updateEarly();
	
	//Palace Skip
	void updateEarlyPalaceSkip();
	void drawPalaceSkip();
	
	//Yoshi Skip
	void updateEarlyYoshiSkip();
	void drawYoshiSkip();
	
	//Button Display
	void drawButtonsDisplay();
	
private:
	Timer<uint32_t> mPalaceSkipTimer;
	bool mPaused = false;
	
	void (*mPFN_makeKey_trampoline)() = nullptr;
	char mDisplayBuffer[256];
};

}