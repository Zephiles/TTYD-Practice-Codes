#pragma once

#include "global.h"

#include <ttyd/party.h>

#include <cstdint>

namespace mod {

void closeMenu();
void closeSecondaryMenu();
void enterNextMenu(uint32_t nextMenu, uint32_t currentMenuOption);
void enterPreviousMenu();
void resetMenu();
void resetMenuToRoot();
void resetAndCloseSecondaryMenu();
void resetImportantItemsPauseMenu();
void resetPartnerPauseMenu();
void recheckUpgradesBattles(int32_t item);

void raiseSystemLevel();
void lowerSystemLevel();

void partnerMenuRemoveOrBringOut(void *partnerEnabledAddress);
void lockFlagsMenuBackUpFlags(uint32_t index);
void lockFlagsMenuSetNewArea(uint32_t index);
char *lockFlagsMenuGetAreaLockedString(uint32_t index);
const char *getItemName(int16_t item);
void *getFreeSlotPointer();
int32_t getTotalItems();
void getUpperAndLowerBounds(int32_t arrayOut[2], uint32_t currentMenu);
uint32_t *getPouchAddressAndSize(uint32_t tempArray[2]);
bool checkForItemsOnNextPage(uint32_t currentPage);
bool checkErrorMessageReqs();
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
uint32_t partnerChangeYoshiColorButtonControls();
uint32_t followersOptionsButtonControls();

void adjustMenuItemBoundsMain(int32_t valueChangedBy, int32_t lowerBound, int32_t upperBound);
void adjustMenuItemBounds(int32_t valueChangedBy, uint32_t currentMenu);
void adjustAddByIdValue(int32_t value, uint32_t currentMenu);

uint32_t getMarioStatsValueOffset(uint32_t currentMenuOption);
void setMarioStatsValue(uint32_t currentMenuOption);
void setPartnerStatsValue(uint32_t currentMenuOption);
void setBattlesActorValue(uint32_t currentMenuOption);
void setBattlesActorStatusValue(uint32_t currentMenuOption);
ttyd::party::PartyMembers getSelectedOptionPartnerValue();
void *getPartnerEnabledAddress();
bool checkIfPartnerOutSelected();

void setAddByIdValue(void *address);
void setAddByIconValue(void *address);
void duplicateCurrentItem(void *address);
void deleteItem();
int32_t changeItem();

int32_t resolveFade(uint32_t index);
int32_t getGW(uint32_t gw);
void setGW(uint32_t gw, uint32_t value);
bool getGF(uint32_t gf);
void setGF(uint32_t gf);
int32_t getGlobalFlagValue(uint32_t currentMenu, uint32_t flag);
void cheatClearAreaFlags(uint32_t currentMenuOption);
const char *getMapFromIndex(int32_t index);
int32_t getMapIndex();

const char *getPartyName(ttyd::party::PartyMembers id);
int32_t getTotalStageEvents();
bool indexToStageAndEvent(int32_t index, int32_t arrayOut[2]);
bool checkForValidStageAndEvent(int32_t stageId, int32_t eventId);
uint16_t getGsw0ForEvent(int32_t stageId, int32_t eventId);
bool getEventDetails(int32_t index, WarpByEventDetailsStruct *warpByEventDetails);
void *initStageEvents();

#ifdef TTYD_JP
bool getStageString(char *stageNameBuffer, uint32_t sequencePosition);
#endif

#ifdef TTYD_JP
bool getSequenceStageAndEvent(const char *arrayOut[2], char *stageNameBuffer, uint32_t sequencePosition);
#else
bool getSequenceStageAndEvent(const char *arrayOut[2], uint32_t sequencePosition);
#endif

double getStickAngle(int32_t stickXYOut[2]); // If the X and Y values are not wanted, then pass in nullptr
void getStickAngleString(char *stringOut);
char *getTimeString(char *stringOut, int64_t time);
void updateOnScreenTimerVars();

// void getButtonsPressedDynamic(uint8_t *buttonArrayOut, uint32_t currentButtonCombo);
void getButtonsPressed(uint8_t *buttonArrayOut, uint32_t currentButtonCombo);
void createButtonStringArray(uint8_t *buttonArray, char *stringOut, uint32_t stringOutSize);
bool incrementCheatsBButtonCounter(uint32_t buttonInput);
bool cheatsManageTimer(uint32_t buttonInput);

bool checkForDPADInput();
uint32_t checkButtonSingleFrame();
// void correctPageSingleColumn(uint32_t button, uint8_t &currentPage)
void default_DPAD_Actions(uint32_t button);
void adjustMenuNoPageEdit(uint32_t button);
void adjustCheatsResolveFadesSelection(uint32_t button);
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