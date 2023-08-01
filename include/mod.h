#ifndef MOD_H
#define MOD_H

#include <cstdint>

#define VERSION_STRING "v4.0-dev"

#define DRAW_ORDER_PROJECT_INFO -1.f
#define DRAW_ORDER_DISPLAYS 0.f
#define DRAW_ORDER_DISPLAY_GUARD_SUPERGUARD_TIMINGS 1.f
#define DRAW_ORDER_CHEAT_SPAWN_ITEM 2.f
#define DRAW_ORDER_DISPLAY_ERRORS 900.f
#define DRAW_ORDER_MENU 1000.f

enum class VariableType : uint8_t
{
    s8 = 0,
    s16,
    s32,
    s64,
    u8,
    u16,
    u32,
    u64,
    f32,
    f64,
    string,
    time,
};

union ValueType
{
    int8_t s8;
    int16_t s16;
    int32_t s32;
    int64_t s64;
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
    float f32;
    double f64;
};

class Mod
{
   public:
    Mod() {}
    ~Mod() {}

    bool menuIsHidden() const { return this->menuHidden; }
    void hideMenu() { this->menuHidden = true; }
    void showMenu() { this->menuHidden = false; }

    bool changingButtonCombo() const { return this->changingButtonCombos; }
    void startChangingButtonCombo() { this->changingButtonCombos = true; }
    void stopChangingButtonCombo() { this->changingButtonCombos = false; }

    bool checkIfSystemLevelIsSet() const { return this->systemLevelIsSet; }
    void setSystemLevel() { this->systemLevelIsSet = true; }
    void clearSystemLevel() { this->systemLevelIsSet = false; }

    bool shouldClearMarioStatsCache() const { return this->clearMarioStats; }
    void clearMarioStatsCache() { this->clearMarioStats = true; }
    void resetMarioStatsCacheBool() { this->clearMarioStats = false; }

    bool shouldClearPartnerStatsCache() const { return this->clearPartnerStats; }
    void clearPartnerStatsCache() { this->clearPartnerStats = true; }
    void resetPartnerStatsCacheBool() { this->clearPartnerStats = false; }

   private:
    bool menuHidden;
    bool changingButtonCombos;
    bool systemLevelIsSet;

    // Used to clear the cache for Mario/partner stats before a battle starts
    bool clearMarioStats;
    bool clearPartnerStats;
};

extern Mod gMod;
extern const char *gHelpTextButtonCombo;
extern const char *gHelpTextAConfirmBCancel;
extern const char *gTimeStringFormat;

#endif
