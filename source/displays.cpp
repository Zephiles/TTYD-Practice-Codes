#include "displays.h"
#include "drawText.h"
#include "mod.h"
#include "menuUtils.h"
#include "assembly.h"
#include "memoryEditor.h"
#include "classes/positionEditor.h"
#include "classes/valueEditor.h"
#include "classes/window.h"
#include "gc/pad.h"
#include "gc/types.h"
#include "gc/OSAlloc.h"
#include "gc/OSTime.h"
#include "gc/mtx.h"
#include "gc/gx.h"
#include "menus/displaysMenu.h"
#include "menus/battlesMenu.h"
#include "misc/utils.h"
#include "misc/functionHooks.h"
#include "ttyd/memory.h"
#include "ttyd/camdrv.h"
#include "ttyd/dispdrv.h"
#include "ttyd/mariost.h"
#include "ttyd/system.h"
#include "ttyd/mario.h"
#include "ttyd/effdrv.h"
#include "ttyd/evtmgr.h"
#include "ttyd/battle.h"
#include "ttyd/battle_unit.h"
#include "ttyd/battle_database_common.h"
#include "ttyd/battle_ac.h"
#include "ttyd/mario_pouch.h"
#include "ttyd/hitdrv.h"
#include "ttyd/party.h"
#include "ttyd/npcdrv.h"
#include "ttyd/itemdrv.h"
#include "ttyd/item_data.h"
#include "ttyd/mario_motion.h"
#include "ttyd/statuswindow.h"
#include "ttyd/icondrv.h"
#include "ttyd/battle_disp.h"
#include "ttyd/sac_scissor.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

Displays *gDisplays = nullptr;

bool Displays::checkDisplayButtonCombo(uint32_t displayWithCombo) const
{
    // Make sure displayWithCombo is valid
    if (displayWithCombo >= TOTAL_DISPLAYS_BUTTON_COMBOS)
    {
        return false;
    }

    return checkButtonCombo(this->buttonCombos[displayWithCombo]);
}

bool Displays::checkDisplayButtonComboEveryFrame(uint32_t displayWithCombo) const
{
    // Make sure displayWithCombo is valid
    if (displayWithCombo >= TOTAL_DISPLAYS_BUTTON_COMBOS)
    {
        return false;
    }

    return checkButtonComboEveryFrame(this->buttonCombos[displayWithCombo]);
}

uint32_t Displays::getDisplayButtonCombo(uint32_t displayWithCombo)
{
    // Make sure displayWithCombo is valid
    if (displayWithCombo >= TOTAL_DISPLAYS_BUTTON_COMBOS)
    {
        return 0;
    }

    return this->buttonCombos[displayWithCombo];
}

void Displays::setDisplayButtonCombo(uint32_t displayWithCombo, uint32_t buttonCombo)
{
    // Make sure displayWithCombo is valid
    if (displayWithCombo >= TOTAL_DISPLAYS_BUTTON_COMBOS)
    {
        return;
    }

    this->buttonCombos[displayWithCombo] = static_cast<uint16_t>(buttonCombo);
}

void Displays::handleEnablingTrickDisplayFlag(uint32_t enabledFlag)
{
    // Only one trick display should be drawn at a time, so handle disabling other flags if a trick display was just enabled
    // Make sure the enabled flag is for one of the tricks
    switch (enabledFlag)
    {
        case DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_YOSHI_SKIP:
        case DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_PALACE_SKIP:
        case DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_PALACE_SKIP_MINIMAL:

#ifdef TTYD_JP
        case DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_AMW_SPIN_JUMP:
#endif

        case DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_JABBI_HIVE_SKIP:
        case DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_BRIDGE_SKIP:
        case DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_BLIMP_TICKET_SKIP:
        {
            // Disable all of the trick display flags
            for (uint32_t i = DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_YOSHI_SKIP;
                 i <= DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_BLIMP_TICKET_SKIP;
                 i++)
            {
                this->clearEnabledFlag(i);
            }

            // Enable the current trick display flag
            this->setEnabledFlag(enabledFlag);
        }
        default:
        {
            break;
        }
    }
}

bool Displays::anyHeapDisplayIsEnabled()
{
    for (uint32_t i = 0; i < DISPLAYS_TOTAL_HEAPS; i++)
    {
        if (this->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_HEAP_0 + i))
        {
            return true;
        }
    }

    return false;
}

bool Displays::anyShouldDrawFlagIsSet() const
{
    const uint32_t *flagsPtr = this->shouldDrawFlags;
    for (uint32_t i = 0; i < DISPLAYS_SHOULD_DRAW_ARRAY_SIZE; i++)
    {
        if (flagsPtr[i])
        {
            return true;
        }
    }
    return false;
}

void Displays::clearAllShouldDrawFlags()
{
    uint32_t *flagsPtr = this->shouldDrawFlags;
    for (uint32_t i = 0; i < DISPLAYS_SHOULD_DRAW_ARRAY_SIZE; i++)
    {
        flagsPtr[i] = 0;
    }
}

static bool checkIfAdjustingManualPositionOrScale(uint32_t enabledFlag)
{
    // If the Displays menu is open but hidden, then the display may need to be handled
    DisplaysMenu *displaysMenuPtr = gDisplaysMenu;

    if (displaysMenuPtr && gMenu && gMod->flagIsSet(ModFlag::MOD_FLAG_MENU_IS_HIDDEN))
    {
        // If the selected display in the menu is for the current display, then assume that the player is currently manually
        // positioning the display
        return indexToDisplayEnabledFlag(displaysMenuPtr->getSelectedDisplay()) == enabledFlag;
    }

    return false;
}

bool Displays::displayShouldBeHandled(uint32_t enabledFlag) const
{
    // If the enabled flag is set, then the display should be handled
    if (this->enabledFlagIsSet(enabledFlag))
    {
        return true;
    }

    // Check if the player is currently adjusting the position or scale of the display
    return checkIfAdjustingManualPositionOrScale(enabledFlag);
}

DisplayManuallyPosition *Displays::getDisplayManuallyPositionPtr(uint32_t manuallyPositionFlag)
{
    // Make sure the flag is valid
    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
    constexpr uint32_t maxFlags = DISPLAYS_MANUALLY_POSITION_FLAGS_ARRAY_SIZE * bitsPerWord;

    if (manuallyPositionFlag >= maxFlags)
    {
        return nullptr;
    }

    // Make sure the flag does not exceed TOTAL_DISPLAYS_MANUALLY_POSITION_FLAGS
    if (manuallyPositionFlag >= TOTAL_DISPLAYS_MANUALLY_POSITION_FLAGS)
    {
        return nullptr;
    }

    return &this->manuallyPosition[manuallyPositionFlag];
}

float Displays::getErrorTextPosYDecrement()
{
    const float posY = this->defaultPosYErrors;

    // Move down to the next line
    this->defaultPosYErrors = posY - (LINE_HEIGHT_FLOAT * DISPLAYS_DEFAULT_SCALE);

    return posY;
}

float Displays::getErrorTextOrder()
{
    const float order = this->defaultOrderErrors;

    // Add one for each set of error text to keep the order consistent, as otherwise their positioning would be somewhat random
    this->defaultOrderErrors = order + 1.f;
    return order;
}

Displays::Displays()
{
    // Set default displays enabled
    // this->setEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_ONSCREEN_TIMER);
    // this->setEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_FRAME_COUNTER);
    // this->setEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MARIO_COORDINATES);
    // this->setEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MARIO_SPEED_X_Z);
    // this->setEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_JUMP_STORAGE);
    // this->setEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_BUTTON_INPUTS);
    // this->setEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_STICK_ANGLE);
    // this->setEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_STAR_POWER_VALUE);
    // this->setEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_DPAD_OPTIONS);
    this->setEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_GUARD_SUPERGUARD_TIMINGS);
    // this->setEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_ART_ATTACK_HITBOXES);
    // this->setEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_EFFS_ACTIVE);
    // this->setEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_EVTS_ACTIVE);
    // this->setEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_ENEMY_ENCOUNTER_NOTIFIER);
    // this->setEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_HIT_CHECK_VISUALIZATION);
    this->setEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_SHOULD_DRAW_HITS);
    this->setEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_SHOULD_DRAW_MISSES);
    // this->setEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_YOSHI_SKIP);
    // this->setEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_PALACE_SKIP);
    // this->setEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_PALACE_SKIP_MINIMAL);

#ifdef TTYD_JP
    // this->setEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_AMW_SPIN_JUMP);
#endif

    // this->setEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_JABBI_HIVE_SKIP);
    // this->setEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_BRIDGE_SKIP);
    // this->setEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_BLIMP_TICKET_SKIP);

    // Set default displays misc flags enabled
    this->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_ONSCREEN_TIMER_PAUSED);
    this->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_FRAME_COUNTER_PAUSED);
    this->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_YOSHI_SKIP_TIMER_STOPPED);
    this->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_PALACE_SKIP_TIMER_STOPPED);
    this->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BRIDGE_SKIP_TIMER_STOPPED);

#ifdef TTYD_JP
    this->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_AMW_SPIN_JUMP_PAUSE_TIMER_STOPPED);
    this->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_AMW_SPIN_JUMP_SPIN_JUMP_TIMER_STOPPED);
#endif

    this->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BLIMP_SKIP_UP_RIGHT_TIMER_STOPPED);
    this->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BLIMP_SKIP_STRAIGHT_UP_TIMER_STOPPED);

    // Set default display button combos
    this->setDisplayButtonCombo(DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_ONSCREEN_TIMER_START_PAUSE_RESUME,
                                PadInput::PAD_L | PadInput::PAD_Z);

    this->setDisplayButtonCombo(DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_ONSCREEN_TIMER_RESET,
                                PadInput::PAD_L | PadInput::PAD_DPAD_RIGHT);

    this->setDisplayButtonCombo(DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_FRAME_COUNTER_START_PAUSE_RESUME,
                                PadInput::PAD_L | PadInput::PAD_Z);

    this->setDisplayButtonCombo(DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_FRAME_COUNTER_RESET,
                                PadInput::PAD_L | PadInput::PAD_DPAD_RIGHT);

    this->setDisplayButtonCombo(DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_JUMP_STORAGE, PadInput::PAD_Y);
    this->setDisplayButtonCombo(DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_ART_ATTACK_HITBOXES, PadInput::PAD_Y);
    this->setDisplayButtonCombo(DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_YOSHI_SKIP, PadInput::PAD_B);
    this->setDisplayButtonCombo(DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_PALACE_SKIP, PadInput::PAD_B);
    this->setDisplayButtonCombo(DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_PALACE_SKIP_MINIMAL, PadInput::PAD_B);
    this->setDisplayButtonCombo(DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_JABBI_HIVE_SKIP, PadInput::PAD_B);
    this->setDisplayButtonCombo(DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_BRIDGE_SKIP, PadInput::PAD_B);
    this->setDisplayButtonCombo(DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_BLIMP_TICKET_SKIP, PadInput::PAD_B);

#ifdef TTYD_JP
    this->setDisplayButtonCombo(DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_AMW_SPIN_JUMP, PadInput::PAD_Y);
#endif

    // Set default positions and scales for certain displays when drawing them manually
    DisplayManuallyPosition *manuallyPositionPtrBase = this->getDisplayManuallyPositionPtr(0);
    DisplayManuallyPosition *manuallyPositionPtr;

    float onScreenTimerScale = DISPLAYS_DEFAULT_SCALE + 0.05f;

#ifdef TTYD_JP
    onScreenTimerScale += 0.05f;
#endif

    const float lineDecrement = 20.f; // Rough estimate to stick to whole numbers

    // Set up an arbitrary posY variable to help with setting the positions
    float posY = -158.f;

    // Frame Counter
    manuallyPositionPtr = &manuallyPositionPtrBase[DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_FRAME_COUNTER];
    manuallyPositionPtr->setPosX(128.f); // Rough estimate to stick to whole numbers based on right-aligning
    manuallyPositionPtr->setPosY(posY);
    manuallyPositionPtr->setScale(onScreenTimerScale);

    // On-Screen Timer
    posY += lineDecrement;

    manuallyPositionPtr =
        &manuallyPositionPtrBase[DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_ONSCREEN_TIMER];

    manuallyPositionPtr->setPosX(128.f); // Rough estimate to stick to whole numbers based on right-aligning
    manuallyPositionPtr->setPosY(posY);
    manuallyPositionPtr->setScale(onScreenTimerScale);

    // Button Inputs
    posY = DISPLAYS_DEFAULT_POS_Y_BOTTOM;
    manuallyPositionPtr = &manuallyPositionPtrBase[DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_BUTTON_INPUTS];
    manuallyPositionPtr->setPosX(DISPLAYS_DEFAULT_POS_X_LEFT);
    manuallyPositionPtr->setPosY(posY);
    manuallyPositionPtr->setScale(DISPLAYS_DEFAULT_SCALE);

    // Mario's Coordinates
    posY += lineDecrement;

    manuallyPositionPtr =
        &manuallyPositionPtrBase[DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_MARIO_COORDINATES];

    manuallyPositionPtr->setPosX(DISPLAYS_DEFAULT_POS_X_LEFT);
    manuallyPositionPtr->setPosY(posY);
    manuallyPositionPtr->setScale(DISPLAYS_DEFAULT_SCALE);

    // Mario Speed XZ
    posY += lineDecrement;

    manuallyPositionPtr =
        &manuallyPositionPtrBase[DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_MARIO_SPEED_X_Z];

    manuallyPositionPtr->setPosX(DISPLAYS_DEFAULT_POS_X_LEFT);
    manuallyPositionPtr->setPosY(posY);
    manuallyPositionPtr->setScale(DISPLAYS_DEFAULT_SCALE);

    // Stick Angle
    posY += lineDecrement;
    manuallyPositionPtr = &manuallyPositionPtrBase[DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_STICK_ANGLE];
    manuallyPositionPtr->setPosX(DISPLAYS_DEFAULT_POS_X_LEFT);
    manuallyPositionPtr->setPosY(posY);
    manuallyPositionPtr->setScale(DISPLAYS_DEFAULT_SCALE);

    // Memory Usage
    posY = DISPLAYS_DEFAULT_POS_Y_TOP;
    manuallyPositionPtr = &manuallyPositionPtrBase[DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_MEMORY_USAGE];
    manuallyPositionPtr->setPosX(DISPLAYS_DEFAULT_POS_X_LEFT);
    manuallyPositionPtr->setPosY(posY);
    manuallyPositionPtr->setScale(DISPLAYS_DEFAULT_SCALE);

    // Effs Active
    posY -= lineDecrement;
    manuallyPositionPtr = &manuallyPositionPtrBase[DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_EFFS_ACTIVE];
    manuallyPositionPtr->setPosX(DISPLAYS_DEFAULT_POS_X_LEFT);
    manuallyPositionPtr->setPosY(posY);
    manuallyPositionPtr->setScale(DISPLAYS_DEFAULT_SCALE);

    // Evts Active
    posY -= lineDecrement;
    manuallyPositionPtr = &manuallyPositionPtrBase[DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_EVTS_ACTIVE];
    manuallyPositionPtr->setPosX(DISPLAYS_DEFAULT_POS_X_LEFT);
    manuallyPositionPtr->setPosY(posY);
    manuallyPositionPtr->setScale(DISPLAYS_DEFAULT_SCALE);

    // Enemy Encounter Notifier
    posY -= lineDecrement;

    manuallyPositionPtr =
        &manuallyPositionPtrBase[DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_ENEMY_ENCOUNTER_NOTIFIER];

    manuallyPositionPtr->setPosX(DISPLAYS_DEFAULT_POS_X_LEFT);
    manuallyPositionPtr->setPosY(posY);
    manuallyPositionPtr->setScale(DISPLAYS_DEFAULT_SCALE);

    // Jump Storage
    manuallyPositionPtr = &manuallyPositionPtrBase[DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_JUMP_STORAGE];
    manuallyPositionPtr->setPosX(127.f); // Rough estimate to stick to whole numbers based on right-aligning
    manuallyPositionPtr->setPosY(DISPLAYS_DEFAULT_POS_Y_TOP);
    manuallyPositionPtr->setScale(DISPLAYS_DEFAULT_SCALE);

    // Guard/Superguard Timings
    manuallyPositionPtr =
        &manuallyPositionPtrBase[DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_GUARD_SUPERGUARD_TIMINGS];

    manuallyPositionPtr->setPosX(DISPLAYS_DEFAULT_POS_X_LEFT);
    manuallyPositionPtr->setPosY(-134.f);
    manuallyPositionPtr->setScale(DISPLAYS_DEFAULT_SCALE + 0.05f);

    // Yoshi Skip
    posY = DISPLAYS_DEFAULT_POS_Y_BOTTOM + lineDecrement;
    manuallyPositionPtr = &manuallyPositionPtrBase[DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_YOSHI_SKIP];
    manuallyPositionPtr->setPosX(DISPLAYS_DEFAULT_POS_X_LEFT);
    manuallyPositionPtr->setPosY(posY);
    manuallyPositionPtr->setScale(DISPLAYS_DEFAULT_SCALE);

    // Palace Skip
    manuallyPositionPtr = &manuallyPositionPtrBase[DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_PALACE_SKIP];
    manuallyPositionPtr->setPosX(DISPLAYS_DEFAULT_POS_X_LEFT);
    manuallyPositionPtr->setPosY(posY);
    manuallyPositionPtr->setScale(DISPLAYS_DEFAULT_SCALE);

    // Palace Skip (Minimal)
    manuallyPositionPtr =
        &manuallyPositionPtrBase[DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_PALACE_SKIP_MINIMAL];

    manuallyPositionPtr->setPosX(DISPLAYS_DEFAULT_POS_X_LEFT);
    manuallyPositionPtr->setPosY(posY);
    manuallyPositionPtr->setScale(DISPLAYS_DEFAULT_SCALE);

    // Jabbi Hive Skip
    manuallyPositionPtr =
        &manuallyPositionPtrBase[DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_JABBI_HIVE_SKIP];

    manuallyPositionPtr->setPosX(DISPLAYS_DEFAULT_POS_X_LEFT);
    manuallyPositionPtr->setPosY(posY);
    manuallyPositionPtr->setScale(DISPLAYS_DEFAULT_SCALE);

    // Bridge Skip
    manuallyPositionPtr = &manuallyPositionPtrBase[DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_BRIDGE_SKIP];
    manuallyPositionPtr->setPosX(DISPLAYS_DEFAULT_POS_X_LEFT);
    manuallyPositionPtr->setPosY(posY);
    manuallyPositionPtr->setScale(DISPLAYS_DEFAULT_SCALE);

    // Blimp Ticket Skip
    manuallyPositionPtr =
        &manuallyPositionPtrBase[DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_BLIMP_TICKET_SKIP];

    manuallyPositionPtr->setPosX(DISPLAYS_DEFAULT_POS_X_LEFT);
    manuallyPositionPtr->setPosY(posY);
    manuallyPositionPtr->setScale(DISPLAYS_DEFAULT_SCALE);

    // Initialize the default Y positions and error text draw order
    this->defaultPosYBottomLeft = DISPLAYS_DEFAULT_POS_Y_BOTTOM;
    this->defaultPosYBottomRight = DISPLAYS_DEFAULT_POS_Y_BOTTOM;
    this->defaultPosYTopLeft = DISPLAYS_DEFAULT_POS_Y_TOP;
    this->defaultPosYTopRight = DISPLAYS_DEFAULT_POS_Y_TOP;
    this->defaultPosYErrors = DISPLAYS_DEFAULT_POS_Y_TOP_ERROR_TEXT;
    this->defaultOrderErrors = DRAW_ORDER_DISPLAY_ERRORS;
}

