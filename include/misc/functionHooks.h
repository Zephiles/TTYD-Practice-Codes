#ifndef MISC_FUNCTIONHOOKS_H
#define MISC_FUNCTIONHOOKS_H

#include "gc/OSModule.h"
#include "ttyd/seqdrv.h"
#include "ttyd/item_data.h"
#include "ttyd/npcdrv.h"
#include "ttyd/battle_unit.h"
#include "ttyd/battle_database_common.h"
#include "ttyd/camdrv.h"
#include "ttyd/hitdrv.h"
#include "ttyd/mapdata.h"
#include "ttyd/evtmgr.h"

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
extern void (*g_BattlePadManager_trampoline)();
extern bool (*g_pouchRemoveItemIndex_trampoline)(ItemId item, uint32_t index);
extern void (*g_DEMOPadRead_trampoline)();
extern void (*g_viPostCallback_trampoline)(uint32_t retraceCount);
extern NpcEntry *(*g_fbatHitCheck_trampoline)(uint32_t flags, void *unk);
extern void (*g_seqSetSeq_trampoline)(SeqIndex seq, const char *map, const char *bero);
extern void (*g_statusWinDisp_trampoline)();
extern int32_t (*g_BattleActionCommandCheckDefence_trampoline)(BattleWorkUnit *battleUnitPtr, BattleWeapon *weapon);
extern void (*g_scissor_disp_control_trampoline)(CameraId cameraId, void *user);
extern uint32_t (*g_scissor_timer_main_trampoline)();
extern void (*g_winMarioDisp_trampoline)(CameraId cameraId, void *winWorkPtr, int32_t index);
extern HitEntry *(*g_hitCheckVecFilter_trampoline)(HitCheckQuery *pQuery, PFN_HitFilterFunction filterFunction);
extern NpcEntry *(*g_npcNameToPtr_trampoline)(const char *name);
extern void (*g_animPoseMain_trampoline)(int32_t poseId);
extern MapData *(*g_mapDataPtr_trampoline)(const char *mapName);
extern void (*g__unload_trampoline)(const char *currentMap, const char *nextMap, const char *nextBero);
extern void (*g_relSetEvtAddr_trampoline)(const char *mapName, const void *pInitEvtCode);
extern int32_t (*g_evt_bero_get_info_trampoline)(EvtEntry *evtPtr, bool isFirstCall);
extern int32_t (*g_SoundEfxPlayEx_trampoline)(int32_t soundId, uint8_t a2, uint8_t vol, uint8_t pan);

extern bool (
    *g_psndBGMOn_f_d_trampoline)(int32_t flags, const char *bgmName, uint16_t wFadeTime, uint16_t wFadeTime2, bool unused);

extern bool (*g_psndENVOn_f_d_trampoline)(int32_t flags, const char *envName, int32_t wFadeTime, bool unused);

// Functions used for hooks
void runOncePerFrame();
void displayTitleScreenInfo(SeqWork *seqWork);
void displayFileSelectScreenInfo(SeqWork *seqWork);

#endif
