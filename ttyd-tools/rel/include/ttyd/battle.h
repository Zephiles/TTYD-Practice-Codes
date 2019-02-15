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
// _EquipItem
// BtlUnit_EquipItem
// BattleTransPartyIdToUnitKind
// BattleTransPartyId
// BattleChangeParty
// BattlePartyAnimeLoad
void *BattleGetPartnerPtr(void *battleAddressesPointer, void *battleMarioPointer);
// BattleGetPartyPtr
void *BattleGetMarioPtr(void *battleAddressesPointer);
// BattleGetSystemPtr
// BattleGetUnitPartsPtr
// BattleSetUnitPtr
void *BattleGetUnitPtr(void *battleAddressesPointer, uint32_t slot);
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

}

}
