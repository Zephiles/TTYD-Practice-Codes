#include "menufunctions.h"
#include "global.h"
#include "commonfunctions.h"
#include "memorywatch.h"
#include "codes.h"
#include "items.h"

#include <ttyd/win_main.h>
#include <ttyd/win_item.h>
#include <ttyd/msgdrv.h>
#include <ttyd/mario_pouch.h>
#include <ttyd/party.h>
#include <ttyd/mario.h>
#include <ttyd/swdrv.h>
#include <ttyd/battle.h>
#include <ttyd/evtmgr.h>
#include <ttyd/string.h>
#include <ttyd/system.h>
#include <ttyd/__mem.h>

#include <cstdio>

namespace mod {

void closeMenu()
{
	MenuIsDisplayed = false;
	
	resetMenuToRoot();
	
	// Lower the System Level if not in a battle
	lowerSystemLevel();
	
	// Enable the Pause Menu
	ttyd::win_main::winOpenEnable();
}

void closeSecondaryMenu()
{
	// Reset the current menu option
	CurrentMenuOption = SelectedOption;
	
	// Close the menu
	SelectedOption = 0;
	
	// Reset the function return code
	FunctionReturnCode = 0;
}


void resetMenu()
{
	CurrentPage = 0;
	resetMenuNoPageReset();
}

void resetMenuNoPageReset()
{
	CurrentMenuOption 		= 0;
	SecondaryMenuOption 	= 0;
	SelectedOption 			= 0;
	SecondaryPage 			= 0;
	Timer 					= 0;
	MenuSelectionStates 	= 0;
	MenuSecondaryValue 		= 0;
}

void resetMenuToRoot()
{
	CurrentMenu 			= ROOT;
	MenuSelectedOption 		= 0;
	HideMenu 				= false;
	resetMenu();
}

void resetAndCloseSecondaryMenu()
{
	// Reset the value used by the secondary menus
	MenuSecondaryValue = 0;
	
	// Reset the timer
	Timer = 0;
	
	// Reset the current menu option and then close the menu
	closeSecondaryMenu();
}

void resetImportantItemsPauseMenu()
{
	// Only run for the important items menu
	if (MenuSelectedOption != INVENTORY_IMPORTANT)
	{
		// Not modifying important items, so do nothing
		return;
	}
	
	// Only run if the pause menu is currently open
	uint32_t SystemLevel = getSystemLevel();
	if (SystemLevel != 15)
	{
		// The pause menu is not open, so do nothing
		return;
	}
	
	void *PauseMenuPointer = reinterpret_cast<void *>(
		*reinterpret_cast<uint32_t *>(PauseMenuStartAddress));
	
	// Get the current submenu that is open in the items menu
	uint32_t ImportantItemsSubMenu = *reinterpret_cast<uint32_t *>(
		reinterpret_cast<uint32_t>(PauseMenuPointer) + 0x210);
	
	// Reset the item menu
	ttyd::win_item::winItemExit(PauseMenuPointer);
	
	// Re-init the menu
	ttyd::win_item::winItemInit(PauseMenuPointer);
	
	// Restore the submenu
	*reinterpret_cast<uint32_t *>(
		reinterpret_cast<uint32_t>(PauseMenuPointer) + 
			0x210) = ImportantItemsSubMenu;
}

void recheckUpgradesBattles(int32_t item)
{
	// Only run while in a battle
	if (!checkForSpecificSeq(ttyd::seqdrv::SeqIndex::kBattle))
	{
		return;
	}
	
	// Only run if the current item is a boot or hammer upgrade
	if ((item >= Boots) && (item <= UltraHammer))
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
		uint32_t SystemLevel = getSystemLevel();
		
		// Increase the System Level by 1
		setSystemLevel(SystemLevel + 1);
	}
}

void lowerSystemLevel()
{
	// Lower the System Level if not in a battle
	if (checkIfInGame())
	{
		uint32_t SystemLevel = getSystemLevel();
		if (SystemLevel > 0)
		{
			// Decrease the System Level by 1
			// Only decrease if not already at 0
			setSystemLevel(SystemLevel - 1);
		}
	}
}

const char *getItemName(int16_t item)
{
	ItemData *item_db = ItemDataTable;
	
	return ttyd::msgdrv::msgSearch(item_db[item].item_name_msg);
}

