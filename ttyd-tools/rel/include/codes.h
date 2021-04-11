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
bool disableBattles();
void checkIfSystemLevelShouldBeLowered();
void reloadRoomMain();
void reloadRoom();
void levitate();
uint32_t autoMashText(uint32_t controllerPort);
void generateLagSpike();
void lockMarioHPToMax();
void bobberyEarly();
void spawnItem();
void checkIfAreaFlagsShouldBeCleared();
void lockFlags();

void displaySequenceInPauseMenu();
void displayOnScreenTimer();
void displayFrameCounter();
void displayMarioCoordinates();
void displayMarioSpeedXZ();
void displayJumpStorageDetails();
void displayButtonInputs();
void displayStickAngle();
void displayMemoryWatches();
void displayMemoryEditor();

void displayYoshiSkipDetails();
void displayPalaceSkipDetails();
void displayBridgeSkipDetails();
void displayBlimpTicketSkipDetails();
void displayActionCommandsTiming();
void displayEffsActive();
void displayEvtsActive();

int32_t warpToMap(uint32_t value);
int32_t warpToMapByEvent(int32_t index);
int32_t warpToBoss(uint32_t index);
int32_t warpToMapByString(const char *map);

}