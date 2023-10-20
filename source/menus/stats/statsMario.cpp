#include "mod.h"
#include "menuUtils.h"
#include "drawIcon.h"
#include "drawText.h"
#include "classes/window.h"
#include "menus/statsMenu.h"
#include "menus/rootMenu.h"
#include "misc/utils.h"
#include "ttyd/icondrv.h"
#include "ttyd/mario_pouch.h"
#include "ttyd/evt_yuugijou.h"
#include "ttyd/camdrv.h"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cinttypes>

static void controls(Menu *menuPtr, MenuButtonInput button);
static void draw(CameraId cameraId, void *user);
static void selectedOptionChangeValue(Menu *menuPtr);
static void selectedOptionToggleSpecialMoves(Menu *menuPtr);

static const IconId gOptionsIcons[] = {
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

static const MenuOption gOptions[] = {
    "Coins",
    selectedOptionChangeValue,

    "HP",
    selectedOptionChangeValue,

    "FP",
    selectedOptionChangeValue,

    "BP",
    selectedOptionChangeValue,

    "Max HP",
    selectedOptionChangeValue,

    "Max FP",
    selectedOptionChangeValue,

    "Level",
    selectedOptionChangeValue,

    "Rank",
    selectedOptionChangeValue,

    "Star Points",
    selectedOptionChangeValue,

    "Star Pieces",
    selectedOptionChangeValue,

    "Shine Sprites",
    selectedOptionChangeValue,

    "Special Moves",
    selectedOptionToggleSpecialMoves,

    "Star Power",
    selectedOptionChangeValue,

    "Max Star Power",
    selectedOptionChangeValue,

    "Shop Points",
    selectedOptionChangeValue,

    "Piantas Stored",
    selectedOptionChangeValue,

    "Current Piantas",
    selectedOptionChangeValue,
};

static const MenuFunctions gFuncs = {
    gOptions,
    controls,
    draw,
    nullptr, // Exit function not needed
};

void statsMenuMarioInit(Menu *menuPtr)
{
    (void)menuPtr;

    // Reset currentIndex
    gStatsMenu->setCurrentIndex(0);

    constexpr uint32_t totalOptions = sizeof(gOptions) / sizeof(MenuOption);
    enterNextMenu(&gFuncs, totalOptions);
}

static void dpadControls(MenuButtonInput button, uint8_t *currentIndexPtr)
{
    menuControlsVertical(button,
                         currentIndexPtr,
                         nullptr,
                         STATS_MARIO_TOTAL_ENTRIES,
                         STATS_MARIO_ENTRIES_PER_COLUMN * STATS_MARIO_ENTRIES_PER_ROW,
                         STATS_MARIO_ENTRIES_PER_ROW,
                         true);
}

static void controls(Menu *menuPtr, MenuButtonInput button)
{
    StatsMenu *statsMenuPtr = gStatsMenu;

    // If the value editor is open, then handle the controls for that
    ValueEditor *valueEditorPtr = statsMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->controls(button);
        return;
    }

    // If the window for toggling a special move is open, then handle the controls for that
    SpecialMoveToggler *specialMoveTogglerPtr = statsMenuPtr->getSpecialMoveTogglerPtr();
    if (specialMoveTogglerPtr->shouldDraw())
    {
        specialMoveTogglerPtr->controls(button);
        return;
    }

    // The function for checking for auto-incrementing needs to run every frame to be handled correctly
    const bool autoIncrement = handleMenuAutoIncrement(statsMenuPtr->getAutoIncrementPtr());

    // Handle held button inputs if auto-incrementing should be done
    if (autoIncrement)
    {
        const MenuButtonInput buttonHeld = getMenuButtonInput(false);
        switch (buttonHeld)
        {
            case MenuButtonInput::DPAD_LEFT:
            case MenuButtonInput::DPAD_RIGHT:
            case MenuButtonInput::DPAD_DOWN:
            case MenuButtonInput::DPAD_UP:
            {
                dpadControls(buttonHeld, statsMenuPtr->getCurrentIndexPtr());
                break;
            }
            default:
            {
                break;
            }
        }
    }

    // Handle the button inputs pressed this frame
    switch (button)
    {
        case MenuButtonInput::DPAD_LEFT:
        case MenuButtonInput::DPAD_RIGHT:
        case MenuButtonInput::DPAD_DOWN:
        case MenuButtonInput::DPAD_UP:
        {
            dpadControls(button, statsMenuPtr->getCurrentIndexPtr());
            break;
        }
        case MenuButtonInput::A:
        {
            // Currently using currentIndex from gStatsMenu, so need to update the current index in menuPtr
            menuPtr->setCurrentIndex(statsMenuPtr->getCurrentIndex());

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

static int32_t getStat(const PouchData *pouchPtr, uint32_t index)
{
    uint32_t piantaParlorPtrRaw = reinterpret_cast<uint32_t>(_yuugijouWorkPtr);
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

static void changeValue(const ValueType *valuePtr)
{
    StatsMenu *statsMenuPtr = gStatsMenu;

    // Make sure the current index is valid
    const uint32_t index = statsMenuPtr->getCurrentIndex();
    if (index >= STATS_MARIO_TOTAL_ENTRIES)
    {
        // Failsafe: Reset the current index to 0
        statsMenuPtr->setCurrentIndex(0);
        return;
    }

    const int32_t value = valuePtr->s32;
    PouchData *pouchPtr = pouchGetPtr();
    uint32_t piantaParlorPtrRaw = reinterpret_cast<uint32_t>(_yuugijouWorkPtr);

    // Set the new value
    switch (index)
    {
        case StatsMarioOptions::STATS_MARIO_COINS:
        {
            pouchPtr->coins = static_cast<int16_t>(value);
            break;
        }
        case StatsMarioOptions::STATS_MARIO_CURRENT_HP:
        {
            pouchPtr->currentHp = static_cast<int16_t>(value);
            break;
        }
        case StatsMarioOptions::STATS_MARIO_CURRENT_FP:
        {
            pouchPtr->currentFp = static_cast<int16_t>(value);
            break;
        }
        case StatsMarioOptions::STATS_MARIO_BP:
        {
            pouchPtr->totalBp = static_cast<int16_t>(value);

            // Force the game to recalculate how much BP is left
            pouchReviseMarioParam();
            break;
        }
        case StatsMarioOptions::STATS_MARIO_MAX_HP:
        {
            pouchPtr->maxHp = static_cast<int16_t>(value);

            // Reset the value for entering battles
            pouchPtr->baseMaxHp = static_cast<int16_t>(value);

            // Prevent the current value from exceeding the max value
            if (pouchPtr->currentHp > value)
            {
                pouchPtr->currentHp = static_cast<int16_t>(value);
            }
            break;
        }
        case StatsMarioOptions::STATS_MARIO_MAX_FP:
        {
            pouchPtr->maxFp = static_cast<int16_t>(value);

            // Reset the value for entering battles
            pouchPtr->baseMaxFp = static_cast<int16_t>(value);

            // Prevent the current value from exceeding the max value
            if (pouchPtr->currentFp > value)
            {
                pouchPtr->currentFp = static_cast<int16_t>(value);
            }
            break;
        }
        case StatsMarioOptions::STATS_MARIO_LEVEL:
        {
            pouchPtr->level = static_cast<int16_t>(value);
            break;
        }
        case StatsMarioOptions::STATS_MARIO_RANK:
        {
            pouchPtr->rank = static_cast<int16_t>(value);
            break;
        }
        case StatsMarioOptions::STATS_MARIO_STAR_POINTS:
        {
            pouchPtr->starPoints = static_cast<int16_t>(value);
            break;
        }
        case StatsMarioOptions::STATS_MARIO_STAR_PIECES:
        {
            pouchPtr->starPieces = static_cast<int16_t>(value);
            break;
        }
        case StatsMarioOptions::STATS_MARIO_SHINE_SPRITES:
        {
            pouchPtr->shineSprites = static_cast<int16_t>(value);
            break;
        }
        case StatsMarioOptions::STATS_MARIO_CURRENT_STAR_POWER:
        {
            pouchPtr->currentSp = static_cast<int16_t>(value);
            break;
        }
        case StatsMarioOptions::STATS_MARIO_MAX_STAR_POWER:
        {
            pouchPtr->maxSp = static_cast<int16_t>(value);

            // Prevent the current value from exceeding the max value
            if (pouchPtr->currentSp > value)
            {
                pouchPtr->currentSp = static_cast<int16_t>(value);
            }
            break;
        }
        case StatsMarioOptions::STATS_MARIO_SHOP_POINTS:
        {
            pouchPtr->shopPoints = static_cast<int16_t>(value);
            break;
        }
        case StatsMarioOptions::STATS_MARIO_PIANTAS_STORED:
        {
            *reinterpret_cast<int32_t *>(piantaParlorPtrRaw + 0x4) = value;
            break;
        }
        case StatsMarioOptions::STATS_MARIO_CURRENT_PIANTAS:
        {
            *reinterpret_cast<int32_t *>(piantaParlorPtrRaw + 0x8) = value;
            break;
        }
        default:
        {
            break;
        }
    }

    // Depending on whicb option was selected, the cache for it will need to be cleared before a battle starts
    switch (index)
    {
        case StatsMarioOptions::STATS_MARIO_CURRENT_HP:
        case StatsMarioOptions::STATS_MARIO_CURRENT_FP:
        case StatsMarioOptions::STATS_MARIO_MAX_HP:
        case StatsMarioOptions::STATS_MARIO_MAX_FP:
        case StatsMarioOptions::STATS_MARIO_LEVEL:
        case StatsMarioOptions::STATS_MARIO_RANK:
        case StatsMarioOptions::STATS_MARIO_STAR_POINTS:
        case StatsMarioOptions::STATS_MARIO_CURRENT_STAR_POWER:
        case StatsMarioOptions::STATS_MARIO_MAX_STAR_POWER:
        {
            // Only set the flag for clearing the cache if not in a battle
            if (!checkForSpecificSeq(SeqIndex::kBattle))
            {
                gMod->setFlag(ModFlag::MOD_FLAG_CLEAR_MARIO_STATS);
            }
            break;
        }
        default:
        {
            break;
        }
    }

    // Close the value editor
    statsMenuCancelChangingValue();
}

static void selectedOptionChangeValue(Menu *menuPtr)
{
    (void)menuPtr;

    StatsMenu *statsMenuPtr = gStatsMenu;

    // Make sure the current index is valid
    const uint32_t index = statsMenuPtr->getCurrentIndex();
    if (index >= STATS_MARIO_TOTAL_ENTRIES)
    {
        // Failsafe: Reset the current index to 0
        statsMenuPtr->setCurrentIndex(0);
        return;
    }

    // Get the current, min, and max values
    // Also get the variable type
    const PouchData *pouchPtr = pouchGetPtr();
    int32_t currentValue = getStat(pouchPtr, index);

    // Make sure the value isn't negative
    if (currentValue < 0)
    {
        currentValue = 0;
    }

    int32_t minValue = 0;
    int32_t maxValue = 999;
    VariableType type = VariableType::s16;

    switch (index)
    {
        case StatsMarioOptions::STATS_MARIO_CURRENT_HP:
        {
            maxValue = pouchPtr->maxHp;
            break;
        }
        case StatsMarioOptions::STATS_MARIO_CURRENT_FP:
        {
            maxValue = pouchPtr->maxFp;
            break;
        }
        case StatsMarioOptions::STATS_MARIO_BP:
        case StatsMarioOptions::STATS_MARIO_LEVEL:
        case StatsMarioOptions::STATS_MARIO_STAR_POINTS:
        {
            maxValue = 99;
            break;
        }
        case StatsMarioOptions::STATS_MARIO_RANK:
        {
            maxValue = 3;
            break;
        }
        case StatsMarioOptions::STATS_MARIO_CURRENT_STAR_POWER:
        {
            maxValue = pouchPtr->maxSp;
            break;
        }
        case StatsMarioOptions::STATS_MARIO_MAX_STAR_POWER:
        {
            maxValue = 800;
            break;
        }
        case StatsMarioOptions::STATS_MARIO_SHOP_POINTS:
        {
            maxValue = 300;
            break;
        }
        case StatsMarioOptions::STATS_MARIO_PIANTAS_STORED:
        case StatsMarioOptions::STATS_MARIO_CURRENT_PIANTAS:
        {
            maxValue = 99999;
            type = VariableType::s32;
            break;
        }
        default:
        {
            break;
        }
    }

    // Initialize the value editor
    ValueEditor *valueEditorPtr = statsMenuPtr->getValueEditorPtr();

    uint32_t flags = 0;
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Y_SET_MAX);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Z_SET_MIN);

    const Window *rootWindowPtr = gRootWindow;
    valueEditorPtr->init(&currentValue, &minValue, &maxValue, rootWindowPtr, flags, type, rootWindowPtr->getAlpha());
    valueEditorPtr->startDrawing(changeValue, statsMenuCancelChangingValue);
}

static void cancelToggleSpecialMoves()
{
    gStatsMenu->getSpecialMoveTogglerPtr()->stopDrawing();
}

static void selectedOptionToggleSpecialMoves(Menu *menuPtr)
{
    (void)menuPtr;

    // Initialize the special move toggler
    StatsMenu *statsMenuPtr = gStatsMenu;
    SpecialMoveToggler *specialMoveTogglerPtr = statsMenuPtr->getSpecialMoveTogglerPtr();

    const Window *rootWindowPtr = gRootWindow;
    specialMoveTogglerPtr->init(rootWindowPtr, rootWindowPtr->getAlpha());
    specialMoveTogglerPtr->startDrawing(nullptr, cancelToggleSpecialMoves);
}

void StatsMenu::drawMarioStats() const
{
    // Draw the main text and icons
    float iconPosXBase;
    float iconPosYBase;
    constexpr float scale = MENU_SCALE;
    gRootWindow->getIconPosXY(WindowAlignment::TOP_LEFT, scale, &iconPosXBase, &iconPosYBase);

    PouchData *pouchPtr = pouchGetPtr();
    constexpr float iconPosXIncrement = 317.f * scale;
    constexpr float starPowerAdjustment = 8.f * scale;
    constexpr float lineDecrement = SPACE_USED_PER_ICON(scale);

    float iconPosX = iconPosXBase;
    float iconPosY = iconPosYBase;

    const uint32_t currentIndex = this->currentIndex;
    const IconId *optionsIconsPtr = gOptionsIcons;
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

        // The icons for the Diamond Star and star power need to be moved up slightly
        float tempPosY = iconPosY;

        switch (i)
        {
            case StatsMarioOptions::STATS_MARIO_SPECIAL_MOVES:
            {
                tempPosY += (3.f * scale);
                break;
            }
            case StatsMarioOptions::STATS_MARIO_CURRENT_STAR_POWER:
            case StatsMarioOptions::STATS_MARIO_MAX_STAR_POWER:
            {
                tempPosY += starPowerAdjustment;
                break;
            }
            default:
            {
                break;
            }
        }

        // Draw the icon
        drawIcon(iconPosX, tempPosY, scale, optionsIconsPtr[i]);

        // Draw the icons for the special moves
        if (i == StatsMarioOptions::STATS_MARIO_SPECIAL_MOVES)
        {
            constexpr float specialMovesPosXIncrement = 13.f;
            const IconId *specialMoveIconsPtr = gSpecialMoveIcons;
            const uint32_t starPowersObtained = pouchPtr->starPowersObtained;

            constexpr float specialMovesScale = 0.37f;
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

    const MenuOption *optionsPtr = gOptions;
    float textPosX = textPosXBase;
    float textPosY = textPosYBase;

    const float valuePosXBase = textPosX + (253.f * scale);
    constexpr float valuePosXIncrement = 417.f * scale;

    float valuePosX = valuePosXBase;
    float maxWidth = 48.f;
    char buf[32];

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
        drawText(optionsPtr[i].name, textPosX, textPosY, scale, color);

        // Draw the value
        if (i != StatsMarioOptions::STATS_MARIO_SPECIAL_MOVES)
        {
            // Get the value for the current option
            value = getStat(pouchPtr, i);

            snprintf(buf, sizeof(buf), "%" PRId32, value);
            drawText(buf, valuePosX, textPosY, scale, maxWidth, getColorWhite(0xFF), TextAlignment::RIGHT);
        }

        textPosY -= lineDecrement;
    }
}

static void draw(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Draw the main window
    drawMainWindow();

    // Draw Mario's stats
    StatsMenu *statsMenuPtr = gStatsMenu;
    statsMenuPtr->drawMarioStats();

    // Draw the value editor if applicable
    ValueEditor *valueEditorPtr = statsMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->draw();
    }

    // Draw the special move toggler if applicable
    SpecialMoveToggler *specialMoveTogglerPtr = statsMenuPtr->getSpecialMoveTogglerPtr();
    if (specialMoveTogglerPtr->shouldDraw())
    {
        specialMoveTogglerPtr->draw();
    }
}