void *getFreeSlotPointer()
{
	uint32_t Address_and_Size[2];
	uint32_t *tempArray		= getPouchAddressAndSize(Address_and_Size);
	
	if (tempArray == nullptr)
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
	
	if (tempArray == nullptr)
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

int32_t *getUpperAndLowerBounds(int32_t *tempArray, uint32_t currentMenu)
{
	uint32_t tempMenuSelectedOption = MenuSelectedOption;
	uint32_t tempSelectedOption = SelectedOption;
	int32_t LowerBound = 0;
	int32_t UpperBound = 0;
	
	switch (currentMenu)
	{
		case INVENTORY_STANDARD:
		case INVENTORY_STORED_ITEMS:
		{
			LowerBound = GoldBar;
			UpperBound = FreshJuice;
			break;
		}
		case INVENTORY_IMPORTANT:
		{
			LowerBound = StrangeSack;
			UpperBound = CrystalStar;
			break;
		}
		case INVENTORY_BADGES:
		{
			LowerBound = PowerJump;
			UpperBound = SuperChargeP;
			break;
		}
		case CHEATS_MANAGE_FLAGS_MAIN:
		{
			uint32_t tempCurrentMenuOption = CurrentMenuOption;
			switch (MenuSelectionStates)
			{
				case SET_GSW:
				{
					if (tempCurrentMenuOption == CHANGE_GLOBAL_WORD)
					{
						UpperBound = 2048;
					}
					else if (ManageFlags.FlagToSet == 0) // 0 = Sequence position
					{
						UpperBound = 405;
					}
					else
					{
						UpperBound = 255;
					}
					break;
				}
				case SET_GSWF:
				{
					if (tempCurrentMenuOption == CHANGE_GLOBAL_FLAG)
					{
						UpperBound = 8192;
					}
					else
					{
						UpperBound = 1;
					}
					break;
				}
				case SET_GW:
				{
					if (tempCurrentMenuOption == CHANGE_GLOBAL_WORD)
					{
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
						UpperBound = 96;
					}
					else
					{
						UpperBound = 1;
					}
					break;
				}
				case SET_LSW:
				{
					if (tempCurrentMenuOption == CHANGE_GLOBAL_WORD)
					{
						UpperBound = 1024;
					}
					else
					{
						UpperBound = 255;
					}
					break;
				}
				case SET_LSWF:
				{
					if (tempCurrentMenuOption == CHANGE_GLOBAL_FLAG)
					{
						UpperBound = 512;
					}
					else
					{
						UpperBound = 1;
					}
					break;
				}
				default:
				{
					UpperBound = 0;
					break;
				}
			}
			break;
		}
		case STATS_MARIO:
		{
			uint32_t PouchPtr = reinterpret_cast<uint32_t>(ttyd::mario_pouch::pouchGetPtr());
			switch (tempMenuSelectedOption)
			{
				case COINS:
				case MARIO_MAX_HP:
				case MARIO_MAX_FP:
				case STAR_PIECES:
				case SHINE_SPRITES:
				{
					UpperBound = 999;
					break;
				}
				case MARIO_HP:
				{
					UpperBound = *reinterpret_cast<int16_t *>(PouchPtr + 0x72); // Max HP
					break;
				}
				case MARIO_FP:
				{
					UpperBound = *reinterpret_cast<int16_t *>(PouchPtr + 0x76); // Max FP
					break;
				}
				case BP:
				case MARIO_LEVEL:
				case STAR_POINTS:
				{
					UpperBound = 99;
					break;
				}
				case MARIO_RANK:
				{
					UpperBound = 3;
					break;
				}
				case STAR_POWER:
				{
					UpperBound = *reinterpret_cast<int16_t *>(PouchPtr + 0x7C); // Max Star Power
					break;
				}
				case MAX_STAR_POWER:
				{
					UpperBound = 800;
					break;
				}
				case SHOP_POINTS:
				{
					UpperBound = 300;
					break;
				}
				case PIANTAS_STORED:
				case CURRENT_PIANTAS:
				{
					UpperBound = 99999;
					break;
				}
				default:
				{
					UpperBound = 0;
					break;
				}
			}
			break;
		}
		case STATS_PARTNERS:
		{
			uint32_t PartnerEnabledAddress = reinterpret_cast<uint32_t>(getPartnerEnabledAddress());
			switch (tempMenuSelectedOption)
			{
				case PARTNER_HP:
				{
					UpperBound = *reinterpret_cast<int16_t *>(PartnerEnabledAddress + 0x2); // Max HP
					break;
				}
				case PARTNER_MAX_HP:
				{
					UpperBound = 999;
					break;
				}
				case PARTNER_RANK:
				{
					UpperBound = 2;
					break;
				}
				default:
				{
					UpperBound = 0;
					break;
				}
			}
			break;
		}
		case BATTLES_CURRENT_ACTOR:
		{
			uint32_t ActorAddress = reinterpret_cast<uint32_t>(getActorPointer(tempMenuSelectedOption));
			if (ActorAddress == 0)
			{
				break;
			}
			
			switch (tempSelectedOption)
			{
				case CHANGE_ACTOR_HP:
				{
					UpperBound = *reinterpret_cast<int16_t *>(ActorAddress + 0x108); // Max HP
					break;
				}
				case CHANGE_ACTOR_FP:
				{
					UpperBound = *reinterpret_cast<int16_t *>(ActorAddress + 0x10E); // Max FP
					break;
				}
				case CHANGE_HELD_ITEM:
				{
					LowerBound = ThunderBolt;
					UpperBound = SuperChargeP;
					break;
				}
				default:
				{
					UpperBound = 999;
					break;
				}
			}
			break;
		}
		case BATTLES_STATUSES:
		{
			uint32_t ActorAddress = reinterpret_cast<uint32_t>(getActorPointer(tempMenuSelectedOption));
			if (ActorAddress == 0)
			{
				break;
			}
			
			switch (tempSelectedOption)
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
					UpperBound = 1;
					break;
				}
				default:
				{
					UpperBound = 99;
					break;
				}
			}
			break;
		}
		case CHEATS_NPC_FORCE_DROP:
		{
			LowerBound = GoldBar;
			UpperBound = SuperChargeP;
			break;
		}
		case CHEATS_CHANGE_SEQUENCE:
		{
			// LowerBound = 0;
			UpperBound = 405;
			break;
		}
		case WARPS:
		{
			LowerBound = 1;
			UpperBound = 100;
			break;
		}
		case SPAWN_ITEM_MENU_VALUE:
		{
			LowerBound = StrangeSack;
			UpperBound = SuperChargeP;
			break;
		}
		default:
		{
			break;
		}
	}
	
	tempArray[0] = LowerBound;
	tempArray[1] = UpperBound;
	return tempArray;
}

uint32_t *getPouchAddressAndSize(uint32_t *tempArray)
{
	uint32_t tempAddressOffset;
	uint32_t tempSize;
	
	switch (MenuSelectedOption)
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
	
	if (tempArray == nullptr)
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
	
	if (CurrentItem != 0)
	{
		// Found an item
		return true;
	}
	else
	{
		return false;
	}
}

bool checkForClosingErrorMessage()
{
	uint32_t tempTimer = Timer;
	if (checkForDPADInput())
	{
		Timer 				= 0;
		FunctionReturnCode 	= 0;
		return true;
	}
	else
	{
		tempTimer--;
		Timer = tempTimer;
		
		if (tempTimer == 0)
		{
			FunctionReturnCode 	= 0;
			return true;
		}
	}
	return false;
}

void correctInventoryCurrentMenuOptionAndPage(uint32_t maxOptionsPerPage)
{
	// Make sure that an option has already been selected
	uint32_t tempSelectedOption = SelectedOption;
	if (tempSelectedOption != 0)
	{
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
			CurrentPage = 0;
			return;
		}
		
		uint32_t TotalPages 			= 1 + ((TotalMenuOptions - 1) / maxOptionsPerPage); // Round up
		uint32_t tempCurrentMenuOption 	= CurrentMenuOption;
		uint32_t tempCurrentPage 		= CurrentPage;
		
		if ((tempSelectedOption >= DUPLICATE) && 
			(tempSelectedOption <= DELETE))
		{
			if (tempCurrentMenuOption > (TotalMenuOptions - 1))
			{
				CurrentMenuOption = TotalMenuOptions - 1;
			}
		}
		
		if (tempCurrentPage > (TotalPages - 1))
		{
			CurrentPage = TotalPages - 1;
		}
	}
	else
	{
		// No option is currently selected, so do nothing
		return;
	}
}

uint32_t getHighestAdjustableValueDigit(uint32_t currentMenu)
{
	int32_t UpperAndLowerBounds[2];
	int32_t *tempArray = getUpperAndLowerBounds(UpperAndLowerBounds, currentMenu);
	int32_t LowerBound = tempArray[0];
	int32_t UpperBound = tempArray[1];
	
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
	int32_t *tempArray = getUpperAndLowerBounds(UpperAndLowerBounds, currentMenu);
	int32_t UpperBound = tempArray[1];
	
	MenuSecondaryValue = UpperBound;
}

