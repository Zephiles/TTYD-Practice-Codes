#ifndef MISC_FUNCTIONHOOKS_H
#define MISC_FUNCTIONHOOKS_H

#include "gc/OSModule.h"
#include "ttyd/seqdrv.h"

#include <cstdint>

// Function hooks
extern void (*g_marioStMain_trampoline)();
extern void (*g_titleMain_trampoline)(SeqWork *seqWork);
extern void (*g_seq_loadMain_trampoline)(SeqWork *seqWork);
extern const char *(*g_msgSearch_trampoline)(const char *key);
extern int32_t (*g_winRootMain_trampoline)(void *pauseMenuPtr);
extern int32_t (*g_marioKeyOn_trampoline)();
extern bool (*g_OSLink_trampoline)(OSModuleInfo *module, void *bss);
extern bool (*g_battle_init_trampoline)();

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
