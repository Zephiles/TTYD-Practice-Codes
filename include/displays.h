#ifndef DISPLAYS_H
#define DISPLAYS_H

#include "cxx.h"
#include "drawText.h"
#include "gc/types.h"
#include "gc/OSAlloc.h"
#include "misc/utils.h"
#include "ttyd/memory.h"
#include "ttyd/dispdrv.h"
#include "ttyd/camdrv.h"
#include "ttyd/battle_unit.h"
#include "ttyd/battle_database_common.h"
#include "ttyd/hitdrv.h"
#include "ttyd/npcdrv.h"

#include <cstdint>
#include <cstring>

#define DISPLAYS_DEFAULT_POS_X_LEFT -233.f
#define DISPLAYS_DEFAULT_POS_X_RIGHT 233.f
#define DISPLAYS_DEFAULT_POS_Y_TOP_ERROR_TEXT 120.f
#define DISPLAYS_DEFAULT_POS_Y_TOP 100.f
#define DISPLAYS_DEFAULT_POS_Y_BOTTOM -160.f

#define DISPLAYS_DEFAULT_SCALE 0.65f
#define DISPLAYS_DEFAULT_SCALE_ERRORS (DISPLAYS_DEFAULT_SCALE - 0.05f)
#define GET_DISPLAYS_DEFAULT_SCALED_LINE_DECREMENT (LINE_HEIGHT_FLOAT * DISPLAYS_DEFAULT_SCALE)

class Displays;
typedef void (*DisplaysArrayFunc)(Displays *displaysPtr);

enum class DisplaysScreenPosition
{
    // DISPLAYS_POSITION_NONE should be used when a display needs to get its manual position, but the default position is
    // handled separately
    DISPLAYS_POSITION_NONE = 0,
    DISPLAYS_POSITION_BOTTOM_LEFT,
    DISPLAYS_POSITION_BOTTOM_RIGHT,
    DISPLAYS_POSITION_TOP_LEFT,
    DISPLAYS_POSITION_TOP_RIGHT,
};

// Enabled flags are mainly used for whether a display is enabled or not, but can also be for misc flags that are saved to the
// settings file
enum DisplaysEnabledFlag
{
    DISPLAYS_ENABLED_FLAG_ONSCREEN_TIMER = 0,
    DISPLAYS_ENABLED_FLAG_FRAME_COUNTER,
    DISPLAYS_ENABLED_FLAG_MARIO_COORDINATES,
    DISPLAYS_ENABLED_FLAG_MARIO_SPEED_X_Z,
    DISPLAYS_ENABLED_FLAG_JUMP_STORAGE,
    DISPLAYS_ENABLED_FLAG_BUTTON_INPUTS,
    DISPLAYS_ENABLED_FLAG_STICK_ANGLE,
    DISPLAYS_ENABLED_FLAG_STAR_POWER_VALUE,
    DISPLAYS_ENABLED_FLAG_DPAD_OPTIONS,
    DISPLAYS_ENABLED_FLAG_GUARD_SUPERGUARD_TIMINGS,
    DISPLAYS_ENABLED_FLAG_ART_ATTACK_HITBOXES,
    DISPLAYS_ENABLED_FLAG_EFFS_ACTIVE,
    DISPLAYS_ENABLED_FLAG_EVTS_ACTIVE,
    DISPLAYS_ENABLED_FLAG_ENEMY_ENCOUNTER_NOTIFIER,
    DISPLAYS_ENABLED_FLAG_HIT_CHECK_VISUALIZATION,

    // All of the flags for the Hit Check Visualization display need to be next to each other in order to work correctly
    // The code assumes that DRAW_HITS comes first
    DISPLAYS_ENABLED_FLAG_SHOULD_DRAW_HITS,   // Hit Check Visualization
    DISPLAYS_ENABLED_FLAG_SHOULD_DRAW_MISSES, // Hit Check Visualization

    // All of the flags for the trick displays need to be next to each other in order to work correctly
    // The code assumes that YOSHI_SKIP is first
    // The code assumes that BLIMP_TICKET_SKIP is last
    DISPLAYS_ENABLED_FLAG_YOSHI_SKIP,
    DISPLAYS_ENABLED_FLAG_PALACE_SKIP,
    DISPLAYS_ENABLED_FLAG_PALACE_SKIP_MINIMAL,

#ifdef TTYD_JP
    DISPLAYS_ENABLED_FLAG_AMW_SPIN_JUMP,
#endif

    DISPLAYS_ENABLED_FLAG_JABBI_HIVE_SKIP,
    DISPLAYS_ENABLED_FLAG_BRIDGE_SKIP,
    DISPLAYS_ENABLED_FLAG_BLIMP_TICKET_SKIP,

    // All of the flags for the Memory Usage display need to be next to each other in order to work correctly
    // The code assumes that HEAP_0 is first
    DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_HEAP_0, // Memory Usage
    DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_HEAP_1, // Memory Usage
    DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_HEAP_2, // Memory Usage
    DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_HEAP_3, // Memory Usage
    DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_HEAP_4, // Memory Usage

#ifdef TTYD_JP
    DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_HEAP_5, // Memory Usage
#endif

    DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_SMART_HEAP, // Memory Usage
    DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_MAP_HEAP,   // Memory Usage

#ifndef TTYD_JP
    DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_BATTLE_MAP_HEAP, // Memory Usage
#endif

    DISPLAYS_ENABLED_FLAG_MAX_VALUE, // Don't use this directly other than for defines
};

#define TOTAL_DISPLAYS_ENABLED_FLAGS DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MAX_VALUE
#define DISPLAYS_ENABLED_FLAGS_ARRAY_SIZE intCeil(TOTAL_DISPLAYS_ENABLED_FLAGS, sizeof(uint32_t) * 8)

#ifdef TTYD_JP
#define DISPLAYS_TOTAL_MAIN_HEAPS                                     \
    (DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_HEAP_5 - \
     DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_HEAP_0 + 1)

#define DISPLAYS_TOTAL_EXTRA_HEAPS                                      \
    (DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_MAP_HEAP - \
     DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_SMART_HEAP + 1)
