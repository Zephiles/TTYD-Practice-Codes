#include "menufunctions.h"
#include "global.h"
#include "commonfunctions.h"
#include "codes.h"
#include "memorywatch.h"

#include <gc/os.h>
#include <gc/OSTime.h>
#include <ttyd/win_main.h>
#include <ttyd/mariost.h>
#include <ttyd/win_item.h>
#include <ttyd/win_party.h>
#include <ttyd/seqdrv.h>
#include <ttyd/item_data.h>
#include <ttyd/seq_mapchange.h>
#include <ttyd/msgdrv.h>
#include <ttyd/mario_party.h>
#include <ttyd/party.h>
#include <ttyd/mario_pouch.h>
#include <ttyd/mario.h>
#include <ttyd/swdrv.h>
#include <ttyd/evt_yuugijou.h>
#include <ttyd/battle.h>
#include <ttyd/fadedrv.h>
#include <ttyd/evtmgr.h>
#include <ttyd/system.h>
#include <ttyd/mapdata.h>
#include <ttyd/event.h>
#include <ttyd/mario_motion.h>
#include <ttyd/evt_pouch.h>

#include <cstdio>
#include <cstring>
#include <cinttypes>
#include <cmath>

namespace mod {

void closeMenu()
{
	MenuVar.MenuIsDisplayed = false;
	
	resetMenuToRoot();
	
	// Lower the System Level if not in a battle
	lowerSystemLevel();
	
	// Enable the Pause Menu
	ttyd::win_main::winOpenEnable();
}

void closeSecondaryMenu()
{
	// Reset the current menu option
	MenuVar.CurrentMenuOption = MenuVar.SelectedOption - 1;
	
	// Close the menu
	MenuVar.SelectedOption = 0;
	
	// Reset the function return code
	MenuVar.FunctionReturnCode = 0;
}

void enterNextMenu(uint32_t nextMenu, uint32_t currentMenuOption)
{
	uint32_t i = PrevMenuAndOption.CurrentIndex;
	PrevMenuAndOption.PreviousPage[i] = MenuVar.CurrentPage;
	PrevMenuAndOption.PreviousMenu[i] = MenuVar.CurrentMenu;
	PrevMenuAndOption.PreviousMenuOption[i] = currentMenuOption;
	
	MenuVar.CurrentMenu = nextMenu;
	
	uint32_t Size = sizeof(PrevMenuAndOption.PreviousPage) / 
		sizeof(PrevMenuAndOption.PreviousPage[0]);
	
	if (i < (Size - 1))
	{
		PrevMenuAndOption.CurrentIndex = ++i;
	}
}

void enterPreviousMenu()
{
	uint32_t i = PrevMenuAndOption.CurrentIndex;
	
	if (i > 0)
	{
		i--;
		PrevMenuAndOption.CurrentIndex = i;
	}
	
	MenuVar.CurrentPage = PrevMenuAndOption.PreviousPage[i];
	MenuVar.CurrentMenu = PrevMenuAndOption.PreviousMenu[i];
	MenuVar.CurrentMenuOption = PrevMenuAndOption.PreviousMenuOption[i];
}

void resetMenu()
{
	MenuVar.CurrentPage 			= 0;
	MenuVar.CurrentMenuOption 		= 0;
	MenuVar.SecondaryMenuOption 	= 0;
	MenuVar.SelectedOption 			= 0;
	MenuVar.SecondaryPage 			= 0;
	MenuVar.Timer 					= 0;
	MenuVar.MenuSelectionStates 	= 0;
	MenuVar.MenuSecondaryValue 		= 0;
}

void resetMenuToRoot()
{
	MenuVar.CurrentMenu 		= ROOT;
	MenuVar.MenuSelectedOption 	= 0;
	MenuVar.FunctionReturnCode 	= 0;
	MenuVar.HideMenu 			= false;
	resetMenu();
	
	MenuVar.CurrentMenuOption = PrevMenuAndOption.PreviousMenuOption[0];
	clearMemory(&PrevMenuAndOption, sizeof(PrevMenuAndOption));
}

void resetAndCloseSecondaryMenu()
{
	// Reset the value used by the secondary menus
	MenuVar.MenuSecondaryValue = 0;
	
	// Reset the timer
	MenuVar.Timer = 0;
	
	// Reset the current menu option and then close the menu
	closeSecondaryMenu();
}

void resetImportantItemsPauseMenu()
{
	// Only run for the important items menu
	if (MenuVar.MenuSelectedOption != INVENTORY_IMPORTANT)
	{
		// Not modifying important items, so do nothing
		return;
	}
	
	// Only run if the pause menu is currently open
	uint32_t SystemLevel = ttyd::mariost::marioStGetSystemLevel();
	if (SystemLevel != 15)
	{
		// The pause menu is not open, so do nothing
		return;
	}
	
	void *PauseMenuPointer = ttyd::win_main::winGetPtr();
	
	// Get the current submenu that is open in the items menu
	uint32_t ImportantItemsSubMenu = *reinterpret_cast<uint32_t *>(
		reinterpret_cast<uint32_t>(PauseMenuPointer) + 0x210);
	
	// Clear the item menu
	ttyd::win_item::winItemExit(PauseMenuPointer);
	
	// Re-init the item menu
	ttyd::win_item::winItemInit(PauseMenuPointer);
	
	// Restore the submenu
	*reinterpret_cast<uint32_t *>(
		reinterpret_cast<uint32_t>(PauseMenuPointer) + 
			0x210) = ImportantItemsSubMenu;
}

void resetPartnerPauseMenu()
{
	// Only run if the pause menu is currently open
	uint32_t SystemLevel = ttyd::mariost::marioStGetSystemLevel();
	if (SystemLevel != 15)
	{
		// The pause menu is not open, so do nothing
		return;
	}
	
	void *PauseMenuPointer = ttyd::win_main::winGetPtr();
	
	// Clear the party menu
	ttyd::win_party::winPartyExit(PauseMenuPointer);
	
	// Re-init the party menu
	ttyd::win_party::winPartyInit(PauseMenuPointer);
}

void recheckUpgradesBattles(int32_t item)
{
	// Only run while in a battle
	if (!checkForSpecificSeq(ttyd::seqdrv::SeqIndex::kBattle))
	{
		return;
	}
	
	// Only run if the current item is a boot or hammer upgrade
	if ((item >= ttyd::item_data::Item::Boots) && 
		(item <= ttyd::item_data::Item::UltraHammer))
	{
		// Call the functions for checking which upgrades the player has
		recheckJumpAndHammerLevels();
	}
}

void raiseSystemLevel()
{
	// Raise the System Level if not in a battle
	if (checkIfInGame())
	{
		uint32_t SystemLevel = ttyd::mariost::marioStGetSystemLevel();
		
		// Increase the System Level by 1
		ttyd::mariost::marioStSystemLevel(SystemLevel + 1);
	}
}

void lowerSystemLevel()
{
	// Lower the System Level if not in a battle
	if (checkIfInGame())
	{
		uint32_t SystemLevel = ttyd::mariost::marioStGetSystemLevel();
		if (SystemLevel > 0)
		{
			// Decrease the System Level by 1
			// Only decrease if not already at 0
			ttyd::mariost::marioStSystemLevel(SystemLevel - 1);
		}
	}
}

void partnerMenuRemoveOrBringOut(void *partnerEnabledAddress)
{
	if (checkIfPartnerOutSelected())
	{
		// Draw the option to remove the partner from the overworld
		removePartnerFromOverworld();
	}
	else
	{
		// Draw the option to bring a partner out
		// Make sure a file is loaded
		if (checkIfInGame())
		{
			uint32_t PartnerEnabledAddress = reinterpret_cast<uint32_t>(partnerEnabledAddress);
			
			// Preserve the current value of the enabled bool
			bool CurrentPartnerEnabled = *reinterpret_cast<bool *>(
				PartnerEnabledAddress + 1);
			
			// Make sure the selected partner is enabled, as marioPartyEntry needs it to be enabled
			*reinterpret_cast<bool *>(
				PartnerEnabledAddress + 1) = true;
			
			// Bring the partner out
			spawnPartnerOrFollower(getSelectedOptionPartnerValue());
			
			// Restore the value of the enabled bool
			*reinterpret_cast<bool *>(
				PartnerEnabledAddress + 1) = CurrentPartnerEnabled;
			
			MenuVar.Timer = 0;
		}
		else
		{
			MenuVar.FunctionReturnCode 	= NOT_IN_GAME;
			MenuVar.Timer 				= secondsToFrames(3);
		}
	}
}

void lockFlagsMenuBackUpFlags(uint32_t index)
{
	// Get the region to work with
	LockFlagsRegion *Region = &LockFlags.Region[index];
	
	// Flip the bool
	bool FlagsLocked = !Region->MemoryRegionLocked;
	Region->MemoryRegionLocked = FlagsLocked;
	
	uint8_t *tempMemory = Region->MemoryRegion;
	if (FlagsLocked)
	{
		// Allocate memory for the flags if memory is not allocated already
		uint32_t Size = Region->Size;
		if (!tempMemory)
		{
			tempMemory = new uint8_t[Size];
			Region->MemoryRegion = tempMemory;
		}
		
		// Back up the memory
		if ((index == GW) || (index == GF))
		{
			// Back up the GWs or GFs
			ttyd::evtmgr::EvtWork *EventWork = ttyd::evtmgr::evtGetWork();
			void *SourceMemoryStart;
			
			if (index == GW)
			{
				// Back up the GWs
				SourceMemoryStart = EventWork->gwData;
			}
			else // index == GF
			{
				// Back up the GFs
				SourceMemoryStart = EventWork->gfData;
			}
			
			memcpy(&tempMemory[0], SourceMemoryStart, Size);
		}
		else
		{
			// Back up the standard flags
			uint32_t GlobalWorkPtrRaw = reinterpret_cast<uint32_t>(ttyd::mariost::globalWorkPointer);
			void *MemoryStart = reinterpret_cast<void *>(GlobalWorkPtrRaw + Region->Offset);
			memcpy(&tempMemory[0], MemoryStart, Size);
		}
		
		// Do other stuff depending on the index
		switch (index)
		{
			case GSW:
			{
				// Back up the Sequence Position
				LockFlags.SequencePosition = static_cast<uint16_t>(getSequencePosition());
				break;
			}
			case LSW:
			case LSWF:
			{
				// Update the area for the flags to be locked
				char *Area = lockFlagsMenuGetAreaLockedString(index);
				strcpy(Area, ttyd::seq_mapchange::NextArea);
				break;
			}
			default:
			{
				break;
			}
		}
	}
	else if (tempMemory)
	{
		// Clear the memory for the flags if memory is currently allocated for them
		delete[] (tempMemory);
		Region->MemoryRegion = nullptr;
	}
}

void lockFlagsMenuSetNewArea(uint32_t index)
{
	// Get the region to work with
	LockFlagsRegion *Region = &LockFlags.Region[index];
	
	// Only run if the bool is currently on
	bool Flag = Region->MemoryRegionLocked;
	uint8_t *tempMemory = Region->MemoryRegion;
	
	if (Flag && tempMemory)
	{
		// Back up the standard flags
		uint32_t GlobalWorkPtrRaw = reinterpret_cast<uint32_t>(ttyd::mariost::globalWorkPointer);
		void *MemoryStart = reinterpret_cast<void *>(GlobalWorkPtrRaw + Region->Offset);
		memcpy(&tempMemory[0], MemoryStart, Region->Size);
		
		// Update the area for the flags to be locked
		char *Area = lockFlagsMenuGetAreaLockedString(index);
		strcpy(Area, ttyd::seq_mapchange::NextArea);
	}
}

char *lockFlagsMenuGetAreaLockedString(uint32_t index)
{
	uint32_t AreaLockedIndex;
	if (index == LSW)
	{
		AreaLockedIndex = 0;
	}
	else
	{
		AreaLockedIndex = 1;
	}
	
	return LockFlags.AreaLocked[AreaLockedIndex];
}

const char *getItemName(int16_t item)
{
	ttyd::item_data::ItemData *ItemDb = ttyd::item_data::itemDataTable;
	return ttyd::msgdrv::msgSearch(ItemDb[item].itemNameMsg);
}

void *getFreeSlotPointer()
{
	uint32_t Address_and_Size[2];
	uint32_t *tempArray		= getPouchAddressAndSize(Address_and_Size);
	
	if (!tempArray)
	{
		return nullptr;
	}
	
	uint32_t tempAddress 	= tempArray[0];
	uint32_t tempSize 		= tempArray[1];
	
	// Make sure a slot is available
	bool FoundEmptySlot = false;
	for (uint32_t i = 0; i < tempSize; i++)
	{
		int16_t CurrentItem = *reinterpret_cast<int16_t *>(tempAddress);
		if (CurrentItem != 0)
		{
			// Adjust the address for the next item
			tempAddress += 0x2;
		}
		else
		{
			// Found an empty slot, so exit the loop
			FoundEmptySlot = true;
			break;
		}
	}
	
	// Check if theres an empty slot
	if (!FoundEmptySlot)
	{
		// No slot is available
		return nullptr;
	}
	else
	{
		// Return the address of the open slot
		return reinterpret_cast<void *>(tempAddress);
	}
}

int32_t getTotalItems()
{
	uint32_t Address_and_Size[2];
	uint32_t *tempArray		= getPouchAddressAndSize(Address_and_Size);
	
	if (!tempArray)
	{
		return NULL_PTR;
	}
	
	uint32_t tempAddress 	= tempArray[0];
	uint32_t tempSize 		= tempArray[1];
	
	uint32_t Counter = 0;
	for (uint32_t i = 0; i < tempSize; i++)
	{
		int16_t CurrentItem = *reinterpret_cast<int16_t *>(tempAddress + (i * 0x2));
		if (CurrentItem == 0)
		{
			return Counter;
		}
		Counter++;
	}
	
	return Counter;
}

void getUpperAndLowerBounds(int32_t arrayOut[2], uint32_t currentMenu)
{
	uint32_t tempMenuSelectedOption = MenuVar.MenuSelectedOption;
	uint32_t tempCurrentMenuOption = MenuVar.CurrentMenuOption;
	uint32_t tempMenuSelectionStates = MenuVar.MenuSelectionStates;
	int32_t LowerBound = 0;
	int32_t UpperBound = 0;
	
	switch (currentMenu)
	{
		case INVENTORY_STANDARD:
		case INVENTORY_STORED_ITEMS:
		{
			LowerBound = ttyd::item_data::Item::GoldBar;
			UpperBound = ttyd::item_data::Item::FreshJuice;
			break;
		}
		case INVENTORY_IMPORTANT:
		{
			LowerBound = ttyd::item_data::Item::StrangeSack;
			UpperBound = ttyd::item_data::Item::CrystalStar;
			break;
		}
		case INVENTORY_BADGES:
		{
			LowerBound = ttyd::item_data::Item::PowerJump;
			UpperBound = ttyd::item_data::Item::SuperChargeP;
			break;
		}
		case CHEATS_CHANGE_SEQUENCE:
		{
			// LowerBound = 0;
			UpperBound = 405;
			break;
		}
		case CHEATS_NPC_FORCE_DROP:
		{
			LowerBound = ttyd::item_data::Item::GoldBar;
			UpperBound = ttyd::item_data::Item::SuperChargeP;
			break;
		}
		case CHEATS_MANAGE_FLAGS_MAIN:
		{
			switch (tempMenuSelectionStates)
			{
				case SET_GSW:
				{
					if (tempCurrentMenuOption == CHANGE_GLOBAL_WORD)
					{
						// LowerBound = 0;
						UpperBound = 2048;
					}
					else if (ManageFlags.FlagToSet == 0) // 0 = Sequence position
					{
						// LowerBound = 0;
						UpperBound = 405;
					}
					else
					{
						// LowerBound = 0;
						UpperBound = 255;
					}
					break;
				}
				case SET_GSWF:
				{
					if (tempCurrentMenuOption == CHANGE_GLOBAL_FLAG)
					{
						// LowerBound = 0;
						UpperBound = 8192;
					}
					else
					{
						// LowerBound = 0;
						UpperBound = 1;
					}
					break;
				}
				case SET_GW:
				{
					if (tempCurrentMenuOption == CHANGE_GLOBAL_WORD)
					{
						// LowerBound = 0;
						UpperBound = 32;
					}
					else
					{
						LowerBound = -0x80000000;
						UpperBound = 0x7FFFFFFF;
					}
					break;
				}
				case SET_GF:
				{
					if (tempCurrentMenuOption == CHANGE_GLOBAL_FLAG)
					{
						// LowerBound = 0;
						UpperBound = 96;
					}
					else
					{
						// LowerBound = 0;
						UpperBound = 1;
					}
					break;
				}
				case SET_LSW:
				{
					if (tempCurrentMenuOption == CHANGE_GLOBAL_WORD)
					{
						// LowerBound = 0;
						UpperBound = 1024;
					}
					else
					{
						// LowerBound = 0;
						UpperBound = 255;
					}
					break;
				}
				case SET_LSWF:
				{
					if (tempCurrentMenuOption == CHANGE_GLOBAL_FLAG)
					{
						// LowerBound = 0;
						UpperBound = 512;
					}
					else
					{
						// LowerBound = 0;
						UpperBound = 1;
					}
					break;
				}
				default:
				{
					// LowerBound = 0;
					UpperBound = 0;
					break;
				}
			}
			break;
		}
		case STATS_MARIO:
		{
			uint32_t PouchPtr = reinterpret_cast<uint32_t>(ttyd::mario_pouch::pouchGetPtr());
			switch (tempMenuSelectionStates)
			{
				case COINS:
				case MARIO_MAX_HP:
				case MARIO_MAX_FP:
				case STAR_PIECES:
				case SHINE_SPRITES:
				{
					// LowerBound = 0;
					UpperBound = 999;
					break;
				}
				case MARIO_HP:
				{
					// LowerBound = 0;
					UpperBound = *reinterpret_cast<int16_t *>(PouchPtr + 0x72); // Max HP
					break;
				}
				case MARIO_FP:
				{
					// LowerBound = 0;
					UpperBound = *reinterpret_cast<int16_t *>(PouchPtr + 0x76); // Max FP
					break;
				}
				case BP:
				case MARIO_LEVEL:
				case STAR_POINTS:
				{
					// LowerBound = 0;
					UpperBound = 99;
					break;
				}
				case MARIO_RANK:
				{
					// LowerBound = 0;
					UpperBound = 3;
					break;
				}
				case STAR_POWER:
				{
					// LowerBound = 0;
					UpperBound = *reinterpret_cast<int16_t *>(PouchPtr + 0x7C); // Max Star Power
					break;
				}
				case MAX_STAR_POWER:
				{
					// LowerBound = 0;
					UpperBound = 800;
					break;
				}
				case SHOP_POINTS:
				{
					// LowerBound = 0;
					UpperBound = 300;
					break;
				}
				case PIANTAS_STORED:
				case CURRENT_PIANTAS:
				{
					// LowerBound = 0;
					UpperBound = 99999;
					break;
				}
				default:
				{
					// LowerBound = 0;
					UpperBound = 0;
					break;
				}
			}
			break;
		}
		case STATS_PARTNERS:
		{
			uint32_t PartnerEnabledAddress = reinterpret_cast<uint32_t>(getPartnerEnabledAddress());
			switch (tempMenuSelectionStates)
			{
				case PARTNER_HP:
				{
					// LowerBound = 0;
					UpperBound = *reinterpret_cast<int16_t *>(PartnerEnabledAddress + 0x2); // Max HP
					break;
				}
				case PARTNER_MAX_HP:
				{
					// LowerBound = 0;
					UpperBound = 999;
					break;
				}
				case PARTNER_RANK:
				{
					// LowerBound = 0;
					UpperBound = 2;
					break;
				}
				default:
				{
					// LowerBound = 0;
					UpperBound = 0;
					break;
				}
			}
			break;
		}
		case BATTLES_CURRENT_ACTOR:
		{
			uint32_t ActorAddress = reinterpret_cast<uint32_t>(getActorPointer(tempMenuSelectedOption));
			if (!ActorAddress)
			{
				break;
			}
			
			switch (tempMenuSelectionStates)
			{
				case CHANGE_ACTOR_HP:
				{
					// LowerBound = 0;
					UpperBound = *reinterpret_cast<int16_t *>(ActorAddress + 0x108); // Max HP
					break;
				}
				case CHANGE_ACTOR_FP:
				{
					// LowerBound = 0;
					UpperBound = *reinterpret_cast<int16_t *>(ActorAddress + 0x10E); // Max FP
					break;
				}
				case CHANGE_HELD_ITEM:
				{
					LowerBound = ttyd::item_data::Item::ThunderBolt;
					UpperBound = ttyd::item_data::Item::SuperChargeP;
					break;
				}
				default:
				{
					// LowerBound = 0;
					UpperBound = 999;
					break;
				}
			}
			break;
		}
		case BATTLES_STATUSES:
		{
			uint32_t ActorAddress = reinterpret_cast<uint32_t>(getActorPointer(tempMenuSelectedOption));
			if (!ActorAddress)
			{
				break;
			}
			
			switch (tempMenuSelectionStates)
			{
				case BIG_SHRINK_POWER_AMOUNT:
				case ATTACK_UP_DOWN_POWER_AMOUNT:
				case DEFENSE_UP_DOWN_POWER_AMOUNT:
				{
					LowerBound = -99;
					UpperBound = 99;
					break;
				}
				case DEFEATED_FLAG:
				{
					// LowerBound = 0;
					UpperBound = 1;
					break;
				}
				default:
				{
					// LowerBound = 0;
					UpperBound = 99;
					break;
				}
			}
			break;
		}
		case WARPS:
		{
			LowerBound = 1;
			UpperBound = 100;
			break;
		}
		case WARPS_EVENT:
		{
			// LowerBound = 0;
			UpperBound = getTotalStageEvents() - 1;
			break;
		}
		case WARPS_INDEX:
		{
			if ((tempCurrentMenuOption + 1) == INDEX_SELECT_MAP)
			{
				// LowerBound = 0;
				UpperBound = 271;
			}
			else
			{
				LowerBound = 1;
				UpperBound = sizeof(WarpByIndex.EntranceList) / sizeof(WarpByIndex.EntranceList[0]);
			}
			break;
		}
		case SPAWN_ITEM_MENU_VALUE:
		{
			LowerBound = ttyd::item_data::Item::StrangeSack;
			UpperBound = ttyd::item_data::Item::SuperChargeP;
			break;
		}
		default:
		{
			break;
		}
	}
	
	arrayOut[0] = LowerBound;
	arrayOut[1] = UpperBound;
}

uint32_t *getPouchAddressAndSize(uint32_t tempArray[2])
{
	uint32_t tempAddressOffset;
	uint32_t tempSize;
	
	switch (MenuVar.MenuSelectedOption)
	{
		case INVENTORY_STANDARD:
		{
			tempAddressOffset 	= 0x192;
			tempSize 			= 20;
			break;
		}
		case INVENTORY_IMPORTANT:
		{
			tempAddressOffset 	= 0xA0;
			tempSize 			= 121;
			break;
		}
		case INVENTORY_BADGES:
		{
			tempAddressOffset 	= 0x1FA;
			tempSize 			= 200;
			break;
		}
		case INVENTORY_STORED_ITEMS:
		{
			tempAddressOffset 	= 0x1BA;
			tempSize 			= 32;
			break;
		}
		default:
		{
			return nullptr;
		}
	}
	
	uint32_t PouchPtr = reinterpret_cast<uint32_t>(ttyd::mario_pouch::pouchGetPtr());
	tempArray[0] = PouchPtr + tempAddressOffset;
	tempArray[1] = tempSize;
	return tempArray;
}

bool checkForItemsOnNextPage(uint32_t currentPage)
{
	uint32_t Address_and_Size[2];
	uint32_t *tempArray		= getPouchAddressAndSize(Address_and_Size);
	
	if (!tempArray)
	{
		return false;
	}
	
	uint32_t tempAddress 	= tempArray[0];
	uint32_t tempSize 		= tempArray[1];
	
	uint32_t MaxIconsPerPage = 20;
	uint32_t MaxPage = 1 + ((tempSize - 1) / MaxIconsPerPage); // Round up;
	int16_t CurrentItem;
	
	if ((currentPage + 1) < MaxPage)
	{
		CurrentItem = *reinterpret_cast<int16_t *>(
			tempAddress + ((currentPage + 1) * (MaxIconsPerPage * 2)));
	}
	else
	{
		return false;
	}
	
	return CurrentItem != 0;
}

bool checkErrorMessageReqs()
{
	if (MenuVar.FunctionReturnCode >= 0)
	{
		return false;
	}
	
	if (MenuVar.Timer == 0)
	{
		return false;
	}
	
	if (checkForClosingErrorMessage())
	{
		return false;
	}
	
	return true;
}

bool checkForClosingErrorMessage()
{
	uint32_t tempTimer = MenuVar.Timer;
	if (checkForDPADInput())
	{
		MenuVar.Timer 				= 0;
		MenuVar.FunctionReturnCode 	= 0;
		return true;
	}
	else
	{
		tempTimer--;
		MenuVar.Timer = tempTimer;
		
		if (tempTimer == 0)
		{
			MenuVar.FunctionReturnCode = 0;
			return true;
		}
	}
	return false;
}

void correctInventoryCurrentMenuOptionAndPage(uint32_t maxOptionsPerPage)
{
	// Make sure that an option has already been selected
	uint32_t tempSelectedOption = MenuVar.SelectedOption;
	if (tempSelectedOption == 0)
	{
		// No option is currently selected, so do nothing
		return;
	}
	
	// Make sure the number of items if valid
	int32_t tempTotalMenuOptions = getTotalItems();
	if (tempTotalMenuOptions == NULL_PTR)
	{
		return;
	}
	
	uint32_t TotalMenuOptions = static_cast<uint32_t>(tempTotalMenuOptions);
	if (TotalMenuOptions == 0)
	{
		// No items in the inventory, so reset the current page and leave
		MenuVar.CurrentPage = 0;
		return;
	}
	
	uint32_t TotalPages 			= 1 + ((TotalMenuOptions - 1) / maxOptionsPerPage); // Round up
	uint32_t tempCurrentMenuOption 	= MenuVar.CurrentMenuOption;
	uint32_t tempCurrentPage 		= MenuVar.CurrentPage;
	
	if ((tempSelectedOption >= DUPLICATE) && 
		(tempSelectedOption <= DELETE))
	{
		if (tempCurrentMenuOption > (TotalMenuOptions - 1))
		{
			MenuVar.CurrentMenuOption = TotalMenuOptions - 1;
		}
	}
	
	if (tempCurrentPage > (TotalPages - 1))
	{
		MenuVar.CurrentPage = TotalPages - 1;
	}
}

uint32_t getHighestAdjustableValueDigit(uint32_t currentMenu)
{
	int32_t UpperAndLowerBounds[2];
	getUpperAndLowerBounds(UpperAndLowerBounds, currentMenu);
	
	int32_t LowerBound = UpperAndLowerBounds[0];
	int32_t UpperBound = UpperAndLowerBounds[1];
	
	// Make sure each value is positive
	if (LowerBound < 0)
	{
		LowerBound = -LowerBound;
	}
	
	if (UpperBound < 0)
	{
		UpperBound = -UpperBound;
	}
	
	// Use the biggest value
	int32_t tempValue;
	if (UpperBound > LowerBound)
	{
		tempValue = UpperBound;
	}
	else
	{
		tempValue = LowerBound;
	}
	
	// Get the highest digit for the current value
	uint32_t Counter = 0;
	while (tempValue > 0)
	{
		tempValue /= 10;
		Counter++;
	}
	
	return Counter;
}

int32_t getDigitBeingChanged(int32_t number, int32_t valueChangedBy)
{
	// Get the digit being changed
	uint32_t Counter = 0;
	
	while (valueChangedBy > 0)
	{
		valueChangedBy /= 10;
		Counter++;
	}
	
	for (uint32_t i = 0; i < (Counter - 1); i++)
	{
		number /= 10;
	}
	
	return number %= 10;
}

void setAdjustableValueToMax(uint32_t currentMenu)
{
	int32_t UpperAndLowerBounds[2];
	getUpperAndLowerBounds(UpperAndLowerBounds, currentMenu);
	
	int32_t UpperBound = UpperAndLowerBounds[1];
	MenuVar.MenuSecondaryValue = UpperBound;
}

void setAdjustableValueToMin(uint32_t currentMenu)
{
	int32_t UpperAndLowerBounds[2];
	getUpperAndLowerBounds(UpperAndLowerBounds, currentMenu);
	
	int32_t LowerBound = UpperAndLowerBounds[0];
	MenuVar.MenuSecondaryValue = LowerBound;
}

uint32_t adjustableValueButtonControls(uint32_t currentMenu)
{
	// Get the amount of numbers to draw
	uint32_t AmountOfNumbers = getHighestAdjustableValueDigit(currentMenu);
	
	// Make sure there is at least 1 number to display
	bool NoNumbersToDisplay = false;
	uint32_t Button = 0;
	
	if (AmountOfNumbers == 0)
	{
		// Close the menu if there are no numbers to draw
		NoNumbersToDisplay = true;
		Button = B;
	}
	else
	{
		// Check to see if D-Pad Up or D-Pad Down are being held
		if (checkButtonComboEveryFrame(PAD_DPAD_UP))
		{
			Button = DPADUP;
		}
		else if (checkButtonComboEveryFrame(PAD_DPAD_DOWN))
		{
			Button = DPADDOWN;
		}
		
		if (Button != 0)
		{
			// Check to see if the value should begin to auto-increment
			if (AdjustableValueMenu.WaitFramesToBeginIncrement >= 
				ttyd::system::sysMsec2Frame(500))
			{
				// Check to see if the number should increment or not
				if (AdjustableValueMenu.WaitFramesToPerformIncrement >= 1)
				{
					// Auto-increment the value
					int32_t IncrementAmount;
					if (Button == DPADUP)
					{
						IncrementAmount = 1;
					}
					else // (Button == DPADDOWN)
					{
						IncrementAmount = -1;
					}
					
					adjustAddByIdValue(IncrementAmount, currentMenu);
					AdjustableValueMenu.WaitFramesToPerformIncrement = 0;
					return Button;
				}
				else
				{
					AdjustableValueMenu.WaitFramesToPerformIncrement++;
				}
			}
			else
			{
				AdjustableValueMenu.WaitFramesToBeginIncrement++;
			}
		}
		else
		{
			// Reset the counters
			AdjustableValueMenu.WaitFramesToBeginIncrement = 0;
			AdjustableValueMenu.WaitFramesToPerformIncrement = 0;
		}
	}
	
	if (!NoNumbersToDisplay)
	{
		Button = checkButtonSingleFrame();
	}
	
	uint32_t tempMenuSelectedOption = MenuVar.MenuSelectedOption;
	uint32_t tempCurrentMenuOption = MenuVar.CurrentMenuOption;
	
	switch (Button)
	{
		case DPADLEFT:
		{
			uint32_t tempSecondaryMenuOption = MenuVar.SecondaryMenuOption;
			if (tempSecondaryMenuOption == 0)
			{
				// Loop to the last option
				MenuVar.SecondaryMenuOption = getHighestAdjustableValueDigit(currentMenu) - 1;
			}
			else
			{
				// Move left once
				MenuVar.SecondaryMenuOption = tempSecondaryMenuOption - 1;
			}
			
			MenuVar.FrameCounter = 1;
			return Button;
		}
		case DPADRIGHT:
		{
			uint32_t tempSecondaryMenuOption = MenuVar.SecondaryMenuOption;
			if (tempSecondaryMenuOption == (getHighestAdjustableValueDigit(currentMenu) - 1))
			{
				// Loop to the first option
				MenuVar.SecondaryMenuOption = 0;
			}
			else
			{
				// Move right once
				MenuVar.SecondaryMenuOption = tempSecondaryMenuOption + 1;
			}
			
			MenuVar.FrameCounter = 1;
			return Button;
		}
		case DPADDOWN:
		{
			// Decrement the current value for the current slot in the drawAddById function
			adjustAddByIdValue(-1, currentMenu);
			
			MenuVar.FrameCounter = 1;
			return Button;
		}
		case DPADUP:
		{
			// Increment the current value for the current slot in the drawAddById function
			adjustAddByIdValue(1, currentMenu);
			
			MenuVar.FrameCounter = 1;
			return Button;
		}
		case Z:
		{
			if (currentMenu == INVENTORY_MAIN)
			{
				setAdjustableValueToMin(tempMenuSelectedOption);
			}
			else
			{
				setAdjustableValueToMin(currentMenu);
			}
			
			MenuVar.FrameCounter = 1;
			return Button;
		}
		case A:
		{
			switch (currentMenu)
			{
				case INVENTORY_STANDARD:
				case INVENTORY_IMPORTANT:
				case INVENTORY_BADGES:
				case INVENTORY_STORED_ITEMS:
				{
					switch (MenuVar.SelectedOption)
					{
						case ADD_BY_ID:
						{
							void *tempAddress = getFreeSlotPointer();
							if (!tempAddress)
							{
								return 0;
							}
							
							setAddByIdValue(tempAddress);
							
							MenuVar.FrameCounter = 1;
							return ADDING_BY_ID;
						}
						case CHANGE_BY_ID:
						{
							if (getTotalItems() <= 0)
							{
								return 0;
							}
							
							changeItem();
							MenuVar.MenuSelectionStates = 0;
							
							MenuVar.FrameCounter = 1;
							return Button;
						}
						default:
						{
							return 0;
						}
					}
				}
				case CHEATS_CHANGE_SEQUENCE:
				{
					setSequencePosition(static_cast<uint32_t>(MenuVar.MenuSecondaryValue));
					MenuVar.MenuSelectionStates = 0;
					
					MenuVar.FrameCounter = 1;
					return Button;
				}
				case CHEATS_NPC_FORCE_DROP:
				{
					MenuVar.ForcedNPCItemDrop = static_cast<int16_t>(MenuVar.MenuSecondaryValue);
					MenuVar.MenuSelectionStates = 0;
					
					MenuVar.FrameCounter = 1;
					return Button;
				}
				case CHEATS_MANAGE_FLAGS_MAIN:
				{
					switch (MenuVar.MenuSelectionStates)
					{
						case SET_GSW:
						case SET_GW:
						case SET_LSW:
						{
							if (tempCurrentMenuOption == CHANGE_GLOBAL_WORD)
							{
								ManageFlags.FlagToSet = static_cast<uint16_t>(MenuVar.MenuSecondaryValue);
							}
							else
							{
								ManageFlags.ValueToSet = MenuVar.MenuSecondaryValue;
							}
							
							MenuVar.SelectedOption = 0;
							
							MenuVar.FrameCounter = 1;
							return Button;
						}
						case SET_GSWF:
						case SET_GF:
						case SET_LSWF:
						{
							if (tempCurrentMenuOption == CHANGE_GLOBAL_FLAG)
							{
								ManageFlags.FlagToSet = static_cast<uint16_t>(MenuVar.MenuSecondaryValue);
							}
							else
							{
								ManageFlags.ValueToSet = MenuVar.MenuSecondaryValue;
							}
							
							MenuVar.SelectedOption = 0;
							
							MenuVar.FrameCounter = 1;
							return Button;
						}
						default:
						{
							return 0;
						}
					}
				}
				case STATS_MARIO:
				{
					setMarioStatsValue(tempCurrentMenuOption + 1);
					MenuVar.MenuSelectedOption = 0;
					
					MenuVar.FrameCounter = 1;
					return Button;
				}
				case STATS_PARTNERS:
				{
					setPartnerStatsValue(tempCurrentMenuOption + 1);
					MenuVar.MenuSelectedOption = 0;
					
					MenuVar.FrameCounter = 1;
					return Button;
				}
				case BATTLES_CURRENT_ACTOR:
				{
					setBattlesActorValue(tempCurrentMenuOption + 1);
					MenuVar.SelectedOption = 0;
					
					MenuVar.FrameCounter = 1;
					return Button;
				}
				case BATTLES_STATUSES:
				{
					setBattlesActorStatusValue(tempCurrentMenuOption);
					MenuVar.SelectedOption = 0;
					
					MenuVar.FrameCounter = 1;
					return Button;
				}
				case WARPS:
				{
					ttyd::swdrv::swByteSet(1321, static_cast<uint32_t>(
						MenuVar.MenuSecondaryValue - 1)); // GSW(1321)
					
					MenuVar.MenuSelectedOption = 0;
					
					// Warp to the currently selected map and close the menu
					int32_t ReturnCode = warpToMap(tempCurrentMenuOption);
					switch (ReturnCode)
					{
						case UNKNOWN_BEHAVIOR:
						{
							break;
						}
						case SUCCESS:
						{
							closeMenu();
							Button = NO_NUMBERS_TO_DISPLAY; // Force the adjustable menu to not display
							break;
						}
						case NOT_IN_GAME:
						{
							MenuVar.FunctionReturnCode 	= ReturnCode;
							MenuVar.Timer 				= secondsToFrames(3);
							break;
						}
						default:
						{
							break;
						}
					}
					
					MenuVar.FrameCounter = 1;
					return Button;
				}
				case WARPS_EVENT:
				{
					WarpByEvent.CurrentIndex = MenuVar.MenuSecondaryValue;
					MenuVar.SelectedOption = 0;
					
					MenuVar.FrameCounter = 1;
					return Button;
				}
				case WARPS_INDEX:
				{
					if ((tempCurrentMenuOption + 1) == INDEX_SELECT_MAP)
					{
						WarpByIndex.MapId = static_cast<uint16_t>(MenuVar.MenuSecondaryValue);
					}
					else
					{
						WarpByIndex.EntranceId = static_cast<uint8_t>(MenuVar.MenuSecondaryValue);
					}
					
					MenuVar.SelectedOption = 0;
					
					MenuVar.FrameCounter = 1;
					return Button;
				}
				case SPAWN_ITEM_MENU_VALUE:
				{
					MenuVar.FrameCounter = 1;
					return Button;
				}
				default:
				{
					return 0;
				}
			}
		}
		case B:
		{
			if (currentMenu != SPAWN_ITEM_MENU_VALUE)
			{
				switch (currentMenu)
				{
					case INVENTORY_STANDARD:
					case INVENTORY_IMPORTANT:
					case INVENTORY_BADGES:
					case INVENTORY_STORED_ITEMS:
					{
						if (MenuVar.SelectedOption == ADD_BY_ID)
						{
							MenuVar.Timer = 0;
							closeSecondaryMenu();
						}
						else // (MenuVar.SelectedOption == CHANGE_BY_ID)
						{
							MenuVar.MenuSelectionStates = 0;
						}
						
						MenuVar.FrameCounter = 1;
						
						if (!NoNumbersToDisplay)
						{
							return Button;
						}
						else
						{
							return NO_NUMBERS_TO_DISPLAY;
						}
					}
					case CHEATS_CHANGE_SEQUENCE:
					case CHEATS_NPC_FORCE_DROP:
					{
						MenuVar.MenuSelectionStates = 0;
						
						MenuVar.FrameCounter = 1;
						
						if (!NoNumbersToDisplay)
						{
							return Button;
						}
						else
						{
							return NO_NUMBERS_TO_DISPLAY;
						}
					}
					case STATS_MARIO:
					case STATS_PARTNERS:
					case WARPS:
					{
						MenuVar.MenuSelectedOption = 0;
						
						MenuVar.FrameCounter = 1;
						
						if (!NoNumbersToDisplay)
						{
							return Button;
						}
						else
						{
							return NO_NUMBERS_TO_DISPLAY;
						}
					}
					case CHEATS_MANAGE_FLAGS_MAIN:
					case BATTLES_CURRENT_ACTOR:
					case BATTLES_STATUSES:
					case WARPS_EVENT:
					case WARPS_INDEX:
					{
						MenuVar.SelectedOption = 0;
						
						MenuVar.FrameCounter = 1;
						
						if (!NoNumbersToDisplay)
						{
							return Button;
						}
						else
						{
							return NO_NUMBERS_TO_DISPLAY;
						}
					}
					default:
					{
						return 0;
					}
				}
			}
		}
		case Y:
		{
			if (currentMenu == INVENTORY_MAIN)
			{
				setAdjustableValueToMax(tempMenuSelectedOption);
			}
			else
			{
				setAdjustableValueToMax(currentMenu);
			}
			
			MenuVar.FrameCounter = 1;
			return Button;
		}
		default:
		{
			return 0;
		}
	}
}

uint32_t addByIconButtonControls(uint32_t currentMenu)
{
	uint32_t tempSelectedOption = MenuVar.SelectedOption;
	uint32_t Button = checkButtonSingleFrame();
	
	switch (Button)
	{
		case DPADLEFT:
		case DPADRIGHT:
		case DPADDOWN:
		case DPADUP:
		{
			int32_t UpperAndLowerBounds[2];
			getUpperAndLowerBounds(UpperAndLowerBounds, currentMenu);
			int32_t LowerBound 				= UpperAndLowerBounds[0];
			int32_t UpperBound 				= UpperAndLowerBounds[1];
			
			uint32_t MaxOptionsPerRow 		= 16;
			uint32_t tempTotalMenuOptions 	= UpperBound - LowerBound + 1;
			uint32_t MaxOptionsPerPage 		= tempTotalMenuOptions;
			
			adjustMenuSelectionHorizontal(Button, MenuVar.SecondaryMenuOption, 
				MenuVar.SecondaryPage, tempTotalMenuOptions, MaxOptionsPerPage, 
					MaxOptionsPerRow, false);
			
			MenuVar.FrameCounter = 1;
			return Button;
		}
		case A:
		{
			if (tempSelectedOption == ADD_BY_ICON)
			{
				void *tempAddress = getFreeSlotPointer();
				if (!tempAddress)
				{
					return 0;
				}
				
				setAddByIconValue(tempAddress);
				
				MenuVar.FrameCounter = 1;
				return ADDING_BY_ICON;
			}
			else // (tempSelectedOption == CHANGE_BY_ICON)
			{
				if (getTotalItems() <= 0)
				{
					return 0;
				}
				
				MenuVar.MenuSelectionStates = 0;
				changeItem();
				
				MenuVar.FrameCounter = 1;
				return Button;
			}
		}
		case B:
		{
			if (tempSelectedOption == ADD_BY_ICON)
			{
				resetAndCloseSecondaryMenu();
			}
			else // (tempSelectedOption == CHANGE_BY_ICON)
			{
				MenuVar.MenuSelectionStates = 0;
			}
			
			MenuVar.FrameCounter = 1;
			return Button;
		}
		default:
		{
			return 0;
		}
	}
}

uint32_t memoryAddressTypeButtonControls()
{
	uint32_t Button = checkButtonSingleFrame();
	uint32_t tempSecondaryMenuOption = MenuVar.SecondaryMenuOption;
	
	switch (Button)
	{
		case DPADDOWN:
		case DPADUP:
		{
			uint32_t tempMemoryTypeLinesSize = MemoryTypeLinesSize;
			uint32_t TotalMenuOptions = tempMemoryTypeLinesSize;
			uint32_t MaxOptionsPerRow = 1;
			uint32_t TotalRows = 1 + ((TotalMenuOptions - 1) / MaxOptionsPerRow); // Round up
			uint32_t MaxOptionsPerPage = TotalRows * MaxOptionsPerRow;
			
			adjustMenuSelectionVertical(Button, MenuVar.SecondaryMenuOption, 
				MenuVar.SecondaryPage, TotalMenuOptions, MaxOptionsPerPage, 
					MaxOptionsPerRow, false);
			
			MenuVar.FrameCounter = 1;
			return Button;
		}
		case A:
		{
			uint32_t tempMenuSelectedOption = MenuVar.MenuSelectedOption;
			MemoryWatch[tempMenuSelectedOption].Type = tempSecondaryMenuOption;
			
			// Make sure the address being read does not exceed 0x817FFFFF
			MemoryWatch[tempMenuSelectedOption].Address = reinterpret_cast<uint32_t>(
				fixBaseAddress(tempMenuSelectedOption, reinterpret_cast<void *>(
					MemoryWatch[tempMenuSelectedOption].Address)));
			
			MenuVar.SelectedOption = 0;
			
			// Adjust the hex setting if necessary
			switch (tempSecondaryMenuOption)
			{
				case string:
				case time:
				{
					MemoryWatch[tempMenuSelectedOption].ShowAsHex = false;
					break;
				}
				default:
				{
					break;
				}
			}
			
			MenuVar.FrameCounter = 1;
			return Button;
		}
		case B:
		{
			MenuVar.SelectedOption = 0;
			
			MenuVar.FrameCounter = 1;
			return Button;
		}
		default:
		{
			return 0;
		}
	}
}

uint32_t memoryChangeWatchPositionButtonControls()
{
	uint32_t tempMenuSelectedOption = MenuVar.MenuSelectedOption;
	bool CurrentlyAutoIncrementing = false;
	uint32_t Button = 0;
	
	// Check to see if a D-Pad Button is being held
	if (checkButtonComboEveryFrame(PAD_DPAD_LEFT))
	{
		Button = DPADLEFT;
	}
	else if (checkButtonComboEveryFrame(PAD_DPAD_RIGHT))
	{
		Button = DPADRIGHT;
	}
	else if (checkButtonComboEveryFrame(PAD_DPAD_UP))
	{
		Button = DPADUP;
	}
	else if (checkButtonComboEveryFrame(PAD_DPAD_DOWN))
	{
		Button = DPADDOWN;
	}
	
	if (Button != 0)
	{
		// Check to see if the value should begin to auto-increment
		if (MemoryWatchPosition.WaitFramesToBeginIncrement >= 
			ttyd::system::sysMsec2Frame(500))
		{
			// Check to see if the number should increment or not
			if (MemoryWatchPosition.WaitFramesToPerformIncrement >= 1)
			{
				// Auto-increment the value
				switch (Button)
				{
					case DPADLEFT:
					{
						int32_t PosX = MemoryWatchPosition.PosX;
						MemoryWatchPosition.PosX = PosX - 5;
						break;
					}
					case DPADRIGHT:
					{
						int32_t PosX = MemoryWatchPosition.PosX;
						MemoryWatchPosition.PosX = PosX + 5;
						break;
					}
					case DPADUP:
					{
						int32_t PosY = MemoryWatchPosition.PosY;
						MemoryWatchPosition.PosY = PosY + 5;
						break;
					}
					case DPADDOWN:
					{
						int32_t PosY = MemoryWatchPosition.PosY;
						MemoryWatchPosition.PosY = PosY - 5;
						break;
					}
					default:
					{
						break;
					}
				}
				
				MemoryWatchPosition.WaitFramesToPerformIncrement = 0;
				return Button;
			}
			else
			{
				MemoryWatchPosition.WaitFramesToPerformIncrement++;
				CurrentlyAutoIncrementing = true;
			}
		}
		else
		{
			MemoryWatchPosition.WaitFramesToBeginIncrement++;
		}
	}
	else
	{
		// Reset the counters
		MemoryWatchPosition.WaitFramesToBeginIncrement = 0;
		MemoryWatchPosition.WaitFramesToPerformIncrement = 0;
	}
	
	if (CurrentlyAutoIncrementing)
	{
		return 0;
	}
	
	Button = checkButtonSingleFrame();
	switch (Button)
	{
		case DPADLEFT:
		{
			int32_t PosX = MemoryWatchPosition.PosX;
			MemoryWatchPosition.PosX = PosX - 1;
			
			MenuVar.FrameCounter = 1;
			return Button;
		}
		case DPADRIGHT:
		{
			int32_t PosX = MemoryWatchPosition.PosX;
			MemoryWatchPosition.PosX = PosX + 1;
			
			MenuVar.FrameCounter = 1;
			return Button;
		}
		case DPADUP:
		{
			int32_t PosY = MemoryWatchPosition.PosY;
			MemoryWatchPosition.PosY = PosY + 1;
			
			MenuVar.FrameCounter = 1;
			return Button;
		}
		case DPADDOWN:
		{
			int32_t PosY = MemoryWatchPosition.PosY;
			MemoryWatchPosition.PosY = PosY - 1;
			
			MenuVar.FrameCounter = 1;
			return Button;
		}
		case A:
		{
			MemoryWatch[tempMenuSelectedOption].PosX = MemoryWatchPosition.PosX;
			MemoryWatch[tempMenuSelectedOption].PosY = MemoryWatchPosition.PosY;
			
			MenuVar.SelectedOption = 0;
			MenuVar.HideMenu = false;
			
			MenuVar.FrameCounter = 1;
			return Button;
		}
		case B:
		{
			MenuVar.SelectedOption = 0;
			MenuVar.HideMenu = false;
			
			MenuVar.FrameCounter = 1;
			return Button;
		}
		default:
		{
			return 0;
		}
	}
}

uint32_t marioSpecialMovesButtonControls()
{
	uint32_t Button = checkButtonSingleFrame();
	switch (Button)
	{
		case DPADDOWN:
		case DPADUP:
		{
			uint32_t tempMarioStatsSpecialMovesOptionsSize = MarioStatsSpecialMovesOptionsSize;
			uint32_t TotalMenuOptions = tempMarioStatsSpecialMovesOptionsSize;
			uint32_t MaxOptionsPerRow = 1;
			uint32_t TotalRows = 1 + ((TotalMenuOptions - 1) / MaxOptionsPerRow); // Round up
			uint32_t MaxOptionsPerPage = TotalRows * MaxOptionsPerRow;
			
			adjustMenuSelectionVertical(Button, MenuVar.SecondaryMenuOption, 
				MenuVar.SecondaryPage, TotalMenuOptions, MaxOptionsPerPage, 
					MaxOptionsPerRow, false);
			
			MenuVar.FrameCounter = 1;
			return Button;
		}
		case A:
		{
			uint32_t PouchPtr = reinterpret_cast<uint32_t>(ttyd::mario_pouch::pouchGetPtr());
			*reinterpret_cast<int16_t *>(PouchPtr + 0x8C) ^= (1 << MenuVar.SecondaryMenuOption);
			
			MenuVar.FrameCounter = 1;
			return Button;
		}
		case B:
		{
			MenuVar.MenuSelectedOption = 0;
			
			MenuVar.FrameCounter = 1;
			return Button;
		}
		default:
		{
			return 0;
		}
	}
}

uint32_t partnerChangeYoshiColorButtonControls()
{
	uint32_t Button = checkButtonSingleFrame();
	switch (Button)
	{
		case DPADDOWN:
		case DPADUP:
		{
			uint32_t tempStatsYoshiColorOptionsLinesSize = StatsYoshiColorOptionsLinesSize;
			uint32_t TotalMenuOptions = tempStatsYoshiColorOptionsLinesSize;
			uint32_t MaxOptionsPerPage = TotalMenuOptions;
			uint32_t MaxOptionsPerRow = 1;
			
			adjustMenuSelectionVertical(Button, MenuVar.SecondaryMenuOption, 
				MenuVar.SecondaryPage, TotalMenuOptions, MaxOptionsPerPage, 
					MaxOptionsPerRow, false);
			
			MenuVar.FrameCounter = 1;
			return Button;
		}
		case A:
		{
			setNewYoshiColorId(MenuVar.SecondaryMenuOption);
			
			// Reset the partner menu
			resetPartnerPauseMenu();
			
			MenuVar.MenuSelectedOption = 0;
			
			MenuVar.FrameCounter = 1;
			return Button;
		}
		case B:
		{
			MenuVar.MenuSelectedOption = 0;
			
			MenuVar.FrameCounter = 1;
			return Button;
		}
		default:
		{
			return 0;
		}
	}
}

uint32_t followersOptionsButtonControls()
{
	uint32_t Button = checkButtonSingleFrame();
	switch (Button)
	{
		case DPADDOWN:
		case DPADUP:
		{
			uint32_t tempStatsFollowerOptionsLinesSize = StatsFollowerOptionsLinesSize;
			uint32_t TotalMenuOptions = tempStatsFollowerOptionsLinesSize;
			uint32_t MaxOptionsPerRow = 1;
			uint32_t TotalRows = 1 + ((TotalMenuOptions - 1) / MaxOptionsPerRow); // Round up
			uint32_t MaxOptionsPerPage = TotalRows * MaxOptionsPerRow;
			
			adjustMenuSelectionVertical(Button, MenuVar.SecondaryMenuOption, 
				MenuVar.SecondaryPage, TotalMenuOptions, MaxOptionsPerPage, 
					MaxOptionsPerRow, false);
			
			MenuVar.FrameCounter = 1;
			return Button;
		}
		case A:
		{
			if (!checkIfInGame())
			{
				return 0;
			}
			
			// Get the follower id to use
			uint32_t NewFollower = MenuVar.SecondaryMenuOption + 8; // Start at the egg
			
			// Spawn the new follower
			spawnPartnerOrFollower(static_cast<ttyd::party::PartyMembers>(NewFollower));
			
			closeSecondaryMenu();
			
			MenuVar.FrameCounter = 1;
			return Button;
		}
		case B:
		{
			closeSecondaryMenu();
			
			MenuVar.FrameCounter = 1;
			return Button;
		}
		default:
		{
			return 0;
		}
	}
}

/*void adjustMenuItemBoundsMain(int32_t valueChangedBy, int32_t lowerBound, int32_t upperBound)
{
	int32_t tempMenuSecondaryValue = MenuVar.MenuSecondaryValue;
	
	if (tempMenuSecondaryValue == 0)
	{
		if (lowerBound <= 0)
		{
			// The value can be 0 or less, so do nothing
			return;
		}
		else if (valueChangedBy != 0)
		{
			// Loop to the end
			MenuVar.MenuSecondaryValue = upperBound;
		}
		else
		{
			// Set to the lower bound by default
			MenuVar.MenuSecondaryValue = lowerBound;
		}
	}
	else if (valueChangedBy != 0)
	{
		// Make sure the number is positive
		int32_t tempValueChangedBy = valueChangedBy;
		if (tempValueChangedBy < 0)
		{
			tempValueChangedBy = -tempValueChangedBy;
		}
		
		// Get the digit being changed
		int32_t tempDigit = getDigitBeingChanged(tempMenuSecondaryValue, tempValueChangedBy);
		
		// Get the digits of the lower and upper bounds
		// int32_t LowerBoundsDigit = getDigitBeingChanged(lowerBound, tempValueChangedBy);
		// int32_t UpperBoundsDigit = getDigitBeingChanged(upperBound, tempValueChangedBy);
		
		// Add/subtract 1 from the current digit
		if (valueChangedBy > 0)
		{
			tempDigit++;
			
			// Make sure the digit is valid
			if (tempDigit > 9)
			{
				int32_t NewValue = tempMenuSecondaryValue + (-9 * valueChangedBy);
				
				// Make sure the new value doesn't go below the lower bound
				while (NewValue < lowerBound)
				{
					NewValue += (1 * valueChangedBy);
				}
				
				// if (NewValue < lowerBound)
				// {
				// 	// NewValue = upperBound;
				// 	NewValue += (LowerBoundsDigit * (1 * valueChangedBy));
				// }
				
				MenuVar.MenuSecondaryValue = NewValue;
			}
			else
			{
				int32_t NewValue = tempMenuSecondaryValue + (1 * valueChangedBy);
				
				// Make sure the new value doesn't go above the upper bound
				if (NewValue > upperBound)
				{
					NewValue -= (tempDigit * valueChangedBy);
					
					// Make sure the new value doesn't go below the lower bound
					while (NewValue < lowerBound)
					{
						NewValue += (1 * valueChangedBy);
					}
				}
				
				MenuVar.MenuSecondaryValue = NewValue;
			}
		}
		else
		{
			// Make the number positive
			valueChangedBy = -valueChangedBy;
			
			tempDigit--;
			
			// Make sure the digit is valid
			if (tempDigit < 0)
			{
				int32_t NewValue = tempMenuSecondaryValue + (9 * valueChangedBy);
				
				// Make sure the new value doesn't go above the upper bound
				while (NewValue > upperBound)
				{
					NewValue -= (1 * valueChangedBy);
				}
				
				MenuVar.MenuSecondaryValue = NewValue;
			}
			else
			{
				int32_t NewValue = tempMenuSecondaryValue - (1 * valueChangedBy);
				
				// Make sure the new value doesn't go below the lower bound
				if (NewValue < lowerBound)
				{
					// NewValue = upperBound;
					NewValue = tempMenuSecondaryValue + ((9 - tempDigit - 1) * valueChangedBy);
					
					// Make sure the new value doesn't go above the upper bound
					while (NewValue > upperBound)
					{
						NewValue -= (1 * valueChangedBy);
					}
				}
				
				MenuVar.MenuSecondaryValue = NewValue;
			}
		}
	}
}*/

void adjustMenuItemBoundsMain(int32_t valueChangedBy, int32_t lowerBound, int32_t upperBound)
{
	int32_t tempMenuSecondaryValue = MenuVar.MenuSecondaryValue + valueChangedBy;
	MenuVar.MenuSecondaryValue = tempMenuSecondaryValue;
	
	if (tempMenuSecondaryValue == 0)
	{
		if (lowerBound <= 0)
		{
			// The value can be 0 or less, so do nothing
			return;
		}
		else if (valueChangedBy != 0)
		{
			// Loop to the end
			MenuVar.MenuSecondaryValue = upperBound;
		}
		else
		{
			// Set to the lower bound by default
			MenuVar.MenuSecondaryValue = lowerBound;
		}
	}
	else if (tempMenuSecondaryValue < lowerBound)
	{
		// Loop to the end
		MenuVar.MenuSecondaryValue = upperBound;
	}
	else if (tempMenuSecondaryValue > upperBound)
	{
		// Loop to the beginning
		MenuVar.MenuSecondaryValue = lowerBound;
	}
}

void adjustMenuItemBounds(int32_t valueChangedBy, uint32_t currentMenu)
{
	int32_t tempMenuSecondaryValue = MenuVar.MenuSecondaryValue + valueChangedBy;
	switch (currentMenu)
	{
		case CHEATS_NPC_FORCE_DROP:
		{
			if ((tempMenuSecondaryValue > ttyd::item_data::Item::FreshJuice) && 
				(tempMenuSecondaryValue < ttyd::item_data::Item::PowerJump))
			{
				if (valueChangedBy > 0)
				{
					MenuVar.MenuSecondaryValue = ttyd::item_data::Item::PowerJump;
				}
				else
				{
					MenuVar.MenuSecondaryValue = ttyd::item_data::Item::FreshJuice;
				}
				return;
			}
			break;
		}
		case BATTLES_CURRENT_ACTOR:
		{
			if (MenuVar.SelectedOption == CHANGE_HELD_ITEM)
			{
				if (tempMenuSecondaryValue == ttyd::item_data::Item::TradeOff)
				{
					if (valueChangedBy > 0)
					{
						MenuVar.MenuSecondaryValue = tempMenuSecondaryValue + 1;
					}
					else
					{
						MenuVar.MenuSecondaryValue = tempMenuSecondaryValue - 1;
					}
					return;
				}
				else if ((tempMenuSecondaryValue > ttyd::item_data::Item::Cake) && 
					(tempMenuSecondaryValue < ttyd::item_data::Item::PowerJump))
				{
					if (valueChangedBy > 0)
					{
						MenuVar.MenuSecondaryValue = ttyd::item_data::Item::PowerJump;
					}
					else
					{
						MenuVar.MenuSecondaryValue = ttyd::item_data::Item::Cake;
					}
					return;
				}
			}
			break;
		}
		default:
		{
			break;
		}
	}
	
	int32_t UpperAndLowerBounds[2];
	getUpperAndLowerBounds(UpperAndLowerBounds, currentMenu);
	
	int32_t LowerBound = UpperAndLowerBounds[0];
	int32_t UpperBound = UpperAndLowerBounds[1];
	
	adjustMenuItemBoundsMain(valueChangedBy, LowerBound, UpperBound);
}

void adjustAddByIdValue(int32_t value, uint32_t currentMenu)
{
	uint32_t HighestDigit = getHighestAdjustableValueDigit(currentMenu);
	int32_t newValue = value;
	
	for (uint32_t i = 0; i < (HighestDigit - MenuVar.SecondaryMenuOption - 1); i++)
	{
		newValue *= 10;
	}
	
	// Make sure the total value is valid
	adjustMenuItemBounds(newValue, currentMenu);
}

uint32_t getMarioStatsValueOffset(uint32_t currentMenuOption)
{
	uint32_t offset;
	switch (currentMenuOption)
	{
		case COINS:
		{
			offset = 0x78;
			break;
		}
		case MARIO_HP:
		{
			offset = 0x70;
			break;
		}
		case MARIO_FP:
		{
			offset = 0x74;
			break;
		}
		case BP:
		{
			offset = 0x94;
			break;
		}
		case MARIO_MAX_HP:
		{
			offset = 0x72;
			break;
		}
		case MARIO_MAX_FP:
		{
			offset = 0x76;
			break;
		}
		case MARIO_LEVEL:
		{
			offset = 0x8A;
			break;
		}
		case MARIO_RANK:
		{
			offset = 0x88;
			break;
		}
		case STAR_POINTS:
		{
			offset = 0x96;
			break;
		}
		case STAR_PIECES:
		{
			offset = 0x9A;
			break;
		}
		case SHINE_SPRITES:
		{
			offset = 0x9C;
			break;
		}
		case STAR_POWER:
		{
			offset = 0x7A;
			break;
		}
		case MAX_STAR_POWER:
		{
			offset = 0x7C;
			break;
		}
		case SHOP_POINTS:
		{
			offset = 0x5B0;
			break;
		}
		case PIANTAS_STORED:
		{
			offset = 0x4;
			break;
		}
		case CURRENT_PIANTAS:
		{
			offset = 0x8;
			break;
		}
		default:
		{
			return 0;
		}
	}
	
	return offset;
}

void setMarioStatsValue(uint32_t currentMenuOption)
{
	uint32_t offset = getMarioStatsValueOffset(currentMenuOption);
	if (offset == 0)
	{
		return;
	}
	
	uint32_t PouchPtr = reinterpret_cast<uint32_t>(ttyd::mario_pouch::pouchGetPtr());
	int32_t tempMenuSecondaryValue = MenuVar.MenuSecondaryValue;
	
	if ((currentMenuOption >= PIANTAS_STORED) && 
		(currentMenuOption <= CURRENT_PIANTAS))
	{
		uint32_t PiantaParlorPtr = reinterpret_cast<uint32_t>(ttyd::evt_yuugijou::yuugijouWorkPointer);
		*reinterpret_cast<int32_t *>(PiantaParlorPtr + offset) = tempMenuSecondaryValue;
	}
	else
	{
		*reinterpret_cast<int16_t *>(PouchPtr + offset) = static_cast<int16_t>(tempMenuSecondaryValue);
	}
	
	// Perform adjustments on certain addresses
	switch (currentMenuOption)
	{
		case MARIO_HP:
		case MARIO_FP:
		case MARIO_RANK:
		case SPECIAL_MOVES:
		case STAR_POWER:
		{
			// Only set the flag for clearing the cache if not in a battle
			if (!checkForSpecificSeq(ttyd::seqdrv::SeqIndex::kBattle))
			{
				ClearCacheForBattles.MarioStatsShouldBeCleared = true;
			}
			break;
		}
		case BP:
		{
			// Force the game to recalculate how much BP is left
			ttyd::mario_pouch::pouchReviseMarioParam();
			break;
		}
		case MARIO_MAX_HP:
		{
			// Reset the value for entering battles
			int16_t MaxHP = *reinterpret_cast<int16_t *>(PouchPtr + 0x72);
			*reinterpret_cast<int16_t *>(PouchPtr + 0x72 + 0x1C) = MaxHP;
			
			// Prevent the current value from exceeding the max value
			int16_t CurrentHP = *reinterpret_cast<int16_t *>(PouchPtr + 0x70);
			if (CurrentHP > MaxHP)
			{
				*reinterpret_cast<int16_t *>(PouchPtr + 0x70) = MaxHP;
			}
			
			// Only set the flag for clearing the cache if not in a battle
			if (!checkForSpecificSeq(ttyd::seqdrv::SeqIndex::kBattle))
			{
				ClearCacheForBattles.MarioStatsShouldBeCleared = true;
			}
			break;
		}
		case MARIO_MAX_FP:
		{
			// Reset the value for entering battles
			int16_t MaxFP = *reinterpret_cast<int16_t *>(PouchPtr + 0x76);
			*reinterpret_cast<int16_t *>(PouchPtr + 0x76 + 0x1A) = MaxFP;
			
			// Prevent the current value from exceeding the max value
			int16_t CurrentFP = *reinterpret_cast<int16_t *>(PouchPtr + 0x74);
			if (CurrentFP > MaxFP)
			{
				*reinterpret_cast<int16_t *>(PouchPtr + 0x74) = MaxFP;
			}
			
			// Only set the flag for clearing the cache if not in a battle
			if (!checkForSpecificSeq(ttyd::seqdrv::SeqIndex::kBattle))
			{
				ClearCacheForBattles.MarioStatsShouldBeCleared = true;
			}
			break;
		}
		case MARIO_LEVEL:
		{
			// Adjust Mario's rank with the level
			/*int16_t MarioLevel = *reinterpret_cast<int16_t *>(PouchPtr + 0x8A);
			int16_t NewRank = MarioLevel / 10;
			
			if (NewRank > 3)
			{
				NewRank = 3;
			}
			
			*reinterpret_cast<int16_t *>(PouchPtr + 0x88) = NewRank;*/
			
			// Only set the flag for clearing the cache if not in a battle
			if (!checkForSpecificSeq(ttyd::seqdrv::SeqIndex::kBattle))
			{
				ClearCacheForBattles.MarioStatsShouldBeCleared = true;
			}
			break;
		}
		case MAX_STAR_POWER:
		{
			// Prevent the current value from exceeding the max value
			int16_t CurrentSP = *reinterpret_cast<int16_t *>(PouchPtr + 0x7A);
			int16_t MaxSP = *reinterpret_cast<int16_t *>(PouchPtr + 0x7C);
			
			if (CurrentSP > MaxSP)
			{
				*reinterpret_cast<int16_t *>(PouchPtr + 0x7A) = MaxSP;
			}
			
			// Only set the flag for clearing the cache if not in a battle
			if (!checkForSpecificSeq(ttyd::seqdrv::SeqIndex::kBattle))
			{
				ClearCacheForBattles.MarioStatsShouldBeCleared = true;
			}
			break;
		}
		default:
		{
			break;
		}
	}
}

void setPartnerStatsValue(uint32_t currentMenuOption)
{
	uint32_t PartnerEnabledAddress = reinterpret_cast<uint32_t>(
		getPartnerEnabledAddress());
	
	int32_t tempMenuSecondaryValue = MenuVar.MenuSecondaryValue;
	switch (currentMenuOption)
	{
		case PARTNER_HP:
		{
			*reinterpret_cast<int16_t *>(PartnerEnabledAddress + 0x6) = 
				static_cast<int16_t>(tempMenuSecondaryValue);
			
			// Only set the flag for clearing the cache if not in a battle
			if (!checkForSpecificSeq(ttyd::seqdrv::SeqIndex::kBattle))
			{
				ClearCacheForBattles.PartnerStatsShouldBeCleared = true;
			}
			break;
		}
		case PARTNER_MAX_HP:
		{
			*reinterpret_cast<int16_t *>(PartnerEnabledAddress + 0x2) = 
				static_cast<int16_t>(tempMenuSecondaryValue);
			
			// Set the Max HP entering battle
			*reinterpret_cast<int16_t *>(PartnerEnabledAddress + 0x4) = 
				static_cast<int16_t>(tempMenuSecondaryValue);
			
			// Only set the flag for clearing the cache if not in a battle
			if (!checkForSpecificSeq(ttyd::seqdrv::SeqIndex::kBattle))
			{
				ClearCacheForBattles.PartnerStatsShouldBeCleared = true;
			}
			break;
		}
		case PARTNER_RANK:
		{
			*reinterpret_cast<int16_t *>(PartnerEnabledAddress + 0xA) = 
				static_cast<int16_t>(tempMenuSecondaryValue);
			
			// Set the rank for battles
			*reinterpret_cast<int16_t *>(PartnerEnabledAddress + 0xC) = 
				static_cast<int16_t>(tempMenuSecondaryValue);
			
			// Set the rank used to verify the partner's stats when equipping/unequipping badges
			*reinterpret_cast<int16_t *>(PartnerEnabledAddress + 0x8) = 
				static_cast<int16_t>(tempMenuSecondaryValue);
			
			// Only set the flag for clearing the cache if not in a battle
			if (!checkForSpecificSeq(ttyd::seqdrv::SeqIndex::kBattle))
			{
				ClearCacheForBattles.PartnerStatsShouldBeCleared = true;
			}
			break;
		}
		default:
		{
			break;
		}
	}
}

void setBattlesActorValue(uint32_t currentMenuOption)
{
	uint32_t ActorAddress = reinterpret_cast<uint32_t>(getActorPointer(MenuVar.MenuSelectedOption));
	if (!ActorAddress)
	{
		return;
	}
	
	int32_t tempMenuSecondaryValue = MenuVar.MenuSecondaryValue;
	switch (currentMenuOption)
	{
		case CHANGE_ACTOR_HP:
		{
			*reinterpret_cast<int16_t *>(ActorAddress + 0x10C) = 
				static_cast<int16_t>(tempMenuSecondaryValue);
			break;
		}
		case CHANGE_ACTOR_MAX_HP:
		{
			*reinterpret_cast<int16_t *>(ActorAddress + 0x108) = 
				static_cast<int16_t>(tempMenuSecondaryValue);
			break;
		}
		case CHANGE_ACTOR_FP:
		{
			*reinterpret_cast<int16_t *>(ActorAddress + 0x112) = 
				static_cast<int16_t>(tempMenuSecondaryValue);
			break;
		}
		case CHANGE_ACTOR_MAX_FP:
		{
			*reinterpret_cast<int16_t *>(ActorAddress + 0x10E) = 
				static_cast<int16_t>(tempMenuSecondaryValue);
			break;
		}
		case CHANGE_HELD_ITEM:
		{
			#ifdef TTYD_US
			uint32_t HeldItemOffset = 0x308;
			uint32_t BadgeFlagOffsetStart = 0x2E0;
			#elif defined TTYD_JP
			uint32_t HeldItemOffset = 0x304;
			uint32_t BadgeFlagOffsetStart = 0x2DC;
			#elif defined TTYD_EU
			uint32_t HeldItemOffset = 0x30C;
			uint32_t BadgeFlagOffsetStart = 0x2E4;
			#endif
			
			// Set the current item/badge
			*reinterpret_cast<int32_t *>(ActorAddress + HeldItemOffset) = tempMenuSecondaryValue;
			
			// If the new item is a badge, then unequip all current badges and equip the new badge
			if (tempMenuSecondaryValue < ttyd::item_data::Item::PowerJump)
			{
				break;
			}
			
			// Do not modify for Mario or the partners
			uint32_t CurrentActorId = *reinterpret_cast<uint32_t *>(ActorAddress + 0x8);
			const uint32_t MarioId 	= 222;
			const uint32_t MowzId 	= 230;
			
			if ((CurrentActorId >= MarioId) && (CurrentActorId <= MowzId))
			{
				break;
			}
			
			// Clear all of the currently equipped badges
			clearMemory(reinterpret_cast<void *>(ActorAddress + BadgeFlagOffsetStart), 0x28);
			
			// Equip the new badge
			ttyd::battle::_EquipItem(reinterpret_cast<void *>(ActorAddress), 
				1, static_cast<int16_t>(tempMenuSecondaryValue));
			break;
		}
		default:
		{
			break;
		}
	}
}

void setBattlesActorStatusValue(uint32_t currentMenuOption)
{
	uint32_t ActorAddress = reinterpret_cast<uint32_t>(getActorPointer(MenuVar.MenuSelectedOption));
	if (!ActorAddress)
	{
		return;
	}
	
	uint32_t Counter = 0;
	if (currentMenuOption >= 1) // Sleep flags
	{
		Counter++;
		
		if (currentMenuOption >= 18) // Can't use items
		{
			Counter++;
			
			if (currentMenuOption >= 20) // Explosion turns left
			{
				Counter++;
			}
		}
	}
	
	*reinterpret_cast<int8_t *>(ActorAddress + (
		0x118 + currentMenuOption + Counter)) = MenuVar.MenuSecondaryValue;
}

ttyd::party::PartyMembers getSelectedOptionPartnerValue()
{
	uint32_t tempSelectedOption = MenuVar.SelectedOption;
	uint32_t tempCurrentMenuOption = MenuVar.CurrentMenuOption;
	uint32_t tempOption;
	
	if (tempSelectedOption == 0)
	{
		tempOption = tempCurrentMenuOption + 1;
	}
	else
	{
		tempOption = tempSelectedOption;
	}
	
	switch (tempOption)
	{
		case 1:
		{
			return ttyd::party::PartyMembers::kGoombella;
		}
		case 2:
		{
			return ttyd::party::PartyMembers::kKoops;
		}
		case 3:
		{
			return ttyd::party::PartyMembers::kFlurrie;
		}
		case 4:
		{
			return ttyd::party::PartyMembers::kYoshi;
		}
		case 5:
		{
			return ttyd::party::PartyMembers::kVivian;
		}
		case 6:
		{
			return ttyd::party::PartyMembers::kBobbery;
		}
		case 7:
		{
			return ttyd::party::PartyMembers::kMsMowz;
		}
		default:
		{
			return ttyd::party::PartyMembers::kNone;
		}
	}
}

void *getPartnerEnabledAddress()
{
	uint32_t PouchPtr = reinterpret_cast<uint32_t>(ttyd::mario_pouch::pouchGetPtr());
	uint8_t CurrentPartnerValue = static_cast<uint8_t>(getSelectedOptionPartnerValue());
	uint32_t PartnerEnabledAddress = PouchPtr + (CurrentPartnerValue * 0xE);
	return reinterpret_cast<void *>(PartnerEnabledAddress);
}

bool checkIfPartnerOutSelected()
{
	uint32_t PartnerPointer = reinterpret_cast<uint32_t>(getPartnerPointer());
	if (!PartnerPointer)
	{
		return false;
	}
	
	ttyd::party::PartyMembers CurrentPartnerOut = *reinterpret_cast<ttyd::party::PartyMembers *>(PartnerPointer + 0x31);
	if (CurrentPartnerOut == ttyd::party::PartyMembers::kNone)
	{
		return false;
	}
	
	return CurrentPartnerOut == getSelectedOptionPartnerValue();
}

void setAddByIdValue(void *address)
{
	int32_t tempMenuSecondaryValue = MenuVar.MenuSecondaryValue;
	*reinterpret_cast<int16_t *>(
		reinterpret_cast<uint32_t>(address)) = tempMenuSecondaryValue;
	
	recheckUpgradesBattles(tempMenuSecondaryValue);
	resetImportantItemsPauseMenu();
}

void setAddByIconValue(void *address)
{
	int32_t UpperAndLowerBounds[2];
	getUpperAndLowerBounds(UpperAndLowerBounds, MenuVar.MenuSelectedOption);
	
	int32_t LowerBound 					= UpperAndLowerBounds[0];
	uint32_t tempSecondaryMenuOption 	= MenuVar.SecondaryMenuOption;
	int32_t NewItem = LowerBound + tempSecondaryMenuOption;
	
	*reinterpret_cast<int16_t *>(
		reinterpret_cast<uint32_t>(address)) = NewItem;
	
	recheckUpgradesBattles(NewItem);
	resetImportantItemsPauseMenu();
}

void duplicateCurrentItem(void *address)
{
	uint32_t Address_and_Size[2];
	uint32_t *tempArray	= getPouchAddressAndSize(Address_and_Size);
	
	if (!tempArray)
	{
		return;
	}
	
	uint32_t tempAddress = tempArray[0];
	
	int16_t CurrentItem = *reinterpret_cast<int16_t *>(
		tempAddress + (MenuVar.CurrentMenuOption * 0x2));
	
	*reinterpret_cast<int16_t *>(
		reinterpret_cast<uint32_t>(address)) = CurrentItem;
	
	resetImportantItemsPauseMenu();
}

void deleteItem()
{
	uint32_t Address_and_Size[2];
	uint32_t *tempArray	= getPouchAddressAndSize(Address_and_Size);
	
	if (!tempArray)
	{
		return;
	}
	
	uint32_t tempAddress 	= tempArray[0];
	uint32_t tempSize 		= tempArray[1];
	
	uint32_t tempCurrentMenuOption = MenuVar.CurrentMenuOption;
	int16_t CurrentItem = *reinterpret_cast<int16_t *>(
		tempAddress + (tempCurrentMenuOption * 0x2));
	
	for (uint32_t i = tempCurrentMenuOption; i < (tempSize - 1); i++)
	{
		*reinterpret_cast<int16_t *>(tempAddress + 
			(tempCurrentMenuOption * 0x2)) = 
				*reinterpret_cast<int16_t *>(tempAddress + 
					((tempCurrentMenuOption + 1) * 0x2));
		
		tempAddress += 0x2;
	}
	
	*reinterpret_cast<int16_t *>(tempAddress + 
		(tempCurrentMenuOption * 0x2)) = 0;
	
	recheckUpgradesBattles(CurrentItem);
	resetImportantItemsPauseMenu();
}

int32_t changeItem()
{
	uint32_t Address_and_Size[2];
	uint32_t *tempArray	= getPouchAddressAndSize(Address_and_Size);
	
	if (!tempArray)
	{
		return NULL_PTR;
	}
	
	uint32_t tempAddress 			= tempArray[0];
	
	uint32_t CurrentItemAddress 	= tempAddress + (MenuVar.CurrentMenuOption * 0x2);
	int16_t CurrentItem 			= *reinterpret_cast<int16_t *>(CurrentItemAddress);
	
	if (CurrentItem == 0)
	{
		return INVENTORY_EMPTY;
	}
	
	if (MenuVar.SelectedOption == CHANGE_BY_ID)
	{
		setAddByIdValue(reinterpret_cast<void *>(CurrentItemAddress));
	}
	else // MenuVar.SelectedOption == CHANGE_BY_ICON
	{
		setAddByIconValue(reinterpret_cast<void *>(CurrentItemAddress));
	}
	
	// Close the menu being used to select the new item
	MenuVar.MenuSelectionStates = 0;
	return 0;
}

int32_t resolveFade(uint32_t index)
{
	// Check if the current fade is active
	ttyd::fadedrv::FadeEntry *FadeEntry = &ttyd::fadedrv::gpFadeWork->entry[index];
	
	bool FadeIsActive = FadeEntry->flags & (1 << 0); // Check if 0 bit is active
	if (!FadeIsActive)
	{
		return FADE_NOT_ACTIVE;
	}
	
	// Check if the selected fade needs to be resolved
	ttyd::fadedrv::FadeType Type = FadeEntry->fadeType;
	
	switch (Type)
	{
		case ttyd::fadedrv::FadeType::kBlackMarioHeadFadeOut:
		{
			Type = ttyd::fadedrv::FadeType::kBlackMarioHeadFadeIn;
			break;
		}
		case ttyd::fadedrv::FadeType::kBlackCircleFadeOut:
		{
			Type = ttyd::fadedrv::FadeType::kBlackCircleFadeIn;
			break;
		}
		case ttyd::fadedrv::FadeType::kBlackCircleFadeOut2:
		{
			Type = ttyd::fadedrv::FadeType::kBlackCircleFadeIn2;
			break;
		}
		case ttyd::fadedrv::FadeType::kBlackCircleFadeOut3:
		{
			Type = ttyd::fadedrv::FadeType::kBlackCircleFadeIn3;
			break;
		}
		case ttyd::fadedrv::FadeType::kBlackFadeOut:
		{
			Type = ttyd::fadedrv::FadeType::kBlackFadeIn;
			break;
		}
		case ttyd::fadedrv::FadeType::kBlackFadeOut2:
		{
			Type = ttyd::fadedrv::FadeType::kBlackFadeIn2;
			break;
		}
		case ttyd::fadedrv::FadeType::kBlackStatic:
		case ttyd::fadedrv::FadeType::kBlackFadeOut3:
		{
			Type = ttyd::fadedrv::FadeType::kBlackFadeIn3;
			break;
		}
		case ttyd::fadedrv::FadeType::kWhiteFadeOut:
		{
			Type = ttyd::fadedrv::FadeType::kWhiteFadeIn;
			break;
		}
		case ttyd::fadedrv::FadeType::kWhiteFadeOut2:
		{
			Type = ttyd::fadedrv::FadeType::kWhiteFadeIn2;
			break;
		}
		case ttyd::fadedrv::FadeType::kWhiteFadeOut3:
		{
			Type = ttyd::fadedrv::FadeType::kWhiteFadeIn3;
			break;
		}
		case ttyd::fadedrv::FadeType::kCurtainPullDown:
		{
			Type = ttyd::fadedrv::FadeType::kCurtainPullUp;
			break;
		}
		case ttyd::fadedrv::FadeType::kCurtainPullLeft:
		{
			Type = ttyd::fadedrv::FadeType::kCurtainPullRight;
			break;
		}
		case ttyd::fadedrv::FadeType::kCurtainStatic:
		case ttyd::fadedrv::FadeType::kCurtainPullLeft2:
		{
			Type = ttyd::fadedrv::FadeType::kCurtainPullRight2;
			break;
		}
		case ttyd::fadedrv::FadeType::kTitleScreenCurtainClose:
		case ttyd::fadedrv::FadeType::kTitleScreenCurtainClosedStatic:
		{
			Type = ttyd::fadedrv::FadeType::kTitleScreenCurtainOpen;
			break;
		}
		case ttyd::fadedrv::FadeType::kTitleScreenCurtainOpen:
		case ttyd::fadedrv::FadeType::kTitleScreenCurtainOpenStatic:
		case ttyd::fadedrv::FadeType::kTitleScreenCurtainAppearGradual:
		{
			Type = ttyd::fadedrv::FadeType::kTitleScreenCurtainLeaveGradual;
			break;
		}
		case ttyd::fadedrv::FadeType::kTitleScreenCurtainOpen2:
		case ttyd::fadedrv::FadeType::kTitleScreenCurtainOpenStatic2:
		case ttyd::fadedrv::FadeType::kTitleScreenCurtainAppearGradual2:
		{
			Type = ttyd::fadedrv::FadeType::kTitleScreenCurtainLeaveGradual2;
			break;
		}
		case ttyd::fadedrv::FadeType::kTitleScreenCurtainClosed2:
		case ttyd::fadedrv::FadeType::kTitleScreenCurtainClosedStatic2:
		{
			Type = ttyd::fadedrv::FadeType::kTitleScreenCurtainOpen2;
			break;
		}
		case ttyd::fadedrv::FadeType::kEnterPipeFromUp:
		{
			Type = ttyd::fadedrv::FadeType::kExitPipeFromUp;
			break;
		}
		case ttyd::fadedrv::FadeType::kEnterPipeFromDown:
		{
			Type = ttyd::fadedrv::FadeType::kExitPipeFromDown;
			break;
		}
		case ttyd::fadedrv::FadeType::kEnterPipeFromLeft:
		{
			Type = ttyd::fadedrv::FadeType::kExitPipeFromLeft;
			break;
		}
		case ttyd::fadedrv::FadeType::kEnterPipeFromRight:
		{
			Type = ttyd::fadedrv::FadeType::kExitPipeFromRight;
			break;
		}
		case ttyd::fadedrv::FadeType::kFreezeScreen:
		{
			Type = ttyd::fadedrv::FadeType::kUnfreezeScreenPullTopRight;
			break;
		}
		case ttyd::fadedrv::FadeType::kMarioHeadFadeOut:
		case ttyd::fadedrv::FadeType::kMarioHeadStaticBlinking:
		{
			Type = ttyd::fadedrv::FadeType::kMarioHeadFadeIn;
			break;
		}
		case ttyd::fadedrv::FadeType::kPeachHeadFadeOut:
		case ttyd::fadedrv::FadeType::kPeachHeadStatic:
		{
			Type = ttyd::fadedrv::FadeType::kPeachHeadFadeIn;
			break;
		}
		case ttyd::fadedrv::FadeType::kBowserHeadFadeOut:
		case ttyd::fadedrv::FadeType::kBowserHeadStatic:
		{
			Type = ttyd::fadedrv::FadeType::kBowserHeadFadeIn;
			break;
		}
		case ttyd::fadedrv::FadeType::kCurtainPullLeft3:
		{
			Type = ttyd::fadedrv::FadeType::kCurtainPullRight3;
			break;
		}
		default:
		{
			// The selected fade does not need to be resolved
			return FADE_DONT_RESOLVE;
		}
	}
	
	// Resolve the selected fade
	uint32_t Color = 0x000000FF;
	int32_t Duration = 0;
	
	ttyd::fadedrv::fadeEntry(Type, Duration, reinterpret_cast<uint8_t *>(&Color));
	return FADE_RESOLVE_SUCCESS;
}

int32_t getGW(uint32_t gw)
{
	ttyd::evtmgr::EvtWork *EventWork = ttyd::evtmgr::evtGetWork();
	return EventWork->gwData[gw];
}

void setGW(uint32_t gw, uint32_t value)
{
	ttyd::evtmgr::EvtWork *EventWork = ttyd::evtmgr::evtGetWork();
	EventWork->gwData[gw] = value;
}

bool getGF(uint32_t gf)
{
	ttyd::evtmgr::EvtWork *EventWork = ttyd::evtmgr::evtGetWork();
	uint32_t Size = sizeof(uint32_t) * 8;
	return (EventWork->gfData[gf / Size] >> (gf % Size)) & 1;
}

void setGF(uint32_t gf)
{
	ttyd::evtmgr::EvtWork *EventWork = ttyd::evtmgr::evtGetWork();
	uint32_t Size = sizeof(uint32_t) * 8;
	EventWork->gfData[gf / Size] ^= (1 << (gf % Size)); // Toggle the bit
}

int32_t getGlobalFlagValue(uint32_t currentMenu, uint32_t flag)
{
	switch (currentMenu)
	{
		case SET_GSW:
		{
			return static_cast<int32_t>(ttyd::swdrv::swByteGet(flag));
		}
		case SET_GSWF:
		{
			return static_cast<int32_t>(ttyd::swdrv::swGet(flag));
		}
		case SET_GW:
		{
			return getGW(flag);
		}
		case SET_GF:
		{
			return static_cast<int32_t>(getGF(flag));
		}
		case SET_LSW:
		{
			return static_cast<int32_t>(ttyd::swdrv::_swByteGet(flag));
		}
		case SET_LSWF:
		{
			return static_cast<int32_t>(ttyd::swdrv::_swGet(flag));
		}
		default:
		{
			return 0;
		}
	}
}

void cheatClearAreaFlags(uint32_t currentMenuOption)
{
	switch (currentMenuOption)
	{
		case AREA_GOR:
		{
			// Clear the regular flags
			clearGSWFsRange(1175, 1250);
			
			// Clear the Shine Sprites
			clearGSWFsRange(5525, 5529);
			
			// Clear the Star Pieces
			clearGSWFsRange(5567, 5584);
			
			// Clear misc flags not part of the standard gor memory area
			// Set up the flags to clear
			static const uint16_t FlagsToClear[] = 
			{
				9, // Post-Chapter 3 Koopie Koo Email
				10, // Post-Chapter 4 Petuni Email
				11, // Email from Boo after Chapter 5 Intermission
				14, // Real Zip Toad Email after Chapter 6
				20, // RDM #4 Email after Chapter 5
				23, // Koopook Email after Chapter 2
				24, // Email from Pine T. Jr. after Chapter 3
				25, // Email from Jolene after Zess Trouble
				26, // Email from Bub-ulber after Mowz Trouble
				27, // Eve Email after Chapter 6
			};
			
			uint32_t Size = sizeof(FlagsToClear) / sizeof(FlagsToClear[0]);
			for (uint32_t i = 0; i < Size; i++)
			{
				ttyd::swdrv::swClear(FlagsToClear[i]);
			}
			break;
		}
		case AREA_TIK:
		{
			// Clear the regular flags
			clearGSWFsRange(1325, 1371);
			
			// Clear the Shine Sprites
			clearGSWFsRange(5530, 5537);
			
			// Clear the Star Pieces
			clearGSWFsRange(5585, 5597);
			
			// Clear misc flags not part of the standard tik memory area
			// Set up the flags to clear
			static const uint16_t FlagsToClear[] = 
			{
				16, // RDM #7 Email after Chapter 7
			};
			
			uint32_t Size = sizeof(FlagsToClear) / sizeof(FlagsToClear[0]);
			for (uint32_t i = 0; i < Size; i++)
			{
				ttyd::swdrv::swClear(FlagsToClear[i]);
			}
			break;
		}
		case AREA_HEI:
		{
			// Clear the regular flags
			clearGSWFsRange(1774, 1806);
			
			// Clear the Star Pieces
			clearGSWFsRange(5598, 5600);
			break;
		}
		case AREA_NOK:
		{
			// Clear the regular flags
			clearGSWFsRange(1624, 1629);
			
			// Clear the Star Pieces
			clearGSWFsRange(5601, 5602);
			
			// Clear misc flags not part of the standard nok memory area
			// Set up the flags to clear
			static const uint16_t FlagsToClear[] = 
			{
				7, // Frankly Email at right side of town
			};
			
			uint32_t Size = sizeof(FlagsToClear) / sizeof(FlagsToClear[0]);
			for (uint32_t i = 0; i < Size; i++)
			{
				ttyd::swdrv::swClear(FlagsToClear[i]);
			}
			break;
		}
		case AREA_GON:
		{
			// Clear the regular flags
			clearGSWFsRange(1476, 1511);
			
			// Clear the Shine Sprites
			clearGSWFsRange(5538, 5540);
			
			// Clear the Star Pieces
			clearGSWFsRange(5603, 5607);
			break;
		}
		case AREA_WIN:
		{
			// Clear the regular flags
			clearGSWFsRange(2675, 2687);
			
			// Clear the Shine Sprites
			clearGSWFsRange(5541, 5541);
			
			// Clear the Star Pieces
			clearGSWFsRange(5608, 5611);
			
			// Clear misc flags not part of the standard win memory area
			// Set up the flags to clear
			static const uint16_t FlagsToClear[] = 
			{
				17, // RDM #1 Email after getting Flurrie
			};
			
			uint32_t Size = sizeof(FlagsToClear) / sizeof(FlagsToClear[0]);
			for (uint32_t i = 0; i < Size; i++)
			{
				ttyd::swdrv::swClear(FlagsToClear[i]);
			}
			break;
		}
		case AREA_MRI:
		{
			// Clear the regular flags
			clearGSWFsRange(2825, 2885);
			
			// Clear the Shine Sprites
			clearGSWFsRange(5542, 5545);
			
			// Clear the Star Pieces
			clearGSWFsRange(5612, 5617);
			break;
		}
		case AREA_TOU:
		{
			// Clear the regular flags
			clearGSWFsRange(2374, 2533);
			
			// Clear the Shine Sprites
			clearGSWFsRange(5546, 5547);
			
			// Clear the Star Pieces
			clearGSWFsRange(5618, 5627);
			
			// Clear misc flags not part of the standard tou memory area
			// Set up the flags to clear
			static const uint16_t FlagsToClear[] = 
			{
				18, // RDM #2 Email after getting Egg
				250, // RDM #2 Email after getting Egg
				251, // RDM #4 Email after Chapter 5
			};
			
			uint32_t Size = sizeof(FlagsToClear) / sizeof(FlagsToClear[0]);
			for (uint32_t i = 0; i < Size; i++)
			{
				ttyd::swdrv::swClear(FlagsToClear[i]);
			}
			break;
		}
		case AREA_USU:
		{
			// Clear the regular flags
			clearGSWFsRange(1925, 1939);
			
			// Clear the Star Pieces
			clearGSWFsRange(5628, 5630);
			break;
		}
		case AREA_GRA:
		{
			// Clear the regular flags
			clearGSWFsRange(2075, 2091);
			
			// Clear the Shine Sprites
			clearGSWFsRange(5548, 5548);
			
			// Clear the Star Pieces
			clearGSWFsRange(5631, 5633);
			break;
		}
		case AREA_JIN:
		{
			// Clear the regular flags
			clearGSWFsRange(2226, 2241);
			
			// Clear the Shine Sprites
			clearGSWFsRange(5549, 5551);
			
			// Clear the Star Pieces
			clearGSWFsRange(5634, 5637);
			break;
		}
		case AREA_MUJ:
		{
			// Clear the regular flags
			clearGSWFsRange(3126, 3158);
			
			// Clear the Shine Sprites
			clearGSWFsRange(5552, 5553);
			
			// Clear the Star Pieces
			clearGSWFsRange(5638, 5643);
			break;
		}
		case AREA_DOU:
		{
			// Clear the regular flags
			clearGSWFsRange(2975, 2994);
			
			// Clear the Shine Sprites
			clearGSWFsRange(5554, 5558);
			
			// Clear the Star Pieces
			clearGSWFsRange(5644, 5647);
			break;
		}
		case AREA_HOM:
		{
			// Clear the regular flags
			clearGSWFsRange(3574, 3575);
			
			// Clear misc flags not part of the standard hom memory area
			// Set up the flags to clear
			static const uint16_t FlagsToClear[] = 
			{
				13, // King K. Email after fixing drawbridge
			};
			
			uint32_t Size = sizeof(FlagsToClear) / sizeof(FlagsToClear[0]);
			for (uint32_t i = 0; i < Size; i++)
			{
				ttyd::swdrv::swClear(FlagsToClear[i]);
			}
			break;
		}
		case AREA_RSH:
		{
			// Clear the regular flags
			clearGSWFsRange(3425, 3462);
			
			// Clear the Shine Sprites
			clearGSWFsRange(5559, 5560);
			
			// Clear the Star Pieces
			clearGSWFsRange(5648, 5652);
			
			// Clear misc flags not part of the standard rsh memory area
			// Set up the flags to clear
			static const uint16_t FlagsToClear[] = 
			{
				12, // Email from Pa-Patch on Day 3
				21, // RDM #5 Email on second day
			};
			
			uint32_t Size = sizeof(FlagsToClear) / sizeof(FlagsToClear[0]);
			for (uint32_t i = 0; i < Size; i++)
			{
				ttyd::swdrv::swClear(FlagsToClear[i]);
			}
			break;
		}
		case AREA_EKI:
		{
			// Clear the regular flags
			clearGSWFsRange(3725, 3754);
			
			// Clear the Shine Sprites
			clearGSWFsRange(5561, 5562);
			
			// Clear the Star Pieces
			clearGSWFsRange(5653, 5653);
			break;
		}
		case AREA_PIK:
		{
			// Clear the regular flags
			clearGSWFsRange(3276, 3279);
			
			// Clear the Shine Sprites
			clearGSWFsRange(5563, 5564);
			
			// Clear the Star Pieces
			clearGSWFsRange(5654, 5657);
			
			// Clear misc flags not part of the standard pik memory area
			// Set up the flags to clear
			static const uint16_t FlagsToClear[] = 
			{
				22, // RDM #6 Email after Goldbob cannon
				252, // RDM #6 Email after Goldbob cannon
			};
			
			uint32_t Size = sizeof(FlagsToClear) / sizeof(FlagsToClear[0]);
			for (uint32_t i = 0; i < Size; i++)
			{
				ttyd::swdrv::swClear(FlagsToClear[i]);
			}
			break;
		}
		case AREA_BOM:
		{
			// Clear the regular flags
			clearGSWFsRange(3874, 3892);
			
			// Clear the Shine Sprites
			clearGSWFsRange(5565, 5566);
			
			// Clear the Star Pieces
			clearGSWFsRange(5658, 5663);
			
			// Clear misc flags not part of the standard bom memory area
			// Set up the flags to clear
			static const uint16_t FlagsToClear[] = 
			{
				15, // Rawk Hawk Email after General White quest
			};
			
			uint32_t Size = sizeof(FlagsToClear) / sizeof(FlagsToClear[0]);
			for (uint32_t i = 0; i < Size; i++)
			{
				ttyd::swdrv::swClear(FlagsToClear[i]);
			}
			break;
		}
		case AREA_MOO:
		{
			// Clear the regular flags
			clearGSWFsRange(4025, 4039);
			
			// Clear the Star Pieces
			clearGSWFsRange(5664, 5664);
			break;
		}
		case AREA_AJI:
		{
			// Clear the regular flags
			clearGSWFsRange(4175, 4217);
			
			// Clear the Star Pieces
			clearGSWFsRange(5665, 5666);
			break;
		}
		case AREA_LAS:
		{
			// Clear the regular flags
			clearGSWFsRange(4326, 4394);
			break;
		}
		case AREA_JON:
		{
			// Clear the regular flags
			clearGSWFsRange(5075, 5085);
			break;
		}
		default:
		{
			return;
		}
	}
}

const char *getMapFromIndex(int32_t index)
{
	int32_t MaxIndex = 271;
	if (index >= MaxIndex)
	{
		return "title";
	}
	else if (index <= 0)
	{
		return "aaa_00";
	}
	else
	{
		// Skip invalid values
		int32_t NewIndex = index - 1;
		uint32_t Counter = 0;
		
		if (NewIndex >= 269) // dig_00
		{
			Counter += 32;
		}
		else if (NewIndex >= 266) // qiz_00
		{
			Counter += 24;
		}
		else if (NewIndex >= 90) // hei_bt00
		{
			Counter += 16;
		}
		else if (NewIndex >= 74) // gon_bt00
		{
			Counter += 8;
		}
		
		uint32_t WorldDataPointer = reinterpret_cast<uint32_t>(ttyd::mapdata::worldData.unk_18);
		
		return reinterpret_cast<const char *>(
			*reinterpret_cast<uint32_t *>(
				*reinterpret_cast<uint32_t *>(
					WorldDataPointer + 0x10) + Counter + (NewIndex * 0x8)));
	}
}

int32_t getMapIndex()
{
	int32_t MaxIndex = 271;
	for (int32_t i = 0; i <= MaxIndex; i++)
	{
		if (compareStringToNextMap(getMapFromIndex(i)))
		{
			return i;
		}
	}
	return -1;
}

const char *getPartyName(ttyd::party::PartyMembers id)
{
	switch (id)
	{
		case ttyd::party::PartyMembers::kNone:
		{
			return "None";
		}
		case ttyd::party::PartyMembers::kGoombella:
		{
			return "Goombella";
		}
		case ttyd::party::PartyMembers::kKoops:
		{
			return "Koops";
		}
		case ttyd::party::PartyMembers::kBobbery:
		{
			return "Bobbery";
		}
		case ttyd::party::PartyMembers::kYoshi:
		{
			return "Yoshi";
		}
		case ttyd::party::PartyMembers::kFlurrie:
		{
			return "Flurrie";
		}
		case ttyd::party::PartyMembers::kVivian:
		{
			return "Vivian";
		}
		case ttyd::party::PartyMembers::kMsMowz:
		{
			return "Ms. Mowz";
		}
		case ttyd::party::PartyMembers::kEgg:
		{
			return "Egg";
		}
		case ttyd::party::PartyMembers::kFlavio:
		{
			return "Flavio";
		}
		case ttyd::party::PartyMembers::kPunio:
		{
			return "Punio";
		}
		case ttyd::party::PartyMembers::kFrankly:
		{
			return "Frankly";
		}
		case ttyd::party::PartyMembers::kGus:
		{
			return "Gus";
		}
		case ttyd::party::PartyMembers::kGoombellaFollower:
		{
			return "Goombella (Follower)";
		}
		case ttyd::party::PartyMembers::kKoopsFollower:
		{
			return "Koops (Follower)";
		}
		case ttyd::party::PartyMembers::kBobberyFollower:
		{
			return "Bobbery (Follower)";
		}
		case ttyd::party::PartyMembers::kYoshiFollower:
		{
			return "Yoshi (Follower)";
		}
		case ttyd::party::PartyMembers::kFlurrieFollower:
		{
			return "Flurrie (Follower)";
		}
		case ttyd::party::PartyMembers::kVivianFollower:
		{
			return "Vivian (Follower)";
		}
		case ttyd::party::PartyMembers::kMsMowzFollower:
		{
			return "Ms. Mowz (Follower)";
		}
		default:
		{
			return "";
		}
	}
}

int32_t getTotalStageEvents()
{
	int32_t NumberOfStages = ttyd::event::eventStgNum();
	int32_t TotalEvents = 0;
	
	for (int32_t i = 0; i < NumberOfStages; ++i)
	{
		const ttyd::event::EventStageDescription *Stage = ttyd::event::eventStgDtPtr(i);
		TotalEvents += Stage->eventCount;
	}
	
	return TotalEvents;
}

bool indexToStageAndEvent(int32_t index, int32_t arrayOut[2])
{
	if ((index < 0) || (index >= getTotalStageEvents()))
	{
		return false;
	}

	int32_t NumberOfStages = ttyd::event::eventStgNum();
	int32_t StageStartIndex = 0;
	
	for (int32_t s = 0; s < NumberOfStages; ++s)
	{
		const ttyd::event::EventStageDescription *TargetStage = ttyd::event::eventStgDtPtr(s);
		int32_t StageEventCount = TargetStage->eventCount;
		
		if ((index >= StageStartIndex) && (index < (StageStartIndex + StageEventCount)))
		{
			arrayOut[0] = s; // StageId
			arrayOut[1] = index - StageStartIndex; // EventId
			return true;
		}

		StageStartIndex += StageEventCount;
	}
	
	return false;
}

uint16_t getGsw0ForEvent(int32_t stageId, int32_t eventId)
{
	if ((stageId <= 0) && (eventId <= 0))
	{
		 return 0;
	}
	
	if (!checkForValidStageAndEvent(stageId, eventId))
	{
		return 0;
	}
	
	const ttyd::event::EventStageDescription &CurrentStage = *ttyd::event::eventStgDtPtr(stageId);
	uint16_t CurrentSequencePosition = CurrentStage.pEvents[eventId].gsw0;
	uint16_t NewSequencePosition;
	
	while (1)
	{
		if (eventId <= 0)
		{
			stageId--;
			const ttyd::event::EventStageDescription &NewStage = *ttyd::event::eventStgDtPtr(stageId);
			eventId = NewStage.eventCount - 1;
			NewSequencePosition = NewStage.pEvents[eventId].gsw0;
		}
		else
		{
			const ttyd::event::EventStageDescription &NewStage = *ttyd::event::eventStgDtPtr(stageId);
			eventId--;
			NewSequencePosition = NewStage.pEvents[eventId].gsw0;
		}
		
		if (NewSequencePosition < CurrentSequencePosition)
		{
			return NewSequencePosition;
		}
		
		if ((stageId <= 0) && (eventId <= 0))
		{
			 return 0;
		}
	}
}

bool checkForValidStageAndEvent(int32_t stageId, int32_t eventId)
{
	if ((stageId < 0) || (stageId >= ttyd::event::eventStgNum()))
	{
		return false;
	}
	
	const ttyd::event::EventStageDescription *TargetStage = ttyd::event::eventStgDtPtr(stageId);
	if ((eventId < 0) || (eventId >= TargetStage->eventCount))
	{
		return false;
	}
	
	return true;
}

bool getEventDetails(int32_t index, WarpByEventDetailsStruct *warpByEventDetails)
{
	int32_t StageAndEventIds[2];
	if (!indexToStageAndEvent(index, StageAndEventIds))
	{
		return false;
	}
	
	int32_t StageId = StageAndEventIds[0];
	int32_t EventId = StageAndEventIds[1];
	
	if (!checkForValidStageAndEvent(StageId, EventId))
	{
		return false;
	}
	
	const ttyd::event::EventStageDescription *TargetStage = ttyd::event::eventStgDtPtr(StageId);
	const ttyd::event::EventStageEventDescription *TargetEvent = &TargetStage->pEvents[EventId];
	
	#ifdef TTYD_JP
	char *StageNameBuffer = warpByEventDetails->Stage;
	if (!getStageString(StageNameBuffer, getGsw0ForEvent(StageId, EventId)))
	{
		StageNameBuffer[0] = '\0';
	}
	
	if (index < static_cast<int32_t>(WarpsEventNamesSize))
	{
		warpByEventDetails->Event = WarpsEventNames[index];
	}
	else
	{
		warpByEventDetails->Event = "";
	}
	#else
	warpByEventDetails->Stage = 			TargetStage->nameEn;
	warpByEventDetails->Event = 			TargetEvent->nameEn;
	#endif
	
	warpByEventDetails->Partner = 			getPartyName(TargetEvent->partyId[0]);
	warpByEventDetails->Follower = 			getPartyName(TargetEvent->partyId[1]);
	warpByEventDetails->Map = 				TargetEvent->map;
	warpByEventDetails->Bero = 				TargetEvent->bero;
	warpByEventDetails->SequencePosition = 	getGsw0ForEvent(StageId, EventId);
	return true;
}

void *initStageEvents()
{
	if (!WarpByEvent.ShouldInit)
	{
		// The overwritten instruction sets r3 to the global work pointer, so return the global work pointer
		return ttyd::mariost::globalWorkPointer;
	}
	
	WarpByEvent.ShouldInit = false;
	int32_t StageAndEventIds[2];
	
	int32_t CurrentIndex = WarpByEvent.CurrentIndex;
	if (!indexToStageAndEvent(CurrentIndex, StageAndEventIds))
	{
		// The overwritten instruction sets r3 to the global work pointer, so return the global work pointer
		return ttyd::mariost::globalWorkPointer;
	}
	
	int32_t StageId = StageAndEventIds[0];
	int32_t EventId = StageAndEventIds[1];
	
	if (!checkForValidStageAndEvent(StageId, EventId))
	{
		// The overwritten instruction sets r3 to the global work pointer, so return the global work pointer
		return ttyd::mariost::globalWorkPointer;
	}
	
	// Back up the inventory if desired
	WarpByEventInventoryStruct *WarpByEventInventory = nullptr;
	int16_t *StandardInventoryStart = nullptr;
	int16_t *BadgesInventoryStart = nullptr;
	
	bool ShouldKeepInventory = WarpByEvent.ShouldKeepInventory;
	if (ShouldKeepInventory)
	{
		WarpByEventInventory = new WarpByEventInventoryStruct;
		
		uint32_t PouchPtrRaw = reinterpret_cast<uint32_t>(ttyd::mario_pouch::pouchGetPtr());
		StandardInventoryStart = reinterpret_cast<int16_t *>(PouchPtrRaw + 0x192);
		BadgesInventoryStart = reinterpret_cast<int16_t *>(PouchPtrRaw + 0x1FA);
		
		// Back up the standard inventory
		memcpy(WarpByEventInventory->StandardItems, 
			StandardInventoryStart, 
			sizeof(WarpByEventInventory->StandardItems));
		
		// Back up the badges
		memcpy(WarpByEventInventory->Badges, 
			BadgesInventoryStart, 
			sizeof(WarpByEventInventory->Badges));
	}
	
	// Clear all current states
	ttyd::swdrv::swInit();
	
	// Run the init functions for each event, up to and including the current one
	for (int32_t s = 0; s <= StageId; ++s)
	{
		const ttyd::event::EventStageDescription *Stage = ttyd::event::eventStgDtPtr(s);
		int32_t lastEvent = (s == StageId ? EventId : (Stage->eventCount - 1));
		
		for (int32_t e = 0; e <= lastEvent; ++e)
		{
			void (*initFunction)() = Stage->pEvents[e].pfnInit;
			if (initFunction)
			{
				initFunction();
			}
		}
	}
	
	// Restore the standard inventory if desired
	if (ShouldKeepInventory)
	{
		auto addItemToInventory = [](int16_t *inventoryStart, uint32_t totalSlots, int16_t item)
		{
			for (uint32_t i = 0; i < totalSlots; i++)
			{
				if (inventoryStart[i] == 0)
				{
					inventoryStart[i] = item;
					break;
				}
			}
		};
		
		// Restore the standard inventory
		memcpy(StandardInventoryStart, 
			WarpByEventInventory->StandardItems, 
			sizeof(WarpByEventInventory->StandardItems));
		
		// Restore the badges
		memcpy(BadgesInventoryStart, 
			WarpByEventInventory->Badges, 
			sizeof(WarpByEventInventory->Badges));
		
		delete WarpByEventInventory;
		
		// Add any items or badges that the player should have gotten
		
		/* If the player warped to an event that is 58 or higher, then they should be given 
			Attack FX R, Power Bounce, Multibounce, and Power Smash. Attack FX R should also
			be equipped if going to fight Hooktail.*/
		if (CurrentIndex >= 58)
		{
			// Set up the badges to give
			static const int16_t BadgesToGive[] = 
			{
				ttyd::item_data::Item::AttackFXR,
				ttyd::item_data::Item::PowerBounce,
				ttyd::item_data::Item::Multibounce,
				ttyd::item_data::Item::PowerSmash,
			};
			
			// Give the badges
			// Only give the badges if the player does not have them already
			uint32_t Size = sizeof(BadgesToGive) / sizeof(BadgesToGive[0]);
			for (uint32_t i = 0; i < Size; i++)
			{
				int16_t CurrentBadge = BadgesToGive[i];
				if (!checkIfHaveItem(CurrentBadge))
				{
					addItemToInventory(BadgesInventoryStart, 200, CurrentBadge);
				}
			}
			
			// Unequip Attack FX R if it's equipped, as the new badge order may cause it to break
			ttyd::mario_pouch::pouchUnEquipBadgeID(ttyd::item_data::Item::AttackFXR);
			
			// Re-equip Attack FX R if currently going to fight Hooktail
			if (CurrentIndex == 58)
			{
				ttyd::mario_pouch::pouchEquipBadgeID(ttyd::item_data::Item::AttackFXR);
			}
		}
		
		/* If the player warped to the event with the index of 278, then they should be 
			given a Coconut, as it is used to get the Chuckola Cola from Flavio*/
		if (CurrentIndex == 278)
		{
			// Only give the Coconut if the player doesn't have one already
			if (!checkIfHaveItem(ttyd::item_data::Item::Coconut))
			{
				uint32_t TotalSlots = 10;
				if (checkIfHaveItem(ttyd::item_data::Item::StrangeSack))
				{
					TotalSlots = 20;
				}
				
				addItemToInventory(StandardInventoryStart, TotalSlots, ttyd::item_data::Item::Coconut);
			}
		};
	}
	
	// Set specific flags if desired
	if (WarpByEvent.ShouldSetFlags)
	{
		// Set up the flags to set
		static const uint16_t FlagsToSet[] = 
		{
			0, // Shop tutorial
			37, // Got first email
			38, // Read any email
			233, // Save block tutorial
			234, // Recovery block tutorial
			235, // Item tutorial
		};
		
		// Set the flags
		uint32_t Size = sizeof(FlagsToSet) / sizeof(FlagsToSet[0]);
		for (uint32_t i = 0; i < Size; i++)
		{
			ttyd::swdrv::swSet(FlagsToSet[i]);
		}
	}
	
	// Set the Sequence to the value for the current event
	setSequencePosition(getGsw0ForEvent(StageId, EventId));
	
	// Reset Mario's motion
	ttyd::mario_motion::marioChgMot(ttyd::mario_motion::MarioMotions::kNone);
	
	const ttyd::event::EventStageDescription *TargetStage = ttyd::event::eventStgDtPtr(StageId);
	const ttyd::event::EventStageEventDescription *TargetEvent = &TargetStage->pEvents[EventId];
	
	// Set Mario's motion
	switch (TargetEvent->entryMotionType)
	{
		case 0:
		{
			ttyd::mario_motion::marioChgStayMotion();
			break;
		}
		case 1:
		{
			ttyd::mario_motion::marioChgShipMotion();
			break;
		}
		default:
		{
			break;
		}
	}
	
	// Remove any partners and followers that are currently out from the overworld
	ttyd::mario_party::marioPartyKill();
	
	// Reset the previous partner and follower ids
	ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
	player->prevFollowerId[0] = ttyd::party::PartyMembers::kNone;
	player->prevFollowerId[1] = ttyd::party::PartyMembers::kNone;
	
	// Spawn the partner for the current event
	ttyd::party::PartyMembers PartnerId = TargetEvent->partyId[0];
	if (PartnerId != ttyd::party::PartyMembers::kNone)
	{
		spawnPartnerOrFollower(PartnerId);
	}
	
	// Spawn the follower for the current event
	ttyd::party::PartyMembers FollowerId = TargetEvent->partyId[1];
	if (FollowerId != ttyd::party::PartyMembers::kNone)
	{
		spawnPartnerOrFollower(FollowerId);
	}
	
	// Perform a full recovery for Mario and the partners
	ttyd::mario_pouch::pouchReviseMarioParam();
	ttyd::evt_pouch::evt_pouch_mario_recovery();
	
	/* Must call pouchRevisePartyParam to properly set each partner's stats, 
	otherwise they will each have a maximum of 10 HP */
	ttyd::mario_pouch::pouchRevisePartyParam();
	ttyd::evt_pouch::evt_pouch_all_party_recovery();
	
	// Make sure the values are properly changed when entering a battle
	ClearCacheForBattles.MarioStatsShouldBeCleared = true;
	ClearCacheForBattles.PartnerStatsShouldBeCleared = true;
	
	// Override any flags set in this function if the previous flags were locked
	lockFlags();
	
	// The overwritten instruction sets r3 to the global work pointer, so return the global work pointer
	return ttyd::mariost::globalWorkPointer;
}

#ifdef TTYD_JP
bool getStageString(char *stageNameBuffer, uint32_t sequencePosition)
{
	// Make sure the Sequence value is valid
	if (sequencePosition > 405)
	{
		return false;
	}
	
	if ((sequencePosition >= 0) && (sequencePosition <= 22))
	{
		strcpy(stageNameBuffer, "Opening");
		return true;
	}
	
	if ((sequencePosition >= 403) && (sequencePosition <= 405))
	{
		strcpy(stageNameBuffer, "Ending");
		return true;
	}
	
	uint32_t StageNumber;
	if ((sequencePosition >= 23) && (sequencePosition <= 70))
	{
		StageNumber = 1;
	}
	else if ((sequencePosition >= 71) && (sequencePosition <= 126))
	{
		StageNumber = 2;
	}
	else if ((sequencePosition >= 127) && (sequencePosition <= 177))
	{
		StageNumber = 3;
	}
	else if ((sequencePosition >= 178) && (sequencePosition <= 229))
	{
		StageNumber = 4;
	}
	else if ((sequencePosition >= 230) && (sequencePosition <= 281))
	{
		StageNumber = 5;
	}
	else if ((sequencePosition >= 282) && (sequencePosition <= 351))
	{
		StageNumber = 6;
	}
	else if ((sequencePosition >= 352) && (sequencePosition <= 381))
	{
		StageNumber = 7;
	}
	else // if ((sequencePosition >= 382) && (sequencePosition <= 402))
	{
		StageNumber = 8;
	}
	
	sprintf(stageNameBuffer,
		"Stage %" PRIu32,
		StageNumber);
	
	return true;
}
#endif

#ifdef TTYD_JP
bool getSequenceStageAndEvent(const char *arrayOut[2], char *stageNameBuffer, uint32_t sequencePosition)
#else
bool getSequenceStageAndEvent(const char *arrayOut[2], uint32_t sequencePosition)
#endif
{
	const char *StageName = nullptr;
	const char *EventName = nullptr;
	
	#ifdef TTYD_JP
	if (!getStageString(stageNameBuffer, sequencePosition))
	{
		return false;
	}
	
	StageName = stageNameBuffer;
	EventName = CheatsEventNames[sequencePosition];
	#else
	uint16_t tempSequencePosition = static_cast<uint16_t>(sequencePosition);
	int32_t NumberOfStages = ttyd::event::eventStgNum();
	bool FoundName = false;
	
	for (int32_t i = 0; i < NumberOfStages; ++i)
	{
		const ttyd::event::EventStageDescription *StageDesc = ttyd::event::eventStgDtPtr(i);
		for (int32_t j = 0; j < StageDesc->eventCount; ++j)
		{
			const ttyd::event::EventStageEventDescription &EventDesc = StageDesc->pEvents[j];
			if (EventDesc.gsw0 >= tempSequencePosition)
			{
				StageName = StageDesc->nameEn;
				EventName = EventDesc.nameEn;
				FoundName = true;
				break;
			}
		}
		
		if (FoundName)
		{
			break;
		}
	}
	
	if (!FoundName)
	{
		return false;
	}
	#endif
	
	arrayOut[0] = StageName;
	arrayOut[1] = EventName;
	return true;
}

double getStickAngle(int32_t stickXYOut[2])
{
	int32_t StickXInt = static_cast<int32_t>(ttyd::system::keyGetStickX(0));
	int32_t StickYInt = static_cast<int32_t>(ttyd::system::keyGetStickY(0));
	
	// Check if the stick is at the neutral position
	if ((StickXInt == 0) && (StickYInt == 0))
	{
		// The stick is currently at the neutral position
		if (stickXYOut)
		{
			stickXYOut[0] = 0;
			stickXYOut[1] = 0;
		}
		return -1000;
	}
	
	if (StickXInt > 127)
	{
		StickXInt -= 256;
	}
	
	if (StickYInt > 127)
	{
		StickYInt -= 256;
	}
	
	// Store the individual stick values if desired
	if (stickXYOut)
	{
		stickXYOut[0] = StickXInt;
		stickXYOut[1] = StickYInt;
	}
	
	double StickX = static_cast<double>(StickXInt);
	double StickY = static_cast<double>(StickYInt);
	const double PI = 3.14159265358979323846;
	
	double StickAngle = (atan2(StickX, StickY)) * (180 / PI);
	if (StickAngle < 0)
	{
		StickAngle += 360;
	}
	
	return StickAngle;
}

void getStickAngleString(char *stringOut)
{
	int32_t StickXYAngles[2];
	double StickAngle = getStickAngle(StickXYAngles);
	
	// Check if the stick is at the neutral position
	if (StickAngle == -1000)
	{
		// The stick is currently at the neutral position
		strcpy(stringOut, "Neutral");
		return;
	}
	
	sprintf(stringOut, 
		"%.2f  %" PRId32 "  %" PRId32, 
		StickAngle, 
		StickXYAngles[0], 
		StickXYAngles[1]);
}

char *getTimeString(char *stringOut, int64_t time)
{
	uint32_t ConsoleBusSpeed = gc::os::OSBusClock;
	uint32_t TimeBase = ConsoleBusSpeed / 4;
	uint32_t FPS = getCurrentFPS();
	
	int64_t CurrentTime = time / (TimeBase / FPS);
	
	// Check if the value is negative
	const char *Format;
	if (CurrentTime < 0)
	{
		// Convert the number to positive
		CurrentTime = -CurrentTime;
		
		Format = "-%02" PRIu32 ":%02" PRIu32 ":%02" PRIu32 ".%02" PRIu32;
	}
	else
	{
		Format = "%02" PRIu32 ":%02" PRIu32 ":%02" PRIu32 ".%02" PRIu32;
	}
	
	// Handle the value as unsigned
	uint64_t CurrentTimeUnsigned = static_cast<uint64_t>(CurrentTime);
	
	uint32_t Hour = CurrentTimeUnsigned / 3600 / FPS;
	uint32_t Minute = (CurrentTimeUnsigned / 60 / FPS) % 60;
	uint32_t Second = (CurrentTimeUnsigned / FPS) % 60;
	uint32_t Frame = CurrentTimeUnsigned % FPS;
	
	sprintf(stringOut,
		Format,
		Hour,
		Minute,
		Second,
		Frame);
	
	return stringOut;
}

void updateOnScreenTimerVars()
{
	if (!Displays[ONSCREEN_TIMER])
	{
		return;
	}
	
	int64_t CurrentFrameTime = gc::OSTime::OSGetTime();
	
	// Make sure the previous frame time has a value set
	int64_t PreviousFrameTime = OnScreenTimer.PreviousFrameTime;
	if (PreviousFrameTime != 0)
	{
		if (!OnScreenTimer.TimerPaused && 
		(!MenuVar.MenuIsDisplayed || MenuVar.HideMenu) && 
		!SpawnItem.InAdjustableValueMenu && 
		!MenuVar.ChangingCheatButtonCombo)
		{
			int64_t IncrementAmount = CurrentFrameTime - PreviousFrameTime;
			OnScreenTimer.MainTimer += IncrementAmount;
		}
	}
	
	OnScreenTimer.PreviousFrameTime = CurrentFrameTime;
}

/*void getButtonsPressedDynamic(uint8_t *buttonArrayOut, uint32_t currentButtonCombo)
{
	uint32_t Counter = 0;
	uint32_t Size = 1;
	
	for (uint32_t i = 0; i < 13; i++)
	{
		if (i == 7)
		{
			// Skip unused value
			i++;
		}
		
		if (currentButtonCombo & (1 << i))
		{
			if (!buttonArrayOut)
			{
				buttonArrayOut = new uint8_t[2]; // Extra spot for a 0 at the end of the array
				clearMemory(buttonArrayOut, (2 * sizeof(uint8_t)));
			}
			else
			{
				Size++;
				
				// Create a new array with a new spot for the next value
				uint8_t *tempButtonArray = new uint8_t[Size + 1];
				clearMemory(tempButtonArray, ((Size + 1) * sizeof(uint8_t)));
				
				// Copy the contents of the old array to the new array
				memcpy(tempButtonArray, buttonArrayOut, ((Size - 1) * sizeof(uint8_t)));
				
				// Delete the old array
				delete[] (buttonArrayOut);
				
				// Set the new array as the current array
				buttonArrayOut = tempButtonArray;
			}
			
			buttonArrayOut[Size - 1] = Counter + 1;
		}
		
		Counter++;
	}
	
	if (buttonArrayOut)
	{
		buttonArrayOut[Size] = 0;
	}
	else
	{
		buttonArrayOut = new uint8_t[1];
		buttonArrayOut[0] = 0;
	}
}*/

/*void getButtonsPressedDynamic(uint8_t *buttonArrayOut, uint32_t currentButtonCombo)
{
	if (!buttonArrayOut)
	{
		buttonArrayOut = new uint8_t[14]; // Extra spot for a 0 at the end of the array
	}
	
	// Clear the memory, so that the previous results do not interfere with the new results
	clearMemory(buttonArrayOut, (14 * sizeof(uint8_t)));
	
	getButtonsPressed(buttonArrayOut, currentButtonCombo);
}*/

void getButtonsPressed(uint8_t *buttonArrayOut, uint32_t currentButtonCombo)
{
	uint32_t Counter = 1;
	uint32_t Size = 0;
	
	for (uint32_t i = 0; i < 13; i++)
	{
		if (i == 7)
		{
			// Skip unused value
			i++;
		}
		
		if (currentButtonCombo & (1 << i))
		{
			buttonArrayOut[Size] = Counter;
			Size++;
		}
		
		Counter++;
	}
}

void createButtonStringArray(uint8_t *buttonArray, char *stringOut, uint32_t stringOutSize)
{
	// Make sure the output array has been cleared
	clearMemory(stringOut, stringOutSize);
	
	// Get the text for each button
	char *tempDisplayBuffer = DisplayBuffer;
	const char *ButtonText;
	uint32_t i = 0;
	
	while (buttonArray[i])
	{
		switch (buttonArray[i])
		{
			case DPADLEFT:
			{
				ButtonText = "D-Pad Left";
				break;
			}
			case DPADRIGHT:
			{
				ButtonText = "D-Pad Right";
				break;
			}
			case DPADDOWN:
			{
				ButtonText = "D-Pad Down";
				break;
			}
			case DPADUP:
			{
				ButtonText = "D-Pad Up";
				break;
			}
			case Z:
			{
				ButtonText = "Z";
				break;
			}
			case R:
			{
				ButtonText = "R";
				break;
			}
			case L:
			{
				ButtonText = "L";
				break;
			}
			case A:
			{
				ButtonText = "A";
				break;
			}
			case B:
			{
				ButtonText = "B";
				break;
			}
			case X:
			{
				ButtonText = "X";
				break;
			}
			case Y:
			{
				ButtonText = "Y";
				break;
			}
			case START:
			{
				ButtonText = "Start";
				break;
			}
			default:
			{
				ButtonText = "";
				break;
			}
		}
		
		if (i == 0)
		{
			// Set the initial button pressed
			strcpy(stringOut, ButtonText);
		}
		else
		{
			// Add the next button pressed
			int32_t NewTextSize = sprintf(tempDisplayBuffer,
				" + %s",
				ButtonText);
			
			// Make sure adding the new text will not result in an overflow
			uint32_t CurrentTextSize = strlen(stringOut);
			uint32_t TotalTextSize = CurrentTextSize + static_cast<uint32_t>(NewTextSize) + 1;
			
			if (TotalTextSize > stringOutSize)
			{
				// Adding the new text will result in an overflow, so don't add it
				return;
			}
			
			// Add the new text
			strcat(stringOut, tempDisplayBuffer);
		}
		
		i++;
	}
}

bool incrementCheatsBButtonCounter(uint32_t buttonInput)
{
	// Check for B Button inputs
	uint32_t ButtonInputTrg = ttyd::system::keyGetButtonTrg(0);
	uint32_t tempCheatsBButtonCounter = CheatsDisplayButtons.CheatsBButtonCounter;
	
	if (ButtonInputTrg == PAD_B)
	{
		// Make sure no other buttons are currently being held
		if (buttonInput == PAD_B)
		{
			// Increment the B Button counter
			CheatsDisplayButtons.CheatsBButtonCounter = tempCheatsBButtonCounter + 1;
		}
		else
		{
			return false;
		}
	}
	else if (ButtonInputTrg != 0)
	{
		// Another button has been pressed, so reset the B Button counter
		CheatsDisplayButtons.CheatsBButtonCounter = 0;
		return false;
	}
	
	// Close this menu if the B button has been pressed 3 times in succession
	if (tempCheatsBButtonCounter == 3)
	{
		CheatsDisplayButtons.CheatsBButtonCounter = 0;
		return true;
	}
	else
	{
		return false;
	}
}

bool cheatsManageTimer(uint32_t buttonInput)
{
	if (buttonInput != 0)
	{
		bool FoundDifference = false;
		uint32_t i = 0;
		
		while (CheatsDisplayButtons.CheatsCurrentButtonsHeld[i] != 0)
		{
			if (CheatsDisplayButtons.CheatsPreviousButtonsHeld[i] != 
				CheatsDisplayButtons.CheatsCurrentButtonsHeld[i])
			{
				FoundDifference = true;
				break;
			}
			
			i++;
		}
		
		if (CheatsDisplayButtons.CheatsPreviousButtonsHeld[i] != 0)
		{
			// Button(s) were released
			FoundDifference = true;
		}
		
		if (FoundDifference)
		{
			// New button(s) were pressed, so reset the timer
			MenuVar.Timer = secondsToFrames(3);
			
			// Copy the values from the current buttons held to the previous buttons held
			memcpy(CheatsDisplayButtons.CheatsPreviousButtonsHeld, 
				CheatsDisplayButtons.CheatsCurrentButtonsHeld, (14 * sizeof(uint8_t)));
			
			return false;
		}
		else
		{
			// Decrement the timer
			uint32_t tempTimer = MenuVar.Timer;
			tempTimer--;
			MenuVar.Timer = tempTimer;
			
			return tempTimer == 0;
		}
	}
	else
	{
		// No buttons are currently held, so reset the timer
		MenuVar.Timer = secondsToFrames(3);
		return false;
	}
}

bool checkForDPADInput()
{
	uint32_t ButtonInputTrg = ttyd::system::keyGetButtonTrg(0);
	for (uint32_t i = 0; i < 4; i++)
	{
		if (ButtonInputTrg & (1 << i))
		{
			return true;
		}
	}
	return false;
}

uint32_t checkButtonSingleFrame()
{
	uint32_t ButtonInputTrg = ttyd::system::keyGetButtonTrg(0);
	uint32_t Counter = 1;
	
	for (uint32_t i = 0; i <= 12; i++)
	{
		if (i == 7)
		{
			// Skip unused value
			i++;
		}
		
		if (ButtonInputTrg & (1 << i))
		{
			return Counter;
		}
		
		Counter++;
	}
	
	// Return 0 if no input was found
	return 0;
}

/*void correctPageSingleColumn(uint32_t button, uint8_t &currentPage)
{
	uint32_t tempCurrentMenu 		= MenuVar.CurrentMenu;
	uint32_t tempTotalMenuOptions 	= Menu[tempCurrentMenu].TotalMenuOptions;
	uint32_t tempColumnSplitAmount 	= Menu[tempCurrentMenu].ColumnSplitAmount;
	uint32_t TotalPages = 1 + ((tempTotalMenuOptions - 1) / tempColumnSplitAmount); // Round up
	
	// Make sure there is more than one page
	if (TotalPages == 1)
	{
		return;
	}
	
	uint32_t tempCurrentMenuOption 	= MenuVar.CurrentMenuOption;
	uint32_t tempCurrentPage 		= currentPage;
	
	switch (button)
	{
		case DPADDOWN:
		{
			if (tempCurrentMenuOption == ((tempCurrentPage + 1) * (tempColumnSplitAmount - 1)))
			{
				// Currently on the last option of the page
				if (tempCurrentPage == TotalPages - 1)
				{
					// Currently on the last page, so go to the first page
					currentPage = 0;
				}
				else
				{
					// Go to the next page
					currentPage = tempCurrentPage + 1;
				}
			}
			break;
		}
		case DPADUP:
		{
			if (tempCurrentMenuOption == (tempCurrentPage * tempColumnSplitAmount))
			{
				// Currently on the first option of the page
				if (tempCurrentPage == 0)
				{
					// Currently on the first page, so go to the last page
					currentPage = TotalPages - 1;
				}
				else
				{
					// Go to the previous page
					currentPage = tempCurrentPage - 1;
				}
			}
			break;
		}
		default:
		{
			return;
		}
	}
}*/

void default_DPAD_Actions(uint32_t button)
{
	uint32_t tempCurrentMenu 		= MenuVar.CurrentMenu;
	uint32_t tempTotalMenuOptions 	= Menu[tempCurrentMenu].TotalMenuOptions;
	uint32_t tempColumnSplitAmount 	= Menu[tempCurrentMenu].ColumnSplitAmount;
	uint32_t tempTotalMenuColumns 	= Menu[tempCurrentMenu].TotalMenuColumns;
	uint32_t MaxOptionsPerPage 		= tempColumnSplitAmount * tempTotalMenuColumns;
	uint32_t MaxOptionsPerRow 		= tempTotalMenuColumns;
	
	adjustMenuSelectionVertical(button, MenuVar.CurrentMenuOption, 
		MenuVar.CurrentPage, tempTotalMenuOptions, MaxOptionsPerPage, 
			MaxOptionsPerRow, false);
}

void adjustMenuNoPageEdit(uint32_t button)
{
	uint32_t tempCurrentMenu 		= MenuVar.CurrentMenu;
	uint32_t tempTotalMenuOptions 	= Menu[tempCurrentMenu].TotalMenuOptions;
	uint32_t tempColumnSplitAmount 	= Menu[tempCurrentMenu].ColumnSplitAmount;
	uint32_t tempTotalMenuColumns 	= Menu[tempCurrentMenu].TotalMenuColumns;
	uint32_t MaxOptionsPerPage 		= tempColumnSplitAmount * tempTotalMenuColumns;
	uint32_t MaxOptionsPerRow 		= tempTotalMenuColumns;
	uint8_t tempPage[1];
	tempPage[0] 					= 0;
	
	adjustMenuSelectionVertical(button, MenuVar.CurrentMenuOption, 
		tempPage[0], tempTotalMenuOptions, MaxOptionsPerPage, 
			MaxOptionsPerRow, false);
}

void adjustCheatsResolveFadesSelection(uint32_t button)
{
	uint32_t tempTotalMenuOptions 	= 5;
	uint32_t MaxOptionsPerPage 		= tempTotalMenuOptions;
	uint32_t MaxOptionsPerRow 		= 1;
	uint8_t tempPage[1];
	tempPage[0] 					= 0;
	
	adjustMenuSelectionVertical(button, MenuVar.CurrentMenuOption, 
		tempPage[0], tempTotalMenuOptions, MaxOptionsPerPage, 
			MaxOptionsPerRow, false);
}

void adjustCheatsManageFlagsMainMenu(uint32_t button)
{
	uint32_t TotalMenuOptions;
	switch (MenuVar.MenuSelectionStates)
	{
		case SET_GSW:
		case SET_GW:
		case SET_LSW:
		{
			TotalMenuOptions = CheatsManageGlobalWordsOptionsSize + 1;
			break;
		}
		case SET_GSWF:
		case SET_GF:
		case SET_LSWF:
		default:
		{
			TotalMenuOptions = CheatsManageGlobalFlagsOptionsSize + 1;
		}
	}
	
	uint32_t tempColumnSplitAmount 	= TotalMenuOptions;
	uint32_t MaxOptionsPerPage 		= tempColumnSplitAmount;
	uint32_t MaxOptionsPerRow 		= 1;
	uint8_t tempPage[1];
	tempPage[0] 					= 0;
	
	adjustMenuSelectionVertical(button, MenuVar.CurrentMenuOption, 
		tempPage[0], TotalMenuOptions, MaxOptionsPerPage, 
			MaxOptionsPerRow, false);
}

void adjustMenuSelectionInventory(uint32_t button)
{
	int32_t TotalMenuOptions 			= getTotalItems();
	
	if (TotalMenuOptions == NULL_PTR)
	{
		return;
	}
	
	uint32_t MaxOptionsPerPage 			= 20;
	uint32_t MaxOptionsPerRow 			= 2;
	uint32_t tempMenuSelectedOption 	= MenuVar.MenuSelectedOption;
	
	if ((tempMenuSelectedOption == INVENTORY_BADGES) || 
		(tempMenuSelectedOption == INVENTORY_STORED_ITEMS))
	{
		adjustMenuSelectionVertical(button, MenuVar.CurrentMenuOption, 
			MenuVar.CurrentPage, TotalMenuOptions, MaxOptionsPerPage, 
				MaxOptionsPerRow, true);
	}
	else
	{
		adjustMenuSelectionHorizontal(button, MenuVar.CurrentMenuOption, 
			MenuVar.CurrentPage, TotalMenuOptions, MaxOptionsPerPage, 
				MaxOptionsPerRow, true);
	}
}

/*void adjustAddByIconCurrentOption(uint32_t button)
{
	int32_t UpperAndLowerBounds[2];
	getUpperAndLowerBounds(UpperAndLowerBounds, MenuVar.MenuSelectedOption);
	
	int32_t LowerBound 				= UpperAndLowerBounds[0];
	int32_t UpperBound 				= UpperAndLowerBounds[1];
	
	uint32_t MaxIconsPerRow 		= 16;
	
	uint32_t tempCurrentMenuOption 	= MenuVar.CurrentMenuOption;
	uint32_t tempTotalMenuOptions 	= UpperBound - LowerBound + 1;
	uint32_t TotalRows 				= 1 + ((tempTotalMenuOptions - 1) / MaxIconsPerRow); // Round up
	uint32_t TotalFreeSpaces 		= ((TotalRows * MaxIconsPerRow) - 1) - (tempTotalMenuOptions - 1);
	
	switch (button)
	{
		case DPADLEFT:
		{
			if ((tempCurrentMenuOption % MaxIconsPerRow) == 0)
			{
				// Currently on the furthest left side, so go to the furthest right option
				tempCurrentMenuOption += (MaxIconsPerRow - 1);
				
				// Make sure the current option is valid
				if (tempCurrentMenuOption > (tempTotalMenuOptions - 1))
				{
					// Go to the furthest right option
					MenuVar.CurrentMenuOption = tempCurrentMenuOption - TotalFreeSpaces;
				}
				else
				{
					MenuVar.CurrentMenuOption = tempCurrentMenuOption;
				}
			}
			else
			{
				MenuVar.CurrentMenuOption = tempCurrentMenuOption - 1;
			}
			break;
		}
		case DPADRIGHT:
		{
			if ((tempCurrentMenuOption % MaxIconsPerRow) == (MaxIconsPerRow - 1))
			{
				// Currently on the furthest right side, so go to the furthest left option
				MenuVar.CurrentMenuOption = tempCurrentMenuOption - (MaxIconsPerRow - 1);
			}
			else
			{
				tempCurrentMenuOption += 1;
				
				// Make sure the current option is valid
				if (tempCurrentMenuOption > (tempTotalMenuOptions - 1))
				{
					// Go to the furthest left option
					MenuVar.CurrentMenuOption = tempCurrentMenuOption - (MaxIconsPerRow - TotalFreeSpaces);
				}
				else
				{
					MenuVar.CurrentMenuOption = tempCurrentMenuOption;
				}
			}
			break;
		}
		case DPADDOWN:
		{
			tempCurrentMenuOption += MaxIconsPerRow;
			
			if (tempCurrentMenuOption > (tempTotalMenuOptions - 1))
			{
				// Go to the top of the current column
				MenuVar.CurrentMenuOption = tempCurrentMenuOption % MaxIconsPerRow;
			}
			else
			{
				MenuVar.CurrentMenuOption = tempCurrentMenuOption;
			}
			break;
		}
		case DPADUP:
		{
			// Check if currently at the top of the current column
			if (tempCurrentMenuOption < MaxIconsPerRow)
			{
				// Loop to the last option in the current column
				tempCurrentMenuOption += (TotalRows - 1) * MaxIconsPerRow;
				
				// Make sure the current option is valid
				if (tempCurrentMenuOption > (tempTotalMenuOptions - 1))
				{
					// Go to the option in the previous row
					MenuVar.CurrentMenuOption = tempCurrentMenuOption - MaxIconsPerRow;
				}
				else
				{
					MenuVar.CurrentMenuOption = tempCurrentMenuOption;
				}
			}
			else
			{
				MenuVar.CurrentMenuOption = tempCurrentMenuOption - MaxIconsPerRow;
			}
			break;
		}
		default:
		{
			return;
		}
	}
}*/

void adjustCheatClearAreaFlagSelection(uint32_t button)
{
	uint32_t tempCheatsClearAreaFlagsAreasSize = CheatsClearAreaFlagsAreasSize;
	uint32_t TotalMenuOptions = tempCheatsClearAreaFlagsAreasSize;
	uint32_t MaxOptionsPerRow = 4;
	uint32_t TotalRows = 1 + ((TotalMenuOptions - 1) / MaxOptionsPerRow); // Round up
	uint32_t MaxOptionsPerPage = TotalRows * MaxOptionsPerRow;
	
	adjustMenuSelectionVertical(button, MenuVar.SecondaryMenuOption, 
		MenuVar.SecondaryPage, TotalMenuOptions, MaxOptionsPerPage, 
			MaxOptionsPerRow, false);
}

void adjustMarioStatsSelection(uint32_t button)
{
	uint32_t tempStatsMarioOptionsLinesSize = StatsMarioOptionsLinesSize;
	uint32_t TotalMenuOptions = tempStatsMarioOptionsLinesSize;
	uint32_t MaxOptionsPerRow = 2;
	uint32_t TotalRows = 1 + ((TotalMenuOptions - 1) / MaxOptionsPerRow); // Round up
	uint32_t MaxOptionsPerPage = TotalRows * MaxOptionsPerRow;
	
	adjustMenuSelectionVertical(button, MenuVar.CurrentMenuOption, 
		MenuVar.CurrentPage, TotalMenuOptions, MaxOptionsPerPage, 
			MaxOptionsPerRow, true);
}

void adjustPartnerStatsSelection(uint32_t button)
{
	uint32_t tempStatsPartnerOptionsLinesSize = StatsPartnerOptionsLinesSize;
	uint32_t TotalMenuOptions = tempStatsPartnerOptionsLinesSize;
	
	// Add the necessary extra lines
	ttyd::party::PartyMembers CurrentPartner = getSelectedOptionPartnerValue();
	if (CurrentPartner == ttyd::party::PartyMembers::kYoshi)
	{
		TotalMenuOptions += 2;
	}
	else
	{
		TotalMenuOptions += 1;
	}
	
	uint32_t MaxOptionsPerRow = 1;
	uint32_t TotalRows = 1 + ((TotalMenuOptions - 1) / MaxOptionsPerRow); // Round up
	uint32_t MaxOptionsPerPage = TotalRows * MaxOptionsPerRow;
	
	adjustMenuSelectionVertical(button, MenuVar.CurrentMenuOption, 
		MenuVar.CurrentPage, TotalMenuOptions, MaxOptionsPerPage, 
			MaxOptionsPerRow, true);
}

void adjustMemoryWatchSelection(uint32_t button)
{
	uint32_t TotalMenuOptions = 0;
	uint32_t Size = sizeof(MemoryWatch) / sizeof(MemoryWatch[0]);
	
	for (uint32_t i = 0; i < Size; i++)
	{
		if (!MemoryWatch[i].Address)
		{
			break;
		}
		TotalMenuOptions++;
	}
	
	uint32_t MaxOptionsPerRow = 1;
	uint32_t MaxOptionsPerPage = 10;
	
	adjustMenuSelectionVertical(button, MenuVar.CurrentMenuOption, 
		MenuVar.CurrentPage, TotalMenuOptions, MaxOptionsPerPage, 
			MaxOptionsPerRow, false);
}

void adjustMemoryChangeAddressOrPointerSelection(uint32_t button)
{
	uint32_t TotalMenuOptions = MemoryWatch[MenuVar.MenuSelectedOption].AddressOffsetAmount + 1;
	uint32_t MaxOptionsPerRow = 1;
	uint32_t MaxOptionsPerPage = 11;
	uint8_t tempPage[1];
	tempPage[0] = 0;
	
	adjustMenuSelectionVertical(button, MenuVar.CurrentMenuOption, 
		tempPage[0], TotalMenuOptions, MaxOptionsPerPage, 
			MaxOptionsPerRow, false);
}

void adjustBattlesActorSelection(uint32_t button)
{
	// Get the highest slot in use
	uint32_t TotalMenuOptions = 62;
	uint32_t MaxOptionsPerPage = 18;
	int32_t HighestSlot = 0;
	
	for (uint32_t i = 1; i <= TotalMenuOptions; i++) // Start at 1 to skip System
	{
		if (getActorPointer(i))
		{
			HighestSlot = i;
		}
	}
	
	uint32_t tempCurrentMenuOption = MenuVar.CurrentMenuOption;
	uint32_t tempCurrentPage = MenuVar.CurrentPage;
	
	switch (button)
	{
		case DPADDOWN:
		{
			// Check to see if at the bottom of the current page
			if (tempCurrentMenuOption == (((tempCurrentPage + 1) * MaxOptionsPerPage) - 1))
			{
				// Prevent going to the next page if there are no more free spaces
				if (HighestSlot <= static_cast<int32_t>((tempCurrentPage + 1) * MaxOptionsPerPage))
				{
					// Go to the first page
					MenuVar.CurrentMenuOption = 0;
					MenuVar.CurrentPage = 0;
					return;
				}
			}
			break;
		}
		case DPADUP:
		{
			// Check to see if at the top of the current page
			if (tempCurrentMenuOption == (tempCurrentPage * MaxOptionsPerPage))
			{
				// Only run if on the first page
				if (tempCurrentPage == 0)
				{
					// Go to the last page that has slots in use
					while (HighestSlot > static_cast<int32_t>((tempCurrentPage + 1) * MaxOptionsPerPage))
					{
						tempCurrentPage++;
						tempCurrentMenuOption += MaxOptionsPerPage;
						HighestSlot -= MaxOptionsPerPage;
					}
					
					MenuVar.CurrentPage = tempCurrentPage;
					MenuVar.CurrentMenuOption = tempCurrentMenuOption + MaxOptionsPerPage - 1;
					return;
				}
			}
			break;
		}
		default:
		{
			break;
		}
	}
	
	uint32_t MaxOptionsPerRow = 1;
	
	adjustMenuSelectionVertical(button, MenuVar.CurrentMenuOption, 
		MenuVar.CurrentPage, TotalMenuOptions, MaxOptionsPerPage, 
			MaxOptionsPerRow, false);
}

void adjustBattlesStatusSelection(uint32_t button)
{
	uint32_t tempBattlesStatusesLinesSize = BattlesStatusesLinesSize;
	uint32_t TotalMenuOptions = tempBattlesStatusesLinesSize;
	uint32_t MaxOptionsPerRow = 1;
	uint32_t MaxOptionsPerPage = 12;
	
	adjustMenuSelectionVertical(button, MenuVar.CurrentMenuOption, 
		MenuVar.CurrentPage, TotalMenuOptions, MaxOptionsPerPage, 
			MaxOptionsPerRow, false);
}

void adjustWarpsSelection(uint32_t button)
{
	uint32_t tempWarpDestinationsSize = WarpDestinationsSize;
	uint32_t TotalMenuOptions = tempWarpDestinationsSize;
	uint32_t MaxOptionsPerRow = 4;
	uint32_t TotalRows = 1 + ((TotalMenuOptions - 1) / MaxOptionsPerRow); // Round up
	uint32_t MaxOptionsPerPage = TotalRows * MaxOptionsPerRow;
	
	adjustMenuSelectionVertical(button, MenuVar.CurrentMenuOption, 
		MenuVar.CurrentPage, TotalMenuOptions, MaxOptionsPerPage, 
			MaxOptionsPerRow, false);
}

void adjustIndexWarpCurrentMapEntrancesPage(uint32_t button)
{
	uint32_t tempCurrentPage = MenuVar.CurrentPage;
	switch (button)
	{
		case DPADDOWN:
		{
			uint32_t MaxSlotsInArray = sizeof(WarpByIndex.EntranceList) / sizeof(WarpByIndex.EntranceList[0]);
			uint32_t MaxEntrancesPerPage = 12;
			uint32_t TotalEntrances = 0;
			
			// Get the total entrances for the current map
			const char **tempEntrancesArray = WarpByIndex.EntranceList;
			for (uint32_t i = 0; i < MaxSlotsInArray; i++)
			{
				if (tempEntrancesArray[i])
				{
					TotalEntrances++;
				}
				else
				{
					break;
				}
			}
			
			// Make sure entrances exist on the next page
			if ((MaxEntrancesPerPage * (tempCurrentPage + 1)) < TotalEntrances)
			{
				MenuVar.CurrentPage = tempCurrentPage + 1;
			}
			break;
		}
		case DPADUP:
		{
			// Make sure the current page isn't already at 0
			if (tempCurrentPage > 0)
			{
				MenuVar.CurrentPage = tempCurrentPage - 1;
			}
			break;
		}
		default:
		{
			break;
		}
	}
}

void adjustMenuSelectionVertical(uint32_t button, uint8_t &currentMenuOption, 
	uint8_t &currentPage, uint32_t totalMenuOptions, 
		uint32_t maxOptionsPerPage, uint32_t maxOptionsPerRow,
			bool allowUpwardsSnapFromLeftOrRight)
{
	uint32_t tempCurrentMenuOption 	= currentMenuOption;
	uint32_t tempCurrentPage 		= currentPage;
	
	uint32_t TotalRowsPerPage 		= 1 + ((maxOptionsPerPage - 1) / maxOptionsPerRow); // Round up
	uint32_t TotalColumns 			= 1 + ((maxOptionsPerPage - 1) / TotalRowsPerPage); // Round up
	uint32_t ColumnSplitAmount 		= 1 + ((maxOptionsPerPage - 1) / TotalColumns); // Round up
	
	uint32_t LastValidOption 		= totalMenuOptions - 1;
	uint32_t FirstOptionOnPage 		= maxOptionsPerPage * tempCurrentPage;
	uint32_t LastOptionOnPage 		= (maxOptionsPerPage * (tempCurrentPage + 1)) - 1;
	uint32_t LastValidOptionOnPage 	= LastOptionOnPage;
	
	int32_t NewCurrentMenuOption;
	
	// Make sure the last option on the page is valid
	if (LastValidOptionOnPage > LastValidOption)
	{
		LastValidOptionOnPage = LastValidOption;
	}
	
	switch (button)
	{
		case DPADLEFT:
		{
			// Check if currently on the furthest left column
			NewCurrentMenuOption = tempCurrentMenuOption - ColumnSplitAmount;
			if (NewCurrentMenuOption < static_cast<int32_t>(FirstOptionOnPage))
			{
				// Currently on the furthest left column, so go to the furthest right column
				// Check to see if there are more options to the right of the current column
				NewCurrentMenuOption = tempCurrentMenuOption + ColumnSplitAmount;
				uint32_t LastColumnFirstOption = LastOptionOnPage - ColumnSplitAmount + 1;
				
				if ((LastValidOptionOnPage >= LastColumnFirstOption) && 
					(LastValidOptionOnPage <= LastOptionOnPage))
				{
					// Move to the furthest right option on the page
					for (uint32_t i = 0; i < (TotalColumns - 1); i++)
					{
						tempCurrentMenuOption += ColumnSplitAmount;
					}
					
					// Make sure the value is valid
					if (allowUpwardsSnapFromLeftOrRight)
					{
						// Move to the furthest right option on the page
						while (tempCurrentMenuOption > LastValidOptionOnPage)
						{
							tempCurrentMenuOption--;
						}
					}
					else
					{
						// Move to the furthest right option without moving up
						while (tempCurrentMenuOption > LastValidOptionOnPage)
						{
							tempCurrentMenuOption -= ColumnSplitAmount;
						}
					}
					
					currentMenuOption = tempCurrentMenuOption;
				}
				else
				{
					// There are no options in the right columns, so do nothing
					return;
				}
			}
			else
			{
				// Move to the next left column
				currentMenuOption = tempCurrentMenuOption - ColumnSplitAmount;
			}
			return;
		}
		case DPADRIGHT:
		{
			// Check if currently on the furthest right column
			uint32_t FurthestRightColumnStart = (LastOptionOnPage - ColumnSplitAmount) + 1;
			if (tempCurrentMenuOption >= FurthestRightColumnStart)
			{
				// Move to the furthest left option on the page
				for (uint32_t i = 0; i < (TotalColumns - 1); i++)
				{
					tempCurrentMenuOption -= ColumnSplitAmount;
				}
				
				currentMenuOption = tempCurrentMenuOption;
			}
			else
			{
				// Check to see if there are more options to the right of the current column
				tempCurrentMenuOption += ColumnSplitAmount;
				uint32_t LastColumnFirstOption = LastOptionOnPage - ColumnSplitAmount + 1;
				
				if ((LastValidOptionOnPage >= LastColumnFirstOption) && 
					(LastValidOptionOnPage <= LastOptionOnPage))
				{
					// Make sure the current option is valid
					if (allowUpwardsSnapFromLeftOrRight)
					{
						while (tempCurrentMenuOption > LastValidOptionOnPage)
						{
							// Go to the option in the previous row
							tempCurrentMenuOption--;
						}
					}
					else
					{
						if (tempCurrentMenuOption > LastValidOptionOnPage)
						{
							// Move to the furthest left option
							uint32_t OptionAfterFirstColumn = FirstOptionOnPage + ColumnSplitAmount;
							do
							{
								tempCurrentMenuOption -= ColumnSplitAmount;
							}
							while (tempCurrentMenuOption >= OptionAfterFirstColumn);
						}
					}
					
					currentMenuOption = tempCurrentMenuOption;
				}
				else
				{
					// There are no options in the right columns, so do nothing
					return;
				}
			}
			return;
		}
		case DPADDOWN:
		{
			// Check if currently at the bottom of the current column
			bool CurrentlyAtBottom = false;
			for (uint32_t i = 0; i < TotalColumns; i++)
			{
				uint32_t CurrentOption = LastOptionOnPage - (ColumnSplitAmount * i);
				if (tempCurrentMenuOption == CurrentOption)
				{
					CurrentlyAtBottom = true;
					break;
				}
			}
			
			if (CurrentlyAtBottom)
			{
				// Check to see if the next page has a valid option
				// Get the valid option on the next page if it exists
				NewCurrentMenuOption = tempCurrentMenuOption + 1;
				for (uint32_t i = 0; i < (TotalColumns - 1); i++)
				{
					NewCurrentMenuOption += ColumnSplitAmount;
				}
				
				uint32_t NextPageFirstColumnOption = LastOptionOnPage + 1;
				uint32_t tempNewCurrentMenuOption = static_cast<uint32_t>(NewCurrentMenuOption);
				bool FoundValidOption = false;
				
				for (uint32_t i = NextPageFirstColumnOption; 
					i <= tempNewCurrentMenuOption; i++)
				{
					if (i > LastValidOption)
					{
						break;
					}
					
					NewCurrentMenuOption = i;
					FoundValidOption = true;
				}
				
				if (FoundValidOption)
				{
					// Go to the valid option on the next page
					currentMenuOption = static_cast<uint32_t>(NewCurrentMenuOption);
					currentPage = tempCurrentPage + 1;
				}
				else
				{
					// Go to the top of the column on the first page
					currentMenuOption = (tempCurrentMenuOption % maxOptionsPerPage) - ColumnSplitAmount + 1;
					currentPage = 0;
				}
			}
			else if (tempCurrentMenuOption == LastValidOption)
			{
				// Currently on the last option, so go to the top of the column on the first page
				uint32_t LastOptionCurrentColumn = 0;
				for (uint32_t i = 0; i <= LastValidOption; i += ColumnSplitAmount)
				{
					LastOptionCurrentColumn += ColumnSplitAmount;
				}
				
				uint32_t TotalFreeSpaces = LastOptionCurrentColumn - tempCurrentMenuOption - 1;
				currentMenuOption = (((tempCurrentMenuOption % maxOptionsPerPage) - ColumnSplitAmount) + TotalFreeSpaces) + 1;
				currentPage = 0;
			}
			else
			{
				// Move to the next option
				currentMenuOption = tempCurrentMenuOption + 1;
			}
			return;
		}
		case DPADUP:
		{
			// Check if currently at the top of the current column
			bool CurrentlyAtTop = false;
			for (uint32_t i = 0; i < TotalColumns; i++)
			{
				uint32_t CurrentOption = FirstOptionOnPage + (ColumnSplitAmount * i);
				if (tempCurrentMenuOption == CurrentOption)
				{
					CurrentlyAtTop = true;
					break;
				}
			}
			
			if (CurrentlyAtTop)
			{
				// Check if currently on the first page
				if (tempCurrentPage == 0)
				{
					// Go to the last option of the current column on the last page
					uint32_t LastPage = (1 + ((totalMenuOptions - 1) / maxOptionsPerPage)) - 1;
					uint32_t LastPageLastColumnLastOption = (maxOptionsPerPage * (LastPage + 1)) - 1;
					
					uint32_t DecrementAmount = ColumnSplitAmount * (TotalColumns - 1);
					for (uint32_t i = 0; i < tempCurrentMenuOption; i += ColumnSplitAmount)
					{
						DecrementAmount -= ColumnSplitAmount;
					}
					
					tempCurrentMenuOption = LastPageLastColumnLastOption - DecrementAmount;
					
					// Make sure the new option is valid
					while (tempCurrentMenuOption > LastValidOption)
					{
						tempCurrentMenuOption--;
					}
					
					currentMenuOption = tempCurrentMenuOption;
					currentPage = LastPage;
				}
				else
				{
					// Go to the last option of the current column on the previous page
					NewCurrentMenuOption = tempCurrentMenuOption - 1;
					for (uint32_t i = 0; i < (TotalColumns - 1); i++)
					{
						NewCurrentMenuOption -= ColumnSplitAmount;
					}
					
					currentMenuOption = static_cast<uint32_t>(NewCurrentMenuOption);
					currentPage = tempCurrentPage - 1;
				}
			}
			else
			{
				// Move to the next option
				currentMenuOption = tempCurrentMenuOption - 1;
			}
			return;
		}
		default:
		{
			return;
		}
	}
}

void adjustMenuSelectionHorizontal(uint32_t button, uint8_t &currentMenuOption, 
	uint8_t &currentPage, uint32_t totalMenuOptions, 
		uint32_t maxOptionsPerPage, uint32_t maxOptionsPerRow,
			bool allowUpwardsSnapFromLeftOrRight)
{
	uint32_t tempCurrentMenuOption 	= currentMenuOption;
	uint32_t tempCurrentPage 		= currentPage;
	
	uint32_t LastValidOption 		= totalMenuOptions - 1;
	uint32_t CurrentColumn 			= tempCurrentMenuOption % maxOptionsPerRow;
	uint32_t LastColumn 			= maxOptionsPerRow - 1;
	uint32_t LastPage 				= (1 + ((totalMenuOptions - 1) / maxOptionsPerPage)) - 1;
	
	uint32_t TotalRows 				= 1 + ((totalMenuOptions - 1) / maxOptionsPerRow); // Round up
	uint32_t TotalFreeSpaces 		= ((TotalRows * maxOptionsPerRow) - 1) - (totalMenuOptions - 1);
	
	uint32_t TotalColumns 			= maxOptionsPerRow;
	
	// Make sure the total columns is valid
	if (TotalColumns > totalMenuOptions)
	{
		TotalColumns = totalMenuOptions;
	}
	
	switch (button)
	{
		case DPADLEFT:
		{
			// Check to see if currently on the furthest left column
			if (CurrentColumn == 0)
			{
				// Check to see if currently on the last option
				if ((tempCurrentMenuOption + LastColumn) > LastValidOption)
				{
					if (allowUpwardsSnapFromLeftOrRight)
					{
						// Go to the furthest right option and up one column
						currentMenuOption = tempCurrentMenuOption - 1;
					}
					else
					{
						// Go to the furthest left option
						currentMenuOption = (tempCurrentMenuOption + LastColumn) - TotalFreeSpaces;
					}
				}
				else
				{
					// Go to the furthest right option in the current row
					currentMenuOption = tempCurrentMenuOption + LastColumn;
				}
			}
			else
			{
				// Go to the next left option in the current row
				currentMenuOption = tempCurrentMenuOption - 1;
			}
			return;
		}
		case DPADRIGHT:
		{
			// Check to see if currently on the furthest right column
			if (CurrentColumn == LastColumn)
			{
				// Currently on the furthest right side, so go to the furthest left option
				currentMenuOption = tempCurrentMenuOption - LastColumn;
			}
			else
			{
				// Check to see if currently on the last option
				if (tempCurrentMenuOption >= LastValidOption)
				{
					if (allowUpwardsSnapFromLeftOrRight)
					{
						// Go to the next right option and up one column
						currentMenuOption = tempCurrentMenuOption - LastColumn;
					}
					else
					{
						// Go to the furthest left option
						currentMenuOption = (tempCurrentMenuOption + 1) - 
							(maxOptionsPerRow - TotalFreeSpaces);
					}
				}
				else
				{
					// Go to the next right option in the current row
					currentMenuOption = tempCurrentMenuOption + 1;
				}
			}
			return;
		}
		case DPADDOWN:
		{
			// Check if currently at the bottom of the current column
			uint32_t LastOptionOnPage = (maxOptionsPerPage * (tempCurrentPage + 1)) - 1;
			bool CurrentlyAtBottom = false;
			
			for (uint32_t i = 0; i < TotalColumns; i++)
			{
				uint32_t CurrentOption = LastOptionOnPage - i;
				if (tempCurrentMenuOption == CurrentOption)
				{
					CurrentlyAtBottom = true;
					break;
				}
			}
			
			if (CurrentlyAtBottom)
			{
				// Check to see if currently on the last page or not
				if (tempCurrentPage == LastPage)
				{
					// Currently on the last page, so go to the first page
					currentPage = 0;
					
					// Go to the top of the current column
					currentMenuOption = CurrentColumn;
				}
				else
				{
					// Go to the next page
					currentPage = tempCurrentPage + 1;
					currentMenuOption = tempCurrentMenuOption + maxOptionsPerRow;
				}
			}
			else if ((tempCurrentMenuOption + maxOptionsPerRow) > LastValidOption)
			{
				// Go to the first page
				currentPage = 0;
					
				// Go to the top of the current column
				currentMenuOption = CurrentColumn;
			}
			else
			{
				// Go to the next option in the current column
				currentMenuOption = tempCurrentMenuOption + maxOptionsPerRow;
			}
			return;
		}
		case DPADUP:
		{
			// Check if currently at the top of the current column
			uint32_t FirstOptionOnPage = maxOptionsPerPage * tempCurrentPage;
			bool CurrentlyAtTop = false;
			
			for (uint32_t i = 0; i < TotalColumns; i++)
			{
				uint32_t CurrentOption = FirstOptionOnPage + i;
				if (tempCurrentMenuOption == CurrentOption)
				{
					CurrentlyAtTop = true;
					break;
				}
			}
			
			if (CurrentlyAtTop)
			{
				// Check if currently on the first page
				if (tempCurrentPage == 0)
				{
					// Go to the last page
					currentPage = LastPage;
					
					// Go to the last option in the current column
					tempCurrentMenuOption += (TotalRows - 1) * maxOptionsPerRow;
					
					// Make sure the current option is valid
					while (tempCurrentMenuOption > LastValidOption)
					{
						// Go to the option in the previous row
						tempCurrentMenuOption -= maxOptionsPerRow;
					}
					
					currentMenuOption = tempCurrentMenuOption;
				}
				else
				{
					// Go to the previous page
					currentPage = tempCurrentPage - 1;
					
					// Go to the last option in the current column
					currentMenuOption = tempCurrentMenuOption - maxOptionsPerRow;
				}
			}
			else
			{
				// Go to the previous option in the current column
				currentMenuOption = tempCurrentMenuOption - maxOptionsPerRow;
			}
			return;
		}
		default:
		{
			return;
		}
	}
}

}