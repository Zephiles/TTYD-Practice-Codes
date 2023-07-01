#ifndef MENUS_BATTLES_MENU_H
#define MENUS_BATTLES_MENU_H

#include "menuUtils.h"
#include "classes/valueEditor.h"
#include "ttyd/camdrv.h"
#include "ttyd/battle_unit.h"
#include "ttyd/battle.h"

#include <cstdint>

#define BATTLES_SELECT_ACTOR_TOTAL_ACTORS_PER_PAGE 18
#define BATTLES_STATUSES_MAX_STATUSES_PER_PAGE 12

enum StatsBattlesStatsOptions
{
    BATTLES_STATS_ACTOR_CURRENT_HP = 0,
    BATTLES_STATS_ACTOR_MAX_HP,
    BATTLES_STATS_ACTOR_CURRENT_FP,
    BATTLES_STATS_ACTOR_MAX_FP,
    BATTLES_STATS_ACTOR_CHANGE_HELD_ITEM,
};

enum StatsBattlesStatusesOptions
{
    BATTLES_STATUSES_SLEEP_TURNS_LEFT = 0,
    BATTLES_STATUSES_IMMOBILIZED_TURNS_LEFT,
    BATTLES_STATUSES_DIZZY_TURNS_LEFT,
    BATTLES_STATUSES_POISON_TURNS_LEFT,
    BATTLES_STATUSES_POISON_DAMAGE_AMOUNT,
    BATTLES_STATUSES_CONFUSED_TURNS_LEFT,
    BATTLES_STATUSES_ELECTRIFIED_TURNS_LEFT,
    BATTLES_STATUSES_DODGY_TURNS_LEFT,
    BATTLES_STATUSES_BURN_TURNS_LEFT,
    BATTLES_STATUSES_FROZEN_TURNS_LEFT,
    BATTLES_STATUSES_BIG_SHRINK_TURNS_LEFT,
    BATTLES_STATUSES_BIG_SHRINK_POWER_AMOUNT,
    BATTLES_STATUSES_ATTACK_UP_DOWN_TURNS_LEFT,
    BATTLES_STATUSES_ATTACK_UP_DOWN_POWER_AMOUNT,
    BATTLES_STATUSES_DEFENSE_UP_DOWN_TURNS_LEFT,
    BATTLES_STATUSES_DEFENSE_UP_DOWN_POWER_AMOUNT,
    BATTLES_STATUSES_CHARGED_TURNS_LEFT,
    BATTLES_STATUSES_ALLERGIC_TURNS_LEFT,
    BATTLES_STATUSES_INVISIBLE_TURNS_LEFT,
    BATTLES_STATUSES_PAYBACK_TURNS_LEFT,
    BATTLES_STATUSES_FAST_TURNS_LEFT,
    BATTLES_STATUSES_SLOW_TURNS_LEFT,
    BATTLES_STATUSES_HP_REGEN_TURNS_LEFT,
    BATTLES_STATUSES_HP_REGEN_AMOUNT,
    BATTLES_STATUSES_FP_REGEN_TURNS_LEFT,
    BATTLES_STATUSES_FP_REGEN_AMOUNT,
    BATTLES_STATUSES_DEFEATED_FLAG,
};

enum BattlesStatsFlag
{
    BATTLES_FLAG_STATS_CURRENTLY_SELECTING_ID = 0,
};

enum BattlesStatusesFlag
{
    BATTLES_FLAG_STATUSES_CURRENTLY_SELECTING_ID = 0,
};

class BattlesMenu
{
   public:
    BattlesMenu(float scale)
    {
        this->scale = scale;
        this->battlesMenu = gMenu;
    }

    ~BattlesMenu() {}

    ValueEditor *getValueEditor() { return &this->valueEditor; }
    float getScale() const { return this->scale; }
    MenuAutoIncrement *getAutoIncrementPtr() { return &this->autoIncrement; }
    const Menu *getBattlesMenu() const { return this->battlesMenu; }

    uint32_t getCurrentActorIndex() const { return this->currentActorIndex; }
    void setCurrentActorIndex(uint32_t index) { this->currentActorIndex = static_cast<uint8_t>(index); }

    void drawBattlesActors() const;
    void drawBattleActorStats(BattleWorkUnit *actorPtr) const;
    void drawBattleActorStatuses(BattleWorkUnit *actorPtr) const;

   private:
    ValueEditor valueEditor;
    float scale;
    Menu *battlesMenu; // Backup of gMenu when the Battles menu was initially opened

    MenuAutoIncrement autoIncrement;
    uint8_t currentActorIndex; // Index for the selected actor
};

extern BattlesMenu *gBattlesMenu;
extern const char *gBattleActors[TOTAL_BATTLE_UNIT_TYPES];

inline uint32_t getbattlesMenuInitMaxIndex()
{
    constexpr uint32_t maxActors = sizeof(BattleWork::battle_units) / sizeof(BattleWork::battle_units[0]);

    // Subtract one to skip the System actor, and subtract another one due to needing to add one to getActorBattlePtr in the
    // drawing function to get the proper pointer
    return maxActors - 2;
}

void battlesMenuInit(Menu *menuPtr);
void battlesMenuInitControls(Menu *menuPtr, MenuButtonInput button);
void battlesMenuInitDraw(CameraId cameraId, void *user);
void battlesMenuInitExit();

// Called when inside part of the Battles menu in which an actor was selected, but the actor is no longer in the battle
void battlesMenuReturnToInitMenu();

void battlesMenuStatsInit();
void battlesMenuStatsControls(Menu *menuPtr, MenuButtonInput button);
void battlesMenuStatsDraw(CameraId cameraId, void *user);

void selectedOptionBattlesChangeValue(Menu *menuPtr);
void selectedOptionBattlesClearHeldItem(Menu *menuPtr);

void battlesMenuStatusesInit(Menu *menuPtr);
void battlesMenuStatusesControls(Menu *menuPtr, MenuButtonInput button);
void battlesMenuStatusesDraw(CameraId cameraId, void *user);

BattleWork *getBattleWorkPtr();
BattleWorkUnit *getMarioBattlePtr();
BattleWorkUnit *getPartnerBattlePtr();
BattleWorkUnit *getActorBattlePtr(uint32_t slot);

#endif