#else
#define DISPLAYS_TOTAL_MAIN_HEAPS                                     \
    (DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_HEAP_4 - \
     DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_HEAP_0 + 1)

#define DISPLAYS_TOTAL_EXTRA_HEAPS                                             \
    (DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_BATTLE_MAP_HEAP - \
     DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_SMART_HEAP + 1)
#endif

#define DISPLAYS_TOTAL_HEAPS (DISPLAYS_TOTAL_MAIN_HEAPS + DISPLAYS_TOTAL_EXTRA_HEAPS)

// The proper order that the enabled flags appear in the settings file. Needed to properly restore them from the memory card.
enum DisplaysEnabledFlagOrder
{
    DISPLAYS_ENABLED_FLAG_ORDER_ONSCREEN_TIMER = 0,
    DISPLAYS_ENABLED_FLAG_ORDER_FRAME_COUNTER,
    DISPLAYS_ENABLED_FLAG_ORDER_MARIO_COORDINATES,
    DISPLAYS_ENABLED_FLAG_ORDER_MARIO_SPEED_X_Z,
    DISPLAYS_ENABLED_FLAG_ORDER_JUMP_STORAGE,
    DISPLAYS_ENABLED_FLAG_ORDER_BUTTON_INPUTS,
    DISPLAYS_ENABLED_FLAG_ORDER_STICK_ANGLE,
    DISPLAYS_ENABLED_FLAG_ORDER_STAR_POWER_VALUE,
    DISPLAYS_ENABLED_FLAG_ORDER_DPAD_OPTIONS,
    DISPLAYS_ENABLED_FLAG_ORDER_GUARD_SUPERGUARD_TIMINGS,
    DISPLAYS_ENABLED_FLAG_ORDER_ART_ATTACK_HITBOXES,
    DISPLAYS_ENABLED_FLAG_ORDER_EFFS_ACTIVE,
    DISPLAYS_ENABLED_FLAG_ORDER_EVTS_ACTIVE,
    DISPLAYS_ENABLED_FLAG_ORDER_ENEMY_ENCOUNTER_NOTIFIER,
    DISPLAYS_ENABLED_FLAG_ORDER_HIT_CHECK_VISUALIZATION,

    DISPLAYS_ENABLED_FLAG_ORDER_SHOULD_DRAW_HITS,   // Hit Check Visualization
    DISPLAYS_ENABLED_FLAG_ORDER_SHOULD_DRAW_MISSES, // Hit Check Visualization

    DISPLAYS_ENABLED_FLAG_ORDER_YOSHI_SKIP,
    DISPLAYS_ENABLED_FLAG_ORDER_PALACE_SKIP,
    DISPLAYS_ENABLED_FLAG_ORDER_PALACE_SKIP_MINIMAL,
    DISPLAYS_ENABLED_FLAG_ORDER_JABBI_HIVE_SKIP,
    DISPLAYS_ENABLED_FLAG_ORDER_BRIDGE_SKIP,
    DISPLAYS_ENABLED_FLAG_ORDER_BLIMP_TICKET_SKIP,

    DISPLAYS_ENABLED_FLAG_ORDER_MEMORY_USAGE_HEAP_0, // Memory Usage
    DISPLAYS_ENABLED_FLAG_ORDER_MEMORY_USAGE_HEAP_1, // Memory Usage
    DISPLAYS_ENABLED_FLAG_ORDER_MEMORY_USAGE_HEAP_2, // Memory Usage
    DISPLAYS_ENABLED_FLAG_ORDER_MEMORY_USAGE_HEAP_3, // Memory Usage
    DISPLAYS_ENABLED_FLAG_ORDER_MEMORY_USAGE_HEAP_4, // Memory Usage

#ifdef TTYD_JP
    DISPLAYS_ENABLED_FLAG_ORDER_MEMORY_USAGE_HEAP_5, // Memory Usage
#endif

    DISPLAYS_ENABLED_FLAG_ORDER_MEMORY_USAGE_SMART_HEAP, // Memory Usage
    DISPLAYS_ENABLED_FLAG_ORDER_MEMORY_USAGE_MAP_HEAP,   // Memory Usage

#ifndef TTYD_JP
    DISPLAYS_ENABLED_FLAG_ORDER_MEMORY_USAGE_BATTLE_MAP_HEAP, // Memory Usage
#endif

#ifdef TTYD_JP
    DISPLAYS_ENABLED_FLAG_ORDER_AMW_SPIN_JUMP,
#endif

    DISPLAYS_ENABLED_FLAG_ORDER_MAX_VALUE, // Don't use this directly other than for defines
};

// Manually position flags are for whether the display will be de drawn with the player's chosen position and scale or not
enum DisplaysManuallyPositionFlag
{
    DISPLAYS_MANUALLY_POSITION_FLAG_ONSCREEN_TIMER = 0,
    DISPLAYS_MANUALLY_POSITION_FLAG_FRAME_COUNTER,
    DISPLAYS_MANUALLY_POSITION_FLAG_MARIO_COORDINATES,
    DISPLAYS_MANUALLY_POSITION_FLAG_MARIO_SPEED_X_Z,
    DISPLAYS_MANUALLY_POSITION_FLAG_JUMP_STORAGE,
    DISPLAYS_MANUALLY_POSITION_FLAG_BUTTON_INPUTS,
    DISPLAYS_MANUALLY_POSITION_FLAG_STICK_ANGLE,
    DISPLAYS_MANUALLY_POSITION_FLAG_GUARD_SUPERGUARD_TIMINGS,
    DISPLAYS_MANUALLY_POSITION_FLAG_MEMORY_USAGE,
    DISPLAYS_MANUALLY_POSITION_FLAG_EFFS_ACTIVE,
    DISPLAYS_MANUALLY_POSITION_FLAG_EVTS_ACTIVE,
    DISPLAYS_MANUALLY_POSITION_FLAG_ENEMY_ENCOUNTER_NOTIFIER,

    // All of the flags for the trick displays need to be next to each other in order to work correctly
    // The code assumes that YOSHI_SKIP is first
    DISPLAYS_MANUALLY_POSITION_FLAG_YOSHI_SKIP,
    DISPLAYS_MANUALLY_POSITION_FLAG_PALACE_SKIP,
    DISPLAYS_MANUALLY_POSITION_FLAG_PALACE_SKIP_MINIMAL,

#ifdef TTYD_JP
    DISPLAYS_MANUALLY_POSITION_FLAG_AMW_SPIN_JUMP,
#endif