void setAdjustableValueToMin(uint32_t currentMenu)
{
	int32_t UpperAndLowerBounds[2];
	int32_t *tempArray = getUpperAndLowerBounds(UpperAndLowerBounds, currentMenu);
	int32_t LowerBound = tempArray[0];
	
	MenuSecondaryValue = LowerBound;
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
		// Close the menu IF there are no numbers to draw
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
	
	uint32_t tempMenuSelectedOption = MenuSelectedOption;
	uint32_t tempCurrentMenuOption = CurrentMenuOption;
	
	switch (Button)
	{
		case DPADLEFT:
		{
			uint32_t tempSecondaryMenuOption = SecondaryMenuOption;
			if (SecondaryMenuOption == 0)
			{
				// Loop to the last option
				SecondaryMenuOption = getHighestAdjustableValueDigit(currentMenu) - 1;
			}
			else
			{
				// Move left once
				SecondaryMenuOption = tempSecondaryMenuOption - 1;
			}
			
			FrameCounter = 1;
			return Button;
		}
		case DPADRIGHT:
		{
			uint32_t tempSecondaryMenuOption = SecondaryMenuOption;
			if (SecondaryMenuOption == (getHighestAdjustableValueDigit(currentMenu) - 1))
			{
				// Loop to the first option
				SecondaryMenuOption = 0;
			}
			else
			{
				// Move right once
				SecondaryMenuOption = tempSecondaryMenuOption + 1;
			}
			
			FrameCounter = 1;
			return Button;
		}
		case DPADDOWN:
		{
			// Decrement the current value for the current slot in the drawAddById function
			adjustAddByIdValue(-1, currentMenu);
			
			FrameCounter = 1;
			return Button;
		}
		case DPADUP:
		{
			// Increment the current value for the current slot in the drawAddById function
			adjustAddByIdValue(1, currentMenu);
			
			FrameCounter = 1;
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
			
			FrameCounter = 1;
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
					switch (SelectedOption)
					{
						case ADD_BY_ID:
						{
							void *tempAddress = getFreeSlotPointer();
							if (tempAddress != 0)
							{
								setAddByIdValue(tempAddress);
								
								FrameCounter = 1;
								return ADDING_BY_ID;
							}
							else
							{
								return 0;
							}
						}
						case CHANGE_BY_ID:
						{
							if (getTotalItems() > 0)
							{
								changeItem();
								MenuSelectionStates = 0;
								
								FrameCounter = 1;
								return Button;
							}
							else
							{
								return 0;
							}
						}
						default:
						{
							return 0;
						}
					}
				}
				case CHEATS_CHANGE_SEQUENCE:
				{
					setSequencePosition(static_cast<uint32_t>(MenuSecondaryValue));
					MenuSelectionStates = 0;
					
					FrameCounter = 1;
					return Button;
				}
				case CHEATS_NPC_FORCE_DROP:
				{
					ForcedNPCItemDrop = static_cast<int16_t>(MenuSecondaryValue);
					MenuSelectionStates = 0;
					
					FrameCounter = 1;
					return Button;
				}
				case CHEATS_MANAGE_FLAGS_MAIN:
				{
					switch (MenuSelectionStates)
					{
						case SET_GSW:
						case SET_GW:
						case SET_LSW:
						{
							if (tempCurrentMenuOption == CHANGE_GLOBAL_WORD)
							{
								ManageFlags.FlagToSet = static_cast<uint16_t>(MenuSecondaryValue);
							}
							else
							{
								ManageFlags.ValueToSet = MenuSecondaryValue;
							}
							
							SelectedOption = 0;
							
							FrameCounter = 1;
							return Button;
						}
						case SET_GSWF:
						case SET_GF:
						case SET_LSWF:
						{
							if (tempCurrentMenuOption == CHANGE_GLOBAL_FLAG)
							{
								ManageFlags.FlagToSet = static_cast<uint16_t>(MenuSecondaryValue);
							}
							else
							{
								ManageFlags.ValueToSet = MenuSecondaryValue;
							}
							
							SelectedOption = 0;
							
							FrameCounter = 1;
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
					MenuSelectedOption = 0;
					
					FrameCounter = 1;
					return Button;
				}
				case STATS_PARTNERS:
				{
					setPartnerStatsValue(tempCurrentMenuOption + 1);
					MenuSelectedOption = 0;
					
					FrameCounter = 1;
					return Button;
				}
				case BATTLES_CURRENT_ACTOR:
				{
					setBattlesActorValue(tempCurrentMenuOption);
					SelectedOption = 0;
					
					FrameCounter = 1;
					return Button;
				}
				case BATTLES_STATUSES:
				{
					setBattlesActorStatusValue(tempCurrentMenuOption);
					SelectedOption = 0;
					
					FrameCounter = 1;
					return Button;
				}
				case WARPS:
				{
					ttyd::swdrv::swByteSet(1321, static_cast<uint32_t>(
						MenuSecondaryValue - 1)); // GSW(1321)
					
					MenuSelectedOption = 0;
					
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
						}
						case NOT_IN_GAME:
						{
							FunctionReturnCode 	= ReturnCode;
							Timer 				= secondsToFrames(3);
							break;
						}
						default:
						{
							break;
						}
					}
					
					FrameCounter = 1;
					return Button;
				}
				case SPAWN_ITEM_MENU_VALUE:
				{
					FrameCounter = 1;
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
						if (SelectedOption == ADD_BY_ID)
						{
							resetAndCloseSecondaryMenu();
						}
						else // (SelectedOption == CHANGE_BY_ID)
						{
							MenuSelectionStates = 0;
						}
						
						FrameCounter = 1;
						
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
						MenuSelectionStates = 0;
						
						FrameCounter = 1;
						
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
						MenuSelectedOption = 0;
						
						FrameCounter = 1;
						
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
					{
						SelectedOption = 0;
						
						FrameCounter = 1;
						
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
			
			FrameCounter = 1;
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
	uint32_t tempSelectedOption = SelectedOption;
	uint32_t Button = checkButtonSingleFrame();
	
	switch (Button)
	{
		case DPADLEFT:
		case DPADRIGHT:
		case DPADDOWN:
		case DPADUP:
		{
			int32_t UpperAndLowerBounds[2];
			int32_t *tempArray 				= getUpperAndLowerBounds(UpperAndLowerBounds, currentMenu);
			int32_t LowerBound 				= tempArray[0];
			int32_t UpperBound 				= tempArray[1];
			
			uint32_t MaxOptionsPerRow 		= 16;
			uint32_t tempTotalMenuOptions 	= UpperBound - LowerBound + 1;
			uint32_t MaxOptionsPerPage 		= tempTotalMenuOptions;
			
			adjustMenuSelectionHorizontal(Button, SecondaryMenuOption, 
				SecondaryPage, tempTotalMenuOptions, MaxOptionsPerPage, 
					MaxOptionsPerRow, false);
			
			FrameCounter = 1;
			return Button;
		}
		case A:
		{
			if (tempSelectedOption == ADD_BY_ICON)
			{
				void *tempAddress = getFreeSlotPointer();
				if (tempAddress != nullptr)
				{
					setAddByIconValue(tempAddress);
					
					FrameCounter = 1;
					return Button;
				}
				else
				{
					return 0;
				}
			}
			else // (tempSelectedOption == CHANGE_BY_ICON)
			{
				if (getTotalItems() > 0)
				{
					MenuSelectionStates = 0;
					changeItem();
					
					FrameCounter = 1;
					return Button;
				}
				else
				{
					return 0;
				}
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
				MenuSelectionStates = 0;
			}
			
			FrameCounter = 1;
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
	uint32_t tempSecondaryMenuOption = SecondaryMenuOption;
	
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
			
			adjustMenuSelectionVertical(Button, SecondaryMenuOption, 
				SecondaryPage, TotalMenuOptions, MaxOptionsPerPage, 
					MaxOptionsPerRow, false);
			
			FrameCounter = 1;
			return Button;
		}
		case A:
		{
			uint32_t tempMenuSelectedOption = MenuSelectedOption;
			MemoryWatch[tempMenuSelectedOption].Type = tempSecondaryMenuOption;
			
			// Make sure the address being read does not exceed 0x817FFFFF
			MemoryWatch[tempMenuSelectedOption].Address = reinterpret_cast<uint32_t>(
				fixBaseAddress(tempMenuSelectedOption, reinterpret_cast<void *>(
					MemoryWatch[tempMenuSelectedOption].Address)));
			
			SelectedOption = 0;
			
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
			
			FrameCounter = 1;
			return Button;
		}
		case B:
		{
			SelectedOption = 0;
			
			FrameCounter = 1;
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
	uint32_t tempMenuSelectedOption = MenuSelectedOption;
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
						int32_t PosX = MemoryWatch[tempMenuSelectedOption].PosX;
						MemoryWatch[tempMenuSelectedOption].PosX = PosX - 5;
						break;
					}
					case DPADRIGHT:
					{
						int32_t PosX = MemoryWatch[tempMenuSelectedOption].PosX;
						MemoryWatch[tempMenuSelectedOption].PosX = PosX + 5;
						break;
					}
					case DPADUP:
					{
						int32_t PosY = MemoryWatch[tempMenuSelectedOption].PosY;
						MemoryWatch[tempMenuSelectedOption].PosY = PosY + 5;
						break;
					}
					case DPADDOWN:
					{
						int32_t PosY = MemoryWatch[tempMenuSelectedOption].PosY;
						MemoryWatch[tempMenuSelectedOption].PosY = PosY - 5;
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
			int32_t PosX = MemoryWatch[tempMenuSelectedOption].PosX;
			MemoryWatch[tempMenuSelectedOption].PosX = PosX - 1;
			
			FrameCounter = 1;
			return Button;
		}
		case DPADRIGHT:
		{
			int32_t PosX = MemoryWatch[tempMenuSelectedOption].PosX;
			MemoryWatch[tempMenuSelectedOption].PosX = PosX + 1;
			
			FrameCounter = 1;
			return Button;
		}
		case DPADUP:
		{
			int32_t PosY = MemoryWatch[tempMenuSelectedOption].PosY;
			MemoryWatch[tempMenuSelectedOption].PosY = PosY + 1;
			
			FrameCounter = 1;
			return Button;
		}
		case DPADDOWN:
		{
			int32_t PosY = MemoryWatch[tempMenuSelectedOption].PosY;
			MemoryWatch[tempMenuSelectedOption].PosY = PosY - 1;
			
			FrameCounter = 1;
			return Button;
		}
		case A:
		{
			SelectedOption = 0;
			HideMenu = false;
			
			FrameCounter = 1;
			return Button;
		}
		case B:
		{
			MemoryWatch[tempMenuSelectedOption].PosX = MemoryWatchPosition.PosX;
			MemoryWatch[tempMenuSelectedOption].PosY = MemoryWatchPosition.PosY;
			SelectedOption = 0;
			HideMenu = false;
			
			FrameCounter = 1;
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
			
			adjustMenuSelectionVertical(Button, SecondaryMenuOption, 
				SecondaryPage, TotalMenuOptions, MaxOptionsPerPage, 
					MaxOptionsPerRow, false);
			
			FrameCounter = 1;
			return Button;
		}
		case A:
		{
			uint32_t PouchPtr = reinterpret_cast<uint32_t>(ttyd::mario_pouch::pouchGetPtr());
			*reinterpret_cast<int16_t *>(PouchPtr + 0x8C) ^= (1 << SecondaryMenuOption);
			
			FrameCounter = 1;
			return Button;
		}
		case B:
		{
			MenuSelectedOption = 0;
			
			FrameCounter = 1;
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
			
			adjustMenuSelectionVertical(Button, SecondaryMenuOption, 
				SecondaryPage, TotalMenuOptions, MaxOptionsPerPage, 
					MaxOptionsPerRow, false);
			
			FrameCounter = 1;
			return Button;
		}
		case A:
		{
			if (checkIfInGame())
			{
				int32_t FollowerId = getFollowerID();
				if (FollowerId >= 0)
				{
					// A follower is currently out, so remove it
					removeFollowerFromOverworld();
				}
				
				// Spawn the new follower
				uint32_t NewFollower = SecondaryMenuOption + 8; // Start at the egg
				
				// Spawn the follower
				int32_t ReturnCode = ttyd::party::partyEntry2Hello(
					static_cast<ttyd::party::PartyMembers>(NewFollower));
				
				// Set specific bytes
				if (ReturnCode >= 0)
				{
					#ifndef TTYD_JP
					ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
					player->wFollowerFlags[1] = ReturnCode; // 0x246
					player->prevFollowerId[1] = NewFollower; // 0x248
					#else
					uint32_t MarioPtr = reinterpret_cast<uint32_t>(ttyd::mario::marioGetPtr());
					*reinterpret_cast<uint8_t *>(MarioPtr + 0x242) = ReturnCode;
					*reinterpret_cast<uint8_t *>(MarioPtr + 0x244) = NewFollower;
					#endif
				}
				
				closeSecondaryMenu();
				
				FrameCounter = 1;
				return Button;
			}
			else
			{
				return 0;
			}
		}
		case B:
		{
			closeSecondaryMenu();
			
			FrameCounter = 1;
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
	int32_t tempMenuSecondaryValue = MenuSecondaryValue;
	
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
			MenuSecondaryValue = upperBound;
		}
		else
		{
			// Set to the lower bound by default
			MenuSecondaryValue = lowerBound;
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
				
				MenuSecondaryValue = NewValue;
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
				
				MenuSecondaryValue = NewValue;
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
				
				MenuSecondaryValue = NewValue;
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
				
				MenuSecondaryValue = NewValue;
			}
		}
	}
}*/

void adjustMenuItemBoundsMain(int32_t valueChangedBy, int32_t lowerBound, int32_t upperBound)
{
	int32_t tempMenuSecondaryValue = MenuSecondaryValue + valueChangedBy;
	MenuSecondaryValue = tempMenuSecondaryValue;
	
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
			MenuSecondaryValue = upperBound;
		}
		else
		{
			// Set to the lower bound by default
			MenuSecondaryValue = lowerBound;
		}
	}
	else if (tempMenuSecondaryValue < lowerBound)
	{
		// Loop to the end
		MenuSecondaryValue = upperBound;
	}
	else if (tempMenuSecondaryValue > upperBound)
	{
		// Loop to the beginning
		MenuSecondaryValue = lowerBound;
	}
}

