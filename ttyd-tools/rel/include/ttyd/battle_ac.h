#pragma once

#include <ttyd/battle_unit.h>

#include <cstdint>

namespace ttyd::battle_ac {

extern "C" {

// BattleAcGaugeSeDelete
// BattleAcGaugeSeUpdate
// BattleAcGaugeSeInit
// BattleAcDrawGauge
// BattleACGetButtonIcon
// BattleActionCommandGetPrizeLv
// BattleActionCommandSetDifficulty
uint8_t BattleActionCommandGetDifficulty(void *battleWorkPtr);
// BattleActionCommandResetDefenceResult
// BattleActionCommandGetDefenceResult
uint32_t BattleACPadCheckRecordTrigger(uint32_t frame, uint32_t button);
int32_t BattleActionCommandCheckDefence(void *battleUnitPtr, ttyd::battle_unit::AttackParams *attackParams);
// BattleActionCommandStop
// BattleActionCommandStart
// BattleActionCommandSetup
// BattleActionCommandDeclareACResult
// BattleActionCommandResult
// BattleActionCommandManager
// BattleActionCommandManagerInit

extern uint8_t GuardFrames[];
extern uint8_t SuperguardFrames[];

}

}
