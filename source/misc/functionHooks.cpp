#include "misc/functionHooks.h"

#include "ttyd/seqdrv.h"

#include <cstdint>

void (*marioStMain_trampoline)();
void (*titleMain_trampoline)(SeqWork *seqWork);
void (*seq_loadMain_trampoline)(SeqWork *seqWork);
const char *(*msgSearch_trampoline)(const char *key);
int32_t (*winRootMain_trampoline)(void *pauseMenuPtr);
int32_t (*marioKeyOn_trampoline)();
bool (*OSLink_trampoline)(OSModuleInfo *module, void *bss);
bool (*battle_init_trampoline)();
