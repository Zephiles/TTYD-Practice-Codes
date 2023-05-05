#ifndef DRAWTEXT_H
#define DRAWTEXT_H

#include "ttyd/fontmgr.h"

#include <cstdint>

#define LINE_HEIGHT_ADJUSTMENT_4(scale) (4.f * (scale))
#define LINE_HEIGHT_ADJUSTMENT_5(scale) (5.f * (scale))

#define LINE_HEIGHT 32
#define LINE_HEIGHT_FLOAT 32.f

#define COLOR_WHITE 0xFFFFFF00
#define COLOR_BLACK 0x00000000
#define COLOR_BLUE 0x5B59DE00
#define COLOR_GREEN 0x1BBE2300
#define COLOR_RED 0xFF141400

#define DISPLAY_BUFFER_SIZE 256
extern char *gDisplayBuffer;

inline uint32_t getColorWhite(uint8_t alpha)
{
    return COLOR_WHITE | alpha;
}

inline uint32_t getColorBlack(uint8_t alpha)
{
    return COLOR_BLACK | alpha;
}

inline uint32_t getColorBlue(uint8_t alpha)
{
    return COLOR_BLUE | alpha;
}

inline uint32_t getColorGreen(uint8_t alpha)
{
    return COLOR_GREEN | alpha;
}

inline uint32_t getColorRed(uint8_t alpha)
{
    return COLOR_RED | alpha;
}

inline void getOnOffTextAndColor(bool valueToCheck, const char **textOut, uint32_t *colorOut, uint8_t alpha)
{
    if (valueToCheck)
    {
        *textOut = "On";
        *colorOut = getColorGreen(alpha);
    }
    else
    {
        *textOut = "Off";
        *colorOut = getColorRed(alpha);
    }
}

inline void getYesNoTextAndColor(bool valueToCheck, const char **textOut, uint32_t *colorOut, uint8_t alpha)
{
    if (valueToCheck)
    {
        *textOut = "Yes";
        *colorOut = getColorGreen(alpha);
    }
    else
    {
        *textOut = "No";
        *colorOut = getColorRed(alpha);
    }
}

inline uint32_t getCurrentOptionColor(bool currentOption, uint8_t alpha)
{
    if (currentOption)
    {
        return getColorBlue(alpha);
    }
    else
    {
        return getColorWhite(alpha);
    }
}

// If either the width or the height are not needed, then they can be set to nullptr
void getTextWidthHeight(const char *text, float scale, float *widthOut, float *heightOut);

// Calls drawText with the width set to 0.f and alignRight set to false
void drawText(const char *text, float posX, float posY, float scale, uint32_t color);

// Calls drawText with the width set to 0.f
void drawText(const char *text, float posX, float posY, float scale, uint32_t color, bool alignRight);

// Calls drawText with alignRight set to false
void drawText(const char *text, float posX, float posY, float scale, float width, uint32_t color);

void drawText(const char *text, float posX, float posY, float scale, float width, uint32_t color, bool alignRight);

// Sets the alpha to 0xFF
inline void drawTextInit(bool drawFontEdge)
{
    // FontDrawStart sets the alpha to 0xFF
    FontDrawStart();

    // Check if the font edge should be on or off
    // FontDrawStart sets it to off by default
    if (drawFontEdge)
    {
        FontDrawEdge();
    }
}

inline void drawTextInit(uint8_t alpha, bool drawFontEdge)
{
    FontDrawStart_alpha(alpha);

    // Check if the font edge should be on or off
    // FontDrawStart_alpha sets it to off by default
    if (drawFontEdge)
    {
        FontDrawEdge();
    }
}

void drawTextMain(const char *text, float posX, float posY, float scale, float width, uint32_t color, bool alignRight);

#endif
