#include "misc/functionHooks.h"

#include "ttyd/seqdrv.h"

#include <cstdint>

void (*g_marioStMain_trampoline)();
void (*g_titleMain_trampoline)(SeqWork *seqWork);
void (*g_seq_loadMain_trampoline)(SeqWork *seqWork);
const char *(*g_msgSearch_trampoline)(const char *key);
int32_t (*g_winRootMain_trampoline)(void *pauseMenuPtr);
int32_t (*g_marioKeyOn_trampoline)();
bool (*g_OSLink_trampoline)(OSModuleInfo *module, void *bss);
bool (*g_battle_init_trampoline)();