    DISPLAYS_MANUALLY_POSITION_FLAG_JABBI_HIVE_SKIP,
    DISPLAYS_MANUALLY_POSITION_FLAG_BRIDGE_SKIP,
    DISPLAYS_MANUALLY_POSITION_FLAG_BLIMP_TICKET_SKIP,

    DISPLAYS_MANUALLY_POSITION_FLAG_MAX_VALUE, // Don't use this directly other than for defines
};

#define TOTAL_DISPLAYS_MANUALLY_POSITION_FLAGS DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_MAX_VALUE
#define DISPLAYS_MANUALLY_POSITION_FLAGS_ARRAY_SIZE intCeil(TOTAL_DISPLAYS_MANUALLY_POSITION_FLAGS, sizeof(uint32_t) * 8)

// The proper order that the manually position flags appear in the settings file. Needed to properly restore them from the
// memory card.
enum DisplaysManuallyPositionFlagOrder
{
    DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_ONSCREEN_TIMER = 0,
    DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_FRAME_COUNTER,
    DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_MARIO_COORDINATES,
    DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_MARIO_SPEED_X_Z,
    DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_JUMP_STORAGE,
    DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_BUTTON_INPUTS,
    DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_STICK_ANGLE,
    DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_GUARD_SUPERGUARD_TIMINGS,
    DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_MEMORY_USAGE,
    DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_EFFS_ACTIVE,
    DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_EVTS_ACTIVE,
    DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_ENEMY_ENCOUNTER_NOTIFIER,

    DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_YOSHI_SKIP,
    DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_PALACE_SKIP,
    DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_PALACE_SKIP_MINIMAL,
    DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_JABBI_HIVE_SKIP,
    DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_BRIDGE_SKIP,
    DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_BLIMP_TICKET_SKIP,

#ifdef TTYD_JP
    DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_AMW_SPIN_JUMP,
#endif

    DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_MAX_VALUE, // Don't use this directly other than for defines
};

// Misc flags are mainly for things that will not be saved to the settings file
enum DisplaysMiscFlag
{
    DISPLAYS_MISC_FLAG_ONSCREEN_TIMER_PAUSED = 0, // On-Screen Timer
    DISPLAYS_MISC_FLAG_FRAME_COUNTER_PAUSED,      // Frame Counter

    DISPLAYS_MISC_FLAG_YOSHI_SKIP_TIMER_STOPPED, // Yoshi Skip
    DISPLAYS_MISC_FLAG_YOSHI_SKIP_TIMER_PAUSED,  // Yoshi Skip

    DISPLAYS_MISC_FLAG_PALACE_SKIP_SHOULD_DRAW_MINIMAL, // Palace Skip, Palace Skip Minimal
    DISPLAYS_MISC_FLAG_PALACE_SKIP_TIMER_STOPPED,       // Palace Skip, Palace Skip Minimal
    DISPLAYS_MISC_FLAG_PALACE_SKIP_TIMER_PAUSED,        // Palace Skip, Palace Skip Minimal

#ifdef TTYD_JP
    DISPLAYS_MISC_FLAG_AMW_SPIN_JUMP_PAUSE_TIMER_STOPPED, // AMW - Spin Jump method
    DISPLAYS_MISC_FLAG_AMW_SPIN_JUMP_PAUSE_TIMER_PAUSED,  // AMW - Spin Jump method

    DISPLAYS_MISC_FLAG_AMW_SPIN_JUMP_SPIN_JUMP_TIMER_STOPPED, // AMW - Spin Jump method
    DISPLAYS_MISC_FLAG_AMW_SPIN_JUMP_SPIN_JUMP_TIMER_PAUSED,  // AMW - Spin Jump method
#endif

    DISPLAYS_MISC_FLAG_JABBI_HIVE_SKIP_INITIAL_OPEN_PAUSE_MENU_BUTTON_PRESSED, // Jabbi Hive Skip; either D-Pad Left or A was
                                                                               // pressed for opening the pause menu

    DISPLAYS_MISC_FLAG_JABBI_HIVE_SKIP_INITIAL_OPEN_PAUSE_MENU_FRAMES_STOPPED, // Jabbi Hive Skip
    DISPLAYS_MISC_FLAG_JABBI_HIVE_SKIP_PAUSE_MENU_OPEN_FRAMES_STOPPED,         // Jabbi Hive Skip

    DISPLAYS_MISC_FLAG_BRIDGE_SKIP_PRESSED_EARLY,                // Bridge Skip
    DISPLAYS_MISC_FLAG_BRIDGE_SKIP_PRESSED_EARLY_SHOULD_DISABLE, // Bridge Skip
    DISPLAYS_MISC_FLAG_BRIDGE_SKIP_TIMER_STOPPED,                // Bridge Skip
    DISPLAYS_MISC_FLAG_BRIDGE_SKIP_TIMER_PAUSED,                 // Bridge Skip

    DISPLAYS_MISC_FLAG_BLIMP_SKIP_TIMERS_PAUSED,             // Blimp Ticket Skip
    DISPLAYS_MISC_FLAG_BLIMP_SKIP_UP_RIGHT_TIMER_STOPPED,    // Blimp Ticket Skip
    DISPLAYS_MISC_FLAG_BLIMP_SKIP_STRAIGHT_UP_TIMER_STOPPED, // Blimp Ticket Skip

    DISPLAYS_MISC_FLAG_MAX_VALUE, // Don't use this directly other than for defines
};

#define TOTAL_DISPLAYS_MISC_FLAGS DisplaysMiscFlag::DISPLAYS_MISC_FLAG_MAX_VALUE
#define DISPLAYS_MISC_FLAGS_ARRAY_SIZE intCeil(TOTAL_DISPLAYS_MISC_FLAGS, sizeof(uint32_t) * 8)