const void *checkIndividualStandardHeap(const ChunkInfo *start)
{
    const ChunkInfo *prevChunk = nullptr;
    for (const ChunkInfo *currentChunk = start; currentChunk; currentChunk = currentChunk->next)
    {
        // Check pointer sanity
        if (!ptrIsValid(const_cast<ChunkInfo *>(currentChunk)))
        {
            return currentChunk;
        }

        // Sanity check size
        if (currentChunk->size >= 0x1800000)
        {
            return currentChunk;
        }

        // Check linked list integrity
        if (prevChunk != currentChunk->prev)
        {
            return currentChunk;
        }

        prevChunk = currentChunk;
    }

    return nullptr;
}

const void *checkIndividualSmartHeap(const SmartAllocationData *start)
{
    const SmartAllocationData *prevChunk = nullptr;
    for (const SmartAllocationData *currentChunk = start; currentChunk; currentChunk = currentChunk->pNext)
    {
        // Check pointer sanity
        if (!ptrIsValid(const_cast<SmartAllocationData *>(currentChunk)))
        {
            return currentChunk;
        }

        // Sanity check size
        if (currentChunk->usedSize >= 0x1800000)
        {
            return currentChunk;
        }

        // Check linked list integrity
        if (prevChunk != currentChunk->pPrev)
        {
            return currentChunk;
        }

        prevChunk = currentChunk;
    }

    return nullptr;
}

const void *checkIndividualMapHeap(const MapAllocEntry *start)
{
    for (const MapAllocEntry *currentChunk = start; currentChunk; currentChunk = currentChunk->next)
    {
        // Check pointer sanity
        if (!ptrIsValid(const_cast<MapAllocEntry *>(currentChunk)))
        {
            return currentChunk;
        }

        // Sanity check size
        if (currentChunk->size >= 0x1800000)
        {
            return currentChunk;
        }
    }

    return nullptr;
}

void Displays::handleStandardHeapChunkResults(const void *addressWithError,
                                              const ChunkInfo *chunk,
                                              const HeapInfo *heap,
                                              int32_t heapIndex,
                                              uint32_t memoryUsageBufferIndex,
                                              uint32_t enabledFlag,
                                              bool isUsedPortion)
{
    // Get the used or free text
    const char *usedOrFreeString;

    if (isUsedPortion)
    {
        usedOrFreeString = "used";
    }
    else
    {
        usedOrFreeString = "free";
    }

    MemoryUsageDisplay *memoryUsagePtr = &this->memoryUsage;

    if (addressWithError)
    {
        // Error occured, so add error text to the corruption buffer
        // Make sure heapCorruptioBufferIndex is valid
        if (memoryUsagePtr->verifyHeapCorruptionBufferIndex())
        {
            int32_t index = memoryUsagePtr->getHeapCorruptionBufferIndex();

            index += snprintf(memoryUsagePtr->initHeapCorruptionBufferEntry(index),
                              MEMORY_USAGE_HEAP_CORRUPTION_BUFFER_SIZE - index,
                              "Main Heap %" PRId32 " (%s) corrupt at 0x%08" PRIX32 "\n",
                              heapIndex,
                              usedOrFreeString,
                              reinterpret_cast<uint32_t>(addressWithError));

            // Update the index
            memoryUsagePtr->setHeapCorruptionBufferIndex(index);
        }
    }
    else if (this->enabledFlagIsSet(enabledFlag))
    {
        // Add info about the memory usage to the memory usage buffer
        int32_t usage = 0;
        int32_t chunks = 0;

        for (const ChunkInfo *tempChunk = chunk; tempChunk; tempChunk = tempChunk->next)
        {
            usage += static_cast<int32_t>(tempChunk->size);
            chunks++;
        }

        snprintf(memoryUsagePtr->initMemoryUsageBufferEntry(memoryUsageBufferIndex),
                 MEMORY_USAGE_BUFFER_SINGLE_LINE,
                 "Main Heap %" PRId32 " (%s): %.2f/%.2fkb, %" PRId32 " cks",
                 heapIndex,
                 usedOrFreeString,
                 intToFloat(usage) / 1024.f,
                 intToFloat(static_cast<int32_t>(heap->capacity)) / 1024.f,
                 chunks);
    }
}

void Displays::handleSmartHeapChunkResults(const void *addressWithError,
                                           const SmartAllocationData *chunk,
                                           uint32_t memoryUsageBufferIndex,
                                           uint32_t enabledFlag,
                                           bool isUsedPortion)
{
    MemoryUsageDisplay *memoryUsagePtr = &this->memoryUsage;

    if (addressWithError)
    {
        // Make sure heapCorruptionBufferIndex is valid
        if (memoryUsagePtr->verifyHeapCorruptionBufferIndex())
        {
            // Get the used or free text
            const char *usedOrFreeString;

            if (isUsedPortion)
            {
                usedOrFreeString = "used";
            }
            else
            {
                usedOrFreeString = "free";
            }

            int32_t index = memoryUsagePtr->getHeapCorruptionBufferIndex();

            index += snprintf(memoryUsagePtr->initHeapCorruptionBufferEntry(index),
                              MEMORY_USAGE_HEAP_CORRUPTION_BUFFER_SIZE - index,
                              "Smart Heap (%s) corrupt at 0x%08" PRIX32 "\n",
                              usedOrFreeString,
                              reinterpret_cast<uint32_t>(addressWithError));

            // Update the index
            memoryUsagePtr->setHeapCorruptionBufferIndex(index);
        }
    }
    else if (isUsedPortion && this->enabledFlagIsSet(enabledFlag)) // Don't draw the free portion
    {
        // Add info about the memory usage to the memory usage buffer
        int32_t usage = 0;
        int32_t chunks = 0;

        for (const SmartAllocationData *tempChunk = chunk; tempChunk; tempChunk = tempChunk->pNext)
        {
            usage += static_cast<int32_t>(tempChunk->usedSize);
            chunks++;
        }

        const uint32_t heapEndRaw = reinterpret_cast<uint32_t>(heapEnd.pHeapSmart);
        const uint32_t heapStartRaw = reinterpret_cast<uint32_t>(heapStart.pHeapSmart);
        const int32_t totalSize = heapEndRaw - heapStartRaw - 0x20;

        snprintf(memoryUsagePtr->initMemoryUsageBufferEntry(memoryUsageBufferIndex),
                 MEMORY_USAGE_BUFFER_SINGLE_LINE,
                 "Smart Heap (used): %.2f/%.2fkb, %" PRId32 " cks",
                 intToFloat(usage) / 1024.f,
                 intToFloat(totalSize) / 1024.f,
                 chunks);
    }
}

#ifdef TTYD_JP
void Displays::handleMapHeapChunkResults(const void *addressWithError,
                                         const MapAllocEntry *chunk,
                                         uint32_t memoryUsageBufferIndex,
                                         uint32_t enabledFlag)
#else
void Displays::handleMapHeapChunkResults(const void *addressWithError,
                                         const MapAllocEntry *chunk,
                                         uint32_t memoryUsageBufferIndex,
                                         uint32_t enabledFlag,
                                         bool isBattleHeap)
#endif
{
    MemoryUsageDisplay *memoryUsagePtr = &this->memoryUsage;

    if (addressWithError)
    {
        // Make sure the heapCorruptioBufferIndex is valid
        if (memoryUsagePtr->verifyHeapCorruptionBufferIndex())
        {
            // Get the used or free text
            const char *usedOrFreeString;
            if (chunk->inUse)
            {
                usedOrFreeString = "used";
            }
            else
            {
                usedOrFreeString = "free";
            }

#ifdef TTYD_JP
            const char *format = "Map Heap (%s) corrupt at 0x%08" PRIX32 "\n";
#else
            // Get the battle text if checking the battle heap
            const char *currentHeap = nullptr;

            if (isBattleHeap)
            {
                currentHeap = "Battle ";
            }

            const char *format = "%sMap Heap (%s) corrupt at 0x%08" PRIX32 "\n";
#endif
            int32_t index = memoryUsagePtr->getHeapCorruptionBufferIndex();

            index += snprintf(memoryUsagePtr->initHeapCorruptionBufferEntry(index),
                              MEMORY_USAGE_HEAP_CORRUPTION_BUFFER_SIZE - index,
                              format,
#ifndef TTYD_JP
                              currentHeap,
#endif
                              usedOrFreeString,
                              reinterpret_cast<uint32_t>(addressWithError));

            // Update the index
            memoryUsagePtr->setHeapCorruptionBufferIndex(index);
        }
    }
    else if (this->enabledFlagIsSet(enabledFlag))
    {
        // Add info about the memory usage to the memory usage buffer
        int32_t usage = 0;
        int32_t chunks = 0;

        for (const MapAllocEntry *tempChunk = chunk; tempChunk; tempChunk = tempChunk->next)
        {
            if (tempChunk->inUse) // Don't draw the free portion
            {
                usage += static_cast<int32_t>(tempChunk->size);
                chunks++;
            }
        }

        // Get the battle text if checking the battle heap, as well as the size of the current heap
        uint32_t heapSize;

#ifdef TTYD_JP
        heapSize = mapalloc_size;
        const char *format = "Map Heap (used): %.2f/%.2fkb, %" PRId32 " cks";
#else
        const char *currentHeap = nullptr;
        const char *format = "%sMap Heap (used): %.2f/%.2fkb, %" PRId32 " cks";

        if (!isBattleHeap)
        {
            heapSize = mapalloc_size;
        }
        else
        {
            currentHeap = "Battle ";
            heapSize = R_battlemapalloc_size;
        }
#endif
        snprintf(memoryUsagePtr->initMemoryUsageBufferEntry(memoryUsageBufferIndex),
                 MEMORY_USAGE_BUFFER_SINGLE_LINE,
                 format,
#ifndef TTYD_JP
                 currentHeap,
#endif
                 intToFloat(usage) / 1024.f,
                 intToFloat(static_cast<int32_t>(heapSize)) / 1024.f,
                 chunks);
    }
}

static bool getDisplayMenuPosAndScale(uint32_t manuallyPositionFlag, DisplayManuallyPosition *dataOut)
{
    // Check if the player is currently manually positioning the display or adjusting the scale for the manual positioning
    // Make sure the Displays menu is open
    DisplaysMenu *displaysMenuPtr = gDisplaysMenu;
    if (!displaysMenuPtr)
    {
        return false;
    }

    // If the selected display in the menu is not for the current display, then assume that the player is not
    // currently manually positioning or scaling
    if (indexToDisplayManuallyPositionFlag(displaysMenuPtr->getSelectedDisplay()) != manuallyPositionFlag)
    {
        return false;
    }

    // Make sure the pointer to the manually position data for the current display is properly retrieved
    const DisplayManuallyPosition *manuallyPositionPtr = gDisplays->getDisplayManuallyPositionPtr(manuallyPositionFlag);
    if (!manuallyPositionPtr)
    {
        return false;
    }

    // Check if the player is currently manually positioning the display
    const PositionEditor *positonEditorPtr = displaysMenuPtr->getPositionEditorPtr();
    if (positonEditorPtr->shouldDraw())
    {
        if (dataOut)
        {
            dataOut->setPosX(positonEditorPtr->getPosX());
            dataOut->setPosY(positonEditorPtr->getPosY());
            dataOut->setScale(manuallyPositionPtr->getScale());
        }
        return true;
    }

    // Check if the player is currently adjusting the scale for the manual positioning
    const ValueEditor *valueEditorPtr = displaysMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        // Try to get the current value from the value editor
        ValueType value;
        if (valueEditorPtr->getValueFromString(&value))
        {
            if (dataOut)
            {
                dataOut->setPosX(manuallyPositionPtr->getPosX());
                dataOut->setPosY(manuallyPositionPtr->getPosY());
                dataOut->setScale(value.f32);
            }
            return true;
        }
    }

    return false;
}

static bool menuCurrentlyAdjustingPosOrScale(uint32_t manuallyPositionFlag)
{
    return getDisplayMenuPosAndScale(manuallyPositionFlag, nullptr);
}

static bool getDisplayPosAndScale(DisplaysScreenPosition screenPos,
                                  uint32_t manuallyPositionFlag,
                                  DisplayManuallyPosition *dataOut,
                                  const char *text)
{
    // Check if the player is currently manually positioning the display or adjusting the scale for the manual positioning
    if (getDisplayMenuPosAndScale(manuallyPositionFlag, dataOut))
    {
        return true;
    }

    // If the manually position flag is set, then get the position and scale for that
    Displays *displaysPtr = gDisplays;
    if (displaysPtr->manuallyPositionFlagIsSet(manuallyPositionFlag))
    {
        // Make sure the pointer to the manually position data for the current display is properly retrieved
        const DisplayManuallyPosition *manuallyPositionPtr = displaysPtr->getDisplayManuallyPositionPtr(manuallyPositionFlag);
        if (manuallyPositionPtr)
        {
            dataOut->setPosX(manuallyPositionPtr->getPosX());
            dataOut->setPosY(manuallyPositionPtr->getPosY());
            dataOut->setScale(manuallyPositionPtr->getScale());
            return true;
        }
    }

    // All of the default scales should be the same, regardless of position
    float scale = DISPLAYS_DEFAULT_SCALE;

    // Handle adjusting the scale for specific displays
    // This must be done now since the scale will be used in the code below for displays on the right side of the screen
    switch (manuallyPositionFlag)
    {
        case DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_ONSCREEN_TIMER:
        case DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_FRAME_COUNTER:
        {
#ifdef TTYD_JP
            scale += 0.1f;
#else
            scale += 0.05f;
#endif
            break;
        }
        default:
        {
            break;
        }
    }

    dataOut->setScale(scale);

    constexpr float lineDecrement = GET_DISPLAYS_DEFAULT_SCALED_LINE_DECREMENT;
    switch (screenPos)
    {
        case DisplaysScreenPosition::DISPLAYS_POSITION_BOTTOM_LEFT:
        {
            const float posY = displaysPtr->getDefaultPosBottomLeft();
            dataOut->setPosY(posY);

            // Increment the default pos for the next display
            displaysPtr->setDefaultPosBottomLeft(posY + lineDecrement);

            dataOut->setPosX(DISPLAYS_DEFAULT_POS_X_LEFT);
            break;
        }
        case DisplaysScreenPosition::DISPLAYS_POSITION_BOTTOM_RIGHT:
        {
            const float posY = displaysPtr->getDefaultPosBottomRight();
            dataOut->setPosY(posY);

            // Increment the default pos for the next display
            displaysPtr->setDefaultPosBottomRight(posY + lineDecrement);

            // This is assuming the text will be aligned to the left to avoid having it move constantly from constantly changing
            // values
            const float width = getTextWidth(text, scale);
            dataOut->setPosX(DISPLAYS_DEFAULT_POS_X_RIGHT - width);
            break;
        }
        case DisplaysScreenPosition::DISPLAYS_POSITION_TOP_LEFT:
        {
            const float posY = displaysPtr->getDefaultPosTopLeft();
            dataOut->setPosY(posY);

            // Decrement the default pos for the next display
            displaysPtr->setDefaultPosTopLeft(posY - lineDecrement);

            dataOut->setPosX(DISPLAYS_DEFAULT_POS_X_LEFT);
            break;
        }
        case DisplaysScreenPosition::DISPLAYS_POSITION_TOP_RIGHT:
        {
            const float posY = displaysPtr->getDefaultPosTopRight();
            dataOut->setPosY(posY);

            // Decrement the default pos for the next display
            displaysPtr->setDefaultPosTopRight(posY - lineDecrement);

            // This is assuming the text will be aligned to the left to avoid having it move constantly from constantly changing
            // values
            const float width = getTextWidth(text, scale);
            dataOut->setPosX(DISPLAYS_DEFAULT_POS_X_RIGHT - width);
            break;
        }
        default:
        {
            dataOut->setPosX(0.f);
            dataOut->setPosY(0.f);
            break;
        }
    }

    return false;
}

static bool getDisplayPosAndScale(DisplaysScreenPosition screenPos,
                                  uint32_t manuallyPositionFlag,
                                  DisplayManuallyPosition *dataOut)
{
    return getDisplayPosAndScale(screenPos, manuallyPositionFlag, dataOut, nullptr);
}

static void drawButtonInputs(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Get the position and scale for the text

    // This needs to run now since nothing will be drawn if no buttons are held this frame, but the Y position of all the text
    // still needs to be incremented
    DisplayManuallyPosition data;

    getDisplayPosAndScale(DisplaysScreenPosition::DISPLAYS_POSITION_BOTTOM_LEFT,
                          DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_BUTTON_INPUTS,
                          &data);

    // Make sure at least one button is held
    const uint32_t buttonsHeld = keyGetButton(PadId::CONTROLLER_ONE);
    if (!buttonsHeld)
    {
        return;
    }

    const char *format;

#ifdef TTYD_JP
    format = "%s";

    static const char *buttonInputDisplay[] = {
        "\x81\xA9",
        "\x81\xA8",
        "\x81\xAB",
        "\x81\xAA",
        "Z",
        "R",
        "L",
        "A",
        "B",
        "X",
        "Y",
        "S",
    };
#else
    format = "%c";

    static const char buttonInputDisplay[] = {
        static_cast<char>(171), // Left
        static_cast<char>(187), // Right
        static_cast<char>(179), // Down
        static_cast<char>(178), // Up
        'Z',
        'R',
        'L',
        'A',
        'B',
        'X',
        'Y',
        'S',
    };
#endif

    const float characterIncrement = 30.f * data.getScale();
    float posX = data.getPosX();
    char buf[16];

    for (uint32_t i = 0, counter = 0; i < TOTAL_MENU_INPUT_BUTTONS; i++, counter++, posX += characterIncrement)
    {
        if (i == 7)
        {
            // Skip unused value
            i++;
        }

        if (!((buttonsHeld >> i) & 1U))
        {
            // The current button is not held
            continue;
        }

        // Draw the text for the current button
        snprintf(buf, sizeof(buf), format, buttonInputDisplay[counter]);
        drawText(buf, posX, data.getPosY(), data.getScale(), getColorWhite(0xFF));
    }
}

