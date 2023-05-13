#ifndef MISC_FUNCTIONHOOKS_H
#define MISC_FUNCTIONHOOKS_H

#include "gc/OSModule.h"
#include "ttyd/seqdrv.h"

#include <cstdint>

// Function hooks
extern void (*marioStMain_trampoline)();
extern void (*titleMain_trampoline)(SeqWork *seqWork);
extern void (*seq_loadMain_trampoline)(SeqWork *seqWork);
extern const char *(*msgSearch_trampoline)(const char *key);
extern int32_t (*winRootMain_trampoline)(void *pauseMenuPtr);
extern int32_t (*marioKeyOn_trampoline)();
extern bool (*OSLink_trampoline)(OSModuleInfo *module, void *bss);
extern bool (*battle_init_trampoline)();

// Functions used for hooks
void runOncePerFrame();
void displayTitleScreenInfo(SeqWork *seqWork);
void displayFileSelectScreenInfo(SeqWork *seqWork);
const char *getCustomMessage(const char *key);
int32_t pauseMenuPreventUnpause(void *pauseMenuPtr);
int32_t fixMarioKeyOn();
bool applyRelPatches(OSModuleInfo *module, void *bss);
bool performPreBattleActions();

#endif
