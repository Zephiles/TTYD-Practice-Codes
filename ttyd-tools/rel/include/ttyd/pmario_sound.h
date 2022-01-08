#pragma once

#include <cstdint>

namespace ttyd::pmario_sound {

extern "C" {

// psndENV_LPF
bool psndENVOff_f_d(int32_t flags, int32_t wFadeTime, bool unused);
bool psndENVOff(int32_t flags);
bool psndENVOn_f_d(int32_t flags, const char *envName, int32_t wFadeTime, bool unused);
// psndENVOn
// psndENVMain
// psndENV_stop
// psndSFX_get_vol
// psndSFXChk
// psndSFXOff
// psndSFX_dist
// psndSFX_pos
// psndSFX_pit
// psndSFX_vol
// psndSFXOnEx_3D
// psndSFXOn_3D
// psndSFXOnVol
// psndSFXOn
// psndSFXOn_
// __psndSFXOn
// psndSFXMain
// psndBGMPlayTime
// psndBGMStartCheck
// psndBGMScope
// psndBGMChkSilent
// psndBGMChk
bool psndBGMOff_f_d(int32_t flags, uint16_t fadeTime, bool unkSwitch);
bool psndBGMOff(int32_t flags);
bool psndBGMOn_f_d(int32_t flags, const char *bgmName, uint16_t wFadeTime, uint16_t wFadeTime2, bool unused);
// psndBGMOn
// psndBGMMain
// psndBGM_rate
// L_psndBGM_stop
// psndSFXAllOff
// psndMapChange
// psndGetFlag
void psndClearFlag(uint16_t unkBit);
// psndSetFlag
// psndSetReverb
// psndStopAllFadeOut
// psndSetPosDirListener
// psndPushGroup
void psndExit();
void psndMainInt();
// psndMain
// psndInit
// searchPSSFXList
// calc3D
// angleABf

}

}
