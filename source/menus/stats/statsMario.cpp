#include "drawIcon.h"
#include "drawText.h"
#include "classes/window.h"
#include "menus/stats.h"
#include "menus/root.h"
#include "ttyd/icondrv.h"
#include "ttyd/item_data.h"
#include "ttyd/mario_pouch.h"
#include "ttyd/evt_yuugijou.h"
#include "ttyd/dispdrv.h"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cinttypes>

const IconId statsMenuMarioTextIcons[] = {
    IconId::ICON_BATTLE_DROP_COIN,                      // Coins
    IconId::ICON_PAUSE_MENU_HEART,                      // HP
    IconId::ICON_PAUSE_MENU_FLOWER,                     // FP
    IconId::ICON_PAUSE_MENU_BADGE_POINTS,               // BP
    IconId::ICON_PAUSE_MENU_HEART,                      // Max HP
    IconId::ICON_PAUSE_MENU_FLOWER,                     // Max FP
    IconId::ICON_PAUSE_MENU_MARIO_HEAD,                 // Level
    IconId::ICON_PAUSE_MENU_MARIO_HEAD,                 // Rank
    IconId::ICON_BATTLE_STAR_POINTS_FROM_ENEMIES_COUNT, // Star Points
    IconId::ICON_STAR_PIECE,                            // Star Pieces
    IconId::ICON_SHINE_SPRITE,                          // Shine Sprites
    IconId::ICON_DIAMOND_STAR,                          // Special Moves
    IconId::ICON_ACTION_COMMAND_GREEN_CIRCLE,           // Star Power
    IconId::ICON_ACTION_COMMAND_GREEN_CIRCLE,           // Max Star Power
    IconId::ICON_KOOPA_CURSE,                           // Shop Points
    IconId::ICON_PIANTA,                                // Piantas Stored
    IconId::ICON_PIANTA,                                // Current Piantas
};

const MenuOption statsMenuMarioOptions[] = {
    "Coins",
    nullptr,

    "HP",
    nullptr,

    "FP",
    nullptr,

    "BP",
    nullptr,

    "Max HP",
    nullptr,

    "Max FP",
    nullptr,

    "Level",
    nullptr,

    "Rank",
    nullptr,

    "Star Points",
    nullptr,

    "Star Pieces",
    nullptr,

    "Shine Sprites",
    nullptr,

    "Special Moves",
    nullptr,

    "Star Power",
    nullptr,

    "Max Star Power",
    nullptr,

    "Shop Points",
    nullptr,

    "Piantas Stored",
    nullptr,

    "Current Piantas",
    nullptr,
};

const MenuFunctions statsMenuMarioFuncs = {
    controlsBasicMenuLayout,
    statsMenuMarioDraw,
    nullptr, // Exit function not needed
};

void statsMenuMarioInit(Menu *menuPtr)
{
    (void)menuPtr;

    constexpr uint32_t totalOptions = sizeof(statsMenuMarioOptions) / sizeof(statsMenuMarioOptions[0]);
    enterNextMenu(statsMenuMarioOptions, &statsMenuMarioFuncs, totalOptions);
}

int32_t getMarioStat(PouchData *pouchPtr, uint32_t index)
{
    switch (index)
    {
        case StatsMarioOptions::STATS_MARIO_COINS:
        {
            return pouchPtr->coins;
        }
        case StatsMarioOptions::STATS_MARIO_CURRENT_HP:
        {
            return pouchPtr->currentHp;
        }
        case StatsMarioOptions::STATS_MARIO_CURRENT_FP:
        {
            return pouchPtr->currentFp;
        }
        case StatsMarioOptions::STATS_MARIO_BP:
        {
            return pouchPtr->totalBp;
        }
        case StatsMarioOptions::STATS_MARIO_MAX_HP:
        {
            return pouchPtr->maxHp;
        }
        case StatsMarioOptions::STATS_MARIO_MAX_FP:
        {
            return pouchPtr->maxFp;
        }
        case StatsMarioOptions::STATS_MARIO_LEVEL:
        {
            return pouchPtr->level;
        }
        case StatsMarioOptions::STATS_MARIO_RANK:
        {
            return pouchPtr->rank;
        }
        case StatsMarioOptions::STATS_MARIO_STAR_POINTS:
        {
            return pouchPtr->starPoints;
        }
        case StatsMarioOptions::STATS_MARIO_STAR_PIECES:
        {
            return pouchPtr->starPieces;
        }
        case StatsMarioOptions::STATS_MARIO_SHINE_SPRITES:
        {
            return pouchPtr->shineSprites;
        }
        case StatsMarioOptions::STATS_MARIO_CURRENT_STAR_POWER:
        {
            return pouchPtr->currentSp;
        }
        case StatsMarioOptions::STATS_MARIO_MAX_STAR_POWER:
        {
            return pouchPtr->maxSp;
        }
        case StatsMarioOptions::STATS_MARIO_SHOP_POINTS:
        {
            return pouchPtr->shopPoints;
        }
        default:
        {
            return 0;
        }
    }
}