// Draw flags are for whether the indicated display should be drawn or not on the current frame. This does not include displays
// that are handled via other function hooks, such as Art Attack Hitboxes. The order here is assumed to be the order that the
// displays will be drawn, which is important for their postioning. The enum order here needs to match the gDisplaysDrawFuncs
// function order. These will not be saved to the settings file.
enum DisplaysShouldDrawFlag
{
    // Bottom-left positioning
    DISPLAYS_SHOULD_DRAW_FLAG_BUTTON_INPUTS = 0,
    DISPLAYS_SHOULD_DRAW_FLAG_MARIO_COORDINATES,
    DISPLAYS_SHOULD_DRAW_FLAG_MARIO_SPEED_X_Z,
    DISPLAYS_SHOULD_DRAW_FLAG_STICK_ANGLE,

    // Bottom-right positioning
    DISPLAYS_SHOULD_DRAW_FLAG_ONSCREEN_TIMER,
    DISPLAYS_SHOULD_DRAW_FLAG_FRAME_COUNTER,

    // Top-left positioning
    DISPLAYS_SHOULD_DRAW_FLAG_MEMORY_USAGE,
    DISPLAYS_SHOULD_DRAW_FLAG_EFFS_ACTIVE,
    DISPLAYS_SHOULD_DRAW_FLAG_EVTS_ACTIVE,
    DISPLAYS_SHOULD_DRAW_FLAG_ENEMY_ENCOUNTER_NOTIFIER,

    // Top-right positioning
    DISPLAYS_SHOULD_DRAW_FLAG_JUMP_STORAGE,

    // All of the tricks should be handled last, as they may clear other draw flags
    DISPLAYS_SHOULD_DRAW_FLAG_YOSHI_SKIP,
    DISPLAYS_SHOULD_DRAW_FLAG_PALACE_SKIP, // Minimal also uses this

#ifdef TTYD_JP
    DISPLAYS_SHOULD_DRAW_FLAG_AMW_SPIN_JUMP,
#endif

    DISPLAYS_SHOULD_DRAW_FLAG_JABBI_HIVE_SKIP,
    DISPLAYS_SHOULD_DRAW_FLAG_BRIDGE_SKIP,
    DISPLAYS_SHOULD_DRAW_FLAG_BLIMP_TICKET_SKIP,

    DISPLAYS_SHOULD_DRAW_FLAG_MAX_VALUE, // Don't use this directly other than for defines
};

#define TOTAL_DISPLAYS_SHOULD_DRAW_FLAGS DisplaysShouldDrawFlag::DISPLAYS_SHOULD_DRAW_FLAG_MAX_VALUE
#define DISPLAYS_SHOULD_DRAW_ARRAY_SIZE intCeil(TOTAL_DISPLAYS_SHOULD_DRAW_FLAGS, sizeof(uint32_t) * 8)

enum DisplaysWithButtonCombo
{
    DISPLAYS_BUTTON_COMBO_ONSCREEN_TIMER_START_PAUSE_RESUME = 0,
    DISPLAYS_BUTTON_COMBO_ONSCREEN_TIMER_RESET,
    DISPLAYS_BUTTON_COMBO_FRAME_COUNTER_START_PAUSE_RESUME,
    DISPLAYS_BUTTON_COMBO_FRAME_COUNTER_RESET,
    DISPLAYS_BUTTON_COMBO_JUMP_STORAGE,
    DISPLAYS_BUTTON_COMBO_ART_ATTACK_HITBOXES,
    DISPLAYS_BUTTON_COMBO_YOSHI_SKIP,
    DISPLAYS_BUTTON_COMBO_PALACE_SKIP,
    DISPLAYS_BUTTON_COMBO_PALACE_SKIP_MINIMAL,

    DISPLAYS_BUTTON_COMBO_JABBI_HIVE_SKIP,
    DISPLAYS_BUTTON_COMBO_BRIDGE_SKIP,
    DISPLAYS_BUTTON_COMBO_BLIMP_TICKET_SKIP,

#ifdef TTYD_JP
    DISPLAYS_BUTTON_COMBO_AMW_SPIN_JUMP,
#endif

    DISPLAYS_BUTTON_COMBO_MAX_VALUE, // Don't use this directly other than for defines
};

#define TOTAL_DISPLAYS_BUTTON_COMBOS DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_MAX_VALUE

// This class needs to be packed in order for it to work properly in the settings
class DisplayManuallyPosition
{
   public:
    DisplayManuallyPosition() {}
    ~DisplayManuallyPosition() {}

    float getPosX() const { return this->posX; }
    float *getPosXPtr() { return &this->posX; }
    void setPosX(float pos) { this->posX = pos; }

    float getPosY() const { return this->posY; }
    float *getPosYPtr() { return &this->posY; }
    void setPosY(float pos) { this->posY = pos; }

    float getScale() const { return this->scale; }
    float *getScalePtr() { return &this->scale; }
    void setScale(float scale) { this->scale = scale; }

   private:
    float posX;
    float posY;
    float scale;
} __attribute__((__packed__));

static_assert(sizeof(DisplayManuallyPosition) == 0xC);

class OnScreenTimerDisplay
{
   public:
    OnScreenTimerDisplay() {}
    ~OnScreenTimerDisplay() {}

    OSTime getMainTimer() const { return this->mainTimer; }
    void setMainTimer(OSTime time) { this->mainTimer = time; }

    OSTime getPrevFrameTime() const { return this->prevFrameTime; }
    void setPrevFrameTime(OSTime time) { this->prevFrameTime = time; }

   private:
    OSTime mainTimer;
    OSTime prevFrameTime;
};

class FrameCounterDisplay
{
   public:
    FrameCounterDisplay() {}
    ~FrameCounterDisplay() {}

    uint32_t getTimer() const { return this->timer; }
    void setTimer(uint32_t time) { this->timer = time; }

   private:
    uint32_t timer;
};

#define MEMORY_USAGE_BUFFER_SINGLE_LINE 64
#define MEMORY_USAGE_HEAP_CORRUPTION_BUFFER_SIZE (MEMORY_USAGE_BUFFER_SINGLE_LINE * DISPLAYS_TOTAL_HEAPS)
#define MEMORY_USAGE_TOTAL_MEMORY_USAGE_ENTRIES ((DISPLAYS_TOTAL_MAIN_HEAPS * 2) + DISPLAYS_TOTAL_EXTRA_HEAPS)

