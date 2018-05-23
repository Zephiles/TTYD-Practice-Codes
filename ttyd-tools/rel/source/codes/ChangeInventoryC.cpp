#include "mod.h"

#include <ttyd/system.h>
#include <ttyd/mariost.h>
#include <ttyd/mario_pouch.h>

#include <cstdio>

extern uint32_t PauseMenuAddressesStart;
extern uint16_t InventoryAddItemOrBadgeCombo;
extern uint16_t InventoryRemoveItemOrBadgeCombo;
extern uint16_t InventoryPreviousValueCombo;
extern uint16_t InventoryNextValueCombo;
extern uint16_t InventoryAddItemOrBadgeFirstButtonCombo;
extern uint16_t InventoryRemoveItemOrBadgeFirstButtonCombo;
extern uint16_t InventoryPreviousValueFirstButtonCombo;
extern uint16_t InventoryNextValueFirstButtonCombo;

void addItemOrBadge(uint32_t PouchAddress, uint16_t ItemOrBadge, uint8_t LoopCounter, uint32_t SubMenu)
{
	if (SubMenu != 1)
	{
		// Not in important items/equipped badges
		for (int i = 0; i < LoopCounter; i++)
		{
			uint16_t tempItem = *reinterpret_cast<uint16_t *>(PouchAddress);
			if (tempItem != 0)
			{
				// Slot is not empty, so check next item
				PouchAddress += 2;
			}
			else
			{
				// Slot is empty, so add item
				*reinterpret_cast<uint16_t *>(PouchAddress) = ItemOrBadge;
				break;
			}
		}
	}
}

void removeItemOrBadge(uint32_t PouchAddress, uint32_t Index, uint8_t LoopCounter, bool BadgeFlag, uint32_t SubMenu)
{
	if (SubMenu != 1)
	{
		// Not in important items/equipped badges
		PouchAddress += Index * 2;
		
		for (int i = 0; i < LoopCounter - Index - 1; i++)
		{
			if (BadgeFlag)
			{
				// Modify equipped badges
				uint32_t tempAddress = PouchAddress + 0x190;
				*reinterpret_cast<uint16_t *>(tempAddress) = *reinterpret_cast<uint16_t *>(tempAddress + 2);
			}
			
			*reinterpret_cast<uint16_t *>(PouchAddress) = *reinterpret_cast<uint16_t *>(PouchAddress + 2);
			PouchAddress += 2;
		}
		
		*reinterpret_cast<uint16_t *>(PouchAddress)= 0;
	}
}

void incrementOrDecrementItemOrBadge(uint32_t PouchAddress, uint32_t Index, uint16_t UpperBound, uint16_t LowerBound, int8_t Value, bool BadgeFlag, uint32_t SubMenu)
{
	if (SubMenu != 1)
	{
		// Not in important items/equipped badges
		PouchAddress += Index * 2;
		uint16_t tempItem = *reinterpret_cast<uint16_t *>(PouchAddress);
		tempItem += Value;
		
		if (tempItem > UpperBound)
		{
			tempItem = LowerBound;
		}
		else if (tempItem < LowerBound)
		{
			tempItem = UpperBound;
		}
		
		if (BadgeFlag)
		{
			// Modify equipped badge
			uint32_t tempAddress = PouchAddress + 0x190;
			uint16_t tempEquippedItem = *reinterpret_cast<uint16_t *>(tempAddress);
			if (tempEquippedItem != 0)
			{
				// Badge is equipped, so needs to be changed
				*reinterpret_cast<uint16_t *>(tempAddress) = tempItem;
			}
		}
		
		*reinterpret_cast<uint16_t *>(PouchAddress) = tempItem;
	}
}

