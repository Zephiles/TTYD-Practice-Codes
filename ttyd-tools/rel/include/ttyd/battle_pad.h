#pragma once

#include <cstdint>

namespace ttyd::battle_pad {

extern "C" {

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

}
