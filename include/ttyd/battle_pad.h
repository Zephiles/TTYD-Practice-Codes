#ifndef TTYD_BATTLE_PAD_H
#define TTYD_BATTLE_PAD_H

#include <cstdint>

extern "C"
{
    // BattlePadMultiCheckRepeat
    // BattlePadCheckRepeat
    // BattlePadMultiCheckUp
    // BattlePadCheckUp
    // BattlePadMultiCheckNow
    // BattlePadCheckNow
    // BattlePadMultiCheckRecordTrigger
    // BattlePadCheckRecordTrigger
    // BattlePadMultiCheckTrigger
    // BattlePadCheckTrigger
    // BattlePadGetNow
    // BattlePadGetTrigger
    // BtlPad_WorkUpdate
    // BtlPad_WorkInit
    void BattlePadManager();
    // BattlePadInit
}

#endif
