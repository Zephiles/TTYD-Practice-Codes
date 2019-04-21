#pragma once

#include <cstdint>

namespace mod {

void closeMenu();
void closeSecondaryMenu();
void resetMenu();
void resetMenuNoPageReset();
void resetMenuToRoot();
void resetAndCloseSecondaryMenu();
void resetImportantItemsPauseMenu();
void recheckUpgradesBattles(int32_t item);

void raiseSystemLevel();
void lowerSystemLevel();

const char *getItemName(int16_t item);
void *getFreeSlotPointer();
int32_t getTotalItems();
int32_t *getUpperAndLowerBounds(int32_t *tempArray, uint32_t currentMenu);
uint32_t *getPouchAddressAndSize(uint32_t *tempArray);
bool checkForItemsOnNextPage(uint32_t currentPage);
bool checkForClosingErrorMessage();

void correctInventoryCurrentMenuOptionAndPage(uint32_t maxOptionsPerPage);
uint32_t getHighestAdjustableValueDigit(uint32_t currentMenu);
int32_t getDigitBeingChanged(int32_t number, int32_t valueChangedBy);
void setAdjustableValueToMax(uint32_t currentMenu);
void setAdjustableValueToMin(uint32_t currentMenu);
uint32_t adjustableValueButtonControls(uint32_t currentMenu);
uint32_t addByIconButtonControls(uint32_t currentMenu);
uint32_t memoryAddressTypeButtonControls();
uint32_t memoryChangeWatchPositionButtonControls();
uint32_t marioSpecialMovesButtonControls();
uint32_t followersOptionsButtonControls();
void adjustMenuItemBoundsMain(int32_t valueChangedBy, int32_t lowerBound, int32_t upperBound);
void adjustMenuItemBounds(int32_t valueChangedBy, uint32_t currentMenu);
void adjustAddByIdValue(int32_t value, uint32_t currentMenu);
uint32_t getMarioStatsValueOffset(uint32_t currentMenuOption);
void setMarioStatsValue(uint32_t currentMenuOption);
void setPartnerStatsValue(uint32_t currentMenuOption);
void setBattlesActorValue(uint32_t currentMenuOption);
void setBattlesActorStatusValue(uint32_t currentMenuOption);
uint8_t getSelectedOptionPartnerValue();
void *getPartnerEnabledAddress();
bool checkIfPartnerOutSelected();
void setAddByIdValue(void *address);
void setAddByIconValue(void *address);
void duplicateCurrentItem(void *address);
void deleteItem();
int32_t changeItem();
int32_t getGW(uint32_t gw);
void setGW(uint32_t gw, uint32_t value);
bool getGF(uint32_t gf);
void setGF(uint32_t gf);
int32_t getGlobalFlagValue(uint32_t currentMenu, uint32_t flag);
void cheatClearAreaFlags(uint32_t currentMenuOption);
const char *getMapFromIndex(int32_t index);
int32_t getMapIndex();

// uint8_t *getButtonsPressedDynamic(uint8_t *buttonArray, uint16_t currentButtonCombo);
uint8_t *getButtonsPressed(uint8_t *buttonArray, uint16_t currentButtonCombo);
char *createButtonStringArray(char *tempArray, uint8_t *buttonArray);
bool incrementCheatsBButtonCounter(uint32_t buttonInput);
bool cheatsManageTimer(uint32_t buttonInput);

bool checkForDPADInput();
uint32_t checkButtonSingleFrame();
// void correctPageSingleColumn(uint32_t button, uint8_t &currentPage)
void default_DPAD_Actions(uint32_t button);
void adjustMenuNoPageEdit(uint32_t button);
void adjustCheatsManageFlagsMainMenu(uint32_t button);
void adjustMenuSelectionInventory(uint32_t button);
void adjustCheatClearAreaFlagSelection(uint32_t button);
void adjustMarioStatsSelection(uint32_t button);
void adjustPartnerStatsSelection(uint32_t button);
void adjustMemoryWatchSelection(uint32_t button);
void adjustMemoryChangeAddressOrPointerSelection(uint32_t button);
void adjustBattlesActorSelection(uint32_t button);
void adjustBattlesStatusSelection(uint32_t button);
void adjustWarpsSelection(uint32_t button);
void adjustIndexWarpCurrentMapEntrancesPage(uint32_t button);

void adjustMenuSelectionVertical(uint32_t button, uint8_t &currentMenuOption, 
	uint8_t &currentPage, uint32_t totalMenuOptions, 
		uint32_t maxOptionsPerPage, uint32_t maxOptionsPerRow,
			bool allowUpwardsSnapFromLeftOrRight);

void adjustMenuSelectionHorizontal(uint32_t button, uint8_t &currentMenuOption, 
	uint8_t &currentPage, uint32_t totalMenuOptions, 
		uint32_t maxOptionsPerPage, uint32_t maxOptionsPerRow, 
			bool allowUpwardsSnapFromLeftOrRight);

}