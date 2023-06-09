#ifndef MENUS_BATTLES_H
#define MENUS_BATTLES_H

#include "menuUtils.h"
#include "classes/valueEditor.h"
#include "ttyd/dispdrv.h"
#include "ttyd/battle_unit.h"
#include "ttyd/battle.h"

#include <cstdint>

#define BATTLES_TOTAL_ACTORS_PER_PAGE 18

enum BattlesStatsFlag
{
    BATTLES_FLAG_STATS_CURRENTLY_SELECTING_ID = 0,
};

class Battles
{
   public:
    Battles(float scale)
    {
        this->scale = scale;
        this->battlesMenu = gMenu;
    }

    ~Battles() {}

    ValueEditor *getValueEditor() { return &this->valueEditor; }
    float getScale() const { return this->scale; }
    MenuAutoIncrement *getAutoIncrementPtr() { return &this->autoIncrement; }
    Menu *getBattlesMenu() { return this->battlesMenu; }

    uint32_t getCurrentActorIndex() const { return this->currentActorIndex; }
    void setCurrentActorIndex(uint32_t index) { this->currentActorIndex = static_cast<uint8_t>(index); }

    void drawBattlesActors() const;
    void drawBattleActorStats() const;

   private:
    ValueEditor valueEditor;
    float scale;
    Menu *battlesMenu; // Backup of gMenu when the Battles menu was initially opened

    int32_t minValue;
    int32_t maxValue;

    MenuAutoIncrement autoIncrement;
    uint8_t currentActorIndex; // Index for the selected actor

    uint8_t currentPage;
    uint8_t currentIndex; // Current cursor position
};

extern Battles *gBattles;
extern const char *gBattleActors[TOTAL_BATTLE_UNIT_TYPES];

inline uint32_t getbattlesMenuSelectActorMaxIndex()
{
    constexpr uint32_t maxActors = sizeof(BattleWork::battle_units) / sizeof(BattleWork::battle_units[0]);

    // Subtract one to skip the System actor, and subtract another one due to needing to add one to getActorBattlePtr in the
    // drawing function to get the proper pointer
    return maxActors - 2;
}

void battlesMenuSelectActorInit(Menu *menuPtr);
void battlesMenuSelectActorControls(Menu *menuPtr, MenuButtonInput button);
void battlesMenuSelectActorDraw(CameraId cameraId, void *user);
void battlesMenuSelectActorExit();

// Called when inside part of the Battles menu in which an actor was selected, but the actor is no longer in the battle
void battlesMenuReturnToSelectActorMenu();

void battlesMenuStatsInit();
void battlesMenuStatsControls(Menu *menuPtr, MenuButtonInput button);
void battlesMenuStatsDraw(CameraId cameraId, void *user);

void selectedOptionBattlesClearHeldItem(Menu *menuPtr);

BattleWork *getBattleWorkPtr();
BattleWorkUnit *getMarioBattlePtr();
BattleWorkUnit *getPartnerBattlePtr();
BattleWorkUnit *getActorBattlePtr(uint32_t slot);

#endif
