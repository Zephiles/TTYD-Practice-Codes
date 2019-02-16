#pragma once

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
int8_t BattleActionCommandGetDifficulty(void *battlePointer);
// BattleActionCommandResetDefenceResult
// BattleActionCommandGetDefenceResult
uint32_t BattleACPadCheckRecordTrigger(uint32_t frame, uint32_t button);
int32_t BattleActionCommandCheckDefence(void *battle_unit, void *attack_params);
// BattleActionCommandStop
// BattleActionCommandStart
// BattleActionCommandSetup
// BattleActionCommandDeclareACResult
// BattleActionCommandResult
// BattleActionCommandManager
// BattleActionCommandManagerInit

}

}
