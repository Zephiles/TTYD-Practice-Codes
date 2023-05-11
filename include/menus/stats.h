#ifndef MENUS_STATS_H
#define MENUS_STATS_H

#include "classes/menu.h"
#include "classes/valueEditor.h"
#include "classes/specialMoveToggler.h"
#include "misc/utils.h"
#include "ttyd/dispdrv.h"

#include <cstdint>

#define STATS_MARIO_TOTAL_ENTRIES 17
#define STATS_MARIO_ENTRIES_PER_COLUMN 9
#define STATS_MARIO_ENTRIES_PER_ROW roundIntUpUnsigned(STATS_MARIO_TOTAL_ENTRIES, STATS_MARIO_ENTRIES_PER_COLUMN)

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

enum StatsFlagMario
{
    STATS_FLAG_MARIO_CURRENTLY_SELECTING_ID = 0,
    STATS_FLAG_MARIO_CURRENTLY_TOGGLING_SPECIAL_MOVES,
};

class Stats
{
   public:
    Stats(float scale) { this->scale = scale; }
    ~Stats() {};

    ValueEditor *getValueEditor() { return &this->valueEditor; }
    SpecialMoveToggler *getSpecialMoveToggler() { return &this->specialMoveToggler; }
    float getScale() const { return this->scale; }
    int32_t *getMinValuePtr() { return &this->minValue; }
    int32_t *getMaxValuePtr() { return &this->maxValue; }

    MenuAutoIncrement *getAutoIncrementPtr() { return &this->autoIncrement; }
    uint32_t getCurrentIndex() const { return this->currentIndex; }
    uint8_t *getCurrentIndexPtr() { return &this->currentIndex; }

    void setMinValue(int32_t value) { this->minValue = value; }
    void setMaxValue(int32_t value) { this->maxValue = value; }
    void setCurrentIndex(uint32_t index) { this->currentIndex = static_cast<uint8_t>(index); }

    void drawMarioStats() const;

   private:
    ValueEditor valueEditor;
    SpecialMoveToggler specialMoveToggler;
    float scale;

    int32_t minValue;
    int32_t maxValue;

    MenuAutoIncrement autoIncrement;
    uint8_t currentIndex; // Current cursor position
};

extern Stats *gStats;

void statsMenuMarioInit(Menu *menuPtr);
void statsMenuMarioControls(Menu *menuPtr, MenuButtonInput button);
void statsMenuMarioDraw(CameraId cameraId, void *user);

void selectedOptionMenuMarioChangeValue(Menu *menuPtr);
void selectedOptionMenuMarioSpecialMoves(Menu *menuPtr);

// Called when initially entering the part of the stats menu for selecting whether to work with Mario, partners, or followers
void statsMenuInit(Menu *menuPtr);

void statsMenuInitExit(); // Called when exiting the init part of the stats menu

#endif