void adjustMenuItemBounds(int32_t valueChangedBy, uint32_t currentMenu)
{
	int32_t tempMenuSecondaryValue = MenuSecondaryValue + valueChangedBy;
	switch (currentMenu)
	{
		case CHEATS_NPC_FORCE_DROP:
		{
			if ((tempMenuSecondaryValue > FreshJuice) && 
				(tempMenuSecondaryValue < PowerJump))
			{
				if (valueChangedBy > 0)
				{
					MenuSecondaryValue = PowerJump;
				}
				else
				{
					MenuSecondaryValue = FreshJuice;
				}
				return;
			}
			break;
		}
		case BATTLES_CURRENT_ACTOR:
		{
			if (SelectedOption == CHANGE_HELD_ITEM)
			{
				if (tempMenuSecondaryValue == TradeOff)
				{
					if (valueChangedBy > 0)
					{
						MenuSecondaryValue = tempMenuSecondaryValue + 1;
					}
					else
					{
						MenuSecondaryValue = tempMenuSecondaryValue - 1;
					}
					return;
				}
				else if ((tempMenuSecondaryValue > Cake) && 
					(tempMenuSecondaryValue < PowerJump))
				{
					if (valueChangedBy > 0)
					{
						MenuSecondaryValue = PowerJump;
					}
					else
					{
						MenuSecondaryValue = Cake;
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
	int32_t *tempArray = getUpperAndLowerBounds(UpperAndLowerBounds, currentMenu);
	int32_t LowerBound = tempArray[0];
	int32_t UpperBound = tempArray[1];
	
	adjustMenuItemBoundsMain(valueChangedBy, LowerBound, UpperBound);
}

void adjustAddByIdValue(int32_t value, uint32_t currentMenu)
{
	uint32_t HighestDigit = getHighestAdjustableValueDigit(currentMenu);
	int32_t newValue = value;
	
	for (uint32_t i = 0; i < (HighestDigit - SecondaryMenuOption - 1); i++)
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
	int32_t tempMenuSecondaryValue = MenuSecondaryValue;
	
	if ((currentMenuOption >= PIANTAS_STORED) && 
		(currentMenuOption <= CURRENT_PIANTAS))
	{
		uint32_t PiantaParlorPtr = *reinterpret_cast<uint32_t *>(PiantaParlorAddressesStart);
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
	
	int32_t tempMenuSecondaryValue = MenuSecondaryValue;
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
			break;;
		}
	}
}

void setBattlesActorValue(uint32_t currentMenuOption)
{
	uint32_t ActorAddress = reinterpret_cast<uint32_t>(getActorPointer(MenuSelectedOption));
	if (ActorAddress == 0)
	{
		return;
	}
	
	int32_t tempMenuSecondaryValue = MenuSecondaryValue;
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
			if (tempMenuSecondaryValue < PowerJump)
			{
				break;
			}
			
			// Do not modify for Mario or the partners
			uint32_t CurrentActorId = *reinterpret_cast<uint32_t *>(ActorAddress + 0x8);
			const uint32_t MarioId 	= 222;
			const uint32_t MowzId 	= 230;
			
			if ((CurrentActorId >= MarioId) && 
				(CurrentActorId <= MowzId))
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
	uint32_t ActorAddress = reinterpret_cast<uint32_t>(getActorPointer(MenuSelectedOption));
	if (ActorAddress == 0)
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
		0x118 + currentMenuOption + Counter)) = MenuSecondaryValue;
}

uint8_t getSelectedOptionPartnerValue()
{
	const uint8_t Goombella 	= 1;
	const uint8_t Koops 		= 2;
	const uint8_t Flurrie 		= 5;
	const uint8_t Yoshi 		= 4;
	const uint8_t Vivian 		= 6;
	const uint8_t Bobbery 		= 3;
	const uint8_t Mowz 			= 7;
	
	uint32_t tempSelectedOption = SelectedOption;
	uint32_t tempCurrentMenuOption = CurrentMenuOption;
	uint32_t tempOption;
	
	if (tempSelectedOption == 0)
	{
		if (tempCurrentMenuOption == 0)
		{
			return 0;
		}
		else
		{
			tempOption = tempCurrentMenuOption;
		}
	}
	else
	{
		tempOption = tempSelectedOption;
	}
	
	switch (tempOption)
	{
		case 1:
		{
			return Goombella;
		}
		case 2:
		{
			return Koops;
		}
		case 3:
		{
			return Flurrie;
		}
		case 4:
		{
			return Yoshi;
		}
		case 5:
		{
			return Vivian;
		}
		case 6:
		{
			return Bobbery;
		}
		case 7:
		{
			return Mowz;
		}
		default:
		{
			return 0;
		}
	}
}

void *getPartnerEnabledAddress()
{
	uint32_t PouchPtr = reinterpret_cast<uint32_t>(ttyd::mario_pouch::pouchGetPtr());
	uint32_t CurrentPartnerValue = getSelectedOptionPartnerValue();
	uint32_t PartnerEnabledAddress = PouchPtr + (CurrentPartnerValue * 0xE);
	return reinterpret_cast<void *>(PartnerEnabledAddress);
}

bool checkIfPartnerOutSelected()
{
	uint32_t PartnerPointer = reinterpret_cast<uint32_t>(getPartnerPointer());
	if (PartnerPointer == 0)
	{
		return false;
	}
	
	uint8_t CurrentPartnerOut = *reinterpret_cast<uint8_t *>(PartnerPointer + 0x31);
	if (CurrentPartnerOut == 0)
	{
		return false;
	}
	
	if (CurrentPartnerOut == getSelectedOptionPartnerValue())
	{
		return true;
	}
	else
	{
		return false;
	}
}

void setAddByIdValue(void *address)
{
	int32_t tempMenuSecondaryValue = MenuSecondaryValue;
	*reinterpret_cast<int16_t *>(
		reinterpret_cast<uint32_t>(address)) = tempMenuSecondaryValue;
	
	recheckUpgradesBattles(tempMenuSecondaryValue);
	resetImportantItemsPauseMenu();
}

void setAddByIconValue(void *address)
{
	int32_t UpperAndLowerBounds[2];
	int32_t *tempArray 	= getUpperAndLowerBounds(UpperAndLowerBounds, MenuSelectedOption);
	int32_t LowerBound 					= tempArray[0];
	uint32_t tempSecondaryMenuOption 	= SecondaryMenuOption;
	int32_t NewItem = LowerBound + tempSecondaryMenuOption;
	
	*reinterpret_cast<int16_t *>(
		reinterpret_cast<uint32_t>(address)) = NewItem;
	
	recheckUpgradesBattles(NewItem);
	resetImportantItemsPauseMenu();
}

void duplicateCurrentItem(void *address)
{
	uint32_t Address_and_Size[2];
	uint32_t *tempArray		= getPouchAddressAndSize(Address_and_Size);
	
	if (tempArray == nullptr)
	{
		return;
	}
	
	uint32_t tempAddress 	= tempArray[0];
	
	int16_t CurrentItem = *reinterpret_cast<int16_t *>(
		tempAddress + (CurrentMenuOption * 0x2));
	
	*reinterpret_cast<int16_t *>(
		reinterpret_cast<uint32_t>(address)) = CurrentItem;
	
	resetImportantItemsPauseMenu();
}

void deleteItem()
{
	uint32_t Address_and_Size[2];
	uint32_t *tempArray		= getPouchAddressAndSize(Address_and_Size);
	
	if (tempArray == nullptr)
	{
		return;
	}
	
	uint32_t tempAddress 	= tempArray[0];
	uint32_t tempSize 		= tempArray[1];
	
	uint32_t tempCurrentMenuOption = CurrentMenuOption;
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
	uint32_t *tempArray				= getPouchAddressAndSize(Address_and_Size);
	
	if (tempArray == nullptr)
	{
		return NULL_PTR;
	}
	
	uint32_t tempAddress 			= tempArray[0];
	
	uint32_t CurrentItemAddress 	= tempAddress + (CurrentMenuOption * 0x2);
	int16_t CurrentItem 			= *reinterpret_cast<int16_t *>(CurrentItemAddress);
	
	if (CurrentItem == 0)
	{
		return INVENTORY_EMPTY;
	}
	
	if (SelectedOption == CHANGE_BY_ID)
	{
		setAddByIdValue(reinterpret_cast<void *>(CurrentItemAddress));
	}
	else // SelectedOption == CHANGE_BY_ICON
	{
		setAddByIconValue(reinterpret_cast<void *>(CurrentItemAddress));
	}
	
	// Close the menu being used to select the new item
	MenuSelectionStates = 0;
	return 0;
}

int32_t getGW(uint32_t gw)
{
	ttyd::evtmgr::EvtWork *EventWork = getCurrentEventWork();
	return EventWork->gwData[gw];
}

void setGW(uint32_t gw, uint32_t value)
{
	ttyd::evtmgr::EvtWork *EventWork = getCurrentEventWork();
	EventWork->gwData[gw] = value;
}

bool getGF(uint32_t gf)
{
	ttyd::evtmgr::EvtWork *EventWork = getCurrentEventWork();
	uint32_t Size = sizeof(uint32_t) * 8;
	return (EventWork->gfData[gf / Size] >> (gf % Size)) & 1;
}

void setGF(uint32_t gf)
{
	ttyd::evtmgr::EvtWork *EventWork = getCurrentEventWork();
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

/*uint8_t *getButtonsPressedDynamic(uint8_t *buttonArray, uint16_t currentButtonCombo)
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
			if (buttonArray == nullptr)
			{
				buttonArray = new uint8_t[2]; // Extra spot for a 0 at the end of the array
				clearMemory(buttonArray, (2 * sizeof(uint8_t)));
			}
			else
			{
				Size++;
				
				// Create a new array with a new spot for the next value
				uint8_t *tempButtonArray = new uint8_t[Size + 1];
				clearMemory(tempButtonArray, ((Size + 1) * sizeof(uint8_t)));
				
				// Copy the contents of the old array to the new array
				ttyd::__mem::memcpy(tempButtonArray, buttonArray, ((Size - 1) * sizeof(uint8_t)));
				
				// Delete the old array
				delete[] (buttonArray);
				
				// Set the new array as the current array
				buttonArray = tempButtonArray;
			}
			
			buttonArray[Size - 1] = Counter + 1;
		}
		
		Counter++;
	}
	
	if (buttonArray != nullptr)
	{
		buttonArray[Size] = 0;
	}
	else
	{
		buttonArray = new uint8_t[1];
		buttonArray[0] = 0;
	}
	
	return buttonArray;
}*/

/*uint8_t *getButtonsPressedDynamic(uint8_t *buttonArray, uint16_t currentButtonCombo)
{
	if (buttonArray == nullptr)
	{
		buttonArray = new uint8_t[14]; // Extra spot for a 0 at the end of the array
	}
	
	// Clear the memory, so that the previous results do not interfere with the new results
	clearMemory(buttonArray, (14 * sizeof(uint8_t)));
	
	return getButtonsPressed(buttonArray, currentButtonCombo);
}*/

uint8_t *getButtonsPressed(uint8_t *buttonArray, uint16_t currentButtonCombo)
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
			buttonArray[Size] = Counter;
			Size++;
		}
		
		Counter++;
	}
	
	return buttonArray;
}

