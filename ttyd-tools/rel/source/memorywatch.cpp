#include "memorywatch.h"
#include "global.h"
#include "commonfunctions.h"
#include "menufunctions.h"

#include <ttyd/system.h>

#include <cstdio>
#include <cinttypes>
#include <cstring>

namespace mod {

int32_t getEmptyWatchSlot()
{
	int32_t Size = sizeof(MemoryWatch) / sizeof(MemoryWatch[0]);
	for (int32_t i = 0; i < Size; i++)
	{
		if (!MemoryWatch[i].Address)
		{
			return i;
		}
	}
	return NO_SLOTS_LEFT;
}

const char *getAddressStringNoLetterP(int32_t slot)
{
	char *tempDisplayBuffer = DisplayBuffer;
	sprintf(tempDisplayBuffer,
		"0x%08" PRIX32,
		MemoryWatch[slot].Address);
	
	return tempDisplayBuffer;
}

const char *getAddressString(int32_t slot)
{
	// Check if the current address uses pointers or not
	const char *tempPointerText;
	if (MemoryWatch[slot].AddressOffsetAmount > 0)
	{
		#ifdef TTYD_JP
		tempPointerText = PointerText;
		#else
		tempPointerText = "P=";
		#endif
	}
	else
	{
		tempPointerText = "";
	}
	
	char *tempDisplayBuffer = DisplayBuffer;
	sprintf(tempDisplayBuffer,
		"%s0x%08" PRIX32,
		tempPointerText,
		MemoryWatch[slot].Address);
	
	return tempDisplayBuffer;
}

const char *getAddressStringFromOffsets(int32_t slot, uint32_t maxOffset)
{
	int32_t OffsetArray[maxOffset];
	for (uint32_t i = 0; i < maxOffset; i++)
	{
		OffsetArray[i] = MemoryWatch[slot].AddressOffset[i];
	}
	
	uint32_t Address = reinterpret_cast<uint32_t>(
		getLastPointerFromPath(reinterpret_cast<void *>(
			MemoryWatch[slot].Address), OffsetArray, maxOffset));
	
	// Make sure the address is valid
	if (!checkIfPointerIsValid(reinterpret_cast<void *>(Address)))
	{
		// Address is invalid
		return "???";
	}
	
	// Make sure the address being read does not exceed 0x817FFFFF
	Address = reinterpret_cast<uint32_t>(fixBaseAddress(
		slot, reinterpret_cast<void *>(Address)));
	
	char *tempDisplayBuffer = DisplayBuffer;
	sprintf(tempDisplayBuffer,
		"0x%08" PRIX32,
		Address);
	
	return tempDisplayBuffer;
}

const char *getValueString(int32_t slot)
{
	uint32_t Address = reinterpret_cast<uint32_t>(
		getLastPointerFromPath(reinterpret_cast<void *>(MemoryWatch[slot].Address), 
			MemoryWatch[slot].AddressOffset, MemoryWatch[slot].AddressOffsetAmount));
	
	// Make sure the address is valid
	if (!checkIfPointerIsValid(reinterpret_cast<void *>(Address)))
	{
		// Address is invalid
		return "???";
	}
	
	// Make sure the address being read does not exceed 0x817FFFFF
	Address = reinterpret_cast<uint32_t>(fixBaseAddress(
		slot, reinterpret_cast<void *>(Address)));
	
	bool ShowAsHex = MemoryWatch[slot].ShowAsHex;
	char *tempDisplayBuffer = DisplayBuffer;
	
	const char *Format;
	switch (MemoryWatch[slot].Type)
	{
		case string:
		{
			return reinterpret_cast<const char *>(Address);
		}
		case time:
		{
			int64_t CurrentTime = *reinterpret_cast<int64_t *>(Address);
			return getTimeString(tempDisplayBuffer, CurrentTime);
		}
		case s8:
		{
			int8_t Value = *reinterpret_cast<int8_t *>(Address);
			if (ShowAsHex)
			{
				Format = "0x%02" PRIX8;
			}
			else
			{
				Format = "%" PRId8;
			}
			
			sprintf(tempDisplayBuffer,
				Format,
				Value);
			
			return tempDisplayBuffer;
		}
		case s16:
		{
			int16_t Value = *reinterpret_cast<int16_t *>(Address);
			if (ShowAsHex)
			{
				Format = "0x%04" PRIX16;
			}
			else
			{
				Format = "%" PRId16;
			}
			
			sprintf(tempDisplayBuffer,
				Format,
				Value);
			
			return tempDisplayBuffer;
		}
		case s32:
		{
			int32_t Value = *reinterpret_cast<int32_t *>(Address);
			if (ShowAsHex)
			{
				Format = "0x%08" PRIX32;
			}
			else
			{
				Format = "%" PRId32;
			}
			
			sprintf(tempDisplayBuffer,
				Format,
				Value);
			
			return tempDisplayBuffer;
		}
		case s64:
		{
			int64_t Value = *reinterpret_cast<int64_t *>(Address);
			if (ShowAsHex)
			{
				Format = "0x%016" PRIX64;
			}
			else
			{
				Format = "%" PRId64;
			}
			
			sprintf(tempDisplayBuffer,
				Format,
				Value);
			
			return tempDisplayBuffer;
		}
		case u8:
		{
			uint8_t Value = *reinterpret_cast<uint8_t *>(Address);
			if (ShowAsHex)
			{
				Format = "0x%02" PRIX8;
			}
			else
			{
				Format = "%" PRIu8;
			}
			
			sprintf(tempDisplayBuffer,
				Format,
				Value);
			
			return tempDisplayBuffer;
		}
		case u16:
		{
			uint16_t Value = *reinterpret_cast<uint16_t *>(Address);
			if (ShowAsHex)
			{
				Format = "0x%04" PRIX16;
			}
			else
			{
				Format = "%" PRIu16;
			}
			
			sprintf(tempDisplayBuffer,
				Format,
				Value);
			
			return tempDisplayBuffer;
		}
		case u32:
		{
			uint32_t Value = *reinterpret_cast<uint32_t *>(Address);
			if (ShowAsHex)
			{
				Format = "0x%08" PRIX32;
			}
			else
			{
				Format = "%" PRIu32;
			}
			
			sprintf(tempDisplayBuffer,
				Format,
				Value);
			
			return tempDisplayBuffer;
		}
		case u64:
		{
			uint64_t Value = *reinterpret_cast<uint64_t *>(Address);
			if (ShowAsHex)
			{
				Format = "0x%016" PRIX64;
			}
			else
			{
				Format = "%" PRIu64;
			}
			
			sprintf(tempDisplayBuffer,
				Format,
				Value);
			
			return tempDisplayBuffer;
		}
		case f32:
		{
			if (ShowAsHex)
			{
				sprintf(tempDisplayBuffer,
					"0x%08" PRIX32,
					*reinterpret_cast<uint32_t *>(Address));
			}
			else
			{
				sprintf(tempDisplayBuffer,
					"%.2f",
					*reinterpret_cast<float *>(Address));
			}
			return tempDisplayBuffer;
		}
		case f64:
		{
			if (ShowAsHex)
			{
				sprintf(tempDisplayBuffer,
					"0x%016" PRIX64,
					*reinterpret_cast<uint64_t *>(Address));
			}
			else
			{
				sprintf(tempDisplayBuffer,
					"%.2f",
					*reinterpret_cast<double *>(Address));
			}
			return tempDisplayBuffer;
		}
		default:
		{
			return "";
		}
	}
}

void addMemoryWatch(int32_t slot)
{
	MemoryWatch[slot].Address 		= 0x80000000;
	MemoryWatch[slot].Type 			= string;
	MemoryWatch[slot].ShowAsHex 	= false;
	MemoryWatch[slot].Display 		= false;
	MemoryWatch[slot].PosX 			= -44;
	MemoryWatch[slot].PosY 			= -32;
}

void deleteWatch(int32_t slot)
{
	uint32_t TotalOptionsPerPage = 10;
	int32_t TotalSlots = sizeof(MemoryWatch) / sizeof(MemoryWatch[0]);
	int32_t TotalMenuOptions = 0;
	
	for (int32_t i = 0; i < TotalSlots; i++)
	{
		if (!MemoryWatch[i].Address)
		{
			break;
		}
		TotalMenuOptions++;
	}
	
	// Adjust for 0 index
	TotalMenuOptions--;
	
	if (slot < TotalMenuOptions)
	{
		uint32_t Size = sizeof(MemoryWatch[0]) * (TotalMenuOptions - slot);
		memcpy(&MemoryWatch[slot], &MemoryWatch[slot + 1], Size);
	}
	
	clearMemory(&MemoryWatch[TotalMenuOptions], sizeof(MemoryWatch[0]));
	TotalMenuOptions--;
	
	// Check if there are any watches left
	if (TotalMenuOptions < 0)
	{
		closeSecondaryMenu();
		return;
	}
	
	// Make sure the currently selected option is valid
	uint32_t tempCurrentMenuOption = MenuVar.CurrentMenuOption;
	if (tempCurrentMenuOption > static_cast<uint32_t>(TotalMenuOptions))
	{
		MenuVar.CurrentMenuOption = tempCurrentMenuOption - 1;
	}
	
	// Make sure the current page is valid
	uint32_t tempCurrentPage = MenuVar.CurrentPage;
	if ((tempCurrentPage * TotalOptionsPerPage) > static_cast<uint32_t>(TotalMenuOptions))
	{
		MenuVar.CurrentPage = tempCurrentPage - 1;
	}
}

void duplicateWatch(int32_t currentSlot, int32_t emptySlot)
{
	uint32_t Size = sizeof(MemoryWatch[0]);
	memcpy(&MemoryWatch[emptySlot], &MemoryWatch[currentSlot], Size);
}

uint32_t adjustWatchValueControls(int32_t slot)
{
	uint32_t tempCurrentMenu 		= MenuVar.CurrentMenu;
	uint32_t tempCurrentMenuOption 	= MenuVar.CurrentMenuOption;
	
	// Get the amount of numbers to draw
	bool NoNumbersToDisplay = false;
	uint32_t HighestDigit = 0;
	uint32_t Button = 0;
	
	switch (tempCurrentMenu)
	{
		case MEMORY_CHANGE_ADDRESS:
		{
			HighestDigit = 8;
			break;
		}
		default:
		{
			NoNumbersToDisplay = true;
			Button = B;
			break;
		}
	}
	
	// Make sure there is at least 1 number to display
	if (!NoNumbersToDisplay)
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
			if (MemoryWatchAdjustableValueMenu.WaitFramesToBeginIncrement >= 
				ttyd::system::sysMsec2Frame(500))
			{
				// Check to see if the number should increment or not
				if (MemoryWatchAdjustableValueMenu.WaitFramesToPerformIncrement >= 1)
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
					
					adjustWatchTempValueAndBounds(slot, HighestDigit, IncrementAmount);
					MemoryWatchAdjustableValueMenu.WaitFramesToPerformIncrement = 0;
					return Button;
				}
				else
				{
					MemoryWatchAdjustableValueMenu.WaitFramesToPerformIncrement++;
				}
			}
			else
			{
				MemoryWatchAdjustableValueMenu.WaitFramesToBeginIncrement++;
			}
		}
		else
		{
			// Reset the counters
			MemoryWatchAdjustableValueMenu.WaitFramesToBeginIncrement = 0;
			MemoryWatchAdjustableValueMenu.WaitFramesToPerformIncrement = 0;
		}
		
		Button = checkButtonSingleFrame();
	}
	
	switch (Button)
	{
		case DPADLEFT:
		{
			uint32_t tempSecondaryMenuOption = MenuVar.SecondaryMenuOption;
			if (tempSecondaryMenuOption == 0)
			{
				// Loop to the last option
				MenuVar.SecondaryMenuOption = HighestDigit - 1;
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
			if (tempSecondaryMenuOption == (HighestDigit - 1))
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
			// Decrement the current value for the current slot
			adjustWatchTempValueAndBounds(slot, HighestDigit, -1);
			
			MenuVar.FrameCounter = 1;
			return Button;
		}
		case DPADUP:
		{
			// Increment the current value for the current slot
			adjustWatchTempValueAndBounds(slot, HighestDigit, 1);
			
			MenuVar.FrameCounter = 1;
			return Button;
		}
		case A:
		{
			switch (tempCurrentMenu)
			{
				case MEMORY_CHANGE_ADDRESS:
				{
					switch (tempCurrentMenuOption)
					{
						case 0:
						{
							// Modifying the address
							// Make sure the address being read does not exceed 0x817FFFFF
							MemoryWatch[slot].Address = reinterpret_cast<uint32_t>(
								fixBaseAddress(slot, reinterpret_cast<void *>(MenuVar.MemoryWatchSecondaryValue)));
							
							MenuVar.MenuSelectionStates = 0;
							
							MenuVar.FrameCounter = 1;
							return Button;
						}
						default:
						{
							// Modifying the pointer offsets
							MemoryWatch[slot].AddressOffset[tempCurrentMenuOption - 1] = 
								static_cast<int32_t>(MenuVar.MemoryWatchSecondaryValue);
							MenuVar.MenuSelectionStates = 0;
							
							MenuVar.FrameCounter = 1;
							return Button;
						}
					}
					break;
				}
				default:
				{
					return 0;
				}
			}
			break;
		}
		case B:
		{
			switch (tempCurrentMenu)
			{
				case MEMORY_CHANGE_ADDRESS:
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
				default:
				{
					return 0;
				}
			}
			break;
		}
		default:
		{
			return 0;
		}
	}
}

