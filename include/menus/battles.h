#ifndef MENUS_BATTLES_H
#define MENUS_BATTLES_H

#include "classes/valueEditor.h"
#include "classes/menu.h"
#include "ttyd/dispdrv.h"
#include "ttyd/battle_unit.h"
#include "ttyd/battle.h"

#include <cstdint>

#define BATTLES_TOTAL_ACTORS_PER_PAGE 18

enum BattlesSelectActorFlag
{
    BATTLES_FLAG_SELECT_ACTOR_CURRENTLY_SELECTING_ID = 0,
};

class Battles
{
   public:
    Battles(float scale) { this->scale = scale; }
    ~Battles() {}

    ValueEditor *getValueEditor() { return &this->valueEditor; }
    float getScale() const { return this->scale; }
    MenuAutoIncrement *getAutoIncrementPtr() { return &this->autoIncrement; }

    uint32_t getCurrentActorIndex() const { return this->currentActorIndex; }
    void setCurrentActorIndex(uint32_t index) { this->currentActorIndex = static_cast<uint8_t>(index); }

    void drawBattlesActors() const;

   private:
    ValueEditor valueEditor;
    float scale;

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

    // Subtract one to skip the System actor, and subtract another one due to needing to add one to getActorPointer in the
    // drawing function to get the proper pointer
    return maxActors - 2;
}

void battlesMenuSelectActorInit(Menu *menuPtr);
void battlesMenuSelectActorControls(Menu *menuPtr, MenuButtonInput button);
void battlesMenuSelectActorDraw(CameraId cameraId, void *user);
void battlesMenuSelectActorExit();

void battlesMenuStatsInit();
void battlesMenuStatsControls(Menu *menuPtr, MenuButtonInput button);
void battlesMenuStatsDraw(CameraId cameraId, void *user);

BattleWork *getBattleWorkPtr();
BattleWorkUnit *getMarioBattlePointer();
BattleWorkUnit *getPartnerBattlePointer();
BattleWorkUnit *getActorPointer(uint32_t slot);

#endif
