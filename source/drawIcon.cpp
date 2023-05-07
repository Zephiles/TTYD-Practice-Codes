#include "drawIcon.h"
#include "drawText.h"
#include "gc/types.h"
#include "ttyd/item_data.h"
#include "ttyd/icondrv.h"
#include "ttyd/msgdrv.h"
#include "misc/utils.h"

#include <cstdint>

IconId getIconFromItem(ItemId item)
{
    // Make sure the item is valid
    if (!itemIsValid(item))
    {
        return IconId::ICON_INVALID_ITEM_ZAP_TAP;
    }

    const ItemData *itemDb = itemDataTable;
    const ItemData *currentItemDb = &itemDb[static_cast<uint32_t>(item)];

    switch (item)
    {
        default:
        {
            return currentItemDb->iconId;
        }
        case ItemId::ITEM_WEDDING_RING:
        {
            // Use the larger icon; the smaller one is used by default
            return IconId::ICON_WEDDING_RING_LARGE;
        }
        case ItemId::ITEM_DIAMOND_STAR:
        {
            return IconId::ICON_DIAMOND_STAR;
        }
        case ItemId::ITEM_EMERALD_STAR:
        {
            return IconId::ICON_EMERALD_STAR;
        }
        case ItemId::ITEM_GOLD_STAR:
        {
            return IconId::ICON_GOLD_STAR;
        }
        case ItemId::ITEM_RUBY_STAR:
        {
            return IconId::ICON_RUBY_STAR;
        }
        case ItemId::ITEM_SAPPHIRE_STAR:
        {
            return IconId::ICON_SAPPHIRE_STAR;
        }
        case ItemId::ITEM_GARNET_STAR:
        {
            return IconId::ICON_GARNET_STAR;
        }
        case ItemId::ITEM_CRYSTAL_STAR:
        {
            return IconId::ICON_CRYSTAL_STAR;
        }
    }
}

float adjustItemIconScale(ItemId item, float scale)
{
    switch (item)
    {
        case ItemId::ITEM_PAPER_MODE_CURSE:
        case ItemId::ITEM_TUBE_MODE_CURSE:
        case ItemId::ITEM_PLANE_MODE_CURSE:
        case ItemId::ITEM_BOAT_MODE_CURSE:
        case ItemId::ITEM_NORMAL_BOOTS:
        case ItemId::ITEM_SUPER_BOOTS:
        case ItemId::ITEM_ULTRA_BOOTS:
        case ItemId::ITEM_NORMAL_HAMMER:
        case ItemId::ITEM_SUPER_HAMMER:
        case ItemId::ITEM_ULTRA_HAMMER:
        {
            return scale - 0.1f;
        }
        case ItemId::ITEM_MAGICAL_MAP_LARGE:
        {
            return scale - 0.2f;
        }
        default:
        {
            return scale;
        }
    }
}

// TODO: Move to different cpp file
const char *getItemName(ItemId item)
{
    const ItemData *itemDb = itemDataTable;

    // Make sure the item is valid
    if (!itemIsValid(item))
    {
        item = ItemId::ITEM_NONE;
    }

    return msgSearch(itemDb[static_cast<uint32_t>(item)].itemNameMsg);
}

void getIconWidthHeight(IconId icon, float *widthOut, float *heightOut)
{
    // Make sure the icon is valid
    if (!iconIsValid(icon))
    {
        if (widthOut)
        {
            *widthOut = ICON_SIZE_FLOAT;
        }

        if (heightOut)
        {
            *heightOut = ICON_SIZE_FLOAT;
        }

        return;
    }

    uint16_t width;
    uint16_t height;
    iconGetWidthHight(&width, &height, icon);

    if (widthOut)
    {
        *widthOut = intToFloat(static_cast<int32_t>(width));
    }

    if (heightOut)
    {
        *heightOut = intToFloat(static_cast<int32_t>(height));
    }
}

void drawIcon(float posX, float posY, float scale, IconId icon)
{
    drawIcon(posX, posY, scale, icon, 0xFF);
}

void drawIcon(float posX, float posY, float scale, IconId icon, uint8_t alpha)
{
    // Make sure the icon is valid
    if (!iconIsValid(icon))
    {
        icon = IconId::ICON_INVALID_ITEM_ZAP_TAP;
    }

    Vec3 pos;
    pos.x = posX;
    pos.y = posY;
    pos.z = 0.f;

    iconDispGxAlpha(&pos, 24, icon, alpha, scale);
}

void drawIconWithText(float posX, float posY, float iconScale, float scale, IconId icon, const char *text, uint32_t textColor)
{
    drawIconWithText(posX, posY, iconScale, scale, 0.f, icon, text, textColor, 0xFF);
}

void drawIconWithText(float posX,
                      float posY,
                      float iconScale,
                      float scale,
                      float width,
                      IconId icon,
                      const char *text,
                      uint32_t textColor)
{
    drawIconWithText(posX, posY, iconScale, scale, width, icon, text, textColor, 0xFF);
}

void drawIconWithText(float posX,
                      float posY,
                      float iconScale,
                      float scale,
                      float width,
                      IconId icon,
                      const char *text,
                      uint32_t textColor,
                      uint8_t alpha)
{
    // Based on icon position
    // Draw the icon
    drawIcon(posX, posY, iconScale, icon, alpha);

    // Initialize text drawing
    drawTextInit(alpha, false);

    // Icons are rotated on the Y axis by default, so the text will need to be moved down by the height of it
    const float iconAdjustment = ICON_SIZE_FLOAT * scale;

    // Align the text so that the icon is roughly centered with it
    const float textPosY = posY + iconAdjustment - (7.f * scale);

    // Draw the text
    const float textPosX = posX + iconAdjustment - (15.f * scale);
    drawText(text, textPosX, textPosY, scale, width, (textColor & 0xFFFFFF00) | alpha, false);

    /*
    // Based on text position
    // Icons are rotated on the Y axis by default, so it will need to be moved down by the height of it
    const float iconHeightAdjustment = ICON_SIZE_FLOAT * scale;

    // Align the icon so that the text is roughly centered with it
    const float iconPosY = posY - iconHeightAdjustment + (7.f * scale);

    // Draw the icon
    drawIcon(posX, iconPosY, iconScale, icon, alpha);

    // Initialize text drawing
    drawTextInit(alpha, false);

    // Draw the text
    const float textPosX = posX + (ICON_SIZE_FLOAT * scale) - (15.f * scale);
    drawText(text, textPosX, posY, scale, width, (textColor & 0xFFFFFF00) | alpha, false);
    */
}
