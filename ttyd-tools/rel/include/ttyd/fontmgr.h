#pragma once

#include <cstdint>

#include <gc/mtx.h>

namespace ttyd::fontmgr {

extern "C" {

void fontmgrInit();
void fontmgrTexSetup();
void FontDrawStart();
void FontDrawStart_alpha(uint8_t alpha);
void FontDrawEdge();
void FontDrawEdgeOff();
void FontDrawRainbowColor();
void FontDrawRainbowColorOff();
void FontDrawNoise();
void FontDrawNoiseOff();
void FontDrawColorIDX(uint32_t idx);
void FontDrawColor(uint8_t color[4]);
void FontDrawColor_();
uint32_t FontGetDrawColor();
void FontDrawScale(float scale);
void FontDrawScaleVec(gc::mtx::Vec3 *scale);
void FontDrawCode(float x, float y, uint16_t code);
void FontDrawCodeMtx(gc::mtx::mtx34 matrix, uint16_t code);
void FontDrawString(float x, float y, const char *text);
void FontDrawStringPitch(float x, float y, float pitch, const char *text);
void FontDrawStringVecPitch(gc::mtx::Vec3 *position, float pitch, const char *text);
void FontDrawStringMtx(gc::mtx::mtx34 matrix, const char *text);
void FontDrawStringCenterMtx(gc::mtx::mtx34 matrix, const char *text);
void FontDrawStringShake(float x, float y, const char *text);
void FontDrawMessage(int32_t x, int32_t y, const char *message);
void FontDrawMessageMtx(gc::mtx::mtx34 matrix, const char *message);
uint16_t FontGetMessageWidthLine(const char *message, uint16_t *outLines);
uint16_t FontGetMessageWidth(const char *message);

uint16_t hankakuSearch(uint8_t code);
uint16_t kanjiSearch(uint16_t code);
uint8_t kanjiGetWidth(uint16_t code);
uint32_t HSV2RGB(uint8_t hsva[4]);

void JUTFontSetup(uint32_t index);
uint16_t JUTFont_CodeToGlyph(uint16_t code);
void JUTFont_DrawStart(uint8_t color[4]);
uint8_t _JUTFont_DrawPos(uint16_t glyph, int16_t x, int16_t y);

}

}
