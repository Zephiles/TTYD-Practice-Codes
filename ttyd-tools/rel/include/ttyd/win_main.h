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

#ifdef TTYD_JP
void winFontSetPitch(float pos[3], float scale[3], uint8_t color[4], float pitch, const char *format, ...);
#endif

// label is passed into msgSearch, and the returned string is drawn
void winFontSetLabel(float pos[3], float scale[3], uint8_t color[4], const char *label);

#ifndef TTYD_JP
void winFontSetEdgeWidth(float pos[3], float scale[3], uint8_t color[4], float width, const char *format, ...);
#endif

void winFontSetEdge(float pos[3], float scale[3], uint8_t color[4], const char *format, ...);

#ifdef TTYD_EU
void winFontSetCenter(float pos[3], float scale[3], uint8_t color[4], float center, const char *format, ...);
#endif

void winFontSetR(float pos[3], float scale[3], uint8_t color[4], const char *format, ...);

#ifndef TTYD_JP
void winFontSetWidth(float pos[3], float scale[3], uint8_t color[4], float width, const char *format, ...);
#endif

void winFontSet(float pos[3], float scale[3], uint8_t color[4], const char *format, ...);

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