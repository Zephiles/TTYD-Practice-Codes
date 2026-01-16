#ifndef GAMEPATCHES_H
#define GAMEPATCHES_H

#include "gc/OSModule.h"
#include "ttyd/npcdrv.h"

#include <cstdint>

const char *getCustomMessage(const char *key);
int32_t pauseMenuPreventUnpause(void *pauseMenuPtr);
int32_t fixMarioKeyOn();
bool applyRelPatches(OSModuleInfo *module, void *bss);
bool performPreBattleActions();
void performBattleChecks();
NpcEntry *fbatHitCheck_Work(uint32_t flags, void *unk);
void psndSFXOff_Work(int32_t flags);

void applyGameFixes();
void applyVariousGamePatches();
void applyCheatAndDisplayInjects();

#endif
