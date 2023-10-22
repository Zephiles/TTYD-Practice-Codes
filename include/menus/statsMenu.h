#ifndef MENUS_STATS_MENU_H
#define MENUS_STATS_MENU_H

#include "classes/menu.h"
#include "classes/valueEditor.h"
#include "classes/specialMoveToggler.h"
#include "classes/followerSelector.h"
#include "classes/yoshiColorSelector.h"
#include "classes/nameEditor.h"
#include "classes/errorWindow.h"
#include "misc/utils.h"

#include <cstdint>

#define STATS_MARIO_TOTAL_ENTRIES 17
#define STATS_MARIO_ENTRIES_PER_COLUMN 9
#define STATS_MARIO_ENTRIES_PER_ROW intCeil(STATS_MARIO_TOTAL_ENTRIES, STATS_MARIO_ENTRIES_PER_COLUMN)

#define STATS_PARTNERS_DRAWING_YOSHI_STATS(index) (index == 3) // Assuming Yoshi should be the 4th option

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

enum StatsPartnersCurrentPartnerOption
{
    STATS_PARTNER_SET_CURRENT_HP,
    STATS_PARTNER_SET_MAX_HP,
    STATS_PARTNER_SET_RANK,
    STATS_PARTNER_TOGGLE_ENABLED_BOOL,
    STATS_PARTNER_BRING_OUT_OR_REMOVE,
};

enum StatsPartnersCurrentPartnerOptionYoshi
{
    STATS_PARTNER_YOSHI_SET_CURRENT_HP,
    STATS_PARTNER_YOSHI_SET_MAX_HP,
    STATS_PARTNER_YOSHI_SET_RANK,
    STATS_PARTNER_YOSHI_TOGGLE_ENABLED_BOOL,
    STATS_PARTNER_YOSHI_SET_COLOR,
    STATS_PARTNER_YOSHI_SET_NAME,
    STATS_PARTNER_YOSHI_BRING_OUT_OR_REMOVE,
};

enum StatsFlagPartner
{
    STATS_FLAG_PARTNER_SELECTED_PARTNER = 0,
};

class StatsMenu
{
   public:
    StatsMenu() {}
    ~StatsMenu() {}

    ValueEditor *getValueEditorPtr() { return &this->valueEditor; }
    SpecialMoveToggler *getSpecialMoveTogglerPtr() { return &this->specialMoveToggler; }
    FollowerSelector *getFollowerSelectorPtr() { return &this->followerSelector; }
    YoshiColorSelector *getYoshiColorSelectorPtr() { return &this->yoshiColorSelector; }
    NameEditor *getNameEditorPtr() { return &this->nameEditor; }
    ErrorWindow *getErrorWindowPtr() { return &this->errorWindow; }

    MenuAutoIncrement *getAutoIncrementPtr() { return &this->autoIncrement; }

    uint32_t getCurrentIndex() const { return this->currentIndex; }
    uint8_t *getCurrentIndexPtr() { return &this->currentIndex; }
    void setCurrentIndex(uint32_t index) { this->currentIndex = static_cast<uint8_t>(index); }

    void initErrorWindow(bool drawForPartner);

   private:
    ValueEditor valueEditor;
    SpecialMoveToggler specialMoveToggler;
    FollowerSelector followerSelector;
    YoshiColorSelector yoshiColorSelector;
    NameEditor nameEditor;
    ErrorWindow errorWindow;

    MenuAutoIncrement autoIncrement;
    uint8_t currentIndex; // Current cursor position

    char errorMessage[128]; // Needed since the error message is created via snprintf
};

extern StatsMenu *gStatsMenu;

// statsInit
// Called when initially entering the part of the stats menu for selecting whether to work with Mario, partners, or followers
void statsMenuInit(Menu *menuPtr);

void statsMenuCancelChangingValue();

// statsMario
void statsMenuMarioInit(Menu *menuPtr);

// statsPartners
void statsMenuPartnersInit(Menu *menuPtr);

// statsFollowers
void statsMenuFollowersInit(Menu *menuPtr);

#endif