static void handleButtonInputs(Displays *displaysPtr)
{
    if (!displaysPtr->displayShouldBeHandled(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_BUTTON_INPUTS))
    {
        return;
    }

    displaysPtr->setShouldDrawFlag(DisplaysShouldDrawFlag::DISPLAYS_SHOULD_DRAW_FLAG_BUTTON_INPUTS);
}

static void drawMarioCoordinates(float posX, float posY, float scale)
{
    // Set up the text
    const Player *playerPtr = marioGetPtr();
    char buf[128];

    snprintf(buf,
             sizeof(buf),
             "MarPos: %.2f  %.2f  %.2f",
             playerPtr->playerPosition.x,
             playerPtr->playerPosition.y,
             playerPtr->playerPosition.z);

    // Draw the text
    drawText(buf, posX, posY, scale, getColorWhite(0xFF));
}

static void drawMarioCoordinatesMain(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Get the position and scale for the text
    DisplayManuallyPosition data;

    getDisplayPosAndScale(DisplaysScreenPosition::DISPLAYS_POSITION_BOTTOM_LEFT,
                          DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_MARIO_COORDINATES,
                          &data);
    // Draw the text
    drawMarioCoordinates(data.getPosX(), data.getPosY(), data.getScale());
}

static void handleMarioCoordinates(Displays *displaysPtr)
{
    if (!displaysPtr->displayShouldBeHandled(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MARIO_COORDINATES))
    {
        return;
    }

    displaysPtr->setShouldDrawFlag(DisplaysShouldDrawFlag::DISPLAYS_SHOULD_DRAW_FLAG_MARIO_COORDINATES);
}

static void drawStickAngle(float posX, float posY, float scale)
{
    // Get the stick angle string
    char stickAngleBuf[32];
    constexpr uint32_t stickAngleBufSize = sizeof(stickAngleBuf);
    getStickAngleString(stickAngleBuf, stickAngleBufSize);

    // Set up the main text
    char buf[32 + stickAngleBufSize];
    snprintf(buf, sizeof(buf), "StickAngle: %s", stickAngleBuf);

    // Draw the text
    drawText(buf, posX, posY, scale, getColorWhite(0xFF));
}

static void drawStickAngleMain(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Get the position and scale for the text
    DisplayManuallyPosition data;

    getDisplayPosAndScale(DisplaysScreenPosition::DISPLAYS_POSITION_BOTTOM_LEFT,
                          DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_STICK_ANGLE,
                          &data);
    // Draw the text
    drawStickAngle(data.getPosX(), data.getPosY(), data.getScale());
}

static void handleStickAngle(Displays *displaysPtr)
{
    if (!displaysPtr->displayShouldBeHandled(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_STICK_ANGLE))
    {
        return;
    }

    displaysPtr->setShouldDrawFlag(DisplaysShouldDrawFlag::DISPLAYS_SHOULD_DRAW_FLAG_STICK_ANGLE);
}

static void drawMarioSpeedXZ(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Set up the text
    const Player *playerPtr = marioGetPtr();
    char buf[64];

    snprintf(buf, sizeof(buf), "MarSpeedXZ: %.6f", playerPtr->wPlayerEffectiveSpeed);

    // Get the position and scale for the text
    DisplayManuallyPosition data;

    getDisplayPosAndScale(DisplaysScreenPosition::DISPLAYS_POSITION_BOTTOM_LEFT,
                          DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_MARIO_SPEED_X_Z,
                          &data);
    // Draw the text
    drawText(buf, data.getPosX(), data.getPosY(), data.getScale(), getColorWhite(0xFF));
}

static void handleMarioSpeedXZ(Displays *displaysPtr)
{
    if (!displaysPtr->displayShouldBeHandled(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MARIO_SPEED_X_Z))
    {
        return;
    }

    displaysPtr->setShouldDrawFlag(DisplaysShouldDrawFlag::DISPLAYS_SHOULD_DRAW_FLAG_MARIO_SPEED_X_Z);
}

static void drawOnScreenTimer(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Get the time string
    char buf[64];
    Displays *displaysPtr = gDisplays;
    getTimeString(buf, sizeof(buf), displaysPtr->getOnScreenTimerDisplayPtr()->getMainTimer());

    // Get the position and scale for the time string
    DisplayManuallyPosition data;

    const bool customPos = getDisplayPosAndScale(DisplaysScreenPosition::DISPLAYS_POSITION_BOTTOM_RIGHT,
                                                 DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_ONSCREEN_TIMER,
                                                 &data,
                                                 "00:00:00.00");
    float posY = data.getPosY();

#ifdef TTYD_JP
    if (!customPos)
    {
        // JP uses a slightly different posY from other displays
        posY += 2.f;
    }
#else
    (void)customPos;
#endif

    // Draw the time string
    const float scale = data.getScale();
    drawText(buf, data.getPosX(), posY, scale, 172.f * scale, getColorWhite(0xFF));
}

static void handleOnScreenTimer(Displays *displaysPtr)
{
    if (!displaysPtr->displayShouldBeHandled(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_ONSCREEN_TIMER))
    {
        return;
    }

    displaysPtr->setShouldDrawFlag(DisplaysShouldDrawFlag::DISPLAYS_SHOULD_DRAW_FLAG_ONSCREEN_TIMER);

    // Only allow handling if the enabled flag is set
    if (!displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_ONSCREEN_TIMER))
    {
        return;
    }

    if (displaysPtr->checkDisplayButtonCombo(DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_ONSCREEN_TIMER_START_PAUSE_RESUME))
    {
        displaysPtr->toggleMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_ONSCREEN_TIMER_PAUSED);
    }
    else if (displaysPtr->checkDisplayButtonCombo(DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_ONSCREEN_TIMER_RESET))
    {
        // Reset the timer
        displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_ONSCREEN_TIMER_PAUSED);
        displaysPtr->getOnScreenTimerDisplayPtr()->setMainTimer(0);
    }
}

static void updateOnScreenTimer()
{
    Displays *displaysPtr = gDisplays;
    if (!displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_ONSCREEN_TIMER))
    {
        return;
    }

    const OSTime currentFrameTime = OSGetTime();

    // Make sure the previous frame time has a value set
    OnScreenTimerDisplay *onScreenTimerPtr = displaysPtr->getOnScreenTimerDisplayPtr();
    const OSTime previousFrameTime = onScreenTimerPtr->getPrevFrameTime();

    if (previousFrameTime != 0)
    {
        const Mod *modPtr = gMod;

        if (!displaysPtr->miscFlagIsSet(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_ONSCREEN_TIMER_PAUSED) &&
            (!gMenu || modPtr->flagIsSet(ModFlag::MOD_FLAG_MENU_IS_HIDDEN)) &&
            !modPtr->flagIsSet(ModFlag::MOD_FLAG_CHANGING_BUTTON_COMBO))
        {
            const OSTime incrementAmount = currentFrameTime - previousFrameTime;
            onScreenTimerPtr->setMainTimer(onScreenTimerPtr->getMainTimer() + incrementAmount);
        }
    }

    onScreenTimerPtr->setPrevFrameTime(currentFrameTime);
}

static void drawFrameCounter(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Get the current frame counter timer
    Displays *displaysPtr = gDisplays;
    FrameCounterDisplay *frameCounterPtr = displaysPtr->getFrameCounterDisplayPtr();
    const uint32_t timer = frameCounterPtr->getTimer();

    // The frame counter is based on how many frames are drawn, so update the timer now if it is not paused
    if (!displaysPtr->miscFlagIsSet(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_FRAME_COUNTER_PAUSED))
    {
        frameCounterPtr->setTimer(timer + 1);
    }

    // Get the time string
    const uint32_t currentFps = static_cast<uint32_t>(_globalWorkPtr->framerate);
    const uint32_t totalSeconds = timer / currentFps;
    const uint32_t totalMinutes = totalSeconds / 60;

    const uint32_t hours = totalMinutes / 60;
    const uint32_t minutes = totalMinutes % 60;
    const uint32_t seconds = totalSeconds % 60;
    const uint32_t frames = timer % currentFps;

    char buf[64];
    snprintf(buf, sizeof(buf), gTimeStringFormat, hours, minutes, seconds, frames);

    // Get the position and scale for the time string
    DisplayManuallyPosition data;

    const bool customPos = getDisplayPosAndScale(DisplaysScreenPosition::DISPLAYS_POSITION_BOTTOM_RIGHT,
                                                 DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_FRAME_COUNTER,
                                                 &data,
                                                 "00:00:00.00");
    float posY = data.getPosY();

#ifdef TTYD_JP
    if (!customPos)
    {
        // JP uses a slightly different posY from other displays
        posY += 2.f;
    }
#else
    (void)customPos;
#endif

    // Draw the time string
    const float scale = data.getScale();
    drawText(buf, data.getPosX(), posY, scale, 172.f * scale, getColorWhite(0xFF));
}

static void handleFrameCounter(Displays *displaysPtr)
{
    if (!displaysPtr->displayShouldBeHandled(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_FRAME_COUNTER))
    {
        return;
    }

    displaysPtr->setShouldDrawFlag(DisplaysShouldDrawFlag::DISPLAYS_SHOULD_DRAW_FLAG_FRAME_COUNTER);

    // Only allow handling if the enabled flag is set
    if (!displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_FRAME_COUNTER))
    {
        return;
    }

    if (displaysPtr->checkDisplayButtonCombo(DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_FRAME_COUNTER_START_PAUSE_RESUME))
    {
        displaysPtr->toggleMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_FRAME_COUNTER_PAUSED);
    }
    else if (displaysPtr->checkDisplayButtonCombo(DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_FRAME_COUNTER_RESET))
    {
        // Reset the timer
        displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_FRAME_COUNTER_PAUSED);
        displaysPtr->getFrameCounterDisplayPtr()->setTimer(0);
    }
}

static void drawHeapCorruptionErrors(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Initialize text drawing
    drawTextInit(true);

    Displays *displaysPtr = gDisplays;
    MemoryUsageDisplay *memoryUsagePtr = displaysPtr->getMemoryUsageDisplayPtr();
    const char *text = memoryUsagePtr->getHeapCorruptionBufferPtr();

    // Decrement the default Pos Y value to account for how many lines are in the heap corruption buffer
    constexpr float scale = DISPLAYS_DEFAULT_SCALE_ERRORS;
    const float posY = displaysPtr->getErrorTextPosY();
    displaysPtr->decrementDefaultPosErrorText(getTextMultilineIncrement(text, scale, 0));

    // Draw the text
    drawText(text, DISPLAYS_DEFAULT_POS_X_LEFT, posY, scale, getColorWhite(0xFF));

    // Done, so reset heapCorruptioBufferIndex and free the memory used by the heap corruption buffer
    memoryUsagePtr->setHeapCorruptionBufferIndex(0);
    memoryUsagePtr->freeHeapCorruptionBuffer();
}

static void drawMemoryUsage(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    auto decrementPosY = [](bool customPos, Displays *displaysPtr, float *posYOut, float lineDecrement)
    {
        *posYOut -= lineDecrement;

        if (!customPos)
        {
            displaysPtr->decrementDefaultPosTopLeft(lineDecrement);
        }
    };

    // Get the position and scale for the text
    DisplayManuallyPosition data;

    const bool customPos = getDisplayPosAndScale(DisplaysScreenPosition::DISPLAYS_POSITION_TOP_LEFT,
                                                 DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_MEMORY_USAGE,
                                                 &data);

    // Draw the text for the main heaps
    Displays *displaysPtr = gDisplays;
    MemoryUsageDisplay *memoryUsagePtr = displaysPtr->getMemoryUsageDisplayPtr();
    const char *text = memoryUsagePtr->getMemoryUsageBufferPtr();
    bool drawnText = false;

    const float posX = data.getPosX();
    float posY = data.getPosY();
    const float scale = data.getScale();
    const float lineDecrement = LINE_HEIGHT_FLOAT * scale;

    // Make sure the memory usage buffer has been initialized
    if (text)
    {
        for (int32_t i = 0; i < DISPLAYS_TOTAL_MAIN_HEAPS; i++, text += (MEMORY_USAGE_BUFFER_SINGLE_LINE * 2))
        {
            // Only draw if the current option is enabled
            if (displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_HEAP_0 + i))
            {
                drawnText = true;

                // Draw the used and free text
                const char *string = text;
                if (string[0])
                {
                    drawText(text, posX, posY, scale, getColorWhite(0xFF));
                    decrementPosY(customPos, displaysPtr, &posY, lineDecrement);
                }

                string = &text[MEMORY_USAGE_BUFFER_SINGLE_LINE];
                if (string[0])
                {
                    drawText(string, posX, posY, scale, getColorWhite(0xFF));
                    decrementPosY(customPos, displaysPtr, &posY, lineDecrement);
                }
            }
        }

        // Draw the text for the smart heap and map heap(s)
        for (uint32_t i = 0; i < DISPLAYS_TOTAL_EXTRA_HEAPS; i++, text += MEMORY_USAGE_BUFFER_SINGLE_LINE)
        {
            if (displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_SMART_HEAP + i))
            {
                drawnText = true;

                if (text[0])
                {
                    drawText(text, posX, posY, scale, getColorWhite(0xFF));
                    decrementPosY(customPos, displaysPtr, &posY, lineDecrement);
                }
            }
        }
    }

    // If manually changing position/scale and nothing was drawn, then manually draw a hardcoded string
    if (!drawnText &&
        menuCurrentlyAdjustingPosOrScale(DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_MEMORY_USAGE))
    {
        drawText("Main Heap 0 (used): 0.00/0.00kb, 0 cks", posX, posY, scale, getColorWhite(0xFF));
    }

    if (!customPos)
    {
        // Increment the default Pos Y value to fix positioning for other displays
        displaysPtr->incrementDefaultPosTopLeft(lineDecrement);
    }

    // Done, so free the memory used by the memory usage buffer
    memoryUsagePtr->freeMemoryUsageBuffer();
}

static void handleMemoryUsage(Displays *displaysPtr)
{
    const MemoryUsageDisplay *memoryUsagePtr = displaysPtr->getMemoryUsageDisplayPtr();

    // Draw any heap errors that occured
    if (memoryUsagePtr->shouldDrawHeapCorruptionBuffer())
    {
        drawOnDebugLayer(drawHeapCorruptionErrors, displaysPtr->getErrorTextOrder());
    }

    // Draw the memory usage of the heaps if any of the enabled flags for them are set
    if (memoryUsagePtr->shouldDrawMemoryUsageBuffer())
    {
        displaysPtr->setShouldDrawFlag(DisplaysShouldDrawFlag::DISPLAYS_SHOULD_DRAW_FLAG_MEMORY_USAGE);
    }

    // Check if this display is in the process of being manually positioned or scaled
    else if (menuCurrentlyAdjustingPosOrScale(DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_MEMORY_USAGE))
    {
        displaysPtr->setShouldDrawFlag(DisplaysShouldDrawFlag::DISPLAYS_SHOULD_DRAW_FLAG_MEMORY_USAGE);
    }
}

static void drawEffsActive(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Get the total amount of effs active
    const EffWork *effWorkPtr = _effWorkPtr;
    const uint32_t maxEntries = effWorkPtr->maxEntries;

    const EffEntry *entry = &effWorkPtr->pEntries[0];
    uint32_t count = 0;

    for (uint32_t i = 0; i < maxEntries; i++, entry++)
    {
        if (entry->wFlags)
        {
            count++;
        }
    }

    // Draw the total amount of effs active out of the max
    char buf[64];
    snprintf(buf, sizeof(buf), "Effs Active: %" PRIu32 "/%" PRIu32, count, maxEntries);

    // Get the position and scale for the text
    DisplayManuallyPosition data;

    getDisplayPosAndScale(DisplaysScreenPosition::DISPLAYS_POSITION_TOP_LEFT,
                          DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_EFFS_ACTIVE,
                          &data);
    // Draw the text
    drawText(buf, data.getPosX(), data.getPosY(), data.getScale(), getColorWhite(0xFF));
}

static void handleEffsActive(Displays *displaysPtr)
{
    if (!displaysPtr->displayShouldBeHandled(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_EFFS_ACTIVE))
    {
        return;
    }

    displaysPtr->setShouldDrawFlag(DisplaysShouldDrawFlag::DISPLAYS_SHOULD_DRAW_FLAG_EFFS_ACTIVE);
}

static void drawEvtsActive(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Get the total amount of evts active
    const EvtWork *evtWorkPtr = evtGetWork();
    const uint32_t maxEntries = evtWorkPtr->entryCount;

    const EvtEntry *entry = &evtWorkPtr->entries[0];
    uint32_t count = 0;

    for (uint32_t i = 0; i < maxEntries; i++, entry++)
    {
        if (entry->flags & 0x1)
        {
            count++;
        }
    }

    // Draw the total amount of evts active out of the max
    char buf[64];
    snprintf(buf, sizeof(buf), "Evts Active: %" PRId32 "/%" PRId32, count, maxEntries);

    // Get the position and scale for the text
    DisplayManuallyPosition data;

    getDisplayPosAndScale(DisplaysScreenPosition::DISPLAYS_POSITION_TOP_LEFT,
                          DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_EVTS_ACTIVE,
                          &data);
    // Draw the text
    drawText(buf, data.getPosX(), data.getPosY(), data.getScale(), getColorWhite(0xFF));
}

static void handleEvtsActive(Displays *displaysPtr)
{
    if (!displaysPtr->displayShouldBeHandled(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_EVTS_ACTIVE))
    {
        return;
    }

    displaysPtr->setShouldDrawFlag(DisplaysShouldDrawFlag::DISPLAYS_SHOULD_DRAW_FLAG_EVTS_ACTIVE);
}

