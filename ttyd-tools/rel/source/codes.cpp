#include "codes.h"
#include "global.h"
#include "commonfunctions.h"
#include "mod.h"
#include "menufunctions.h"
#include "draw.h"
#include "patch.h"
#include "assembly.h"

#include <ttyd/npcdrv.h>
#include <ttyd/item_data.h>
#include <ttyd/mariost.h>
#include <ttyd/system.h>
#include <ttyd/mario.h>
#include <ttyd/evtmgr.h>
#include <ttyd/seqdrv.h>
#include <ttyd/seq_mapchange.h>
#include <ttyd/camdrv.h>
#include <ttyd/dispdrv.h>
#include <ttyd/pmario_sound.h>
#include <ttyd/mario_cam.h>
#include <ttyd/mario_pouch.h>
#include <ttyd/mapdrv.h>
#include <ttyd/swdrv.h>
#include <ttyd/win_main.h>
#include <ttyd/itemdrv.h>
#include <ttyd/battle_unit.h>
#include <ttyd/battle_ac.h>
#include <ttyd/party.h>

#include <cstring>
#include <cstdio>
#include <cinttypes>

namespace mod {

ttyd::npcdrv::NpcEntry *Mod::disableBattles(uint32_t flags, void *unk)
{
	// Call the original function immediately
	ttyd::npcdrv::NpcEntry *Result = mPFN_fbatHitCheck_trampoline(flags, unk);
	
	uint32_t ReloadRoomCombo 	= PAD_L | PAD_B;
	uint32_t OpenMenuCombo 		= PAD_L | PAD_START;
	
	// Prevent entering a non-cutscene battle if opening the menu
	if (checkButtonComboEveryFrame(OpenMenuCombo))
	{
		return nullptr;
	}
	else if (!MenuVar.ChangingCheatButtonCombo)
	{
		// Prevent entering a non-cutscene battle if reloading the room
		if (checkButtonComboEveryFrame(ReloadRoomCombo))
		{
			return nullptr;
		}
		else if (Cheat[DISABLE_BATTLES].Active && 
			checkButtonComboEveryFrame(Cheat[DISABLE_BATTLES].ButtonCombo))
		{
			return nullptr;
		}
	}
	
	return Result;
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
		return static_cast<int32_t>(MenuVar.ForcedNPCItemDrop);
	}
	
	return *reinterpret_cast<int32_t *>(
		reinterpret_cast<uint32_t>(ptr) + 0x23C);
}

void *fallThroughMostObjects(void *ptr)
{
	if (!Cheat[FALL_THROUGH_FLOORS].Active)
	{
		return ptr;
	}
	
	// Don't fall if currently changing button combos
	if (MenuVar.ChangingCheatButtonCombo)
	{
		return ptr;
	}
	
	if (!checkButtonComboEveryFrame(Cheat[FALL_THROUGH_FLOORS].ButtonCombo))
	{
		return ptr;
	}
	
	return nullptr;
}

void Mod::performBattleChecks()
{
#ifdef TTYD_US
	const uint32_t DebugBadgeAddressOffset = 0x307;
#elif defined TTYD_JP
	const uint32_t DebugBadgeAddressOffset = 0x303;
#elif defined TTYD_EU
	const uint32_t DebugBadgeAddressOffset = 0x30B;
#endif
	
	// Make sure Mario is in the battle before making any changes
	uint32_t MarioBattlePointer = reinterpret_cast<uint32_t>(getMarioBattlePointer());
	
	if (MarioBattlePointer)
	{
		auto setDebugBadgeAddressesValue = [MarioBattlePointer](uint8_t value)
		{
			// Set the value for Mario
			*reinterpret_cast<uint8_t *>(MarioBattlePointer + DebugBadgeAddressOffset) = value;
			
			// Set the value for the partner
			uint32_t PartnerBattlePointer = reinterpret_cast<uint32_t>(getPartnerBattlePointer());
			if (PartnerBattlePointer)
			{
				*reinterpret_cast<uint8_t *>(PartnerBattlePointer + DebugBadgeAddressOffset) = value;
			}
		};
		
		// Check to see if the Auto Action Commands cheat is active or not
		if (Cheat[AUTO_ACTION_COMMANDS].Active)
		{
			// Check to see if currently changing button combos
			if (!MenuVar.ChangingCheatButtonCombo)
			{
				if (checkButtonComboEveryFrame(Cheat[AUTO_ACTION_COMMANDS].ButtonCombo) || 
					checkIfBadgeEquipped(ttyd::item_data::Item::DebugBadge))
				{
					setDebugBadgeAddressesValue(1);
				}
				else
				{
					setDebugBadgeAddressesValue(0);
				}
			}
			else if (checkIfBadgeEquipped(ttyd::item_data::Item::DebugBadge))
			{
				setDebugBadgeAddressesValue(1);
			}
			else
			{
				setDebugBadgeAddressesValue(0);
			}
		}
		else if (checkIfBadgeEquipped(ttyd::item_data::Item::DebugBadge))
		{
			setDebugBadgeAddressesValue(1);
		}
		else
		{
			setDebugBadgeAddressesValue(0);
		}
	}
	
	// Prevent all buttons from being pressed when the menu is open, exccept for R and X
	if (MenuVar.MenuIsDisplayed)
	{
		if (!checkButtonComboEveryFrame(PAD_R) && !checkButtonComboEveryFrame(PAD_X))
		{
			// The menu is open and neither R nor X are being pressed, so prevent the function from running
			return;
		}
	}

	// Call original function
	mPFN_BattlePadManager_trampoline();
}

