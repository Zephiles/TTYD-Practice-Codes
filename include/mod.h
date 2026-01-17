#ifndef MOD_H
#define MOD_H

#include "misc/utils.h"
#include "ttyd/mapdata.h"

#include <cstdint>

#define VERSION_STRING "v4.0.1-dev7"

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
    MOD_FLAG_PERFORMING_AMW_GLITCH,
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

enum ModSaveFlag
{
    MOD_SAVE_FLAG_USE_ANALOG_STICK_IN_MENUS = 0,

    MOD_SAVE_FLAG_MAX_VALUE, // Don't use this directly other than for defines
};

#define TOTAL_MOD_SAVE_FLAGS ModSaveFlag::MOD_SAVE_FLAG_MAX_VALUE
#define MOD_SAVE_FLAGS_ARRAY_SIZE intCeil(TOTAL_MOD_SAVE_FLAGS, sizeof(uint32_t) * 8)

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

    const uint32_t *getFlagsPtr() const { return this->flags; }
    const uint32_t *getSaveFlagsPtr() const { return this->saveFlags; }
    WarpByIndex *getWarpByIndexPtr() { return &this->warpByIndex; }
    WarpByEvent *getWarpByEventPtr() { return &this->warpByEvent; }

    bool flagIsSet(uint32_t flag) const;
    void setFlag(uint32_t flag);
    void clearFlag(uint32_t flag);
    bool toggleFlag(uint32_t flag);

    bool saveFlagIsSet(uint32_t flag) const;
    void setSaveFlag(uint32_t flag);
    void clearSaveFlag(uint32_t flag);
    bool toggleSaveFlag(uint32_t flag);

   private:
    uint32_t flags[MOD_FLAGS_ARRAY_SIZE];
    uint32_t saveFlags[MOD_SAVE_FLAGS_ARRAY_SIZE];
    WarpByIndex warpByIndex;
    WarpByEvent warpByEvent;
};

extern Mod *gMod;
extern const char *gHelpTextButtonCombo;
extern const char *gHelpTextAConfirmBCancel;
extern const char *gTimeStringFormat;

bool _flagIsSet(const uint32_t *flagsPtr, uint32_t flag, uint32_t maxFlags, uint32_t totalFlags);
void _setFlag(uint32_t *flagsPtr, uint32_t flag, uint32_t maxFlags, uint32_t totalFlags);
void _clearFlag(uint32_t *flagsPtr, uint32_t flag, uint32_t maxFlags, uint32_t totalFlags);
bool _toggleFlag(uint32_t *flagsPtr, uint32_t flag, uint32_t maxFlags, uint32_t totalFlags);

#endif