static void drawEnemyEncounterNotifier(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Get the current counter value
    EnemyEncounterNotifierDisplay *enemyEncounterNotifierPtr = gDisplays->getEnemyEncounterNotifierDisplayPtr();
    const uint32_t counter = enemyEncounterNotifierPtr->getCounter();

    // Decrement the timer, and reset the counter if the time reaches 0
    uint32_t timer = enemyEncounterNotifierPtr->getTimer();
    if (timer > 0)
    {
        enemyEncounterNotifierPtr->setTimer(--timer);

        if (timer == 0)
        {
            enemyEncounterNotifierPtr->resetCounter();
        }
    }

    char buf[64];
    snprintf(buf, sizeof(buf), "Encounter occured x%" PRIu32, counter);

    // Get the position and scale for the text
    DisplayManuallyPosition data;

    getDisplayPosAndScale(DisplaysScreenPosition::DISPLAYS_POSITION_TOP_LEFT,
                          DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_ENEMY_ENCOUNTER_NOTIFIER,
                          &data);
    // Draw the text
    drawText(buf, data.getPosX(), data.getPosY(), data.getScale(), getColorWhite(0xFF));
}

void checkForEnemyEncounters(void *ptr)
{
    Displays *displaysPtr = gDisplays;
    if (!displaysPtr->displayShouldBeHandled(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_ENEMY_ENCOUNTER_NOTIFIER))
    {
        return;
    }

    if (!ptr)
    {
        return;
    }

    // ptr is set, so an encounter would normally occur now
    EnemyEncounterNotifierDisplay *enemyEncounterNotifierPtr = displaysPtr->getEnemyEncounterNotifierDisplayPtr();
    enemyEncounterNotifierPtr->setTimer(sysMsec2Frame(3000));
    enemyEncounterNotifierPtr->incrementCounter();
}

static void handleEnemyEncounterNotifier(Displays *displaysPtr)
{
    if ((displaysPtr->getEnemyEncounterNotifierDisplayPtr()->getTimer() > 0) ||
        checkIfAdjustingManualPositionOrScale(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_ENEMY_ENCOUNTER_NOTIFIER))
    {
        displaysPtr->setShouldDrawFlag(DisplaysShouldDrawFlag::DISPLAYS_SHOULD_DRAW_FLAG_ENEMY_ENCOUNTER_NOTIFIER);
    }
}

static void drawJumpStorage(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Get the text for whether the jump storage flag is set
    const Player *playerPtr = marioGetPtr();
    const bool jumpStorageFlag = playerPtr->flags3 & 0x10000;

    uint32_t color;
    const char *string;
    getOnOffTextAndColor(jumpStorageFlag, &string, &color, 0xFF);

    char buf[64];
    snprintf(buf, sizeof(buf), "JS: <col %" PRIx32 ">%s\nSpdY: %.2f", color, string, playerPtr->wJumpVelocityY);

    // Get the position and scale for the text
    DisplayManuallyPosition data;

    getDisplayPosAndScale(DisplaysScreenPosition::DISPLAYS_POSITION_TOP_RIGHT,
                          DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_JUMP_STORAGE,
                          &data,
                          "SpdY: -18.00");

    // Draw the text
    const float scale = data.getScale();
    drawText(buf, data.getPosX(), data.getPosY(), scale, 170.f * scale, getColorWhite(0xFF));
}

static void handleJumpStorage(Displays *displaysPtr)
{
    if (!displaysPtr->displayShouldBeHandled(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_JUMP_STORAGE))
    {
        return;
    }

    displaysPtr->setShouldDrawFlag(DisplaysShouldDrawFlag::DISPLAYS_SHOULD_DRAW_FLAG_JUMP_STORAGE);

    // Only allow handling if the enabled flag is set
    if (!displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_JUMP_STORAGE))
    {
        return;
    }

    JumpStorageDisplay *jumpStoragePtr = displaysPtr->getJumpStorageDisplayPtr();

    // Check for incrementing the counter
    if (displaysPtr->checkDisplayButtonCombo(DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_JUMP_STORAGE))
    {
        jumpStoragePtr->incrementCounter();
    }

    if (jumpStoragePtr->shouldSetFlag())
    {
        jumpStoragePtr->resetCounter();

        // Enable jump storage
        marioGetPtr()->flags3 |= 0x10000;
    }
}

static void drawGuardSuperguardTimings(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Check if this display is in the process of being manually positioned or scaled
    DisplayManuallyPosition data;

    const bool currentlyAdjusting =
        getDisplayMenuPosAndScale(DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_GUARD_SUPERGUARD_TIMINGS,
                                  &data);

    // Make sure Mario is in the battle, as this is required to check how many Simplifier/Unsimplifiers are equipped
    // The player could stall the timer by opening the menu while the battle is ending, which makes this check necessary
    const BattleWorkUnit *marioBattlePtr = getMarioBattlePtr();
    if (!marioBattlePtr && !currentlyAdjusting)
    {
        // Assume that since Mario is no longer in the battle, that the battle has either ended or is about to end, so set the
        // timer to 0 to prevent this display from being drawn again
        gDisplays->getGuardSuperguardTimingDisplayPtr()->setTimer(0);
        return;
    }

    // Get the difficulty set by how many Simplifier/Unsimplifiers are equipped
    int32_t difficulty = 3;
    if (marioBattlePtr)
    {
        const BadgesEquipped *equippedBadgesPtr = &marioBattlePtr->badges_equipped;
        difficulty = equippedBadgesPtr->unsimplifier - equippedBadgesPtr->simplifier + 3;

        // The value is capped to between and including 0-6
        if (difficulty < 0)
        {
            difficulty = 0;
        }
        else if (difficulty > 6)
        {
            difficulty = 6;
        }
    }

    // Check to see which text should be displayed
    GuardSuperguardTimingDisplay *guardSuperguardTimingPtr = gDisplays->getGuardSuperguardTimingDisplayPtr();
    const int32_t lastAFrame = guardSuperguardTimingPtr->getLastAFrame();
    const int32_t lastBFrame = guardSuperguardTimingPtr->getLastBFrame();

    // Decrement the timer now since it needs to be done sometime in this function anyway, and it doesn't matter when
    guardSuperguardTimingPtr->decrementTimer();

    char buf[128];
    constexpr uint32_t bufSize = sizeof(buf);
    const char *text = buf;

    switch (guardSuperguardTimingPtr->getType())
    {
        case GuardSuperguardTimingDisplay::DefenceResult::DEFENCE_RESULT_SUCCESSFUL_TIMING:
        {
            uint32_t difficultyFrames;
            int32_t framePressed;
            char button;

            if (lastAFrame > -1)
            {
                difficultyFrames = _guardFrames[difficulty];
                framePressed = difficultyFrames - lastAFrame;
                button = 'A';
            }
            else
            {
                difficultyFrames = _superguardFrames[difficulty];
                framePressed = difficultyFrames - lastBFrame;
                button = 'B';
            }

            snprintf(buf, bufSize, "Pressed %c on frame %" PRId32 " of %" PRIu32, button, framePressed, difficultyFrames);
            break;
        }
        case GuardSuperguardTimingDisplay::DefenceResult::DEFENCE_RESULT_PRESSED_TOO_MANY_BUTTONS:
        {
            text = "Pressed too many buttons";
            break;
        }
        case GuardSuperguardTimingDisplay::DefenceResult::DEFENCE_RESULT_PRESSED_TOO_EARLY:
        {
            uint32_t difficultyFrames;
            int32_t framesEarly;
            char button;

            if (lastAFrame > -1)
            {
                difficultyFrames = _guardFrames[difficulty];
                framesEarly = lastAFrame - difficultyFrames + 1;
                button = 'A';
            }
            else
            {
                difficultyFrames = _superguardFrames[difficulty];
                framesEarly = lastBFrame - difficultyFrames + 1;
                button = 'B';
            }

            const char *plural;
            if (framesEarly != 1)
            {
                plural = "s";
            }
            else
            {
                plural = nullptr;
            }

            snprintf(buf, bufSize, "Pressed %c %" PRId32 " frame%s early", button, framesEarly, plural);
            break;
        }
        case GuardSuperguardTimingDisplay::DefenceResult::DEFENCE_RESULT_CANNOT_BE_SUPERGUARD:
        {
            text = "Cannot superguard this attack";
            break;
        }
        default:
        {
            if (currentlyAdjusting)
            {
                // Use a hardcoded string for a successful timing
                text = "Pressed A on frame 1 of 8";
            }
            else
            {
                // Set the timer to 0 to prevent this display from being drawn again
                guardSuperguardTimingPtr->setTimer(0);
                return;
            }
            break;
        }
    }

    // Initialize the window based on the text
    const bool manualPositioning =
        getDisplayPosAndScale(DisplaysScreenPosition::DISPLAYS_POSITION_NONE,
                              DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_GUARD_SUPERGUARD_TIMINGS,
                              &data);

    float scale;
    if (manualPositioning)
    {
        scale = data.getScale();
    }
    else
    {
        scale = DISPLAYS_DEFAULT_SCALE + 0.05f;
    }

    Window window;
    window.setWidthHeightFromTextAndInit(text, scale, getColorBlack(0xE5), 10.f, 20.f);

    // Set the proper posX and posY
    if (manualPositioning)
    {
        window.setPosX(data.getPosX());
        window.setPosY(data.getPosY());
    }
    else
    {
        window.setPosX(DISPLAYS_DEFAULT_POS_X_LEFT);
        window.setPosY(-134.f);
    }

    // Draw the window
    window.draw();

    // Initialize text drawing
    drawTextInit(false);

    // Draw the text
    float posX;
    float posY;
    window.getTextPosXY(nullptr, WindowAlignment::TOP_LEFT, scale, &posX, &posY);
    drawText(text, posX, posY, scale, getColorWhite(0xFF));
}

int32_t initGuardSuperguardTimings(BattleWorkUnit *battleUnitPtr, BattleWeapon *weapon)
{
    // Credits to Jdaster64 for writing the original code for this function
    Displays *displaysPtr = gDisplays;
    if (!displaysPtr->displayShouldBeHandled(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_GUARD_SUPERGUARD_TIMINGS))
    {
        // Call the original function
        return g_BattleActionCommandCheckDefence_trampoline(battleUnitPtr, weapon);
    }

    // Make sure Mario is in the battle, as this is required to check how many Simplifier/Unsimplifiers are equipped
    const BattleWorkUnit *marioBattlePtr = getMarioBattlePtr();
    if (!marioBattlePtr)
    {
        // Assume that since Mario is no longer in the battle, that the battle has either ended or is about to end, so just call
        // the original function
        return g_BattleActionCommandCheckDefence_trampoline(battleUnitPtr, weapon);
    }

    // Check to see if the attack will be guarded/superguarded automatically or not
    if (marioBattlePtr->badges_equipped.auto_command_badge > 0)
    {
        // The attack will be guarded/superguarded automatically, so call the original function
        return g_BattleActionCommandCheckDefence_trampoline(battleUnitPtr, weapon);
    }

    // Reset the values checked when drawing the text
    GuardSuperguardTimingDisplay *guardSuperguardTimingPtr = displaysPtr->getGuardSuperguardTimingDisplayPtr();
    guardSuperguardTimingPtr->resetValues();

    // Loop through the buttons pressed from the previous frame to look for A and B presses
    uint32_t buttonPresses = 0;
    int32_t lastAFrame = -1;
    int32_t lastBFrame = -1;

    for (int32_t frame = 0; frame < 15; frame++)
    {
        if (BattleACPadCheckRecordTrigger(frame, PadInput::PAD_A))
        {
            lastAFrame = frame;
            buttonPresses++;
        }

        if (BattleACPadCheckRecordTrigger(frame, PadInput::PAD_B))
        {
            lastBFrame = frame;
            buttonPresses++;
        }
    }

    // Call the original function now, as its result is needed
    const int32_t ret = g_BattleActionCommandCheckDefence_trampoline(battleUnitPtr, weapon);
    constexpr uint32_t displayTime = 3000;

    if (ret == 4)
    {
        // Successful guard, print last A frame
        guardSuperguardTimingPtr->setLastAFrame(lastAFrame);
        guardSuperguardTimingPtr->setType(GuardSuperguardTimingDisplay::DefenceResult::DEFENCE_RESULT_SUCCESSFUL_TIMING);
        guardSuperguardTimingPtr->setTimer(sysMsec2Frame(displayTime));
    }
    else if (ret == 5)
    {
        // Successful superguard, print last B frame
        guardSuperguardTimingPtr->setLastBFrame(lastBFrame);
        guardSuperguardTimingPtr->setType(GuardSuperguardTimingDisplay::DefenceResult::DEFENCE_RESULT_SUCCESSFUL_TIMING);
        guardSuperguardTimingPtr->setTimer(sysMsec2Frame(displayTime));
    }
    else if (buttonPresses > 1) // Unsuccessful, otherwise...
    {
        // Hit too many buttons in the last 15 frames
        guardSuperguardTimingPtr->setType(GuardSuperguardTimingDisplay::DefenceResult::DEFENCE_RESULT_PRESSED_TOO_MANY_BUTTONS);
        guardSuperguardTimingPtr->setTimer(sysMsec2Frame(displayTime));
    }
    else if (lastAFrame > -1)
    {
        // Print how many frames early the player pressed A
        guardSuperguardTimingPtr->setLastAFrame(lastAFrame);
        guardSuperguardTimingPtr->setType(GuardSuperguardTimingDisplay::DefenceResult::DEFENCE_RESULT_PRESSED_TOO_EARLY);
        guardSuperguardTimingPtr->setTimer(sysMsec2Frame(displayTime));
    }
    else if (lastBFrame > -1)
    {
        // Check if the attack can be superguarded or not
        if (weapon->superguards_allowed > 0)
        {
            // Print how many frames early the player pressed B
            guardSuperguardTimingPtr->setLastBFrame(lastBFrame);
            guardSuperguardTimingPtr->setType(GuardSuperguardTimingDisplay::DefenceResult::DEFENCE_RESULT_PRESSED_TOO_EARLY);
            guardSuperguardTimingPtr->setTimer(sysMsec2Frame(displayTime));
        }
        else
        {
            // The attack cannot be superguarded, so print the text saying so
            guardSuperguardTimingPtr->setType(GuardSuperguardTimingDisplay::DefenceResult::DEFENCE_RESULT_CANNOT_BE_SUPERGUARD);
            guardSuperguardTimingPtr->setTimer(sysMsec2Frame(displayTime));
        }
    }

    return ret;
}

static void handleGuardSuperguardTimings(Displays *displaysPtr)
{
    if (!displaysPtr->displayShouldBeHandled(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_GUARD_SUPERGUARD_TIMINGS))
    {
        return;
    }

    auto drawGuardSuperguardTimningsDisplay = []()
    {
        // This display draws a window under the text, and thus needs to re-initialize text drawing. This display also should be
        // drawn on top of other displays. Because of these, handle drawing it separately from the main drawing function, and
        // use a higher order.
        drawOnDebugLayer(drawGuardSuperguardTimings, DRAW_ORDER_DISPLAY_GUARD_SUPERGUARD_TIMINGS);
    };

    if (displaysPtr->getGuardSuperguardTimingDisplayPtr()->getTimer() > 0)
    {
        drawGuardSuperguardTimningsDisplay();
    }

    // Check if this display is in the process of being manually positioned or scaled
    else if (menuCurrentlyAdjustingPosOrScale(
                 DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_GUARD_SUPERGUARD_TIMINGS))
    {
        drawGuardSuperguardTimningsDisplay();
    }
}

// Credits to PistonMiner for writing the original code for this function
static void drawHitCheckVisualization(CameraId cameraId, void *user)
{
    (void)user;

    GXSetCullMode(GXCullMode::GX_CULL_NONE);

    // Configure vertex format
    GXClearVtxDesc();
    GXSetVtxDesc(GXAttribute::GX_VA_POS, GXAttributeType::GX_DIRECT); // Enable position, not indexed

    GXSetVtxAttrFmt(GXVtxFmt::GX_VTXFMT0,
                    GXAttribute::GX_VA_POS,
                    GXComponentContents::GX_POS_XYZ,
                    GXComponentType::GX_F32,
                    0); // xyz, float

    GXSetCullMode(GXCullMode::GX_CULL_NONE);

    // Configure vertex format
    GXClearVtxDesc();
    GXSetVtxDesc(GXAttribute::GX_VA_POS, GXAttributeType::GX_DIRECT); // Enable position, not indexed

    GXSetVtxAttrFmt(GXVtxFmt::GX_VTXFMT0,
                    GXAttribute::GX_VA_POS,
                    GXComponentContents::GX_POS_XYZ,
                    GXComponentType::GX_F32,
                    0); // xyz, float

    // Configure color output
    GXSetNumChans(1);

    GXSetChanCtrl(GXChannelID::GX_COLOR0A0,
                  false,
                  GXColorSrc::GX_SRC_REG,
                  GXColorSrc::GX_SRC_REG,
                  GXLightID::GX_LIGHT_NULL,
                  GXDiffuseFn::GX_DF_NONE,
                  GXAttnFn::GX_AF_NONE); // No lighting, use mat color from reg

    // Disable texgens
    GXSetNumTexGens(0);

    // Configure TEV to directly output raster color
    GXSetNumTevStages(1);

    GXSetTevOrder(GXTevStageID::GX_TEVSTAGE0,
                  GXTexCoordID::GX_TEXCOORD_NULL,
                  GXTexMapID::GX_TEXMAP_NULL,
                  GXChannelID::GX_COLOR0A0);

    GXSetTevOp(GXTevStageID::GX_TEVSTAGE0, GXTevMode::GX_PASSCLR); // Pass through color

    // Configure view transform
    GXLoadPosMtxImm(camGetPtr(cameraId)->view, GXTexMtxType::GX_MTX3x4);
    GXSetCurrentMtx(0);

    // Set line width to something reasonable
    GXSetLineWidth(9, GXTexOffset::GX_TO_ZERO);

    // Figure out how many entries we get
    HitCheckVisualizationDisplay *hitCheckVisualizationPtr = gDisplays->getHitCheckVisualizationDisplayPtr();
    uint32_t entryCount = hitCheckVisualizationPtr->getEntryCount();
    hitCheckVisualizationPtr->setEntryCount(0);

    if (entryCount > HIT_CHECK_BUFFER_CAPACITY)
    {
        entryCount = HIT_CHECK_BUFFER_CAPACITY;
    }

    // Ready to draw
    uint8_t *hitsColor = hitCheckVisualizationPtr->getHitsColorPtr();
    uint8_t *missesColor = hitCheckVisualizationPtr->getMissesColorPtr();
    const HitCheckResult *bufferPtr = hitCheckVisualizationPtr->getBufferPtr();

    for (uint32_t i = 0; i < entryCount; i++)
    {
        const HitCheckResult *currentEntry = &bufferPtr[i];

        // Get the line color
        uint8_t *lineColor;
        if (currentEntry->checkIfHit())
        {
            lineColor = hitsColor;
        }
        else
        {
            lineColor = missesColor;
        }

        // Set the line color
        GXSetChanMatColor(GXChannelID::GX_COLOR0A0, lineColor);

        // Draw the line
        GXBegin(GXPrimitive::GX_LINES, GXVtxFmt::GX_VTXFMT0, 2);

        const Vec3 *startPos = currentEntry->getStartPosPtr();
        GXPosition3f32(startPos->x, startPos->y, startPos->z);

        const Vec3 *endPos = currentEntry->getEndPosPtr();
        GXPosition3f32(endPos->x, endPos->y, endPos->z);
    }

    // All work is done, so free the memory used by the buffer
    hitCheckVisualizationPtr->freeBuffer();
}