class MemoryUsageDisplay
{
   public:
    MemoryUsageDisplay() {}

    void freeMemoryUsageBuffer()
    {
        char *buf = this->memoryUsageBuffer;
        if (buf)
        {
            delete[] buf;
            this->memoryUsageBuffer = nullptr;
        }
    }

    void freeHeapCorruptionBuffer()
    {
        char *buf = this->heapCorruptionBuffer;
        if (buf)
        {
            delete[] buf;
            this->heapCorruptionBuffer = nullptr;
        }
    }

    ~MemoryUsageDisplay()
    {
        this->freeMemoryUsageBuffer();
        this->freeHeapCorruptionBuffer();
    }

    char *getMemoryUsageBufferPtr() { return this->memoryUsageBuffer; }
    bool shouldDrawMemoryUsageBuffer() const { return this->memoryUsageBuffer != nullptr; }

    char *initMemoryUsageBuffer()
    {
        char *buf = this->memoryUsageBuffer;
        if (!buf)
        {
            buf = new char[MEMORY_USAGE_BUFFER_SINGLE_LINE * MEMORY_USAGE_TOTAL_MEMORY_USAGE_ENTRIES];
            this->memoryUsageBuffer = buf;
        }

        return buf;
    }

    char *initMemoryUsageBufferEntry(uint32_t slot)
    {
        char *buf = this->initMemoryUsageBuffer();
        return &buf[MEMORY_USAGE_BUFFER_SINGLE_LINE * slot];
    }

    char *getHeapCorruptionBufferPtr() { return this->heapCorruptionBuffer; }
    bool shouldDrawHeapCorruptionBuffer() const { return this->heapCorruptionBuffer != nullptr; }

    char *initHeapCorruptionBuffer()
    {
        char *buf = this->heapCorruptionBuffer;
        if (!buf)
        {
            buf = new char[MEMORY_USAGE_HEAP_CORRUPTION_BUFFER_SIZE];
            this->heapCorruptionBuffer = buf;
        }

        return buf;
    }

    char *initHeapCorruptionBufferEntry(uint32_t index)
    {
        char *buf = this->initHeapCorruptionBuffer();
        return &buf[index];
    }

    int32_t getHeapCorruptionBufferIndex() const { return this->heapCorruptioBufferIndex; }
    void setHeapCorruptionBufferIndex(int32_t index) { this->heapCorruptioBufferIndex = static_cast<int16_t>(index); }

    bool verifyHeapCorruptionBufferIndex() const
    {
        const int32_t index = this->heapCorruptioBufferIndex;
        return (index >= 0) && (index < MEMORY_USAGE_HEAP_CORRUPTION_BUFFER_SIZE);
    }

   private:
    char *memoryUsageBuffer;
    char *heapCorruptionBuffer;
    int16_t heapCorruptioBufferIndex;
};

class EnemyEncounterNotifierDisplay
{
   public:
    EnemyEncounterNotifierDisplay() {}
    ~EnemyEncounterNotifierDisplay() {}

    uint32_t getCounter() const { return this->counter; }
    void resetCounter() { this->counter = 0; }
    void incrementCounter() { this->counter++; }

    uint32_t getTimer() const { return this->timer; }
    void setTimer(uint32_t time) { this->timer = static_cast<uint16_t>(time); }

   private:
    uint16_t counter;
    uint16_t timer;
};

class HitCheckResult
{
   public:
    HitCheckResult() {}
    ~HitCheckResult() {}

    const Vec3 *getStartPosPtr() const { return &this->startPos; }
    void copyPosToStartPos(const Vec3 *posPtr) { memcpy(&this->startPos, posPtr, sizeof(Vec3)); }

    const Vec3 *getEndPosPtr() const { return &this->endPos; }
    void copyPosToEndPos(const Vec3 *posPtr) { memcpy(&this->endPos, posPtr, sizeof(Vec3)); }

    bool checkIfHit() const { return this->hit; }
    void setHit(bool hit) { this->hit = hit; }

   private:
    Vec3 startPos;
    Vec3 endPos;
    bool hit;
    uint8_t padding[3];
};

#define HIT_CHECK_BUFFER_CAPACITY 256
class HitCheckVisualizationDisplay
{
   public:
    HitCheckVisualizationDisplay()
    {
        this->hitsColor = 0xA000D0E0;   // Purple
        this->missesColor = 0x00D000E0; // Green
    }

    void freeBuffer()
    {
        HitCheckResult *buf = this->buffer;
        if (buf)
        {
            delete[] buf;
            this->buffer = nullptr;
        }
    }

    ~HitCheckVisualizationDisplay() { this->freeBuffer(); }

    HitCheckResult *initBufferPtr()
    {
        HitCheckResult *buf = this->buffer;
        if (!buf)
        {
            buf = new HitCheckResult[HIT_CHECK_BUFFER_CAPACITY];
            this->buffer = buf;
        }

        return buf;
    }

    HitCheckResult *getBufferSlotPtr(uint32_t slot)
    {
        // Make sure the slot is valid
        if (slot >= HIT_CHECK_BUFFER_CAPACITY)
        {
            return nullptr;
        }

        HitCheckResult *buf = this->initBufferPtr();
        return &buf[slot];
    }

    HitCheckResult *getBufferPtr() const { return this->buffer; }

    uint32_t getHitsColor() const { return this->hitsColor; }
    uint8_t *getHitsColorPtr() { return reinterpret_cast<uint8_t *>(&this->hitsColor); }
    void setHitsColor(uint32_t color) { this->hitsColor = color; }

    uint32_t getMissesColor() const { return this->missesColor; }
    uint8_t *getMissesColorPtr() { return reinterpret_cast<uint8_t *>(&this->missesColor); }
    void setMissesColor(uint32_t color) { this->missesColor = color; }

    uint32_t getEntryCount() const { return this->entryCount; }
    void setEntryCount(uint32_t count) { this->entryCount = static_cast<uint16_t>(count); }

   private:
    HitCheckResult *buffer;
    uint32_t hitsColor;
    uint32_t missesColor;
    uint16_t entryCount;
};

