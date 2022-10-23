#pragma once

#include <gc/mtx.h>

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
void winFontSetPitch(gc::mtx::Vec3 *pos, gc::mtx::Vec3 *scale, uint8_t color[4], float pitch, const char *format, ...);
#endif

// label is passed into msgSearch, and the returned string is drawn
void winFontSetLabel(gc::mtx::Vec3 *pos, gc::mtx::Vec3 *scale, uint8_t color[4], const char *label);

#ifndef TTYD_JP
void winFontSetEdgeWidth(gc::mtx::Vec3 *pos, gc::mtx::Vec3 *scale, uint8_t color[4], float width, const char *format, ...);
#endif

void winFontSetEdge(gc::mtx::Vec3 *pos, gc::mtx::Vec3 *scale, uint8_t color[4], const char *format, ...);

#ifdef TTYD_EU
void winFontSetCenter(gc::mtx::Vec3 *pos, gc::mtx::Vec3 *scale, uint8_t color[4], float center, const char *format, ...);
#endif

void winFontSetR(gc::mtx::Vec3 *pos, gc::mtx::Vec3 *scale, uint8_t color[4], const char *format, ...);

#ifndef TTYD_JP
void winFontSetWidth(gc::mtx::Vec3 *pos, gc::mtx::Vec3 *scale, uint8_t color[4], float width, const char *format, ...);
#endif

void winFontSet(gc::mtx::Vec3 *pos, gc::mtx::Vec3 *scale, uint8_t color[4], const char *format, ...);

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


extern const char str_999_jpn_winMain[];

#ifndef TTYD_JP
extern const char str_999_winMain[];
#endif

}

}