// Credits to PistonMiner for writing the original code for this function
HitEntry *checkForVecHits(HitCheckQuery *pQuery, PFN_HitFilterFunction filterFunction)
{
    // Call the original function immediately, as its result is needed
    HitEntry *ret = g_hitCheckVecFilter_trampoline(pQuery, filterFunction);

    // Exit if the display is not enabled
    Displays *displaysPtr = gDisplays;
    if (!displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_HIT_CHECK_VISUALIZATION))
    {
        return ret;
    }

    // Do not run if the menu is currently open
    // Run if currently repositioning/scaling displays
    // Do not run if the pause menu is open
    // Do not run if the memory editor is currently open
    if ((gMenu && !gMod->flagIsSet(ModFlag::MOD_FLAG_MENU_IS_HIDDEN)) || ((marioStGetSystemLevel() & 15) == 15) ||
        memoryEditorIsOpen())
    {
        // Since the lines are not being drawn, reset the entry count to avoid drawing lines for outdated data
        displaysPtr->getHitCheckVisualizationDisplayPtr()->setEntryCount(0);
        return ret;
    }

    // If the current check is a hit and hits are disabled, then exit
    const bool shouldDrawHits = displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_SHOULD_DRAW_HITS);
    if (ret && !shouldDrawHits)
    {
        return ret;
    }

    // If the current check is a miss and misses are disabled, then exit
    const bool shouldDrawMisses = displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_SHOULD_DRAW_MISSES);
    if (!ret && !shouldDrawMisses)
    {
        return ret;
    }

    // If hits and misses are both disabled, then exit
    if (!shouldDrawHits && !shouldDrawMisses)
    {
        // Reset the entry count to be safe
        displaysPtr->getHitCheckVisualizationDisplayPtr()->setEntryCount(0);
        return ret;
    }

    // If there are no more entry slots available, then exit
    HitCheckVisualizationDisplay *hitCheckVisualizationPtr = displaysPtr->getHitCheckVisualizationDisplayPtr();
    const uint32_t entry = hitCheckVisualizationPtr->getEntryCount();

    if (entry >= HIT_CHECK_BUFFER_CAPACITY)
    {
        return ret;
    }

    // Current entry can be used, so update the entry count
    hitCheckVisualizationPtr->setEntryCount(entry + 1);

    // Get the data for the current entry
    bool hit;
    Vec3 timeoutEndPos;
    Vec3 *startPos = &pQuery->targetPosition;
    Vec3 *endPos;

    if (ret)
    {
        endPos = &pQuery->hitPosition;
        hit = true;
    }
    else
    {
        endPos = &timeoutEndPos;
        PSVECScale(&pQuery->targetDirection, endPos, pQuery->inOutTargetDistance);
        PSVECAdd(endPos, startPos, endPos);
        hit = false;
    }

    HitCheckResult *currentEntry = hitCheckVisualizationPtr->getBufferSlotPtr(entry);
    currentEntry->copyPosToStartPos(startPos);
    currentEntry->copyPosToEndPos(endPos);
    currentEntry->setHit(hit);

    return ret;
}

void handleHitCheckVisualization()
{
    Displays *displaysPtr = gDisplays;
    if (!displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_HIT_CHECK_VISUALIZATION))
    {
        return;
    }

    // Do not run if the menu is currently open
    // Run if currently repositioning/scaling displays
    if (gMenu && !gMod->flagIsSet(ModFlag::MOD_FLAG_MENU_IS_HIDDEN))
    {
        return;
    }

    // Do not run if the memory editor is currently open
    if (memoryEditorIsOpen())
    {
        return;
    }

    // Make sure there is at least one hit/miss to draw
    const HitCheckVisualizationDisplay *hitCheckVisualizationPtr = displaysPtr->getHitCheckVisualizationDisplayPtr();
    if (hitCheckVisualizationPtr->getEntryCount() == 0)
    {
        return;
    }

    // Make sure the buffer for the entries is set
    if (!hitCheckVisualizationPtr->getBufferPtr())
    {
        return;
    }

    // Specific dispEntry usage
    dispEntry(CameraId::k3d,
              10, // RenderMode::kAlphaBlendLate
              1.f,
              drawHitCheckVisualization,
              nullptr);
}

static void drawYoshiSkip(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Get the position and scale for the text
    DisplayManuallyPosition data;

    getDisplayPosAndScale(DisplaysScreenPosition::DISPLAYS_POSITION_BOTTOM_LEFT,
                          DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_YOSHI_SKIP,
                          &data);

    const float posX = data.getPosX();
    float posY = data.getPosY();
    const float scale = data.getScale();

    // Draw Mario's Coordinates
    drawMarioCoordinates(posX, posY, scale);

    const float lineIncrement = LINE_HEIGHT_FLOAT * scale;
    posY += lineIncrement;

    // Draw the Stick Angle
    drawStickAngle(posX, posY, scale);
    posY += lineIncrement;

    // Get the current timer value
    Displays *displaysPtr = gDisplays;
    YoshiSkipDisplay *yoshiSkipPtr = displaysPtr->getYoshiSkipDisplayPtr();
    const uint32_t timer = yoshiSkipPtr->getTimer();

    // Increment the timer if it is not stopped, since it needs to be done sometime in this function anyway, and it doesn't
    // matter when
    if (!displaysPtr->miscFlagIsSet(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_YOSHI_SKIP_TIMER_STOPPED))
    {
        yoshiSkipPtr->setTimer(timer + 1);
    }

    // Get the text
    char buf[64];
    snprintf(buf, sizeof(buf), "YST: %" PRIu32, timer);

    // Draw the text
    drawText(buf, posX, posY, scale, getColorWhite(0xFF));
}

static void handleYoshiSkip(Displays *displaysPtr)
{
    if (!displaysPtr->displayShouldBeHandled(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_YOSHI_SKIP))
    {
        return;
    }

    displaysPtr->setShouldDrawFlag(DisplaysShouldDrawFlag::DISPLAYS_SHOULD_DRAW_FLAG_YOSHI_SKIP);

    // This display draws the Stick Angle and Mario's Coordinates, so clear the draw flags for those to avoid duplicate draws
    displaysPtr->clearShouldDrawFlag(DisplaysShouldDrawFlag::DISPLAYS_SHOULD_DRAW_FLAG_STICK_ANGLE);
    displaysPtr->clearShouldDrawFlag(DisplaysShouldDrawFlag::DISPLAYS_SHOULD_DRAW_FLAG_MARIO_COORDINATES);

    // Only allow handling if the enabled flag is set
    if (!displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_YOSHI_SKIP))
    {
        return;
    }

    YoshiSkipDisplay *yoshiSkipPtr = displaysPtr->getYoshiSkipDisplayPtr();

    if (checkForSpecificSeq(SeqIndex::kBattle)) // In battle
    {
        // Pause in battle
        displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_YOSHI_SKIP_TIMER_STOPPED);
        displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_YOSHI_SKIP_TIMER_PAUSED);
    }
    else if (displaysPtr->miscFlagIsSet(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_YOSHI_SKIP_TIMER_PAUSED) &&
             (marioGetPtr()->flags1 & 0x1))
    {
        // Reset and Start when leaving battle
        yoshiSkipPtr->resetTimer();
        displaysPtr->clearMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_YOSHI_SKIP_TIMER_STOPPED);
        displaysPtr->clearMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_YOSHI_SKIP_TIMER_PAUSED);
    }

    if (checkButtonCombo(PadInput::PAD_A))
    {
        // Stop when A is pressed
        displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_YOSHI_SKIP_TIMER_STOPPED);
    }

    if (displaysPtr->checkDisplayButtonComboEveryFrame(DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_YOSHI_SKIP))
    {
        // Hold the button combo to increment the reset counter
        uint32_t counter = yoshiSkipPtr->getCounter();
        yoshiSkipPtr->setCounter(++counter);

        if (counter > sysMsec2Frame(2000))
        {
            // Reset the timer when the button combo is held for 2 seconds
            displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_YOSHI_SKIP_TIMER_STOPPED);
            yoshiSkipPtr->resetTimer();
            yoshiSkipPtr->resetCounter();
        }
    }
    else
    {
        yoshiSkipPtr->resetCounter();
    }
}

static void drawPalaceSkip(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Get the current timer value
    Displays *displaysPtr = gDisplays;
    PalaceSkipDisplay *palaceSkipPtr = displaysPtr->getPalaceSkipDisplayPtr();
    const uint32_t timer = palaceSkipPtr->getTimer();

    // Increment the timer if it is not stopped, since it needs to be done sometime in this function anyway, and it doesn't
    // matter when
    if (!displaysPtr->miscFlagIsSet(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_PALACE_SKIP_TIMER_STOPPED))
    {
        palaceSkipPtr->setTimer(timer + 1);
    }

    // Check to see if drawing the normal or minimal version of this display
    char buf[192];
    constexpr uint32_t bufSize = sizeof(buf);
    DisplayManuallyPosition data;

    if (displaysPtr->miscFlagIsSet(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_PALACE_SKIP_SHOULD_DRAW_MINIMAL))
    {
        // Only the timer is drawn for the minimal version, so just handle that now
        // Get the text
        snprintf(buf, bufSize, "PST: %" PRIu32, timer);

        // Get the position and scale for the text
        getDisplayPosAndScale(DisplaysScreenPosition::DISPLAYS_POSITION_BOTTOM_LEFT,
                              DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_PALACE_SKIP_MINIMAL,
                              &data);
        // Draw the text
        drawText(buf, data.getPosX(), data.getPosY(), data.getScale(), getColorWhite(0xFF));
        return;
    }

    // Get the position and scale for the text
    getDisplayPosAndScale(DisplaysScreenPosition::DISPLAYS_POSITION_BOTTOM_LEFT,
                          DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_PALACE_SKIP,
                          &data);

    // Draw Mario's Coordinates
    drawMarioCoordinates(data.getPosX(), data.getPosY(), data.getScale());

    // Get the Partner's Y coordinate
    const PartyEntry *partnerPtr = getPartnerPtr();
    float partnerPosY;

    if (partnerPtr)
    {
        partnerPosY = partnerPtr->partnerPosition.y;
    }
    else
    {
        partnerPosY = 0.f;
    }

    // Get the Phantom Ember's Y coordinate
    // Check if NPC 2 is active, followed by NPC 1, and then default to 0.f if neither is active
    NpcEntry *phantomEmberPtr = &npcWork[0].entries[1]; // NPC 2
    float phantomEmberPosY = 0.f;

    for (uint32_t i = 0; i < 2; i++, phantomEmberPtr--)
    {
        if (phantomEmberPtr->flags & 0x1)
        {
            phantomEmberPosY = phantomEmberPtr->position.y;
            break;
        }
    }

    // Loop through each item to find the one that was dropped by the Phantom Ember, and get the current despawn timer value for
    // it
    const ItemWork *itemWorkPtr = &itemDrvWork[0];
    const int32_t totalEntries = itemWorkPtr->count;
    ItemEntry *entryPtr = itemWorkPtr->entries;
    int32_t despawnTimer = 0;

    for (int32_t i = 0; i < totalEntries; i++, entryPtr++)
    {
        switch (static_cast<ItemId>(entryPtr->itemId))
        {
            case ItemId::ITEM_NONE:
            case ItemId::ITEM_COIN:
            case ItemId::ITEM_BATTLE_DROP_HEART:
            case ItemId::ITEM_BATTLE_DROP_FLOWER:
            {
                // Don't want this item, so go to the next one
                continue;
            }
            default:
            {
                break;
            }
        }

        // Check if the item was spawned upon entering the room
        if (entryPtr->mode == 7)
        {
            // The item was already spawned upon entering the room, so go to the next one
            continue;
        }

        // Check if item is currently spawned
        if (!(entryPtr->flags & 0x1))
        {
            // Item is not spawned, so go to the next one
            continue;
        }

        // Found the item we want, so assign the timer and exit the loop
        despawnTimer = entryPtr->wDespawnTimer;
        break;
    }

    // Get the text
    snprintf(buf,
             bufSize,
             "PST: %" PRIu32 "\nItemTimer: %" PRId32 "\nPhaEmY: %.2f\nParY: %.2f",
             timer,
             despawnTimer,
             phantomEmberPosY,
             partnerPosY);

    // Properly position the text above Mario's Coordinates
    float posY = data.getPosY();
    const float scale = data.getScale();
    posY += getTextMultilineIncrement(buf, scale, 1);

    // Draw the text
    drawText(buf, data.getPosX(), posY, scale, getColorWhite(0xFF));
}

static void handlePalaceSkip(Displays *displaysPtr)
{
    // Check to see if drawing the normal or minimal version of this display
    PalaceSkipDisplay *palaceSkipPtr = displaysPtr->getPalaceSkipDisplayPtr();
    uint32_t enabledFlag;

    if (displaysPtr->displayShouldBeHandled(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_PALACE_SKIP))
    {
        displaysPtr->clearMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_PALACE_SKIP_SHOULD_DRAW_MINIMAL);
        enabledFlag = DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_PALACE_SKIP;
    }
    else
    {
        enabledFlag = DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_PALACE_SKIP_MINIMAL;

        if (displaysPtr->displayShouldBeHandled(enabledFlag))
        {
            displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_PALACE_SKIP_SHOULD_DRAW_MINIMAL);
        }
        else
        {
            return;
        }
    }

    displaysPtr->setShouldDrawFlag(DisplaysShouldDrawFlag::DISPLAYS_SHOULD_DRAW_FLAG_PALACE_SKIP);

    if (enabledFlag == DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_PALACE_SKIP)
    {
        // The normal version of this display draws Mario's Coordinates, so clear the draw flag for that to avoid duplicate
        // draws
        displaysPtr->clearShouldDrawFlag(DisplaysShouldDrawFlag::DISPLAYS_SHOULD_DRAW_FLAG_MARIO_COORDINATES);
    }

    // Only allow handling if the enabled flag is set
    if (!displaysPtr->enabledFlagIsSet(enabledFlag))
    {
        return;
    }

    if ((marioStGetSystemLevel() & 15) == 15)
    {
        // Stop upon pausing
        displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_PALACE_SKIP_TIMER_STOPPED);
        displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_PALACE_SKIP_TIMER_PAUSED);
    }
    else if (displaysPtr->miscFlagIsSet(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_PALACE_SKIP_TIMER_PAUSED) && systemLevelIsZero())
    {
        // Reset and Start when unpausing
        palaceSkipPtr->resetTimer();
        displaysPtr->clearMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_PALACE_SKIP_TIMER_STOPPED);
        displaysPtr->clearMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_PALACE_SKIP_TIMER_PAUSED);
    }

    if (checkButtonCombo(PadInput::PAD_X))
    {
        // Stop when pressing X
        displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_PALACE_SKIP_TIMER_STOPPED);
    }

    // Get the button combo enum for the current Palace Skip being used
    uint32_t buttonComboEnum;
    if (enabledFlag == DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_PALACE_SKIP)
    {
        buttonComboEnum = DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_PALACE_SKIP;
    }
    else
    {
        buttonComboEnum = DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_PALACE_SKIP_MINIMAL;
    }

    if (displaysPtr->checkDisplayButtonComboEveryFrame(buttonComboEnum))
    {
        // Hold the button combo to increment the reset counter
        uint32_t counter = palaceSkipPtr->getCounter();
        palaceSkipPtr->setCounter(++counter);

        if (counter > sysMsec2Frame(2000))
        {
            // Reset the timer when the button combo is held for 2 seconds
            displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_PALACE_SKIP_TIMER_STOPPED);
            palaceSkipPtr->resetTimer();
            palaceSkipPtr->resetCounter();
        }
    }
    else
    {
        palaceSkipPtr->resetCounter();
    }
}

#ifdef TTYD_JP
static void drawAMWSpinJump(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    Displays *displaysPtr = gDisplays;
    AMWSpinJumpDisplay *amwSpinJumpPtr = displaysPtr->getAMWSpinJumpDisplay();

    // Increment the pause timer if it is not stopped, since it needs to be done sometime in this function anyway, and it
    // doesn't matter when
    const uint32_t pauseTimer = amwSpinJumpPtr->getPauseTimer();
    if (!displaysPtr->miscFlagIsSet(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_AMW_SPIN_JUMP_PAUSE_TIMER_STOPPED))
    {
        amwSpinJumpPtr->setPauseTimer(pauseTimer + 1);
    }

    // Increment the Spin Jump timer if it is not stopped, since it needs to be done sometime in this function anyway, and it
    // doesn't matter when
    const uint32_t spinJumpTimer = amwSpinJumpPtr->getSpinJumpTimer();
    if (!displaysPtr->miscFlagIsSet(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_AMW_SPIN_JUMP_SPIN_JUMP_TIMER_STOPPED))
    {
        amwSpinJumpPtr->setSpinJumpTimer(spinJumpTimer + 1);
    }

    // Get the position and scale for the text
    DisplayManuallyPosition data;

    getDisplayPosAndScale(DisplaysScreenPosition::DISPLAYS_POSITION_BOTTOM_LEFT,
                          DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_AMW_SPIN_JUMP,
                          &data);

    // Get the color to use for the Z coordinate
    const uint32_t *marioPosZPtr = reinterpret_cast<const uint32_t *>(&marioGetPtr()->playerPosition.z);
    const uint32_t marioPosZ = *marioPosZPtr;
    uint32_t posZColor;

    if (marioPosZ == 0xC25A5307) // 1st pause
    {
        posZColor = getColorLightOrange(0xFF);
    }
    else if (marioPosZ == 0xC25A06E3) // 2nd pause
    {
        posZColor = getColorGreen(0xFF);
    }
    else
    {
        posZColor = getColorWhite(0xFF);
    }

    // Get the text
    char buf[64];

    snprintf(buf,
             sizeof(buf),
             "PT: %" PRIu32 "\nSJT: %" PRIu32 "\nPos Z: <col %" PRIx32 ">0x%08" PRIX32,
             pauseTimer,
             spinJumpTimer,
             posZColor,
             marioPosZ);

    // Properly position the text
    float posY = data.getPosY();
    const float scale = data.getScale();
    posY += getTextMultilineIncrement(buf, scale, 0);

    // Draw the text
    drawText(buf, data.getPosX(), posY, scale, getColorWhite(0xFF));
}