char *createButtonStringArray(char *tempArray, uint8_t *buttonArray)
{
	char *tempDisplayBuffer = DisplayBuffer;
	const char *Button = "";
	uint32_t i = 0;
	
	while (buttonArray[i] != 0)
	{
		switch (buttonArray[i])
		{
			case DPADLEFT:
			{
				Button = "D-Pad Left";
				break;
			}
			case DPADRIGHT:
			{
				Button = "D-Pad Right";
				break;
			}
			case DPADDOWN:
			{
				Button = "D-Pad Down";
				break;
			}
			case DPADUP:
			{
				Button = "D-Pad Up";
				break;
			}
			case Z:
			{
				Button = "Z";
				break;
			}
			case R:
			{
				Button = "R";
				break;
			}
			case L:
			{
				Button = "L";
				break;
			}
			case A:
			{
				Button = "A";
				break;
			}
			case B:
			{
				Button = "B";
				break;
			}
			case X:
			{
				Button = "X";
				break;
			}
			case Y:
			{
				Button = "Y";
				break;
			}
			case START:
			{
				Button = "Start";
				break;
			}
			default:
			{
				break;
			}
		}
		
		if (i == 0)
		{
			// Set the initial button pressed
			ttyd::string::strcpy(tempArray, Button);
		}
		else
		{
			// Add the next button pressed onto the first button pressed
			sprintf(tempDisplayBuffer,
				" + %s",
				Button);
			ttyd::string::strcat(tempArray, tempDisplayBuffer);
		}
		
		i++;
	}
	
	return tempArray;
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
			Timer = secondsToFrames(3);
			
			// Copy the values from the current buttons held to the previous buttons held
			ttyd::__mem::memcpy(CheatsDisplayButtons.CheatsPreviousButtonsHeld, 
				CheatsDisplayButtons.CheatsCurrentButtonsHeld, (14 * sizeof(uint8_t)));
			
			return false;
		}
		else
		{
			// Decrement the timer
			uint32_t tempTimer = Timer;
			tempTimer--;
			Timer = tempTimer;
			
			if (tempTimer == 0)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		// No buttons are currently held, so reset the timer
		Timer = secondsToFrames(3);
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
	uint32_t tempCurrentMenu 		= CurrentMenu;
	uint32_t tempTotalMenuOptions 	= Menu[tempCurrentMenu].TotalMenuOptions;
	uint32_t tempColumnSplitAmount 	= Menu[tempCurrentMenu].ColumnSplitAmount;
	uint32_t TotalPages = 1 + ((tempTotalMenuOptions - 1) / tempColumnSplitAmount); // Round up
	
	// Make sure there is more than one page
	if (TotalPages == 1)
	{
		return;
	}
	
	uint32_t tempCurrentMenuOption 	= CurrentMenuOption;
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
	uint32_t tempCurrentMenu 		= CurrentMenu;
	uint32_t tempTotalMenuColumns 	= Menu[tempCurrentMenu].TotalMenuColumns;
	
	// Pressing either of these causes the current option to jump to the last option when theres only one column - look into fixing
	if (((button == DPADLEFT) || (button == DPADRIGHT)) && 
		(tempTotalMenuColumns == 1))
	{
		return;
	}
	
	uint32_t tempTotalMenuOptions 	= Menu[tempCurrentMenu].TotalMenuOptions;
	uint32_t tempColumnSplitAmount 	= Menu[tempCurrentMenu].ColumnSplitAmount;
	uint32_t MaxOptionsPerPage 		= tempColumnSplitAmount * tempTotalMenuColumns;
	uint32_t MaxOptionsPerRow 		= tempTotalMenuColumns;
	
	adjustMenuSelectionVertical(button, CurrentMenuOption, 
		CurrentPage, tempTotalMenuOptions, MaxOptionsPerPage, 
			MaxOptionsPerRow, true);
}

