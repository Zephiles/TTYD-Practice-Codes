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
  
private:
  // --Tricks--
  // Palace Skip
  void palaceSkip();
  Timer<uint32_t> mPalaceSkipTimer;
  bool mPalaceSkipTimerPaused = false;
  bool mPalaceSkipInfoEnabled = false;
  
  // Yoshi Skip
  void yoshiSkip();
  Timer<uint32_t> mYoshiSkipTimer;
  bool mYoshiSkipTimerPaused = false;
  bool mYoshiSkipInfoEnabled = false;
  
  // --Codes--
  // Force Phantom Ember Drop
  void forcePhantomEmberDrop();
  
  // Text Storage
  void textStorage();
  
  // Levitate
  void levitate();
  
  // Speed Up Mario
  void speedUpMario();
  
  // Disable Battles
  void disableBattles();
  
  // Save/Load Mario/Partner Positions
  void saveLoadPositions();
  float MarioSavePositions[4];
  float PartnerSavePositions[4];
  
  // Reload Screen
  void reloadScreen();
  
  // On-Screen Timer
  void onScreenTimer();
  Timer<uint32_t> mOnScreenTimer;
  bool mOnScreenTimerEnabled = false;
  
  // Auto Action Commands
  void autoActionCommands();
  
  // Game Over
  void gameOver();
  
  // Save Anywhere
  void saveAnywhere();
  bool mSaveAnywhereScriptRunning = false;
  uint32_t mSaveAnywhereThreadID;
  
  // Adjust Coin Count + Set Sequence
  void adjustCoinCountSetSequence();
  uint32_t CoinCountButtonHoldCounter;
  
  // Warp
  void warp();
  
  // Bobbery Early
  void bobberyEarly();
  
  // Add/Remove Partners
  void addOrRemovePartners();
  bool addOrRemovePartnersDisable = false;
  
  // Reset GSWF Flags
  void resetGSWFFlags();
  
  // Lock Mario's HP, Toggle
  void lockMarioHPToggle();
  bool MarioHPCurrentlyLocked = false;
  bool lockMarioHPToggleDisable = false;
  
  // Change Inventory
  void changeInventory();
  uint16_t FirstButtonPressed;
  uint32_t ChangeInventoryButtonHoldCounter;
  
  // Allow running from any fight
  void allowRunAway();
  
  // Infinite item usage
  void infiniteItemUsage();
  
  // Art Attack hitboxes
  void artAttackHitboxes();
  
  // Spawn Item
  void spawnItem();
  uint8_t spawnItemNameCounter;
  bool spawnItemDisable = false;
  
  // Enable Debug Mode
  void enableDebugMode();
  
  // Button Input Display
  void buttonInputDisplay();
  uint32_t mButtonInputDisplayCounter;
  bool mButtonInputDisplayEnabled = false;
  
private:
  uint32_t mResetCounter;
  uint32_t mEnabledOrDisabledCounter;
  
  void (*mPFN_makeKey_trampoline)() = nullptr;
  char mDisplayBuffer[256];
};

}