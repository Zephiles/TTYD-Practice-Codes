#pragma once

#include <cstdint>

namespace mod {

void drawFunctionOnDebugLayer(void (*func)());
void drawFunctionOn2DLayer(void (*func)());

void drawMenuWindow();

void drawWindow(uint32_t color, int32_t x, int32_t y, int32_t width, 
	int32_t height, int32_t curve);

int32_t *drawIcon(int32_t position[3], int16_t iconNum, float scale);
int32_t *drawIconFromItem(int32_t position[3], int16_t itemNum, float scale);
void drawStringMultiline(float x, float y, const char *text);
void startDrawString(uint8_t alpha, uint32_t color, float scale);

void drawText(const char *text, int32_t x, int32_t y, uint8_t alpha, 
	uint32_t color, float scale);

void drawTextWithWindow(const char *text, int32_t textPosX, int32_t textPosY, uint8_t alpha, 
	uint32_t textColor, float textScale, int32_t windowWidth, uint32_t windowColor, float windowCurve);

void drawSingleColumnMain();
void drawSingleColumn(uint32_t maxOptionsPerPage, uint32_t currentPage);

void drawMultipleColumnsVertical(int32_t posX, int32_t posY, uint32_t currentMenuOption, 
	uint32_t currentPage, uint32_t size, uint32_t maxOptionsPerPage, 
		uint32_t maxOptionsPerRow, bool showCurrentOption, 
			uint32_t posXIncrementAmount, const char **lines);

void drawInventoryIconAndTextColumns();

void drawItemIconsColumn(uint32_t indexStart, uint32_t indexIncrement, uint32_t size, 
	int32_t posX, int32_t posY, uint32_t address, float scale);

void drawItemTextColumn(uint32_t indexStart, uint32_t indexIncrement, uint32_t size, 
	int32_t posX, int32_t posY, uint32_t address, uint8_t alpha, float scale);

void drawMarioSpecialMovesOptions();
void drawFollowersOptions();
void drawMarioStats();
void drawPartnerStats();
void drawBattlesActorStats();
void drawBattlesActorsHeldItem();
void drawCurrentFollowerOut();
void drawMemoryWatches();
void drawMemoryModifyList();
void drawMemoryTypeList();
void drawMemoryChangeWatchPosition();
void drawMemoryChangeAddressList();
void drawBattlesActorsList();
void drawBattlesStatusesList();

void drawErrorWindow(const char *text, int32_t textPosX, int32_t windowWidth);
void drawErrorMessage(const char *line);
void drawWarpsErrorMessage();
void drawPartnerFollowerMessage(const char *message);
void drawPartnerErrorMessage();
void drawFollowersErrorMessage();
void drawMemoryErrorMessage(const char *message);
void drawNotInBattleErrorMessage();
void drawConfirmationWindow(const char *message);

void drawSingleLineFromArray(int32_t posX, int32_t posY, 
	int32_t currentMenuOption, const char **line);

void drawSingleLineFromStringAndValue(int32_t posX, int32_t posY, 
	const char *line, int32_t value);

void drawSingleLineFromStringAndArray(int32_t posX, int32_t posY, 
	int32_t currentMenuOption, const char *string, const char **line);

void drawAdjustableValueSpawnItem();
void drawAdjustableValue(bool changingItem, uint32_t currentMenu);
void drawMemoryWatchAdjustableValue(uint32_t currentMenu);
void drawAddByIconMain(uint32_t currentMenu);
void drawAddByIcon(uint32_t currentMenu);
void drawAddById(uint32_t currentMenu);

void drawVersionNumber(int32_t posX, int32_t posY);
void drawPageNumber(int32_t posX, int32_t posY, uint32_t currentPage);
void drawBoolOnOrOff(bool tempBool, const char *currentLine, int32_t posY);

#ifdef TTYD_JP
bool getSequenceStageAndEvent(const char **returnArray, char *stageNameBuffer, uint32_t sequencePosition);
#else
bool getSequenceStageAndEvent(const char **returnArray, uint32_t sequencePosition);
#endif

void drawCheatsChangeSequence();
void drawCheatsBool(int32_t posY);
void drawButtonCombo(uint16_t buttonCombo, int32_t posY, const char *description);
void drawChangeButtonCombo(uint16_t &currentButtonCombo);
void drawCheatsForcedDropItem();
void drawCheatsManageFlagsMain(uint32_t currentMenu);
void drawCheatsClearArea();
void drawWarpsOptions();
void drawOnScreenTimerButtonCombos();

void drawOnScreenTimer();
void drawSequenceInPauseMenu();
void drawMarioCoordinates();
void drawJumpStorageDetails();
void drawButtonInputs();
void drawStickAngle();
void drawMemoryWatchesOnOverworld();

void drawYoshiSkipDetails();
void drawPalaceSkipDetails();
void drawActionCommandsTiming();
void drawSettingsCurrentWork();

void drawTitleScreenInfo();
void drawFileSelectScreenInfo();

}