#pragma once

#include <cstdint>

namespace mod {

void walkThroughMostObjects();
void saveMarioAndPartnerPositions();
void loadMarioAndPartnerPositions();
void saveAnywhere();
void setTextStorage();
void setTimeStopTextStorage();
void speedUpMario();
void checkIfSystemLevelShouldBeLowered();
void reloadRoomMain();
void reloadRoom();
void levitate();
void lockMarioHPToMax();
void bobberyEarly();
void spawnItem();
void checkIfAreaFlagsShouldBeCleared();
double getStickAngle();

void displaySequenceInPauseMenu();
void displayOnScreenTimer();
void displayMarioCoordinatesBoolCheck();
void displayJumpStorageDetails();
void displayButtonInputs();
void displayStickAngle();
void displayMemoryWatches();

void displayYoshiSkipDetails();
void displayPalaceSkipDetails();
void actionCommandsTimingsInit();
void displayActionCommandsTiming();

int32_t warpToMap(uint32_t value);

}