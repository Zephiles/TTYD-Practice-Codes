#include "drawIcon.h"
#include "drawText.h"
#include "classes/window.h"
#include "menus/stats.h"
#include "menus/root.h"
#include "ttyd/icondrv.h"
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
    selectedOptionMenuMarioChangeValue,

    "HP",
    selectedOptionMenuMarioChangeValue,

    "FP",
    selectedOptionMenuMarioChangeValue,

    "BP",
    selectedOptionMenuMarioChangeValue,

    "Max HP",
    selectedOptionMenuMarioChangeValue,

    "Max FP",
    selectedOptionMenuMarioChangeValue,

    "Level",
    selectedOptionMenuMarioChangeValue,

    "Rank",
    selectedOptionMenuMarioChangeValue,

    "Star Points",
    selectedOptionMenuMarioChangeValue,

    "Star Pieces",
    selectedOptionMenuMarioChangeValue,

    "Shine Sprites",
    selectedOptionMenuMarioChangeValue,

    "Special Moves",
    selectedOptionMenuMarioSpecialMoves,

    "Star Power",
    selectedOptionMenuMarioChangeValue,

    "Max Star Power",
    selectedOptionMenuMarioChangeValue,

    "Shop Points",
    selectedOptionMenuMarioChangeValue,

    "Piantas Stored",
    selectedOptionMenuMarioChangeValue,

    "Current Piantas",
    selectedOptionMenuMarioChangeValue,
};

const MenuFunctions statsMenuMarioFuncs = {
    statsMenuMarioControls,
    statsMenuMarioDraw,
    nullptr, // Exit function not needed
};

void statsMenuMarioInit(Menu *menuPtr)
{
    (void)menuPtr;

    // Reset currentIndex
    gStats->setCurrentIndex(0);

    constexpr uint32_t totalOptions = sizeof(statsMenuMarioOptions) / sizeof(statsMenuMarioOptions[0]);
    enterNextMenu(statsMenuMarioOptions, &statsMenuMarioFuncs, totalOptions);
}

void statsMenuMarioControls(Menu *menuPtr, MenuButtonInput button)
{
    Stats *statsPtr = gStats;

    // If the value editor is open, then handle the controls for that
    if (menuPtr->flagIsSet(StatsFlagMario::STATS_FLAG_MARIO_CURRENTLY_SELECTING_ID))
    {
        statsPtr->getValueEditor()->controls(button);
        return;
    }

    // If the window for toggling a special move is open, then handle the controls for that
    else if (menuPtr->flagIsSet(StatsFlagMario::STATS_FLAG_MARIO_CURRENTLY_TOGGLING_SPECIAL_MOVES))
    {
        statsPtr->getSpecialMoveToggler()->controls(button);
        return;
    }

    switch (button)
    {
        case MenuButtonInput::DPAD_LEFT:
        case MenuButtonInput::DPAD_RIGHT:
        case MenuButtonInput::DPAD_DOWN:
        case MenuButtonInput::DPAD_UP:
        {
            menuControlsVertical(button,
                                 statsPtr->getCurrentIndexPtr(),
                                 nullptr,
                                 STATS_MARIO_TOTAL_ENTRIES,
                                 STATS_MARIO_ENTRIES_PER_COLUMN * STATS_MARIO_ENTRIES_PER_ROW,
                                 STATS_MARIO_ENTRIES_PER_ROW,
                                 true);
            break;
        }
        case MenuButtonInput::A:
        {
            // Currently using currentIndex from gStats, so need to update currentIndex in menuPtr
            menuPtr->setCurrentIndex(statsPtr->getCurrentIndex());

            menuPtr->runSelectedOptionFunc();
            break;
        }
        case MenuButtonInput::B:
        {
            enterPrevMenu();
            break;
        }
        default:
        {
            break;
        }
    }
}

int32_t getMarioStat(PouchData *pouchPtr, uint32_t index)
{
    uint32_t piantaParlorPtrRaw = reinterpret_cast<uint32_t>(yuugijouWorkPtr);
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
        case StatsMarioOptions::STATS_MARIO_PIANTAS_STORED:
        {
            return *reinterpret_cast<int32_t *>(piantaParlorPtrRaw + 0x4);
        }
        case StatsMarioOptions::STATS_MARIO_CURRENT_PIANTAS:
        {
            return *reinterpret_cast<int32_t *>(piantaParlorPtrRaw + 0x8);
        }
        default:
        {
            return 0;
        }
    }
}

int32_t getMarioStatMaxValue(PouchData *pouchPtr, uint32_t index)
{
    switch (index)
    {
        case StatsMarioOptions::STATS_MARIO_COINS:
        case StatsMarioOptions::STATS_MARIO_MAX_HP:
        case StatsMarioOptions::STATS_MARIO_MAX_FP:
        case StatsMarioOptions::STATS_MARIO_STAR_PIECES:
        case StatsMarioOptions::STATS_MARIO_SHINE_SPRITES:
        {
            return 999;
        }
        case StatsMarioOptions::STATS_MARIO_CURRENT_HP:
        {
            return pouchPtr->maxHp;
        }
        case StatsMarioOptions::STATS_MARIO_CURRENT_FP:
        {
            return pouchPtr->maxFp;
        }
        case StatsMarioOptions::STATS_MARIO_BP:
        case StatsMarioOptions::STATS_MARIO_LEVEL:
        case StatsMarioOptions::STATS_MARIO_STAR_POINTS:
        {
            return 99;
        }
        case StatsMarioOptions::STATS_MARIO_RANK:
        {
            return 3;
        }
        case StatsMarioOptions::STATS_MARIO_CURRENT_STAR_POWER:
        {
            return pouchPtr->maxSp;
        }
        case StatsMarioOptions::STATS_MARIO_MAX_STAR_POWER:
        {
            return 800;
        }
        case StatsMarioOptions::STATS_MARIO_SHOP_POINTS:
        {
            return 300;
        }
        case StatsMarioOptions::STATS_MARIO_PIANTAS_STORED:
        case StatsMarioOptions::STATS_MARIO_CURRENT_PIANTAS:
        {
            return 99999;
        }
        default:
        {
            return 0;
        }
    }
}

