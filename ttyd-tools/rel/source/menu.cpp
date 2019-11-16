#include "menu.h"
#include "menufunctions.h"
#include "global.h"
#include "commonfunctions.h"
#include "memcard.h"
#include "memorywatch.h"
#include "codes.h"
#include "draw.h"

#include <gc/card.h>
#include <ttyd/mariost.h>
#include <ttyd/seq_mapchange.h>
#include <ttyd/swdrv.h>
#include <ttyd/evt_yuugijou.h>
#include <ttyd/mario_pouch.h>
#include <ttyd/win_main.h>
#include <ttyd/party.h>

namespace mod {

void menuCheckButton()
{
	// Only run if a button was pressed
	uint32_t CurrentButton 					= checkButtonSingleFrame();
	if (CurrentButton == 0)
	{
		return;
	}
	
	uint32_t tempCurrentMenu 				= MenuVar.CurrentMenu;
	uint32_t tempCurrentMenuOption 			= MenuVar.CurrentMenuOption;
	// uint32_t tempSecondaryMenuOption 	= MenuVar.SecondaryMenuOption;
	// uint32_t tempTotalMenuColumns 		= Menu[tempCurrentMenu].TotalMenuColumns;
	// uint32_t tempColumnSplitAmount 		= Menu[tempCurrentMenu].ColumnSplitAmount;
	// uint32_t tempTotalMenuOptions 		= Menu[tempCurrentMenu].TotalMenuOptions;
	uint32_t tempSelectedOption 			= MenuVar.SelectedOption;
	uint32_t tempCurrentPage 				= MenuVar.CurrentPage;
	uint32_t tempMenuSelectionStates 		= MenuVar.MenuSelectionStates;
	uint32_t tempMenuSelectedOption 		= MenuVar.MenuSelectedOption;
	// uint32_t tempMenuSecondaryValue 		= MenuVar.MenuSecondaryValue;
	
	switch (tempCurrentMenu)
	{
		case ROOT:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					adjustMenuNoPageEdit(CurrentButton);
					break;
				}
				case A:
				{
					uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
					switch (CurrentMenuOptionCheck)
					{
						case BATTLES:
						{
							// Make sure the player is currently in a battle
							if (getBattleWorkPointer())
							{
								// Currently in a battle, so enter the next menu
								enterNextMenu(BATTLES, tempCurrentMenuOption);
								resetMenu();
							}
							else
							{
								MenuVar.FunctionReturnCode = NOT_IN_BATTLE;
								MenuVar.Timer = secondsToFrames(3);
							}
							break;
						}
						default:
						{
							// Enter the next menu
							enterNextMenu(CurrentMenuOptionCheck, tempCurrentMenuOption);
							resetMenu();
							break;
						}
					}
					break;
				}
				case B:
				{
					// Close the menu
					closeMenu();
					return;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case INVENTORY:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					adjustMenuNoPageEdit(CurrentButton);
					break;
				}
				case A:
				{
					// Enter the next menu
					MenuVar.MenuSelectedOption = tempCurrentMenuOption;
					enterNextMenu(INVENTORY_MAIN, tempCurrentMenuOption);
					resetMenu();
					break;
				}
				case B:
				{
					// Go back to the previous menu
					resetMenu();
					enterPreviousMenu();
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case INVENTORY_MAIN:
		{
			uint32_t MaxOptionsPerPage = 20;
			switch (CurrentButton)
			{
				case DPADLEFT:
				case DPADRIGHT:
				case DPADDOWN:
				case DPADUP:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							adjustMenuNoPageEdit(CurrentButton);
							break;
						}
						case DUPLICATE:
						case DELETE:
						{
							adjustMenuSelectionInventory(CurrentButton);
							break;
						}
						case CHANGE_BY_ID:
						case CHANGE_BY_ICON:
						{
							if (tempMenuSelectionStates == 0)
							{
								adjustMenuSelectionInventory(CurrentButton);
							}
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case R:
				{
					if (checkForItemsOnNextPage(tempCurrentPage))
					{
						// Go to the next page
						if (tempMenuSelectionStates == 0)
						{
							tempCurrentPage++;
							MenuVar.CurrentPage = tempCurrentPage;
							
							if ((tempSelectedOption >= DUPLICATE) && 
								(tempSelectedOption <= DELETE))
							{
								MenuVar.CurrentMenuOption = tempCurrentPage * MaxOptionsPerPage;
							}
						}
					}
					break;
				}
				case L:
				{
					if (tempCurrentPage > 0)
					{
						// Go to the previous page
						if (tempMenuSelectionStates == 0)
						{
							tempCurrentPage--;
							MenuVar.CurrentPage = tempCurrentPage;
							
							if ((tempSelectedOption >= DUPLICATE) && 
								(tempSelectedOption <= DELETE))
							{
								MenuVar.CurrentMenuOption = tempCurrentPage * MaxOptionsPerPage;
							}
						}
					}
					break;
				}
				case A:
				{
					correctInventoryCurrentMenuOptionAndPage(MaxOptionsPerPage);
					switch (tempSelectedOption)
					{
						case 0:
						{
							uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
							switch (CurrentMenuOptionCheck)
							{
								case ADD_BY_ID:
								{
									if (getFreeSlotPointer())
									{
										MenuVar.MenuSecondaryValue 		= 0;
										MenuVar.SecondaryMenuOption 	= getHighestAdjustableValueDigit(tempMenuSelectedOption) - 1;
										MenuVar.Timer 					= 0;
										MenuVar.SelectedOption 			= CurrentMenuOptionCheck;
									}
									else
									{
										// Inventory is currently full
										MenuVar.FunctionReturnCode 		= INVENTORY_FULL;
										MenuVar.Timer 					= secondsToFrames(3);
										MenuVar.SelectedOption 			= CurrentMenuOptionCheck;
									}
									break;
								}
								case ADD_BY_ICON:
								{
									if (getFreeSlotPointer())
									{
										MenuVar.SecondaryMenuOption 	= 0;
										MenuVar.Timer 					= 0;
										MenuVar.SelectedOption 			= CurrentMenuOptionCheck;
									}
									else
									{
										// Inventory is currently full
										MenuVar.FunctionReturnCode 		= INVENTORY_FULL;
										MenuVar.Timer 					= secondsToFrames(3);
										MenuVar.SelectedOption 			= CurrentMenuOptionCheck;
									}
									break;
								}
								case DUPLICATE:
								{
									if (getTotalItems() > 0)
									{
										if (getFreeSlotPointer())
										{
											MenuVar.CurrentMenuOption 	= tempCurrentPage * MaxOptionsPerPage;
											MenuVar.Timer 				= 0;
											MenuVar.SelectedOption 		= CurrentMenuOptionCheck;
										}
										else
										{
											// Inventory is currently full
											MenuVar.FunctionReturnCode 	= INVENTORY_FULL;
											MenuVar.Timer 				= secondsToFrames(3);
											MenuVar.SelectedOption 		= CurrentMenuOptionCheck;
										}
									}
									else
									{
										// Inventory is currently empty
										MenuVar.FunctionReturnCode 		= INVENTORY_EMPTY;
										MenuVar.Timer 					= secondsToFrames(3);
										MenuVar.SelectedOption 			= CurrentMenuOptionCheck;
									}
									break;
								}
								case CHANGE_BY_ID:
								case CHANGE_BY_ICON:
								case DELETE:
								{
									if (getTotalItems() > 0)
									{
										MenuVar.CurrentMenuOption 		= tempCurrentPage * MaxOptionsPerPage;
										MenuVar.Timer 					= 0;
										MenuVar.SelectedOption 			= CurrentMenuOptionCheck;
									}
									else
									{
										// Inventory is currently empty
										MenuVar.FunctionReturnCode 		= INVENTORY_EMPTY;
										MenuVar.Timer 					= secondsToFrames(3);
										MenuVar.SelectedOption 			= CurrentMenuOptionCheck;
									}
									break;
								}
								default:
								{
									break;
								}
							}
							break;
						}
						case ADD_BY_ID:
						case ADD_BY_ICON:
						{
							if (getFreeSlotPointer())
							{
								MenuVar.Timer = 0;
							}
							else
							{
								// Inventory is currently full
								MenuVar.FunctionReturnCode = INVENTORY_FULL;
								MenuVar.Timer = secondsToFrames(3);
							}
							break;
						}
						case DUPLICATE:
						{
							if (getTotalItems() > 0)
							{
								MenuVar.Timer = 0;
								
								void *tempAddress = getFreeSlotPointer();
								if (tempAddress)
								{
									duplicateCurrentItem(tempAddress);
									
									// Recheck the inventory
									if (!getFreeSlotPointer())
									{
										// No more free spaces, so leave the duplicate menu
										closeSecondaryMenu();
									}
								}
								else
								{
									// Inventory is currently full
									MenuVar.FunctionReturnCode = INVENTORY_FULL;
									MenuVar.Timer = secondsToFrames(3);
								}
							}
							else
							{
								// Inventory is currently empty
								MenuVar.FunctionReturnCode 	= INVENTORY_EMPTY;
								MenuVar.Timer 				= secondsToFrames(3);
							}
							break;
						}
						case CHANGE_BY_ID:
						{
							if (getTotalItems() > 0)
							{
								uint32_t Address_and_Size[2];
								uint32_t *tempArray	= getPouchAddressAndSize(Address_and_Size);
								
								if (tempArray)
								{
									uint32_t tempAddress = tempArray[0];
									uint32_t CurrentItemAddress = tempAddress + (tempCurrentMenuOption * 0x2);
									int16_t CurrentItem = *reinterpret_cast<int16_t *>(CurrentItemAddress);
									MenuVar.MenuSecondaryValue = CurrentItem;
								}
								else
								{
									MenuVar.MenuSecondaryValue = 0;
								}
								
								MenuVar.Timer = 0;
								if (tempMenuSelectionStates == 0)
								{
									MenuVar.MenuSelectionStates = tempSelectedOption;
									MenuVar.SecondaryMenuOption = getHighestAdjustableValueDigit(tempMenuSelectedOption) - 1;
								}
							}
							else
							{
								// Inventory is currently empty
								MenuVar.FunctionReturnCode 	= INVENTORY_EMPTY;
								MenuVar.Timer 				= secondsToFrames(3);
							}
							break;
						}
						case CHANGE_BY_ICON:
						{
							if (getTotalItems() > 0)
							{
								uint32_t Address_and_Size[2];
								uint32_t *tempArray	= getPouchAddressAndSize(Address_and_Size);
								
								if (tempArray)
								{
									uint32_t tempAddress = tempArray[0];
									uint32_t CurrentItemAddress = tempAddress + (tempCurrentMenuOption * 0x2);
									int16_t CurrentItem = *reinterpret_cast<int16_t *>(CurrentItemAddress);
									
									if (CurrentItem != 0)
									{
										int32_t UpperAndLowerBounds[2];
										getUpperAndLowerBounds(UpperAndLowerBounds, tempMenuSelectedOption);
										
										int32_t LowerBound = UpperAndLowerBounds[0];
										MenuVar.SecondaryMenuOption = CurrentItem - LowerBound;
									}
									else
									{
										MenuVar.SecondaryMenuOption = 0;
									}
								}
								else
								{
									MenuVar.SecondaryMenuOption = 0;
								}
								
								MenuVar.Timer = 0;
								if (tempMenuSelectionStates == 0)
								{
									MenuVar.MenuSelectionStates = tempSelectedOption;
								}
							}
							else
							{
								// Inventory is currently empty
								MenuVar.FunctionReturnCode 	= INVENTORY_EMPTY;
								MenuVar.Timer 				= secondsToFrames(3);
							}
							break;
						}
						case DELETE:
						{
							if (getTotalItems() > 0)
							{
								MenuVar.Timer = 0;
								deleteItem();
								
								// Recheck the inventory
								if (getTotalItems() <= 0)
								{
									// Inventory is currently empty, so leave the delete menu
									closeSecondaryMenu();
								}
							}
							else
							{
								// Inventory is currently empty
								MenuVar.FunctionReturnCode 	= INVENTORY_EMPTY;
								MenuVar.Timer 				= secondsToFrames(3);
							}
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case B:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							// Go back to the previous menu
							MenuVar.MenuSelectedOption = 0;
							resetMenu();
							enterPreviousMenu();
							break;
						}
						case CHANGE_BY_ID:
						case CHANGE_BY_ICON:
						{
							if (tempMenuSelectionStates == 0)
							{
								resetAndCloseSecondaryMenu();
							}
							break;
						}
						default:
						{
							resetAndCloseSecondaryMenu();
							break;
						}
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case CHEATS:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					default_DPAD_Actions(CurrentButton);
					break;
				}
				case A:
				{
					// Enter the next menu
					uint32_t MenuToEnter;
					switch (tempCurrentMenuOption)
					{
						case CHANGE_SEQUENCE:
						{
							MenuToEnter = CHEATS_CHANGE_SEQUENCE;
							break;
						}
						case WALK_THROUGH_WALLS:
						case FALL_THROUGH_FLOORS:
						case SAVE_COORDINATES:
						case LOAD_COORDINATES:
						case SPAWN_ITEM:
						case SAVE_ANYWHERE:
						case TEXT_STORAGE:
						case TIME_STOP_TEXT_STORAGE:
						case SPEED_UP_MARIO:
						case DISABLE_BATTLES:
						case AUTO_ACTION_COMMANDS:
						case INFINITE_ITEM_USAGE:
						case RELOAD_ROOM:
						case LEVITATE:
						{
							MenuToEnter = CHEATS_STANDARD;
							break;
						}
						case LOCK_MARIO_HP_TO_MAX:
						case RUN_FROM_BATTLES:
						case DISABLE_MENU_SOUNDS:
						case BOBBERY_EARLY:
						{
							MenuToEnter = CHEATS_NO_BUTTON_COMBO;
							break;
						}
						case FORCE_ITEM_DROP:
						{
							MenuToEnter = CHEATS_NPC_FORCE_DROP;
							break;
						}
						case RESOLVE_FADES:
						{
							MenuToEnter = CHEATS_RESOLVE_FADES;
							break;
						}
						case LOCK_FLAGS:
						{
							MenuToEnter = CHEATS_LOCK_FLAGS;
							break;
						}
						case MANAGE_FLAGS:
						{
							MenuToEnter = CHEATS_MANAGE_FLAGS;
							break;
						}
						case CLEAR_AREA_FLAGS:
						default:
						{
							MenuToEnter = CHEATS_CLEAR_AREA_FLAGS;
							break;
						}
					}
					
					MenuVar.MenuSelectedOption = tempCurrentMenuOption;
					enterNextMenu(MenuToEnter, tempCurrentMenuOption);
					resetMenu();
					break;
				}
				case B:
				{
					// Go back to the previous menu
					resetMenu();
					enterPreviousMenu();
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case CHEATS_CHANGE_SEQUENCE:
		{
			switch (CurrentButton)
			{
				case A:
				{
					if (tempMenuSelectionStates == 0)
					{
						MenuVar.MenuSecondaryValue 	= static_cast<int32_t>(getSequencePosition());
						MenuVar.MenuSelectionStates = CHANGE_SEQUENCE_VALUE;
						MenuVar.SecondaryMenuOption = getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
					}
					break;
				}
				case B:
				{
					if (tempMenuSelectionStates != CHANGE_SEQUENCE_VALUE)
					{
						// Go back to the previous menu
						MenuVar.MenuSelectedOption = 0;
						enterPreviousMenu();
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case CHEATS_STANDARD:
		case CHEATS_NO_BUTTON_COMBO:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					if (tempCurrentMenu == CHEATS_STANDARD)
					{
						adjustMenuNoPageEdit(CurrentButton);
					}
					break;
				}
				case A:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
							switch (CurrentMenuOptionCheck)
							{
								case TURN_ON_OR_OFF:
								{
									// Flip the bool for the current cheat
									bool CheatActive = Cheat[tempMenuSelectedOption].Active;
									Cheat[tempMenuSelectedOption].Active = !CheatActive;
									break;
								}
								case CHANGE_BUTTON_COMBO:
								{
									MenuVar.ChangingCheatButtonCombo 	= true;
									MenuVar.Timer 						= secondsToFrames(3);
									MenuVar.SelectedOption 				= CurrentMenuOptionCheck;
									break;
								}
								default:
								{
									break;
								}
							}
							break;
						}
						default:
						{
							// Currently changing a cheat button combo, so do nothing
							break;
						}
					}
					break;
				}
				case B:
				{
					switch (tempSelectedOption)
					{
						case CHANGE_BUTTON_COMBO:
						{
							// The work for this is done in drawChangeButtonCombo
							break;
						}
						default:
						{
							// Go back to the previous menu
							MenuVar.MenuSelectedOption = 0;
							enterPreviousMenu();
							break;
						}
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case CHEATS_NPC_FORCE_DROP:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					if (tempMenuSelectionStates == 0)
					{
						adjustMenuNoPageEdit(CurrentButton);
					}
					break;
				}
				case A:
				{
					uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
					switch (CurrentMenuOptionCheck)
					{
						case ITEM_DROP_TURN_ON_OR_OFF:
						{
							// Flip the bool for the current cheat
							bool CheatActive = Cheat[tempMenuSelectedOption].Active;
							Cheat[tempMenuSelectedOption].Active = !CheatActive;
							break;
						}
						case ITEM_DROP_CHANGE_BY_ID:
						{
							if (tempMenuSelectionStates == 0)
							{
								MenuVar.MenuSelectionStates = CurrentMenuOptionCheck;
								MenuVar.SecondaryMenuOption = getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
							}
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case B:
				{
					if (tempMenuSelectionStates != ITEM_DROP_CHANGE_BY_ID)
					{
						// Go back to the previous menu
						MenuVar.MenuSelectedOption = 0;
						enterPreviousMenu();
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case CHEATS_RESOLVE_FADES:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					adjustCheatsResolveFadesSelection(CurrentButton);
					break;
				}
				case A:
				{
					// Resolve the selected fade
					int32_t ReturnCode = resolveFade(tempCurrentMenuOption);
					MenuVar.FunctionReturnCode = static_cast<int8_t>(ReturnCode);
					MenuVar.Timer = secondsToFrames(3);
					break;
				}
				case B:
				{
					// Go back to the previous menu
					MenuVar.Timer = 0;
					MenuVar.FunctionReturnCode = 0;
					MenuVar.MenuSelectedOption = 0;
					enterPreviousMenu();
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case CHEATS_LOCK_FLAGS:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					adjustMenuNoPageEdit(CurrentButton);
					break;
				}
				case A:
				{
					uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
					switch (CurrentMenuOptionCheck)
					{
						case LOCK_GSW_FLAGS:
						{
							// Back up the main memory
							lockFlagsMenuBackUpFlags(GSW);
							break;
						}
						case LOCK_GSWF_FLAGS:
						{
							// Back up the main memory
							lockFlagsMenuBackUpFlags(GSWF);
							break;
						}
						case LOCK_GW_FLAGS:
						{
							// Back up the main memory
							lockFlagsMenuBackUpFlags(GW);
							break;
						}
						case LOCK_GF_FLAGS:
						{
							// Back up the main memory
							lockFlagsMenuBackUpFlags(GF);
							break;
						}
						case LOCK_LSW_FLAGS:
						{
							// Back up the main memory
							lockFlagsMenuBackUpFlags(LSW);
							break;
						}
						case LOCK_LSWF_FLAGS:
						{
							// Back up the main memory
							lockFlagsMenuBackUpFlags(LSWF);
							break;
						}
						case SET_NEW_LSW_AREA:
						{
							// Back up the main memory and set the new area
							lockFlagsMenuSetNewArea(LSW);
							break;
						}
						case SET_NEW_LSWF_AREA:
						{
							// Back up the main memory and set the new area
							lockFlagsMenuSetNewArea(LSWF);
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case B:
				{
					// Go back to the previous menu
					MenuVar.MenuSelectedOption = 0;
					enterPreviousMenu();
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case CHEATS_MANAGE_FLAGS:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					adjustMenuNoPageEdit(CurrentButton);
					break;
				}
				case A:
				{
					// Clear the current values set for managing flags
					clearMemory(&ManageFlags, sizeof(ManageFlags));
					
					// Enter the next menu
					MenuVar.MenuSelectionStates = tempCurrentMenuOption + 1;
					enterNextMenu(CHEATS_MANAGE_FLAGS_MAIN, tempCurrentMenuOption);
					MenuVar.CurrentMenuOption = 0;
					break;
				}
				case B:
				{
					// Go back to the previous menu
					MenuVar.MenuSelectedOption = 0;
					enterPreviousMenu();
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case CHEATS_MANAGE_FLAGS_MAIN:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					if (tempSelectedOption == 0)
					{
						adjustCheatsManageFlagsMainMenu(CurrentButton);
					}
					break;
				}
				case A:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							uint32_t FlagToSet = ManageFlags.FlagToSet;
							uint32_t ValueToSet = static_cast<uint32_t>(ManageFlags.ValueToSet);
							
							switch (tempMenuSelectionStates)
							{
								case SET_GSW:
								case SET_GW:
								case SET_LSW:
								{
									switch (tempCurrentMenuOption)
									{
										case CHANGE_GLOBAL_WORD:
										{
											MenuVar.MenuSecondaryValue 		= FlagToSet;
											MenuVar.SecondaryMenuOption 	= getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
											MenuVar.SelectedOption 			= tempCurrentMenuOption + 1;
											break;
										}
										case CHANGE_GLOBAL_WORD_VALUE:
										{
											MenuVar.MenuSecondaryValue 		= getGlobalFlagValue(tempMenuSelectionStates, FlagToSet);
											MenuVar.SecondaryMenuOption 	= getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
											MenuVar.SelectedOption 			= tempCurrentMenuOption + 1;
											break;
										}
										case SET_GLOBAL_WORD_VALUE:
										{
											switch (tempMenuSelectionStates)
											{
												case SET_GSW:
												{
													ttyd::swdrv::swByteSet(FlagToSet, ValueToSet);
													break;
												}
												case SET_GW:
												{
													setGW(FlagToSet, ValueToSet);
													break;
												}
												case SET_LSW:
												{
													ttyd::swdrv::_swByteSet(FlagToSet, ValueToSet);
													break;
												}
												default:
												{
													break;
												}
											}
											break;
										}
										default:
										{
											break;
										}
									}
									break;
								}
								case SET_GSWF:
								case SET_GF:
								case SET_LSWF:
								{
									switch (tempCurrentMenuOption)
									{
										case CHANGE_GLOBAL_FLAG:
										{
											MenuVar.MenuSecondaryValue 		= FlagToSet;
											MenuVar.SecondaryMenuOption 	= getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
											MenuVar.SelectedOption 			= tempCurrentMenuOption + 1;
											break;
										}
										case SET_GLOBAL_FLAG:
										{
											switch (tempMenuSelectionStates)
											{
												case SET_GSWF:
												{
													if (ttyd::swdrv::swGet(FlagToSet))
													{
														ttyd::swdrv::swClear(FlagToSet);
													}
													else
													{
														ttyd::swdrv::swSet(FlagToSet);
													}
													break;
												}
												case SET_GF:
												{
													setGF(FlagToSet); // setGF automatically toggles the value
													break;
												}
												case SET_LSWF:
												{
													if (ttyd::swdrv::_swGet(FlagToSet))
													{
														ttyd::swdrv::_swClear(FlagToSet);
													}
													else
													{
														ttyd::swdrv::_swSet(FlagToSet);
													}
													break;
												}
												default:
												{
													break;
												}
											}
											break;
										}
										default:
										{
											break;
										}
									}
									break;
								}
								default:
								{
									break;
								}
							}
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case B:
				{
					if (tempSelectedOption == 0)
					{
						// Go back to the previous menu
						MenuVar.MenuSelectionStates = 0;
						enterPreviousMenu();
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case CHEATS_CLEAR_AREA_FLAGS:
		{
			switch (CurrentButton)
			{
				case DPADLEFT:
				case DPADRIGHT:
				{
					switch (tempSelectedOption)
					{
						case SELECT_AREA:
						{
							adjustCheatClearAreaFlagSelection(CurrentButton);
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case DPADDOWN:
				case DPADUP:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							adjustMenuNoPageEdit(CurrentButton);
							break;
						}
						case SELECT_AREA:
						{
							adjustCheatClearAreaFlagSelection(CurrentButton);
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case A:
				{
					uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
					switch (CurrentMenuOptionCheck)
					{
						case SELECT_AREA:
						{
							switch (tempSelectedOption)
							{
								case 0:
								{
									MenuVar.SecondaryMenuOption 	= 0;
									MenuVar.SelectedOption 			= CurrentMenuOptionCheck;
									break;
								}
								default:
								{
									MenuVar.MenuSecondaryValue 		= MenuVar.SecondaryMenuOption;
									MenuVar.SelectedOption 			= 0;
									break;
								}
							}
							break;
						}
						case CLEAR_FLAGS:
						{
							switch (tempSelectedOption)
							{
								case 0:
								{
									MenuVar.SecondaryMenuOption 	= 1;
									MenuVar.SelectedOption 			= CurrentMenuOptionCheck;
									break;
								}
								default:
								{
									switch (MenuVar.SecondaryMenuOption)
									{
										case 0:
										{
											// Selected yes
											ClearAreaFlags.CurrentOption = MenuVar.MenuSecondaryValue;
											ClearAreaFlags.FlagsShouldBeCleared = true;
											MenuVar.SelectedOption = 0;
											break;
										}
										default:
										{
											// Selected no
											MenuVar.SelectedOption = 0;
											break;
										}
									}
									break;
								}
							}
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case B:
				{
					switch (tempSelectedOption)
					{
						case SELECT_AREA:
						case CLEAR_FLAGS:
						{
							MenuVar.SelectedOption = 0;
							break;
						}
						default:
						{
							// Go back to the previous menu
							MenuVar.MenuSelectedOption = 0;
							enterPreviousMenu();
							break;
						}
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case STATS:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					adjustMenuNoPageEdit(CurrentButton);
					break;
				}
				case A:
				{
					// Enter the next menu
					MenuVar.MenuSelectedOption = 0;
					enterNextMenu(STATS_MARIO + tempCurrentMenuOption, tempCurrentMenuOption);
					resetMenu();
					break;
				}
				case B:
				{
					// Go back to the previous menu
					resetMenu();
					enterPreviousMenu();
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case STATS_MARIO:
		{
			switch (CurrentButton)
			{
				case DPADLEFT:
				case DPADRIGHT:
				case DPADDOWN:
				case DPADUP:
				{
					adjustMarioStatsSelection(CurrentButton);
					break;
				}
				case A:
				{
					switch (tempMenuSelectedOption)
					{
						case 0:
						{
							uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
							MenuVar.MenuSelectedOption = CurrentMenuOptionCheck;
							MenuVar.MenuSelectionStates = CurrentMenuOptionCheck;
							
							switch (CurrentMenuOptionCheck)
							{
								case SPECIAL_MOVES:
								{
									MenuVar.SecondaryMenuOption = 0;
									break;
								}
								default:
								{
									MenuVar.SecondaryMenuOption = getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
								
									uint32_t offset = getMarioStatsValueOffset(CurrentMenuOptionCheck);
									if (offset == 0)
									{
										break;
									}
									
									// Set the current value in the display to the current value of the address being changed
									if ((CurrentMenuOptionCheck >= PIANTAS_STORED) && 
										(CurrentMenuOptionCheck <= CURRENT_PIANTAS))
									{
										uint32_t PiantaParlorPtr = reinterpret_cast<uint32_t>(
											ttyd::evt_yuugijou::yuugijouWorkPointer);
										
										MenuVar.MenuSecondaryValue = *reinterpret_cast<int32_t *>(PiantaParlorPtr + offset);
									}
									else
									{
										uint32_t PouchPtr = reinterpret_cast<uint32_t>(
											ttyd::mario_pouch::pouchGetPtr());
										
										MenuVar.MenuSecondaryValue = *reinterpret_cast<int16_t *>(PouchPtr + offset);
									}
									break;
								}
							}
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case B:
				{
					if (tempMenuSelectedOption == 0)
					{
						// Go back to the previous menu
						resetMenu();
						enterPreviousMenu();
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case STATS_PARTNERS:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							adjustMenuNoPageEdit(CurrentButton);
							break;
						}
						default:
						{
							adjustPartnerStatsSelection(CurrentButton);
							break;
						}
					}
					break;
				}
				case A:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							MenuVar.CurrentMenuOption = 0;
							MenuVar.SelectedOption = tempCurrentMenuOption + 1;
							break;
						}
						default:
						{
							switch (tempMenuSelectedOption)
							{
								case 0:
								{
									uint32_t PartnerEnabledAddress = reinterpret_cast<uint32_t>(getPartnerEnabledAddress());
									
									uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
									switch (CurrentMenuOptionCheck)
									{
										case PARTNER_HP:
										{
											MenuVar.MenuSecondaryValue = *reinterpret_cast<int16_t *>(
												PartnerEnabledAddress + 0x6);
											
											MenuVar.MenuSelectedOption = CurrentMenuOptionCheck;
											MenuVar.MenuSelectionStates = CurrentMenuOptionCheck;
											MenuVar.SecondaryMenuOption = getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
											break;
										}
										case PARTNER_MAX_HP:
										{
											MenuVar.MenuSecondaryValue = *reinterpret_cast<int16_t *>(
												PartnerEnabledAddress + 0x2);
											
											MenuVar.MenuSelectedOption = CurrentMenuOptionCheck;
											MenuVar.MenuSelectionStates = CurrentMenuOptionCheck;
											MenuVar.SecondaryMenuOption = getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
											break;
										}
										case PARTNER_RANK:
										{
											MenuVar.MenuSecondaryValue = *reinterpret_cast<int16_t *>(
												PartnerEnabledAddress + 0xA);
											
											MenuVar.MenuSelectedOption = CurrentMenuOptionCheck;
											MenuVar.MenuSelectionStates = CurrentMenuOptionCheck;
											MenuVar.SecondaryMenuOption = getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
											break;
										}
										case TOGGLE:
										{
											bool *AddressToChange = reinterpret_cast<bool *>(
												PartnerEnabledAddress + 1);
											
											*AddressToChange = !*AddressToChange;
											
											// Reset the partner menu
											resetPartnerPauseMenu();
											break;
										}
										case TOGGLE + 1:
										{
											// Check if currently on Yoshi
											ttyd::party::PartyMembers CurrentPartner = getSelectedOptionPartnerValue();
											if (CurrentPartner == ttyd::party::PartyMembers::kYoshi)
											{
												MenuVar.MenuSelectedOption = STATS_PARTNER_DISPLAY_YOSHI_COLORS;
												uint32_t CurrentColorId = getCurrentYoshiColorId();
												
												// Make sure the current color is valid
												const uint32_t ColorIdWhite = 6;
												if (CurrentColorId <= ColorIdWhite)
												{
													MenuVar.SecondaryMenuOption = CurrentColorId;
												}
												else
												{
													MenuVar.SecondaryMenuOption = 0;
												}
											}
											else
											{
												partnerMenuRemoveOrBringOut(reinterpret_cast<void *>(PartnerEnabledAddress));
											}
											break;
										}
										case TOGGLE + 2:
										{
											// This option should only be available for Yoshi
											ttyd::party::PartyMembers CurrentPartner = getSelectedOptionPartnerValue();
											if (CurrentPartner == ttyd::party::PartyMembers::kYoshi)
											{
												partnerMenuRemoveOrBringOut(reinterpret_cast<void *>(PartnerEnabledAddress));
											}
											break;
										}
										default:
										{
											break;
										}
									}
									break;
								}
								default:
								{
									break;
								}
							}
							break;
						}
					}
					break;
				}
				case B:
				{
					if (tempMenuSelectedOption == 0)
					{
						switch (tempSelectedOption)
						{
							case 0:
							{
								// Go back to the previous menu
								resetMenu();
								enterPreviousMenu();
								break;
							}
							default:
							{
								closeSecondaryMenu();
								break;
							}
						}
					}
					break;
				}
			}
			break;
		}
		case STATS_FOLLOWERS:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					if (tempSelectedOption == 0)
					{
						adjustMenuNoPageEdit(CurrentButton);
					}
					break;
				}
				case A:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
							switch (CurrentMenuOptionCheck)
							{
								case BRING_OUT_FOLLOWER:
								{
									// Make sure a file is loaded
									if (checkIfInGame())
									{
										MenuVar.SecondaryMenuOption = 0;
										MenuVar.Timer 				= 0;
										MenuVar.SelectedOption 		= CurrentMenuOptionCheck;
									}
									else
									{
										MenuVar.FunctionReturnCode 	= NOT_IN_GAME;
										MenuVar.Timer 				= secondsToFrames(3);
									}
									break;
								}
								case REMOVE_FOLLOWER:
								{
									removeFollowerFromOverworld();
									break;
								}
								default:
								{
									break;
								}
							}
							break;
						}
						default:
						{
							// Make sure a file is loaded again
							if (checkIfInGame())
							{
								MenuVar.Timer = 0;
							}
							else
							{
								MenuVar.FunctionReturnCode 	= NOT_IN_GAME;
								MenuVar.Timer 				= secondsToFrames(3);
							}
							break;
						}
					}
					break;
				}
				case B:
				{
					if (tempSelectedOption == 0)
					{
						// Go back to the previous menu
						MenuVar.FunctionReturnCode = 0;
						resetMenu();
						enterPreviousMenu();
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case SETTINGS:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					MenuVar.Timer = 0;
					adjustMenuNoPageEdit(CurrentButton);
					break;
				}
				case A:
				{
					uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
					switch (CurrentMenuOptionCheck)
					{
						case LOAD_SETTINGS:
						{
							int32_t ReturnCode = loadSettings();
							switch (ReturnCode)
							{
								case CARD_RESULT_READY:
								{
									MenuSettings.ReturnCode = LOAD_SUCCESSFUL;
									MenuVar.Timer = secondsToFrames(3);
									break;
								}
								case CARD_RESULT_NOFILE:
								{
									MenuSettings.ReturnCode = LOAD_FAILED_NO_FILE;
									MenuVar.Timer = secondsToFrames(3);
									break;
								}
								default:
								{
									MenuSettings.ReturnCode = LOAD_FAILED;
									MenuVar.Timer = secondsToFrames(3);
									break;
								}
							}
							break;
						}
						case SAVE_SETTINGS:
						{
							int32_t ReturnCode = saveSettings();
							switch (ReturnCode)
							{
								case CARD_RESULT_READY:
								{
									MenuSettings.ReturnCode = SAVE_SUCCESSFUL;
									MenuVar.Timer = secondsToFrames(3);
									break;
								}
								default:
								{
									MenuSettings.ReturnCode = SAVE_FAILED;
									MenuVar.Timer = secondsToFrames(3);
									break;
								}
							}
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case B:
				{
					// Go back to the previous menu
					resetMenu();
					enterPreviousMenu();
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case MEMORY:
		{
			uint32_t MaxOptionsPerPage = 10;
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							adjustMenuNoPageEdit(CurrentButton);
							break;
						}
						default:
						{
							adjustMemoryWatchSelection(CurrentButton);
							break;
						}
					}
					break;
				}
				case A:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
							switch (CurrentMenuOptionCheck)
							{
								case ADD_WATCH:
								{
									int32_t EmptyWatchSlot = getEmptyWatchSlot();
									if (EmptyWatchSlot >= 0)
									{
										addMemoryWatch(EmptyWatchSlot);
									}
									else
									{
										// There are no more free slots
										MenuVar.FunctionReturnCode = NO_SLOTS_LEFT;
										MenuVar.Timer = secondsToFrames(3);
									}
									break;
								}
								case DUPLICATE_WATCH:
								{
									uint32_t tempAddress = MemoryWatch[0].Address;
									if (tempAddress)
									{
										int32_t EmptyWatchSlot = getEmptyWatchSlot();
										if (EmptyWatchSlot >= 0)
										{
											MenuVar.CurrentMenuOption 	= tempCurrentPage * MaxOptionsPerPage;
											MenuVar.SelectedOption 		= CurrentMenuOptionCheck;
										}
										else
										{
											// There are no more free slots
											MenuVar.FunctionReturnCode = NO_SLOTS_LEFT;
											MenuVar.Timer = secondsToFrames(3);
										}
									}
									else
									{
										// All slots are empty
										MenuVar.FunctionReturnCode = ALL_SLOTS_EMPTY;
										MenuVar.Timer = secondsToFrames(3);
									}
									break;
								}
								case MODIFY_WATCH:
								{
									uint32_t tempAddress = MemoryWatch[0].Address;
									if (tempAddress)
									{
										MenuVar.CurrentMenuOption 	= tempCurrentPage * MaxOptionsPerPage;
										MenuVar.SelectedOption 		= CurrentMenuOptionCheck;
									}
									else
									{
										// All slots are empty
										MenuVar.FunctionReturnCode = ALL_SLOTS_EMPTY;
										MenuVar.Timer = secondsToFrames(3);
									}
									break;
								}
								case DELETE_WATCH:
								{
									uint32_t tempAddress = MemoryWatch[0].Address;
									if (tempAddress)
									{
										MenuVar.CurrentMenuOption 	= tempCurrentPage * MaxOptionsPerPage;
										MenuVar.SelectedOption 		= CurrentMenuOptionCheck;
									}
									else
									{
										// All slots are empty
										MenuVar.FunctionReturnCode = ALL_SLOTS_EMPTY;
										MenuVar.Timer = secondsToFrames(3);
									}
									break;
								}
								default:
								{
									break;
								}
							}
							break;
						}
						case DUPLICATE_WATCH:
						{
							uint32_t tempAddress = MemoryWatch[0].Address;
							if (tempAddress)
							{
								int32_t EmptyWatchSlot = getEmptyWatchSlot();
								if (EmptyWatchSlot >= 0)
								{
									duplicateWatch(static_cast<int32_t>(tempCurrentMenuOption), EmptyWatchSlot);
									
									// Recheck to see if there are any empty slots left
									if (getEmptyWatchSlot() < 0)
									{
										// There are no more free slots
										closeSecondaryMenu();
									}
								}
								else
								{
									// There are no more free slots
									closeSecondaryMenu();
									MenuVar.FunctionReturnCode = NO_SLOTS_LEFT;
									MenuVar.Timer = secondsToFrames(3);
								}
							}
							else
							{
								// All slots are empty
								closeSecondaryMenu();
								MenuVar.FunctionReturnCode = NO_SLOTS_LEFT;
								MenuVar.Timer = secondsToFrames(3);
							}
							break;
						}
						case MODIFY_WATCH:
						{
							uint32_t tempAddress = MemoryWatch[0].Address;
							if (tempAddress)
							{
								// Enter the next menu
								MenuVar.MenuSelectedOption = tempCurrentMenuOption;
								enterNextMenu(MEMORY_MODIFY, tempSelectedOption - 1);
								resetMenu();
							}
							else
							{
								// All slots are empty
								closeSecondaryMenu();
								MenuVar.FunctionReturnCode = NO_SLOTS_LEFT;
								MenuVar.Timer = secondsToFrames(3);
							}
							break;
						}
						case DELETE_WATCH:
						{
							uint32_t tempAddress = MemoryWatch[0].Address;
							if (tempAddress)
							{
								deleteWatch(static_cast<int32_t>(tempCurrentMenuOption));
							}
							else
							{
								// All slots are empty
								closeSecondaryMenu();
								MenuVar.FunctionReturnCode = NO_SLOTS_LEFT;
								MenuVar.Timer = secondsToFrames(3);
							}
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case B:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							// Go back to the previous menu
							MenuVar.FunctionReturnCode = 0;
							resetMenu();
							enterPreviousMenu();
							break;
						}
						default:
						{
							closeSecondaryMenu();
							break;
						}
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case MEMORY_MODIFY:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					if (tempSelectedOption == 0)
					{
						adjustMenuNoPageEdit(CurrentButton);
					}
					break;
				}
				case A:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
							switch (CurrentMenuOptionCheck)
							{
								case CHANGE_ADDRESS:
								{
									// Enter the next menu
									enterNextMenu(MEMORY_CHANGE_ADDRESS, tempCurrentMenuOption);
									resetMenu();
									break;
								}
								case CHANGE_TYPE:
								{
									MenuVar.SecondaryMenuOption = MemoryWatch[tempMenuSelectedOption].Type;
									MenuVar.SelectedOption = CurrentMenuOptionCheck;
									break;
								}
								case SHOW_AS_HEX:
								{
									// Only change for types that can be displayed in hex
									switch (MemoryWatch[tempMenuSelectedOption].Type)
									{
										case string:
										case time:
										{
											break;
										}
										default:
										{
											bool ShowAsHex = MemoryWatch[tempMenuSelectedOption].ShowAsHex;
											MemoryWatch[tempMenuSelectedOption].ShowAsHex = !ShowAsHex;
											break;
										}
									}
									break;
								}
								case CHANGE_WATCH_POSITION:
								{
									MenuVar.HideMenu = true;
									MemoryWatchPosition.PosX = MemoryWatch[tempMenuSelectedOption].PosX;
									MemoryWatchPosition.PosY = MemoryWatch[tempMenuSelectedOption].PosY;
									MenuVar.SelectedOption = CurrentMenuOptionCheck;
									break;
								}
								case DISPLAY_OUT_OF_MENU:
								{
									bool DisplayOutOfMenu = MemoryWatch[tempMenuSelectedOption].Display;
									MemoryWatch[tempMenuSelectedOption].Display = !DisplayOutOfMenu;
									break;
								}
								default:
								{
									break;
								}
							}
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case B:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							// Go back to the previous menu
							MenuVar.MenuSelectedOption = 0;
							resetMenu();
							enterPreviousMenu();
							break;
						}
						default:
						{
							closeSecondaryMenu();
							break;
						}
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case MEMORY_CHANGE_ADDRESS:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					switch (tempMenuSelectionStates)
					{
						case 0:
						{
							switch (tempSelectedOption)
							{
								case 0:
								{
									adjustMenuNoPageEdit(CurrentButton);
									break;
								}
								default:
								{
									adjustMemoryChangeAddressOrPointerSelection(CurrentButton);
									break;
								}
							}
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case A:
				{
					switch (tempMenuSelectionStates)
					{
						case 0:
						{
							switch (tempSelectedOption)
							{
								case 0:
								{
									uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
									switch (CurrentMenuOptionCheck)
									{
										case CHANGE_ADDRESS_OR_POINTERS:
										{
											MenuVar.CurrentMenuOption 	= 0;
											MenuVar.SelectedOption 		= CurrentMenuOptionCheck;
											break;
										}
										case ADD_POINTER_LEVEL:
										{
											// Only add a pointer level if not already at the max
											uint32_t AddressOffsetTotal = MemoryWatch[tempMenuSelectedOption].AddressOffsetAmount;
											if (AddressOffsetTotal < 10)
											{
												MemoryWatch[tempMenuSelectedOption].AddressOffsetAmount = AddressOffsetTotal + 1;
												MemoryWatch[tempMenuSelectedOption].AddressOffset[AddressOffsetTotal] = 0;
											}
											break;
										}
										case REMOVE_POINTER_LEVEL:
										{
											// Only remove a pointer level if there is at least one left
											uint32_t AddressOffsetTotal = MemoryWatch[tempMenuSelectedOption].AddressOffsetAmount;
											if (AddressOffsetTotal > 0)
											{
												MemoryWatch[tempMenuSelectedOption].AddressOffsetAmount = AddressOffsetTotal - 1;
											}
											break;
										}
										default:
										{
											break;
										}
									}
									break;
								}
								case CHANGE_ADDRESS_OR_POINTERS:
								{
									MenuVar.MenuSelectionStates = tempCurrentMenuOption + 1;
									MenuVar.SecondaryMenuOption = 7;
									
									switch (tempCurrentMenuOption)
									{
										case 0:
										{
											MenuVar.MemoryWatchSecondaryValue = MemoryWatch[tempMenuSelectedOption].Address;
											break;
										}
										default:
										{
											MenuVar.MemoryWatchSecondaryValue = MemoryWatch[tempMenuSelectedOption].AddressOffset[tempCurrentMenuOption - 1];
											break;
										}
									}
									break;
								}
								default:
								{
									break;
								}
							}
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case B:
				{
					switch (tempMenuSelectionStates)
					{
						case 0:
						{
							switch (tempSelectedOption)
							{
								case 0:
								{
									// Go back to the previous menu
									resetMenu();
									enterPreviousMenu();
									break;
								}
								default:
								{
									closeSecondaryMenu();
									break;
								}
							}
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case BATTLES:
		{
			// Close the menu if not in a battle
			if (!getBattleWorkPointer())
			{
				resetMenuToRoot();
				MenuVar.FunctionReturnCode = NOT_IN_BATTLE;
				MenuVar.Timer = secondsToFrames(3);
			}
			else
			{
				switch (CurrentButton)
				{
					case DPADDOWN:
					case DPADUP:
					{
						adjustBattlesActorSelection(CurrentButton);
						break;
					}
					case A:
					{
						// Make sure the current slot isn't empty
						uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
						void *CurrentActorPointer = getActorPointer(CurrentMenuOptionCheck); // Add 1 to skip System
						
						if (CurrentActorPointer)
						{
							// Go to the next menu
							MenuVar.MenuSelectedOption = CurrentMenuOptionCheck; // Add 1 to skip System
							enterNextMenu(BATTLES_CURRENT_ACTOR, tempCurrentMenuOption);
							resetMenu();
						}
						break;
					}
					case B:
					{
						// Go back to the previous menu
						resetMenu();
						enterPreviousMenu();
						break;
					}
					default:
					{
						break;
					}
				}
			}
			break;
		}
		case BATTLES_CURRENT_ACTOR:
		{
			// Close the menu if not in a battle
			if (!getBattleWorkPointer())
			{
				resetMenuToRoot();
				MenuVar.FunctionReturnCode = NOT_IN_BATTLE;
				MenuVar.Timer = secondsToFrames(3);
			}
			else
			{
				switch (CurrentButton)
				{
					case DPADDOWN:
					case DPADUP:
					{
						if (tempSelectedOption == 0)
						{
							adjustMenuNoPageEdit(CurrentButton);
						}
						break;
					}
					case A:
					{
						switch (tempSelectedOption)
						{
							case 0:
							{
								uint32_t ActorAddress = reinterpret_cast<uint32_t>(
									getActorPointer(tempMenuSelectedOption));
								
								uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
								switch (CurrentMenuOptionCheck)
								{
									case CLEAR_HELD_ITEM:
									{
										if (!ActorAddress)
										{
											break;
										}
										
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
										
										// Clear the held item
										*reinterpret_cast<int32_t *>(ActorAddress + HeldItemOffset) = 0;
										
										// Do not clear the equipped badges for Mario or the partners
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
										break;
									}
									case CHANGE_ACTOR_STATUSES:
									{
										// Go to the next menu
										enterNextMenu(BATTLES_STATUSES, tempCurrentMenuOption);
										resetMenu();
										break;
									}
									default:
									{
										if (!ActorAddress)
										{
											break;
										}
										
										MenuVar.MenuSelectionStates 	= CurrentMenuOptionCheck;
										MenuVar.SecondaryMenuOption 	= getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
										MenuVar.SelectedOption 			= CurrentMenuOptionCheck;
										
										switch (CurrentMenuOptionCheck)
										{
											case CHANGE_ACTOR_HP:
											{
												MenuVar.MenuSecondaryValue = *reinterpret_cast<int16_t *>(
													ActorAddress + 0x10C);
												break;
											}
											case CHANGE_ACTOR_MAX_HP:
											{
												MenuVar.MenuSecondaryValue = *reinterpret_cast<int16_t *>(
													ActorAddress + 0x108);
												break;
											}
											case CHANGE_ACTOR_FP:
											{
												MenuVar.MenuSecondaryValue = *reinterpret_cast<int16_t *>(
													ActorAddress + 0x112);
												break;
											}
											case CHANGE_ACTOR_MAX_FP:
											{
												MenuVar.MenuSecondaryValue = *reinterpret_cast<int16_t *>(
													ActorAddress + 0x10E);
												break;
											}
											case CHANGE_HELD_ITEM:
											{
												#ifdef TTYD_US
												uint32_t offset = 0x308;
												#elif defined TTYD_JP
												uint32_t offset = 0x304;
												#elif defined TTYD_EU
												uint32_t offset = 0x30C;
												#endif
												
												MenuVar.MenuSecondaryValue = *reinterpret_cast<int32_t *>(
													ActorAddress + offset);
												break;
											}
											default:
											{
												break;
											}
										}
										break;
									}
								}
								break;
							}
							default:
							{	
								break;
							}
						}
						break;
					}
					case B:
					{
						switch (tempSelectedOption)
						{
							case 0:
							{
								// Go back to the previous menu
								resetMenu();
								enterPreviousMenu();
								break;
							}
							default:
							{
								closeSecondaryMenu();
								break;
							}
						}
						break;
					}
					default:
					{
						break;
					}
				}
			}
			break;
		}
		case BATTLES_STATUSES:
		{
			// Close the menu if not in a battle
			if (!getBattleWorkPointer())
			{
				resetMenuToRoot();
				MenuVar.FunctionReturnCode = NOT_IN_BATTLE;
				MenuVar.Timer = secondsToFrames(3);
			}
			else
			{
				switch (CurrentButton)
				{
					case DPADDOWN:
					case DPADUP:
					{
						if (tempSelectedOption == 0)
						{
							adjustBattlesStatusSelection(CurrentButton);
						}
						break;
					}
					case A:
					{
						if (tempSelectedOption == 0)
						{
							uint32_t ActorAddress = reinterpret_cast<uint32_t>(
								getActorPointer(tempMenuSelectedOption));
							
							if (!ActorAddress)
							{
								break;
							}
							
							uint32_t Counter = 0;
							if (tempCurrentMenuOption >= 20) // Explosion turns left
							{
								Counter += 3;
							}
							else if (tempCurrentMenuOption >= 18) // Can't use items
							{
								Counter += 2;
							}
							else if (tempCurrentMenuOption >= 1) // Sleep flags
							{
								Counter++;
							}
							
							uint32_t TotalOptions = BattlesStatusesLinesSize;
							uint32_t OffsetToLoad = 0x118 + tempCurrentMenuOption + Counter;
							
							if (tempCurrentMenuOption == (TotalOptions - 1))
							{
								// Currently modifying the defeated flag
								// Toggle the value
								bool *AddressToChange = reinterpret_cast<bool *>(
									ActorAddress + OffsetToLoad);
								
								*AddressToChange = !*AddressToChange;
							}
							else
							{
								MenuVar.SelectedOption = tempCurrentMenuOption + 1;
								MenuVar.MenuSelectionStates = tempCurrentMenuOption + 1;
								MenuVar.SecondaryMenuOption = getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
								
								MenuVar.MenuSecondaryValue = *reinterpret_cast<int8_t *>(
									ActorAddress + OffsetToLoad);
							}
						}
						break;
					}
					case B:
					{
						switch (tempSelectedOption)
						{
							case 0:
							{
								// Go back to the previous menu
								resetMenu();
								enterPreviousMenu();
								break;
							}
							default:
							{
								closeSecondaryMenu();
								break;
							}
						}
						break;
					}
					default:
					{
						break;
					}
				}
			}
			break;
		}
		case DISPLAYS:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					adjustMenuNoPageEdit(CurrentButton);
					break;
				}
				case A:
				{
					// Enter the next menu
					uint32_t MenuToEnter;
					switch (tempCurrentMenuOption)
					{
						case ONSCREEN_TIMER:
						case ONSCREEN_TIMER_FRAME_COUNTER:
						{
							MenuToEnter = DISPLAYS_ONSCREEN_TIMER;
							break;
						}
						default:
						{
							MenuToEnter = DISPLAYS_NO_BUTTON_COMBO;
							break;
						}
					}
					
					MenuVar.MenuSelectedOption = tempCurrentMenuOption;
					enterNextMenu(MenuToEnter, tempCurrentMenuOption);
					resetMenu();
					break;
				}
				case B:
				{
					// Go back to the previous menu
					resetMenu();
					enterPreviousMenu();
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case DISPLAYS_ONSCREEN_TIMER:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					adjustMenuNoPageEdit(CurrentButton);
					break;
				}
				case A:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
							switch (CurrentMenuOptionCheck)
							{
								case ONSCREEN_TIMER_TURN_ON_OR_OFF:
								{
									// Flip the bool for the current display
									bool DisplayActive = !Displays[tempMenuSelectedOption];
									Displays[tempMenuSelectedOption] = DisplayActive;
									
									if (DisplayActive)
									{
										// Reset the timer
										if (tempMenuSelectedOption == ONSCREEN_TIMER)
										{
											OnScreenTimer.MainTimer = 0;
											OnScreenTimer.PreviousFrameTime = 0;
											OnScreenTimer.TimerPaused = true;
										}
										else // tempMenuSelectedOption == ONSCREEN_TIMER_FRAME_COUNTER
										{
											FrameCounter.MainTimer = 0;
											FrameCounter.TimerPaused = true;
										}
									}
									break;
								}
								case CHANGE_START_PAUSE_RESUME_BUTTON_COMBO:
								case CHANGE_RESET_BUTTON_COMBO:
								{
									MenuVar.ChangingCheatButtonCombo 	= true;
									MenuVar.Timer 						= secondsToFrames(3);
									MenuVar.SelectedOption 				= CurrentMenuOptionCheck;
									break;
								}
								default:
								{
									break;
								}
							}
							break;
						}
						default:
						{
							// Currently changing a display button combo, so do nothing
							break;
						}
					}
					break;
				}
				case B:
				{
					if ((tempSelectedOption >= CHANGE_START_PAUSE_RESUME_BUTTON_COMBO) && 
						(tempSelectedOption <= CHANGE_RESET_BUTTON_COMBO))
					{
						// The work for this is done in drawChangeButtonCombo
						break;
					}
					else
					{
						// Go back to the previous menu
						MenuVar.MenuSelectedOption = 0;
						enterPreviousMenu();
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case DISPLAYS_NO_BUTTON_COMBO:
		{
			switch (CurrentButton)
			{
				case A:
				{
					// Flip the bool for the current display
					bool DisplayActive = !Displays[tempMenuSelectedOption];
					Displays[tempMenuSelectedOption] = DisplayActive;
					
					// Exit if the display was turned off
					if (!DisplayActive)
					{
						break;
					}
					
					// If one of the trick displays was turned on, then turn off the other trick displays
					switch (tempMenuSelectedOption)
					{
						case YOSHI_SKIP:
						{
							Displays[PALACE_SKIP] = false;
							Displays[BLIMP_TICKET_SKIP] = false;
							break;
						}
						case PALACE_SKIP:
						{
							Displays[YOSHI_SKIP] = false;
							Displays[BLIMP_TICKET_SKIP] = false;
							break;
						}
						case BLIMP_TICKET_SKIP:
						{
							Displays[YOSHI_SKIP] = false;
							Displays[PALACE_SKIP] = false;
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case B:
				{
					// Go back to the previous menu
					MenuVar.MenuSelectedOption = 0;
					enterPreviousMenu();
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case WARPS:
		{
			switch (CurrentButton)
			{
				case DPADLEFT:
				case DPADRIGHT:
				case DPADDOWN:
				case DPADUP:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							adjustMenuNoPageEdit(CurrentButton);
							break;
						}
						case SELECT_WARP:
						{
							adjustWarpsSelection(CurrentButton);
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case A:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
							switch (CurrentMenuOptionCheck)
							{
								case SELECT_WARP:
								{
									MenuVar.CurrentMenuOption 	= 0;
									MenuVar.Timer 				= 0;
									MenuVar.SelectedOption 		= CurrentMenuOptionCheck;
									break;
								}
								case WARP_BY_EVENT:
								{
									// Enter the next menu
									// WarpByEvent.CurrentIndex = 0;
									enterNextMenu(WARPS_EVENT, tempCurrentMenuOption);
									resetMenu();
									break;
								}
								case WARP_BY_INDEX:
								{
									// Enter the next menu
									enterNextMenu(WARPS_INDEX, tempCurrentMenuOption);
									resetMenu();
									break;
								}
								default:
								{
									break;
								}
							}
							break;
						}
						default:
						{
							switch (tempCurrentMenuOption)
							{
								case PIT_OF_100_TRIALS:
								{
									switch (tempMenuSelectedOption)
									{
										case 0:
										{
											if (checkIfInGame())
											{
												MenuVar.MenuSelectedOption = SELECTING_VALUE;
												MenuVar.MenuSecondaryValue = getCurrentPitFloor();
												
												MenuVar.SecondaryMenuOption = getHighestAdjustableValueDigit(
													tempCurrentMenu) - 1;
												
												MenuVar.Timer = 0;
											}
											else
											{
												MenuVar.FunctionReturnCode 	= NOT_IN_GAME;
												MenuVar.Timer 				= secondsToFrames(3);
											}
											break;
										}
										default:
										{
											break;
										}
									}
									break;
								}
								default:
								{
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
											return;
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
									break;
								}
							}
							break;
						}
					}
					break;
				}
				case B:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							// Go back to the previous menu
							resetMenu();
							enterPreviousMenu();
							break;
						}
						default:
						{
							if (tempMenuSelectedOption == 0)
							{
								closeSecondaryMenu();
							}
							break;
						}
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case WARPS_EVENT:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					if (tempSelectedOption == 0)
					{
						adjustMenuNoPageEdit(CurrentButton);
					}
					break;
				}
				case A:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
							switch (CurrentMenuOptionCheck)
							{
								case EVENT_SELECT_EVENT:
								{
									MenuVar.SelectedOption = CurrentMenuOptionCheck;
									MenuVar.MenuSecondaryValue = WarpByEvent.CurrentIndex;
									MenuVar.SecondaryMenuOption = getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
									MenuVar.Timer = 0;
									break;
								}
								case EVENT_KEEP_INVENTORY:
								{
									// Flip the bool
									bool ShouldKeepInventory = WarpByEvent.ShouldKeepInventory;
									WarpByEvent.ShouldKeepInventory = !ShouldKeepInventory;
									break;
								}
								case EVENT_SET_FLAGS:
								{
									// Flip the bool
									bool ShouldSetFlags = WarpByEvent.ShouldSetFlags;
									WarpByEvent.ShouldSetFlags = !ShouldSetFlags;
									break;
								}
								case EVENT_WARP_NOW:
								{
									int32_t ReturnCode = warpToMapByEvent(WarpByEvent.CurrentIndex);
									switch (ReturnCode)
									{
										case UNKNOWN_BEHAVIOR:
										{
											break;
										}
										case SUCCESS:
										{
											closeMenu();
											return;
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
									break;
								}
								default:
								{
									break;
								}
							}
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case B:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							// Go back to the previous menu
							MenuVar.FunctionReturnCode = 0;
							resetMenu();
							enterPreviousMenu();
							break;
						}
						default:
						{
							closeSecondaryMenu();
							break;
						}
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case WARPS_INDEX:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					switch (tempMenuSelectedOption)
					{
						case 0:
						{
							if (tempSelectedOption == 0)
							{
								adjustMenuNoPageEdit(CurrentButton);
							}
							break;
						}
						default:
						{
							adjustIndexWarpCurrentMapEntrancesPage(CurrentButton);
							break;
						}
					}
					break;
				}
				case A:
				{
					switch (tempMenuSelectedOption)
					{
						case 0:
						{
							switch (tempSelectedOption)
							{
								case 0:
								{
									uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
									switch (CurrentMenuOptionCheck)
									{
										case INDEX_VIEW_CURRENT_MAP_ENTRANCES:
										{
											MenuVar.MenuSelectedOption = CurrentMenuOptionCheck;
											break;
										}
										case INDEX_WARP_NOW:
										{
											int32_t ReturnCode = warpToMapByString(
												getMapFromIndex(static_cast<int32_t>(WarpByIndex.MapId)));
											
											switch (ReturnCode)
											{
												case SUCCESS:
												{
													// Set the flag for the loading zone to be adjusted
													WarpByIndex.RunIndexWarpCode = true;
													
													closeMenu();
													return;
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
											break;
										}
										default:
										{
											MenuVar.SecondaryMenuOption = getHighestAdjustableValueDigit(
												tempCurrentMenu) - 1;
											
											MenuVar.Timer = 0;
											MenuVar.SelectedOption = CurrentMenuOptionCheck;
											
											if (CurrentMenuOptionCheck == INDEX_SELECT_MAP)
											{
												int32_t MapIndex = getMapIndex();
												if (MapIndex < 0)
												{
													MenuVar.MenuSecondaryValue = 0;
												}
												else
												{
													MenuVar.MenuSecondaryValue = MapIndex;
												}
											}
											else
											{
												MenuVar.MenuSecondaryValue = static_cast<int32_t>(WarpByIndex.EntranceId);
											}
											break;
										}
									}
									break;
								}
								default:
								{
									break;
								}
							}
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case B:
				{
					switch (tempMenuSelectedOption)
					{
						case 0:
						{
							switch (tempSelectedOption)
							{
								case 0:
								{
									// Go back to the previous menu
									MenuVar.FunctionReturnCode = 0;
									resetMenu();
									enterPreviousMenu();
									break;
								}
								default:
								{
									closeSecondaryMenu();
									break;
								}
							}
							break;
						}
						default:
						{
							MenuVar.MenuSelectedOption = 0;
							break;
						}
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		default:
		{
			break;
		}
	}
}

void drawMenu()
{
	// Display the window for the text to go in
	if (!MenuVar.HideMenu)
	{
		drawMenuWindow();
	}
	
	uint32_t tempCurrentMenu 			= MenuVar.CurrentMenu;
	// uint32_t tempCurrentMenuOption 	= MenuVar.CurrentMenuOption;
	uint32_t tempSelectedOption 		= MenuVar.SelectedOption;
	uint32_t tempMenuSelectionStates 	= MenuVar.MenuSelectionStates;
	// uint32_t tempSecondaryMenuOption = MenuVar.SecondaryMenuOption;
	// int32_t tempMenuSecondaryValue 	= MenuVar.MenuSecondaryValue;
	uint32_t tempMenuSelectedOption 	= MenuVar.MenuSelectedOption;
	uint32_t tempCurrentPage 			= MenuVar.CurrentPage;
	int32_t tempFunctionReturnCode 		= MenuVar.FunctionReturnCode;
	// uint32_t tempColumnSplitAmount 	= Menu[tempCurrentMenu].ColumnSplitAmount;
	// uint32_t tempTotalMenuOptions 	= Menu[tempCurrentMenu].TotalMenuOptions;
	
	// Display the text for the current page
	switch (tempCurrentMenu)
	{
		case ROOT:
		{
			// Draw the text for the options
			drawSingleColumnMain();
			
			// Draw the version number
			int32_t PosX = 150;
			int32_t PosY = 180;
			drawVersionNumber(PosX, PosY);
			
			// Draw the error message if the player tried to use the battle menu while not in a battle
			if (tempFunctionReturnCode == NOT_IN_BATTLE)
			{
				drawNotInBattleErrorMessage();
			}
			break;
		}
		case INVENTORY_MAIN:
		{
			uint32_t MaxOptionsPerPage = 20;
			correctInventoryCurrentMenuOptionAndPage(MaxOptionsPerPage);
			
			// Draw the text for the options
			drawSingleColumnSelectedOption();
			
			// Draw the item icons and text
			drawInventoryIconAndTextColumns();
			
			// Draw the appropriate display for the selected option
			switch (tempSelectedOption)
			{
				case ADD_BY_ID:
				{
					drawAddById(tempMenuSelectedOption);
					break;
				}
				case ADD_BY_ICON:
				{
					drawAddByIcon(tempMenuSelectedOption);
					break;
				}
				case DUPLICATE:
				case DELETE:
				{
					switch (tempFunctionReturnCode)
					{
						case 0:
						{
							if (getTotalItems() <= 0)
							{
								// Inventory just became empty, so exit the current menu
								closeSecondaryMenu();
							}
							break;
						}
						case INVENTORY_FULL:
						{
							// Inventory is currently full
							const char *CurrentLine = "The inventory is currently full.";
							int32_t TextPosX = -130;
							int32_t TextPosY = 40;
							drawErrorWindowAutoWidth(CurrentLine, TextPosX, TextPosY);
							break;
						}
						case INVENTORY_EMPTY:
						{
							// Inventory is currently empty
							const char *CurrentLine = "The inventory is currently empty.";
							int32_t TextPosX = -130;
							int32_t TextPosY = 40;
							drawErrorWindowAutoWidth(CurrentLine, TextPosX, TextPosY);
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case CHANGE_BY_ID:
				case CHANGE_BY_ICON:
				{
					switch (tempFunctionReturnCode)
					{
						case 0:
						{
							if (getTotalItems() <= 0)
							{
								// Inventory just became empty, so exit the current menu
								tempMenuSelectionStates 	= 0;
								MenuVar.MenuSelectionStates 		= tempMenuSelectionStates;
								closeSecondaryMenu();
							}
							break;
						}
						case INVENTORY_EMPTY:
						{
							// Inventory is currently empty
							const char *CurrentLine = "The inventory is currently empty.";
							int32_t TextPosX = -130;
							int32_t TextPosY = 40;
							drawErrorWindowAutoWidth(CurrentLine, TextPosX, TextPosY);
							break;
						}
						default:
						{
							break;
						}
					}
					
					if (tempMenuSelectionStates != 0)
					{
						switch (tempSelectedOption)
						{
							case CHANGE_BY_ID:
							{
								drawAdjustableValue(true, tempMenuSelectedOption);
								break;
							}
							case CHANGE_BY_ICON:
							{
								drawAddByIconMain(tempMenuSelectedOption);
								break;
							}
							default:
							{
								break;
							}
						}
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case CHEATS:
		{
			// Draw the text for the options
			uint32_t MaxOptionsPerPage = 18;
			int32_t PosY = 180;
			drawSingleColumn(PosY, MaxOptionsPerPage, tempCurrentPage, false);
			
			// Draw the page number
			int32_t PosX = 150;
			// int32_t PosY = 180;
			drawPageNumber(PosX, PosY, tempCurrentPage);
			break;
		}
		case CHEATS_CHANGE_SEQUENCE:
		{
			// Draw the text for the options
			drawSingleColumnMain();
			
			// Draw the text for the Sequence
			drawCheatsChangeSequence();
			
			if (tempMenuSelectionStates == CHANGE_SEQUENCE_VALUE)
			{
				drawAdjustableValue(false, tempCurrentMenu);
			}
			break;
		}
		case CHEATS_STANDARD:
		{
			// Draw the text for the options
			drawSingleColumnSelectedOption();
			
			// Draw the bool
			int32_t PosY = 120;
			drawCheatsBool(PosY);
			
			// Draw the button combo
			uint16_t *CurrentButtonCombo = &Cheat[tempMenuSelectedOption].ButtonCombo;
			const char *Description = "Button Combo (Can be pressed in any order)";
			PosY -= 60;
			drawButtonCombo(*CurrentButtonCombo, PosY, Description);
			
			if (tempSelectedOption == CHANGE_BUTTON_COMBO)
			{
				drawChangeButtonCombo(CurrentButtonCombo);
			}
			break;
		}
		case CHEATS_NO_BUTTON_COMBO:
		{
			// Draw the text for the options
			drawSingleColumnMain();
			
			// Draw the bool
			int32_t PosY = 140;
			drawCheatsBool(PosY);
			break;
		}
		case CHEATS_NPC_FORCE_DROP:
		{
			// Draw the text for the options
			drawSingleColumnMain();
			
			// Draw the bool
			int32_t PosY = 120;
			drawCheatsBool(PosY);
			
			// Draw the current item
			drawCheatsForcedDropItem();
			
			if (tempMenuSelectionStates == ITEM_DROP_CHANGE_BY_ID)
			{
				drawAdjustableValue(true, tempCurrentMenu);
			}
			break;
		}
		case CHEATS_RESOLVE_FADES:
		{
			// Draw the text for the options
			drawCheatsResolveFades();
			
			if (tempFunctionReturnCode != 0)
			{
				drawResolveFadesMessage();
			}
			break;
		}
		case CHEATS_LOCK_FLAGS:
		{
			// Draw the text for the options
			drawSingleColumnMain();
			
			// Draw the text for which area flags are locked
			drawCheatsLockFlags();
			break;
		}
		case CHEATS_MANAGE_FLAGS_MAIN:
		{
			// Draw the text for the options
			drawCheatsManageFlagsMain(tempMenuSelectionStates);
			
			if (tempSelectedOption > 0)
			{
				drawAdjustableValue(false, tempCurrentMenu);
			}
			break;
		}
		case CHEATS_CLEAR_AREA_FLAGS:
		{
			// Draw the text for the options
			drawSingleColumnSelectedOption();
			
			// Draw the options for which area flags to clear
			drawCheatsClearArea();
			
			// Draw the window asking for confirmation to clear the flags
			if (tempSelectedOption == CLEAR_FLAGS)
			{
				const char *ConfirmationMessage = "This will clear all of the GSWF's for the\nselected area when the area is reloaded.\nAre you sure you want to do this?";
				drawConfirmationWindow(ConfirmationMessage);
			}
			break;
		}
		case STATS_MARIO:
		{
			// Draw the text for the options
			drawSingleColumnMain();
			
			// Draw each of Mario's stats
			drawMarioStats();
			
			switch (tempMenuSelectedOption)
			{
				case 0:
				{
					break;
				}
				case SPECIAL_MOVES:
				{
					drawMarioSpecialMovesOptions();
					break;
				}
				default:
				{
					drawAdjustableValue(false, tempCurrentMenu);
					break;
				}
			}
			break;
		}
		case STATS_PARTNERS:
		{
			// Draw the text for the options
			drawSingleColumnSelectedOption();
			
			// Draw each option to choose from
			drawPartnerStats();
			
			if (tempMenuSelectedOption == STATS_PARTNER_DISPLAY_YOSHI_COLORS)
			{
				drawPartnerChangeYoshiColorOptions();
			}
			else if (tempMenuSelectedOption != 0)
			{
				drawAdjustableValue(false, tempCurrentMenu);
			}
			
			// Draw the error message if the player tried to spawn a partner while either not in the game or in a battle
			if (tempFunctionReturnCode < 0)
			{
				ttyd::party::PartyMembers CurrentPartner = getSelectedOptionPartnerValue();
				
				uint32_t AdditionalOptions = 0;
				if (CurrentPartner == ttyd::party::PartyMembers::kYoshi)
				{
					// Add an extra line for Yoshi
					AdditionalOptions++;
				}
				
				uint32_t FirstFreeSlot = TOGGLE; // enum index starts at 1
				if (MenuVar.CurrentMenuOption == (FirstFreeSlot + AdditionalOptions))
				{
					int32_t PosY = 10;
					drawPartnerFollowerMessage(PosY, true);
				}
			}
			break;
		}
		case STATS_FOLLOWERS:
		{
			// Draw the text for the options
			drawSingleColumnSelectedOption();
			
			// Draw the text for the current follower out
			drawCurrentFollowerOut();
			
			if (tempSelectedOption != 0)
			{
				drawFollowersOptions();
			}
			
			// Draw the error message if the player tried to spawn a partner while either not in the game or in a battle
			if ((((MenuVar.CurrentMenuOption + 1) == BRING_OUT_FOLLOWER) || 
				(tempSelectedOption == BRING_OUT_FOLLOWER)) && 
					(tempFunctionReturnCode < 0))
			{
				int32_t PosY = 50;
				drawPartnerFollowerMessage(PosY, false);
			}
			break;
		}
		case SETTINGS:
		{
			// Draw the text for the options
			drawSingleColumnMain();
			
			if (MenuVar.Timer > 0)
			{
				drawSettingsCurrentWork();
			}
			break;
		}
		case MEMORY:
		{
			// Draw the text for the options
			drawSingleColumnSelectedOption();
			
			// Draw the memory watches
			drawMemoryWatches();
			
			switch (tempFunctionReturnCode)
			{
				case ALL_SLOTS_EMPTY:
				{
					const char *CurrentLine = "All slots are currently empty.";
					int32_t TextPosX = -130;
					int32_t TextPosY = 80;
					drawErrorWindowAutoWidth(CurrentLine, TextPosX, TextPosY);
					break;
				}
				case NO_SLOTS_LEFT:
				{
					const char *CurrentLine = "There are no more free slots.";
					int32_t TextPosX = -130;
					int32_t TextPosY = 80;
					drawErrorWindowAutoWidth(CurrentLine, TextPosX, TextPosY);
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case MEMORY_MODIFY:
		{
			// Draw the main text
			// Don't draw if currently changing display positions
			if (!MenuVar.HideMenu)
			{
				drawMemoryModifyList();
			}
			
			switch (tempSelectedOption)
			{
				case CHANGE_TYPE:
				{
					drawMemoryTypeList();
					break;
				}
				case CHANGE_WATCH_POSITION:
				{
					drawMemoryChangeWatchPosition();
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case MEMORY_CHANGE_ADDRESS:
		{
			// Draw the text for the options
			drawSingleColumnSelectedOption();
			
			// Draw the address, pointer levels, and final value
			drawMemoryChangeAddressList();
			
			if (tempMenuSelectionStates != 0)
			{
				drawMemoryWatchAdjustableValue(tempCurrentMenu);
			}
			break;
		}
		case BATTLES:
		{
			// Close the menu if not in a battle
			if (!getBattleWorkPointer())
			{
				resetMenuToRoot();
				MenuVar.FunctionReturnCode = NOT_IN_BATTLE;
				MenuVar.Timer = secondsToFrames(3);
			}
			else
			{
				// Draw the text for the options
				drawSingleColumnMain();
				
				// Draw the page number
				int32_t PosX = 150;
				int32_t PosY = 180;
				drawPageNumber(PosX, PosY, tempCurrentPage);
				
				// Draw each actor
				drawBattlesActorsList();
			}
			
			// Draw the error message if the player tried to use the battle menu while not in a battle
			if (tempFunctionReturnCode == NOT_IN_BATTLE)
			{
				drawNotInBattleErrorMessage();
			}
			break;
		}
		case BATTLES_CURRENT_ACTOR:
		{
			// Close the menu if not in a battle
			if (!getBattleWorkPointer())
			{
				resetMenuToRoot();
				MenuVar.FunctionReturnCode = NOT_IN_BATTLE;
				MenuVar.Timer = secondsToFrames(3);
			}
			else
			{
				// Draw the text for the options
				drawSingleColumnSelectedOption();
				
				// Draw the HP/FP values
				drawBattlesActorStats();
				
				// Draw the current held item
				drawBattlesActorsHeldItem();
				
				if (tempSelectedOption > 0)
				{
					bool ChangingItem;
					if (tempSelectedOption == CHANGE_HELD_ITEM)
					{
						ChangingItem = true;
					}
					else
					{
						ChangingItem = false;
					}
					
					drawAdjustableValue(ChangingItem, tempCurrentMenu);
				}
			}
			
			// Draw the error message if the player tried to use the battle menu while not in a battle
			if (tempFunctionReturnCode == NOT_IN_BATTLE)
			{
				drawNotInBattleErrorMessage();
			}
			break;
		}
		case BATTLES_STATUSES:
		{
			// Close the menu if not in a battle
			if (!getBattleWorkPointer())
			{
				resetMenuToRoot();
				MenuVar.FunctionReturnCode = NOT_IN_BATTLE;
				MenuVar.Timer = secondsToFrames(3);
			}
			else
			{
				// Draw the text and icons
				drawBattlesStatusesList();
				
				// Draw the page number
				int32_t PosX = 150;
				int32_t PosY = 180;
				drawPageNumber(PosX, PosY, tempCurrentPage);
				
				if (tempSelectedOption > 0)
				{
					drawAdjustableValue(false, tempCurrentMenu);
				}
			}
			
			// Draw the error message if the player tried to use the battle menu while not in a battle
			if (tempFunctionReturnCode == NOT_IN_BATTLE)
			{
				drawNotInBattleErrorMessage();
			}
			break;
		}
		case DISPLAYS_ONSCREEN_TIMER:
		{
			// Draw the text for the options
			drawSingleColumnSelectedOption();
			
			// Draw the bool
			bool CurrentDisplay = Displays[tempMenuSelectedOption];
			const char *CurrentLine = DisplaysLines[tempMenuSelectedOption];
			
			int32_t PosY = 100;
			drawBoolOnOrOff(CurrentDisplay, CurrentLine, PosY);
			
			uint16_t *ButtonComboSource;
			if (tempMenuSelectedOption == ONSCREEN_TIMER)
			{
				ButtonComboSource = OnScreenTimer.ButtonCombo;
			}
			else // tempMenuSelectedOption == ONSCREEN_TIMER_FRAME_COUNTER
			{
				ButtonComboSource = FrameCounter.ButtonCombo;
			}
			
			// Draw each button combo
			drawOnScreenTimerButtonCombos(ButtonComboSource);
			
			if ((tempSelectedOption >= CHANGE_START_PAUSE_RESUME_BUTTON_COMBO) && 
				(tempSelectedOption <= CHANGE_RESET_BUTTON_COMBO))
			{
				drawChangeButtonCombo(&ButtonComboSource[tempSelectedOption - OnScreenTimerOptionsSize]);
			}
			break;
		}
		case DISPLAYS_NO_BUTTON_COMBO:
		{
			// Draw the text for the options
			drawSingleColumnMain();
			
			// Draw the bool
			bool CurrentDisplay = Displays[tempMenuSelectedOption];
			const char *CurrentLine = DisplaysLines[tempMenuSelectedOption];
			int32_t PosY = 140;
			
			drawBoolOnOrOff(CurrentDisplay, CurrentLine, PosY);
			break;
		}
		case WARPS:
		{
			// Draw the text for the options
			drawSingleColumnSelectedOption();
			
			// Draw the warps options
			drawWarpsOptions();
			
			if (tempMenuSelectedOption == SELECTING_VALUE)
			{
				drawAdjustableValue(false, tempCurrentMenu);
			}
			
			// Draw the error message if the player tried to warp while either not in the game or in a battle
			if ((tempSelectedOption == SELECT_WARP) && 
				(tempFunctionReturnCode < 0))
			{
				int32_t PosY = -90;
				drawWarpsErrorMessage(PosY);
			}
			break;
		}
		case WARPS_EVENT:
		{
			// Draw the text for the options
			uint32_t MaxOptionsPerPage = Menu[tempCurrentMenu].TotalMenuOptions;
			int32_t PosY = 140;
			drawSingleColumn(PosY, MaxOptionsPerPage, tempCurrentPage, true);
			
			// Draw the event details
			drawWarpByEventMenuDetails();
			
			if (tempSelectedOption > 0)
			{
				drawAdjustableValue(false, tempCurrentMenu);
			}
			
			// Draw the error message if the player tried to warp while either not in the game or in a battle
			if (tempFunctionReturnCode < 0)
			{
				int32_t PosY = -50;
				drawWarpsErrorMessage(PosY);
			}
			break;
		}
		case WARPS_INDEX:
		{
			// Draw the text for the options
			drawSingleColumnSelectedOption();
			
			// Draw the map and entrance
			drawWarpIndexMapAndEntrance();
			
			if (tempSelectedOption > 0)
			{
				drawAdjustableValue(false, tempCurrentMenu);
			}
			else if (tempMenuSelectedOption > 0)
			{
				drawWarpIndexEntranceList();
			}
			
			// Draw the error message if the player tried to warp while either not in the game or in a battle
			if (tempFunctionReturnCode < 0)
			{
				int32_t PosY = -50;
				drawWarpsErrorMessage(PosY);
			}
			break;
		}
		default:
		{
			drawSingleColumnMain();
			break;
		}
	}
}

void enableOrDisableMenu()
{
	// Check for user input
	uint32_t OpenMenuCombo = PAD_L | PAD_START;
	bool tempMenuIsDisplayed = MenuVar.MenuIsDisplayed;
	
	if (checkButtonCombo(OpenMenuCombo) && !MenuVar.PreventClosingMenu && !MenuVar.ChangingCheatButtonCombo)
	{
		resetMenuToRoot();
		tempMenuIsDisplayed = !tempMenuIsDisplayed;
		MenuVar.MenuIsDisplayed = tempMenuIsDisplayed;
		
		if (tempMenuIsDisplayed)
		{
			// Prevent the menu from checking inputs on the frame that it is opened
			MenuVar.FrameCounter = 1;
			
			// Raise the System Level if not in a battle
			raiseSystemLevel();
			
			// Disable the Pause Menu
			ttyd::win_main::winOpenDisable();
		}
		else
		{
			// Lower the System Level if not in a battle
			lowerSystemLevel();
			
			// Enable the Pause Menu
			ttyd::win_main::winOpenEnable();
		}
	}
	
	// Check if the menu should be displayed or not
	if (tempMenuIsDisplayed)
	{
		// Check for button inputs for the menu
		// Don't check any buttons if the frame counter is not 0
		// Don't check any buttons if currently changing button combos for cheats
		if ((MenuVar.FrameCounter == 0) && !MenuVar.ChangingCheatButtonCombo)
		{
			menuCheckButton();
		}
		
		// Display the menu
		drawFunctionOnDebugLayer(drawMenu);
	}
}

}