#ifndef MOD_H
#define MOD_H

#include "misc/utils.h"
#include "ttyd/mapdata.h"

#include <cstdint>

#define VERSION_STRING "v4.0.3-dev"

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

enum ModFlag
{
    // Misc flags
    MOD_FLAG_MENU_IS_HIDDEN = 0,
    MOD_FLAG_CHANGING_BUTTON_COMBO,
    MOD_FLAG_SYSTEM_LEVEL,
    MOD_FLAG_CLEAR_MARIO_STATS,
    MOD_FLAG_CLEAR_PARTNER_STATS,

    // Warp By Event
    MOD_FLAG_WARP_BY_EVENT_INIT,
    MOD_FLAG_WARP_BY_EVENT_KEEP_INVENTORY,
    MOD_FLAG_WARP_BY_EVENT_EQUIP_BADGES,
    MOD_FLAG_WARP_BY_EVENT_SET_FLAGS,

    // Warp By Index
    MOD_FLAG_WARP_BY_INDEX_INIT,

    MOD_FLAG_MAX_VALUE, // Don't use this directly other than for defines
};

#define TOTAL_MOD_FLAGS ModFlag::MOD_FLAG_MAX_VALUE
#define MOD_FLAGS_ARRAY_SIZE intCeil(TOTAL_MOD_FLAGS, sizeof(uint32_t) * 8)

class WarpByEvent
{
   public:
    WarpByEvent() {}
    ~WarpByEvent() {}

    uint32_t getIndex() const { return this->index; }
    void setIndex(uint32_t index) { this->index = static_cast<uint16_t>(index); }

   private:
    uint16_t index; // Value Editor
};

#define WARP_BY_INDEX_ENTRANCE_LIST_SIZE 32
class WarpByIndex
{
   public:
    WarpByIndex()
    {
        entranceId = 1;
        unusedMapData.mapName = unusedMapName;
    }

    ~WarpByIndex() {}

    const char **getEntranceListPtr() { return this->entranctList; }

    MapData *getUnusedMapDataPtr() { return &this->unusedMapData; }

    const void *getCurrentMapInitEvtCodePtr() const { return this->currentMapInitEvtCode; }
    void setCurrentMapInitEvtCodePtr(const void *initEvtCodePtr) { this->currentMapInitEvtCode = initEvtCodePtr; }
    void clearCurrentMapInitEvtCodePtr() { this->currentMapInitEvtCode = nullptr; }

    uint32_t getMapId() const { return this->mapId; }
    void setMapId(uint32_t id) { this->mapId = static_cast<uint16_t>(id); }

    uint32_t getEntranceId() const { return this->entranceId; }
    void setEntranceId(uint32_t id) { this->entranceId = static_cast<uint8_t>(id); }

    uint32_t getTotalEntrances() const { return this->totalEntrances; }
    void setTotalEntrances(uint32_t entrances) { this->totalEntrances = static_cast<uint8_t>(entrances); }

    char *getUnusedMapNamePtr() { return this->unusedMapName; }
    uint32_t getUnusedMapNameSize() const { return sizeof(this->unusedMapName); }

   private:
    const char *entranctList[WARP_BY_INDEX_ENTRANCE_LIST_SIZE];
    MapData unusedMapData;
    const void *currentMapInitEvtCode;
    uint16_t mapId;
    uint8_t entranceId;
    uint8_t totalEntrances;
    char unusedMapName[9]; // 8 bytes for unusedMapName, 1 byte for null
};

class Mod
{
   public:
    Mod()
    {
        this->setFlag(ModFlag::MOD_FLAG_WARP_BY_EVENT_KEEP_INVENTORY);
        this->setFlag(ModFlag::MOD_FLAG_WARP_BY_EVENT_EQUIP_BADGES);
        this->setFlag(ModFlag::MOD_FLAG_WARP_BY_EVENT_SET_FLAGS);
    }

    ~Mod() {}

    bool flagIsSet(uint32_t flag) const
    {
        // Make sure the flag is valid
        constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
        constexpr uint32_t maxFlags = MOD_FLAGS_ARRAY_SIZE * bitsPerWord;

        if (flag >= maxFlags)
        {
            return false;
        }

        // Make sure the flag does not exceed TOTAL_MOD_FLAGS
        if (flag >= TOTAL_MOD_FLAGS)
        {
            return false;
        }

        return (this->flags[flag / bitsPerWord] >> (flag % bitsPerWord)) & 1U;
    }

    void setFlag(uint32_t flag)
    {
        // Make sure the flag is valid
        constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
        constexpr uint32_t maxFlags = MOD_FLAGS_ARRAY_SIZE * bitsPerWord;

        if (flag >= maxFlags)
        {
            return;
        }

        // Make sure the flag does not exceed TOTAL_MOD_FLAGS
        if (flag >= TOTAL_MOD_FLAGS)
        {
            return;
        }

        this->flags[flag / bitsPerWord] |= (1UL << (flag % bitsPerWord));
    }

    void clearFlag(uint32_t flag)
    {
        // Make sure the flag is valid
        constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
        constexpr uint32_t maxFlags = MOD_FLAGS_ARRAY_SIZE * bitsPerWord;

        if (flag >= maxFlags)
        {
            return;
        }

        // Make sure the flag does not exceed TOTAL_MOD_FLAGS
        if (flag >= TOTAL_MOD_FLAGS)
        {
            return;
        }

        this->flags[flag / bitsPerWord] &= ~(1UL << (flag % bitsPerWord));
    }

    bool toggleFlag(uint32_t flag)
    {
        // Make sure the flag is valid
        constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
        constexpr uint32_t maxFlags = MOD_FLAGS_ARRAY_SIZE * bitsPerWord;

        if (flag >= maxFlags)
        {
            return false;
        }

        // Make sure the flag does not exceed TOTAL_MOD_FLAGS
        if (flag >= TOTAL_MOD_FLAGS)
        {
            return false;
        }

        this->flags[flag / bitsPerWord] ^= (1UL << (flag % bitsPerWord));
        return this->flagIsSet(flag);
    }

    WarpByIndex *getWarpByIndexPtr() { return &this->warpByIndex; }
    WarpByEvent *getWarpByEventPtr() { return &this->warpByEvent; }

   private:
    uint32_t flags[MOD_FLAGS_ARRAY_SIZE];
    WarpByIndex warpByIndex;
    WarpByEvent warpByEvent;
};

extern Mod *gMod;
extern const char *gHelpTextButtonCombo;
extern const char *gHelpTextAConfirmBCancel;
extern const char *gTimeStringFormat;

#endif
