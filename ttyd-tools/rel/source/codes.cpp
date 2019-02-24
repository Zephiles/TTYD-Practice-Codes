#include "mod.h"
#include "codes.h"
#include "commonfunctions.h"
#include "draw.h"
#include "patch.h"
#include "menufunctions.h"
#include "global.h"
#include "maps.h"

#include <ttyd/system.h>
#include <ttyd/mario.h>
#include <ttyd/evtmgr.h>
#include <ttyd/string.h>
#include <ttyd/camdrv.h>
#include <ttyd/pmario_sound.h>
#include <ttyd/mario_cam.h>
#include <ttyd/mario_pouch.h>
#include <ttyd/swdrv.h>
#include <ttyd/win_main.h>
#include <ttyd/itemdrv.h>
#include <ttyd/battle_ac.h>

#include <cstdio>

namespace mod {

// Functions accessed by assembly overwrites
extern "C" {
uint32_t disableBattles(void *ptr)
{
	if (Cheat[DISABLE_BATTLES].Active)
	{
		if (checkButtonComboEveryFrame(Cheat[DISABLE_BATTLES].ButtonCombo))
		{
			return 0;
		}
	}
	
	return *reinterpret_cast<uint32_t *>(
		reinterpret_cast<uint32_t>(ptr) + 0x4);
}

void *autoActionCommands(void *ptr)
{
	uint32_t tempPtr = reinterpret_cast<uint32_t>(ptr);
	
	if (Cheat[AUTO_ACTION_COMMANDS].Active)
	{
		#ifdef TTYD_US
		const uint32_t StoreByteOffset = 0x307;
		#elif defined TTYD_JP
		const uint32_t StoreByteOffset = 0x303;
		#elif defined TTYD_EU
		const uint32_t StoreByteOffset = 0x30B;
		#endif
		
		uint32_t tempStoreAddress = *reinterpret_cast<uint32_t *>(tempPtr + 0x1C90);
		if (tempStoreAddress)
		{
			if (checkButtonComboEveryFrame(Cheat[AUTO_ACTION_COMMANDS].ButtonCombo))
			{
				*reinterpret_cast<bool *>(tempStoreAddress + StoreByteOffset) = true;
			}
			else
			{
				*reinterpret_cast<bool *>(tempStoreAddress + StoreByteOffset) = false;
			}
		}
	}
	
	return *reinterpret_cast<uint32_t **>(tempPtr + 0xEF4);
}

uint32_t allowRunningFromBattles(void *ptr)
{
	if (Cheat[RUN_FROM_BATTLES].Active)
	{
		// Force the branch to be taken
		return 0;
	}
	
	return *reinterpret_cast<uint32_t *>(
		reinterpret_cast<uint32_t>(ptr) + 0x4);
}

int32_t forceNPCItemDrop(void *ptr)
{
	if (Cheat[FORCE_ITEM_DROP].Active)
	{
		return static_cast<int32_t>(ForcedNPCItemDrop);
	}
	
	return *reinterpret_cast<int32_t *>(
		reinterpret_cast<uint32_t>(ptr) + 0x23C);
}

bool checkForArtAttackHitboxesBool()
{
	return Displays[ART_ATTACK_HITBOXES];
}
}

void walkThroughMostObjects()
{
	if (!Cheat[WALK_THROUGH_WALLS].Active || 
		!checkButtonComboEveryFrame(Cheat[WALK_THROUGH_WALLS].ButtonCombo))
	{
		// Check to see if Mario's properties should be reset
		if (ResetMarioProperties)
		{
			ResetMarioProperties = false;
			
			ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
			player->flags1 &= ~(1 << 10); // Turn off the 10 bit
		}
		return;
	}
	
	// Set Mario;s properties to be able to walk through most objects
	ResetMarioProperties = true;
	
	ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
	player->flags1 |= (1 << 10); // Turn on the 10 bit
}

void saveMarioAndPartnerPositions()
{
	if (!Cheat[SAVE_COORDINATES].Active)
	{
		return;
	}
	
	if (!checkButtonCombo(Cheat[SAVE_COORDINATES].ButtonCombo))
	{
		return;
	}
	
	ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
	
	MarioPartnerPositions.MarioPosition[0] = player->playerPosition[0]; // Mario Coordinate X
	MarioPartnerPositions.MarioPosition[1] = player->playerPosition[1]; // Mario Coordinate Y
	MarioPartnerPositions.MarioPosition[2] = player->playerPosition[2]; // Mario Coordinate Z
	MarioPartnerPositions.MarioPosition[3] = player->unk_1a0;
	
	uint32_t PartnerPointer = reinterpret_cast<uint32_t>(getPartnerPointer());
	if (PartnerPointer != 0)
	{
		MarioPartnerPositions.PartnerPosition[0] = *reinterpret_cast<float *>(PartnerPointer + 0x58); // Partner Coordinate X
		MarioPartnerPositions.PartnerPosition[1] = *reinterpret_cast<float *>(PartnerPointer + 0x5C); // Partner Coordinate Y
		MarioPartnerPositions.PartnerPosition[2] = *reinterpret_cast<float *>(PartnerPointer + 0x60); // Partner Coordinate Z
		MarioPartnerPositions.PartnerPosition[3] = *reinterpret_cast<float *>(PartnerPointer + 0x100);
	}
}

void loadMarioAndPartnerPositions()
{
	if (!Cheat[LOAD_COORDINATES].Active)
	{
		return;
	}
	
	if (!checkButtonCombo(Cheat[LOAD_COORDINATES].ButtonCombo))
	{
		return;
	}
	
	ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
	
	player->playerPosition[0] 	= MarioPartnerPositions.MarioPosition[0]; // Mario Coordinate X
	player->playerPosition[1] 	= MarioPartnerPositions.MarioPosition[1]; // Mario Coordinate Y
	player->playerPosition[2] 	= MarioPartnerPositions.MarioPosition[2]; // Mario Coordinate Z
	player->unk_1a0 			= MarioPartnerPositions.MarioPosition[3];
	
	uint32_t PartnerPointer = reinterpret_cast<uint32_t>(getPartnerPointer());
	if (PartnerPointer != 0)
	{
		*reinterpret_cast<float *>(PartnerPointer + 0x58) 	= MarioPartnerPositions.PartnerPosition[0]; // Partner Coordinate X
		*reinterpret_cast<float *>(PartnerPointer + 0x5C) 	= MarioPartnerPositions.PartnerPosition[1]; // Partner Coordinate Y
		*reinterpret_cast<float *>(PartnerPointer + 0x60) 	= MarioPartnerPositions.PartnerPosition[2]; // Partner Coordinate Z
		*reinterpret_cast<float *>(PartnerPointer + 0x100) 	= MarioPartnerPositions.PartnerPosition[3];
	}
}

void saveAnywhere()
{
	uint32_t SystemLevel = getSystemLevel();
	if (!SaveAnywhere.ScriptIsRunning)
	{
		if (!Cheat[SAVE_ANYWHERE].Active || ChangingCheatButtonCombo)
		{
			return;
		}
		
		// Save script is not currently running
		if (checkButtonCombo(Cheat[SAVE_ANYWHERE].ButtonCombo))
		{
			// Make sure a file is currently loaded, and that the pause menu is not currently open
			if (checkIfInGame() && (SystemLevel != 15))
			{
				#ifdef TTYD_US
				void *SaveScript = reinterpret_cast<void *>(0x803BAC3C);
				#elif defined TTYD_JP
				void *SaveScript = reinterpret_cast<void *>(0x803B68BC);
				#elif defined TTYD_EU
				void *SaveScript = reinterpret_cast<void *>(0x803C6C4C);
				#endif
				
				// Take away control from the player and start the Save script
				setSystemLevel(SystemLevel + 1);
				
				SaveAnywhere.ThreadID = *reinterpret_cast<uint32_t *>(
					ttyd::evtmgr::evtEntryType(SaveScript, 0, 0, 0) + 0x15C);
				
				SaveAnywhere.ScriptIsRunning = true;
				
				// Only turn the key off if it's not already off
				if (ttyd::mario::marioKeyOffChk() == 0)
				{
					ttyd::mario::marioKeyOff();
				}
			}
		}
	}
	else if (!ttyd::evtmgr::evtCheckID(SaveAnywhere.ThreadID))
	{
		// Save Script is no longer running, so give back control to the player
		SaveAnywhere.ScriptIsRunning = false;
		
		// Only turn the key on if it's not already on
		if (ttyd::mario::marioKeyOffChk() != 0)
		{
			ttyd::mario::marioKeyOn();
		}
		
		// Only lower the system level if it's not currently at 0
		if (SystemLevel > 0)
		{
			setSystemLevel(SystemLevel - 1);
		}
	}
}

void setTextStorage()
{
	if (!Cheat[TEXT_STORAGE].Active)
	{
		return;
	}
	
	if (!checkButtonCombo(Cheat[TEXT_STORAGE].ButtonCombo))
	{
		return;
	}
	
	uint32_t PartnerPointer = reinterpret_cast<uint32_t>(getPartnerPointer());
	if (PartnerPointer != 0)
	{
		*reinterpret_cast<uint8_t *>(PartnerPointer + 0x39) = 0;
	}
}

void setTimeStopTextStorage()
{
	if (!Cheat[TIME_STOP_TEXT_STORAGE].Active)
	{
		return;
	}
	
	if (!checkButtonCombo(Cheat[TIME_STOP_TEXT_STORAGE].ButtonCombo))
	{
		return;
	}
	
	uint32_t PartnerPointer = reinterpret_cast<uint32_t>(getPartnerPointer());
	if (PartnerPointer != 0)
	{
		*reinterpret_cast<uint8_t *>(PartnerPointer + 0x39) = 21;
	}
}

void speedUpMario()
{
	if (!Cheat[SPEED_UP_MARIO].Active)
	{
		return;
	}
	
	ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
	
	if (checkButtonComboEveryFrame(Cheat[SPEED_UP_MARIO].ButtonCombo))
	{
		player->wPlayerBaseSpeed = 16;
		player->unk_184 = 16;
		player->unk_188 = 16;
	}
	else
	{
		player->unk_184 = 1;
		player->unk_188 = 2.25;
	}
}

bool Mod::infiniteItemUsage(int16_t item, uint32_t index)
{
	if (Cheat[INFINITE_ITEM_USAGE].Active)
	{
		if (checkButtonComboEveryFrame(Cheat[INFINITE_ITEM_USAGE].ButtonCombo))
		{
			// Prevent the function from running
			return false;
		}
	}
	
	// Call original function
	return mPFN_pouchRemoveItemIndex_trampoline(item, index);
}

void checkIfSystemLevelShouldBeLowered()
{
	// Only run when the player has either reloaded the room or warped
	if (!ReloadRoom.SystemLevelShouldBeLowered)
	{
		return;
	}
	
	// Keep lowering the system level until the seq changes to Game
	if (!checkForSpecificSeq(ttyd::seqdrv::SeqIndex::kGame))
	{
		// Only lower the system level if it's not currently at 0
		uint32_t SystemLevel = getSystemLevel();
		if (SystemLevel > 0)
		{
			setSystemLevel(0);
		}
	}
	else
	{
		ReloadRoom.SystemLevelShouldBeLowered = false;
	}
}

void reloadRoomMain()
{
	ttyd::seqdrv::SeqIndex NextSeq 		= ttyd::seqdrv::seqGetNextSeq();
	ttyd::seqdrv::SeqIndex Game 		= ttyd::seqdrv::SeqIndex::kGame;
	ttyd::seqdrv::SeqIndex MapChange 	= ttyd::seqdrv::SeqIndex::kMapChange;
	
	// Only run while a file is loaded
	if ((NextSeq < Game) || (NextSeq > MapChange))
	{
		return;
	}
	
	ttyd::seqdrv::SeqIndex Seq = ttyd::seqdrv::seqGetSeq();
	ttyd::seqdrv::SeqIndex Battle = ttyd::seqdrv::SeqIndex::kBattle;
	
	if (Seq == Battle)
	{
		// Reloading the room with the Seq set to Battle will cause the game to crash, so don't allow it
		return;
	}
	
	// A separate address for NextBero is needed, as the original value will be cleared during the reloading process
	// The game will crash if NextMap is used directly in seqSetSeq, so a separate address must be used instead
	// NewBero and NewMap need to be global variables
	
	char *tempNewBero = ReloadRoom.NewBero;
	char *tempNewMap = ReloadRoom.NewMap;
	
	ttyd::string::strcpy(tempNewBero, NextBero);
	ttyd::string::strcpy(tempNewMap, NextMap);
	setSeq(tempNewMap, tempNewBero);
	
	// Reset the black screen fade effect set when loading into a room via a pipe
	*reinterpret_cast<uint16_t *>(
		*reinterpret_cast<uint32_t *>(wp_fadedrv_Address)) &= 
			~(1 << 15); // Turn off the 15 bit
	
	// Reset the camera - mainly for the black bars at the top and bottom of the screen
	uint32_t CameraPointer = reinterpret_cast<uint32_t>(ttyd::camdrv::camGetPtr(8));
	*reinterpret_cast<uint16_t *>(CameraPointer) &= 
		~((1 << 8) | (1 << 9)); // Turn off the 8 and 9 bits
	
	uint32_t SystemLevel = getSystemLevel();
	if (SystemLevel == 0)
	{
		return;
	}
	
	// Only run the following if the system level is not 0
	if (SystemLevel == 15)
	{
		// Currently in pause menu, so re-enable the camera
		ttyd::camdrv::camDispOn(4);
	}
	
	// Enable sound effects, set the default camera id for Mario, and give back control to the player
	ttyd::pmario_sound::psndClearFlag(0x80);
	ttyd::mario_cam::marioSetCamId(4);
	setSystemLevel(0);
	ReloadRoom.SystemLevelShouldBeLowered = true;
}

void reloadRoom()
{
	if (checkButtonCombo(Cheat[RELOAD_ROOM].ButtonCombo))
	{
		// Prevent being able to reload the room if the menu is open or if currently in the spawn item menu
		if (Cheat[RELOAD_ROOM].Active && !MenuIsDisplayed && 
			!SpawnItem.InAdjustableValueMenu)
		{
			
			{
				reloadRoomMain();
			}
		}
	}
	
	// Manually reset the System Level and the camera if currently getting an item while the room is trying to load
	ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
	const uint16_t ReceivingItem = 15;
	
	ttyd::seqdrv::SeqIndex NextSeq 		= ttyd::seqdrv::seqGetNextSeq();
	ttyd::seqdrv::SeqIndex MapChange 	= ttyd::seqdrv::SeqIndex::kMapChange;
	
	if ((player->currentMotionId == ReceivingItem) && (NextSeq == MapChange))
	{
		// Reset the System Level
		ReloadRoom.SystemLevelShouldBeLowered = true;
		
		// Reset the camera - mainly for the black bars at the top and bottom of the screen
		uint32_t CameraPointer = reinterpret_cast<uint32_t>(ttyd::camdrv::camGetPtr(8));
		*reinterpret_cast<uint16_t *>(CameraPointer) &= 
			~((1 << 8) | (1 << 9)); // Turn off the 8 and 9 bits
	}
}

void levitate()
{
	if (!Cheat[LEVITATE].Active)
	{
		return;
	}
	
	if (!checkButtonComboEveryFrame(Cheat[LEVITATE].ButtonCombo))
	{
		return;
	}
	
	ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
	player->wJumpVelocityY = 4.540000915527344;
}

void lockMarioHPToMax()
{
	if (!Cheat[LOCK_MARIO_HP_TO_MAX].Active)
	{
		return;
	}
	
	uint32_t PouchAddress = reinterpret_cast<uint32_t>(
		ttyd::mario_pouch::pouchGetPtr());
	
	uint32_t MarioHPAddress = PouchAddress + 0x70;
	int16_t MarioMaxHP = *reinterpret_cast<int16_t *>(PouchAddress + 0x72);
	*reinterpret_cast<int16_t *>(MarioHPAddress) = MarioMaxHP; // Copy Max HP to Current HP
	
	uint32_t MarioBattleAddress = reinterpret_cast<uint32_t>(getMarioBattlePointer());
	if (MarioBattleAddress != 0)
	{
		uint32_t BattleMarioHPAddress = MarioBattleAddress + 0x10C;
		int16_t BattleMarioMaxHP = *reinterpret_cast<int16_t *>(
			MarioBattleAddress + 0x108);
		
		*reinterpret_cast<int16_t *>(BattleMarioHPAddress) = BattleMarioMaxHP; // Copy Battle Max HP to Current Battle HP
	}
}

void bobberyEarly()
{
	if (checkForSpecificSeq(ttyd::seqdrv::SeqIndex::kMapChange))
	{
		MarioFreeze = false;
	}
	
	if (MarioFreeze)
	{
		uint32_t RopeAddress = *reinterpret_cast<uint32_t *>(_mapEntAddress);
		RopeAddress = *reinterpret_cast<uint32_t *>(RopeAddress + 0x154);
		float RopePosZ = *reinterpret_cast<float *>(RopeAddress + 0x8B4);
		
		ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
		player->flags1 |= (1 << 1); // Turn on the 1 bit
		player->wJumpVelocityY = 0;
		player->wJumpAccelerationY = 0;
		player->unk_84 = 0;
		player->playerPosition[0] = -1;
		player->playerPosition[1] = 59;
		player->playerPosition[2] = RopePosZ + 10;
		
		#ifdef TTYD_JP
		player->unk_19c = 180;
		player->wPlayerDirection = 180;
		#else
		player->unk_1a0 = 180;
		player->unk_1b0 = 180;
		#endif
	}
	
	if (!Cheat[BOBBERY_EARLY].Active)
	{
		return;
	}
	
	// Turn off GSWF(3137)
	ttyd::swdrv::swClear(3137);
	
	if (compareStringToNextMap(reinterpret_cast<const char *>(muj_05)))
	{
		uint32_t NPC_6_Addresses_Start = (*reinterpret_cast<uint32_t *>(
			NPCAddressesStart)) + 0x1040;
		
		// Allow the Ember to be refought
		*reinterpret_cast<uint8_t *>(NPC_6_Addresses_Start + 0x1D7) = 0;
	}
}

void spawnItem()
{
	if (checkForSpecificSeq(ttyd::seqdrv::SeqIndex::kMapChange))
	{
		SpawnItem.SpawnItemCounter = 0;
	}
	
	bool tempInAdjustableValueMenu = SpawnItem.InAdjustableValueMenu;
	if (Cheat[SPAWN_ITEM].Active && !ChangingCheatButtonCombo)
	{
		uint32_t SystemLevel = getSystemLevel();
		if (checkIfInGame() && (SystemLevel != 15))
		{
			// Currently not in a battle and the pause menu is not open
			// Check to see if the adjustable value menu is open or not
			if (checkButtonCombo(Cheat[SPAWN_ITEM].ButtonCombo) && 
				!tempInAdjustableValueMenu)
			{
				// Not open, so disable the pause menu, raise the system level, and open the menu
				ttyd::win_main::winOpenDisable();
				raiseSystemLevel();
				SecondaryMenuOption = getHighestAdjustableValueDigit(INVENTORY_STANDARD) - 1;
				SpawnItem.InAdjustableValueMenu = true;
			}
			
			if (tempInAdjustableValueMenu)
			{
				// Check for button inputs for the menu
				uint32_t PressedButton = adjustableValueButtonControls(SPAWN_ITEM_MENU_VALUE);
				switch (PressedButton)
				{
					case A:
					{
						// Spawn the current item
						ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
						float CoordinateXAdjustment;
						
						if (player->wPlayerDirection <= 0)
						{
							// Mario is facing left
							CoordinateXAdjustment = -30;
						}
						else
						{
							// Mario is facing right
							CoordinateXAdjustment = 30;
						}
						
						float ItemCoordinateX = player->playerPosition[0] + CoordinateXAdjustment;
						float ItemCoordinateY = player->playerPosition[1];
						float ItemCoordinateZ = player->playerPosition[2];
						
						SpawnItem.SpawnItemCounter++;
						
						char *tempDisplayBuffer = DisplayBuffer;
						sprintf(tempDisplayBuffer,
							"C_Item%d",
							SpawnItem.SpawnItemCounter);
						
						ttyd::itemdrv::itemEntry(tempDisplayBuffer, MenuSecondaryValue, 16, 
							-1, nullptr, ItemCoordinateX, ItemCoordinateY, ItemCoordinateZ);
						
						// Enable the pause menu and lower the system level
						SpawnItem.InAdjustableValueMenu = false;
						ttyd::win_main::winOpenEnable();
						lowerSystemLevel();
						return;
					}
					case B:
					{
						// Enable the pause menu and lower the system level
						SpawnItem.InAdjustableValueMenu = false;
						ttyd::win_main::winOpenEnable();
						lowerSystemLevel();
						return;
					}
					case NO_NUMBERS_TO_DISPLAY:
					{
						// There are no digits of the number to display
						SpawnItem.InAdjustableValueMenu = false;
						ttyd::win_main::winOpenEnable();
						lowerSystemLevel();
						return;
					}
					default:
					{
						break;
					}
				}
				
				// Draw the menu
				drawFunctionOnDebugLayer(drawAdjustableValueSpawnItem);
			}
		}
	}
	else if (tempInAdjustableValueMenu)
	{
		// Enable the pause menu and lower the system level
		SpawnItem.InAdjustableValueMenu = false;
		ttyd::win_main::winOpenEnable();
		lowerSystemLevel();
		return;
	}
}

void checkIfAreaFlagsShouldBeCleared()
{
	// Only run when the player has chosen to clear some flags
	if (!ClearAreaFlags.FlagsShouldBeCleared)
	{
		return;
	}
	
	// Check to see if currently in the process of clearing flags or not
	if (!ClearAreaFlags.StartClearingFlags)
	{
		// Only run if currently in a screen transition
		if (!checkForSpecificSeq(ttyd::seqdrv::SeqIndex::kMapChange))
		{
			return;
		}
		
		// Set up the flags to be cleared
		ClearAreaFlags.StartClearingFlags = true;
	}
	else
	{
		// Keep resetting the flags until the seq changes to Game
		if (!checkForSpecificSeq(ttyd::seqdrv::SeqIndex::kGame))
		{
			// Clear the currently-selected flags
			cheatClearAreaFlags(ClearAreaFlags.CurrentOption);
		}
		else
		{
			ClearAreaFlags.FlagsShouldBeCleared = false;
			ClearAreaFlags.StartClearingFlags = false;
		}
	}
}

void displaySequenceInPauseMenu()
{
	uint32_t PauseMenuAddress = *reinterpret_cast<uint32_t *>(PauseMenuStartAddress);
	uint32_t CurrentTab = *reinterpret_cast<uint32_t *>(PauseMenuAddress + 0x40);
	uint32_t CurrentMenu = *reinterpret_cast<uint32_t *>(PauseMenuAddress + 0x24);
	uint32_t SystemLevel = getSystemLevel();
	
	if ((SystemLevel == 15) && (CurrentTab == 0) && 
		((CurrentMenu == 10) || (CurrentMenu == 11)))
	{
		drawFunctionOnDebugLayer(drawSequenceInPauseMenu);
	}
}

void displayOnScreenTimer()
{
	if (!Displays[ONSCREEN_TIMER])
	{
		return;
	}
	
	if (checkButtonCombo(OnScreenTimer.ButtonCombo[START_PAUSE_RESUME]))
	{
		OnScreenTimer.TimerPaused = !OnScreenTimer.TimerPaused;
	}
	else if (checkButtonCombo(OnScreenTimer.ButtonCombo[RESET]))
	{
		// Reset the timer
		OnScreenTimer.TimerPaused = true;
		OnScreenTimer.MainTimer = 0;
	}
	
	drawFunctionOnDebugLayer(drawOnScreenTimer);
}

void displayMarioCoordinatesBoolCheck()
{
	if (!Displays[MARIO_COORDINATES])
	{
		return;
	}
	
	drawFunctionOnDebugLayer(drawMarioCoordinates);
}

void displayJumpStorageDetails()
{
	if (!Displays[JUMP_STORAGE])
	{
		return;
	}
	
	uint32_t ButtonInputTrg = ttyd::system::keyGetButtonTrg(0);
	if ((ButtonInputTrg & PAD_Y) == PAD_Y)
	{
		// Press Y to increment the counter
		JumpStorageSetCounter++;
	}
	else if (ButtonInputTrg != 0)
	{
		// Reset the counter if a different button was pressed
		JumpStorageSetCounter = 0;
	}
	
	if (JumpStorageSetCounter == 3)
	{
		ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
		player->flags3 |= 1 << 16; // Turn on the 16 bit
		JumpStorageSetCounter = 0;
	}
	
	drawFunctionOnDebugLayer(drawJumpStorageDetails);
}

void displayButtonInputs()
{
	if (!Displays[BUTTON_INPUT_DISPLAY])
	{
		return;
	}
	
	drawFunctionOnDebugLayer(drawButtonInputs);
}

void displayYoshiSkipDetails()
{
	if (!Displays[YOSHI_SKIP])
	{
		return;
	}
	
	// Don't display if the Guard/Superguard timings display is active
	if (DisplayActionCommands.DisplayTimer > 0)
	{
		return;
	}
	
	if (checkForSpecificSeq(ttyd::seqdrv::SeqIndex::kBattle)) // In battle
	{
		// Pause in battle
		YoshiSkip.TimerStopped = true;
		YoshiSkip.TimerPaused = true;
	}
	else
	{
		ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
		bool MarioControl = player->flags1 & (1 << 0); // Check if 0 bit is active
		
		if (MarioControl && YoshiSkip.TimerPaused)
		{
			// Reset and Start when leaving battle
			YoshiSkip.MainTimer = 0;
			YoshiSkip.TimerStopped = false;
			YoshiSkip.TimerPaused = false;
		}
	}
	
	if (checkButtonCombo(PAD_A))
	{
		// Stop when A is pressed
		YoshiSkip.TimerStopped = true;
	}
	
	if (checkButtonComboEveryFrame(PAD_Y))
	{
		// Hold Y to increment the reset counter
		YoshiSkip.ResetTimer++;
	}
	else
	{
		YoshiSkip.ResetTimer = 0;
	}
	
	if (YoshiSkip.ResetTimer > secondsToFrames(2))
	{
		// Reset the timer when button is held for 2 seconds
		YoshiSkip.TimerStopped = true;
		YoshiSkip.MainTimer = 0;
		YoshiSkip.ResetTimer = 0;
	}
	
	drawFunctionOnDebugLayer(drawYoshiSkipDetails);
}

void displayPalaceSkipDetails()
{
	if (!Displays[PALACE_SKIP])
	{
		return;
	}
	
	// Don't display if the Guard/Superguard timings display is active
	if (DisplayActionCommands.DisplayTimer > 0)
	{
		return;
	}
	
	uint32_t SystemLevel = getSystemLevel();
	if (SystemLevel == 15)
	{
		// Stop upon pausing
		PalaceSkip.TimerStopped = true;
		PalaceSkip.TimerPaused = true;
	}
	else if ((SystemLevel == 0) && PalaceSkip.TimerPaused)
	{
		// Reset and Start when unpausing
		PalaceSkip.MainTimer = 0;
		PalaceSkip.TimerStopped = false;
		PalaceSkip.TimerPaused = false;
	}
	
	if (checkButtonCombo(PAD_X))
	{
		// Stop when pressing X
		PalaceSkip.TimerStopped = true;
	}
	
	if (checkButtonComboEveryFrame(PAD_Y))
	{
		// Hold Y to increment the counter
		PalaceSkip.ResetTimer++;
	}
	else
	{
		PalaceSkip.ResetTimer = 0;
	}
	
	if (PalaceSkip.ResetTimer > secondsToFrames(2))
	{
		// Reset the timer when button is held for 2 seconds
		PalaceSkip.TimerStopped = true;
		PalaceSkip.MainTimer = 0;
		PalaceSkip.ResetTimer = 0;
	}
	
	drawFunctionOnDebugLayer(drawPalaceSkipDetails);
}

int32_t Mod::preventMenuSounds(int32_t soundId, uint32_t unk1, 
	uint32_t unk2, uint32_t unk3)
{
	const int32_t OpenMenu = 1169;
	const int32_t CloseMenu = 1168;
	
	if ((soundId == OpenMenu) || (soundId == CloseMenu))
	{
		return -1;
	}
	else
	{
		// Call original function
		return mPFN_SoundEfxPlayEx_trampoline(soundId, unk1, unk2, unk3);
	}
}

void actionCommandsTimingsInit()
{
	// Credits to Jdaster64 for writing the original code for this
	DisplayActionCommands.Trampoline = patch::hookFunction(
	ttyd::battle_ac::BattleActionCommandCheckDefence, [](
		void *battle_unit, void *attack_params)
	{
		if (!Displays[GUARD_SUPERGUARD_TIMINGS])
		{
			return DisplayActionCommands.Trampoline(battle_unit, attack_params);
		}
		
		// Reset the values checked when drawing the text
		DisplayActionCommands.Last_A_Frame 	= -1;
		DisplayActionCommands.Last_B_Frame 	= -1;
		DisplayActionCommands.TypeToDraw 	= -1;
		
		int32_t Last_A_Frame 	= -1;
		int32_t Last_B_Frame 	= -1;
		int32_t ButtonPresses 	= 0;
		
		for (int32_t Frame = 0; Frame < 15; ++Frame)
		{
			if (ttyd::battle_ac::BattleACPadCheckRecordTrigger(Frame, PAD_A))
			{
				Last_A_Frame = Frame;
				++ButtonPresses;
			}
			
			if (ttyd::battle_ac::BattleACPadCheckRecordTrigger(Frame, PAD_B))
			{
				Last_B_Frame = Frame;
				++ButtonPresses;
			}
		}
		
		const int32_t DefenseResult = DisplayActionCommands.Trampoline(
			battle_unit, attack_params);
		
		const int32_t SuccessfulTiming 			= 0;
		const int32_t PressedTooManyButtons 	= 1;
		const int32_t PressedTooEarly 			= 2;
		
		if (DefenseResult == 4)
		{
			// Successful Guard; print Last_A_Frame
			DisplayActionCommands.Last_A_Frame 		= Last_A_Frame;
			DisplayActionCommands.TypeToDraw 		= SuccessfulTiming;
			DisplayActionCommands.DisplayTimer 		= secondsToFrames(3);
		}
		else if (DefenseResult == 5)
		{
			// Successful Superguard; print Last_B_Frame
			DisplayActionCommands.Last_B_Frame 		= Last_B_Frame;
			DisplayActionCommands.TypeToDraw 		= SuccessfulTiming;
			DisplayActionCommands.DisplayTimer 		= secondsToFrames(3);
		}
		else if (ButtonPresses > 1) // Unsuccessful, otherwise...
		{
			// Hit too many buttons in last 15 frames
			DisplayActionCommands.TypeToDraw 		= PressedTooManyButtons;
			DisplayActionCommands.DisplayTimer 		= secondsToFrames(3);
		}
		else if (Last_A_Frame > -1)
		{
			// Print how many frames early the player pressed A
			DisplayActionCommands.Last_A_Frame 	= Last_A_Frame;
			DisplayActionCommands.TypeToDraw 	= PressedTooEarly;
			DisplayActionCommands.DisplayTimer 	= secondsToFrames(3);
		}
		else if (Last_B_Frame > -1)
		{
			// Print how many frames early the player pressed B
			DisplayActionCommands.Last_B_Frame 	= Last_B_Frame;
			DisplayActionCommands.TypeToDraw 	= PressedTooEarly;
			DisplayActionCommands.DisplayTimer 	= secondsToFrames(3);
		}
		
		return DefenseResult;
	});
}

void displayActionCommandsTiming()
{
	// Only display if the timer is not at 0
	if (DisplayActionCommands.DisplayTimer > 0)
	{
		drawFunctionOnDebugLayer(drawActionCommandsTiming);
	}
}

int32_t warpToMap(uint32_t value)
{
	// Make sure the player is currently in the game
	if (!checkIfInGame())
	{
		return NOT_IN_GAME;
	}
	
	uint32_t WarpToMap;
	switch (value)
	{
		case 0: WarpToMap = gor_01; break;
		case 1: WarpToMap = tik_00; break;
		case 2: WarpToMap = hei_00; break;
		case 3: WarpToMap = gon_00; break;
		case 4: WarpToMap = win_06; break;
		case 5: WarpToMap = mri_01; break;
		case 6: WarpToMap = tou_02; break;
		case 7: WarpToMap = tou_05; break;
		case 8: WarpToMap = usu_00; break;
		case 9: WarpToMap = jin_00; break;
		case 10: WarpToMap = muj_01; break;
		case 11: WarpToMap = dou_00; break;
		case 12: WarpToMap = rsh_02_a; break;
		case 13: WarpToMap = eki_00; break;
		case 14: WarpToMap = pik_00; break;
		case 15: WarpToMap = bom_00; break;
		case 16: WarpToMap = moo_00; break;
		case 17: WarpToMap = aji_00; break;
		case 18: WarpToMap = aji_13; break;
		case 19: WarpToMap = las_00; break;
		case 20: WarpToMap = las_09; break;
		case 21: WarpToMap = las_27; break;
		case 22:
		{
			setNextBero("minnnanokoe");
			setSequencePosition(400);
			WarpToMap = las_29;
			break;
		}
		case 23:
		{
			uint32_t NewPitFloor = getCurrentPitFloor();
			uint32_t NewPitMap;

			// Get proper room to use for chosen floor
			if (NewPitFloor % 10 == 0)
			{
				// Current floor is a chest floor or Bonetail
				if (NewPitFloor <= 40)
				{
					NewPitMap = jon_03;
				}
				else if (NewPitFloor <= 70)
				{
					NewPitMap = jon_04;
				}
				else if (NewPitFloor <= 90)
				{
					NewPitMap = jon_05;
				}
				else // NewPitFloor == 100
				{
					NewPitMap = jon_06;
				}
			}
			else // Current floor is not a chest floor nor Bonetail
			{
				if (NewPitFloor < 50)
				{
					NewPitMap = jon_00;
				}
				else if (NewPitFloor < 80)
				{
					NewPitMap = jon_01;
				}
				else // NewPitMap < 100
				{
					NewPitMap = jon_02;
				}
			}
			
			WarpToMap = NewPitMap;
			setNextBero("dokan_2");
			break;
		}
		case 24:
		{
			WarpToMap = reinterpret_cast<uint32_t>("title");
			break;
		}
		default: 
		{
			return UNKNOWN_BEHAVIOR;
		}
	}
	
	char *WantedMap = reinterpret_cast<char *>(WarpToMap);
	setNextMap(WantedMap);
	reloadRoomMain();
	
	return SUCCESS;
}

}