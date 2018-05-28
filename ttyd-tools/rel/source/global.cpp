#include "buttons.h"

#include <cstdint>

extern "C" {

// --Global Variables--
#ifdef TTYD_US
  uint32_t r13 = 0x8041CF20;
  uint32_t NPCAddressesStart = r13 + 0x19A0;
  uint32_t PauseMenuAddressesStart = r13 + 0x1D10;
  uint32_t BattleAddressesStart = r13 + 0x1BE0;
  char *NextBero = reinterpret_cast<char *>(r13 + 0x1688);
  char *NextMap = reinterpret_cast<char *>(r13 + 0x16A8);
  char *NextArea = reinterpret_cast<char *>(r13 + 0x16C8);
#elif defined TTYD_JP
  uint32_t r13 = 0x80417260;
  uint32_t NPCAddressesStart = r13 + 0x1448;
  uint32_t PauseMenuAddressesStart  = r13 + 0x17B0;
  uint32_t BattleAddressesStart  = r13 + 0x1710;
  char *NextBero = reinterpret_cast<char *>(r13 + 0x1128);
  char *NextMap = reinterpret_cast<char *>(r13 + 0x1148);
  char *NextArea = reinterpret_cast<char *>(r13 + 0x1168);
#elif defined TTYD_EU
  uint32_t r13 = 0x80429760;
  uint32_t NPCAddressesStart = r13 + 0x1A80;
  uint32_t PauseMenuAddressesStart  = r13 + 0x1DF0;
  uint32_t BattleAddressesStart  = r13 + 0x1CC0;
  char *NextBero = reinterpret_cast<char *>(r13 + 0x1768);
  char *NextMap = reinterpret_cast<char *>(r13 + 0x1788);
  char *NextArea = reinterpret_cast<char *>(r13 + 0x17A8);
#endif

uint32_t GSWAddressesStart = r13 - 0x6F50;
bool ArtAttackHitboxesEnabled = false;
bool ArtAttackHitboxesDenyInputs = false;

// --Button Combinations--
// Palace Skip
uint16_t PalaceSkipEnableCombo = PAD_X | PAD_Z;

// Yoshi Skip
uint16_t YoshiSkipEnableCombo = PAD_X | PAD_Z;

// Text Storage
uint16_t TextStorageCombo = PAD_L | PAD_X;
uint16_t TimeStopTextStorageCombo = PAD_L | PAD_R;

// Levitate
uint16_t LevitateCombo = PAD_L | PAD_A;

// Speed Up Mario
uint16_t SpeedUpMarioCombo = PAD_L | PAD_Y;

// Disable Battles
uint16_t DisableBattlesCombo = PAD_L | PAD_Y;

// Save/Load Mario/Partner Positions
uint16_t SavePositions = PAD_Y | PAD_Z;
uint16_t LoadPositions = PAD_L | PAD_START;

// Reload Screen
uint16_t ReloadScreenCombo = PAD_L | PAD_B;

// On-Screen Timer
uint16_t EnableOrResetTimerCombo = PAD_L | PAD_DPAD_RIGHT;
uint16_t DisableTimerCombo = PAD_L | PAD_DPAD_LEFT;
uint16_t StartOrResumeTimerCombo = PAD_L | PAD_Z;
uint16_t PauseTimerCombo = PAD_L | PAD_DPAD_UP;

// Auto Action Commands
uint16_t AutoActionCommandsCombo = PAD_R;

// Game Over
uint16_t GameOverCombo = PAD_L | PAD_Y | PAD_X;

// Adjust Coin Count + Set Sequence
uint16_t CoinsSetToZeroCombo = PAD_R | PAD_Z;
uint16_t CoinsSetSequenceCombo = PAD_R | PAD_X;
uint16_t CoinsIncrement1Combo = PAD_R | PAD_DPAD_UP;
uint16_t CoinsIncrement10Combo = PAD_Y | PAD_DPAD_UP;
uint16_t CoinsIncrement100Combo = PAD_Z | PAD_DPAD_UP;
uint16_t CoinsDecrement1Combo = PAD_R | PAD_DPAD_DOWN;
uint16_t CoinsDecrement10Combo = PAD_Y | PAD_DPAD_DOWN;
uint16_t CoinsDecrement100Combo = PAD_Z | PAD_DPAD_DOWN;

// Warp
uint16_t WarpCombo = PAD_A | PAD_Y;

// Add/Remove Partners
uint16_t AddPartnersCombo = PAD_R | PAD_Z;
uint16_t RemovePartnersCombo = PAD_R | PAD_Y;

// Reset GSWF Flags
uint16_t ResetGSWFFlagsCombo = PAD_R | PAD_B | PAD_X;

// Lock Mario's HP; Toggle
uint16_t LockMarioHPToggleCombo = PAD_R | PAD_Y | PAD_X;

// Save Anywhere
uint16_t SaveAnywhereCombo = PAD_Y | PAD_B;

// Change Inventory
uint16_t InventoryAddItemOrBadgeCombo = PAD_A | PAD_Y;
uint16_t InventoryRemoveItemOrBadgeCombo = PAD_A | PAD_X;
uint16_t InventoryPreviousValueCombo = PAD_X | PAD_Y;
uint16_t InventoryNextValueCombo = PAD_Y | PAD_X;
uint16_t InventoryAddItemOrBadgeFirstButtonCombo = PAD_A; // Must be the first button from InventoryAddItemOrBadge
uint16_t InventoryRemoveItemOrBadgeFirstButtonCombo = PAD_A; // Must be the first button from InventoryRemoveItemOrBadge
uint16_t InventoryPreviousValueFirstButtonCombo = PAD_X; // Must be the first button from InventoryPreviousValue
uint16_t InventoryNextValueFirstButtonCombo = PAD_Y; // Must be the first button from InventoryNextValue

// Button Input Display
uint16_t ButtonInputDisplayEnableCombo = PAD_B | PAD_X;

// Infinite Item Usage
uint16_t InfiniteItemsUsageCombo = PAD_Y;

// Art Attack Hitboxes
uint16_t ArtAttackHitboxesCombo = PAD_R | PAD_A;

// Spawn Item
uint16_t SpawnItemCombo = PAD_L | PAD_DPAD_DOWN;
}