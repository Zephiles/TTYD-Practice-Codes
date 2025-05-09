#ifndef TTYD_WIN_MAIN_H
#define TTYD_WIN_MAIN_H

#include "gc/mtx.h"

#include <cstdint>
#include <cstdarg>

extern "C"
{
    extern const char str_999_jpn_winMain[];

#ifndef TTYD_JP
    extern const char str_999_winMain[];
#endif

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
    void winFontSetPitch(Vec3 *pos, Vec3 *scale, uint8_t color[4], float pitch, const char *format, ...);
#endif

    // label is passed into msgSearch, and the returned string is drawn
    void winFontSetLabel(Vec3 *pos, Vec3 *scale, uint8_t color[4], const char *label);

#ifndef TTYD_JP
    void winFontSetEdgeWidth(Vec3 *pos, Vec3 *scale, uint8_t color[4], float width, const char *format, ...);
#endif

    void winFontSetEdge(Vec3 *pos, Vec3 *scale, uint8_t color[4], const char *format, ...);

#ifdef TTYD_EU
    void winFontSetCenter(Vec3 *pos, Vec3 *scale, uint8_t color[4], float center, const char *format, ...);
#endif

    void winFontSetR(Vec3 *pos, Vec3 *scale, uint8_t color[4], const char *format, ...);

#ifndef TTYD_JP
    void winFontSetWidth(Vec3 *pos, Vec3 *scale, uint8_t color[4], float width, const char *format, ...);
#endif

    void winFontSet(Vec3 *pos, Vec3 *scale, uint8_t color[4], const char *format, ...);

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

#endif