/*void DPAD_Actions(uint32_t button)
{
	uint32_t tempCurrentMenu 		= CurrentMenu;
	uint32_t tempTotalMenuColumns 	= Menu[tempCurrentMenu].TotalMenuColumns;
	uint32_t tempCurrentMenuOption 	= CurrentMenuOption;
	uint32_t tempColumnSplitAmount 	= Menu[tempCurrentMenu].ColumnSplitAmount;
	uint32_t tempTotalMenuOptions 	= Menu[tempCurrentMenu].TotalMenuOptions;
	
	switch (button)
	{
		case DPADLEFT:
		{
			// Make sure the current menu has more than one column
			if (tempTotalMenuColumns == 1)
			{
				return;
			}
			else if (tempCurrentMenuOption == 0)
			{
				// Current option is Return, so do nothing
				return;
			}
			else if (tempCurrentMenuOption <= tempColumnSplitAmount)
			{
				// Currently on the furthest left side, so move to the furthest right option
				tempCurrentMenuOption = ((tempColumnSplitAmount * tempTotalMenuColumns) - 
					(tempColumnSplitAmount - tempCurrentMenuOption));
				
				// Make sure the current option is valid
				if (tempCurrentMenuOption > (tempTotalMenuOptions - 1))
				{
					// The current option exceeds the total options, so set the current option to the last option
					CurrentMenuOption = (tempTotalMenuOptions - 1);
				}
				else
				{
					CurrentMenuOption = tempCurrentMenuOption;
				}
			}
			else // if (tempCurrentMenuOption > tempColumnSplitAmount)
			{
				// Currently on the right side, so move to the next left option
				CurrentMenuOption = (tempCurrentMenuOption - tempColumnSplitAmount);
			}
			break;
		}
		case DPADRIGHT:
		{
			// Make sure the current menu has more than one column
			if (tempTotalMenuColumns == 1)
			{
				return;
			}
			else if (tempCurrentMenuOption == 0)
			{
				// Current option is Return, so do nothing
				return;
			}
			else if (tempCurrentMenuOption < ((tempColumnSplitAmount * 
				tempTotalMenuColumns) - (tempColumnSplitAmount - 1)))
			{
				// Currently not on the furthest right side, so move to the next right option
				tempCurrentMenuOption += tempColumnSplitAmount;
				
				// Make sure the current option is valid
				if (tempCurrentMenuOption > (tempTotalMenuOptions - 1))
				{
					// The current option exceeds the total options, so set the current option to the last option
					CurrentMenuOption = (tempTotalMenuOptions - 1);
				}
				else
				{
					CurrentMenuOption = tempCurrentMenuOption;
				}
			}
			else
			{
				// Currently on the furthest right side, so go to the furthest left option
				CurrentMenuOption = (tempCurrentMenuOption - 
					(tempColumnSplitAmount * (tempTotalMenuColumns - 1)));
			}
			break;
		}
		case DPADDOWN:
		{
			// Check if currently at the bottom of the current column
			if ((tempCurrentMenuOption != 0) && 
				(tempCurrentMenuOption % tempColumnSplitAmount == 0))
			{
				// Go to the top of the current column
				CurrentMenuOption = (tempCurrentMenuOption - tempColumnSplitAmount);
			}
			else if (tempCurrentMenuOption == (tempTotalMenuOptions - 1))
			{
				// Currently on the last option, so go to the first option of the last column
				if (tempTotalMenuColumns > 1)
				{
					CurrentMenuOption = (((tempTotalMenuColumns - 1) * 
						tempColumnSplitAmount) + 1);
				}
				else
				{
					// The current menu only has one column, so go to the first option
					CurrentMenuOption = 0;
				}
			}
			else
			{
				CurrentMenuOption = (tempCurrentMenuOption + 1);
			}
			break;
		}
		case DPADUP:
		{
			// Check if currently at the top of the current column
			if ((tempCurrentMenuOption == 0) || 
				(tempCurrentMenuOption % (tempColumnSplitAmount + 1) == 0))
			{
				// Loop to the last option in the current column
				tempCurrentMenuOption += tempColumnSplitAmount;
				
				// Make sure the current option is valid
				if (tempCurrentMenuOption > (tempTotalMenuOptions - 1))
				{
					// The current option exceeds the total options, so set the current option to the last option
					CurrentMenuOption = (tempTotalMenuOptions - 1);
				}
				else
				{
					CurrentMenuOption = tempCurrentMenuOption;
				}
			}
			else
			{
				CurrentMenuOption = (tempCurrentMenuOption - 1);
			}
			break;
		}
		default:
		{
			return;
		}
	}
}*/