void walkThroughMostObjects()
{
	if (Cheat[WALK_THROUGH_WALLS].Active && 
		!MenuVar.ChangingCheatButtonCombo && 
			checkButtonComboEveryFrame(Cheat[WALK_THROUGH_WALLS].ButtonCombo))
	{
		MenuVar.ResetMarioProperties = true;
		
		// Set Mario's properties to be able to walk through most objects
		ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
		player->flags1 |= (1 << 10); // Turn on the 10 bit
	}
	else if (MenuVar.ResetMarioProperties)
	{
		MenuVar.ResetMarioProperties = false;
		
		// Don't reset the properties if Vivian is currently being used
		ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
		const uint32_t UsingVivian = 28;
		
		if (player->currentMotionId != UsingVivian)
		{
			player->flags1 &= ~(1 << 10); // Turn off the 10 bit
		}
	}
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
	MarioPartnerPositions.MarioPosition[3] = player->wPlayerAngleCurrent;
	MarioPartnerPositions.MarioPosition[4] = player->wPlayerAngle;
	MarioPartnerPositions.MarioPosition[5] = player->wPlayerDirectionCurrent;
	MarioPartnerPositions.MarioPosition[6] = player->wPlayerDirection;
	
	uint32_t PartnerPointer = reinterpret_cast<uint32_t>(getPartnerPointer());
	if (!PartnerPointer)
	{
		return;
	}
	
	MarioPartnerPositions.PartnerPosition[0] = *reinterpret_cast<float *>(PartnerPointer + 0x58); // Partner Coordinate X
	MarioPartnerPositions.PartnerPosition[1] = *reinterpret_cast<float *>(PartnerPointer + 0x5C); // Partner Coordinate Y
	MarioPartnerPositions.PartnerPosition[2] = *reinterpret_cast<float *>(PartnerPointer + 0x60); // Partner Coordinate Z
	MarioPartnerPositions.PartnerPosition[3] = *reinterpret_cast<float *>(PartnerPointer + 0xFC); // Partner Angle Current
	MarioPartnerPositions.PartnerPosition[4] = *reinterpret_cast<float *>(PartnerPointer + 0x100); // Partner Angle
	MarioPartnerPositions.PartnerPosition[5] = *reinterpret_cast<float *>(PartnerPointer + 0x10C); // Partner Direction Current
	MarioPartnerPositions.PartnerPosition[6] = *reinterpret_cast<float *>(PartnerPointer + 0x110); // Partner Direction
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
	
	player->playerPosition[0] 			= MarioPartnerPositions.MarioPosition[0]; // Mario Coordinate X
	player->playerPosition[1] 			= MarioPartnerPositions.MarioPosition[1]; // Mario Coordinate Y
	player->playerPosition[2] 			= MarioPartnerPositions.MarioPosition[2]; // Mario Coordinate Z
	player->wPlayerAngleCurrent 		= MarioPartnerPositions.MarioPosition[3];
	player->wPlayerAngle 				= MarioPartnerPositions.MarioPosition[4];
	player->wPlayerDirectionCurrent 	= MarioPartnerPositions.MarioPosition[5];
	player->wPlayerDirection 			= MarioPartnerPositions.MarioPosition[6];
	
	uint32_t PartnerPointer = reinterpret_cast<uint32_t>(getPartnerPointer());
	if (!PartnerPointer)
	{
		return;
	}
	
	*reinterpret_cast<float *>(PartnerPointer + 0x58) 	= MarioPartnerPositions.PartnerPosition[0]; // Partner Coordinate X
	*reinterpret_cast<float *>(PartnerPointer + 0x5C) 	= MarioPartnerPositions.PartnerPosition[1]; // Partner Coordinate Y
	*reinterpret_cast<float *>(PartnerPointer + 0x60) 	= MarioPartnerPositions.PartnerPosition[2]; // Partner Coordinate Z
	*reinterpret_cast<float *>(PartnerPointer + 0xFC) 	= MarioPartnerPositions.PartnerPosition[3]; // Partner Angle Current
	*reinterpret_cast<float *>(PartnerPointer + 0x100) 	= MarioPartnerPositions.PartnerPosition[4]; // Partner Angle
	*reinterpret_cast<float *>(PartnerPointer + 0x10C) 	= MarioPartnerPositions.PartnerPosition[5]; // Partner Direction Current
	*reinterpret_cast<float *>(PartnerPointer + 0x110) 	= MarioPartnerPositions.PartnerPosition[6]; // Partner Direction
}