void Stats::drawMarioStats()
{
    // Draw the main text and icons
    const Window *rootWindowPtr = gRootWindow;
    const float scale = this->scale;

    float iconPosXBase;
    float iconPosYBase;
    rootWindowPtr->getIconPosXY(WindowAlignment::TOP_LEFT, scale, &iconPosXBase, &iconPosYBase);

    PouchData *pouchPtr = pouchGetPtr();
    const float iconPosXIncrement = 317.f * scale;
    const float starPowerAdjustment = 8.f * scale;
    const float lineDecrement = SPACE_USED_PER_ICON(scale);

    float iconPosX = iconPosXBase;
    float iconPosY = iconPosYBase;

    const uint32_t currentIndex = this->currentIndex;
    const IconId *icons = statsMenuMarioTextIcons;
    uint32_t counter = 0;

    // To avoid a lot of unnecessary extra GX calls, draw the icons first
    for (uint32_t i = 0; i < STATS_MARIO_TOTAL_ENTRIES; i++, counter++)
    {
        if (counter >= STATS_MARIO_ENTRIES_PER_COLUMN)
        {
            counter = 0;

            // Move to the next column
            iconPosX += iconPosXIncrement;
            iconPosY = iconPosYBase;
        }

        // The icons for star power need to be moved up slightly
        float tempPosY = iconPosY;
        if ((i == StatsMarioOptions::STATS_MARIO_CURRENT_STAR_POWER) || (i == StatsMarioOptions::STATS_MARIO_MAX_STAR_POWER))
        {
            tempPosY += starPowerAdjustment;
        }

        // Draw the icon
        drawIcon(iconPosX, tempPosY, scale, icons[i]);

        // Draw the icons for the special moves
        if (i == StatsMarioOptions::STATS_MARIO_SPECIAL_MOVES)
        {
            ItemId specialMoveItem;
            const float specialMovesPosXIncrement = 22.f * scale;
            const uint32_t starPowersObtained = pouchPtr->starPowersObtained;

            const float specialMovesScale = 0.37f;
            float specialMovesPosX = iconPosX + (273.f * scale);
            float specialMovesPosY = iconPosY + starPowerAdjustment;

            for (uint32_t i = 0; i < 8; i++)
            {
                // Make sure the bit for the current special move is set
                // The crystal star icons are not in the proper order, so get the icon from the item
                if ((starPowersObtained >> i) & 1U)
                {
                    if (i == 0)
                    {
                        // Set the icon for Sweet Treat
                        specialMoveItem = ItemId::ITEM_MAGICAL_MAP_SMALL;
                    }
                    else
                    {
                        specialMoveItem = static_cast<ItemId>(static_cast<int32_t>(ItemId::ITEM_DIAMOND_STAR) + (i - 1));
                    }

                    drawIcon(specialMovesPosX, specialMovesPosY, specialMovesScale, getIconFromItem(specialMoveItem));
                }

                specialMovesPosX += specialMovesPosXIncrement;
            }
        }

        iconPosY -= lineDecrement;
    }

    // Draw the main text and value for each option
    // Initalize text drawing
    drawTextInit(false);

    uint32_t piantaParlorPtrRaw = reinterpret_cast<uint32_t>(yuugijouWorkPtr);
    const MenuOption *options = statsMenuMarioOptions;

    const float iconAdjustment = ICON_SIZE_FLOAT * scale;
    const float textPosYBase = iconPosYBase + iconAdjustment - (7.f * scale);
    const float textPosXBase = iconPosXBase + iconAdjustment - (15.f * scale);

    float textPosX = textPosXBase;
    float textPosY = textPosYBase;

    const float valuePosXBase = textPosX + (253.f * scale);
    const float valuePosXIncrement = 417.f * scale;

    char *displayBufferPtr = gDisplayBuffer;
    float valuePosX = valuePosXBase;
    float maxWidth = 48.f;

    int32_t value;
    counter = 0;

    for (uint32_t i = 0; i < STATS_MARIO_TOTAL_ENTRIES; i++, counter++)
    {
        if (counter >= STATS_MARIO_ENTRIES_PER_COLUMN)
        {
            counter = 0;

            // Move to the next column
            textPosX += iconPosXIncrement;
            textPosY = textPosYBase;
            valuePosX += valuePosXIncrement;

            // The last column can have longer texts, so the max width must be higher
            maxWidth = 68.f;
        }

        // Get the value for the current option
        switch (i)
        {
            case StatsMarioOptions::STATS_MARIO_PIANTAS_STORED:
            {
                value = *reinterpret_cast<int32_t *>(piantaParlorPtrRaw + 0x4);
                break;
            }
            case StatsMarioOptions::STATS_MARIO_CURRENT_PIANTAS:
            {
                value = *reinterpret_cast<int32_t *>(piantaParlorPtrRaw + 0x8);
                break;
            }
            default:
            {
                value = getMarioStat(pouchPtr, i);
                break;
            }
        }

        // Draw the main text
        uint32_t color = getCurrentOptionColor(currentIndex == i, 0xFF);
        drawText(options[i].name, textPosX, textPosY, scale, color);

        // Draw the value
        if (i != StatsMarioOptions::STATS_MARIO_SPECIAL_MOVES)
        {
            snprintf(displayBufferPtr, DISPLAY_BUFFER_SIZE, "%" PRId32, value);
            drawTextMain(displayBufferPtr, valuePosX, textPosY, scale, maxWidth, getColorWhite(0xFF), true);
        }

        textPosY -= lineDecrement;
    }
}

void statsMenuMarioDraw(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Draw the main window
    gRootWindow->draw();

    // Draw Mario's stats
    gStats->drawMarioStats();
}
