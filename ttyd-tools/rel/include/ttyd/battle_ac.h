#pragma once

#include <cstdint>

namespace ttyd::battle_ac {

extern "C" {

extern uint8_t GuardFrames[];
extern uint8_t SuperguardFrames[];

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
int32_t BattleActionCommandCheckDefence(void *battleUnitPtr, void *attackParams);
// BattleActionCommandStop
// BattleActionCommandStart
// BattleActionCommandSetup
// BattleActionCommandDeclareACResult
// BattleActionCommandResult
// BattleActionCommandManager
// BattleActionCommandManagerInit

}

}
