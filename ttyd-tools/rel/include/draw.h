#pragma once

#include "global.h"

#include <cstdint>

namespace mod {

void drawFunctionOnDebugLayer(void (*func)());
void drawFunctionOnDebugLayerWithOrder(void (*func)(), float order);
void drawFunctionOn2DLayer(void (*func)());
void drawFunctionOn2DLayerWithOrder(void (*func)(), float order);

void drawMenuWindow();

void drawWindow(uint32_t color, int32_t x, int32_t y, int32_t width, int32_t height, int32_t curve);

void getTextDimensions(const char *text, float scale, float *widthOut, float *heightOut);

void drawTextWithWindow(const char *text, int32_t textPosX, int32_t textPosY, 
    uint32_t textColor, float textScale, uint32_t windowColor, int32_t windowCurve);

int32_t *drawIcon(int32_t position[3], int16_t iconNum, float scale);
int32_t *drawIconFromItem(int32_t position[3], int16_t itemNum, float scale);

void drawTextMain(const char *text, int32_t x, int32_t y, uint32_t color, 
    const char *alignBaseString, float scale, float width);

void drawText(const char *text, int32_t x, int32_t y, uint32_t color, float scale);

void drawTextWidth(const char *text, int32_t x, 
    int32_t y, uint32_t color, float scale, float width);

void drawTextAlignRight(const char *text, int32_t x, int32_t y, 
    uint32_t color, const char *alignBaseString, float scale);

void drawTextMultipleLinesMain(const char *text, int32_t x, int32_t y, 
    uint32_t color, const char *alignBaseString, float scale, float width);

void drawTextMultipleLines(const char *text, int32_t x, int32_t y, uint32_t color, float scale);

void drawTextMultipleLinesWidth(const char *text, int32_t x, 
    int32_t y, uint32_t color, float scale, float width);

void drawTextInit(uint8_t alpha, bool drawFontEdge);

void drawTextAndInit(const char *text, int32_t x, int32_t y, 
    uint8_t alpha, uint32_t color, bool drawDontEdge, float scale);

void drawTextMultipleLinesAndInit(const char *text, int32_t x, int32_t y, 
    uint8_t alpha, uint32_t color, bool drawDontEdge, float scale);

void getOnOffTextAndColor(bool valueToCheck, const char **textOut, uint32_t *colorOut);
void getYesNoTextAndColor(bool valueToCheck, const char **textOut, uint32_t *colorOut);
uint32_t getSelectedTextColor(bool valueToCheck);

void getMapAndBeroTextAndColor(const char *mapText, 
    const char *beroText, MapAndBeroDetails *mapAndBeroDetails);

void drawSingleColumnMain();
void drawSingleColumnSelectedOption();

void drawSingleColumn(int32_t posY, uint32_t maxOptionsPerPage, 
    uint32_t currentPage, bool adjustSelectedOption);

void drawMultipleColumnsVertical(int32_t posX, int32_t posY, uint32_t currentMenuOption, 
    uint32_t currentPage, uint32_t size, uint32_t maxOptionsPerPage, 
        uint32_t maxOptionsPerRow, bool showCurrentOption, 
            uint32_t posXIncrementAmount, const char **lines);

void drawInventoryIconAndTextColumns();

void drawItemIconsColumn(uint32_t indexStart, uint32_t indexIncrement, uint32_t size, 
    int32_t posX, int32_t posY, uint32_t address, float scale);

void drawItemTextColumn(uint32_t indexStart, uint32_t indexIncrement, 
    uint32_t size, int32_t posX, int32_t posY, uint32_t address, float scale);

void drawMarioSpecialMovesOptions();
void drawFollowersOptions();
void drawMarioStats();
void drawPartnerStats();
void drawPartnerChangeYoshiColorOptions();
void drawBattlesActorStats();
void drawBattlesActorsHeldItem();
void drawCurrentFollowerOut();

void drawMemoryWatchValueString(int32_t slot, int32_t posX, 
    int32_t posY, uint32_t color, bool drawFontEdge, float scale);

void drawMemoryWatches();
void drawMemoryModifyList();
void drawMemoryTypeList();
void drawMemoryChangeWatchPosition();
void drawMemoryChangeAddressList();
uint16_t *drawMemoryEditorSetup();
void drawMemoryEditorSettingsWindow();
void drawMemoryEditorMainWindow();
void drawMemoryEditor();
void drawBattlesActorsList();
void drawBattlesStatusesList();

void drawErrorWindow(const char *message, int32_t textPosX, int32_t textPosY);
void drawErrorWindowAutoCheckForClose(const char *message, int32_t textPosX, int32_t textPosY);
void drawResolveFadesMessage();
void drawWarpsErrorMessage(int32_t textPosY);
void drawPartnerFollowerMessage(int32_t textPosY, bool drawForPartner);
void drawNotInBattleErrorMessage();
void drawMarioCoordinatesErrorMessage();
void drawConfirmationWindow(const char *message);

void drawSingleLineFromArray(int32_t posX, int32_t posY, 
    int32_t currentMenuOption, const char **line);

void drawSingleLineFromStringAndValue(int32_t posX, int32_t posY, 
    const char *line, int32_t value);

void drawSingleLineFromStringAndArray(int32_t posX, int32_t posY, 
    int32_t currentMenuOption, const char *string, const char **line);

void drawAdjustableValueSpawnItem();
void drawAdjustableValueMemoryEditor();
void drawAdjustableValue(bool changingItem, uint32_t currentMenu);
void drawAdjustableValueHex(uint32_t currentMenu);
void drawAdjustableValueDouble(uint32_t currentMenu, const char *value);
void drawAddByIconMain(uint32_t currentMenu);
void drawAddByIcon(uint32_t currentMenu);
void drawAddById(uint32_t currentMenu);

void drawVersionNumber(int32_t posX, int32_t posY);
void drawPageNumberMain(int32_t posX, int32_t posY, uint32_t currentPage, const char *alignBase);
void drawPageNumberAlignLeft(int32_t posX, int32_t posY, uint32_t currentPage);
void drawPageNumber(int32_t posX, int32_t posY, uint32_t currentPage);
void drawBoolOnOrOff(bool tempBool, const char *currentLine, int32_t posY);

void drawCheatsChangeSequence();
void drawCheatsModifyMarioCoordinates();
void drawCheatsBool(int32_t posY);
void drawButtonCombo(uint32_t buttonCombo, int32_t posY, const char *description);
void drawChangeButtonCombo(uint16_t *currentButtonCombo);
void drawCheatsGenerateLagSpike(uint16_t currentButtonCombo);
void drawCheatsFrameAdvance();
void drawCheatsDisableCertainSounds();
void drawCheatsForcedDropItem();
void drawCheatsResolveFades();
void drawCheatsLockFlags();
void drawCheatsManageFlagsMain(uint32_t currentMenu);
void drawCheatsClearArea();
void drawDisplaysMemoryUsageMenu();
void drawWarpsOptions();
void drawEventDetails(int32_t posX, int32_t posY, int32_t index);
void drawWarpByEventMenuDetails();
void drawWarpsBossesOptions();
void drawWarpIndexMapAndEntrance();
void drawWarpIndexEntranceList();
void drawOnScreenTimerButtonCombos(uint16_t *buttonCombo);

void drawOnScreenTimer();
void drawFrameCounter();
void drawMarioCoordinatesInit();
void drawMarioCoordinates();
void drawMarioSpeedXZ();
void drawJumpStorageDetails();
void drawButtonInputs();
void drawStickAngleInit();
void drawStickAngle();
void drawMemoryWatchesOnOverworld();

void drawYoshiSkipDetails();
void drawPalaceSkipDetails();
void drawBridgeSkipDetails();
void drawBlimpTicketSkipDetails();
void drawActionCommandsTiming();
void drawEffsActive();
void drawEvtsActive();
void drawEnemyEncounterNotifier();
void drawSettingsMemoryCardUsed();
void drawSettingsCurrentWork();

void drawHeapArrayErrors();
void drawMemoryUsage();
void drawNpcNameToPtrError();
void drawTitleScreenInfo();
void drawFileSelectScreenInfo();

}