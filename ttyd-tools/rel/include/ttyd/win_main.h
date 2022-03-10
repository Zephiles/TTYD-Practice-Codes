#pragma once

#include <cstdint>
#include <cstdarg>

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

#ifdef TTYD_JP
void winFontSetR(float pos[3], float scale[3], uint8_t color[4], ...);
void winFontSet(float pos[3], float scale[3], uint8_t color[4], ...);
#else
void winFontSetR(float pos[3], float scale[3], uint8_t color[4], const char *format, ...);
void winFontSet(float pos[3], float scale[3], uint8_t color[4], const char *format, ...);
#endif

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