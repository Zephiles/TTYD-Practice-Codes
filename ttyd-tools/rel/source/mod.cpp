#include "mod.h"
#include "patch.h"

#include <ttyd/system.h>
#include <ttyd/fontmgr.h>
#include <ttyd/dispdrv.h>
#include <ttyd/seqdrv.h>
#include <ttyd/seq_logo.h>
#include <ttyd/string.h>
#include <ttyd/swdrv.h>
#include <ttyd/global.h>

#include <cstdio>

namespace mod {

Mod *gMod = nullptr;

void main()
{
	Mod *mod = new Mod();
	mod->init();
}

Mod::Mod()
{
	
}

void Mod::init()
{
	gMod = this;
	
	mPFN_makeKey_trampoline = patch::hookFunction(ttyd::system::makeKey, []()
	{
		gMod->updateEarly();
	});

	// Initialize typesetting early
	ttyd::fontmgr::fontmgrTexSetup();
	patch::hookFunction(ttyd::fontmgr::fontmgrTexSetup, [](){});

	// Skip the logo
	/*patch::hookFunction(ttyd::seq_logo::seq_logoMain, [](ttyd::seqdrv::SeqInfo *)
	{
		ttyd::seqdrv::seqSetSeq(ttyd::seqdrv::SeqIndex::kTitle, nullptr, nullptr);
	});*/
}

void Mod::updateEarly()
{
	if ((ttyd::string::strcmp(CurrentRoom,las_25) == 0) && (ttyd::swdrv::swByteGet(0) < 390))
	{
		Mod::updateEarlyPalaceSkip();
	}
	
	else if (ttyd::string::strcmp(CurrentRoom,tik_07) == 0)
	{
		Mod::updateEarlyYoshiSkip();
	}
	
	else
	{
		mPalaceSkipTimer.stop();
		mPalaceSkipTimer.setValue(0);
		mResetCounter = 0;
	}
	
	// Check for font load
	ttyd::dispdrv::dispEntry(ttyd::dispdrv::DisplayLayer::kDebug3d, 0, [](ttyd::dispdrv::DisplayLayer layerId, void *user)
	{
		reinterpret_cast<Mod *>(user)->drawButtonsDisplay();
	}, this);
	
	// Call original function
	mPFN_makeKey_trampoline();
}

}