static void handleAMWSpinJump(Displays *displaysPtr)
{
    if (!displaysPtr->displayShouldBeHandled(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_AMW_SPIN_JUMP))
    {
        return;
    }

    displaysPtr->setShouldDrawFlag(DisplaysShouldDrawFlag::DISPLAYS_SHOULD_DRAW_FLAG_AMW_SPIN_JUMP);

    // Only allow handling if the enabled flag is set
    if (!displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_AMW_SPIN_JUMP))
    {
        return;
    }

    AMWSpinJumpDisplay *amwSpinJumpPtr = displaysPtr->getAMWSpinJumpDisplay();

    if ((marioStGetSystemLevel() & 15) == 15)
    {
        // Stop both timers upon pausing
        displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_AMW_SPIN_JUMP_PAUSE_TIMER_STOPPED);
        displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_AMW_SPIN_JUMP_PAUSE_TIMER_PAUSED);
        displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_AMW_SPIN_JUMP_SPIN_JUMP_TIMER_STOPPED);
    }
    else if (displaysPtr->miscFlagIsSet(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_AMW_SPIN_JUMP_PAUSE_TIMER_PAUSED) &&
             systemLevelIsZero())
    {
        // Reset the pause timer and Start/Unpause both timers when unpausing
        amwSpinJumpPtr->resetPauseTimer();
        displaysPtr->clearMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_AMW_SPIN_JUMP_PAUSE_TIMER_STOPPED);
        displaysPtr->clearMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_AMW_SPIN_JUMP_PAUSE_TIMER_PAUSED);
        displaysPtr->clearMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_AMW_SPIN_JUMP_SPIN_JUMP_TIMER_STOPPED);
    }

    const MarioMotion marioCurrentMotion = marioGetPtr()->currentMotionId;

    if (displaysPtr->miscFlagIsSet(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_AMW_SPIN_JUMP_SPIN_JUMP_TIMER_PAUSED))
    {
        // Check if the Spin Jump timer should be started
        if (marioCurrentMotion == MarioMotion::kHip)
        {
            // Reset and start the Spin Jump timer
            amwSpinJumpPtr->resetSpinJumpTimer();
            displaysPtr->clearMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_AMW_SPIN_JUMP_SPIN_JUMP_TIMER_STOPPED);
            displaysPtr->clearMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_AMW_SPIN_JUMP_SPIN_JUMP_TIMER_PAUSED);
        }
    }
    else if (marioCurrentMotion == MarioMotion::kJump)
    {
        // Just started another jump, so pause the Spin Jump timer
        displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_AMW_SPIN_JUMP_SPIN_JUMP_TIMER_STOPPED);
        displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_AMW_SPIN_JUMP_SPIN_JUMP_TIMER_PAUSED);
    }

    if (seqGetNextSeq() == SeqIndex::kBattle) // Battle is initialized
    {
        // Stop the timers when a battle has been initialized
        displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_AMW_SPIN_JUMP_PAUSE_TIMER_STOPPED);
        displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_AMW_SPIN_JUMP_SPIN_JUMP_TIMER_STOPPED);
    }

    if (displaysPtr->checkDisplayButtonComboEveryFrame(DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_AMW_SPIN_JUMP))
    {
        // Hold the button combo to increment the reset counter
        uint32_t counter = amwSpinJumpPtr->getCounter();
        amwSpinJumpPtr->setCounter(++counter);

        if (counter > sysMsec2Frame(2000))
        {
            // Reset the timers when the button combo is held for 2 seconds
            displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_AMW_SPIN_JUMP_PAUSE_TIMER_STOPPED);
            displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_AMW_SPIN_JUMP_SPIN_JUMP_TIMER_STOPPED);
            amwSpinJumpPtr->resetTimers();
            amwSpinJumpPtr->resetCounter();
        }
    }
    else
    {
        amwSpinJumpPtr->resetCounter();
    }
}
#endif

static void drawJabbiHiveSkip(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Check which buttons were pressed first, or if at the same time
    const char *buttonsPressedFirstPtr;
    char buttonsPressedFirstBuf[32];
    constexpr uint32_t buttonsPressedFirstBufSize = sizeof(buttonsPressedFirstBuf);

    Displays *displaysPtr = gDisplays;
    const JabbiHiveSkipDisplay *jabbiHiveSkipPtr = displaysPtr->getJabbiHiveSkipDisplayPtr();

    if (!displaysPtr->miscFlagIsSet(
            DisplaysMiscFlag::DISPLAYS_MISC_FLAG_JABBI_HIVE_SKIP_INITIAL_OPEN_PAUSE_MENU_BUTTON_PRESSED))
    {
        // No buttons have been pressed yet
        buttonsPressedFirstPtr = "Nothing pressed yet";
    }
    else
    {
        const uint32_t buttonsPressed = jabbiHiveSkipPtr->getButtonPressedFirst();
        if (buttonsPressed == 0)
        {
            // Pressed at the same time
            buttonsPressedFirstPtr = "Pressed at same time";
        }
        else
        {
            const char *button;
            if (buttonsPressed == PadInput::PAD_A)
            {
                // Pressed A first
                button = "A";
            }
            else // if (buttonsPressed == PadInput::PAD_DPAD_LEFT)
            {
                // Pressed D-Pad Left first
                button = "D-Pad Left";
            }

            snprintf(buttonsPressedFirstBuf, buttonsPressedFirstBufSize, "Pressed %s first", button);
            buttonsPressedFirstPtr = buttonsPressedFirstBuf;
        }
    }

    // Get the text
    // Draw the text
    char buf[64 + buttonsPressedFirstBufSize];
    snprintf(buf,
             sizeof(buf),
             "%s\nFBP: %" PRIu32 "\nFBUP: %" PRIu32,
             buttonsPressedFirstPtr,
             jabbiHiveSkipPtr->getInitialOpenPauseMenuFrames(),
             jabbiHiveSkipPtr->getPauseMenuOpenFrames());

    // Get the position and scale for the text
    DisplayManuallyPosition data;

    getDisplayPosAndScale(DisplaysScreenPosition::DISPLAYS_POSITION_BOTTOM_LEFT,
                          DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_JABBI_HIVE_SKIP,
                          &data);

    // Properly position the text
    float posY = data.getPosY();
    const float scale = data.getScale();
    posY += getTextMultilineIncrement(buf, scale, 0);

    // Draw the text
    drawText(buf, data.getPosX(), posY, data.getScale(), getColorWhite(0xFF));
}

static void handleJabbiHiveSkip(Displays *displaysPtr)
{
    if (!displaysPtr->displayShouldBeHandled(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_JABBI_HIVE_SKIP))
    {
        return;
    }

    displaysPtr->setShouldDrawFlag(DisplaysShouldDrawFlag::DISPLAYS_SHOULD_DRAW_FLAG_JABBI_HIVE_SKIP);

    // Only allow handling if the enabled flag is set
    if (!displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_JABBI_HIVE_SKIP))
    {
        return;
    }

    const Player *playerPtr = marioGetPtr();
    const uint32_t buttonInputTrg = keyGetButtonTrg(PadId::CONTROLLER_ONE);
    JabbiHiveSkipDisplay *jabbiHiveSkipPtr = displaysPtr->getJabbiHiveSkipDisplayPtr();

    // If Mario is in Paper Mode, both timers are stopped, flagsSetTimer is at 450 milliseconds or higher, and D-Pad Left or A
    // was pressed, then reset everything before continuing
    const bool inPaperMode = (playerPtr->currentMotionId == MarioMotion::kSlit) ||
                             ((playerPtr->currentMotionId == MarioMotion::kJump) && (playerPtr->flags1 & 0x100000));

    constexpr uint32_t possibleButtons = PadInput::PAD_DPAD_LEFT | PadInput::PAD_A;
    const uint32_t buttonsCheck = buttonInputTrg & possibleButtons;

    auto clearJabbiHiveSkipVars = [](JabbiHiveSkipDisplay *jabbiHiveSkipPtr, Displays *displaysPtr)
    {
        displaysPtr->clearMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_JABBI_HIVE_SKIP_INITIAL_OPEN_PAUSE_MENU_BUTTON_PRESSED);
        displaysPtr->clearMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_JABBI_HIVE_SKIP_INITIAL_OPEN_PAUSE_MENU_FRAMES_STOPPED);
        displaysPtr->clearMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_JABBI_HIVE_SKIP_PAUSE_MENU_OPEN_FRAMES_STOPPED);
        clearMemory(jabbiHiveSkipPtr, sizeof(JabbiHiveSkipDisplay));
    };

    // Check if Mario is in Paper Mode
    if (inPaperMode)
    {
        // Check if both timers are stopped
        if (displaysPtr->miscFlagIsSet(
                DisplaysMiscFlag::DISPLAYS_MISC_FLAG_JABBI_HIVE_SKIP_INITIAL_OPEN_PAUSE_MENU_FRAMES_STOPPED) &&
            displaysPtr->miscFlagIsSet(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_JABBI_HIVE_SKIP_PAUSE_MENU_OPEN_FRAMES_STOPPED))
        {
            // Check if flagsSetTimer is at 450 milliseconds or higher
            if (jabbiHiveSkipPtr->getFlagsSetTimer() >= sysMsec2Frame(450))
            {
                // Check if D-Pad Left or A was pressed
                if (buttonsCheck)
                {
                    clearJabbiHiveSkipVars(jabbiHiveSkipPtr, displaysPtr);
                }
            }
        }
    }

    // Check for opening the pause menu
    if (!displaysPtr->miscFlagIsSet(
            DisplaysMiscFlag::DISPLAYS_MISC_FLAG_JABBI_HIVE_SKIP_INITIAL_OPEN_PAUSE_MENU_FRAMES_STOPPED))
    {
        // Check if either buttons have been pressed yet
        if (!displaysPtr->miscFlagIsSet(
                DisplaysMiscFlag::DISPLAYS_MISC_FLAG_JABBI_HIVE_SKIP_INITIAL_OPEN_PAUSE_MENU_BUTTON_PRESSED))
        {
            // Only check for buttons if Mario is currently in Paper Mode
            // Check if D-Pad Left or A was pressed
            if (inPaperMode && buttonsCheck)
            {
                displaysPtr->setMiscFlag(
                    DisplaysMiscFlag::DISPLAYS_MISC_FLAG_JABBI_HIVE_SKIP_INITIAL_OPEN_PAUSE_MENU_BUTTON_PRESSED);

                // Figure out which was pressed first, or if both pressed at the same time
                if (buttonsCheck == possibleButtons)
                {
                    // Pressed at the same time
                    jabbiHiveSkipPtr->setButtonPressedFirst(0);

                    displaysPtr->setMiscFlag(
                        DisplaysMiscFlag::DISPLAYS_MISC_FLAG_JABBI_HIVE_SKIP_INITIAL_OPEN_PAUSE_MENU_FRAMES_STOPPED);
                }
                else if (buttonInputTrg & PadInput::PAD_DPAD_LEFT)
                {
                    // D-Pad Left pressed first
                    jabbiHiveSkipPtr->setButtonPressedFirst(PadInput::PAD_DPAD_LEFT);

                    // Increment the timer
                    jabbiHiveSkipPtr->incrementInitialOpenPauseMenuFrames();
                }
                else // if (buttonInputTrg & PadInput::PAD_A)
                {
                    // A pressed first
                    jabbiHiveSkipPtr->setButtonPressedFirst(PadInput::PAD_A);

                    // Increment the timer
                    jabbiHiveSkipPtr->incrementInitialOpenPauseMenuFrames();
                }
            }
        }
        else
        {
            // Check for the next button pressed
            const uint32_t buttonPressedFirst = jabbiHiveSkipPtr->getButtonPressedFirst();

            if ((buttonPressedFirst == PAD_A) && (buttonInputTrg == PAD_DPAD_LEFT))
            {
                // A was pressed first, and D-Pad Left was just pressed
                displaysPtr->setMiscFlag(
                    DisplaysMiscFlag::DISPLAYS_MISC_FLAG_JABBI_HIVE_SKIP_INITIAL_OPEN_PAUSE_MENU_FRAMES_STOPPED);

                // Increment the timer for the pause menu being open
                jabbiHiveSkipPtr->incrementPauseMenuOpenFrames();
            }
            else if ((buttonPressedFirst == PAD_DPAD_LEFT) && (buttonInputTrg == PAD_A))
            {
                // D-Pad Left was pressed first, and A was just pressed
                displaysPtr->setMiscFlag(
                    DisplaysMiscFlag::DISPLAYS_MISC_FLAG_JABBI_HIVE_SKIP_INITIAL_OPEN_PAUSE_MENU_FRAMES_STOPPED);
            }
            else
            {
                // Increment the timer
                jabbiHiveSkipPtr->incrementInitialOpenPauseMenuFrames();
            }
        }
    }
    else if (!displaysPtr->miscFlagIsSet(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_JABBI_HIVE_SKIP_PAUSE_MENU_OPEN_FRAMES_STOPPED))
    {
        // Check if A was pressed
        if (buttonInputTrg & PadInput::PAD_A)
        {
            displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_JABBI_HIVE_SKIP_PAUSE_MENU_OPEN_FRAMES_STOPPED);
        }
        else
        {
            // Increment the timer
            jabbiHiveSkipPtr->incrementPauseMenuOpenFrames();
        }
    }
    else
    {
        // Both flags are set, so increment flagsSetTimer
        // Do not increment if 450 milliseconds have been reached
        uint32_t flagSetTimer = jabbiHiveSkipPtr->getFlagsSetTimer();
        if (flagSetTimer < sysMsec2Frame(450))
        {
            jabbiHiveSkipPtr->setFlagsSetTimer(++flagSetTimer);
        }
    }

    if (displaysPtr->checkDisplayButtonCombo(DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_JABBI_HIVE_SKIP))
    {
        // Increment the reset counter if the button combo is pressed
        jabbiHiveSkipPtr->incrementCounter();

        if (jabbiHiveSkipPtr->shouldResetEverything())
        {
            // Reset everything if the button combo is pressed 3 times consecutively
            clearJabbiHiveSkipVars(jabbiHiveSkipPtr, displaysPtr);
        }
    }
    else if (buttonInputTrg != 0)
    {
        // Reset the reset counter if any other buttons are pressed
        jabbiHiveSkipPtr->resetCounter();
    }
}

static void drawBridgeSkip(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Get the position and scale for the text
    DisplayManuallyPosition data;

    getDisplayPosAndScale(DisplaysScreenPosition::DISPLAYS_POSITION_BOTTOM_LEFT,
                          DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_BRIDGE_SKIP,
                          &data);

    // Draw Mario's Coordinates
    drawMarioCoordinates(data.getPosX(), data.getPosY(), data.getScale());

    // Get respawnAreaPtrRaw now to avoid backing up other variables on the stack later
    const uint32_t respawnAreaPtrRaw = reinterpret_cast<uint32_t>(marioGetPtr()->wObjHazardRespawn);

    // Get the current timer value
    Displays *displaysPtr = gDisplays;
    BridgeSkipDisplay *bridgeSkipPtr = displaysPtr->getBridgeSkipDisplayPtr();
    uint32_t mainTimer = bridgeSkipPtr->getTimer();

    // Increment the main timer, since it needs to be done sometime in this function anyway, and it doesn't matter when
    if (!displaysPtr->miscFlagIsSet(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BRIDGE_SKIP_TIMER_PAUSED) &&
        !displaysPtr->miscFlagIsSet(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BRIDGE_SKIP_TIMER_STOPPED))
    {
        bridgeSkipPtr->setTimer(mainTimer + 1);
    }

    // Check whether A was pressed early or late
    const char *earlyOrLateText;

    if (!displaysPtr->miscFlagIsSet(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BRIDGE_SKIP_PRESSED_EARLY))
    {
        earlyOrLateText = "Late";
    }
    else
    {
        // Increment the timer by 1 to account for being early by X frames
        mainTimer++;
        earlyOrLateText = "Early";
    }

    // Check if the frames text should be plural
    const char *pluralText;
    if (mainTimer == 1)
    {
        pluralText = nullptr;
    }
    else
    {
        pluralText = "s";
    }

    // Get the text
    char buf[96];
    snprintf(buf,
             sizeof(buf),
             "%" PRIu32 " Frame%s %s\nHRP: 0x%08" PRIX32,
             mainTimer,
             pluralText,
             earlyOrLateText,
             respawnAreaPtrRaw);

    // Properly position the text above Mario's Coordinates
    float posY = data.getPosY();
    const float scale = data.getScale();
    posY += getTextMultilineIncrement(buf, scale, 1);

    // Draw the text
    drawText(buf, data.getPosX(), posY, scale, getColorWhite(0xFF));
}

#ifdef TTYD_EU
void *cJumpOnWater(void *ptr)
{
    // Allow jumping on water if the Bridge Skip display is enabled
    if (gDisplays->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_BRIDGE_SKIP))
    {
        return nullptr;
    }
    else
    {
        // Return the original value
        return ptr;
    }
}
#endif

