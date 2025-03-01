#ifndef TTYD_BATTLE_AC_H
#define TTYD_BATTLE_AC_H

#include "ttyd/battle.h"
#include "ttyd/battle_unit.h"
#include "ttyd/battle_database_common.h"

#include <cstdint>

extern "C"
{
    extern uint8_t _guardFrames[];
    extern uint8_t _superguardFrames[];

    // BattleAcGaugeSeDelete
    // BattleAcGaugeSeUpdate
    // BattleAcGaugeSeInit
    // BattleAcDrawGauge
    // BattleACGetButtonIcon
    // BattleActionCommandGetPrizeLv
    // BattleActionCommandSetDifficulty
    uint8_t BattleActionCommandGetDifficulty(BattleWork *battleWorkPtr);
    // BattleActionCommandResetDefenceResult
    // BattleActionCommandGetDefenceResult
    uint32_t BattleACPadCheckRecordTrigger(uint32_t frame, uint32_t button);
    int32_t BattleActionCommandCheckDefence(BattleWorkUnit *battleUnitPtr, BattleWeapon *weapon);
    // BattleActionCommandStop
    // BattleActionCommandStart
    // BattleActionCommandSetup
    // BattleActionCommandDeclareACResult
    // BattleActionCommandResult
    // BattleActionCommandManager
    // BattleActionCommandManagerInit
}

#endif
