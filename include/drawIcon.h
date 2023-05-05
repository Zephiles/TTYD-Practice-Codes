#ifndef DRAWICON_H
#define DRAWICON_H

#include "ttyd/icondrv.h"
#include "ttyd/item_data.h"

#include <cstdint>

#define ICON_SIZE_FLOAT 40.f
#define SPACE_USED_PER_ICON(scale) (50.f * (scale))

IconId getIconFromItem(ItemId item);

// Adjusts the scale of certain item icons to be closer to that of most items
float adjustItemIconScale(ItemId item, float scale);

const char *getItemName(ItemId item);

// If either the width or the height are not needed, then they can be set to nullptr
void getIconWidthHeight(IconId icon, float *widthOut, float *heightOut);

// Sets the alpha to 0xFF
void drawIcon(float posX, float posY, float scale, IconId icon);

void drawIcon(float posX, float posY, float scale, IconId icon, uint8_t alpha);

// This function is assumed to be used with 40x40 icons, such as inventory items
// Sets the alpha to 0xFF, and textColor alpha is overwritten by 0xFF
void drawIconWithText(float posX, float posY, float iconScale, float scale, IconId icon, const char *text, uint32_t textColor);

// This function is assumed to be used with 40x40 icons, such as inventory items
// textColor alpha is overwritten by the alpha parameter
void drawIconWithText(float posX,
                      float posY,
                      float iconScale,
                      float scale,
                      float width,
                      IconId icon,
                      const char *text,
                      uint32_t textColor,
                      uint8_t alpha);

// textColor alpha is overwritten by the alpha parameter
inline void drawItemIconWithText(float posX,
                                 float posY,
                                 float iconScale,
                                 float scale,
                                 float width,
                                 ItemId item,
                                 uint32_t textColor,
                                 uint8_t alpha)
{
    IconId iconId = getIconFromItem(item);
    const char *text = getItemName(item);
    drawIconWithText(posX, posY, iconScale, scale, width, iconId, text, textColor, alpha);
}

// Sets the alpha to 0xFF, and textColor alpha is overwritten by 0xFF
inline void
    drawItemIconWithText(float posX, float posY, float iconScale, float scale, float width, ItemId item, uint32_t textColor)
{
    drawItemIconWithText(posX, posY, iconScale, scale, width, item, textColor, 0xFF);
}

// Sets iconScale to scale, alpha to 0xFF, and textColor alpha is overwritten by 0xFF
inline void drawItemIconWithText(float posX, float posY, float scale, float width, ItemId item, uint32_t textColor)
{
    drawItemIconWithText(posX, posY, scale, scale, width, item, textColor, 0xFF);
}

// Sets iconScale to scale, and textColor alpha is overwritten by the alpha parameter
inline void
    drawItemIconWithText(float posX, float posY, float scale, float width, ItemId item, uint32_t textColor, uint8_t alpha)
{
    drawItemIconWithText(posX, posY, scale, scale, width, item, textColor, alpha);
}

#endif