static void handleBridgeSkip(Displays *displaysPtr)
{
    if (!displaysPtr->displayShouldBeHandled(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_BRIDGE_SKIP))
    {
        return;
    }

    displaysPtr->setShouldDrawFlag(DisplaysShouldDrawFlag::DISPLAYS_SHOULD_DRAW_FLAG_BRIDGE_SKIP);

    // This display draws Mario's Coordinates, so clear the draw flag for that to avoid duplicate draws
    displaysPtr->clearShouldDrawFlag(DisplaysShouldDrawFlag::DISPLAYS_SHOULD_DRAW_FLAG_MARIO_COORDINATES);

    // Only allow handling if the enabled flag is set
    if (!displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_BRIDGE_SKIP))
    {
        return;
    }

    const uint32_t flags = marioGetPtr()->flags2;
    BridgeSkipDisplay *bridgeSkipPtr = displaysPtr->getBridgeSkipDisplayPtr();

    const bool unkNoLongerOnGround = flags & 0x10000000;
    const bool unkNotAbleToJump = flags & 0x10000;

    if (!unkNoLongerOnGround && unkNotAbleToJump) // Done falling, now starting to stand up
    {
        if (!displaysPtr->miscFlagIsSet(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BRIDGE_SKIP_PRESSED_EARLY))
        {
            // Reset the timer upon landing and while not pressing A
            bridgeSkipPtr->resetTimer();

            // Check if A was pressed too early
            if (displaysPtr->miscFlagIsSet(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BRIDGE_SKIP_TIMER_PAUSED) &&
                displaysPtr->miscFlagIsSet(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BRIDGE_SKIP_TIMER_STOPPED))
            {
                // Pressed A too early, so start the timer
                displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BRIDGE_SKIP_PRESSED_EARLY);
                displaysPtr->clearMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BRIDGE_SKIP_TIMER_PAUSED);
                displaysPtr->clearMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BRIDGE_SKIP_TIMER_STOPPED);
            }
            else
            {
                // Stop the timer
                displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BRIDGE_SKIP_TIMER_PAUSED);
                displaysPtr->clearMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BRIDGE_SKIP_TIMER_STOPPED);
            }
        }
        else if (displaysPtr->miscFlagIsSet(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BRIDGE_SKIP_PRESSED_EARLY_SHOULD_DISABLE))
        {
            // Reset the early state
            displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BRIDGE_SKIP_TIMER_PAUSED);
            displaysPtr->clearMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BRIDGE_SKIP_PRESSED_EARLY);
            displaysPtr->clearMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BRIDGE_SKIP_PRESSED_EARLY_SHOULD_DISABLE);
            displaysPtr->clearMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BRIDGE_SKIP_TIMER_STOPPED);
        }
        else
        {
            // The timer is currently running
        }
    }
    else if (!unkNoLongerOnGround && !unkNotAbleToJump) // A timing, start timer
    {
        if (!displaysPtr->miscFlagIsSet(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BRIDGE_SKIP_PRESSED_EARLY))
        {
            // Prevents running when the display is initially enabled
            if (displaysPtr->miscFlagIsSet(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BRIDGE_SKIP_TIMER_PAUSED))
            {
                if (!displaysPtr->miscFlagIsSet(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BRIDGE_SKIP_TIMER_STOPPED))
                {
                    // Didn't press A, so start the timer
                    displaysPtr->clearMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BRIDGE_SKIP_TIMER_PAUSED);
                }
                else if (!(flags & 0x20000)) // unkCurrentlyJumping
                {
                    // Pressed A one frame early
                    displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BRIDGE_SKIP_PRESSED_EARLY);
                    displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BRIDGE_SKIP_PRESSED_EARLY_SHOULD_DISABLE);
                }
                else
                {
                    // Pressed A on the correct frame, so leave the timer paused
                }
            }
        }
        else
        {
            // Stop the timer
            displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BRIDGE_SKIP_TIMER_PAUSED);
            displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BRIDGE_SKIP_PRESSED_EARLY_SHOULD_DISABLE);
        }
    }

    if (checkButtonCombo(PadInput::PAD_A))
    {
        // Stop when A is pressed
        displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BRIDGE_SKIP_TIMER_STOPPED);
    }

    if (displaysPtr->checkDisplayButtonComboEveryFrame(DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_BRIDGE_SKIP))
    {
        // Hold the button combo to increment the reset counter
        uint32_t counter = bridgeSkipPtr->getCounter();
        bridgeSkipPtr->setCounter(++counter);

        if (counter > sysMsec2Frame(2000))
        {
            // Reset the timer when the button combo is held for 2 seconds
            bridgeSkipPtr->resetTimer();
            bridgeSkipPtr->resetCounter();
            displaysPtr->clearMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BRIDGE_SKIP_PRESSED_EARLY);
            displaysPtr->clearMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BRIDGE_SKIP_PRESSED_EARLY_SHOULD_DISABLE);
            displaysPtr->clearMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BRIDGE_SKIP_TIMER_PAUSED);
            displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BRIDGE_SKIP_TIMER_STOPPED);
        }
    }
    else
    {
        bridgeSkipPtr->resetCounter();
    }
}

static void drawBlimpTicketSkip(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Get the position and scale for the text
    DisplayManuallyPosition data;

    getDisplayPosAndScale(DisplaysScreenPosition::DISPLAYS_POSITION_BOTTOM_LEFT,
                          DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_BLIMP_TICKET_SKIP,
                          &data);

    const float posX = data.getPosX();
    float posY = data.getPosY();
    const float scale = data.getScale();

    // Draw Mario's Coordinates
    drawMarioCoordinates(posX, posY, scale);

    const float lineIncrement = LINE_HEIGHT_FLOAT * scale;
    posY += lineIncrement;

    // Draw the Stick Angle
    drawStickAngle(posX, posY, scale);
    posY += lineIncrement;

    // Get the timer values now
    Displays *displaysPtr = gDisplays;
    BlimpTicketSkipDisplay *blimpTicketSkipPtr = displaysPtr->getBlimpSkipDisplayPtr();

    const uint32_t upRightTimer = blimpTicketSkipPtr->getUpRightTimer();
    const uint32_t straightUpTimer = blimpTicketSkipPtr->getStraightUpTimer();

    // Increment the timers now, since they need to be incremented sometime in this function anyway, and it doesn't matter when
    if (!displaysPtr->miscFlagIsSet(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BLIMP_SKIP_UP_RIGHT_TIMER_STOPPED))
    {
        blimpTicketSkipPtr->setUpRightTimer(upRightTimer + 1);
    }

    if (!displaysPtr->miscFlagIsSet(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BLIMP_SKIP_STRAIGHT_UP_TIMER_STOPPED))
    {
        blimpTicketSkipPtr->setStraightUpTimer(straightUpTimer + 1);
    }

    // Get the text
    char buf[64];
    snprintf(buf, sizeof(buf), "URT: %" PRIu32 "\nSUT: %" PRIu32, upRightTimer, straightUpTimer);

    // Properly position the text above Mario's Coordinates
    posY += getTextMultilineIncrement(buf, scale, 0);

    // Draw the text
    drawText(buf, posX, posY, scale, getColorWhite(0xFF));
}

static void handleBlimpTicketSkip(Displays *displaysPtr)
{
    if (!displaysPtr->displayShouldBeHandled(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_BLIMP_TICKET_SKIP))
    {
        return;
    }

    displaysPtr->setShouldDrawFlag(DisplaysShouldDrawFlag::DISPLAYS_SHOULD_DRAW_FLAG_BLIMP_TICKET_SKIP);

    // This display draws the Stick Angle and Mario's Coordinates, so clear the draw flags for those to avoid duplicate draws
    displaysPtr->clearShouldDrawFlag(DisplaysShouldDrawFlag::DISPLAYS_SHOULD_DRAW_FLAG_STICK_ANGLE);
    displaysPtr->clearShouldDrawFlag(DisplaysShouldDrawFlag::DISPLAYS_SHOULD_DRAW_FLAG_MARIO_COORDINATES);

    // Only allow handling if the enabled flag is set
    if (!displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_BLIMP_TICKET_SKIP))
    {
        return;
    }

    BlimpTicketSkipDisplay *blimpTicketSkipPtr = displaysPtr->getBlimpSkipDisplayPtr();

    if ((marioStGetSystemLevel() & 15) == 15)
    {
        // Stop upon pausing
        displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BLIMP_SKIP_TIMERS_PAUSED);
        displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BLIMP_SKIP_UP_RIGHT_TIMER_STOPPED);
        displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BLIMP_SKIP_STRAIGHT_UP_TIMER_STOPPED);
    }
    else if (displaysPtr->miscFlagIsSet(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BLIMP_SKIP_TIMERS_PAUSED) && systemLevelIsZero())
    {
        // Reset and Start when unpausing
        blimpTicketSkipPtr->resetUpRightTimer();
        blimpTicketSkipPtr->resetStraightUpTimer();
        displaysPtr->clearMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BLIMP_SKIP_TIMERS_PAUSED);
        displaysPtr->clearMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BLIMP_SKIP_UP_RIGHT_TIMER_STOPPED);
        displaysPtr->clearMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BLIMP_SKIP_STRAIGHT_UP_TIMER_STOPPED);
    }

    double stickAngle;
    double upRightAngleStart = 25.0;

#ifndef TTYD_JP
    upRightAngleStart += 2.0;
#endif

    if (getStickAngle(nullptr, &stickAngle))
    {
        if (stickAngle >= upRightAngleStart)
        {
            // Stop the up-right timer when the angle held reaches or exceeds 25 for JP, or 27 for US/EU
            displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BLIMP_SKIP_UP_RIGHT_TIMER_STOPPED);
        }
        else if (stickAngle == 0.0)
        {
            // Stop the straight-up timer when the angle held is 0
            displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BLIMP_SKIP_STRAIGHT_UP_TIMER_STOPPED);
        }
    }

    if (displaysPtr->checkDisplayButtonComboEveryFrame(DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_BLIMP_TICKET_SKIP))
    {
        // Hold the button combo to increment the reset counter
        uint32_t counter = blimpTicketSkipPtr->getCounter();
        blimpTicketSkipPtr->setCounter(++counter);

        if (counter > sysMsec2Frame(2000))
        {
            // Reset the timers when button is held for 2 seconds
            blimpTicketSkipPtr->resetUpRightTimer();
            blimpTicketSkipPtr->resetStraightUpTimer();
            blimpTicketSkipPtr->resetCounter();
            displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BLIMP_SKIP_UP_RIGHT_TIMER_STOPPED);
            displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_BLIMP_SKIP_STRAIGHT_UP_TIMER_STOPPED);
        }
    }
    else
    {
        blimpTicketSkipPtr->resetCounter();
    }
}

static void drawNpcNameToPtrError(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Initialize text drawing
    drawTextInit(true);

    // Get the current counter value
    Displays *displaysPtr = gDisplays;
    NpcNameToPtrErrorDisplay *npcNameToPtrErrorPtr = displaysPtr->getNpcNameToPtrErrorDisplayPtr();
    const uint32_t counter = npcNameToPtrErrorPtr->getCounter();

    // If the timer is at 0, then reset the counter
    if (npcNameToPtrErrorPtr->getTimer() == 0)
    {
        npcNameToPtrErrorPtr->resetCounter();
    }

    // Get the text
    char buf[64];
    snprintf(buf, sizeof(buf), "npcNameToPtr error occured x%" PRIu32, counter);

    // Draw the text
    drawText(buf,
             DISPLAYS_DEFAULT_POS_X_LEFT,
             displaysPtr->getErrorTextPosYDecrement(),
             DISPLAYS_DEFAULT_SCALE_ERRORS,
             getColorWhite(0xFF));
}

NpcEntry *checkForNpcNameToPtrError(const char *name)
{
    // Call the original function immediately, as its result is needed
    NpcEntry *ret = g_npcNameToPtr_trampoline(name);

    // Check if the returned pointer is valid
    const NpcWork *workPtr = npcGetWorkPtr();
    if (ret >= &workPtr->entries[workPtr->npcMaxCount])
    {
        // Didn't find the correct NPC, so print error text
        NpcNameToPtrErrorDisplay *npcNameToPtrErrorPtr = gDisplays->getNpcNameToPtrErrorDisplayPtr();
        npcNameToPtrErrorPtr->setTimer(sysMsec2Frame(5000));
        npcNameToPtrErrorPtr->incrementCounter();
    }

    return ret;
}

static void handleNpcNameToPtrError(Displays *displaysPtr)
{
    NpcNameToPtrErrorDisplay *npcNameToPtrErrorPtr = gDisplays->getNpcNameToPtrErrorDisplayPtr();
    uint32_t timer = npcNameToPtrErrorPtr->getTimer();

    if (timer > 0)
    {
        npcNameToPtrErrorPtr->setTimer(--timer);
        drawOnDebugLayer(drawNpcNameToPtrError, displaysPtr->getErrorTextOrder());
    }
}

static void drawAnimPoseMainError(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Initialize text drawing
    drawTextInit(true);

    // Get the current counter value
    Displays *displaysPtr = gDisplays;
    AnimPoseMainErrorDisplay *animPoseMainErrorPtr = displaysPtr->getAnimPoseMainErrorDisplayPtr();
    const uint32_t counter = animPoseMainErrorPtr->getCounter();

    // If the timer is at 0, then reset the counter
    if (animPoseMainErrorPtr->getTimer() == 0)
    {
        animPoseMainErrorPtr->resetCounter();
    }

    // Get the text
    char buf[64];
    snprintf(buf, sizeof(buf), "animPoseMain error occured x%" PRIu32, counter);

    // Draw the text
    drawText(buf,
             DISPLAYS_DEFAULT_POS_X_LEFT,
             displaysPtr->getErrorTextPosYDecrement(),
             DISPLAYS_DEFAULT_SCALE_ERRORS,
             getColorWhite(0xFF));
}

void preventAnimPoseMainCrash(int32_t poseId)
{
    // Make sure poseId is valid
    if (poseId < 0)
    {
        // poseId is invalid, so print error text
        AnimPoseMainErrorDisplay *animPoseMainErrorPtr = gDisplays->getAnimPoseMainErrorDisplayPtr();
        animPoseMainErrorPtr->setTimer(sysMsec2Frame(5000));
        animPoseMainErrorPtr->incrementCounter();
        return;
    }

    // Call the original function
    return g_animPoseMain_trampoline(poseId);
}

static void handleAnimPoseMainError(Displays *displaysPtr)
{
    AnimPoseMainErrorDisplay *animPoseMainErrorPtr = gDisplays->getAnimPoseMainErrorDisplayPtr();
    uint32_t timer = animPoseMainErrorPtr->getTimer();

    if (timer > 0)
    {
        animPoseMainErrorPtr->setTimer(--timer);
        drawOnDebugLayer(drawAnimPoseMainError, displaysPtr->getErrorTextOrder());
    }
}

bool cDisableDPadOptionsDisplay(uint32_t unkVar)
{
    if (!gDisplays->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_DPAD_OPTIONS))
    {
        return false;
    }

    return unkVar & 0x100;
}

void drawStarPowerValue()
{
    // Call the original function immediately to draw the actual status window first
    g_statusWinDisp_trampoline();

    if (!gDisplays->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_STAR_POWER_VALUE))
    {
        return;
    }

    // Credits to Jdaster64 for writing the original code for this function
    // Don't display SP if no Star Powers have been unlocked yet
    if (pouchGetPtr()->starPowersObtained == 0)
    {
        return;
    }

    // Failsafe: Make sure the current SP is valid to prevent the text from glitching
    const int32_t currentSP = pouchGetAP();
    if (currentSP < 0)
    {
        return;
    }

    // Don't display SP if the status bar is not on-screen
    const float posY = _statusWindowWorkPtr->field_0x24;
    if ((posY > 330.f) || (posY < 100.f))
    {
        return;
    }

    // Draw the current SP
    mtx34 mtx;
    PSMTXTrans(mtx, 192.f, posY - 100.f, 0.f);

    const uint32_t color = getColorWhite(0xFF);
    iconNumberDispGx(mtx, currentSP, true, reinterpret_cast<const uint8_t *>(&color));
}