class JumpStorageDisplay
{
   public:
    JumpStorageDisplay() {}
    ~JumpStorageDisplay() {}

    bool shouldSetFlag() const { return this->counter >= 3; }
    void incrementCounter() { this->counter++; }
    void resetCounter() { this->counter = 0; }

   private:
    uint16_t counter;
};

class GuardSuperguardTimingDisplay
{
   public:
    enum class DefenceResult : uint8_t
    {
        DEFENCE_RESULT_NONE = 0,
        DEFENCE_RESULT_SUCCESSFUL_TIMING,
        DEFENCE_RESULT_PRESSED_TOO_MANY_BUTTONS,
        DEFENCE_RESULT_PRESSED_TOO_EARLY,
        DEFENCE_RESULT_CANNOT_BE_SUPERGUARD,
    };

    GuardSuperguardTimingDisplay() {}
    ~GuardSuperguardTimingDisplay() {}

    uint32_t getTimer() const { return this->timer; }
    void setTimer(uint32_t time) { this->timer = static_cast<uint16_t>(time); }

    void decrementTimer()
    {
        const uint32_t timer = this->timer;
        if (timer > 0)
        {
            this->timer = timer - 1;
        }
    }

    int32_t getLastAFrame() const { return this->lastAFrame; }
    void setLastAFrame(int32_t frame) { this->lastAFrame = static_cast<int8_t>(frame); }

    int32_t getLastBFrame() const { return this->lastBFrame; }
    void setLastBFrame(int32_t frame) { this->lastBFrame = static_cast<int8_t>(frame); }

    GuardSuperguardTimingDisplay::DefenceResult getType() const { return this->type; }
    void setType(GuardSuperguardTimingDisplay::DefenceResult type) { this->type = type; }

    void resetValues()
    {
        this->type = GuardSuperguardTimingDisplay::DefenceResult::DEFENCE_RESULT_NONE;
        this->lastAFrame = -1;
        this->lastBFrame = -1;
    }

   private:
    uint16_t timer;
    GuardSuperguardTimingDisplay::DefenceResult type;
    int8_t lastAFrame;
    int8_t lastBFrame;
};

class YoshiSkipDisplay
{
   public:
    YoshiSkipDisplay() {}
    ~YoshiSkipDisplay() {}

    uint32_t getTimer() const { return this->timer; }
    void setTimer(uint32_t time) { this->timer = static_cast<uint16_t>(time); }
    void resetTimer() { this->timer = 0; }

    uint32_t getCounter() const { return this->counter; }
    void setCounter(uint32_t count) { this->counter = static_cast<uint16_t>(count); }
    void resetCounter() { this->counter = 0; }

   private:
    uint16_t timer;
    uint16_t counter;
};

class PalaceSkipDisplay
{
   public:
    PalaceSkipDisplay() {}
    ~PalaceSkipDisplay() {}

    uint32_t getTimer() const { return this->timer; }
    void setTimer(uint32_t time) { this->timer = static_cast<uint16_t>(time); }
    void resetTimer() { this->timer = 0; }

    uint32_t getCounter() const { return this->counter; }
    void setCounter(uint32_t count) { this->counter = static_cast<uint16_t>(count); }
    void resetCounter() { this->counter = 0; }

   private:
    uint16_t timer;
    uint16_t counter;
};

#ifdef TTYD_JP
class AMWSpinJumpDisplay
{
   public:
    AMWSpinJumpDisplay() {}
    ~AMWSpinJumpDisplay() {}

    uint32_t getPauseTimer() const { return this->pauseTimer; }
    void setPauseTimer(uint32_t time) { this->pauseTimer = static_cast<uint16_t>(time); }
    void resetPauseTimer() { this->pauseTimer = 0; }

    uint32_t getSpinJumpTimer() const { return this->spinJumpTimer; }
    void setSpinJumpTimer(uint32_t time) { this->spinJumpTimer = static_cast<uint16_t>(time); }
    void resetSpinJumpTimer() { this->spinJumpTimer = 0; }

    uint32_t getCounter() const { return this->counter; }
    void setCounter(uint32_t count) { this->counter = static_cast<uint16_t>(count); }
    void resetCounter() { this->counter = 0; }

    void resetTimers()
    {
        this->resetPauseTimer();
        this->resetSpinJumpTimer();
    }

   private:
    uint16_t pauseTimer;
    uint16_t spinJumpTimer;
    uint16_t counter;
};
#endif

class JabbiHiveSkipDisplay
{
   public:
    JabbiHiveSkipDisplay() {}
    ~JabbiHiveSkipDisplay() {}

    uint32_t getInitialOpenPauseMenuFrames() const { return this->initialOpenPauseMenuFrames; }
    void incrementInitialOpenPauseMenuFrames() { this->initialOpenPauseMenuFrames++; }

    uint32_t getPauseMenuOpenFrames() const { return this->pauseMenuOpenFrames; }
    void incrementPauseMenuOpenFrames() { this->pauseMenuOpenFrames++; }

    uint32_t getButtonPressedFirst() const { return this->buttonPressedFirst; }
    void setButtonPressedFirst(uint32_t button) { this->buttonPressedFirst = static_cast<uint16_t>(button); }

    uint32_t getFlagsSetTimer() const { return this->flagsSetTimer; }
    void setFlagsSetTimer(uint32_t time) { this->flagsSetTimer = static_cast<uint8_t>(time); }

    bool shouldResetEverything() const { return this->counter >= 3; }
    void incrementCounter() { this->counter++; }
    void resetCounter() { this->counter = 0; }

   private:
    uint16_t initialOpenPauseMenuFrames; // Frames between pressing D-Pad Left and A when opening the pause menu
    uint16_t pauseMenuOpenFrames;        // How long the pause menu is open for, between the two A presses

    uint16_t buttonPressedFirst; // Which button was pressed first when opening the pause menu. Will be 0 when pressed at the
                                 // same time.

    uint8_t flagsSetTimer; // Starts counting once DISPLAYS_MISC_FLAG_JABBI_HIVE_SKIP_INITIAL_OPEN_PAUSE_MENU_FRAMES_STOPPED and
                           // DISPLAYS_MISC_FLAG_JABBI_HIVE_SKIP_PAUSE_MENU_OPEN_FRAMES_STOPPED are both set, and is used to
                           // prevent clearing everything until an amount of frames have passed after the 2nd A press

