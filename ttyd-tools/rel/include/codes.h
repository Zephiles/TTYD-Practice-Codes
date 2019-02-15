#pragma once

#include <cstdint>

namespace mod {

void saveMarioAndPartnerPositions();
void loadMarioAndPartnerPositions();
void saveAnywhere();
void setTextStorage();
void setTimeStopTextStorage();
void speedUpMario();
void reloadRoomMain();
void reloadRoom();
void levitate();
void lockMarioHPToMax();
void bobberyEarly();
void spawnItem();

void displaySequenceInPauseMenu();
void displayOnScreenTimer();
void displayMarioCoordinatesBoolCheck();
void displayJumpStorageDetails();
void displayButtonInputs();
void displayYoshiSkipDetails();
void displayPalaceSkipDetails();

int32_t warpToMap(uint32_t value);

}