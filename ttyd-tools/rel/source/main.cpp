#include "global.h"
#include "commonfunctions.h"
#include "menufunctions.h"
#include "draw.h"
#include "mod.h"
#include "patch.h"
#include "menu.h"
#include "codes.h"
#include "assembly.h"

#include <gc/OSCache.h>
#include <gc/OSAlloc.h>
#include <ttyd/item_data.h>
#include <ttyd/seq_mapchange.h>
#include <ttyd/seqdrv.h>
#include <ttyd/npcdrv.h>
#include <ttyd/swdrv.h>
#include <ttyd/mario.h>
#include <ttyd/mario_party.h>
#include <ttyd/party.h>
#include <ttyd/memory.h>
#include <ttyd/seq_title.h>
#include <ttyd/mario_pouch.h>
#include <ttyd/sac_scissor.h>

#include <cstdio>
#include <cinttypes>
#include <cstring>

namespace mod {

bool displayMegaJumpBadgeInMenu(uint32_t checkBit)
{
	if (checkIfBadgeEquipped(ttyd::item_data::Item::UpgradedPowerJump))
	{
		return false;
	}
	else
	{
		return (checkBit & (1 << 9)); // Check if the 9 bit is on
	}
}

bool displayMegaHammerBadgesInMenu(uint32_t checkBit)
{
	if (checkIfBadgeEquipped(ttyd::item_data::Item::UpgradedPowerSmash) || 
		checkIfBadgeEquipped(ttyd::item_data::Item::UpgradedQuakeHammer))
	{
		return false;
	}
	else
	{
		return (checkBit & (1 << 10)); // Check if the 10 bit is on
	}
}

uint32_t fixBlooperCrash1(uint32_t unkValue, void *battleUnitPointer)
{
	if (checkIfPointerIsValid(battleUnitPointer))
	{
		#ifdef TTYD_US
		uint32_t offset = 0x218;
		#elif defined TTYD_JP
		uint32_t offset = 0x214;
		#elif defined TTYD_EU
		uint32_t offset = 0x218;
		#endif
		
		*reinterpret_cast<uint32_t *>(
			reinterpret_cast<uint32_t>(battleUnitPointer) 
				+ offset) = unkValue;
	}
	
	return 2;
}

void preventTextboxOptionSelection(char *currentText, void *storeAddress, int32_t selectedOption)
{
	int32_t NewOption;
	// const int32_t FirstOption = 0;
	const int32_t SecondOption = 1;
	const int32_t ThirdOption = 2;
	
	if (compareStrings(currentText, "stg6_rsh_diary_01_yn"))
	{
		// Prevent the first option from being selected, so that the game does not crash when reading the diary
		// Only needs to run when not on the train
		if (!compareStrings(ttyd::seq_mapchange::NextArea, "rsh"))
		{
			if (selectedOption == ThirdOption)
			{
				ttyd::seqdrv::seqSetSeq(ttyd::seqdrv::SeqIndex::kGameOver, nullptr, nullptr);
			}
			
			NewOption = SecondOption;
		}
		else
		{
			// Store the original value
			NewOption = selectedOption;
		}
	}
	else
	{
		// Store the original value
		NewOption = selectedOption;
	}
	
	*reinterpret_cast<int32_t *>(
		reinterpret_cast<uint32_t>(storeAddress) + 0x9C) = NewOption;
}

uint32_t fixRoomProblems()
{
	uint32_t SequencePosition = getSequencePosition();
	
	if (compareStringToNextMap("nok_00"))
	{
		// Prevent the game from crashing if the player enters the intro cutscene after interacting with an NPC that is past slot 10
		// Check if the cutscene is going to play
		if (SequencePosition < 26)
		{
			// Clear the pointer used to check which animation Mario should use when greeting the Koopa
			uint32_t fbatPointer = reinterpret_cast<uint32_t>(ttyd::npcdrv::fbatGetPointer());
			*reinterpret_cast<uint32_t *>(fbatPointer + 0x4) = 0; // Mario will do no animation when the pointer is not set
		}
	}
	else if (compareStringToNextMap("rsh_05_a"))
	{
		// Prevent the game from crashing if the player enters rsh_05_a with the Sequence past 338
		if (SequencePosition > 338)
		{
			// Set the Sequence to 338 to prevent the crash
			setSequencePosition(338);
		}
	}
	else if (compareStringToNextMap("aji_13"))
	{
		// Prevent the game from crashing if the conveyor belt has not been activated
		// Set GW(11) to 0 upon entering the room to prevent the crash
		setGW(11, 0);
	}
	else if (compareStringToNextMap("las_08"))
	{
		// Prevent the game from crashing if the player entered las_08 with the Sequence as 385 and GSW(1121) at 7
		if (SequencePosition == 385)
		{
			// Check if GSW(1121) is currently at 7
			uint32_t GSW_1121 = ttyd::swdrv::swByteGet(1121);
			if (GSW_1121 == 7)
			{
				// Lower the value to 6 to prevent the game from crashing
				ttyd::swdrv::swByteSet(1121, 6);
			}
		}
	}
	
	// The overwritten instruction sets r3 to 512, so return 512
	return 512;
}

void *fixEvtMapBlendSetFlagPartnerCrash(void *partnerPtr)
{
	// Bring out a partner if no partner is currently out
	if (!partnerPtr)
	{
		ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
		ttyd::party::PartyMembers PreviousPartnerOut = player->prevFollowerId[0];

		// Check if a partner was previously out
		if (PreviousPartnerOut != ttyd::party::PartyMembers::kNone)
		{
			// A partner was previously out, so bring them back out
			spawnPartnerOrFollower(PreviousPartnerOut);
		}
		else
		{
			// No partner was previously out, so bring out Goombella
			spawnPartnerOrFollower(ttyd::party::PartyMembers::kGoombella);
		}
		
		return getPartnerPointer();
	}
	else
	{
		return partnerPtr;
	}
}

void *fixEvtMapBlendSetFlagFollowerCrash(void *followerPtr)
{
	// Bring out a follower if no follower is currently out
	if (!followerPtr)
	{
		ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
		ttyd::party::PartyMembers PreviousFollowerOut = player->prevFollowerId[1];

		// Check if a follower was previously out
		if (PreviousFollowerOut != ttyd::party::PartyMembers::kNone)
		{
			// A follower was previously out, so bring them back out
			spawnPartnerOrFollower(PreviousFollowerOut);
		}
		else
		{
			// No follower was previously out, so bring out Gus
			spawnPartnerOrFollower(ttyd::party::PartyMembers::kGus);
		}
		
		return getFollowerPointer();
	}
	else
	{
		return followerPtr;
	}
}

void *preventPouchInitMemoryLeak(uint32_t heap, uint32_t size)
{
	// Check if the memory has already been allocated or not
	void *PouchPtr = ttyd::mario_pouch::pouchGetPtr();
	if (!PouchPtr)
	{
		// The memory has not been allocated, so allocate it
		return ttyd::memory::__memAlloc(heap, size);
	}
	else
	{
		// The memory has already been allocated, so clear it
		return clearMemory(PouchPtr, size);
	}
}

const char *replaceJumpFallAnim(char *jumpFallString)
{
	if (compareStringsSize(jumpFallString, "M_J_", 4))
	{
		const char *MapCheck = "gor_01";
		if (compareStringToNextMap(MapCheck))
		{
			if (MenuVar.MarioFreeze)
			{
				// Return an arbitrary string
				return MapCheck;
			}
			else
			{
				ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
				
				int32_t MarPosX = static_cast<int32_t>(player->playerPosition[0]);
				if ((MarPosX >= -9) && (MarPosX <= 8))
				{
					int32_t MarPosZ = static_cast<int32_t>(player->playerPosition[2]);
					if ((MarPosZ >= 245) && (MarPosZ <= 265))
					{
						int32_t MarPosY = static_cast<int32_t>(player->playerPosition[1]);
						if (MarPosY >= 59)
						{
							MenuVar.MarioFreeze = true;
							
							// Return an arbitrary string
							return MapCheck;
						}
					}
				}
			}
		}
	}
	return jumpFallString;
}

void displayTitleScreenAndFileSelectScreenInfo()
{
	if (checkForSpecificSeq(ttyd::seqdrv::SeqIndex::kTitle))
	{
		// Check to see if the title screen is ready
		// Check to see if the curtain is fully displayed or not
		uint32_t TitleMainCheck = *reinterpret_cast<uint32_t *>(
			reinterpret_cast<uint32_t>(ttyd::seq_title::seqTitleWorkPointer2) + 0x8);
		
		if ((TitleMainCheck >= 2) && (TitleMainCheck < 12))
		{
			// Curtain is not fully displayed
			// Draw the title screen info
			drawFunctionOn2DLayer(drawTitleScreenInfo);
		}
	}
	else if (checkForSpecificSeq(ttyd::seqdrv::SeqIndex::kLoad))
	{
		// Check to see if the curtain is down or not
		uint32_t SeqMainCheck = ttyd::seqdrv::seqWork.wFileSelectScreenProgress;
		
		if (SeqMainCheck == 2)
		{
			// Draw the file select screen info
			drawFunctionOn2DLayer(drawFileSelectScreenInfo);
		}
	}
}

int32_t Mod::pauseMenuPreventUnpause(void *pauseMenuPointer)
{
	// Prevent unpausing if L is being held
	uint32_t OpenMenuCombo = PAD_L | PAD_START;
	
	if (checkButtonComboEveryFrame(OpenMenuCombo))
	{
		// Prevent being able to close the menu if L is being held
		return 0;
	}
	else
	{
		// Call original function
		return mPFN_winRootMain_trampoline(pauseMenuPointer);
	}
}

bool Mod::performPreBattleActions()
{
	// Make sure the Jump and Hammer upgrades have been properly checked
	recheckJumpAndHammerLevels();
	
	// Clear the cache for Mario if his stats were changed manually
	if (ClearCacheForBattles.MarioStatsShouldBeCleared)
	{
		ClearCacheForBattles.MarioStatsShouldBeCleared = false;
		
		uint32_t PouchPtr = reinterpret_cast<uint32_t>(ttyd::mario_pouch::pouchGetPtr());
		void *AddressesToClearStart = reinterpret_cast<void *>(PouchPtr + 0x70);
		gc::OSCache::DCFlushRange(AddressesToClearStart, 0x21);
	}
	
	// Clear the cache for the partners if their stats were changed manually
	if (ClearCacheForBattles.PartnerStatsShouldBeCleared)
	{
		ClearCacheForBattles.PartnerStatsShouldBeCleared = false;
		
		uint32_t PouchPtr = reinterpret_cast<uint32_t>(ttyd::mario_pouch::pouchGetPtr());
		void *AddressesToClearStart = reinterpret_cast<void *>(PouchPtr + 0xE);
		gc::OSCache::DCFlushRange(AddressesToClearStart, 0x61);
	}
	
	// Call original function
	return mPFN_battle_init_trampoline();
}

const char *Mod::getCustomMessage(const char *key)
{
	char *tempNextArea = ttyd::seq_mapchange::NextArea;
	if (compareStrings(key, "stg6_rsh_diary_01"))
	{
		// Change the text asking if you want to read the diary
		// Only needs to change when not on the train
		if (!compareStrings(tempNextArea, "rsh"))
		{
			const char *message = "<system>\n@ me if you think this mod\nsucks, but <shake><col EC2013FF>don't</col></shake><col DCC8DCFF> mention\nwhy you're @ing me.</col>\n<o>";
			
			return message;
		}
	}
	else if (compareStrings(key, "stg6_rsh_diary_01_yn"))
	{
		// Change the yes/no text answers for the diary
		// Only needs to change when not on the train
		if (!compareStrings(tempNextArea, "rsh"))
		{
			#ifdef TTYD_JP
			const char *message = "<select 0 0 300 40>No\nAlright\nChoose this option";
			#else
			const char *message = "<select 0 0 0 40>No\nAlright\nChoose this option";
			#endif
			
			return message;
		}
	}

	// Call original function
	return mPFN_msgSearch_trampoline(key);
}

uint32_t Mod::pauseArtAttackTimer()
{
	if (!Displays[ART_ATTACK_HITBOXES])
	{
		// Call original function
		return mPFN_scissor_timer_main_trampoline();
	}
	
	// Prevent the function from running if Y is being held
	if (checkButtonComboEveryFrame(PAD_Y))
	{
		// Make sure the timer is not at 59 or lower, as pausing it then will cause issues
		void *ArtAttackPointer = ttyd::sac_scissor::GetScissorPtr();
		
		int32_t ArtAttackTimer = *reinterpret_cast<int32_t *>(
			reinterpret_cast<uint32_t>(ArtAttackPointer) + 0x104);
		
		if (ArtAttackTimer > 59)
		{
			return 0;
		}
	}
	
	// Call original function
	return mPFN_scissor_timer_main_trampoline();
}

int32_t Mod::fixMarioKeyOn()
{
	ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
	int8_t MarioKey = player->wKey;
	
	// Make sure the value is greater than 0
	if (MarioKey < 1)
	{
		player->wKey = 1;
	}
	
	// Call original function
	return mPFN_marioKeyOn_trampoline();
}

bool Mod::performRelPatches(gc::OSModule::OSModuleInfo *newModule, void *bss)
{
	// Call the original function immediately, as the REL file should be linked before applying patches
	const bool Result = mPFN_OSLink_trampoline(newModule, bss);
	
	// Make sure a REL file is currently loaded
	gc::OSModule::OSModuleInfo *CurrentRelModuleInfo = getCurrentRelModuleInfo();
	if (!CurrentRelModuleInfo)
	{
		return Result;
	}
	
	#ifdef TTYD_US
	const uint32_t LAS = 0x10;
	const uint32_t SQ_Cutscene_Spawn_Textboxes_Script_Offset = 0x36FBC;
	const uint32_t SQ_Cutscene_Spawn_Textboxes_Script_Loop_Offset = 0x64;
	const uint32_t SQ_Cutscene_Spawn_Textboxes_Script_WaitMS_Offset = 0x114;
	#elif defined TTYD_JP
	const uint32_t LAS = 0x11;
	const uint32_t SQ_Cutscene_Spawn_Textboxes_Script_Offset = 0x36FC0;
	const uint32_t SQ_Cutscene_Spawn_Textboxes_Script_Loop_Offset = 0x58;
	const uint32_t SQ_Cutscene_Spawn_Textboxes_Script_WaitMS_Offset = 0xD4;
	#elif defined TTYD_EU
	const uint32_t LAS = 0x11;
	const uint32_t SQ_Cutscene_Spawn_Textboxes_Script_Offset = 0x36FBC;
	const uint32_t SQ_Cutscene_Spawn_Textboxes_Script_Loop_Offset = 0x64;
	const uint32_t SQ_Cutscene_Spawn_Textboxes_Script_WaitMS_Offset = 0x114;
	#endif
	
	uint32_t CurrentRelModuleInfoRaw = reinterpret_cast<uint32_t>(CurrentRelModuleInfo);
	switch (CurrentRelModuleInfo->id)
	{
		case LAS:
		{
			if (compareStringToNextMap("las_29"))
			{
				// Make changes to the function that spawns the textboxes, to prevent the standard heap from being corrupted
				// Change the loop count from 10 to 5
				*reinterpret_cast<uint32_t *>(
					CurrentRelModuleInfoRaw + 
						SQ_Cutscene_Spawn_Textboxes_Script_Offset + 
							SQ_Cutscene_Spawn_Textboxes_Script_Loop_Offset) = 5;
				
				// Wait for 400ms instead of 200ms at the end of the loop
				*reinterpret_cast<uint32_t *>(
					CurrentRelModuleInfoRaw + 
						SQ_Cutscene_Spawn_Textboxes_Script_Offset + 
							SQ_Cutscene_Spawn_Textboxes_Script_WaitMS_Offset) = 400;
			}
			return Result;
		}
		default:
		{
			return Result;
		}
	}
}

void addTextToHeapArray(char *text)
{
	char *tempHeapBuffer = HeapBuffer;
	
	// Make sure adding the new text will not result in an overflow
	uint32_t NewTextSize = strlen(text);
	uint32_t CurrentHeapSize = strlen(tempHeapBuffer);
	
	uint32_t NewHeapSize = CurrentHeapSize + NewTextSize + 1;
	uint32_t MaxHeapSize = sizeof(HeapBuffer);
	
	if (NewHeapSize > MaxHeapSize)
	{
		// Adding the new text will result in an overflow, so don't add it
		return;
	}
	
	// Add the new text onto the heap
	strcat(tempHeapBuffer, text);
}

void checkHeaps()
{
	char *tempDisplayBuffer = DisplayBuffer;
	
	// Check the standard heaps
	int32_t TotalHeaps = gc::OSAlloc::NumHeaps;
	gc::OSAlloc::HeapInfo *HeapArray = gc::OSAlloc::HeapArray;
	
	for (int32_t i = 0; i < TotalHeaps; i++)
	{
		const gc::OSAlloc::HeapInfo &heap = HeapArray[i];
		bool valid = true;
		
		gc::OSAlloc::ChunkInfo *currentChunk = nullptr;
		gc::OSAlloc::ChunkInfo *prevChunk = nullptr;
		for (currentChunk = heap.firstUsed; currentChunk; currentChunk = currentChunk->next)
		{
			// Check pointer sanity
			if (!checkIfPointerIsValid(currentChunk))
			{
				valid = false;
				break;
			}
			
			// Sanity check size
			if (currentChunk->size >= 0x1800000)
			{
				valid = false;
				break;
			}

			// Check linked list integrity
			if (prevChunk != currentChunk->prev)
			{
				valid = false;
				break;
			}

			prevChunk = currentChunk;
		}
		
		if (!valid)
		{
			sprintf(tempDisplayBuffer,
				"Standard Heap %" PRId32 " corrupt at 0x%08" PRIX32 "\n",
				i,
				reinterpret_cast<uint32_t>(currentChunk));
			
			// Add the text to the heap buffer
			addTextToHeapArray(tempDisplayBuffer);
		}
	}
	
	// Check the smart heap
	ttyd::memory::SmartWork *SmartWorkPtr = ttyd::memory::smartWorkPointer;
	bool valid = true;
	
	ttyd::memory::SmartAllocationData *currentChunk = nullptr;
	ttyd::memory::SmartAllocationData *prevChunk = nullptr;
	for (currentChunk = SmartWorkPtr->pFirstUsed; currentChunk; currentChunk = currentChunk->pNext)
	{
		// Check pointer sanity
		if (!checkIfPointerIsValid(currentChunk))
		{
			valid = false;
			break;
		}
		
		// Sanity check size
		if (currentChunk->usedSize >= 0x1800000)
		{
			valid = false;
			break;
		}

		// Check linked list integrity
		if (prevChunk != currentChunk->pPrev)
		{
			valid = false;
			break;
		}
		
		prevChunk = currentChunk;
	}
	
	if (!valid)
	{
		sprintf(tempDisplayBuffer,
			"Smart Heap corrupt at 0x%08" PRIX32 "\n",
			reinterpret_cast<uint32_t>(currentChunk));
		
		// Add the text to the heap buffer
		addTextToHeapArray(tempDisplayBuffer);
	}
	
	// Draw any errors that occured
	if (HeapBuffer[0] != '\0')
	{
		drawFunctionOnDebugLayer(drawHeapArrayErrors);
	}
}

void initAddressOverwrites()
{
	#ifdef TTYD_US
	void *AllowRunningFromBattlesAddress 				= reinterpret_cast<void *>(0x80123CA4);
	void *ForceNPCItemDropAddress 						= reinterpret_cast<void *>(0x8004EC10);
	void *DebugModeInitialzeAddress 					= reinterpret_cast<void *>(0x80009B2C);
	void *DebugModeShowBuildDateAddress 				= reinterpret_cast<void *>(0x80008FE8);
	void *PauseMenuPartnerMenuAddress 					= reinterpret_cast<void *>(0x801649A0);
	void *PauseMenuBadgeMenuAddress 					= reinterpret_cast<void *>(0x80164A44);
	void *ReplaceJumpAnimAddress 						= reinterpret_cast<void *>(0x800411D0);
	void *PreventImportantItemCutscenesAddress 			= reinterpret_cast<void *>(0x800ABCD8);
	void *msgWindowMrAddress 							= reinterpret_cast<void *>(0x800816F4);
	void *DisplayBattleMenuJumpAddress 					= reinterpret_cast<void *>(0x80122BA4);
	void *DisplayBattleMenuHammerAddress 				= reinterpret_cast<void *>(0x80122BB8);
	void *FixBlooperCrash1Address 						= reinterpret_cast<void *>(0x8010F810);
	void *FixBlooperCrash2Address 						= reinterpret_cast<void *>(0x8010F888);
	void *PreventTextboxSelectionAddress 				= reinterpret_cast<void *>(0x800D214C);
	void *BacktraceScreenFontSizeAddress 				= reinterpret_cast<void *>(0x80428BC0);
	void *BacktraceScreenPPCHaltBranchAddress 			= reinterpret_cast<void *>(0x8025E4A4);
	void *BacktraceScreenEndBranchAddress 				= reinterpret_cast<void *>(0x8025E4A8);
	void *FixRoomProblemsAddress 						= reinterpret_cast<void *>(0x800087C8);
	void *DisableDPadOptionsDisplayAddress 				= reinterpret_cast<void *>(0x8013D148);
	void *FixEvtMapBlendSetFlagPartnerCrashAddress 		= reinterpret_cast<void *>(0x800389C4);
	void *FixEvtMapBlendSetFlagFollowerCrashAddress 	= reinterpret_cast<void *>(0x80038A0C);
	void *PreventPouchInitMemoryLeakAddress 			= reinterpret_cast<void *>(0x800D59DC);
	void *InitStageEventsAddress 						= reinterpret_cast<void *>(0x800080E4);
	void *FallThroughMostObjectsStandAddress 			= reinterpret_cast<void *>(0x8008E9DC);
	void *FallThroughMostObjectsTubeAddress 			= reinterpret_cast<void *>(0x8008E1E8);
	void *FallThroughMostObjectsBowserAddress 			= reinterpret_cast<void *>(0x8021A30C);
	#elif defined TTYD_JP
	void *AllowRunningFromBattlesAddress 				= reinterpret_cast<void *>(0x8011E7DC);
	void *ForceNPCItemDropAddress 						= reinterpret_cast<void *>(0x8004DFB0);
	void *DebugModeInitialzeAddress 					= reinterpret_cast<void *>(0x8000999C);
	void *DebugModeShowBuildDateAddress 				= reinterpret_cast<void *>(0x80008EB8);
	void *PauseMenuPartnerMenuAddress 					= reinterpret_cast<void *>(0x8015EFBC);
	void *PauseMenuBadgeMenuAddress 					= reinterpret_cast<void *>(0x8015F060);
	void *ReplaceJumpAnimAddress 						= reinterpret_cast<void *>(0x80040B34);
	void *PreventImportantItemCutscenesAddress 			= reinterpret_cast<void *>(0x800AA01C);
	void *msgWindowMrAddress 							= reinterpret_cast<void *>(0x80080B6C);
	void *DisplayBattleMenuJumpAddress 					= reinterpret_cast<void *>(0x8011D6DC);
	void *DisplayBattleMenuHammerAddress 				= reinterpret_cast<void *>(0x8011D6F0);
	void *FixBlooperCrash1Address 						= reinterpret_cast<void *>(0x8010A724);
	void *FixBlooperCrash2Address 						= reinterpret_cast<void *>(0x8010A79C);
	void *PreventTextboxSelectionAddress 				= reinterpret_cast<void *>(0x800CE01C);
	void *BacktraceScreenFontSizeAddress 				= reinterpret_cast<void *>(0x80422618);
	void *FixRoomProblemsAddress 						= reinterpret_cast<void *>(0x800086F0);
	void *DisableDPadOptionsDisplayAddress 				= reinterpret_cast<void *>(0x80137C1C);
	void *FixEvtMapBlendSetFlagPartnerCrashAddress 		= reinterpret_cast<void *>(0x80038328);
	void *FixEvtMapBlendSetFlagFollowerCrashAddress 	= reinterpret_cast<void *>(0x80038370);
	void *PreventPouchInitMemoryLeakAddress 			= reinterpret_cast<void *>(0x800D17A8);
	void *InitStageEventsAddress 						= reinterpret_cast<void *>(0x80008054);
	void *FallThroughMostObjectsStandAddress 			= reinterpret_cast<void *>(0x8008D428);
	void *FallThroughMostObjectsTubeAddress 			= reinterpret_cast<void *>(0x8008CC4C);
	void *FallThroughMostObjectsBowserAddress 			= reinterpret_cast<void *>(0x80215668);
	#elif defined TTYD_EU
	void *AllowRunningFromBattlesAddress 				= reinterpret_cast<void *>(0x80124BE4);
	void *ForceNPCItemDropAddress 						= reinterpret_cast<void *>(0x8004ECDC);
	void *DebugModeInitialzeAddress 					= reinterpret_cast<void *>(0x80009CF0);
	void *DebugModeShowBuildDateAddress 				= reinterpret_cast<void *>(0x800091B4);
	void *PauseMenuPartnerMenuAddress 					= reinterpret_cast<void *>(0x80166490);
	void *PauseMenuBadgeMenuAddress 					= reinterpret_cast<void *>(0x80166534);
	void *ReplaceJumpAnimAddress 						= reinterpret_cast<void *>(0x800412B8);
	void *PreventImportantItemCutscenesAddress 			= reinterpret_cast<void *>(0x800AD0A8);
	void *msgWindowMrAddress 							= reinterpret_cast<void *>(0x800829b0);
	void *DisplayBattleMenuJumpAddress 					= reinterpret_cast<void *>(0x80123AE4);
	void *DisplayBattleMenuHammerAddress 				= reinterpret_cast<void *>(0x80123AF8);
	void *FixBlooperCrash1Address 						= reinterpret_cast<void *>(0x801106E8);
	void *FixBlooperCrash2Address 						= reinterpret_cast<void *>(0x80110760);
	void *PreventTextboxSelectionAddress 				= reinterpret_cast<void *>(0x800D2F44);
	void *BacktraceScreenFontSizeAddress 				= reinterpret_cast<void *>(0x804356C8);
	void *BacktraceScreenPPCHaltBranchAddress 			= reinterpret_cast<void *>(0x8026207C);
	void *BacktraceScreenEndBranchAddress 				= reinterpret_cast<void *>(0x80262080);
	void *FixRoomProblemsAddress 						= reinterpret_cast<void *>(0x80008994);
	void *DisableDPadOptionsDisplayAddress 				= reinterpret_cast<void *>(0x8013EC30);
	void *FixEvtMapBlendSetFlagPartnerCrashAddress 		= reinterpret_cast<void *>(0x80038AAC);
	void *FixEvtMapBlendSetFlagFollowerCrashAddress 	= reinterpret_cast<void *>(0x80038AF4);
	void *PreventPouchInitMemoryLeakAddress 			= reinterpret_cast<void *>(0x800D67E4);
	void *InitStageEventsAddress 						= reinterpret_cast<void *>(0x800082BC);
	void *FallThroughMostObjectsStandAddress 			= reinterpret_cast<void *>(0x8008FD38);
	void *FallThroughMostObjectsTubeAddress 			= reinterpret_cast<void *>(0x8008F544);
	void *FallThroughMostObjectsBowserAddress 			= reinterpret_cast<void *>(0x8021DD9C);
	#endif
	
	patch::writeBranchBL(AllowRunningFromBattlesAddress, reinterpret_cast<void *>(StartAllowRunningFromBattles));
	
	patch::writeBranchBL(ForceNPCItemDropAddress, reinterpret_cast<void *>(StartForceNPCItemDrop));
	
	patch::writeBranchBL(ReplaceJumpAnimAddress, reinterpret_cast<void *>(StartReplaceJumpFallAnim));
	
	patch::writeBranchBL(DisplayBattleMenuJumpAddress, reinterpret_cast<void *>(StartDisplayMegaJumpBadgeBattleMenu));
	
	patch::writeBranchBL(DisplayBattleMenuHammerAddress, reinterpret_cast<void *>(StartDisplayMegaHammerBadgesBattleMenu));
	
	patch::writeBranchBL(FixBlooperCrash1Address, reinterpret_cast<void *>(StartFixBlooperCrash1));
	
	patch::writeBranchBL(FixBlooperCrash2Address, reinterpret_cast<void *>(StartFixBlooperCrash2));
	
	patch::writeBranchBL(PreventTextboxSelectionAddress, reinterpret_cast<void *>(StartPreventTextboxSelection));
	
	patch::writeBranchBL(FixRoomProblemsAddress, reinterpret_cast<void *>(fixRoomProblems));
	
	patch::writeBranchBL(DisableDPadOptionsDisplayAddress, reinterpret_cast<void *>(StartDisableDPadOptionsDisplay));
	
	patch::writeBranchBL(FixEvtMapBlendSetFlagPartnerCrashAddress, reinterpret_cast<void *>(StartFixEvtMapBlendSetFlagPartnerCrash));
	
	patch::writeBranchBL(FixEvtMapBlendSetFlagFollowerCrashAddress, reinterpret_cast<void *>(StartFixEvtMapBlendSetFlagFollowerCrash));
	
	patch::writeBranchBL(PreventPouchInitMemoryLeakAddress, reinterpret_cast<void *>(preventPouchInitMemoryLeak));
	
	patch::writeBranchBL(InitStageEventsAddress, reinterpret_cast<void *>(initStageEvents));
	
	patch::writeBranchBL(FallThroughMostObjectsStandAddress, reinterpret_cast<void *>(StartFallThroughMostObjectsStandard));
	
	patch::writeBranchBL(FallThroughMostObjectsTubeAddress, reinterpret_cast<void *>(StartFallThroughMostObjectsStandard));
	
	patch::writeBranchBL(FallThroughMostObjectsBowserAddress, reinterpret_cast<void *>(StartFallThroughMostObjectsBowser));
	
	*reinterpret_cast<uint32_t *>(DebugModeInitialzeAddress) 				= 0x3800FFFF; // li r0,-1
	*reinterpret_cast<uint32_t *>(DebugModeShowBuildDateAddress) 			= 0x60000000; // nop
	
	*reinterpret_cast<uint32_t *>(PauseMenuPartnerMenuAddress) 				= 0x60000000; // nop
	*reinterpret_cast<uint32_t *>(PauseMenuBadgeMenuAddress) 				= 0x60000000; // nop
	
	*reinterpret_cast<uint32_t *>(PreventImportantItemCutscenesAddress) 	= 0x48000030; // b 0x30
	
	*reinterpret_cast<uint32_t *>(msgWindowMrAddress) 						= 0x38830001; // addi r4,r3,1
	
	*reinterpret_cast<float *>(BacktraceScreenFontSizeAddress) 				= 0.66;
	
	#ifndef TTYD_JP
	// This part of the backtrace screen does not need to be modified in JP
	*reinterpret_cast<uint32_t *>(BacktraceScreenPPCHaltBranchAddress) 		= 0x3B400000; // li r26,0
	*reinterpret_cast<uint32_t *>(BacktraceScreenEndBranchAddress) 			= 0x4BFFFDD4; // b -0x22C
	#endif
	
	// Set the initial value for the debug mode variable, to allow backtrace screens before the title sequence begins
	*reinterpret_cast<int32_t *>(
		reinterpret_cast<uint32_t>(
			ttyd::seq_title::seqTitleWorkPointer2) + 0x30) = -1;
}

void Mod::run()
{
	// Display the title screen and file select screen info
	displayTitleScreenAndFileSelectScreenInfo();
	
	// Update the On-Screen Timer variables if the display is on
	updateOnScreenTimerVars();
	
	// Check if the menu should be enabled or disabled
	// Prevent checking it if currently in the process of spawning an item
	bool CurrentlySpawningItem = SpawnItem.InAdjustableValueMenu;
	if (!CurrentlySpawningItem)
	{
		enableOrDisableMenu();
	}
	
	// Run each cheat function
	// Only run button-based codes if currently not changing button combos
	bool tempChangingCheatButtonCombo = MenuVar.ChangingCheatButtonCombo;
	if (!tempChangingCheatButtonCombo)
	{
		saveMarioAndPartnerPositions();
		loadMarioAndPartnerPositions();
		setTextStorage();
		setTimeStopTextStorage();
		lockMarioHPToMax();
		levitate();
	}
	
	// Run each cheat function that isn't button-based
	walkThroughMostObjects(); // Needs to always run due to a variable constantly being checked
	saveAnywhere(); // Needs to always run due to the script taking more than one frame
	speedUpMario(); // Needs to always run due to Mario's base speed constantly being set
	reloadRoom(); // Needs to always run due to the extra code that always does some failsafe checking
	bobberyEarly();
	checkIfAreaFlagsShouldBeCleared();
	checkIfSystemLevelShouldBeLowered();
	lockFlags();
	
	// Run each display function
	// Only run if the mod menu isn't currently open
	// Run if currently repositioning watches
	if (!MenuVar.MenuIsDisplayed || MenuVar.HideMenu)
	{
		// Don't display some displays if the Guard/Superguard timings display is active
		// Don't display some displays if current in the process of spawning an item
		if ((DisplayActionCommands.DisplayTimer == 0) && !CurrentlySpawningItem)
		{
			// Only run button-based displays if currently not changing button combos
			if (!tempChangingCheatButtonCombo)
			{
				displayYoshiSkipDetails();
				displayPalaceSkipDetails();
				displayBlimpTicketSkipDetails();
			}
			
			// Run each display function that isn't button-based
			displayMarioCoordinatesBoolCheck();
			displayMarioSpeedXZ();
			displayStickAngle();
			displayMemoryWatches();
		}
		
		// Only run button-based displays if currently not changing button combos
		if (!tempChangingCheatButtonCombo)
		{
			// Don't display some displays if current in the process of spawning an item
			if (!CurrentlySpawningItem)
			{
				displayOnScreenTimer();
				displayFrameCounter();
				displayJumpStorageDetails();
			}
			
			displayButtonInputs();
		}
		
		// Run each display function that isn't button-based
		spawnItem(); // Needs to always run due to the adjustable value menu sometimes being displayed
		displaySequenceInPauseMenu();
		displayActionCommandsTiming();
		
		// Check the heaps
		checkHeaps();
	}
	
	uint32_t tempFrameCounter = MenuVar.FrameCounter;
	if (tempFrameCounter > 0)
	{
		MenuVar.FrameCounter = tempFrameCounter - 1;
	}
	
	// Call original function
	mPFN_marioStMain_trampoline();
}

}