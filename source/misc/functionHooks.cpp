#include "gc/OSModule.h"
#include "misc/functionHooks.h"
#include "ttyd/seqdrv.h"
#include "ttyd/item_data.h"
#include "ttyd/npcdrv.h"

#include <cstdint>

void (*g_marioStMain_trampoline)();
void (*g_titleMain_trampoline)(SeqWork *seqWork);
void (*g_seq_loadMain_trampoline)(SeqWork *seqWork);
const char *(*g_msgSearch_trampoline)(const char *key);
int32_t (*g_winRootMain_trampoline)(void *pauseMenuPtr);
int32_t (*g_marioKeyOn_trampoline)();
bool (*g_OSLink_trampoline)(OSModuleInfo *module, void *bss);
bool (*g_battle_init_trampoline)();
void (*g_BattlePadManager_trampoline)();
bool (*g_pouchRemoveItemIndex_trampoline)(ItemId item, uint32_t index);
void (*g_DEMOPadRead_trampoline)();
void (*g_viPostCallback_trampoline)(uint32_t retraceCount);
NpcEntry *(*g_fbatHitCheck_trampoline)(uint32_t flags, void *unk);
void (*g_seqSetSeq_trampoline)(SeqIndex seq, const char *map, const char *bero);
int32_t (*g_SoundEfxPlayEx_trampoline)(int32_t soundId, uint8_t a2, uint8_t vol, uint8_t pan);
bool (*g_psndBGMOn_f_d_trampoline)(int32_t flags, const char *bgmName, uint16_t wFadeTime, uint16_t wFadeTime2, bool unused);
bool (*g_psndENVOn_f_d_trampoline)(int32_t flags, const char *envName, int32_t wFadeTime, bool unused);