    uint8_t counter; // Reset counter
};

class BridgeSkipDisplay
{
   public:
    BridgeSkipDisplay() {}
    ~BridgeSkipDisplay() {}

    uint32_t getTimer() const { return this->timer; }
    void setTimer(uint32_t time) { this->timer = static_cast<uint16_t>(time); }
    void resetTimer() { this->timer = 0; }

    uint32_t getCounter() const { return this->counter; }
    void setCounter(uint32_t count) { this->counter = static_cast<uint16_t>(count); }
    void resetCounter() { this->counter = 0; }

   private:
    uint16_t timer;
    uint16_t counter;
};

class BlimpTicketSkipDisplay
{
   public:
    BlimpTicketSkipDisplay() {}
    ~BlimpTicketSkipDisplay() {}

    uint32_t getUpRightTimer() const { return this->upRightTimer; }
    void setUpRightTimer(uint32_t time) { this->upRightTimer = static_cast<uint16_t>(time); }
    void resetUpRightTimer() { this->upRightTimer = 0; }

    uint32_t getStraightUpTimer() const { return this->straightUpTimer; }
    void setStraightUpTimer(uint32_t time) { this->straightUpTimer = static_cast<uint16_t>(time); }
    void resetStraightUpTimer() { this->straightUpTimer = 0; }

    uint32_t getCounter() const { return this->counter; }
    void setCounter(uint32_t count) { this->counter = static_cast<uint16_t>(count); }
    void resetCounter() { this->counter = 0; }

   private:
    uint16_t upRightTimer;
    uint16_t straightUpTimer;
    uint16_t counter;
};

class NpcNameToPtrErrorDisplay
{
   public:
    NpcNameToPtrErrorDisplay() {}
    ~NpcNameToPtrErrorDisplay() {}

    uint32_t getCounter() const { return this->counter; }
    void incrementCounter() { this->counter++; }
    void resetCounter() { this->counter = 0; }

    uint32_t getTimer() const { return this->timer; }
    void setTimer(uint32_t time) { this->timer = static_cast<uint16_t>(time); }

   private:
    uint16_t counter;
    uint16_t timer;
};

class AnimPoseMainErrorDisplay
{
   public:
    AnimPoseMainErrorDisplay() {}
    ~AnimPoseMainErrorDisplay() {}

    uint32_t getCounter() const { return this->counter; }
    void incrementCounter() { this->counter++; }
    void resetCounter() { this->counter = 0; }

    uint32_t getTimer() const { return this->timer; }
    void setTimer(uint32_t time) { this->timer = static_cast<uint16_t>(time); }

   private:
    uint16_t counter;
    uint16_t timer;
};

class Displays
{
   public:
    Displays();
    ~Displays() {}

    bool checkDisplayButtonCombo(uint32_t cheatWithCombo) const;
    bool checkDisplayButtonComboEveryFrame(uint32_t cheatWithCombo) const;
    uint32_t getDisplayButtonCombo(uint32_t cheatWithCombo);
    void setDisplayButtonCombo(uint32_t cheatWithCombo, uint32_t buttonCombo);
    uint16_t *getButtonCombosPtr() { return this->buttonCombos; }

    bool enabledFlagIsSet(uint32_t enabledFlag) const;
    void setEnabledFlag(uint32_t enabledFlag);
    void clearEnabledFlag(uint32_t enabledFlag);
    bool toggleEnabledFlag(uint32_t enabledFlag);
    const uint32_t *getEnabledFlagsPtr() const { return this->enabledFlags; }

    void handleEnablingTrickDisplayFlag(uint32_t enabledFlag);
    bool anyHeapDisplayIsEnabled();

    bool manuallyPositionFlagIsSet(uint32_t manuallyPositionFlag) const;
    void setManuallyPositionFlag(uint32_t manuallyPositionFlag);
    void clearManuallyPositionFlag(uint32_t manuallyPositionFlag);
    bool toggleManuallyPositionFlag(uint32_t manuallyPositionFlag);
    const uint32_t *getManuallyPositionFlagsPtr() const { return this->manuallyPositionFlags; }

    bool miscFlagIsSet(uint32_t miscFlag) const;
    void setMiscFlag(uint32_t miscFlag);
    void clearMiscFlag(uint32_t miscFlag);
    bool toggleMiscFlag(uint32_t miscFlag);

    bool shouldDrawFlagIsSet(uint32_t shouldDrawFlag) const;
    void setShouldDrawFlag(uint32_t shouldDrawFlag);
    void clearShouldDrawFlag(uint32_t shouldDrawFlag);
    bool anyShouldDrawFlagIsSet() const;
    void clearAllShouldDrawFlags();

    void handleStandardHeapChunkResults(const void *addressWithError,
                                        const ChunkInfo *chunk,
                                        const HeapInfo *heap,
                                        int32_t heapIndex,
                                        uint32_t memoryUsageBufferIndex,
                                        uint32_t enabledFlag,
                                        bool isUsedPortion);

    void handleSmartHeapChunkResults(const void *addressWithError,
                                     const SmartAllocationData *chunk,
                                     uint32_t memoryUsageBufferIndex,
                                     uint32_t enabledFlag,
                                     bool isUsedPortion);

#ifdef TTYD_JP
    void handleMapHeapChunkResults(const void *addressWithError,
                                   const MapAllocEntry *chunk,
                                   uint32_t memoryUsageBufferIndex,
                                   uint32_t enabledFlag);
#else
    void handleMapHeapChunkResults(const void *addressWithError,
                                   const MapAllocEntry *chunk,
                                   uint32_t memoryUsageBufferIndex,
                                   uint32_t enabledFlag,
                                   bool isBattleHeap);
#endif

    bool displayShouldBeHandled(uint32_t enabledFlag) const;
    DisplayManuallyPosition *getDisplayManuallyPositionPtr(uint32_t manuallyPositionFlag);
    DisplayManuallyPosition *getManuallyPositionEntriesPtr() { return this->manuallyPosition; }

    float getErrorTextPosY() const { return this->defaultPosYErrors; }
    float getErrorTextPosYDecrement();
    float getErrorTextOrder();