void adjustMenuNoPageEdit(uint32_t button)
{
	uint32_t tempCurrentMenu 		= CurrentMenu;
	uint32_t tempTotalMenuColumns 	= Menu[tempCurrentMenu].TotalMenuColumns;
	
	// Pressing either of these causes the current option to jump to the last option when theres only one column - look into fixing
	if (((button == DPADLEFT) || (button == DPADRIGHT)) && 
		(tempTotalMenuColumns == 1))
	{
		return;
	}
	
	uint32_t tempTotalMenuOptions 	= Menu[tempCurrentMenu].TotalMenuOptions;
	uint32_t tempColumnSplitAmount 	= Menu[tempCurrentMenu].ColumnSplitAmount;
	uint32_t MaxOptionsPerPage 		= tempColumnSplitAmount * tempTotalMenuColumns;
	uint32_t MaxOptionsPerRow 		= tempTotalMenuColumns;
	uint8_t tempPage[1];
	tempPage[0] 					= 0;
	
	adjustMenuSelectionVertical(button, CurrentMenuOption, 
		tempPage[0], tempTotalMenuOptions, MaxOptionsPerPage, 
			MaxOptionsPerRow, true);
}

void adjustCheatsManageFlagsMainMenu(uint32_t button)
{
	// Pressing either of these causes the current option to jump to the last option when theres only one column - look into fixing
	if ((button == DPADLEFT) || (button == DPADRIGHT))
	{
		return;
	}
	
	uint32_t TotalMenuOptions;
	switch (MenuSelectionStates)
	{
		case SET_GSW:
		case SET_GW:
		case SET_LSW:
		{
			TotalMenuOptions = 4;
			break;
		}
		case SET_GSWF:
		case SET_GF:
		case SET_LSWF:
		default:
		{
			TotalMenuOptions = 3;
		}
	}
	
	uint32_t tempColumnSplitAmount 	= TotalMenuOptions;
	uint32_t MaxOptionsPerPage 		= tempColumnSplitAmount;
	uint32_t MaxOptionsPerRow 		= 1;
	uint8_t tempPage[1];
	tempPage[0] 					= 0;
	
	adjustMenuSelectionVertical(button, CurrentMenuOption, 
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
	uint32_t tempMenuSelectedOption 	= MenuSelectedOption;
	
	if ((tempMenuSelectedOption == INVENTORY_BADGES) || 
		(tempMenuSelectedOption == INVENTORY_STORED_ITEMS))
	{
		adjustMenuSelectionVertical(button, CurrentMenuOption, 
			CurrentPage, TotalMenuOptions, MaxOptionsPerPage, 
				MaxOptionsPerRow, true);
	}
	else
	{
		adjustMenuSelectionHorizontal(button, CurrentMenuOption, 
			CurrentPage, TotalMenuOptions, MaxOptionsPerPage, 
				MaxOptionsPerRow, true);
	}
}

/*void adjustAddByIconCurrentOption(uint32_t button)
{
	int32_t UpperAndLowerBounds[2];
	int32_t *tempArray 				= getUpperAndLowerBounds(UpperAndLowerBounds, MenuSelectedOption);
	int32_t LowerBound 				= tempArray[0];
	int32_t UpperBound 				= tempArray[1];
	
	uint32_t MaxIconsPerRow 		= 16;
	
	uint32_t tempCurrentMenuOption 	= CurrentMenuOption;
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
					CurrentMenuOption = tempCurrentMenuOption - TotalFreeSpaces;
				}
				else
				{
					CurrentMenuOption = tempCurrentMenuOption;
				}
			}
			else
			{
				CurrentMenuOption = tempCurrentMenuOption - 1;
			}
			break;
		}
		case DPADRIGHT:
		{
			if ((tempCurrentMenuOption % MaxIconsPerRow) == (MaxIconsPerRow - 1))
			{
				// Currently on the furthest right side, so go to the furthest left option
				CurrentMenuOption = tempCurrentMenuOption - (MaxIconsPerRow - 1);
			}
			else
			{
				tempCurrentMenuOption += 1;
				
				// Make sure the current option is valid
				if (tempCurrentMenuOption > (tempTotalMenuOptions - 1))
				{
					// Go to the furthest left option
					CurrentMenuOption = tempCurrentMenuOption - (MaxIconsPerRow - TotalFreeSpaces);
				}
				else
				{
					CurrentMenuOption = tempCurrentMenuOption;
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
				CurrentMenuOption = tempCurrentMenuOption % MaxIconsPerRow;
			}
			else
			{
				CurrentMenuOption = tempCurrentMenuOption;
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
					CurrentMenuOption = tempCurrentMenuOption - MaxIconsPerRow;
				}
				else
				{
					CurrentMenuOption = tempCurrentMenuOption;
				}
			}
			else
			{
				CurrentMenuOption = tempCurrentMenuOption - MaxIconsPerRow;
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
	uint32_t tempCheatsForceItemDropAreasSize = CheatsForceItemDropAreasSize;
	uint32_t TotalMenuOptions = tempCheatsForceItemDropAreasSize;
	uint32_t MaxOptionsPerRow = 4;
	uint32_t TotalRows = 1 + ((TotalMenuOptions - 1) / MaxOptionsPerRow); // Round up
	uint32_t MaxOptionsPerPage = TotalRows * MaxOptionsPerRow;
	
	adjustMenuSelectionVertical(button, SecondaryMenuOption, 
		SecondaryPage, TotalMenuOptions, MaxOptionsPerPage, 
			MaxOptionsPerRow, false);
}

void adjustMarioStatsSelection(uint32_t button)
{
	uint32_t tempStatsMarioOptionsLinesSize = StatsMarioOptionsLinesSize;
	uint32_t TotalMenuOptions = tempStatsMarioOptionsLinesSize;
	uint32_t MaxOptionsPerRow = 2;
	uint32_t TotalRows = 1 + ((TotalMenuOptions - 1) / MaxOptionsPerRow); // Round up
	uint32_t MaxOptionsPerPage = TotalRows * MaxOptionsPerRow;
	
	adjustMenuSelectionVertical(button, CurrentMenuOption, 
		CurrentPage, TotalMenuOptions, MaxOptionsPerPage, 
			MaxOptionsPerRow, true);
}

void adjustPartnerStatsSelection(uint32_t button)
{
	uint32_t tempStatsPartnerOptionsLinesSize = StatsPartnerOptionsLinesSize;
	uint32_t TotalMenuOptions = tempStatsPartnerOptionsLinesSize - 1;
	uint32_t MaxOptionsPerRow = 1;
	uint32_t TotalRows = 1 + ((TotalMenuOptions - 1) / MaxOptionsPerRow); // Round up
	uint32_t MaxOptionsPerPage = TotalRows * MaxOptionsPerRow;
	
	adjustMenuSelectionVertical(button, CurrentMenuOption, 
		CurrentPage, TotalMenuOptions, MaxOptionsPerPage, 
			MaxOptionsPerRow, false);
}

void adjustMemoryWatchSelection(uint32_t button)
{
	uint32_t TotalMenuOptions = 0;
	uint32_t Size = sizeof(MemoryWatch) / sizeof(MemoryWatch[0]);
	
	for (uint32_t i = 0; i < Size; i++)
	{
		if (MemoryWatch[i].Address == 0)
		{
			break;
		}
		TotalMenuOptions++;
	}
	
	uint32_t MaxOptionsPerRow = 1;
	uint32_t MaxOptionsPerPage = 9;
	
	adjustMenuSelectionVertical(button, CurrentMenuOption, 
		CurrentPage, TotalMenuOptions, MaxOptionsPerPage, 
			MaxOptionsPerRow, false);
}

void adjustMemoryChangeAddressOrPointerSelection(uint32_t button)
{
	uint32_t TotalMenuOptions = MemoryWatch[MenuSelectedOption].AddressOffsetAmount + 1;
	uint32_t MaxOptionsPerRow = 1;
	uint32_t MaxOptionsPerPage = 11;
	
	adjustMenuSelectionVertical(button, CurrentMenuOption, 
		CurrentPage, TotalMenuOptions, MaxOptionsPerPage, 
			MaxOptionsPerRow, false);
}

void adjustBattlesActorSelection(uint32_t button)
{
	// Get the highest slot in use
	int32_t Counter = 0;
	for (uint32_t i = 0; i < 62; i++)
	{
		if (getActorPointer(i + 1) == nullptr) // Add 1 to skip System
		{
			break;
		}
		else
		{
			Counter++;
		}
	}
	
	uint32_t tempCurrentMenuOption = CurrentMenuOption;
	uint32_t tempCurrentPage = CurrentPage;
	
	switch (button)
	{
		case DPADDOWN:
		{
			// Check to see if at the bottom of the current page
			if (tempCurrentMenuOption == (((tempCurrentPage + 1) * 13) - 1))
			{
				// Prevent going to the next page if there are no more free spaces
				if (Counter < static_cast<int32_t>((tempCurrentPage + 1) * 13))
				{
					// Go to the first page
					CurrentMenuOption = 0;
					CurrentPage = 0;
					return;
				}
			}
			break;
		}
		case DPADUP:
		{
			// Check to see if at the top of the current page
			if (tempCurrentMenuOption == (tempCurrentPage * 13))
			{
				// Go to the last page that has slots in use
				while (Counter >= static_cast<int32_t>((tempCurrentPage + 1) * 13))
				{
					tempCurrentPage++;
					tempCurrentMenuOption += 13;
					Counter -= 13;
				}
				
				CurrentPage = tempCurrentPage;
				CurrentMenuOption = tempCurrentMenuOption + 12;
				return;
			}
			break;
		}
		default:
		{
			break;
		}
	}
	
	uint32_t TotalMenuOptions = 62;
	uint32_t MaxOptionsPerRow = 1;
	uint32_t MaxOptionsPerPage = 13;
	
	adjustMenuSelectionVertical(button, CurrentMenuOption, 
		CurrentPage, TotalMenuOptions, MaxOptionsPerPage, 
			MaxOptionsPerRow, false);
}

void adjustBattlesStatusSelection(uint32_t button)
{
	uint32_t tempBattlesStatusesLinesSize = BattlesStatusesLinesSize;
	uint32_t TotalMenuOptions = tempBattlesStatusesLinesSize;
	uint32_t MaxOptionsPerRow = 1;
	uint32_t MaxOptionsPerPage = 12;
	
	adjustMenuSelectionVertical(button, CurrentMenuOption, 
		CurrentPage, TotalMenuOptions, MaxOptionsPerPage, 
			MaxOptionsPerRow, false);
}

void adjustWarpsSelection(uint32_t button)
{
	uint32_t tempWarpDestinationsSize = WarpDestinationsSize;
	uint32_t TotalMenuOptions = tempWarpDestinationsSize;
	uint32_t MaxOptionsPerRow = 4;
	uint32_t TotalRows = 1 + ((TotalMenuOptions - 1) / MaxOptionsPerRow); // Round up
	uint32_t MaxOptionsPerPage = TotalRows * MaxOptionsPerRow;
	
	adjustMenuSelectionVertical(button, CurrentMenuOption, 
		CurrentPage, TotalMenuOptions, MaxOptionsPerPage, 
			MaxOptionsPerRow, false);
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
	while (TotalColumns > totalMenuOptions)
	{
		TotalColumns--;
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