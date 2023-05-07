#ifndef MENUS_STATS_H
#define MENUS_STATS_H

#include "classes/menu.h"
#include "classes/valueEditor.h"
#include "ttyd/dispdrv.h"

#include <cstdint>

#define STATS_MARIO_TOTAL_ENTRIES 17
#define STATS_MARIO_ENTRIES_PER_COLUMN 9

enum StatsMarioOptions
{
    STATS_MARIO_COINS = 0,
    STATS_MARIO_CURRENT_HP,
    STATS_MARIO_CURRENT_FP,
    STATS_MARIO_BP,
    STATS_MARIO_MAX_HP,
    STATS_MARIO_MAX_FP,
    STATS_MARIO_LEVEL,
    STATS_MARIO_RANK,
    STATS_MARIO_STAR_POINTS,
    STATS_MARIO_STAR_PIECES,
    STATS_MARIO_SHINE_SPRITES,
    STATS_MARIO_SPECIAL_MOVES,
    STATS_MARIO_CURRENT_STAR_POWER,
    STATS_MARIO_MAX_STAR_POWER,
    STATS_MARIO_SHOP_POINTS,
    STATS_MARIO_PIANTAS_STORED,
    STATS_MARIO_CURRENT_PIANTAS,
};

enum StatsMenu
{
    STATS_MENU_MARIO = 0,
    STATS_MENU_PARTNERS,
    STATS_MENU_FOLLOWERS,
};

enum StatsFlag
{
    STATS_FLAG_CURRENTLY_SELECTING_ID = 0,
};

class Stats
{
   public:
    Stats(float scale) { this->scale = scale; }
    ~Stats() {};

    ValueEditor *getValueEditor() { return &this->valueEditor; }
    int32_t *getMinValuePtr() { return &this->minValue; }
    int32_t *getMaxValuePtr() { return &this->maxValue; }

    void setMinValue(int32_t value) { this->minValue = value; }
    void setMaxValue(int32_t value) { this->maxValue = value; }

    void drawMarioStats();

   private:
    ValueEditor valueEditor;
    int32_t minValue;
    int32_t maxValue;
    float scale;
    uint8_t currentIndex; // Current cursor position
};

extern Stats *gStats;

void statsMenuMarioInit(Menu *menuPtr);
void statsMenuMarioControls(Menu *menuPtr, MenuButtonInput button);
void statsMenuMarioDraw(CameraId cameraId, void *user);

// Called when initially entering the part of the stats menu for selecting whether to work with Mario, partners, or followers
void statsMenuInit(Menu *menuPtr);

void statsMenuInitExit(); // Called when exiting the init part of the stats menu

#endif