    float getDefaultPosBottomLeft() const { return this->defaultPosYBottomLeft; }
    float getDefaultPosBottomRight() const { return this->defaultPosYBottomRight; }
    float getDefaultPosTopLeft() const { return this->defaultPosYTopLeft; }
    float getDefaultPosTopRight() const { return this->defaultPosYTopRight; }

    void setDefaultPosBottomLeft(float pos) { this->defaultPosYBottomLeft = pos; }
    void setDefaultPosBottomRight(float pos) { this->defaultPosYBottomRight = pos; }
    void setDefaultPosTopLeft(float pos) { this->defaultPosYTopLeft = pos; }
    void setDefaultPosTopRight(float pos) { this->defaultPosYTopRight = pos; }
    void setDefaultPosErrors(float pos) { this->defaultPosYErrors = pos; }
    void setDefaultErrorTextOrder(float order) { this->defaultOrderErrors = order; }

    void incrementDefaultPosTopLeft(float value) { this->defaultPosYTopLeft += value; }
    void decrementDefaultPosTopLeft(float value) { this->defaultPosYTopLeft -= value; }
    void decrementDefaultPosErrorText(float value) { this->defaultPosYErrors -= value; }

    OnScreenTimerDisplay *getOnScreenTimerDisplayPtr() { return &this->onScreenTimer; }
    FrameCounterDisplay *getFrameCounterDisplayPtr() { return &this->frameCounter; }
    GuardSuperguardTimingDisplay *getGuardSuperguardTimingDisplayPtr() { return &this->guardSuperguardTiming; }
    MemoryUsageDisplay *getMemoryUsageDisplayPtr() { return &this->memoryUsage; }
    EnemyEncounterNotifierDisplay *getEnemyEncounterNotifierDisplayPtr() { return &this->enemyEncounterNotifier; }
    HitCheckVisualizationDisplay *getHitCheckVisualizationDisplayPtr() { return &this->hitCheckVisualization; }
    JumpStorageDisplay *getJumpStorageDisplayPtr() { return &this->jumpStorage; }
    YoshiSkipDisplay *getYoshiSkipDisplayPtr() { return &this->yoshiSkip; }
    PalaceSkipDisplay *getPalaceSkipDisplayPtr() { return &this->palaceSkip; }

#ifdef TTYD_JP
    AMWSpinJumpDisplay *getAMWSpinJumpDisplay() { return &this->amwSpinJumpDisplay; }
#endif

    JabbiHiveSkipDisplay *getJabbiHiveSkipDisplayPtr() { return &this->jabbiHiveSkip; }
    BridgeSkipDisplay *getBridgeSkipDisplayPtr() { return &this->bridgeSkip; }
    BlimpTicketSkipDisplay *getBlimpSkipDisplayPtr() { return &this->blimpTicketSkip; }
    NpcNameToPtrErrorDisplay *getNpcNameToPtrErrorDisplayPtr() { return &this->npcNameToPtrError; }
    AnimPoseMainErrorDisplay *getAnimPoseMainErrorDisplayPtr() { return &this->animPoseMainError; }

   private:
    uint32_t enabledFlags[DISPLAYS_ENABLED_FLAGS_ARRAY_SIZE];
    uint32_t manuallyPositionFlags[DISPLAYS_MANUALLY_POSITION_FLAGS_ARRAY_SIZE];
    uint32_t miscFlags[DISPLAYS_MISC_FLAGS_ARRAY_SIZE];
    uint32_t shouldDrawFlags[DISPLAYS_SHOULD_DRAW_ARRAY_SIZE];
    DisplayManuallyPosition manuallyPosition[TOTAL_DISPLAYS_MANUALLY_POSITION_FLAGS];
    uint16_t buttonCombos[TOTAL_DISPLAYS_BUTTON_COMBOS];

    float defaultPosYBottomLeft;
    float defaultPosYTopLeft;
    float defaultPosYTopRight;
    float defaultPosYBottomRight;
    float defaultPosYErrors;
    float defaultOrderErrors;

    OnScreenTimerDisplay onScreenTimer;
    FrameCounterDisplay frameCounter;
    HitCheckVisualizationDisplay hitCheckVisualization;
    JumpStorageDisplay jumpStorage;
    MemoryUsageDisplay memoryUsage;
    GuardSuperguardTimingDisplay guardSuperguardTiming;
    EnemyEncounterNotifierDisplay enemyEncounterNotifier;
    YoshiSkipDisplay yoshiSkip;
    PalaceSkipDisplay palaceSkip;

#ifdef TTYD_JP
    AMWSpinJumpDisplay amwSpinJumpDisplay;
#endif

    JabbiHiveSkipDisplay jabbiHiveSkip;
    BridgeSkipDisplay bridgeSkip;
    BlimpTicketSkipDisplay blimpTicketSkip;
    NpcNameToPtrErrorDisplay npcNameToPtrError;
    AnimPoseMainErrorDisplay animPoseMainError;
};

extern Displays *gDisplays;

const void *checkIndividualStandardHeap(const ChunkInfo *start);
const void *checkIndividualSmartHeap(const SmartAllocationData *start);
const void *checkIndividualMapHeap(const MapAllocEntry *start);
void checkForEnemyEncounters(void *ptr);
int32_t initGuardSuperguardTimings(BattleWorkUnit *battleUnitPtr, BattleWeapon *weapon);
void handleHitCheckVisualization();
NpcEntry *checkForNpcNameToPtrError(const char *name);
void preventAnimPoseMainCrash(int32_t poseId);
void drawStarPowerValue();
void drawArtAttackHitboxes(CameraId cameraId, void *user);
uint32_t pauseArtAttackTimer();
HitEntry *checkForVecHits(HitCheckQuery *pQuery, PFN_HitFilterFunction filterFunction);
void drawSequenceInPauseMenu(CameraId cameraId, void *winWorkPtr, int32_t index);
void runDisplayFuncsEveryFrame();

uint32_t convertDisplaysEnabledFlagOrder(uint32_t flagOrder);
uint32_t convertDisplaysManuallyPositionFlagOrder(uint32_t flagOrder);

#endif
