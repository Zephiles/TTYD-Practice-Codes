#pragma once

#include <cstdint>

namespace ttyd::statuswindow {

extern "C" {

// statusPartyHPBlink
// statusMarioHPBlink
// statusFPBlink
// N_statusClearBlink
// statusAPBlink
// statusGetApPos
// statusWinForceUpdateCoin
// statusWinForceUpdate
// statusWinCheckUpdate
// statusWinCheck
// statusWinDispOff
// statusWinDispOn
// statusWinForceOff
// statusWinForceCloseClear
// statusWinForceClose
// statusWinForceOpen
// statusWinClose
// statusWinOpen
// valueUpdate
// valueCheck
// statusGetValue
void statusWinDisp();
// gaugeDisp
// statusWinMain
// statusWinReInit
// statusWinInit

extern void *statusWindowWorkPointer;

}

}