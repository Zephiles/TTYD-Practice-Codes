#include "mod.h"
#include "menu.h"
#include "commonfunctions.h"
#include "codes.h"
#include "items.h"
#include "global.h"
#include "draw.h"
#include "patch.h"

#include <gc/os.h>
#include <ttyd/swdrv.h>
#include <ttyd/mario.h>
#include <ttyd/evt_sub.h>
#include <ttyd/mario_pouch.h>
#include <ttyd/OSCache.h>
#include <ttyd/sac_scissor.h>
#include <ttyd/string.h>
#include <ttyd/memory.h>

#include <cstdio>
#include <cinttypes>

namespace mod {

// Assembly overwrite functions
extern "C"
{
	void StartPauseMenuPreventUnpause();
	void BranchBackPauseMenuPreventUnpause();
	void StartPreventPreBattleSoftlock();
	void BranchBackPreventPreBattleSoftlock();
	void StartDisableBattles();
	void BranchBackDisableBattles();
	void StartInfiniteItemUsage();
	void BranchBackInfiniteItemUsage();
	void StartReplaceJumpFallAnim();
	void BranchBackReplaceJumpFallAnim();
	void StartAllowRunningFromBattles();
	void BranchBackAllowRunningFromBattles();
	void StartForceNPCItemDrop();
	void BranchBackForceNPCItemDrop();
	void StartDisplayMegaHammerBadgesBattleMenu();
	void BranchBackDisplayMegaHammerBadgesBattleMenu();
	void StartDisplayMegaJumpBadgeBattleMenu();
	void BranchBackDisplayMegaJumpBadgeBattleMenu();
	void StartFixBlooperCrash1();
	void BranchBackFixBlooperCrash1();
	void StartFixBlooperCrash2();
	void BranchBackFixBlooperCrash2();
	void StartFixMarioKeyOn();
	void BranchBackFixMarioKeyOn();
	void StartArtAttackHitboxes();
	void BranchArtAttackHitboxes();
	void StartPreventTextboxSelection();
	void BranchBackPreventTextboxSelection();
	void StartPreventJumpAndHammer();
	void BranchBackPreventJumpAndHammer();
	void StartFixRoomProblems();
	void BranchBackFixRoomProblems();
}

// Functions accessed by assembly overwrites
extern "C" {
void *preventPreBattleSoftlock(void *fbatPointer)
{
	uint16_t ReloadRoomCombo 	= PAD_L | PAD_B;
	uint16_t OpenMenuCombo 		= PAD_L | PAD_START;
	
	// Prevent entering a non-cutscene battle if either reloading the room or opening the menu
	if (checkButtonComboEveryFrame(ReloadRoomCombo) || 
		checkButtonComboEveryFrame(OpenMenuCombo))
	{
		return nullptr;
	}
	else
	{
		return fbatPointer;
	}
}

bool displayMegaJumpBadgeInMenu(uint32_t checkBit)
{
	if (checkIfBadgeEquipped(UpgradedPowerJump))
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
	if (checkIfBadgeEquipped(UpgradedPowerSmash) || 
		checkIfBadgeEquipped(UpgradedQuakeHammer))
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

int32_t fixMarioKeyOn(int32_t currentKeyValue)
{
	// Properly convert key to int8_t
	int8_t NewKeyValue = static_cast<int8_t>(currentKeyValue);
	
	// Prevent key from becoming negative
	if (NewKeyValue < 1)
	{
		return 0;
	}
	else
	{
		return NewKeyValue - 1;
	}
}

void preventTextboxOptionSelection(char *currentText, void *storeAddress, 
	int32_t selectedOption)
{
	int32_t NewOption;
	// const int32_t FirstOption = 0;
	const int32_t SecondOption = 1;
	const int32_t ThirdOption = 2;
	
	if (compareStrings(currentText, "stg6_rsh_diary_01_yn"))
	{
		// Prevent the first option from being selected, so that the game does not crash when reading the diary
		// Only needs to run when not on the train
		if (!compareStrings(NextArea, "rsh"))
		{
			if (selectedOption == ThirdOption)
			{
				setSpecificSeq(ttyd::seqdrv::SeqIndex::kGameOver, nullptr, nullptr);
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

void fixRoomProblems()
{
	// Prevent the game from crashing if the player entered las_08 with the Sequence as 385 and GSW(1121) at 7
	if (compareStringToNextMap("las_08"))
	{
		// Check if the Sequence is currently at 385
		uint32_t SequencePosition = getSequencePosition();
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
}

const char *replaceJumpFallAnim(char *jumpFallString)
{
	if (compareStringsSize(jumpFallString, "M_J_", 4))
	{
		if (compareStringToNextMap("gor_01"))
		{
			// Return an arbitrary string
			const char *newString = "gor_01";
			
			if (MarioFreeze)
			{
				return newString;
			}
			else
			{
				ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
				int32_t PosX = static_cast<int32_t>(player->playerPosition[0]);
				int32_t PosZ = static_cast<int32_t>(player->playerPosition[2]);
				
				int32_t args[9];
				clearMemory(args, sizeof(args));
				
				args[0] = PosX;
				args[1] = PosZ;
				args[2] = PosX + 5;
				args[3] = PosZ + 5;
				args[4] = 10;
				args[5] = 250;
				args[6] = -10;
				args[7] = 267;
				
				*reinterpret_cast<uint32_t *>(&args[8]) = 0xFE363C80;
				
				char fakeContext[0x1B0];
				clearMemory(fakeContext, sizeof(fakeContext));
				
				*reinterpret_cast<int32_t **>(fakeContext + 0x18) = args;
				
				ttyd::evt_sub::evt_sub_check_intersect(fakeContext);
				
				if (*reinterpret_cast<bool *>(
					reinterpret_cast<uint32_t>(fakeContext) + 0x9F))
				{
					if (player->playerPosition[1] >= 58)
					{
						MarioFreeze = true;
						return newString;
					}
				}
			}
		}
	}
	return jumpFallString;
}
}

void displayTitleScreenAndFileSelectScreenInfo()
{
	if (checkForSpecificSeq(ttyd::seqdrv::SeqIndex::kTitle))
	{
		// Check to see if the title screen is ready
		// Check to see if the curtain is fully displayed or not
		uint32_t TitleMainCheck = *reinterpret_cast<uint32_t *>(
			*reinterpret_cast<uint32_t *>(titleMainAddress) + 0x8);
		
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
		uint32_t SeqMainCheck = *reinterpret_cast<uint32_t *>(seqMainAddress + 0x4);
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
	uint16_t OpenMenuCombo 	= PAD_L | PAD_START;
	
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
		ttyd::OSCache::DCFlushRange(AddressesToClearStart, 0x21);
	}
	
	// Clear the cache for the partners if their stats were changed manually
	if (ClearCacheForBattles.PartnerStatsShouldBeCleared)
	{
		ClearCacheForBattles.PartnerStatsShouldBeCleared = false;
		
		uint32_t PouchPtr = reinterpret_cast<uint32_t>(ttyd::mario_pouch::pouchGetPtr());
		void *AddressesToClearStart = reinterpret_cast<void *>(PouchPtr + 0xE);
		ttyd::OSCache::DCFlushRange(AddressesToClearStart, 0x61);
	}
	
	// Call original function
	return mPFN_battle_init_trampoline();
}

const char *Mod::getCustomMessage(const char *msgKey)
{
	// char *tempMsgKey = const_cast<char *>(msgKey);
	char *tempNextArea = NextArea;
	
	if (compareStrings(msgKey, "stg6_rsh_diary_01"))
	{
		// Change the text asking if you want to read the diary
		// Only needs to change when not on the train
		if (!compareStrings(tempNextArea, "rsh"))
		{
			const char *message = "<system>\n@ me if you think this mod\nsucks, but <shake><col EC2013FF>don't</col></shake><col DCC8DCFF> mention\nwhy you're @ing me.</col>\n<o>";
			return message;
		}
	}
	else if (compareStrings(msgKey, "stg6_rsh_diary_01_yn"))
	{
		// Change the yes/no text answers for the diary
		// Only needs to change when not on the train
		if (!compareStrings(tempNextArea, "rsh"))
		{
			#ifndef TTYD_JP
			const char *message = "<select 0 0 0 40>No\nAlright\nChoose this option";
			#else
			const char *message = "<select 0 0 300 40>No\nAlright\nChoose this option";
			#endif
			
			return message;
		}
	}

	// Call original function
	return mPFN_msgSearch_trampoline(msgKey);
}

uint32_t Mod::pauseArtAttackTimer()
{
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

void clearHeapArray(const char **heap)
{
	uint32_t Counter = 0;
	while (heap[Counter])
	{
		delete[] heap[Counter];
		heap[Counter] = nullptr;
		Counter++;
	}
}

void addTextToHeapArray(const char **heap, char *text)
{
	// Get the next available slot in the array
	uint32_t TotalSlots = 16;
	int32_t FreeSlot = -1;
	
	for (uint32_t i = 0; i < TotalSlots; i++)
	{
		if (!heap[i])
		{
			FreeSlot = i;
			break;
		}
	}
	
	// Make sure the new text can be added
	if (FreeSlot == -1)
	{
		// The new text cannot be added
		return;
	}
	
	// Add the new text at the current free slot
	uint32_t TextSize = ttyd::string::strlen(text);
	char *NewText = new char[TextSize + 1];
	ttyd::string::strcpy(NewText, text);
	heap[FreeSlot] = NewText;
}

bool checkIfTextAlreadyAdded(const char **heap, const char *text, uint32_t bytesToCompare)
{
	uint32_t Counter = 0;
	while (heap[Counter])
	{
		if (compareStringsSize(heap[Counter], text, bytesToCompare))
		{
			return true;
		}
		Counter++;
	}
	return false;
}

void checkHeaps()
{
	const char **tempStandardHeapArray = CheckHeap.StandardHeapArray;
	const char **tempSmartHeapArray = CheckHeap.SmartHeapArray;
	char *tempDisplayBuffer = DisplayBuffer;
	
	// Check the standard heaps
	bool ErrorsFound = false;
	for (int32_t i = 0; i < 5; i++)
	{
		const gc::os::HeapInfo &heap = gc::os::OSAlloc_HeapArray[i];
		bool valid = true;
		
		gc::os::ChunkInfo *currentChunk = nullptr;
		gc::os::ChunkInfo *prevChunk = nullptr;
		for (currentChunk = heap.firstUsed; currentChunk; currentChunk = currentChunk->next)
		{
			// Check pointer sanity
			if (!checkIfPointerIsValid(currentChunk))
			{
				ErrorsFound = true;
				valid = false;
				break;
			}
			
			// Sanity check size
			if (currentChunk->size > 0x17FFFFF)
			{
				ErrorsFound = true;
				valid = false;
				break;
			}

			// Check linked list integrity
			if (prevChunk != currentChunk->prev)
			{
				ErrorsFound = true;
				valid = false;
				break;
			}

			prevChunk = currentChunk;
		}
		
		if (!valid)
		{
			sprintf(tempDisplayBuffer,
				"Standard Heap %" PRId32 " corrupted at 0x%08" PRIX32,
				i,
				reinterpret_cast<uint32_t>(currentChunk));
			
			// Only add the current heap once
			if (!checkIfTextAlreadyAdded(tempStandardHeapArray, tempDisplayBuffer, 16))
			{	
				addTextToHeapArray(tempStandardHeapArray, tempDisplayBuffer);
			}
		}
	}
	
	if (!ErrorsFound)
	{
		// No errors were found, so clear the standard heap array
		clearHeapArray(tempStandardHeapArray);
	}
	
	// Check the smart heap
	ttyd::memory::SmartWork *gpSmartWork = *reinterpret_cast<ttyd::memory::SmartWork **>(SmartWorkAddress);
	bool valid = true;
	
	ttyd::memory::SmartAllocationData *currentChunk = nullptr;
	ttyd::memory::SmartAllocationData *prevChunk = nullptr;
	for (currentChunk = gpSmartWork->pFirstUsed; currentChunk; currentChunk = currentChunk->pNext)
	{
		// Check pointer sanity
		if (!checkIfPointerIsValid(currentChunk))
		{
			valid = false;
			break;
		}
		
		// Sanity check size
		if (currentChunk->usedSize > 0x17FFFFF)
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
			"Smart Heap corrupted at 0x%08" PRIX32,
			reinterpret_cast<uint32_t>(currentChunk));
		
		// Only add the current heap once
		if (!checkIfTextAlreadyAdded(tempSmartHeapArray, tempDisplayBuffer, 16))
		{	
			addTextToHeapArray(tempSmartHeapArray, tempDisplayBuffer);
		}
	}
	else
	{
		// No errors were found, so clear the smart heap array
		clearHeapArray(tempSmartHeapArray);
	}
	
	// Draw any errors that occured
	if (tempStandardHeapArray[0] || tempSmartHeapArray[0])
	{
		drawFunctionOnDebugLayer(drawHeapArrayErrors);
	}
}

void writeStandardBranch(void *address, void functionStart(), void functionBranchBack())
{
	void *tempFunctionStart 		= reinterpret_cast<void *>(functionStart);
	void *tempFunctionBranchBack 	= reinterpret_cast<void *>(functionBranchBack);
	void *branchBackAddress 		= reinterpret_cast<void *>(
		(reinterpret_cast<uint32_t>(address) + 0x4));
	
	patch::writeBranch(address, tempFunctionStart);
	patch::writeBranch(tempFunctionBranchBack, branchBackAddress);
}

void initAddressOverwrites()
{
	#ifdef TTYD_US
	void *PreventPreBattleSoftlockAddress 		= reinterpret_cast<void *>(0x800465CC);
	void *DisableBattlesAddress 				= reinterpret_cast<void *>(0x800448CC);
	void *AllowRunningFromBattles 				= reinterpret_cast<void *>(0x80123CA4);
	void *ForceNPCItemDropAddress 				= reinterpret_cast<void *>(0x8004EC10);
	void *DebugModeInitialzeAddress 			= reinterpret_cast<void *>(0x80009B2C);
	void *DebugModeShowBuildDateAddress 		= reinterpret_cast<void *>(0x80008FE8);
	void *PauseMenuPartnerMenuAddress 			= reinterpret_cast<void *>(0x801649A0);
	void *PauseMenuBadgeMenuAddress 			= reinterpret_cast<void *>(0x80164A44);
	void *ReplaceJumpAnimAddress 				= reinterpret_cast<void *>(0x800411D0);
	void *PreventImportantItemCutscenesAddress 	= reinterpret_cast<void *>(0x800ABCD8);
	void *msgWindowMrAddress 					= reinterpret_cast<void *>(0x800816F4);
	void *DisplayBattleMenuHammerAddress 		= reinterpret_cast<void *>(0x80122BB8);
	void *DisplayBattleMenuJumpAddress 			= reinterpret_cast<void *>(0x80122BA4);
	void *FixBlooperCrash1Address 				= reinterpret_cast<void *>(0x8010F810);
	void *FixBlooperCrash2Address 				= reinterpret_cast<void *>(0x8010F888);
	void *FixMarioKeyOnAddress 					= reinterpret_cast<void *>(0x8005C1C8);
	void *PreventTextboxSelectionAddress 		= reinterpret_cast<void *>(0x800D214C);
	void *BacktraceScreenFontSizeAddress 		= reinterpret_cast<void *>(0x80428BC0);
	void *BacktraceScreenPPCHaltBranchAddress 	= reinterpret_cast<void *>(0x8025E4A4);
	void *BacktraceScreenEndBranchAddress 		= reinterpret_cast<void *>(0x8025E4A8);
	void *FixRoomProblemsAddress 				= reinterpret_cast<void *>(0x800087C8);
	void *ArtAttackHitboxesAddress 				= reinterpret_cast<void *>(0x80231928);
	#elif defined TTYD_JP
	void *PreventPreBattleSoftlockAddress 		= reinterpret_cast<void *>(0x80045F28);
	void *DisableBattlesAddress 				= reinterpret_cast<void *>(0x80044228);
	void *AllowRunningFromBattles 				= reinterpret_cast<void *>(0x8011E7DC);
	void *ForceNPCItemDropAddress 				= reinterpret_cast<void *>(0x8004DFB0);
	void *DebugModeInitialzeAddress 			= reinterpret_cast<void *>(0x8000999C);
	void *DebugModeShowBuildDateAddress 		= reinterpret_cast<void *>(0x80008EB8);
	void *PauseMenuPartnerMenuAddress 			= reinterpret_cast<void *>(0x8015EFBC);
	void *PauseMenuBadgeMenuAddress 			= reinterpret_cast<void *>(0x8015F060);
	void *ReplaceJumpAnimAddress 				= reinterpret_cast<void *>(0x80040B34);
	void *PreventImportantItemCutscenesAddress 	= reinterpret_cast<void *>(0x800AA01C);
	void *msgWindowMrAddress 					= reinterpret_cast<void *>(0x80080B6C);
	void *DisplayBattleMenuHammerAddress 		= reinterpret_cast<void *>(0x8011D6F0);
	void *DisplayBattleMenuJumpAddress 			= reinterpret_cast<void *>(0x8011D6DC);
	void *FixBlooperCrash1Address 				= reinterpret_cast<void *>(0x8010A724);
	void *FixBlooperCrash2Address 				= reinterpret_cast<void *>(0x8010A79C);
	void *FixMarioKeyOnAddress 					= reinterpret_cast<void *>(0x8005B370);
	void *PreventTextboxSelectionAddress 		= reinterpret_cast<void *>(0x800CE01C);
	void *BacktraceScreenFontSizeAddress 		= reinterpret_cast<void *>(0x80422618);
	void *FixRoomProblemsAddress 				= reinterpret_cast<void *>(0x800086F0);
	void *ArtAttackHitboxesAddress 				= reinterpret_cast<void *>(0x8022C278);
	#elif defined TTYD_EU
	void *PreventPreBattleSoftlockAddress 		= reinterpret_cast<void *>(0x800466B4);
	void *DisableBattlesAddress 				= reinterpret_cast<void *>(0x800449B4);
	void *AllowRunningFromBattles 				= reinterpret_cast<void *>(0x80124BE4);
	void *ForceNPCItemDropAddress 				= reinterpret_cast<void *>(0x8004ECDC);
	void *DebugModeInitialzeAddress 			= reinterpret_cast<void *>(0x80009CF0);
	void *DebugModeShowBuildDateAddress 		= reinterpret_cast<void *>(0x800091B4);
	void *PauseMenuPartnerMenuAddress 			= reinterpret_cast<void *>(0x80166490);
	void *PauseMenuBadgeMenuAddress 			= reinterpret_cast<void *>(0x80166534);
	void *ReplaceJumpAnimAddress 				= reinterpret_cast<void *>(0x800412B8);
	void *PreventImportantItemCutscenesAddress 	= reinterpret_cast<void *>(0x800AD0A8);
	void *msgWindowMrAddress 					= reinterpret_cast<void *>(0x800829b0);
	void *DisplayBattleMenuHammerAddress 		= reinterpret_cast<void *>(0x80123AF8);
	void *DisplayBattleMenuJumpAddress 			= reinterpret_cast<void *>(0x80123AE4);
	void *FixBlooperCrash1Address 				= reinterpret_cast<void *>(0x801106E8);
	void *FixBlooperCrash2Address 				= reinterpret_cast<void *>(0x80110760);
	void *FixMarioKeyOnAddress 					= reinterpret_cast<void *>(0x8005C300);
	void *PreventTextboxSelectionAddress 		= reinterpret_cast<void *>(0x800D2F44);
	void *BacktraceScreenFontSizeAddress 		= reinterpret_cast<void *>(0x804356C8);
	void *BacktraceScreenPPCHaltBranchAddress 	= reinterpret_cast<void *>(0x8026207C);
	void *BacktraceScreenEndBranchAddress 		= reinterpret_cast<void *>(0x80262080);
	void *FixRoomProblemsAddress 				= reinterpret_cast<void *>(0x80008994);
	void *ArtAttackHitboxesAddress 				= reinterpret_cast<void *>(0x802353B8);
	#endif
	
	writeStandardBranch(PreventPreBattleSoftlockAddress, 
		StartPreventPreBattleSoftlock, BranchBackPreventPreBattleSoftlock);
	
	writeStandardBranch(DisableBattlesAddress, 
		StartDisableBattles, BranchBackDisableBattles);
	
	writeStandardBranch(AllowRunningFromBattles, 
		StartAllowRunningFromBattles, BranchBackAllowRunningFromBattles);
	
	writeStandardBranch(ForceNPCItemDropAddress, 
		StartForceNPCItemDrop, BranchBackForceNPCItemDrop);
	
	writeStandardBranch(ReplaceJumpAnimAddress, 
		StartReplaceJumpFallAnim, BranchBackReplaceJumpFallAnim);
	
	writeStandardBranch(DisplayBattleMenuHammerAddress, 
		StartDisplayMegaHammerBadgesBattleMenu, BranchBackDisplayMegaHammerBadgesBattleMenu);
	
	writeStandardBranch(DisplayBattleMenuJumpAddress, 
		StartDisplayMegaJumpBadgeBattleMenu, BranchBackDisplayMegaJumpBadgeBattleMenu);
	
	writeStandardBranch(FixBlooperCrash1Address, 
		StartFixBlooperCrash1, BranchBackFixBlooperCrash1);
	
	writeStandardBranch(FixBlooperCrash2Address, 
		StartFixBlooperCrash2, BranchBackFixBlooperCrash2);
	
	writeStandardBranch(FixMarioKeyOnAddress, 
		StartFixMarioKeyOn, BranchBackFixMarioKeyOn);
	
	writeStandardBranch(PreventTextboxSelectionAddress, 
		StartPreventTextboxSelection, BranchBackPreventTextboxSelection);
	
	writeStandardBranch(FixRoomProblemsAddress, 
		StartFixRoomProblems, BranchBackFixRoomProblems);
	
	writeStandardBranch(ArtAttackHitboxesAddress, 
		StartArtAttackHitboxes, BranchArtAttackHitboxes);
	
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
	uint32_t tempTitleMainAddress = *reinterpret_cast<uint32_t *>(titleMainAddress);
	if (tempTitleMainAddress != 0)
	{
		*reinterpret_cast<int32_t *>(tempTitleMainAddress + 0x30) = -1;
	}
}

void Mod::run()
{
	// Display the title screen and file select screen info
	displayTitleScreenAndFileSelectScreenInfo();
	
	// Check if the menu should be enabled or disabled
	// Prevent checking it if currently in the process of spawning an item
	if (!SpawnItem.InAdjustableValueMenu)
	{
		enableOrDisableMenu();
	}
	
	// Run each cheat function
	// Only run button-based codes if currently not changing button combos
	bool tempChangingCheatButtonCombo = ChangingCheatButtonCombo;
	if (!tempChangingCheatButtonCombo)
	{
		walkThroughMostObjects();
		saveMarioAndPartnerPositions();
		loadMarioAndPartnerPositions();
		setTextStorage();
		setTimeStopTextStorage();
		lockMarioHPToMax();
		levitate();
	}
	
	// Run each cheat function that isn't button-based
	saveAnywhere(); // Needs to always run due to the script taking more than one frame
	speedUpMario(); // Needs to always run due to Mario's base speed constantly being set
	reloadRoom(); // Needs to always run due to the extra code that always does some failsafe checking
	bobberyEarly();
	checkIfAreaFlagsShouldBeCleared();
	checkIfSystemLevelShouldBeLowered();
	
	// Run each display function
	// Only run if the mod menu isn't currently open
	// Run if currently repositioning watches
	if (!MenuIsDisplayed || HideMenu)
	{
		// Only run button-based codes if currently not changing button combos
		if (!tempChangingCheatButtonCombo)
		{
			displayOnScreenTimer();
			displayJumpStorageDetails();
			displayButtonInputs();
			displayYoshiSkipDetails();
			displayPalaceSkipDetails();
		}
		
		// Run each display function that isn't button-based
		spawnItem(); // Needs to always run due to the adjustable value menu sometimes being displayed
		displaySequenceInPauseMenu();
		displayMarioCoordinatesBoolCheck();
		displayActionCommandsTiming();
		displayStickAngle();
		displayMemoryWatches();
		
		// Check the heaps
		checkHeaps();
	}
	
	uint32_t tempFrameCounter = FrameCounter;
	if (tempFrameCounter > 0)
	{
		FrameCounter = tempFrameCounter - 1;
	}
	
	// Call original function
	mPFN_makeKey_trampoline();
}

}