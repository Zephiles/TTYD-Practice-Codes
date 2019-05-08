#include "menu.h"
#include "menufunctions.h"
#include "global.h"
#include "commonfunctions.h"
#include "draw.h"
#include "codes.h"
#include "mod.h"
#include "memcard.h"
#include "memorywatch.h"

#include <gc/card.h>
#include <ttyd/swdrv.h>
#include <ttyd/mario_pouch.h>
#include <ttyd/win_party.h>
#include <ttyd/mario_party.h>
#include <ttyd/win_main.h>

#include <cstdio>

namespace mod {

void menuCheckButton()
{
	// Only run if a button was pressed
	uint32_t CurrentButton 					= checkButtonSingleFrame();
	if (CurrentButton == 0)
	{
		return;
	}
	
	uint32_t tempCurrentMenu 				= CurrentMenu;
	uint32_t tempCurrentMenuOption 			= CurrentMenuOption;
	// uint32_t tempSecondaryMenuOption 	= SecondaryMenuOption;
	// uint32_t tempTotalMenuColumns 		= Menu[tempCurrentMenu].TotalMenuColumns;
	// uint32_t tempColumnSplitAmount 		= Menu[tempCurrentMenu].ColumnSplitAmount;
	// uint32_t tempTotalMenuOptions 		= Menu[tempCurrentMenu].TotalMenuOptions;
	uint32_t tempPreviousMenu 				= Menu[tempCurrentMenu].PreviousMenu;
	uint32_t tempSelectedOption 			= SelectedOption;
	uint32_t tempCurrentPage 				= CurrentPage;
	uint32_t tempMenuSelectionStates 		= MenuSelectionStates;
	uint32_t tempMenuSelectedOption 		= MenuSelectedOption;
	// uint32_t tempMenuSecondaryValue 		= MenuSecondaryValue;
	
	switch (tempCurrentMenu)
	{
		case ROOT:
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
					switch (tempCurrentMenuOption)
					{
						case 0:
						{
							// Currently selecting Return on the ROOT, so close the menu
							closeMenu();
							return;
						}
						case BATTLES:
						{
							// Make sure the player is currently in a battle
							if (getBattlePointer())
							{
								// Currently in a battle, so enter the menu
								CurrentMenu = tempCurrentMenuOption;
								resetMenu();
							}
							else
							{
								FunctionReturnCode = NOT_IN_BATTLE;
								Timer = secondsToFrames(3);
							}
							break;
						}
						default:
						{
							// Enter the next menu
							CurrentMenu = tempCurrentMenuOption;
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
					default_DPAD_Actions(CurrentButton);
					break;
				}
				case A:
				{
					switch (tempCurrentMenuOption)
					{
						case 0:
						{
							// Go back to the previous menu
							CurrentMenu = tempPreviousMenu;
							resetMenu();
							break;
						}
						default:
						{
							// Enter the next menu
							CurrentMenu 		= INVENTORY_MAIN;
							MenuSelectedOption 	= tempCurrentMenuOption - 1;
							resetMenu();
							break;
						}
					}
					break;
				}
				case B:
				{
					// Go back to the previous menu
					CurrentMenu = tempPreviousMenu;
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
							CurrentPage = tempCurrentPage;
							
							if ((tempSelectedOption >= DUPLICATE) && 
								(tempSelectedOption <= DELETE))
							{
								CurrentMenuOption = tempCurrentPage * MaxOptionsPerPage;
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
							CurrentPage = tempCurrentPage;
							
							if ((tempSelectedOption >= DUPLICATE) && 
								(tempSelectedOption <= DELETE))
							{
								CurrentMenuOption = tempCurrentPage * MaxOptionsPerPage;
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
							switch (tempCurrentMenuOption)
							{
								case 0:
								{
									// No option has been selected yet
									// Go back to the previous menu
									CurrentMenu = tempPreviousMenu;
									MenuSelectedOption = 0;
									break;
								}
								case ADD_BY_ID:
								{
									if (getFreeSlotPointer())
									{
										Timer 					= 0;
										SelectedOption 			= tempCurrentMenuOption;
										SecondaryMenuOption 	= getHighestAdjustableValueDigit(tempMenuSelectedOption) - 1;
									}
									else
									{
										// Inventory is currently full
										SelectedOption 			= tempCurrentMenuOption;
										FunctionReturnCode 		= INVENTORY_FULL;
										Timer = secondsToFrames(3);
									}
									break;
								}
								case ADD_BY_ICON:
								{
									if (getFreeSlotPointer())
									{
										Timer 					= 0;
										SelectedOption 			= tempCurrentMenuOption;
										SecondaryMenuOption 	= 0;
									}
									else
									{
										// Inventory is currently full
										SelectedOption 			= tempCurrentMenuOption;
										FunctionReturnCode 		= INVENTORY_FULL;
										Timer = secondsToFrames(3);
									}
									break;
								}
								case DUPLICATE:
								{
									if (getTotalItems() > 0)
									{
										if (getFreeSlotPointer())
										{
											Timer 				= 0;
											SelectedOption 		= tempCurrentMenuOption;
											CurrentMenuOption 	= tempCurrentPage * MaxOptionsPerPage;
										}
										else
										{
											// Inventory is currently full
											SelectedOption 		= tempCurrentMenuOption;
											FunctionReturnCode 	= INVENTORY_FULL;
											Timer = secondsToFrames(3);
										}
									}
									else
									{
										// Inventory is currently empty
										SelectedOption 			= tempCurrentMenuOption;
										FunctionReturnCode 		= INVENTORY_EMPTY;
										Timer = secondsToFrames(3);
									}
									break;
								}
								case CHANGE_BY_ID:
								case CHANGE_BY_ICON:
								case DELETE:
								{
									if (getTotalItems() > 0)
									{
										Timer 					= 0;
										SelectedOption 			= tempCurrentMenuOption;
										CurrentMenuOption 		= tempCurrentPage * MaxOptionsPerPage;
									}
									else
									{
										// Inventory is currently empty
										SelectedOption 			= tempCurrentMenuOption;
										FunctionReturnCode 		= INVENTORY_EMPTY;
										Timer = secondsToFrames(3);
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
								Timer = 0;
							}
							else
							{
								// Inventory is currently full
								FunctionReturnCode = INVENTORY_FULL;
								Timer = secondsToFrames(3);
							}
							break;
						}
						case DUPLICATE:
						{
							if (getTotalItems() > 0)
							{
								Timer = 0;
								
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
									FunctionReturnCode = INVENTORY_FULL;
									Timer = secondsToFrames(3);
								}
							}
							else
							{
								// Inventory is currently empty
								SelectedOption 		= tempCurrentMenuOption;
								FunctionReturnCode 	= INVENTORY_EMPTY;
								Timer = secondsToFrames(3);
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
									MenuSecondaryValue = CurrentItem;
								}
								else
								{
									MenuSecondaryValue = 0;
								}
								
								Timer = 0;
								if (tempMenuSelectionStates == 0)
								{
									MenuSelectionStates = tempSelectedOption;
									SecondaryMenuOption = getHighestAdjustableValueDigit(tempMenuSelectedOption) - 1;
								}
							}
							else
							{
								// Inventory is currently empty
								SelectedOption 		= tempCurrentMenuOption;
								FunctionReturnCode 	= INVENTORY_EMPTY;
								Timer = secondsToFrames(3);
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
										SecondaryMenuOption = CurrentItem - LowerBound;
									}
									else
									{
										SecondaryMenuOption = 0;
									}
								}
								else
								{
									SecondaryMenuOption = 0;
								}
								
								Timer = 0;
								if (tempMenuSelectionStates == 0)
								{
									MenuSelectionStates = tempSelectedOption;
								}
							}
							else
							{
								// Inventory is currently empty
								SelectedOption 		= tempCurrentMenuOption;
								FunctionReturnCode 	= INVENTORY_EMPTY;
								Timer = secondsToFrames(3);
							}
							break;
						}
						case DELETE:
						{
							if (getTotalItems() > 0)
							{
								Timer = 0;
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
								SelectedOption 		= tempCurrentMenuOption;
								FunctionReturnCode 	= INVENTORY_EMPTY;
								Timer = secondsToFrames(3);
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
							CurrentMenu = tempPreviousMenu;
							MenuSelectedOption = 0;
							resetMenu();
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
					switch (tempCurrentMenuOption)
					{
						case 0:
						{
							// Go back to the previous menu
							CurrentMenu = tempPreviousMenu;
							resetMenu();
							break;
						}
						default:
						{
							// Enter the next menu
							switch (tempCurrentMenuOption - 1)
							{
								case CHANGE_SEQUENCE:
								{
									CurrentMenu = CHEATS_CHANGE_SEQUENCE;
									break;
								}
								case WALK_THROUGH_WALLS:
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
									CurrentMenu = CHEATS_STANDARD;
									break;
								}
								case LOCK_MARIO_HP_TO_MAX:
								case RUN_FROM_BATTLES:
								case DISABLE_DPAD_OPTIONS_DISPLAY:
								case DISABLE_MENU_SOUNDS:
								case BOBBERY_EARLY:
								{
									CurrentMenu = CHEATS_NO_BUTTON_COMBO;
									break;
								}
								case FORCE_ITEM_DROP:
								{
									CurrentMenu = CHEATS_NPC_FORCE_DROP;
									break;
								}
								case MANAGE_FLAGS:
								{
									CurrentMenu = CHEATS_MANAGE_FLAGS;
									break;
								}
								case CLEAR_AREA_FLAGS:
								default:
								{
									CurrentMenu = CHEATS_CLEAR_AREA_FLAGS;
									break;
								}
							}
							
							MenuSelectedOption = tempCurrentMenuOption - 1;
							resetMenuNoPageReset();
							break;
						}
					}
					break;
				}
				case B:
				{
					// Go back to the previous menu
					CurrentMenu = tempPreviousMenu;
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
		case CHEATS_CHANGE_SEQUENCE:
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
					switch (tempCurrentMenuOption)
					{
						case 0:
						{
							// No option has been selected yet
							// Go back to the previous menu
							CurrentMenu 		= tempPreviousMenu;
							CurrentMenuOption 	= tempMenuSelectedOption + 1;
							MenuSelectedOption 	= 0;
							break;
						}
						case CHANGE_SEQUENCE_VALUE:
						{
							if (tempMenuSelectionStates == 0)
							{
								MenuSecondaryValue 	= static_cast<int32_t>(getSequencePosition());
								MenuSelectionStates = tempCurrentMenuOption;
								SecondaryMenuOption = getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
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
					if (tempMenuSelectionStates != CHANGE_SEQUENCE_VALUE)
					{
						// Go back to the previous menu
						CurrentMenu 		= tempPreviousMenu;
						CurrentMenuOption 	= tempMenuSelectedOption + 1;
						MenuSelectedOption 	= 0;
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
					adjustMenuNoPageEdit(CurrentButton);
					break;
				}
				case A:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							switch (tempCurrentMenuOption)
							{
								case 0:
								{
									// No option has been selected yet
									// Go back to the previous menu
									CurrentMenu 		= tempPreviousMenu;
									CurrentMenuOption 	= tempMenuSelectedOption + 1;
									MenuSelectedOption 	= 0;
									break;
								}
								case TURN_ON_OR_OFF:
								{
									// Flip the bool for the current cheat
									bool CheatActive = Cheat[tempMenuSelectedOption].Active;
									Cheat[tempMenuSelectedOption].Active = !CheatActive;
									break;
								}
								case CHANGE_BUTTON_COMBO:
								{
									SelectedOption 				= tempCurrentMenuOption;
									ChangingCheatButtonCombo 	= true;
									Timer 						= secondsToFrames(3);
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
							CurrentMenu 		= tempPreviousMenu;
							CurrentMenuOption 	= tempMenuSelectedOption + 1;
							MenuSelectedOption 	= 0;
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
					switch (tempCurrentMenuOption)
					{
						case 0:
						{
							// No option has been selected yet
							// Go back to the previous menu
							CurrentMenu 		= tempPreviousMenu;
							CurrentMenuOption 	= tempMenuSelectedOption + 1;
							MenuSelectedOption 	= 0;
							break;
						}
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
								MenuSelectionStates = tempCurrentMenuOption;
								SecondaryMenuOption = getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
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
						CurrentMenu 		= tempPreviousMenu;
						CurrentMenuOption 	= tempMenuSelectedOption + 1;
						MenuSelectedOption 	= 0;
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
					switch (tempCurrentMenuOption)
					{
						case 0:
						{
							// Go back to the previous menu
							CurrentMenu 		= tempPreviousMenu;
							CurrentMenuOption 	= tempMenuSelectedOption + 1;
							MenuSelectedOption 	= 0;
							break;
						}
						default:
						{
							// Clear the current values set for managing flags
							clearMemory(&ManageFlags, sizeof(ManageFlags));
							
							// Enter the next menu
							CurrentMenu 			= CHEATS_MANAGE_FLAGS_MAIN;
							MenuSelectionStates 	= tempCurrentMenuOption;
							CurrentMenuOption 		= 0;
							break;
						}
					}
					break;
				}
				case B:
				{
					// Go back to the previous menu
					CurrentMenu 		= tempPreviousMenu;
					CurrentMenuOption 	= tempMenuSelectedOption + 1;
					MenuSelectedOption 	= 0;
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
							switch (tempCurrentMenuOption)
							{
								case 0:
								{
									// Go back to the previous menu
									CurrentMenu 			= tempPreviousMenu;
									CurrentMenuOption 		= 0;
									MenuSelectionStates 	= 0;
									break;
								}
								default:
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
													SelectedOption 			= tempCurrentMenuOption;
													SecondaryMenuOption 	= getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
													MenuSecondaryValue 		= FlagToSet;
													break;
												}
												case CHANGE_GLOBAL_WORD_VALUE:
												{
													SelectedOption 			= tempCurrentMenuOption;
													SecondaryMenuOption 	= getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
													MenuSecondaryValue 		= getGlobalFlagValue(tempMenuSelectionStates, FlagToSet);
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
													SelectedOption 			= tempCurrentMenuOption;
													SecondaryMenuOption 	= getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
													MenuSecondaryValue 		= FlagToSet;
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
						CurrentMenu 			= tempPreviousMenu;
						CurrentMenuOption 		= 0;
						MenuSelectionStates 	= 0;
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
					switch (tempCurrentMenuOption)
					{
						case 0:
						{
							// No option has been selected yet
							// Go back to the previous menu
							CurrentMenu 		= tempPreviousMenu;
							CurrentMenuOption 	= tempMenuSelectedOption + 1;
							MenuSelectedOption 	= 0;
							break;
						}
						case SELECT_AREA:
						{
							switch (tempSelectedOption)
							{
								case 0:
								{
									SelectedOption 			= tempCurrentMenuOption;
									SecondaryMenuOption 	= 0;
									break;
								}
								default:
								{
									SelectedOption 			= 0;
									MenuSecondaryValue 		= SecondaryMenuOption;
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
									SelectedOption 			= tempCurrentMenuOption;
									SecondaryMenuOption 	= 1;
									break;
								}
								default:
								{
									switch (SecondaryMenuOption)
									{
										case 0:
										{
											// Selected yes
											ClearAreaFlags.CurrentOption = MenuSecondaryValue;
											ClearAreaFlags.FlagsShouldBeCleared = true;
											SelectedOption = 0;
											break;
										}
										default:
										{
											// Selected no
											SelectedOption = 0;
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
						case SELECT_AREA:
						case CLEAR_FLAGS:
						{
							SelectedOption = 0;
							break;
						}
						default:
						{
							// Go back to the previous menu
							CurrentMenu 		= tempPreviousMenu;
							CurrentMenuOption 	= tempMenuSelectedOption + 1;
							MenuSelectedOption 	= 0;
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
					default_DPAD_Actions(CurrentButton);
					break;
				}
				case A:
				{
					switch (tempCurrentMenuOption)
					{
						case 0:
						{
							// Go back to the previous menu
							CurrentMenu = tempPreviousMenu;
							resetMenu();
							break;
						}
						default:
						{
							// Enter the next menu
							CurrentMenu = STATS_MARIO + (tempCurrentMenuOption - 1);
							MenuSelectedOption = 0;
							resetMenu();
							break;
						}
					}
					break;
				}
				case B:
				{
					// Go back to the previous menu
					CurrentMenu = tempPreviousMenu;
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
		case STATS_MARIO:
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
							default_DPAD_Actions(CurrentButton);
							break;
						}
						default:
						{
							adjustMarioStatsSelection(CurrentButton);
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
							switch (tempCurrentMenuOption)
							{
								case 0:
								{
									// Go back to the previous menu
									CurrentMenu = tempPreviousMenu;
									break;
								}
								default:
								{
									SelectedOption = tempCurrentMenuOption;
									CurrentMenuOption = 0;
									break;
								}
							}
							break;
						}
						default:
						{
							switch (tempMenuSelectedOption)
							{
								case 0:
								{
									MenuSelectedOption = tempCurrentMenuOption + 1;
									MenuSelectionStates = tempCurrentMenuOption + 1;
									
									switch (tempCurrentMenuOption + 1)
									{
										case SPECIAL_MOVES:
										{
											SecondaryMenuOption = 0;
											break;
										}
										default:
										{
											SecondaryMenuOption = getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
										
											uint32_t offset = getMarioStatsValueOffset(tempCurrentMenuOption + 1);
											if (offset == 0)
											{
												break;
											}
											
											// Set the current value in the display to the current value of the address being changed
											if (((tempCurrentMenuOption + 1) >= PIANTAS_STORED) && 
												((tempCurrentMenuOption + 1) <= CURRENT_PIANTAS))
											{
												uint32_t PiantaParlorPtr = *reinterpret_cast<uint32_t *>(PiantaParlorAddressesStart);
												MenuSecondaryValue = *reinterpret_cast<int32_t *>(PiantaParlorPtr + offset);
											}
											else
											{
												uint32_t PouchPtr = reinterpret_cast<uint32_t>(
													ttyd::mario_pouch::pouchGetPtr());
												
												MenuSecondaryValue = *reinterpret_cast<int16_t *>(PouchPtr + offset);
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
								CurrentMenu = tempPreviousMenu;
								resetMenu();
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
							default_DPAD_Actions(CurrentButton);
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
							switch (tempCurrentMenuOption)
							{
								case 0:
								{
									// Go back to the previous menu
									CurrentMenu = tempPreviousMenu;
									break;
								}
								default:
								{
									SelectedOption = tempCurrentMenuOption;
									CurrentMenuOption = 0;
									break;
								}
							}
							break;
						}
						default:
						{
							switch (tempMenuSelectedOption)
							{
								case 0:
								{
									uint32_t PartnerEnabledAddress = reinterpret_cast<uint32_t>(
											getPartnerEnabledAddress());
									
									switch (tempCurrentMenuOption + 1)
									{
										case PARTNER_HP:
										{
											MenuSecondaryValue = *reinterpret_cast<int16_t *>(
												PartnerEnabledAddress + 0x6);
											
											MenuSelectedOption = tempCurrentMenuOption + 1;
											MenuSelectionStates = tempCurrentMenuOption + 1;
											SecondaryMenuOption = getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
											break;
										}
										case PARTNER_MAX_HP:
										{
											MenuSecondaryValue = *reinterpret_cast<int16_t *>(
												PartnerEnabledAddress + 0x2);
											
											MenuSelectedOption = tempCurrentMenuOption + 1;
											MenuSelectionStates = tempCurrentMenuOption + 1;
											SecondaryMenuOption = getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
											break;
										}
										case PARTNER_RANK:
										{
											MenuSecondaryValue = *reinterpret_cast<int16_t *>(
												PartnerEnabledAddress + 0xA);
											
											MenuSelectedOption = tempCurrentMenuOption + 1;
											MenuSelectionStates = tempCurrentMenuOption + 1;
											SecondaryMenuOption = getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
											break;
										}
										case TOGGLE:
										{
											bool PartnerEnabled = *reinterpret_cast<bool *>(
												PartnerEnabledAddress + 1);
											
											*reinterpret_cast<bool *>(
												PartnerEnabledAddress + 1) = !PartnerEnabled;
											
											// Reset the partner menu if the pause menu is open
											uint32_t SystemLevel = getSystemLevel();
											if (SystemLevel == 15)
											{
												uint32_t *PauseMenuPointer = 
													*reinterpret_cast<uint32_t **>(PauseMenuStartAddress);
												ttyd::win_party::winPartyExit(PauseMenuPointer);
												ttyd::win_party::winPartyInit(PauseMenuPointer);
											}
											break;
										}
										case BRING_OUT:
										case REMOVE:
										{
											if (checkIfPartnerOutSelected())
											{
												// Currently REMOVE
												removePartnerFromOverworld();
											}
											else
											{
												// Currently BRING_OUT
												// Make sure a file is loaded
												if (checkIfInGame())
												{
													Timer = 0;
													
													// Preserve the current value of the enabled bool
													bool CurrentPartnerEnabled = *reinterpret_cast<bool *>(
														PartnerEnabledAddress + 1);
													
													// Make sure the selected partner is enabled, as marioPartyEntry needs it to be enabled
													*reinterpret_cast<bool *>(
														PartnerEnabledAddress + 1) = true;
													
													// Bring the partner out
													ttyd::mario_party::marioPartyEntry(
														static_cast<ttyd::party::PartyMembers>(
															getSelectedOptionPartnerValue()));
													
													// Restore the value of the enabled bool
													*reinterpret_cast<bool *>(
														PartnerEnabledAddress + 1) = CurrentPartnerEnabled;
												}
												else
												{
													FunctionReturnCode 	= NOT_IN_GAME;
													Timer 				= secondsToFrames(3);
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
								CurrentMenu = tempPreviousMenu;
								resetMenu();
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
						default_DPAD_Actions(CurrentButton);
					}
					break;
				}
				case A:
				{
					switch (tempCurrentMenuOption)
					{
						case 0:
						{
							// Go back to the previous menu
							CurrentMenu = tempPreviousMenu;
							break;
						}
						default:
						{
							switch (tempSelectedOption)
							{
								case 0:
								{
									switch (tempCurrentMenuOption)
									{
										case BRING_OUT_FOLLOWER:
										{
											// Make sure a file is loaded
											if (checkIfInGame())
											{
												Timer 				= 0;
												SecondaryMenuOption = 0;
												SelectedOption = tempCurrentMenuOption;
											}
											else
											{
												FunctionReturnCode 	= NOT_IN_GAME;
												Timer 				= secondsToFrames(3);
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
										Timer = 0;
									}
									else
									{
										FunctionReturnCode 	= NOT_IN_GAME;
										Timer 				= secondsToFrames(3);
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
					if (tempSelectedOption == 0)
					{
						// Go back to the previous menu
						CurrentMenu = tempPreviousMenu;
						resetMenu();
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
					Timer = 0;
					default_DPAD_Actions(CurrentButton);
					break;
				}
				case A:
				{
					switch (tempCurrentMenuOption)
					{
						case 0:
						{
							// Go back to the previous menu
							CurrentMenu = tempPreviousMenu;
							resetMenu();
							break;
						}
						case LOAD_SETTINGS:
						{
							int32_t ReturnCode = loadSettings(MenuSettings.FileName, 
								MenuSettings.FileInfo, MenuSettings.WorkArea);
							
							switch (ReturnCode)
							{
								case CARD_ERROR_READY:
								{
									MenuSettings.ReturnCode = LOAD_SUCCESSFUL;
									Timer = secondsToFrames(3);
									break;
								}
								case CARD_ERROR_NOFILE:
								{
									MenuSettings.ReturnCode = LOAD_FAILED_NO_FILE;
									Timer = secondsToFrames(3);
									break;
								}
								default:
								{
									MenuSettings.ReturnCode = LOAD_FAILED;
									Timer = secondsToFrames(3);
									break;
								}
							}
							break;
						}
						case SAVE_SETTINGS:
						{
							int32_t ReturnCode = writeSettings(MenuSettings.Description, 
								MenuSettings.FileName, MenuSettings.FileInfo, MenuSettings.WorkArea);
							
							switch (ReturnCode)
							{
								case CARD_ERROR_READY:
								{
									MenuSettings.ReturnCode = SAVE_SUCCESSFUL;
									Timer = secondsToFrames(3);
									break;
								}
								default:
								{
									MenuSettings.ReturnCode = SAVE_FAILED;
									Timer = secondsToFrames(3);
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
					// Go back to the root
					CurrentMenu = tempPreviousMenu;
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
		case MEMORY:
		{
			uint32_t MaxOptionsPerPage = 9;
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							default_DPAD_Actions(CurrentButton);
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
							switch (tempCurrentMenuOption)
							{
								case 0:
								{
									// Go back to the previous menu
									CurrentMenu = tempPreviousMenu;
									resetMenu();
									break;
								}
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
										FunctionReturnCode = NO_SLOTS_LEFT;
										Timer = secondsToFrames(3);
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
											SelectedOption 		= tempCurrentMenuOption;
											CurrentMenuOption 	= tempCurrentPage * MaxOptionsPerPage;
										}
										else
										{
											// There are no more free slots
											FunctionReturnCode = NO_SLOTS_LEFT;
											Timer = secondsToFrames(3);
										}
									}
									else
									{
										// All slots are empty
										FunctionReturnCode = ALL_SLOTS_EMPTY;
										Timer = secondsToFrames(3);
									}
									break;
								}
								case MODIFY_WATCH:
								{
									uint32_t tempAddress = MemoryWatch[0].Address;
									if (tempAddress)
									{
										SelectedOption 		= tempCurrentMenuOption;
										CurrentMenuOption 	= tempCurrentPage * MaxOptionsPerPage;
									}
									else
									{
										// All slots are empty
										FunctionReturnCode = ALL_SLOTS_EMPTY;
										Timer = secondsToFrames(3);
									}
									break;
								}
								case DELETE_WATCH:
								{
									uint32_t tempAddress = MemoryWatch[0].Address;
									if (tempAddress)
									{
										SelectedOption 		= tempCurrentMenuOption;
										CurrentMenuOption 	= tempCurrentPage * MaxOptionsPerPage;
									}
									else
									{
										// All slots are empty
										FunctionReturnCode = ALL_SLOTS_EMPTY;
										Timer = secondsToFrames(3);
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
									FunctionReturnCode = NO_SLOTS_LEFT;
									Timer = secondsToFrames(3);
								}
							}
							else
							{
								// All slots are empty
								closeSecondaryMenu();
								FunctionReturnCode = NO_SLOTS_LEFT;
								Timer = secondsToFrames(3);
							}
							break;
						}
						case MODIFY_WATCH:
						{
							uint32_t tempAddress = MemoryWatch[0].Address;
							if (tempAddress)
							{
								// Enter the next menu
								CurrentMenu = MEMORY_MODIFY;
								MenuSelectedOption = tempCurrentMenuOption;
								resetMenuNoPageReset();
							}
							else
							{
								// All slots are empty
								closeSecondaryMenu();
								FunctionReturnCode = NO_SLOTS_LEFT;
								Timer = secondsToFrames(3);
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
								FunctionReturnCode = NO_SLOTS_LEFT;
								Timer = secondsToFrames(3);
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
							CurrentMenu = tempPreviousMenu;
							resetMenu();
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
							switch (tempCurrentMenuOption)
							{
								case 0:
								{
									// Go back to the previous menu
									CurrentMenu = tempPreviousMenu;
									MenuSelectedOption = 0;
									resetMenuNoPageReset();
									break;
								}
								case CHANGE_ADDRESS:
								{
									// Enter the next menu
									CurrentMenu = MEMORY_CHANGE_ADDRESS;
									resetMenuNoPageReset();
									break;
								}
								case CHANGE_TYPE:
								{
									SelectedOption = tempCurrentMenuOption;
									SecondaryMenuOption = MemoryWatch[tempMenuSelectedOption].Type;
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
									MemoryWatchPosition.PosX = MemoryWatch[tempMenuSelectedOption].PosX;
									MemoryWatchPosition.PosY = MemoryWatch[tempMenuSelectedOption].PosY;
									SelectedOption = tempCurrentMenuOption;
									HideMenu = true;
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
							CurrentMenu = tempPreviousMenu;
							MenuSelectedOption = 0;
							resetMenuNoPageReset();
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
									switch (tempCurrentMenuOption)
									{
										case 0:
										{
											// Go back to the previous menu
											CurrentMenu = tempPreviousMenu;
											resetMenuNoPageReset();
											break;
										}
										case CHANGE_ADDRESS_OR_POINTERS:
										{
											SelectedOption 		= tempCurrentMenuOption;
											CurrentMenuOption 	= 0;
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
									MenuSelectionStates = tempCurrentMenuOption + 1;
									SecondaryMenuOption = 7;
									
									switch (tempCurrentMenuOption)
									{
										case 0:
										{
											MemoryWatchSecondaryValue = MemoryWatch[tempMenuSelectedOption].Address;
											break;
										}
										default:
										{
											MemoryWatchSecondaryValue = MemoryWatch[tempMenuSelectedOption].AddressOffset[tempCurrentMenuOption - 1];
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
									CurrentMenu = tempPreviousMenu;
									resetMenuNoPageReset();
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
			if (!getBattlePointer())
			{
				resetMenuToRoot();
				FunctionReturnCode = NOT_IN_BATTLE;
				Timer = secondsToFrames(3);
			}
			else
			{
				uint32_t MaxOptionsPerPage = 13;
				switch (CurrentButton)
				{
					case DPADDOWN:
					case DPADUP:
					{
						switch (tempSelectedOption)
						{
							case 0:
							{
								default_DPAD_Actions(CurrentButton);
								break;
							}
							default:
							{
								adjustBattlesActorSelection(CurrentButton);
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
								switch (tempCurrentMenuOption)
								{
									case 0:
									{
										// Go back to the previous menu
										CurrentMenu = tempPreviousMenu;
										resetMenu();
										break;
									}
									default:
									{
										SelectedOption 		= tempCurrentMenuOption;
										CurrentMenuOption 	= tempCurrentPage * MaxOptionsPerPage;
										break;
									}
								}
								break;
							}
							default:
							{
								// Make sure the current slot isn't empty
								void *CurrentActorPointer = getActorPointer(tempCurrentMenuOption + 1); // Add 1 to skip System
								if (CurrentActorPointer)
								{
									// Go to the next menu
									CurrentMenu = BATTLES_CURRENT_ACTOR;
									MenuSelectedOption = tempCurrentMenuOption + 1; // Add 1 to skip System
									resetMenu();
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
								CurrentMenu = tempPreviousMenu;
								resetMenu();
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
		case BATTLES_CURRENT_ACTOR:
		{
			// Close the menu if not in a battle
			if (!getBattlePointer())
			{
				resetMenuToRoot();
				FunctionReturnCode = NOT_IN_BATTLE;
				Timer = secondsToFrames(3);
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
							default_DPAD_Actions(CurrentButton);
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
								
								switch (tempCurrentMenuOption)
								{
									case 0:
									{
										// Go back to the previous menu
										CurrentMenu = tempPreviousMenu;
										resetMenu();
										break;
									}
									case CLEAR_HELD_ITEM:
									{
										if (ActorAddress == 0)
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
										CurrentMenu = BATTLES_STATUSES;
										resetMenu();
										break;
									}
									default:
									{
										if (ActorAddress == 0)
										{
											break;
										}
										
										SelectedOption = tempCurrentMenuOption;
										MenuSelectionStates = tempCurrentMenuOption;
										SecondaryMenuOption = getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
										
										switch (tempCurrentMenuOption)
										{
											case CHANGE_ACTOR_HP:
											{
												MenuSecondaryValue = *reinterpret_cast<int16_t *>(
													ActorAddress + 0x10C);
												break;
											}
											case CHANGE_ACTOR_MAX_HP:
											{
												MenuSecondaryValue = *reinterpret_cast<int16_t *>(
													ActorAddress + 0x108);
												break;
											}
											case CHANGE_ACTOR_FP:
											{
												MenuSecondaryValue = *reinterpret_cast<int16_t *>(
													ActorAddress + 0x112);
												break;
											}
											case CHANGE_ACTOR_MAX_FP:
											{
												MenuSecondaryValue = *reinterpret_cast<int16_t *>(
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
												
												MenuSecondaryValue = *reinterpret_cast<int32_t *>(
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
								CurrentMenu = tempPreviousMenu;
								resetMenu();
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
			if (!getBattlePointer())
			{
				resetMenuToRoot();
				FunctionReturnCode = NOT_IN_BATTLE;
				Timer = secondsToFrames(3);
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
							
							if (ActorAddress == 0)
							{
								break;
							}
							
							SelectedOption = tempCurrentMenuOption + 1;
							MenuSelectionStates = tempCurrentMenuOption + 1;
							SecondaryMenuOption = getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
							
							uint32_t Counter = 0;
							if (tempCurrentMenuOption >= 1) // Sleep flags
							{
								Counter++;
								
								if (tempCurrentMenuOption >= 18) // Can't use items
								{
									Counter++;
									
									if (tempCurrentMenuOption >= 20) // Explosion turns left
									{
										Counter++;
									}
								}
							}
							
							MenuSecondaryValue = *reinterpret_cast<int8_t *>(
								ActorAddress + (0x118 + tempCurrentMenuOption + Counter));
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
								CurrentMenu = tempPreviousMenu;
								resetMenu();
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
					default_DPAD_Actions(CurrentButton);
					break;
				}
				case A:
				{
					switch (tempCurrentMenuOption)
					{
						case 0:
						{
							// Go back to the previous menu
							CurrentMenu = tempPreviousMenu;
							resetMenu();
							break;
						}
						default:
						{
							// Enter the next menu
							switch (tempCurrentMenuOption - 1)
							{
								case ONSCREEN_TIMER:
								{
									CurrentMenu = DISPLAYS_ONSCREEN_TIMER;
									break;
								}
								default:
								{
									CurrentMenu = DISPLAYS_NO_BUTTON_COMBO;
									break;
								}
							}
							
							MenuSelectedOption = tempCurrentMenuOption - 1;
							resetMenu();
							break;
						}
					}
					break;
				}
				case B:
				{
					// Go back to the previous menu
					CurrentMenu = tempPreviousMenu;
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
		case DISPLAYS_ONSCREEN_TIMER:
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
					switch (tempSelectedOption)
					{
						case 0:
						{
							switch (tempCurrentMenuOption)
							{
								case 0:
								{
									// No option has been selected yet
									// Go back to the previous menu
									CurrentMenu 		= tempPreviousMenu;
									CurrentMenuOption 	= tempMenuSelectedOption + 1;
									MenuSelectedOption 	= 0;
									break;
								}
								case ONSCREEN_TIMER_TURN_ON_OR_OFF:
								{
									// Flip the bool for the current cheat
									bool DisplayActive = Displays[tempMenuSelectedOption];
									Displays[tempMenuSelectedOption] = !DisplayActive;
									break;
								}
								case CHANGE_START_PAUSE_RESUME_BUTTON_COMBO:
								case CHANGE_RESET_BUTTON_COMBO:
								{
									SelectedOption 				= tempCurrentMenuOption;
									ChangingCheatButtonCombo 	= true;
									Timer 						= secondsToFrames(3);
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
					if ((tempSelectedOption >= CHANGE_START_PAUSE_RESUME_BUTTON_COMBO) && 
						(tempSelectedOption <= CHANGE_RESET_BUTTON_COMBO))
					{
						// The work for this is done in drawChangeButtonCombo
						break;
					}
					else
					{
						// Go back to the previous menu
						CurrentMenu 		= tempPreviousMenu;
						CurrentMenuOption 	= tempMenuSelectedOption + 1;
						MenuSelectedOption 	= 0;
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
				case DPADDOWN:
				case DPADUP:
				{
					default_DPAD_Actions(CurrentButton);
					break;
				}
				case A:
				{
					switch (tempCurrentMenuOption)
					{
						case 0:
						{
							// No option has been selected yet
							// Go back to the previous menu
							CurrentMenu 		= tempPreviousMenu;
							CurrentMenuOption 	= tempMenuSelectedOption + 1;
							MenuSelectedOption 	= 0;
							break;
						}
						case TURN_ON_OR_OFF:
						{
							// Flip the bool for the current display
							bool DisplayActive = Displays[tempMenuSelectedOption];
							Displays[tempMenuSelectedOption] = !DisplayActive;
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
					CurrentMenu 		= tempPreviousMenu;
					CurrentMenuOption 	= tempMenuSelectedOption + 1;
					MenuSelectedOption 	= 0;
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
							default_DPAD_Actions(CurrentButton);
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
							switch (tempCurrentMenuOption)
							{
								case 0:
								{
									// Go back to the previous menu
									CurrentMenu = tempPreviousMenu;
									resetMenu();
									break;
								}
								case WARP_BY_INDEX:
								{
									// Enter the next menu
									CurrentMenu = WARPS_INDEX;
									resetMenu();
									break;
								}
								case SELECT_WARP:
								{
									Timer = 0;
									SelectedOption = tempCurrentMenuOption;
									CurrentMenuOption = 0;
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
												Timer = 0;
												MenuSelectedOption = SELECTING_VALUE;
												
												SecondaryMenuOption = getHighestAdjustableValueDigit(
													tempCurrentMenu) - 1;
													
												MenuSecondaryValue = getCurrentPitFloor();
											}
											else
											{
												FunctionReturnCode 	= NOT_IN_GAME;
												Timer 				= secondsToFrames(3);
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
											FunctionReturnCode 	= ReturnCode;
											Timer 				= secondsToFrames(3);
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
							// Go back to the root
							CurrentMenu = tempPreviousMenu;
							resetMenu();
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
									switch (tempCurrentMenuOption)
									{
										case 0:
										{
											// Go back to the previous menu
											CurrentMenu = tempPreviousMenu;
											resetMenu();
											break;
										}
										case INDEX_VIEW_CURRENT_MAP_ENTRANCES:
										{
											MenuSelectedOption = tempCurrentMenuOption;
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
													FunctionReturnCode 	= ReturnCode;
													Timer 				= secondsToFrames(3);
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
											Timer = 0;
											SelectedOption = tempCurrentMenuOption;
											
											SecondaryMenuOption = getHighestAdjustableValueDigit(
												tempCurrentMenu) - 1;
											
											if (tempCurrentMenuOption == INDEX_SELECT_MAP)
											{
												int32_t MapIndex = getMapIndex();
												if (MapIndex < 0)
												{
													MenuSecondaryValue = 0;
												}
												else
												{
													MenuSecondaryValue = MapIndex;
												}
											}
											else
											{
												MenuSecondaryValue = static_cast<int32_t>(WarpByIndex.EntranceId);
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
									// Go back to the root
									CurrentMenu = tempPreviousMenu;
									resetMenu();
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
							MenuSelectedOption = 0;
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
	if (!HideMenu)
	{
		drawMenuWindow();
	}
	
	uint32_t tempCurrentMenu 			= CurrentMenu;
	// uint32_t tempCurrentMenuOption 	= CurrentMenuOption;
	uint32_t tempSelectedOption 		= SelectedOption;
	uint32_t tempMenuSelectionStates 	= MenuSelectionStates;
	// uint32_t tempSecondaryMenuOption = SecondaryMenuOption;
	// int32_t tempMenuSecondaryValue 	= MenuSecondaryValue;
	uint32_t tempMenuSelectedOption 	= MenuSelectedOption;
	uint32_t tempCurrentPage 			= CurrentPage;
	int32_t tempFunctionReturnCode 		= FunctionReturnCode;
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
			drawSingleColumnMain();
			
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
							drawErrorMessage(CurrentLine);
							break;
						}
						case INVENTORY_EMPTY:
						{
							// Inventory is currently empty
							const char *CurrentLine = "The inventory is currently empty.";
							drawErrorMessage(CurrentLine);
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
								MenuSelectionStates 		= tempMenuSelectionStates;
								closeSecondaryMenu();
							}
							break;
						}
						case INVENTORY_EMPTY:
						{
							// Inventory is currently empty
							const char *CurrentLine = "The inventory is currently empty.";
							drawErrorMessage(CurrentLine);
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
			drawSingleColumn(MaxOptionsPerPage, tempCurrentPage);
			
			// Draw the page number
			int32_t PosX = 150;
			int32_t PosY = 180;
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
			drawSingleColumnMain();
			
			// Draw the bool
			int32_t PosY = 80;
			drawCheatsBool(PosY);
			
			// Draw the button combo
			uint16_t *CurrentButtonCombo = &Cheat[tempMenuSelectedOption].ButtonCombo;
			const char *Description = "Button Combo (Can be pressed in any order)";
			PosY -= 60;
			drawButtonCombo(*CurrentButtonCombo, PosY, Description);
			
			if (tempSelectedOption == CHANGE_BUTTON_COMBO)
			{
				drawChangeButtonCombo(*CurrentButtonCombo);
			}
			break;
		}
		case CHEATS_NO_BUTTON_COMBO:
		{
			// Draw the text for the options
			drawSingleColumnMain();
			
			// Draw the bool
			int32_t PosY = 100;
			drawCheatsBool(PosY);
			break;
		}
		case CHEATS_NPC_FORCE_DROP:
		{
			// Draw the text for the options
			drawSingleColumnMain();
			
			// Draw the bool
			int32_t PosY = 80;
			drawCheatsBool(PosY);
			
			// Draw the current item
			drawCheatsForcedDropItem();
			
			if (tempMenuSelectionStates == ITEM_DROP_CHANGE_BY_ID)
			{
				drawAdjustableValue(true, tempCurrentMenu);
			}
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
			drawSingleColumnMain();
			
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
			
			if (tempMenuSelectedOption != 0)
			{
				switch (tempMenuSelectedOption)
				{
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
			}
			break;
		}
		case STATS_PARTNERS:
		{
			// Draw the text for the options
			drawSingleColumnMain();
			
			// Draw each option to choose from
			drawPartnerStats();
			
			if (tempMenuSelectedOption != 0)
			{
				drawAdjustableValue(false, tempCurrentMenu);
			}
			
			// Draw the error message if the player tried to spawn a partner while either not in the game or in a battle
			if ((CurrentMenuOption == (BRING_OUT - 1)) && 
				(tempFunctionReturnCode < 0))
			{
				drawPartnerErrorMessage();
			}
			break;
		}
		case STATS_FOLLOWERS:
		{
			// Draw the text for the options
			drawSingleColumnMain();
			
			// Draw the text for the current follower out
			drawCurrentFollowerOut();
			
			if (tempSelectedOption != 0)
			{
				drawFollowersOptions();
			}
			
			// Draw the error message if the player tried to spawn a partner while either not in the game or in a battle
			if (((CurrentMenuOption == BRING_OUT_FOLLOWER) || 
				(tempSelectedOption == BRING_OUT_FOLLOWER)) && 
					(tempFunctionReturnCode < 0))
			{
				drawFollowersErrorMessage();
			}
			break;
		}
		case SETTINGS:
		{
			// Draw the text for the options
			drawSingleColumnMain();
			
			if (Timer > 0)
			{
				drawSettingsCurrentWork();
			}
			break;
		}
		case MEMORY:
		{
			// Draw the text for the options
			drawSingleColumnMain();
			
			// Draw the memory watches
			drawMemoryWatches();
			
			switch (tempFunctionReturnCode)
			{
				case ALL_SLOTS_EMPTY:
				{
					const char *CurrentLine = "All slots are currently empty.";
					drawErrorMessage(CurrentLine);
					break;
				}
				case NO_SLOTS_LEFT:
				{
					const char *CurrentLine = "There are no more free slots.";
					drawErrorMessage(CurrentLine);
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
			if (!HideMenu)
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
			drawSingleColumnMain();
			
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
			if (!getBattlePointer())
			{
				resetMenuToRoot();
				FunctionReturnCode = NOT_IN_BATTLE;
				Timer = secondsToFrames(3);
			}
			else
			{
				// Draw the text for the options
				drawSingleColumnMain();
				
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
			if (!getBattlePointer())
			{
				resetMenuToRoot();
				FunctionReturnCode = NOT_IN_BATTLE;
				Timer = secondsToFrames(3);
			}
			else
			{
				// Draw the text for the options
				drawSingleColumnMain();
				
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
			if (!getBattlePointer())
			{
				resetMenuToRoot();
				FunctionReturnCode = NOT_IN_BATTLE;
				Timer = secondsToFrames(3);
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
			drawSingleColumnMain();
			
			// Draw the bool
			bool CurrentDisplay = Displays[tempMenuSelectedOption];
			const char *CurrentLine = DisplaysLines[tempMenuSelectedOption + 1];
			int32_t PosY = 60;
			drawBoolOnOrOff(CurrentDisplay, CurrentLine, PosY);
			
			// Draw each button combo
			drawOnScreenTimerButtonCombos();
			
			if ((tempSelectedOption >= CHANGE_START_PAUSE_RESUME_BUTTON_COMBO) && 
				(tempSelectedOption <= CHANGE_RESET_BUTTON_COMBO))
			{
				drawChangeButtonCombo(OnScreenTimer.ButtonCombo[
					tempSelectedOption - OnScreenTimerOptionsSize]);
			}
			break;
		}
		case DISPLAYS_NO_BUTTON_COMBO:
		{
			// Draw the text for the options
			drawSingleColumnMain();
			
			// Draw the bool
			bool CurrentDisplay = Displays[tempMenuSelectedOption];
			const char *CurrentLine = DisplaysLines[tempMenuSelectedOption + 1];
			int32_t PosY = 100;
			
			drawBoolOnOrOff(CurrentDisplay, CurrentLine, PosY);
			break;
		}
		case WARPS:
		{
			// Draw the text for the options
			drawSingleColumnMain();
			
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
				drawWarpsErrorMessage();
			}
			break;
		}
		case WARPS_INDEX:
		{
			// Draw the text for the options
			drawSingleColumnMain();
			
			// Draw the map and entrance
			drawWarpIndexMapAndEntrance();
			
			if (tempMenuSelectedOption > 0)
			{
				drawWarpIndexEntranceList();
			}
			
			if (tempSelectedOption > 0)
			{
				drawAdjustableValue(false, tempCurrentMenu);
			}
			
			// Draw the error message if the player tried to warp while either not in the game or in a battle
			if (tempFunctionReturnCode < 0)
			{
				drawWarpsErrorMessage();
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
	uint16_t OpenMenuCombo = PAD_L | PAD_START;
	bool tempMenuIsDisplayed = MenuIsDisplayed;
	
	if (checkButtonCombo(OpenMenuCombo) && !PreventClosingMenu && !ChangingCheatButtonCombo)
	{
		resetMenuToRoot();
		tempMenuIsDisplayed = !tempMenuIsDisplayed;
		MenuIsDisplayed = tempMenuIsDisplayed;
		
		if (tempMenuIsDisplayed)
		{
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
		if ((FrameCounter == 0) && !ChangingCheatButtonCombo)
		{
			menuCheckButton();
		}
		
		// Display the menu
		drawFunctionOnDebugLayer(drawMenu);
	}
}

}