namespace mod {

void Mod::changeInventory()
{
	if (ttyd::mariost::marioStGetSystemLevel() == 15)
	{
		// Currently in the pause menu
		uint32_t ButtonInput = ttyd::system::keyGetButton(0);
		uint8_t PressedOrHeldButtonCount = 0;
		uint16_t ButtonHoldFrames = 45; // 0.75 Seconds
		
		uint32_t PauseMenuAddress = *reinterpret_cast<uint32_t *>(PauseMenuAddressesStart);
		uint32_t CursorItem = *reinterpret_cast<uint32_t *>(PauseMenuAddress + 0x138);
		uint32_t CurrentTab = *reinterpret_cast<uint32_t *>(PauseMenuAddress + 0x40);
		uint32_t SubMenuImportantItems = *reinterpret_cast<uint32_t *>(PauseMenuAddress + 0x210);
		uint32_t InventoryCurrentIndex = *reinterpret_cast<uint32_t *>(PauseMenuAddress + 0x214);
		uint32_t SubMenuEquippedBadges = *reinterpret_cast<uint32_t *>(PauseMenuAddress + 0x3E0);
		uint32_t BadgesCurrentIndex = *reinterpret_cast<uint32_t *>(PauseMenuAddress + 0x3E4);
		
		uint32_t PouchAddress = ttyd::mario_pouch::pouchGetPtr();
		uint32_t PouchStandardInventoryAddress = PouchAddress + 0x192;
		uint32_t PouchBadgeInventoryAddress = PouchAddress + 0x1FA;
		
		uint16_t GoldBar = 126;
		uint16_t PowerJump = 240;
		uint8_t StandardItemsLoopCounter = 20;
		uint8_t BadgesLoopCounter = 200;
		
		uint16_t StandardItemsUpperBound = 235;
		uint16_t StandardItemsLowerBound = GoldBar;
		uint16_t BadgesUpperBound = 338;
		uint16_t BadgesLowerBound = PowerJump;
		bool ItemAddresses = false;
		bool BadgeAddresses = true;
		
		// Set up first button pressed/held
		for (int i = 0; i < 13; i++)
		{
			if (ButtonInput & (1 << i))
			{
				PressedOrHeldButtonCount++;
			}
		}
		
		if (PressedOrHeldButtonCount < 2)
		{
			// Zero or one button(s) are pressed/held, so set FirstButtonPressed to 0 or currently-pressed/held button
			FirstButtonPressed = ButtonInput;
		}

		// Add item/badge
		if (((ButtonInput & InventoryAddItemOrBadgeCombo) == InventoryAddItemOrBadgeCombo) && (FirstButtonPressed == InventoryAddItemOrBadgeFirstButtonCombo))
		{
			if (ChangeInventoryButtonHoldCounter == 0)
			{
				// Add item/badge
				if (CurrentTab == 2)
				{
					// Standard items menu
					addItemOrBadge(PouchStandardInventoryAddress, GoldBar, StandardItemsLoopCounter, SubMenuImportantItems);
				}
				else if (CurrentTab == 3)
				{
					// Badges menu
					addItemOrBadge(PouchBadgeInventoryAddress, PowerJump, BadgesLoopCounter, SubMenuEquippedBadges);
				}
			}
			ChangeInventoryButtonHoldCounter++;
		}
		else if (((ButtonInput & InventoryRemoveItemOrBadgeCombo) == InventoryRemoveItemOrBadgeCombo) && (FirstButtonPressed == InventoryRemoveItemOrBadgeFirstButtonCombo))
		{
			if (ChangeInventoryButtonHoldCounter == 0)
			{
				// Remove item/badge
				if ((CurrentTab == 2) && (CursorItem != 0))
				{
					// Standard items menu, Cursor is over an item
					removeItemOrBadge(PouchStandardInventoryAddress, InventoryCurrentIndex, StandardItemsLoopCounter, ItemAddresses, SubMenuImportantItems);
				}
				else if ((CurrentTab == 3) && (CursorItem != 0))
				{
					// Badges menu, Cursor is over a badge
					removeItemOrBadge(PouchBadgeInventoryAddress, BadgesCurrentIndex, BadgesLoopCounter, BadgeAddresses, SubMenuEquippedBadges);
					ttyd::mario_pouch::pouchReviseMarioParam(); // Check equipped badges and adjust BP used
				}
			}
			ChangeInventoryButtonHoldCounter++;
		}
		else if (((ButtonInput & InventoryPreviousValueCombo) == InventoryPreviousValueCombo) && (FirstButtonPressed == InventoryPreviousValueFirstButtonCombo))
		{
			if (ChangeInventoryButtonHoldCounter == 0)
			{
				if ((CursorItem > 0) && (CursorItem < 236))
				{
					// Change standard items
					incrementOrDecrementItemOrBadge(PouchStandardInventoryAddress, InventoryCurrentIndex, StandardItemsUpperBound, StandardItemsLowerBound, -1, ItemAddresses, SubMenuImportantItems);
				}
				else if ((CursorItem >= 236) && (CursorItem <= 338))
				{
					// Change badges
					incrementOrDecrementItemOrBadge(PouchBadgeInventoryAddress, BadgesCurrentIndex, BadgesUpperBound, BadgesLowerBound, -1, BadgeAddresses, SubMenuEquippedBadges);
					ttyd::mario_pouch::pouchReviseMarioParam(); // Check equipped badges and adjust BP used
				}
			}
			ChangeInventoryButtonHoldCounter++;
		}
		else if (((ButtonInput & InventoryNextValueCombo) == InventoryNextValueCombo) && (FirstButtonPressed == InventoryNextValueFirstButtonCombo))
		{
			if (ChangeInventoryButtonHoldCounter == 0)
			{
				if ((CursorItem > 0) && (CursorItem < 236))
				{
					// Change standard items
					incrementOrDecrementItemOrBadge(PouchStandardInventoryAddress, InventoryCurrentIndex, StandardItemsUpperBound, StandardItemsLowerBound, 1, ItemAddresses, SubMenuImportantItems);
				}
				else if ((CursorItem >= 236) && (CursorItem <= 338))
				{
					// Change badges
					incrementOrDecrementItemOrBadge(PouchBadgeInventoryAddress, BadgesCurrentIndex, BadgesUpperBound, BadgesLowerBound, 1, BadgeAddresses, SubMenuEquippedBadges);
					ttyd::mario_pouch::pouchReviseMarioParam(); // Check equipped badges and adjust BP used
				}
			}
			ChangeInventoryButtonHoldCounter++;
		}
		else
		{
			// Reset counter if no button combo is pressed/held
			ChangeInventoryButtonHoldCounter = 0;
		}
		
		if (ChangeInventoryButtonHoldCounter > ButtonHoldFrames)
		{
			// Reset if counter exceeds ButtonHoldFrames
			ChangeInventoryButtonHoldCounter = 0;
		}
	}
}

}