void adjustWatchTempValueAndBounds(int32_t slot, uint32_t highestDigit, int32_t valueChangedBy)
{
	for (uint32_t i = 0; i < (highestDigit - MenuVar.SecondaryMenuOption - 1); i++)
	{
		valueChangedBy *= 0x10;
	}
	
	uint32_t tempMemoryWatchSecondaryValue = MenuVar.MemoryWatchSecondaryValue + valueChangedBy;
	MenuVar.MemoryWatchSecondaryValue = tempMemoryWatchSecondaryValue;
	
	switch (MenuVar.CurrentMenu)
	{
		case MEMORY_CHANGE_ADDRESS:
		{
			uint32_t UpperBoundUnsigned;
			uint32_t LowerBoundUnsigned;
			int32_t UpperBoundSigned;
			int32_t LowerBoundSigned;
			
			switch (MenuVar.CurrentMenuOption)
			{
				case 0:
				{
					// Modifying the address
					// Make sure the upper bound does not exceed 0x817FFFFF
					UpperBoundUnsigned = reinterpret_cast<uint32_t>(
						fixBaseAddress(slot, reinterpret_cast<void *>(0x817FFFFF)));
					
					LowerBoundUnsigned = 0x80000000;
					
					if (tempMemoryWatchSecondaryValue > UpperBoundUnsigned)
					{
						// Loop to the beginning
						MenuVar.MemoryWatchSecondaryValue = LowerBoundUnsigned;
					}
					else if (tempMemoryWatchSecondaryValue < LowerBoundUnsigned)
					{
						// Loop to the end
						MenuVar.MemoryWatchSecondaryValue = UpperBoundUnsigned;
					}
					break;
				}
				default:
				{
					// Modifying the pointer offsets
					UpperBoundSigned = 0x7FFFFFFF;
					LowerBoundSigned = -0x80000000;
					
					int32_t tempMemoryWatchSecondaryValueSigned = static_cast<int32_t>(tempMemoryWatchSecondaryValue);
					if (tempMemoryWatchSecondaryValueSigned > UpperBoundSigned)
					{
						// Loop to the beginning
						MenuVar.MemoryWatchSecondaryValue = static_cast<uint32_t>(LowerBoundSigned);
					}
					else if (tempMemoryWatchSecondaryValueSigned < LowerBoundSigned)
					{
						// Loop to the end
						MenuVar.MemoryWatchSecondaryValue = static_cast<uint32_t>(UpperBoundSigned);
					}
					break;
				}
			}
			break;
		}
		default:
		{
			return;
		}
	}
}

void *fixBaseAddress(int32_t slot, void *address)
{
	uint32_t CurrentTypeSize;	
	switch (MemoryWatch[slot].Type)
	{
		case time:
		case s64:
		case u64:
		case f64:
		{
			CurrentTypeSize = 8;
			break;
		}
		case s32:
		case u32:
		case f32:
		{
			CurrentTypeSize = 4;
			break;
		}
		case s16:
		case u16:
		{
			CurrentTypeSize = 2;
			break;
		}
		case s8:
		case u8:
		default:
		{
			CurrentTypeSize = 1;
			break;
		}
	}
	
	// Make sure the address does not exceed 0x817FFFFF
	uint32_t tempAddress = reinterpret_cast<uint32_t>(address);
	while ((tempAddress + CurrentTypeSize - 1) >= 0x81800000)
	{
		tempAddress--;
	}
	
	return reinterpret_cast<void *>(tempAddress);
}

}