void drawArtAttackHitboxes(CameraId cameraId, void *user)
{
    // Call the original function immediately, as several functions need to run for the lines to be drawn properly
    g_scissor_disp_control_trampoline(cameraId, user);

    if (!gDisplays->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_ART_ATTACK_HITBOXES))
    {
        return;
    }

    // Set the initial color to use for the lines of the hitboxes
    uint8_t hsva[4];
    *reinterpret_cast<uint32_t *>(&hsva) = 0x00FFCCFF; // RGBA value is 0xCC0000FF

    constexpr uint32_t loopCount = sizeof(BattleWork::battle_units) / sizeof(BattleWorkUnit *);
    for (uint32_t i = 0; i < loopCount; i++)
    {
        BattleWorkUnit *battleUnitPtr = getActorBattlePtr(i);
        if (!battleUnitPtr)
        {
            continue;
        }

        // Check if the current actor is a normal enemy that can be attacked
        if (BtlUnit_GetBelong(battleUnitPtr) != ActorGroupBelong::kEnemy)
        {
            // The current actor is not a normal enemy that can be attacked, so don't draw the hitbox
            continue;
        }

        // Check if the defeated flag is set for the current actor
        constexpr uint32_t defeatedFlag = 27;
        if (BtlUnit_CheckStatus(battleUnitPtr, defeatedFlag))
        {
            // The defeated flag is set for the current actor, so don't draw the hitbox
            continue;
        }

        // Check if the current actor can be attacked
        const int32_t bodyPartsId = BtlUnit_GetBodyPartsId(battleUnitPtr);
        const uint32_t attributeFlags = BtlUnit_GetPartsPtr(battleUnitPtr, bodyPartsId)->part_attribute_flags;

        if (attributeFlags & 0x10000)
        {
            // The current actor cannot be attacked, so don't draw the hitbox
            continue;
        }

        // Get the RGBA equivalent of the HSVA value
        uint32_t hitboxLineColor = HSV2RGB(hsva);

        // Adjust the hue for the lines of the next hitbox
        hsva[0] += 45;

        // Set the color of the lines of the current hitbox
        GXSetChanMatColor(GXChannelID::GX_COLOR0A0, reinterpret_cast<uint8_t *>(&hitboxLineColor));

        // Get the position of the current actor
        Vec3 actorPos;
        BtlUnit_GetPos(battleUnitPtr, &actorPos.x, &actorPos.y, &actorPos.z);

        // Check if the current actor is hanging from the ceiling
        float drawHitboxDirection;
        if (battleUnitPtr->attribute_flags & 0x2)
        {
            // The current actor is hanging from the ceiling
            drawHitboxDirection = -1.f; // Draw down
        }
        else
        {
            // The current actor is not hanging from the ceiling
            drawHitboxDirection = 1.f; // Draw up
        }

        // Get the variables used to calculate the position of the hitbox
        const float actorHitboxWidth = battleUnitPtr->cut_width;
        const float actorHitboxHeight = battleUnitPtr->cut_height;
        const float actorHitboxPosOffsetX = battleUnitPtr->cut_base_offset.x;
        const float actorHitboxPosOffsetY = battleUnitPtr->cut_base_offset.y;
        const float actorSizeScale = battleUnitPtr->sizeMultiplier;

        // Get the X and Y coordinate starting positions
        const float drawHitboxPosXStart = (actorHitboxPosOffsetX * actorSizeScale) + actorPos.x;
        const float drawHitboxPosYStart = (drawHitboxDirection * actorHitboxPosOffsetY * actorSizeScale) + actorPos.y;

        // Get the amount to adjust the starting positions by
        const float drawHitboxAdjustPosX = (actorHitboxWidth * actorSizeScale) * 0.5f;
        const float drawHitboxAdjustPosY = (actorHitboxHeight * actorSizeScale) * 0.5f;

        // Set up a set of points, used to get the starts and ends of lines
        Vec3 screenPoint;

        // Set the Z coordinate for all calculated points, as it will not change
        screenPoint.z = actorPos.z;

        // Set up 2 sets of points; One for the start of a line and one for the end of a line
        Vec3 screenPointOutLineStart;
        Vec3 screenPointOutLineEnd;

        // Draw the 4 lines that show the hitbox
        for (uint32_t i = 0; i < 4; i++)
        {
            if (i == 0)
            {
                // Get the top-left corner of the hitbox
                screenPoint.x = drawHitboxPosXStart - drawHitboxAdjustPosX;
                screenPoint.y = drawHitboxPosYStart + drawHitboxAdjustPosY;
                btlGetScreenPoint(&screenPoint, &screenPointOutLineStart);

                // Get the top-right corner of the hitbox
                screenPoint.x = drawHitboxPosXStart + drawHitboxAdjustPosX;
                screenPoint.y = drawHitboxPosYStart + drawHitboxAdjustPosY;
                btlGetScreenPoint(&screenPoint, &screenPointOutLineEnd);
            }
            else if (i == 1)
            {
                // Get the top-right corner of the hitbox
                screenPoint.x = drawHitboxPosXStart + drawHitboxAdjustPosX;
                screenPoint.y = drawHitboxPosYStart + drawHitboxAdjustPosY;
                btlGetScreenPoint(&screenPoint, &screenPointOutLineStart);

                // Get the bottom-right corner of the hitbox
                screenPoint.x = drawHitboxPosXStart + drawHitboxAdjustPosX;
                screenPoint.y = drawHitboxPosYStart - drawHitboxAdjustPosY;
                btlGetScreenPoint(&screenPoint, &screenPointOutLineEnd);
            }
            else if (i == 2)
            {
                // Get the bottom-right corner of the hitbox
                screenPoint.x = drawHitboxPosXStart + drawHitboxAdjustPosX;
                screenPoint.y = drawHitboxPosYStart - drawHitboxAdjustPosY;
                btlGetScreenPoint(&screenPoint, &screenPointOutLineStart);

                // Get the bottom-left corner of the hitbox
                screenPoint.x = drawHitboxPosXStart - drawHitboxAdjustPosX;
                screenPoint.y = drawHitboxPosYStart - drawHitboxAdjustPosY;
                btlGetScreenPoint(&screenPoint, &screenPointOutLineEnd);
            }
            else // if (i == 3)
            {
                // Get the bottom-left corner of the hitbox
                screenPoint.x = drawHitboxPosXStart - drawHitboxAdjustPosX;
                screenPoint.y = drawHitboxPosYStart - drawHitboxAdjustPosY;
                btlGetScreenPoint(&screenPoint, &screenPointOutLineStart);

                // Get the top-left corner of the hitbox
                screenPoint.x = drawHitboxPosXStart - drawHitboxAdjustPosX;
                screenPoint.y = drawHitboxPosYStart + drawHitboxAdjustPosY;
                btlGetScreenPoint(&screenPoint, &screenPointOutLineEnd);
            }

            // Draw the line from corner 1 to corner 2
            GXBegin(GXPrimitive::GX_LINES, GXVtxFmt::GX_VTXFMT0, 2);
            GXPosition3f32(screenPointOutLineStart.x, screenPointOutLineStart.y, 0.f);
            GXPosition3f32(screenPointOutLineEnd.x, screenPointOutLineEnd.y, 0.f);
        }
    }
}

uint32_t pauseArtAttackTimer()
{
    Displays *displaysPtr = gDisplays;

    if (!displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_ART_ATTACK_HITBOXES))
    {
        // Call the original function
        return g_scissor_timer_main_trampoline();
    }

    // Prevent the function from running if the button combo is being held
    if (displaysPtr->checkDisplayButtonComboEveryFrame(DisplaysWithButtonCombo::DISPLAYS_BUTTON_COMBO_ART_ATTACK_HITBOXES))
    {
        // Make sure the timer is not at 59 or lower, as pausing it then will cause issues
        const void *artAttackPtr = GetScissorPtr();

        const int32_t artAttackTimer = *reinterpret_cast<int32_t *>(reinterpret_cast<uint32_t>(artAttackPtr) + 0x104);
        if (artAttackTimer > 59)
        {
            return 0;
        }
    }

    // Call the original function
    return g_scissor_timer_main_trampoline();
}

void drawSequenceInPauseMenu(CameraId cameraId, void *winWorkPtr, int32_t index)
{
    // Call the original function immediately, as the menu should be drawn before the sequence
    g_winMarioDisp_trampoline(cameraId, winWorkPtr, index);

    // Get the position of the current menu
    const uint32_t currentMenuPtrRaw = reinterpret_cast<uint32_t>(winWorkPtr) + (index * 0x14);
    const float *winPos = reinterpret_cast<float *>(currentMenuPtrRaw + 0xC4);

    // Initialize text drawing
    drawTextInit(true);

    // Draw the sequence text
    const float posX = winPos[0];
    float posY = winPos[1] + 50.f;
    float scale = 0.8f;

#ifdef TTYD_JP
    posY += 1.f;
    scale += 0.1f;
#endif

    drawText("Sequence", posX + 46.f, posY, scale, getColorWhite(0xFF));

    // Draw the sequence value, and align it to the right
    char buf[32];
    snprintf(buf, sizeof(buf), "%" PRIu32, getSequencePosition());

    float posXIncrement = 255.f;

#ifdef TTYD_JP
    posXIncrement += 4.f;
#else
    scale = 0.9f;
#endif

    drawText(buf, posX + posXIncrement, posY, scale, 50.f * scale, getColorWhite(0xFF), TextAlignment::RIGHT);
}

// Handle setting up/calling display draw functions
// The function order here needs to match the DisplaysShouldDrawFlag enum order
static const DispCallback gDisplaysDrawFuncs[TOTAL_DISPLAYS_SHOULD_DRAW_FLAGS] = {
    // Bottom-left positioning
    drawButtonInputs,
    drawMarioCoordinatesMain,
    drawMarioSpeedXZ,
    drawStickAngleMain,

    // Bottom-right positioning
    drawOnScreenTimer,
    drawFrameCounter,

    // Top-left positioning
    drawMemoryUsage,
    drawEffsActive,
    drawEvtsActive,
    drawEnemyEncounterNotifier,

    // Top-right positioning
    drawJumpStorage,

    // Trick displays
    drawYoshiSkip,
    drawPalaceSkip,

#ifdef TTYD_JP
    drawAMWSpinJump,
#endif

    drawJabbiHiveSkip,
    drawBridgeSkip,
    drawBlimpTicketSkip,
};

static void runDisplayDrawFuncs(CameraId cameraId, void *user)
{
    // Initialize text drawing
    drawTextInit(true);

    constexpr uint32_t loopCount = sizeof(gDisplaysDrawFuncs) / sizeof(DispCallback);
    const DispCallback *displaysDrawFuncsPtr = gDisplaysDrawFuncs;
    const Displays *displaysPtr = gDisplays;

    // Run each draw func if the draw flag for it is set
    for (uint32_t i = 0; i < loopCount; i++)
    {
        if (displaysPtr->shouldDrawFlagIsSet(i))
        {
            const DispCallback func = displaysDrawFuncsPtr[i];
            if (func)
            {
                func(cameraId, user);
            }
        }
    }
}

// Handle setting up/calling displays
static const DisplaysArrayFunc gDisplaysNoButtonCombos[] = {
    handleMarioCoordinates,
    handleMarioSpeedXZ,
    handleButtonInputs,
    handleStickAngle,
    handleEffsActive,
    handleEvtsActive,
    handleEnemyEncounterNotifier,
    handleGuardSuperguardTimings,
    handleMemoryUsage,
    handleNpcNameToPtrError,
    handleAnimPoseMainError,
};

static const DisplaysArrayFunc gDisplaysWithButtonCombos[] = {
    handleOnScreenTimer,
    handleFrameCounter,
    handleJumpStorage,

    // All of the tricks should be handled last, as they may clear other draw flags
    handleYoshiSkip,
    handlePalaceSkip,

#ifdef TTYD_JP
    handleAMWSpinJump,
#endif

    handleJabbiHiveSkip,
    handleBridgeSkip,
    handleBlimpTicketSkip,
};

void runDisplayFuncsEveryFrame()
{
    // Do not do anything if the memory editor is currently open
    if (memoryEditorIsOpen())
    {
        return;
    }

    // Update the On-Screen Timer if the display is enabled
    updateOnScreenTimer();

    // Run each display function
    // Only run if the menu isn't currently open
    // Run if currently repositioning/scaling displays
    const Mod *modPtr = gMod;
    if (gMenu && !modPtr->flagIsSet(ModFlag::MOD_FLAG_MENU_IS_HIDDEN))
    {
        return;
    }

    // Reset defaultPosYErrors and defaultOrderErrors, as errors could happen any time outside of this function
    Displays *displaysPtr = gDisplays;
    displaysPtr->setDefaultPosErrors(DISPLAYS_DEFAULT_POS_Y_TOP_ERROR_TEXT);
    displaysPtr->setDefaultErrorTextOrder(DRAW_ORDER_DISPLAY_ERRORS);

    // Clear all draw flags before running any display functions
    displaysPtr->clearAllShouldDrawFlags();

    // Run each display function that isn't button-based

    // These need to run before button-based displays, as some button-based displays clear some draw flags of non-button-based
    // displays
    constexpr uint32_t loopCount = sizeof(gDisplaysNoButtonCombos) / sizeof(DisplaysArrayFunc);
    const DisplaysArrayFunc *displaysNoButtonCombosPtr = gDisplaysNoButtonCombos;

    for (uint32_t i = 0; i < loopCount; i++)
    {
        const DisplaysArrayFunc func = displaysNoButtonCombosPtr[i];
        if (func)
        {
            func(displaysPtr);
        }
    }

    // Only run button-based displays if currently not changing button combos
    if (!modPtr->flagIsSet(ModFlag::MOD_FLAG_CHANGING_BUTTON_COMBO))
    {
        constexpr uint32_t loopCount = sizeof(gDisplaysWithButtonCombos) / sizeof(DisplaysArrayFunc);
        const DisplaysArrayFunc *displaysWithButtonCombosPtr = gDisplaysWithButtonCombos;

        for (uint32_t i = 0; i < loopCount; i++)
        {
            const DisplaysArrayFunc func = displaysWithButtonCombosPtr[i];
            if (func)
            {
                func(displaysPtr);
            }
        }
    }

    // Make sure at least one display draw function will run
    if (displaysPtr->anyShouldDrawFlagIsSet())
    {
        // Reset the default positions
        displaysPtr->setDefaultPosBottomLeft(DISPLAYS_DEFAULT_POS_Y_BOTTOM);
        displaysPtr->setDefaultPosBottomRight(DISPLAYS_DEFAULT_POS_Y_BOTTOM);
        displaysPtr->setDefaultPosTopLeft(DISPLAYS_DEFAULT_POS_Y_TOP);
        displaysPtr->setDefaultPosTopRight(DISPLAYS_DEFAULT_POS_Y_TOP);

        // Run the main drawing function that handles calling the individual drawing functions
        drawOnDebugLayer(runDisplayDrawFuncs, DRAW_ORDER_DISPLAYS);
    }
}

uint32_t convertDisplaysEnabledFlagOrder(uint32_t flagOrder)
{
    switch (flagOrder)
    {
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_ONSCREEN_TIMER:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_ONSCREEN_TIMER;
        }
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_FRAME_COUNTER:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_FRAME_COUNTER;
        }
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_MARIO_COORDINATES:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MARIO_COORDINATES;
        }
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_MARIO_SPEED_X_Z:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MARIO_SPEED_X_Z;
        }
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_JUMP_STORAGE:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_JUMP_STORAGE;
        }
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_BUTTON_INPUTS:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_BUTTON_INPUTS;
        }
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_STICK_ANGLE:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_STICK_ANGLE;
        }
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_STAR_POWER_VALUE:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_STAR_POWER_VALUE;
        }
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_DPAD_OPTIONS:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_DPAD_OPTIONS;
        }
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_GUARD_SUPERGUARD_TIMINGS:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_GUARD_SUPERGUARD_TIMINGS;
        }
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_ART_ATTACK_HITBOXES:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_ART_ATTACK_HITBOXES;
        }
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_EFFS_ACTIVE:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_EFFS_ACTIVE;
        }
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_EVTS_ACTIVE:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_EVTS_ACTIVE;
        }
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_ENEMY_ENCOUNTER_NOTIFIER:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_ENEMY_ENCOUNTER_NOTIFIER;
        }
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_HIT_CHECK_VISUALIZATION:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_HIT_CHECK_VISUALIZATION;
        }
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_SHOULD_DRAW_HITS:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_SHOULD_DRAW_HITS;
        }
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_SHOULD_DRAW_MISSES:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_SHOULD_DRAW_MISSES;
        }
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_YOSHI_SKIP:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_YOSHI_SKIP;
        }
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_PALACE_SKIP:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_PALACE_SKIP;
        }
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_PALACE_SKIP_MINIMAL:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_PALACE_SKIP_MINIMAL;
        }
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_JABBI_HIVE_SKIP:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_JABBI_HIVE_SKIP;
        }
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_BRIDGE_SKIP:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_BRIDGE_SKIP;
        }
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_BLIMP_TICKET_SKIP:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_BLIMP_TICKET_SKIP;
        }
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_MEMORY_USAGE_HEAP_0:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_HEAP_0;
        }
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_MEMORY_USAGE_HEAP_1:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_HEAP_1;
        }
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_MEMORY_USAGE_HEAP_2:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_HEAP_2;
        }
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_MEMORY_USAGE_HEAP_3:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_HEAP_3;
        }
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_MEMORY_USAGE_HEAP_4:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_HEAP_4;
        }

#ifdef TTYD_JP
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_MEMORY_USAGE_HEAP_5:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_HEAP_5;
        }
#endif

        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_MEMORY_USAGE_SMART_HEAP:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_SMART_HEAP;
        }
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_MEMORY_USAGE_MAP_HEAP:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_MAP_HEAP;
        }

#ifndef TTYD_JP
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_MEMORY_USAGE_BATTLE_MAP_HEAP:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_MEMORY_USAGE_BATTLE_MAP_HEAP;
        }
#endif

#ifdef TTYD_JP
        case DisplaysEnabledFlagOrder::DISPLAYS_ENABLED_FLAG_ORDER_AMW_SPIN_JUMP:
        {
            return DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_AMW_SPIN_JUMP;
        }
#endif
        default:
        {
            return static_cast<uint32_t>(-1);
        }
    }
}

uint32_t convertDisplaysManuallyPositionFlagOrder(uint32_t flagOrder)
{
    switch (flagOrder)
    {
        case DisplaysManuallyPositionFlagOrder::DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_ONSCREEN_TIMER:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_ONSCREEN_TIMER;
        }
        case DisplaysManuallyPositionFlagOrder::DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_FRAME_COUNTER:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_FRAME_COUNTER;
        }
        case DisplaysManuallyPositionFlagOrder::DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_MARIO_COORDINATES:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_MARIO_COORDINATES;
        }
        case DisplaysManuallyPositionFlagOrder::DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_MARIO_SPEED_X_Z:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_MARIO_SPEED_X_Z;
        }
        case DisplaysManuallyPositionFlagOrder::DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_JUMP_STORAGE:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_JUMP_STORAGE;
        }
        case DisplaysManuallyPositionFlagOrder::DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_BUTTON_INPUTS:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_BUTTON_INPUTS;
        }
        case DisplaysManuallyPositionFlagOrder::DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_STICK_ANGLE:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_STICK_ANGLE;
        }
        case DisplaysManuallyPositionFlagOrder::DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_GUARD_SUPERGUARD_TIMINGS:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_GUARD_SUPERGUARD_TIMINGS;
        }
        case DisplaysManuallyPositionFlagOrder::DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_MEMORY_USAGE:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_MEMORY_USAGE;
        }
        case DisplaysManuallyPositionFlagOrder::DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_EFFS_ACTIVE:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_EFFS_ACTIVE;
        }
        case DisplaysManuallyPositionFlagOrder::DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_EVTS_ACTIVE:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_EVTS_ACTIVE;
        }
        case DisplaysManuallyPositionFlagOrder::DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_ENEMY_ENCOUNTER_NOTIFIER:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_ENEMY_ENCOUNTER_NOTIFIER;
        }
        case DisplaysManuallyPositionFlagOrder::DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_YOSHI_SKIP:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_YOSHI_SKIP;
        }
        case DisplaysManuallyPositionFlagOrder::DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_PALACE_SKIP:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_PALACE_SKIP;
        }
        case DisplaysManuallyPositionFlagOrder::DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_PALACE_SKIP_MINIMAL:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_PALACE_SKIP_MINIMAL;
        }
        case DisplaysManuallyPositionFlagOrder::DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_JABBI_HIVE_SKIP:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_JABBI_HIVE_SKIP;
        }
        case DisplaysManuallyPositionFlagOrder::DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_BRIDGE_SKIP:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_BRIDGE_SKIP;
        }
        case DisplaysManuallyPositionFlagOrder::DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_BLIMP_TICKET_SKIP:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_BLIMP_TICKET_SKIP;
        }

#ifdef TTYD_JP
        case DisplaysManuallyPositionFlagOrder::DISPLAYS_MANUALLY_POSITION_FLAG_ORDER_AMW_SPIN_JUMP:
        {
            return DisplaysManuallyPositionFlag::DISPLAYS_MANUALLY_POSITION_FLAG_AMW_SPIN_JUMP;
        }
#endif

        default:
        {
            return static_cast<uint32_t>(-1);
        }
    }
}
