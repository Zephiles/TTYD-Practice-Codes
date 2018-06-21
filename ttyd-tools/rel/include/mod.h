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
  bool palaceSkipTimerPaused = false;
  bool palaceSkipInfoEnabled = false;
  
  // Yoshi Skip
  void yoshiSkip();
  Timer<uint32_t> mYoshiSkipTimer;
  bool yoshiSkipTimerPaused = false;
  bool yoshiSkipInfoEnabled = false;
  
  // --Codes--
  // Force Phantom Ember Drop
  void forcePhantomEmberDrop();
  
  // Text Storage
  void textStorage();
  bool textStorageDisable = false;
  
  // Levitate
  void levitate();
  
  // Speed Up Mario
  void speedUpMario();
  
  // Disable Battles
  void disableBattles();
  
  // Save/Load Mario/Partner Positions
  void saveLoadPositions();
  float marioSavePositions[4];
  float partnerSavePositions[4];
  
  // Reload Screen
  void reloadScreen();
  
  // On-Screen Timer
  void onScreenTimer();
  Timer<uint32_t> mOnScreenTimer;
  bool onScreenTimerEnabled = false;
  
  // Auto Action Commands
  void autoActionCommands();
  
  // Game Over
  void gameOver();
  
  // Save Anywhere
  void saveAnywhere();
  bool saveAnywhereScriptRunning = false;
  uint32_t saveAnywhereThreadID;
  
  // Adjust Coin Count + Set Sequence
  void adjustCoinCountSetSequence();
  uint16_t CoinCountButtonHoldCounter;
  
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
  bool marioHPCurrentlyLocked = false;
  bool lockMarioHPToggleDisable = false;
  
  // Change Inventory
  void changeInventory();
  uint16_t firstButtonPressed;
  uint16_t changeInventoryButtonHoldCounter;
  
  // Allow running from any fight
  void allowRunAway();
  
  // Infinite item usage
  void infiniteItemUsage();
  
  // Art Attack hitboxes
  void artAttackHitboxes();
  
  // Spawn Item
  void spawnItem();
  uint16_t spawnItemNameCounter = 0xFFFF;
  bool spawnItemDisable = false;
  void spawnItemPreventCrash();
  
  // Enable Debug Mode
  void enableDebugMode();
  
  // Display all menus inside of the Pause Menu
  void pauseMenuDisplayAllMenus();
  
  // Display Sequence Position in the Pause Menu
  void pauseMenuDisplaySequence();
  
  // Button Input Display
  void buttonInputDisplay();
  uint16_t buttonInputDisplayCounter;
  bool buttonInputDisplayEnabled = false;
  
  // Jump Storage Display
  void jumpStorageDisplay();
  uint16_t jumpStorageDisplayCounter;
  uint8_t jumpStorageButtonCounter;
  bool jumpStorageDisplayEnabled = false;
  
private:
  uint16_t mResetCounter;
  uint16_t mEnabledOrDisabledCounter;
  
  void (*mPFN_makeKey_trampoline)() = nullptr;
  char mDisplayBuffer[256];
};

}