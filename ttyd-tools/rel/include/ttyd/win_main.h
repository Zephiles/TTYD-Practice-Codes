#pragma once

#include <cstdint>

namespace ttyd::win_main {

extern "C" {

// winLectureCheck
// winLectureOn
// winLectureKeyMask
// winIconSet
// winIconGrayInit
// winIconInit
// winTexSet_x2
// winTexInit_x2
// winTexSetRot
// winTexSet
// winTexInit
// winFontSetPitch
// winFontSetLabel
// winFontSetEdge
// winFontSetR
// winFontSet
// winFontInit
// winGhostDiaryChk
// cam_r
// party_color
// famicom_check
// itemUseFunc2
// itemUseFunc
// winDispKoopa
// winDisp
// winMain
void winOpenDisable();
void winOpenEnable();
// winCheck
// winReInit
// winInit
void *winGetPtr();

}

}