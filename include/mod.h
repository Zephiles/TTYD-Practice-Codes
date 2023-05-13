#ifndef MOD_H
#define MOD_H

#include <cstdint>

#define VERSION_STRING "v4.0-dev"

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

// Used to clear the cache for Mario/partner stats before a battle starts
struct ClearStatsCacheForBattles
{
    bool clearMarioStats;
    bool clearPartnerStats;
} __attribute__((__packed__));

class Mod
{
   public:
    bool checkIfSystemLevelIsSet() const { return this->systemLevelIsSet; }
    void setSystemLevel() { this->systemLevelIsSet = true; }
    void clearSystemLevel() { this->systemLevelIsSet = false; }

    bool shouldClearMarioStatsCache() const { return this->clearStatsCacheForBattles.clearMarioStats; }
    void clearMarioStatsCache() { this->clearStatsCacheForBattles.clearMarioStats = true; }
    void resetMarioStatsCacheBool() { this->clearStatsCacheForBattles.clearMarioStats = false; }

    bool shouldClearPartnerStatsCache() const { return this->clearStatsCacheForBattles.clearPartnerStats; }
    void clearPartnerStatsCache() { this->clearStatsCacheForBattles.clearPartnerStats = true; }
    void resetPartnerStatsCacheBool() { this->clearStatsCacheForBattles.clearPartnerStats = false; }

   private:
    bool systemLevelIsSet;
    ClearStatsCacheForBattles clearStatsCacheForBattles;
};

extern Mod *gMod;

#endif
