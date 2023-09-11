#include "gc/OSModule.h"
#include "misc/functionHooks.h"
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

void (*g_marioStMain_trampoline)() = nullptr;
void (*g_titleMain_trampoline)(SeqWork *seqWork) = nullptr;
void (*g_seq_loadMain_trampoline)(SeqWork *seqWork) = nullptr;
const char *(*g_msgSearch_trampoline)(const char *key) = nullptr;
int32_t (*g_winRootMain_trampoline)(void *pauseMenuPtr) = nullptr;
int32_t (*g_marioKeyOn_trampoline)() = nullptr;
bool (*g_OSLink_trampoline)(OSModuleInfo *module, void *bss) = nullptr;
bool (*g_battle_init_trampoline)() = nullptr;
void (*g_BattlePadManager_trampoline)() = nullptr;
bool (*g_pouchRemoveItemIndex_trampoline)(ItemId item, uint32_t index) = nullptr;
void (*g_DEMOPadRead_trampoline)() = nullptr;
void (*g_viPostCallback_trampoline)(uint32_t retraceCount) = nullptr;
NpcEntry *(*g_fbatHitCheck_trampoline)(uint32_t flags, void *unk) = nullptr;
void (*g_seqSetSeq_trampoline)(SeqIndex seq, const char *map, const char *bero) = nullptr;
void (*g_statusWinDisp_trampoline)() = nullptr;
int32_t (*g_BattleActionCommandCheckDefence_trampoline)(BattleWorkUnit *battleUnitPtr, BattleWeapon *weapon) = nullptr;
void (*g_scissor_disp_control_trampoline)(CameraId cameraId, void *user) = nullptr;
uint32_t (*g_scissor_timer_main_trampoline)() = nullptr;
void (*g_winMarioDisp_trampoline)(CameraId cameraId, void *winWorkPtr, int32_t index) = nullptr;
HitEntry *(*g_hitCheckVecFilter_trampoline)(HitCheckQuery *pQuery, PFN_HitFilterFunction filterFunction) = nullptr;
NpcEntry *(*g_npcNameToPtr_trampoline)(const char *name) = nullptr;
void (*g_animPoseMain_trampoline)(int32_t poseId) = nullptr;
MapData *(*g_mapDataPtr_trampoline)(const char *mapName) = nullptr;
void (*g__unload_trampoline)(const char *currentMap, const char *nextMap, const char *nextBero) = nullptr;
void (*g_relSetEvtAddr_trampoline)(const char *mapName, const void *pInitEvtCode);
int32_t (*g_evt_bero_get_info_trampoline)(EvtEntry *evtPtr, bool isFirstCall) = nullptr;

int32_t (*g_SoundEfxPlayEx_trampoline)(int32_t soundId, uint8_t a2, uint8_t vol, uint8_t pan) = nullptr;

bool (*g_psndBGMOn_f_d_trampoline)(int32_t flags, const char *bgmName, uint16_t wFadeTime, uint16_t wFadeTime2, bool unused) =
    nullptr;

bool (*g_psndENVOn_f_d_trampoline)(int32_t flags, const char *envName, int32_t wFadeTime, bool unused) = nullptr;