void saveAnywhere()
{
	if (!SaveAnywhere.ScriptIsRunning)
	{
		// Save script is not currently running
		if (!Cheat[SAVE_ANYWHERE].Active)
		{
			return;
		}
		
		if (MenuVar.ChangingCheatButtonCombo)
		{
			return;
		}
		
		if (!checkButtonCombo(Cheat[SAVE_ANYWHERE].ButtonCombo))
		{
			return;
		}
		
		// Make sure a file is currently loaded, and that the pause menu is not currently open
		if (!checkIfInGame())
		{
			return;
		}
		
		uint32_t SystemLevel = ttyd::mariost::marioStGetSystemLevel();
		if (SystemLevel == 15)
		{
			return;
		}
		
#ifdef TTYD_US
		void *SaveScriptEvtCode = reinterpret_cast<void *>(0x803BAC3C);
#elif defined TTYD_JP
		void *SaveScriptEvtCode = reinterpret_cast<void *>(0x803B68BC);
#elif defined TTYD_EU
		void *SaveScriptEvtCode = reinterpret_cast<void *>(0x803C6C4C);
#endif
		
		// Take away control from the player and start the Save script
		ttyd::evtmgr::EvtEntry *SaveScriptEvt = ttyd::evtmgr::evtEntryType(SaveScriptEvtCode, 0, 0, 0);
		SaveAnywhere.ThreadID = SaveScriptEvt->threadId;
		
		SaveAnywhere.ScriptIsRunning = true;
		raiseSystemLevel();
		
		// Only turn the key off if it's not already off
		if (ttyd::mario::marioKeyOffChk() == 0)
		{
			ttyd::mario::marioKeyOff();
		}
	}
	else if (!ttyd::evtmgr::evtCheckID(SaveAnywhere.ThreadID))
	{
		// Save Script is no longer running, so give back control to the player
		SaveAnywhere.ScriptIsRunning = false;
		lowerSystemLevel();
		
		// Only turn the key on if it's not already on
		if (ttyd::mario::marioKeyOffChk() != 0)
		{
			ttyd::mario::marioKeyOn();
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
	if (PartnerPointer)
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
	if (PartnerPointer)
	{
		*reinterpret_cast<uint8_t *>(PartnerPointer + 0x39) = 21;
	}
}

void speedUpMario()
{
	ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
	float SpeedValueToSet 	= 16;
	
	float current_unk_184 	= SpeedUpMario.MarioVar[0];
	float current_unk_188 	= SpeedUpMario.MarioVar[1];
	float new_unk_184 		= player->unk_184;
	float new_unk_188 		= player->unk_188;
	
	const uint8_t NoChanges 				= 0;
	const uint8_t ValuesModified 			= 1;
	const uint8_t ValuesShouldBeRestored 	= 2;
	
	if (!Cheat[SPEED_UP_MARIO].Active || MenuVar.ChangingCheatButtonCombo)
	{
		// Check if the values should be restored
		if (SpeedUpMario.ValuesChangedState > NoChanges)
		{
			SpeedUpMario.ValuesChangedState = 0;
			player->unk_184 = current_unk_184;
			player->unk_188 = current_unk_188;
		}
		return;
	}
	
	// Check to see if the current values have changed
	if (((current_unk_184 != new_unk_184)) || 
		(current_unk_188 != new_unk_188))
	{
		// Don't update if the new value is what was just set by this function
		if (new_unk_188 != SpeedValueToSet)
		{
			// Set the current values to the new values
			SpeedUpMario.MarioVar[0] = new_unk_184;
			SpeedUpMario.MarioVar[1] = new_unk_188;
		}
	}
	
	if (checkButtonComboEveryFrame(Cheat[SPEED_UP_MARIO].ButtonCombo))
	{
		SpeedUpMario.ValuesChangedState = ValuesModified;
		player->wPlayerBaseSpeed = SpeedValueToSet;
		player->unk_184 = SpeedValueToSet;
		player->unk_188 = SpeedValueToSet;
	}
	else if (SpeedUpMario.ValuesChangedState == ValuesModified)
	{
		SpeedUpMario.ValuesChangedState = ValuesShouldBeRestored;
	}
	
	// Check if the values should be restored
	if (SpeedUpMario.ValuesChangedState == ValuesShouldBeRestored)
	{
		SpeedUpMario.ValuesChangedState = 0;
		player->unk_184 = SpeedUpMario.MarioVar[0];
		player->unk_188 = SpeedUpMario.MarioVar[1];
	}
}

bool Mod::infiniteItemUsage(int16_t item, uint32_t index)
{
	if (!Cheat[INFINITE_ITEM_USAGE].Active)
	{
		// Call original function
		return mPFN_pouchRemoveItemIndex_trampoline(item, index);
	}
	
	if (MenuVar.ChangingCheatButtonCombo)
	{
		// Call original function
		return mPFN_pouchRemoveItemIndex_trampoline(item, index);
	}
	
	if (!checkButtonComboEveryFrame(Cheat[INFINITE_ITEM_USAGE].ButtonCombo))
	{
		// Call original function
		return mPFN_pouchRemoveItemIndex_trampoline(item, index);
	}
	
	// Prevent the function from running
	return false;
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
		uint32_t SystemLevel = ttyd::mariost::marioStGetSystemLevel();
		if (SystemLevel > 0)
		{
			ttyd::mariost::marioStSystemLevel(0);
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
	
	// Resolve any necessary fades, as they can sometimes cause issues if not resolved, such as black screens
	// Only resolve the fades if not currently in a screen transition
	if (checkForSpecificSeq(Game))
	{
		uint32_t MaxFadeEntries = 5;
		for (uint32_t i = 0; i < MaxFadeEntries; i++)
		{
			resolveFade(i);
		}
	}
	
	// A separate address for NextBero is needed, as the original value will be cleared during the reloading process
	// The game will crash if NextMap is used directly in seqSetSeq, so a separate address must be used instead
	// NewBero and NewMap need to be global variables
	
	char *tempNewBero = ReloadRoom.NewBero;
	char *tempNewMap = ReloadRoom.NewMap;
	
	strcpy(tempNewBero, ttyd::seq_mapchange::NextBero);
	strcpy(tempNewMap, ttyd::seq_mapchange::NextMap);
	setSeqMapChange(tempNewMap, tempNewBero);
	
	// Reset the camera - mainly for the black bars at the top and bottom of the screen
	uint32_t CameraPointer = reinterpret_cast<uint32_t>(ttyd::camdrv::camGetPtr(ttyd::dispdrv::CameraId::k2d));
	*reinterpret_cast<uint16_t *>(CameraPointer) &= ~((1 << 8) | (1 << 9)); // Turn off the 8 and 9 bits
	
	uint32_t SystemLevel = ttyd::mariost::marioStGetSystemLevel();
	if (SystemLevel == 0)
	{
		return;
	}
	
	// Only run the following if the system level is not 0
	if (SystemLevel == 15)
	{
		// Currently in pause menu, so re-enable the 3D camera
		ttyd::camdrv::camDispOn(ttyd::dispdrv::CameraId::k3d);
	}
	
	// Enable sound effects, set the default camera id for Mario, and give back control to the player
	ttyd::pmario_sound::psndClearFlag(0x80);
	ttyd::mario_cam::marioSetCamId(4);
	ttyd::mariost::marioStSystemLevel(0);
	ReloadRoom.SystemLevelShouldBeLowered = true;
}

void reloadRoom()
{
	if (!MenuVar.ChangingCheatButtonCombo && checkButtonCombo(Cheat[RELOAD_ROOM].ButtonCombo))
	{
		// Prevent being able to reload the room if the menu is open or if currently in the spawn item menu
		if (Cheat[RELOAD_ROOM].Active && 
			!MenuVar.MenuIsDisplayed && 
			!SpawnItem.InAdjustableValueMenu)
		{
			reloadRoomMain();
		}
	}
	
	// Manually reset the System Level and the camera if currently getting an item while the room is trying to load
	ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
	const uint16_t ReceivingItem = 15;
	
	if (player->currentMotionId != ReceivingItem)
	{
		return;
	}
	
	ttyd::seqdrv::SeqIndex NextSeq 		= ttyd::seqdrv::seqGetNextSeq();
	ttyd::seqdrv::SeqIndex MapChange 	= ttyd::seqdrv::SeqIndex::kMapChange;
	
	if (NextSeq != MapChange)
	{
		return;
	}
	
	// Reset the System Level
	ReloadRoom.SystemLevelShouldBeLowered = true;
	
	// Reset the camera - mainly for the black bars at the top and bottom of the screen
	uint32_t CameraPointer = reinterpret_cast<uint32_t>(ttyd::camdrv::camGetPtr(ttyd::dispdrv::CameraId::k2d));
	*reinterpret_cast<uint16_t *>(CameraPointer) &= ~((1 << 8) | (1 << 9)); // Turn off the 8 and 9 bits
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

uint32_t autoMashText(uint32_t controllerPort)
{
	if (!Cheat[AUTO_MASH_TEXT].Active)
	{
		// Return the intended value
		return ttyd::system::keyGetButtonTrg(controllerPort);
	}
	
	// Don't auto mash if the Palace Skip display is currently on
	if (Displays[PALACE_SKIP])
	{
		// Return the intended value
		return ttyd::system::keyGetButtonTrg(controllerPort);
	}
	
	// Don't auto mash if currently changing button combos
	if (MenuVar.ChangingCheatButtonCombo)
	{
		// Return the intended value
		return ttyd::system::keyGetButtonTrg(controllerPort);
	}
	
	if (!checkButtonComboEveryFrame(Cheat[AUTO_MASH_TEXT].ButtonCombo))
	{
		// Return the intended value
		return ttyd::system::keyGetButtonTrg(controllerPort);
	}
	
	// Return the value for B to make sure the text is being mashed through as fast as possible
	return PAD_B;
}

void lockMarioHPToMax()
{
	if (!Cheat[LOCK_MARIO_HP_TO_MAX].Active)
	{
		return;
	}
	
	uint32_t PouchAddress = reinterpret_cast<uint32_t>(ttyd::mario_pouch::pouchGetPtr());
	
	uint32_t MarioHPAddress = PouchAddress + 0x70;
	int16_t MarioMaxHP = *reinterpret_cast<int16_t *>(PouchAddress + 0x72);
	*reinterpret_cast<int16_t *>(MarioHPAddress) = MarioMaxHP; // Copy Max HP to Current HP
	
	uint32_t MarioBattleAddress = reinterpret_cast<uint32_t>(getMarioBattlePointer());
	if (!MarioBattleAddress)
	{
		return;
	}
	
	uint32_t BattleMarioHPAddress = MarioBattleAddress + 0x10C;
	int16_t BattleMarioMaxHP = *reinterpret_cast<int16_t *>(MarioBattleAddress + 0x108);
	*reinterpret_cast<int16_t *>(BattleMarioHPAddress) = BattleMarioMaxHP; // Copy Battle Max HP to Current Battle HP
}

void bobberyEarly()
{
	if (checkForSpecificSeq(ttyd::seqdrv::SeqIndex::kMapChange))
	{
		MenuVar.MarioFreeze = false;
	}
	
	if (MenuVar.MarioFreeze)
	{
		float RopePosZ = *reinterpret_cast<float *>(
			*reinterpret_cast<uint32_t *>(
				reinterpret_cast<uint32_t>(
					ttyd::mapdrv::mapGetWork()) + 0x154) + 0x8B4);
		
		ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
		player->flags1 |= (1 << 1); // Turn on the 1 bit
		player->wJumpVelocityY = 0;
		player->wJumpAccelerationY = 0;
		player->unk_84 = 0;
		player->playerPosition[0] = -1;
		player->playerPosition[1] = 59;
		player->playerPosition[2] = RopePosZ + 10;
		player->wPlayerAngleCurrent = 180;
		player->wPlayerAngle = 180;
		player->wPlayerDirectionCurrent = 180;
		player->wPlayerDirection = 180;
	}
	
	if (!Cheat[BOBBERY_EARLY].Active)
	{
		return;
	}
	
	// Turn off GSWF(3137)
	ttyd::swdrv::swClear(3137);
	
	if (!compareStringToNextMap("muj_05"))
	{
		return;
	}
	
	ttyd::npcdrv::NpcEntry *NPC = getNpcEntryData(5); // NPC 6
	
	// Allow the Ember to be refought
	*reinterpret_cast<uint8_t *>(
		reinterpret_cast<uint32_t>(NPC) + 0x1D7) = 0;
}

void spawnItem()
{
	if (checkForSpecificSeq(ttyd::seqdrv::SeqIndex::kMapChange))
	{
		SpawnItem.SpawnItemCounter = 0;
	}
	
	bool tempInAdjustableValueMenu = SpawnItem.InAdjustableValueMenu;
	if (Cheat[SPAWN_ITEM].Active && !MenuVar.ChangingCheatButtonCombo)
	{
		if (!checkIfInGame())
		{
			return;
		}
		
		uint32_t SystemLevel = ttyd::mariost::marioStGetSystemLevel();
		if (SystemLevel == 15)
		{
			return;
		}
		
		// Currently not in a battle and the pause menu is not open
		// Check to see if the adjustable value menu is open or not
		if (checkButtonCombo(Cheat[SPAWN_ITEM].ButtonCombo) && !tempInAdjustableValueMenu)
		{
			// Not open, so disable the pause menu, raise the system level, and open the menu
			ttyd::win_main::winOpenDisable();
			raiseSystemLevel();
			MenuVar.SecondaryMenuOption = getHighestAdjustableValueDigit(INVENTORY_STANDARD) - 1;
			SpawnItem.InAdjustableValueMenu = true;
		}
		
		if (!tempInAdjustableValueMenu)
		{
			return;
		}
		
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
				
				char *tempDisplayBuffer = DisplayBuffer;
				uint32_t tempTimer = SpawnItem.SpawnItemCounter;
				SpawnItem.SpawnItemCounter = tempTimer + 1;
				
				sprintf(tempDisplayBuffer,
					"C_Item%" PRIu32,
					tempTimer);
				
				ttyd::itemdrv::itemEntry(tempDisplayBuffer, MenuVar.MenuSecondaryValue, 16, 
					-1, nullptr, ItemCoordinateX, ItemCoordinateY, ItemCoordinateZ);
				
				// Enable the pause menu and lower the system level
				SpawnItem.InAdjustableValueMenu = false;
				ttyd::win_main::winOpenEnable();
				lowerSystemLevel();
				return;
			}
			case B:
			case NO_NUMBERS_TO_DISPLAY:
			{
				// Enable the pause menu and lower the system level
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
		drawFunctionOnDebugLayerWithOrder(drawAdjustableValueSpawnItem, 10.f);
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

void lockFlags()
{
	uint32_t Size = sizeof(LockFlags.Region) / sizeof(LockFlags.Region[0]);
	for (uint32_t Index = 0; Index < Size; Index++)
	{
		// Get the region to work with
		LockFlagsRegion *Region = &LockFlags.Region[Index];
		
		if (!Region->MemoryRegionLocked)
		{
			continue;
		}
		
		uint8_t *tempMemory = Region->MemoryRegion;
		if (!tempMemory)
		{
			continue;
		}
		
		// Restore the memory
		uint32_t Size = Region->Size;
		if ((Index == GW) || (Index == GF))
		{
			// Restore the GWs or GFs
			ttyd::evtmgr::EvtWork *EventWork = ttyd::evtmgr::evtGetWork();
			void *SourceMemoryStart;
			
			if (Index == GW)
			{
				// Restore the GWs
				SourceMemoryStart = EventWork->gwData;
			}
			else // Index == GF
			{
				// Restore the GFs
				SourceMemoryStart = EventWork->gfData;
			}
			
			memcpy(SourceMemoryStart, &tempMemory[0], Size);
		}
		else
		{
			// Do other stuff depending on the index
			switch (Index)
			{
				case GSW:
				{
					// Restore the Sequence Position
					setSequencePosition(LockFlags.SequencePosition);
					break;
				}
				case LSW:
				case LSWF:
				{
					// Only restore if currently in the original area where the code was enabled
					char *Area = lockFlagsMenuGetAreaLockedString(Index);
					if (!compareStrings(Area, ttyd::seq_mapchange::NextArea))
					{
						continue;
					}
					break;
				}
				default:
				{
					break;
				}
			}
			
			// Restore the standard flags
			uint32_t GlobalWorkPtrRaw = reinterpret_cast<uint32_t>(ttyd::mariost::globalWorkPointer);
			void *MemoryStart = reinterpret_cast<void *>(GlobalWorkPtrRaw + Region->Offset);
			memcpy(MemoryStart, &tempMemory[0], Size);
		}
	}
}

void displaySequenceInPauseMenu()
{
	uint32_t SystemLevel = ttyd::mariost::marioStGetSystemLevel();
	if (SystemLevel != 15)
	{
		return;
	}
	
	uint32_t PauseMenuAddress = reinterpret_cast<uint32_t>(ttyd::win_main::winGetPtr());
	uint32_t CurrentTab = *reinterpret_cast<uint32_t *>(PauseMenuAddress + 0x40);
	
	if (CurrentTab != 0)
	{
		return;
	}
	
	uint32_t CurrentMenu = *reinterpret_cast<uint32_t *>(PauseMenuAddress + 0x24);
	if ((CurrentMenu == 10) || (CurrentMenu == 11))
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

void displayFrameCounter()
{
	if (!Displays[ONSCREEN_TIMER_FRAME_COUNTER])
	{
		return;
	}
	
	if (checkButtonCombo(FrameCounter.ButtonCombo[START_PAUSE_RESUME]))
	{
		FrameCounter.TimerPaused = !FrameCounter.TimerPaused;
	}
	else if (checkButtonCombo(FrameCounter.ButtonCombo[RESET]))
	{
		// Reset the timer
		FrameCounter.TimerPaused = true;
		FrameCounter.MainTimer = 0;
	}
	
	drawFunctionOnDebugLayer(drawFrameCounter);
}

void displayMarioCoordinates()
{
	if (!Displays[MARIO_COORDINATES])
	{
		return;
	}
	
	drawFunctionOnDebugLayer(drawMarioCoordinates);
}

void displayMarioSpeedXZ()
{
	if (!Displays[MARIO_SPEED_XZ])
	{
		return;
	}
	
	drawFunctionOnDebugLayer(drawMarioSpeedXZ);
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
		MenuVar.JumpStorageSetCounter++;
	}
	else if (ButtonInputTrg != 0)
	{
		// Reset the counter if a different button was pressed
		MenuVar.JumpStorageSetCounter = 0;
	}
	
	if (MenuVar.JumpStorageSetCounter == 3)
	{
		ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
		player->flags3 |= 1 << 16; // Turn on the 16 bit
		MenuVar.JumpStorageSetCounter = 0;
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

void displayStickAngle()
{
	if (!Displays[STICK_ANGLE])
	{
		return;
	}
	
	drawFunctionOnDebugLayer(drawStickAngle);
}

void displayMemoryWatches()
{
	// Check if a memory watch is currently being repositioned
	bool tempHideMenu = MenuVar.HideMenu;
	bool ChangingMemoryWatchPosition = MenuVar.DrawChangingMemoryWatchPosition;
	
	if (tempHideMenu && ChangingMemoryWatchPosition)
	{
		// A memory watch is being repositioned
		drawFunctionOnDebugLayerWithOrder(drawMemoryChangeWatchPosition, 10.f);
	}
	
	// Make sure at least one watch is going to be displayed
	uint32_t tempMenuSelectionOption = MenuVar.MenuSelectedOption;
	uint32_t Size = sizeof(MemoryWatch) / sizeof(MemoryWatch[0]);
	
	for (uint32_t i = 0; i < Size; i++)
	{
		if (!MemoryWatch[i].Display)
		{
			continue;
		}
		
		// Check to see if this watch is currently being repositioned
		if (tempHideMenu && 
			ChangingMemoryWatchPosition && 
			(tempMenuSelectionOption == i))
		{
			// This watch is currently being repositioned, so do not draw it
		}
		else
		{
			// At least one watch is being drawn
			drawFunctionOnDebugLayer(drawMemoryWatchesOnOverworld);
			return;
		}
	}
}

void displayYoshiSkipDetails()
{
	if (!Displays[YOSHI_SKIP])
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
		bool MarioControl = player->flags1 & (1 << 0); // Check if 0 bit is set
		
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
	
	uint32_t SystemLevel = ttyd::mariost::marioStGetSystemLevel();
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

void displayBridgeSkipDetails()
{
	if (!Displays[BRIDGE_SKIP])
	{
		return;
	}
	
	ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
	bool unkNoLongerOnGround = player->flags2 & (1 << 28); // Check if 28 bit is set
	bool unkNotAbleToJump = player->flags2 & (1 << 16); // Check if 16 bit is set
	
	if (!unkNoLongerOnGround && unkNotAbleToJump) // Done falling, now starting to stand up
	{
		if (!BridgeSkip.PressedEarly)
		{
			// Reset the timer upon landing and while not pressing A
			BridgeSkip.MainTimer = 0;
			
			// Check if A was pressed too early
			if (BridgeSkip.TimerPaused && BridgeSkip.TimerStopped)
			{
				// Pressed A too early, so start the timer
				BridgeSkip.PressedEarly = true;
				BridgeSkip.TimerPaused = false;
				BridgeSkip.TimerStopped = false;
			}
			else
			{
				// Stop the timer
				BridgeSkip.TimerPaused = true;
				BridgeSkip.TimerStopped = false;
			}
		}
		else if (BridgeSkip.PressedEarlyShouldDisable)
		{
			// Reset the early state
			BridgeSkip.TimerPaused = true;
			BridgeSkip.PressedEarly = false;
			BridgeSkip.PressedEarlyShouldDisable = false;
			BridgeSkip.TimerStopped = false;
		}
		else
		{
			// The timer is currently running
		}
	}
	else if (!unkNoLongerOnGround && !unkNotAbleToJump) // A timing, start timer
	{
		if (!BridgeSkip.PressedEarly)
		{
			if (BridgeSkip.TimerPaused) // Prevents running when the display is initially enabled
			{
				if (!BridgeSkip.TimerStopped)
				{
					// Didn't press A, so start the timer
					BridgeSkip.TimerPaused = false;
				}
				else if (!(player->flags2 & (1 << 17))) // Check if 17 bit is not set; unkCurrentlyJumping
				{
					// Pressed A one frame early
					BridgeSkip.PressedEarly = true;
					BridgeSkip.PressedEarlyShouldDisable = true;
				}
				else
				{
					// Pressed A on the correct frame, so leave the timer paused
				}
			}
		}
		else
		{
			// Stop the timer
			BridgeSkip.TimerPaused = true;
			BridgeSkip.PressedEarlyShouldDisable = true;
		}
	}
	
	if (checkButtonCombo(PAD_A))
	{
		// Stop when A is pressed
		BridgeSkip.TimerStopped = true;
	}
	
	if (checkButtonComboEveryFrame(PAD_Y))
	{
		// Hold Y to increment the reset counter
		BridgeSkip.ResetTimer++;
	}
	else
	{
		BridgeSkip.ResetTimer = 0;
	}
	
	if (BridgeSkip.ResetTimer > secondsToFrames(2))
	{
		// Reset the timer when button is held for 2 seconds
		BridgeSkip.MainTimer = 0;
		BridgeSkip.ResetTimer = 0;
		BridgeSkip.PressedEarly = false;
		BridgeSkip.PressedEarlyShouldDisable = false;
		BridgeSkip.TimerPaused = false;
		BridgeSkip.TimerStopped = true;
	}
	
	drawFunctionOnDebugLayer(drawBridgeSkipDetails);
}

void displayBlimpTicketSkipDetails()
{
	if (!Displays[BLIMP_TICKET_SKIP])
	{
		return;
	}
	
	uint32_t SystemLevel = ttyd::mariost::marioStGetSystemLevel();
	if (SystemLevel == 15)
	{
		// Stop upon pausing
		BlimpTicketSkip.UpRightTimerStopped 	= true;
		BlimpTicketSkip.StraightUpTimerStopped 	= true;
		BlimpTicketSkip.TimersPaused 			= true;
	}
	else if ((SystemLevel == 0) && BlimpTicketSkip.TimersPaused)
	{
		// Reset and Start when unpausing
		BlimpTicketSkip.UpRightTimer 			= 0;
		BlimpTicketSkip.StraightUpTimer 		= 0;
		BlimpTicketSkip.TimersPaused 			= false;
		BlimpTicketSkip.UpRightTimerStopped 	= false;
		BlimpTicketSkip.StraightUpTimerStopped 	= false;
	}
	
	double StickAngle = getStickAngle(nullptr);
	double UpRightAngleStart = 25;
	
#ifdef TTYD_JP
	UpRightAngleStart += 2;
#endif
	
	if (StickAngle >= UpRightAngleStart)
	{
		// Stop the up-right timer when the angle held reaches or exceeds 25 for JP, or 27 for US/EU
		BlimpTicketSkip.UpRightTimerStopped = true;
	}
	else if (StickAngle == 0)
	{
		// Stop the straight-up timer when the angle held is 0
		BlimpTicketSkip.StraightUpTimerStopped = true;
	}
	
	if (checkButtonComboEveryFrame(PAD_Y))
	{
		// Hold Y to increment the counter
		BlimpTicketSkip.ResetTimer++;
	}
	else
	{
		BlimpTicketSkip.ResetTimer = 0;
	}
	
	if (BlimpTicketSkip.ResetTimer > secondsToFrames(2))
	{
		// Reset the timers when button is held for 2 seconds
		BlimpTicketSkip.UpRightTimer 			= 0;
		BlimpTicketSkip.StraightUpTimer 		= 0;
		BlimpTicketSkip.ResetTimer 				= 0;
		BlimpTicketSkip.UpRightTimerStopped 	= true;
		BlimpTicketSkip.StraightUpTimerStopped 	= true;
	}
	
	drawFunctionOnDebugLayer(drawBlimpTicketSkipDetails);
}

int32_t Mod::preventMenuSounds(int32_t soundId, uint32_t unk1, uint32_t unk2, uint32_t unk3)
{
	if (Cheat[DISABLE_MENU_SOUNDS].Active)
	{
		const int32_t OpenMenu = 1169;
		const int32_t CloseMenu = 1168;
		
		if ((soundId == OpenMenu) || (soundId == CloseMenu))
		{
			return -1;
		}
	}
	
	// Call original function
	return mPFN_SoundEfxPlayEx_trampoline(soundId, unk1, unk2, unk3);
}

int32_t Mod::displayActionCommandsTimingHook(void *battleUnitPtr, ttyd::battle_unit::AttackParams *attackParams)
{
	// Credits to Jdaster64 for writing the original code for this function
	if (!Displays[GUARD_SUPERGUARD_TIMINGS])
	{
		return mPFN_BattleActionCommandCheckDefence_trampoline(battleUnitPtr, attackParams);
	}
	
	// Check to see if the attack will be automatically guarded/superguarded or not
	uint32_t MarioBattlePointer = reinterpret_cast<uint32_t>(getMarioBattlePointer());
	if (MarioBattlePointer)
	{
#ifdef TTYD_US
		const uint32_t DebugBadgeAddressOffset = 0x307;
#elif defined TTYD_JP
		const uint32_t DebugBadgeAddressOffset = 0x303;
#elif defined TTYD_EU
		const uint32_t DebugBadgeAddressOffset = 0x30B;
#endif
		
		uint8_t DebugBadgeCheck = *reinterpret_cast<uint8_t *>(MarioBattlePointer + DebugBadgeAddressOffset);
		if (DebugBadgeCheck > 0)
		{
			// The attack will be automatically guarded/superguarded
			return mPFN_BattleActionCommandCheckDefence_trampoline(battleUnitPtr, attackParams);
		}
	}
	
	// Reset the values checked when drawing the text
	DisplayActionCommands.TypeToDraw 	= 0;
	DisplayActionCommands.Last_A_Frame 	= -1;
	DisplayActionCommands.Last_B_Frame 	= -1;
	
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
	
	const int32_t DefenseResult = mPFN_BattleActionCommandCheckDefence_trampoline(battleUnitPtr, attackParams);
	
	const uint32_t SuccessfulTiming 		= 1;
	const uint32_t PressedTooManyButtons 	= 2;
	const uint32_t PressedTooEarly 			= 3;
	const uint32_t CannotBeSuperguarded 	= 4;
	
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
		// Check if the attack can be superguarded or not
		int8_t GuardTypesCheck = attackParams->guardTypesAllowed;
		
		if (GuardTypesCheck > 0)
		{
			// Print how many frames early the player pressed B
			DisplayActionCommands.Last_B_Frame 	= Last_B_Frame;
			DisplayActionCommands.TypeToDraw 	= PressedTooEarly;
			DisplayActionCommands.DisplayTimer 	= secondsToFrames(3);
		}
		else
		{
			// The attack cannot be superguarded, so print the text saying so
			DisplayActionCommands.TypeToDraw 	= CannotBeSuperguarded;
			DisplayActionCommands.DisplayTimer 	= secondsToFrames(3);
		}
	}
	
	return DefenseResult;
}

void displayActionCommandsTiming()
{
	// Only display if the timer is not at 0
	if (DisplayActionCommands.DisplayTimer > 0)
	{
		drawFunctionOnDebugLayerWithOrder(drawActionCommandsTiming, 10.f);
	}
}

void displayEffsActive()
{
	if (!Displays[EFFS_ACTIVE])
	{
		return;
	}
	
	drawFunctionOnDebugLayerWithOrder(drawEffsActive, 1.f);
}

int32_t warpToMap(uint32_t value)
{
	// Make sure the player is currently in the game
	if (!checkIfInGame())
	{
		return NOT_IN_GAME;
	}
	
	const char *WarpToMap;
	switch (value)
	{
		case ROGUEPORT_CENTRAL:
		{
			WarpToMap = "gor_01";
			break;
		}
		case ROGUEPORT_SEWERS_UNDERGROUND_SHOP:
		{
			WarpToMap = "tik_00";
			break;
		}
		case PETAL_MEADOWS_ENTRANCE:
		{
			WarpToMap = "hei_00";
			break;
		}
		case HOOKTAIL_CASTLE_ENTRANCE:
		{
			WarpToMap = "gon_00";
			break;
		}
		case BOGGLY_WOODS_ENTRANCE:
		{
			WarpToMap = "win_06";
			break;
		}
		case THE_GREAT_TREE_ENTRANCE:
		{
			WarpToMap = "mri_01";
			break;
		}
		case GLITZVILLE_LOBBY:
		{
			WarpToMap = "tou_02";
			break;
		}
		case GLITZVILLE_GRUBBAS_OFFICE:
		{
			WarpToMap = "tou_05";
			break;
		}
		case TWILIGHT_TOWN_WEST_SIDE:
		{
			WarpToMap = "usu_00";
			break;
		}
		case CREEPY_STEEPLE_ENTRANCE:
		{
			WarpToMap = "jin_00";
			break;
		}
		case KEELHAUL_KEY_SHANTYTOWN:
		{
			WarpToMap = "muj_01";
			break;
		}
		case PIRATES_GROTTO_ENTRANCE:
		{
			WarpToMap = "dou_00";
			break;
		}
		case EXCESS_EXPRESS_PASSENGER_CAR_CABINS_3_5_DAY:
		{
			WarpToMap = "rsh_02_a";
			break;
		}
		case RIVERSIDE_STATION_ENTRANCE:
		{
			WarpToMap = "eki_00";
			break;
		}
		case POSHLEY_HEIGHTS_STATION:
		{
			WarpToMap = "pik_00";
			break;
		}
		case FAHR_OUTPOST_PIPE_ENTRANCE:
		{
			WarpToMap = "bom_00";
			break;
		}
		case MOON_ENTRANCE:
		{
			WarpToMap = "moo_00";
			break;
		}
		case X_NAUT_FORTRESS_ENTRANCE:
		{
			WarpToMap = "aji_00";
			break;
		}
		case X_NAUT_FORTRESS_FACTORY:
		{
			WarpToMap = "aji_13";
			break;
		}
		case PALACE_OF_SHADOW_ENTRANCE:
		{
			WarpToMap = "las_00";
			break;
		}
		case PALACE_OF_SHADOW_OUTSIDE_RIDDLE_TOWER:
		{
			WarpToMap = "las_09";
			break;
		}
		case PALACE_OF_SHADOW_ROOM_BEFORE_GRODUS:
		{
			WarpToMap = "las_27";
			break;
		}
		case PIT_OF_100_TRIALS:
		{
			setNextBero("dokan_2");
			
			uint32_t NewPitFloor = getCurrentPitFloor();
			static char NewPitMap[7];
			char NewPitMapChar;
			
			// Get proper room to use for chosen floor
			if (NewPitFloor % 10 == 0)
			{
				// Current floor is a chest floor or Bonetail
				if (NewPitFloor <= 40)
				{
					NewPitMapChar = '3';
				}
				else if (NewPitFloor <= 70)
				{
					NewPitMapChar = '4';
				}
				else if (NewPitFloor <= 90)
				{
					NewPitMapChar = '5';
				}
				else // NewPitFloor == 100
				{
					NewPitMapChar = '6';
				}
			}
			else // Current floor is not a chest floor nor Bonetail
			{
				if (NewPitFloor < 50)
				{
					NewPitMapChar = '0';
				}
				else if (NewPitFloor < 80)
				{
					NewPitMapChar = '1';
				}
				else // NewPitMap < 100
				{
					NewPitMapChar = '2';
				}
			}
			
			sprintf(NewPitMap,
				"jon_0%c",
				NewPitMapChar);
			
			WarpToMap = NewPitMap;
			break;
		}
		case TITLE_SCREEN:
		{
			WarpToMap = "title";
			break;
		}
		default: 
		{
			return UNKNOWN_BEHAVIOR;
		}
	}
	
	setNextMap(WarpToMap);
	reloadRoomMain();
	
	return SUCCESS;
}

int32_t warpToMapByEvent(int32_t index)
{
	// Make sure the player is currently in the game
	if (!checkIfInGame())
	{
		return NOT_IN_GAME;
	}
	
	WarpByEventDetailsStruct WarpByEventDetails;
	if (!getEventDetails(index, &WarpByEventDetails))
	{
		return UNKNOWN_BEHAVIOR;
	}
	
	WarpByEvent.ShouldInit = true;
	
	// Make sure the new map and bero are valid
	const char *NewMap = WarpByEventDetails.Map;
	if (NewMap && (NewMap[0] != '\0'))
	{
		setNextMap(NewMap);
	}
	
	const char *NewBero = WarpByEventDetails.Bero;
	if (NewBero)
	{
		setNextBero(NewBero);
	}
	else
	{
		setNextBero("");
	}
	
	reloadRoomMain();
	return SUCCESS;
}

int32_t warpToBoss(uint32_t index)
{
	// Make sure the player is currently in the game
	if (!checkIfInGame())
	{
		return NOT_IN_GAME;
	}
	
	int32_t NewSequencePosition = -1;
	const char *BossMap;
	const char *BossBero = "";
	
	switch (index)
	{
		case BOSS_CRUMP_PROLOGUE:
		{
			NewSequencePosition = 4;
			BossMap = "gor_00";
			break;
		}
		case BOSS_GUS:
		{
			BossMap = "gor_02";
			BossBero = "w_bero";
			
			// Turn off GSWF(1213) to allow Gus to be fought again
			ttyd::swdrv::swClear(1213);
			break;
		}
		case BOSS_BLOOPER:
		{
			NewSequencePosition = 20;
			BossMap = "tik_02";
			BossBero = "w_bero_1";
			break;
		}
		case BOSS_GOLD_FUZZY:
		{
			// Turn on GSWF(1774) and GSWF(1775) to allow the Gold Fuzzy to be fought again
			static const uint16_t FlagsToSet[] = 
			{
				1774,
				1775,
			};
			
			// Set the flags
			uint32_t Size = sizeof(FlagsToSet) / sizeof(FlagsToSet[0]);
			for (uint32_t i = 0; i < Size; i++)
			{
				ttyd::swdrv::swSet(FlagsToSet[i]);
			}
			
			NewSequencePosition = 31;
			BossMap = "hei_10";
			break;
		}
		case BOSS_RED_BONES:
		{
			NewSequencePosition = 38;
			BossMap = "gon_03";
			BossBero = "e_bero";
			break;
		}
		case BOSS_HOOKTAIL:
		{
			NewSequencePosition = 54;
			BossMap = "gon_11";
			
			// Turn off GSWF(1498) to allow Hooktail to be fought again
			ttyd::swdrv::swClear(1498);
			break;
		}
		case BOSS_SHADOW_SIRENS_CH2:
		{
			NewSequencePosition = 84;
			BossMap = "win_00";
			BossBero = "w_bero";
			break;
		}
		case BOSS_MAGNUS_CH2:
		{
			NewSequencePosition = 110;
			BossMap = "mri_01";
			BossBero = "e_bero"; // Required to avoid a crash
			break;
		}
		case BOSS_MACHO_GRUBBA:
		{
			NewSequencePosition = 163;
			BossMap = "tou_03";
			BossBero = "w_bero";
			break;
		}
		case BOSS_ATOMIC_BOO:
		{
			NewSequencePosition = 194;
			BossMap = "jin_00";
			
			// Turn off GSWF(2226) to allow the Atomic Boo to be fought again
			ttyd::swdrv::swClear(2226);
			break;
		}
		case BOSS_DOOPLISS_1:
		{
			NewSequencePosition = 199;
			BossMap = "jin_04";
			break;
		}
		case BOSS_DOOPLISS_2:
		{
			// Force Vivian to be the current partner out
			spawnPartnerOrFollower(ttyd::party::PartyMembers::kVivian);
			
			NewSequencePosition = 210;
			BossMap = "jin_04";
			break;
		}
		case BOSS_CORTEZ:
		{
			NewSequencePosition = 252;
			BossMap = "muj_12";
			break;
		}
		case BOSS_CRUMP_CH5:
		{
			NewSequencePosition = 259;
			BossMap = "muj_00";
			break;
		}
		case BOSS_SMORG:
		{
			NewSequencePosition = 331;
			BossMap = "rsh_06_a";
			break;
		}
		case BOSS_MAGNUS_CH8:
		{
			NewSequencePosition = 372;
			BossMap = "aji_14";
			BossBero = "w_bero";
			break;
		}
		case BOSS_DARK_BONES:
		{
			NewSequencePosition = 382;
			BossMap = "las_05";
			BossBero = "e_bero_1";
			break;
		}
		case BOSS_GLOOMTAIL:
		{
			NewSequencePosition = 387;
			BossMap = "las_26";
			break;
		}
		case BOSS_SHADOW_SIRENS_CH8:
		{
			NewSequencePosition = 390;
			BossMap = "las_09";
			BossBero = "majyorin_evt";
			break;
		}
		case BOSS_GRODUS:
		{
			NewSequencePosition = 399;
			BossMap = "las_28";
			BossBero = "e_bero";
			break;
		}
		case BOSS_BOWSER_AND_KAMMY:
		{
			NewSequencePosition = 399;
			BossMap = "las_28";
			BossBero = "koopa_evt";
			break;
		}
		case BOSS_SHADOW_QUEEN_1:
		{
			NewSequencePosition = 400;
			BossMap = "las_29";
			BossBero = "sekai_yami2";
			break;
		}
		case BOSS_SHADOW_QUEEN_2:
		{
			NewSequencePosition = 400;
			BossMap = "las_29";
			BossBero = "minnnanokoe";
			break;
		}
		case BOSS_BONETAIL:
		{
			BossMap = "jon_06";
			BossBero = "dokan_2";
			
			// Set the Pit floor for Bonetail
			setPitFloor(100);
			
			// Turn off GSWF(5084) and GSWF(5085) to allow Bonetail to be fought again
			static const uint16_t FlagsToSet[] = 
			{
				5084,
				5085,
			};
			
			// Set the flags
			uint32_t Size = sizeof(FlagsToSet) / sizeof(FlagsToSet[0]);
			for (uint32_t i = 0; i < Size; i++)
			{
				ttyd::swdrv::swClear(FlagsToSet[i]);
			}
			break;
		}
		default:
		{
			return UNKNOWN_BEHAVIOR;
		}
	}
	
	// Bring out a partner if one is not currently out
	// Do not bring a partner out for the first Crump fight
	if ((index != BOSS_CRUMP_PROLOGUE) && !getPartnerPointer())
	{
		spawnFailsafePartnerOrFollower(true);
	}
	
	// Set the new map, bero, and Sequence position
	setNextMap(BossMap);
	setNextBero(BossBero);
	
	// Make sure the Sequence position should be changed
	if (NewSequencePosition >= 0)
	{
		setSequencePosition(static_cast<uint32_t>(NewSequencePosition));
	}
	
	// Warp to the boss
	reloadRoomMain();
	return SUCCESS;
}

int32_t warpToMapByString(const char *map)
{
	// Make sure the player is currently in the game
	if (!checkIfInGame())
	{
		return NOT_IN_GAME;
	}
	
	setNextMap(map);
	reloadRoomMain();
	
	return SUCCESS;
}

uint32_t Mod::setIndexWarpEntrance(ttyd::evtmgr::EvtEntry *evt, uint32_t waitMode)
{
	// Clear the array holding the loading zone names
	clearMemory(WarpByIndex.EntranceList, sizeof(WarpByIndex.EntranceList));
	
	// Get the start of the loading zone addresses
	uint32_t eventLZAddresses = static_cast<uint32_t>(evt->lwData[0]);
	
	// Copy each loading zone to the array
	const char **tempEntranceArray = WarpByIndex.EntranceList;
	uint32_t tempEventAddress = eventLZAddresses;
	uint32_t LoadingZoneTotal = 0;
	
	bool FoundEndOfArray = false;
	while (!FoundEndOfArray)
	{
		uint32_t tempAddress = *reinterpret_cast<uint32_t *>(tempEventAddress);
		if (tempAddress)
		{
			tempEntranceArray[LoadingZoneTotal] = reinterpret_cast<const char *>(tempAddress);
			tempEventAddress += 0x3C;
			LoadingZoneTotal++;
		}
		else
		{
			FoundEndOfArray = true;
		}
	}
	
	// Check to see if warping by index
	if (WarpByIndex.RunIndexWarpCode)
	{
		WarpByIndex.RunIndexWarpCode = false;
		
		// Make sure the chosen entrance exists for the chosen map
		uint32_t ChosenEntrance = WarpByIndex.EntranceId - 1;
		const char *ChosenEntranceName;
		
		if (ChosenEntrance < LoadingZoneTotal)
		{
			// Valid entrance chosen
			ChosenEntranceName = reinterpret_cast<const char *>(
				*reinterpret_cast<uint32_t *>(
					eventLZAddresses + (ChosenEntrance * 0x3C)));
		}
		else
		{
			// Invalid entrance chosen
			ChosenEntranceName = "";
		}
		
		// Set the chosen entrance
		char *NextBeroGSW = reinterpret_cast<char *>(
			reinterpret_cast<uint32_t>(
				ttyd::mariost::globalWorkPointer) + 0x11C);
		
		strcpy(NextBeroGSW, ChosenEntranceName);
		setNextBero(ChosenEntranceName);
	}
	
	// Call original function
	return mPFN_evt_bero_get_info_trampoline(evt, waitMode);
}

}