void cancelMenuMarioChangeValue()
{
    gStats->getValueEditor()->stopDrawing();
    gMenu->clearFlag(StatsFlagMario::STATS_FLAG_MARIO_CURRENTLY_SELECTING_ID);
}

void selectedOptionMenuMarioChangeValue(Menu *menuPtr)
{
    Stats *statsPtr = gStats;

    // Make sure currentIndex is valid
    const uint32_t index = statsPtr->getCurrentIndex();
    if (index >= STATS_MARIO_TOTAL_ENTRIES)
    {
        // Failsafe: Reset currentIndex to 0
        statsPtr->setCurrentIndex(0);
        return;
    }

    // Bring up the window for selecting an id
    menuPtr->setFlag(StatsFlagMario::STATS_FLAG_MARIO_CURRENTLY_SELECTING_ID);

    // Get the current value
    PouchData *pouchPtr = pouchGetPtr();
    int32_t currentValue = getMarioStat(pouchPtr, index);

    // Make sure the value isn't negative
    if (currentValue < 0)
    {
        currentValue = 0;
    }

    // Get the max value
    int32_t maxValue = getMarioStatMaxValue(pouchPtr, index);

    // Make sure the value isn't negative
    if (maxValue < 0)
    {
        maxValue = 0;
    }

    // Set the min and max values
    statsPtr->setMinValue(0);
    statsPtr->setMaxValue(maxValue);

    // Initialize the value editor
    ValueEditor *valueEditorPtr = statsPtr->getValueEditor();

    uint32_t flags = 0;
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Y_SET_MAX);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Z_SET_MIN);

    const int32_t *minValuePtr = statsPtr->getMinValuePtr();
    const int32_t *maxValuePtr = statsPtr->getMaxValuePtr();

    valueEditorPtr->init(&currentValue, minValuePtr, maxValuePtr, gRootWindow, flags, VariableType::s32, statsPtr->getScale());
    valueEditorPtr->startDrawing(nullptr, cancelMenuMarioChangeValue);
}

void cancelMenuMarioToggleSpecialMoves()
{
    gStats->getSpecialMoveToggler()->stopDrawing();
    gMenu->clearFlag(StatsFlagMario::STATS_FLAG_MARIO_CURRENTLY_TOGGLING_SPECIAL_MOVES);
}

void selectedOptionMenuMarioSpecialMoves(Menu *menuPtr)
{
    // Bring up the window for toggling special moves
    menuPtr->setFlag(StatsFlagMario::STATS_FLAG_MARIO_CURRENTLY_TOGGLING_SPECIAL_MOVES);

    // Initialize the special move toggler
    Stats *statsPtr = gStats;
    SpecialMoveToggler *specialMoveTogglerPtr = statsPtr->getSpecialMoveToggler();

    specialMoveTogglerPtr->init(gRootWindow, statsPtr->getScale());
    specialMoveTogglerPtr->startDrawing(cancelMenuMarioToggleSpecialMoves);
}

void Stats::drawMarioStats() const
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
            const float specialMovesPosXIncrement = 22.f * scale;
            const IconId *specialMoveIconsPtr = specialMoveIcons;
            const uint32_t starPowersObtained = pouchPtr->starPowersObtained;

            const float specialMovesScale = 0.37f;
            float specialMovesPosX = iconPosX + (273.f * scale);
            float specialMovesPosY = iconPosY + starPowerAdjustment;

            for (uint32_t j = 0; j < TOTAL_SPECIAL_MOVES; j++)
            {
                // Make sure the bit for the current special move is set
                if ((starPowersObtained >> j) & 1U)
                {
                    drawIcon(specialMovesPosX, specialMovesPosY, specialMovesScale, specialMoveIconsPtr[j]);
                }

                specialMovesPosX += specialMovesPosXIncrement;
            }
        }

        iconPosY -= lineDecrement;
    }

    // Draw the main text and value for each option
    // Initalize text drawing
    drawTextInit(false);

    float textPosXBase;
    float textPosYBase;
    getTextPosXYByIcon(iconPosXBase, iconPosYBase, scale, &textPosXBase, &textPosYBase);

    const MenuOption *options = statsMenuMarioOptions;
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

        // Draw the main text
        uint32_t color = getCurrentOptionColor(currentIndex == i, 0xFF);
        drawText(options[i].name, textPosX, textPosY, scale, color);

        // Draw the value
        if (i != StatsMarioOptions::STATS_MARIO_SPECIAL_MOVES)
        {
            // Get the value for the current option
            value = getMarioStat(pouchPtr, i);

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
    Stats *statsPtr = gStats;
    statsPtr->drawMarioStats();

    // Draw the value editor if applicable
    ValueEditor *valueEditorPtr = statsPtr->getValueEditor();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->draw();
    }

    // Draw the special move toggler if applicable
    SpecialMoveToggler *specialMoveTogglerPtr = statsPtr->getSpecialMoveToggler();
    if (specialMoveTogglerPtr->shouldDraw())
    {
        specialMoveTogglerPtr->draw();
    }
}
