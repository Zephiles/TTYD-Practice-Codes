#pragma once

#include <cstdint>

namespace ttyd::battle {

extern "C" {

// BattleConsumeReserveItem
// BattleStatusWindowCheck
// BattleStatusWindowSystemOff
// BattleStatusWindowEventOff
// BattleStatusWindowSystemOn
// BattleStatusWindowEventOn
// BattleStatusWindowAPRecoveryOff
// BattleStatusWindowAPRecoveryOn
// BattleMajinaiEndCheck
// BattleMajinaiDone
// BattleMajinaiCheck
// battleDisableHResetCheck
// BattleAfterReactionMain
// BattleAfterReactionRelease
// BattleAfterReactionEntry
// BattleAfterReactionQueueInit
// BattleCheckUnitBroken
// BattleGetFloorHeight
// BattleGetStockExp
// BattleStoreExp
// BattleStoreCoin
// BattlePartyInfoWorkInit
void _EquipItem(void *battleUnitPtr, uint32_t unkBits, int16_t item);
// BtlUnit_EquipItem
// BattleTransPartyIdToUnitKind
// BattleTransPartyId
// BattleChangeParty
// BattlePartyAnimeLoad
void *BattleGetPartnerPtr(void *battleWorkPtr, void *battleMarioPointer);
// BattleGetPartyPtr
void *BattleGetMarioPtr(void *battleWorkPtr);
// BattleGetSystemPtr
// BattleGetUnitPartsPtr
// BattleSetUnitPtr
void *BattleGetUnitPtr(void *battleWorkPtr, uint32_t slot);
// BattleFree
// BattleAlloc
// BattleIncSeq
// BattleGetSeq
// BattleSetSeq
// BattleSetMarioParamToFieldBattle
// Btl_UnitSetup
// BattleEnd
// BattleMain
// BattleInit
// battleSeqEndCheck
// battleMain

extern void *battleWorkPointer;

}

}
