#include "mod.h"
#include "patch.h"

#include <ttyd/system.h>
#include <ttyd/fontmgr.h>
#include <ttyd/seq_logo.h>
#include <ttyd/dispdrv.h>

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
  
  // --Codes (Only run once)--
  Mod::allowRunAway();
  Mod::enableDebugMode();
  
  //--Codes (Only run once - Assembly)--
  Mod::forcePhantomEmberDrop();
  Mod::autoActionCommands();
  Mod::disableBattles();
  Mod::infiniteItemUsage();
  Mod::artAttackHitboxes();
  Mod::spawnItemPreventCrash();
  Mod::pauseMenuDisplayAllMenus();
}

void Mod::updateEarly()
{
  // --Codes (No Display)--
  Mod::textStorage();
  Mod::levitate();
  Mod::speedUpMario();
  Mod::saveLoadPositions();
  Mod::reloadScreen();
  Mod::gameOver();
  Mod::adjustCoinCountSetSequence();
  Mod::warp();
  Mod::bobberyEarly();
  Mod::addOrRemovePartners();
  Mod::resetGSWFFlags();
  Mod::lockMarioHPToggle();
  Mod::saveAnywhere();
  Mod::changeInventory();
  Mod::spawnItem();
  
  // Check for font load
  ttyd::dispdrv::dispEntry(ttyd::dispdrv::DisplayLayer::kDebug3d, 0, [](ttyd::dispdrv::DisplayLayer layerId, void *user)
  {
    // --Tricks (With Display)--
    reinterpret_cast<Mod *>(user)->palaceSkip();
    reinterpret_cast<Mod *>(user)->yoshiSkip();
  
    // --Codes (With Display)--
    reinterpret_cast<Mod *>(user)->onScreenTimer();
    reinterpret_cast<Mod *>(user)->buttonInputDisplay();
  }, this);
  
  // Call original function
  mPFN_makeKey_trampoline();
}

}