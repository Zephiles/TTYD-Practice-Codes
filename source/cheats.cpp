#include "cheats.h"
#include "assembly.h"
#include "mod.h"
#include "evt_cmd.h"
#include "menuUtils.h"
#include "assembly.h"
#include "drawText.h"
#include "displays.h"
#include "memoryEditor.h"
#include "classes/valueEditor.h"
#include "classes/window.h"
#include "gc/pad.h"
#include "gc/OSInterrupt.h"
#include "gc/exi.h"
#include "gc/DEMOPad.h"
#include "gc/OSTime.h"
#include "gc/vi.h"
#include "gc/os.h"
#include "menus/battlesMenu.h"
#include "menus/cheatsMenu.h"
#include "menus/rootMenu.h"
#include "misc/utils.h"
#include "misc/functionHooks.h"
#include "ttyd/battle_unit.h"
#include "ttyd/mario.h"
#include "ttyd/mario_motion.h"
#include "ttyd/party.h"
#include "ttyd/evt_lecture.h"
#include "ttyd/evt_mario.h"
#include "ttyd/evt_memcard.h"
#include "ttyd/evtmgr.h"
#include "ttyd/item_data.h"
#include "ttyd/seqdrv.h"
#include "ttyd/mariost.h"
#include "ttyd/seq_mapchange.h"
#include "ttyd/camdrv.h"
#include "ttyd/pmario_sound.h"
#include "ttyd/mario_cam.h"
#include "ttyd/system.h"
#include "ttyd/cardmgr.h"
#include "ttyd/mario_pouch.h"
#include "ttyd/mapdrv.h"
#include "ttyd/swdrv.h"
#include "ttyd/win_main.h"
#include "ttyd/itemdrv.h"

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cinttypes>

Cheats *gCheats = nullptr;

bool Cheats::checkCheatButtonCombo(uint32_t cheatWithCombo) const
{
    // Make sure cheatWithCombo is valid
    if (cheatWithCombo >= TOTAL_CHEATS_BUTTON_COMBOS)
    {
        return false;
    }

    return checkButtonCombo(this->buttonCombos[cheatWithCombo]);
}

bool Cheats::checkCheatButtonComboEveryFrame(uint32_t cheatWithCombo) const
{
    // Make sure cheatWithCombo is valid
    if (cheatWithCombo >= TOTAL_CHEATS_BUTTON_COMBOS)
    {
        return false;
    }

    return checkButtonComboEveryFrame(this->buttonCombos[cheatWithCombo]);
}

bool Cheats::checkCheatButtonComboDemo(uint32_t cheatWithCombo) const
{
    // Make sure cheatWithCombo is valid
    if (cheatWithCombo >= TOTAL_CHEATS_BUTTON_COMBOS)
    {
        return false;
    }

    return checkButtonComboDemo(this->buttonCombos[cheatWithCombo]);
}

uint32_t Cheats::getCheatButtonCombo(uint32_t cheatWithCombo)
{
    // Make sure cheatWithCombo is valid
    if (cheatWithCombo >= TOTAL_CHEATS_BUTTON_COMBOS)
    {
        return 0;
    }

    return this->buttonCombos[cheatWithCombo];
}

void Cheats::setCheatButtonCombo(uint32_t cheatWithCombo, uint32_t buttonCombo)
{
    // Make sure cheatWithCombo is valid
    if (cheatWithCombo >= TOTAL_CHEATS_BUTTON_COMBOS)
    {
        return;
    }

    this->buttonCombos[cheatWithCombo] = static_cast<uint16_t>(buttonCombo);
}

bool Cheats::enabledFlagIsSet(uint32_t flag) const
{
    // Make sure the flag is valid
    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
    constexpr uint32_t maxFlags = CHEATS_ENABLED_FLAGS_ARRAY_SIZE * bitsPerWord;

    if (flag >= maxFlags)
    {
        return false;
    }

    // Make sure the flag does not exceed TOTAL_CHEATS_ENABLED_FLAGS
    if (flag >= TOTAL_CHEATS_ENABLED_FLAGS)
    {
        return false;
    }

    return (this->enabledFlags[flag / bitsPerWord] >> (flag % bitsPerWord)) & 1U;
}

void Cheats::setEnabledFlag(uint32_t flag)
{
    // Make sure the flag is valid
    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
    constexpr uint32_t maxFlags = CHEATS_ENABLED_FLAGS_ARRAY_SIZE * bitsPerWord;

    if (flag >= maxFlags)
    {
        return;
    }

    // Make sure the flag does not exceed TOTAL_CHEATS_ENABLED_FLAGS
    if (flag >= TOTAL_CHEATS_ENABLED_FLAGS)
    {
        return;
    }

    this->enabledFlags[flag / bitsPerWord] |= (1UL << (flag % bitsPerWord));
}

void Cheats::clearEnabledFlag(uint32_t flag)
{
    // Make sure the flag is valid
    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
    constexpr uint32_t maxFlags = CHEATS_ENABLED_FLAGS_ARRAY_SIZE * bitsPerWord;

    if (flag >= maxFlags)
    {
        return;
    }

    // Make sure the flag does not exceed TOTAL_CHEATS_ENABLED_FLAGS
    if (flag >= TOTAL_CHEATS_ENABLED_FLAGS)
    {
        return;
    }

    this->enabledFlags[flag / bitsPerWord] &= ~(1UL << (flag % bitsPerWord));
}

bool Cheats::toggleEnabledFlag(uint32_t flag)
{
    // Make sure the flag is valid
    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
    constexpr uint32_t maxFlags = CHEATS_ENABLED_FLAGS_ARRAY_SIZE * bitsPerWord;

    if (flag >= maxFlags)
    {
        return false;
    }

    // Make sure the flag does not exceed TOTAL_CHEATS_ENABLED_FLAGS
    if (flag >= TOTAL_CHEATS_ENABLED_FLAGS)
    {
        return false;
    }

    this->enabledFlags[flag / bitsPerWord] ^= (1UL << (flag % bitsPerWord));
    return this->enabledFlagIsSet(flag);
}

bool Cheats::miscFlagIsSet(uint32_t flag) const
{
    // Make sure the flag is valid
    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
    constexpr uint32_t maxFlags = CHEATS_MISC_FLAGS_ARRAY_SIZE * bitsPerWord;

    if (flag >= maxFlags)
    {
        return false;
    }

    // Make sure the flag does not exceed TOTAL_CHEATS_MISC_FLAGS
    if (flag >= TOTAL_CHEATS_MISC_FLAGS)
    {
        return false;
    }

    return (this->miscFlags[flag / bitsPerWord] >> (flag % bitsPerWord)) & 1U;
}

void Cheats::setMiscFlag(uint32_t flag)
{
    // Make sure the flag is valid
    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
    constexpr uint32_t maxFlags = CHEATS_MISC_FLAGS_ARRAY_SIZE * bitsPerWord;

    if (flag >= maxFlags)
    {
        return;
    }

    // Make sure the flag does not exceed TOTAL_CHEATS_MISC_FLAGS
    if (flag >= TOTAL_CHEATS_MISC_FLAGS)
    {
        return;
    }

    this->miscFlags[flag / bitsPerWord] |= (1UL << (flag % bitsPerWord));
}

void Cheats::clearMiscFlag(uint32_t flag)
{
    // Make sure the flag is valid
    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
    constexpr uint32_t maxFlags = CHEATS_MISC_FLAGS_ARRAY_SIZE * bitsPerWord;

    if (flag >= maxFlags)
    {
        return;
    }

    // Make sure the flag does not exceed TOTAL_CHEATS_MISC_FLAGS
    if (flag >= TOTAL_CHEATS_MISC_FLAGS)
    {
        return;
    }

    this->miscFlags[flag / bitsPerWord] &= ~(1UL << (flag % bitsPerWord));
}

bool Cheats::toggleMiscFlag(uint32_t flag)
{
    // Make sure the flag is valid
    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
    constexpr uint32_t maxFlags = CHEATS_MISC_FLAGS_ARRAY_SIZE * bitsPerWord;

    if (flag >= maxFlags)
    {
        return false;
    }

    // Make sure the flag does not exceed TOTAL_CHEATS_MISC_FLAGS
    if (flag >= TOTAL_CHEATS_MISC_FLAGS)
    {
        return false;
    }

    this->miscFlags[flag / bitsPerWord] ^= (1UL << (flag % bitsPerWord));
    return this->miscFlagIsSet(flag);
}

Cheats::Cheats()
{
    // Set default cheats enabled
    // this->setEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_WALK_THROUGH_WALLS);
    // this->setEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_FALL_THROUGH_FLOORS);
    this->setEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_SAVE_COORDINATES);
    this->setEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_LOAD_COORDINATES);
    // this->setEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_SPAWN_ITEM);
    // this->setEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_SAVE_ANYWHERE);
    // this->setEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_TEXT_STORAGE);
    // this->setEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_TIME_STOP_TEXT_STORAGE);
    this->setEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_SPEED_UP_MARIO);
    this->setEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_DISABLE_BATTLES);
    this->setEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_AUTO_ACTION_COMMANDS);
    // this->setEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_INFINITE_ITEM_USAGE);
    this->setEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_RELOAD_ROOM);
    this->setEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_LEVITATE);
    this->setEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_AUTO_MASH_TEXT);
    // this->setEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_FRAME_ADVANCE);
    // this->setEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_GENERATE_LAG_SPIKE);
    // this->setEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_LOCK_MARIO_HP_TO_MAX);
    this->setEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_RUN_FROM_BATTLES);
    // this->setEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_BOBBERY_EARLY);
    // this->setEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_FORCE_NPC_ITEM_DROP);

    // Set default cheat button combos
    this->setCheatButtonCombo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_WALK_THROUGH_WALLS, PadInput::PAD_Z);

    this->setCheatButtonCombo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_FALL_THROUGH_FLOORS,
                              PadInput::PAD_Y | PadInput::PAD_Z);

    this->setCheatButtonCombo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_SAVE_COORDINATES,
                              PadInput::PAD_L | PadInput::PAD_DPAD_LEFT);

    this->setCheatButtonCombo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_LOAD_COORDINATES,
                              PadInput::PAD_L | PadInput::PAD_DPAD_UP);

    this->setCheatButtonCombo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_SPAWN_ITEM, PadInput::PAD_L | PadInput::PAD_DPAD_DOWN);
    this->setCheatButtonCombo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_SAVE_ANYWHERE, PadInput::PAD_Y | PadInput::PAD_B);
    this->setCheatButtonCombo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_TEXT_STORAGE, PadInput::PAD_L | PadInput::PAD_X);

    this->setCheatButtonCombo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_TIME_STOP_TEXT_STORAGE,
                              PadInput::PAD_L | PadInput::PAD_R);

    this->setCheatButtonCombo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_SPEED_UP_MARIO, PadInput::PAD_L | PadInput::PAD_Y);
    this->setCheatButtonCombo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_DISABLE_BATTLES, PadInput::PAD_Y);
    this->setCheatButtonCombo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_AUTO_ACTION_COMMANDS, PadInput::PAD_R);
    this->setCheatButtonCombo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_INFINITE_ITEM_USAGE, PadInput::PAD_Y);
    this->setCheatButtonCombo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_RELOAD_ROOM, PadInput::PAD_L | PadInput::PAD_B);
    this->setCheatButtonCombo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_LEVITATE, PadInput::PAD_L | PadInput::PAD_A);
    this->setCheatButtonCombo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_AUTO_MASH_TEXT, PadInput::PAD_Y);
    this->setCheatButtonCombo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_FRAME_ADVANCE_ADVANCE_FRAME, PadInput::PAD_Y);

    this->setCheatButtonCombo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_FRAME_ADVANCE_PAUSE_RESUME,
                              PadInput::PAD_L | PadInput::PAD_A | PadInput::PAD_Z);

    this->setCheatButtonCombo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_GENERATE_LAG_SPIKE, PadInput::PAD_Y | PadInput::PAD_Z);
}

uint32_t allowRunningFromBattles(void *ptr)
{
    if (gCheats->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_RUN_FROM_BATTLES))
    {
        // Force the branch to be taken
        return 0;
    }

    return *reinterpret_cast<uint32_t *>(reinterpret_cast<uint32_t>(ptr) + 0x4);
}

int32_t forceNpcItemDrop(void *ptr)
{
    Cheats *cheatsPtr = gCheats;

    if (cheatsPtr->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_FORCE_NPC_ITEM_DROP))
    {
        return static_cast<int32_t>(cheatsPtr->getForceNpcItemDropCheatPtr()->getItemDrop());
    }

    return *reinterpret_cast<int32_t *>(reinterpret_cast<uint32_t>(ptr) + 0x23C);
}

void *fallThroughMostObjects(void *ptr)
{
    Cheats *cheatsPtr = gCheats;

    if (!cheatsPtr->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_FALL_THROUGH_FLOORS))
    {
        return ptr;
    }

    // Don't fall if currently changing button combos
    if (gMod.changingButtonCombo())
    {
        return ptr;
    }

    if (!cheatsPtr->checkCheatButtonComboEveryFrame(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_FALL_THROUGH_FLOORS))
    {
        return ptr;
    }

    // Prevent the branch from being taken
    return nullptr;
}

void handleAutoActionCommands()
{
    // Make sure Mario is in the battle before making any changes
    BattleWorkUnit *marioBattlePtr = getMarioBattlePtr();
    if (!marioBattlePtr)
    {
        return;
    }

    auto setDebugBadgeValue = [marioBattlePtr](int8_t value)
    {
        // Set the value for Mario
        marioBattlePtr->badges_equipped.auto_command_badge = value;

        // Set the value for the partner
        BattleWorkUnit *partnerBattlePtr = getPartnerBattlePtr();
        if (partnerBattlePtr)
        {
            partnerBattlePtr->badges_equipped.auto_command_badge = value;
        }
    };

    Cheats *cheatsPtr = gCheats;

    // Check to see if the Auto Action Commands cheat is active or not
    if (cheatsPtr->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_AUTO_ACTION_COMMANDS))
    {
        // Check to see if currently changing button combos
        if (!gMod.changingButtonCombo())
        {
            if (cheatsPtr->checkCheatButtonComboEveryFrame(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_AUTO_ACTION_COMMANDS) ||
                checkIfBadgeEquipped(ItemId::ITEM_DEBUG_BADGE))
            {
                setDebugBadgeValue(1);
            }
            else
            {
                setDebugBadgeValue(0);
            }
        }
        else if (checkIfBadgeEquipped(ItemId::ITEM_DEBUG_BADGE))
        {
            setDebugBadgeValue(1);
        }
        else
        {
            setDebugBadgeValue(0);
        }
    }
    else if (checkIfBadgeEquipped(ItemId::ITEM_DEBUG_BADGE))
    {
        setDebugBadgeValue(1);
    }
    else
    {
        setDebugBadgeValue(0);
    }
}

void walkThroughMostObjects(Cheats *cheatsPtr, Mod *modPtr)
{
    if (cheatsPtr->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_WALK_THROUGH_WALLS) &&
        !modPtr->changingButtonCombo() &&
        cheatsPtr->checkCheatButtonComboEveryFrame(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_WALK_THROUGH_WALLS))
    {
        cheatsPtr->setMiscFlag(CheatsMiscFlag::CHEATS_MISC_FLAG_RESET_MARIO_PROPERTIES);

        // Set Mario's properties to be able to walk through most objects
        Player *playerPtr = marioGetPtr();
        playerPtr->flags1 |= 0x400;
    }
    else if (cheatsPtr->miscFlagIsSet(CheatsMiscFlag::CHEATS_MISC_FLAG_RESET_MARIO_PROPERTIES))
    {
        cheatsPtr->clearMiscFlag(CheatsMiscFlag::CHEATS_MISC_FLAG_RESET_MARIO_PROPERTIES);

        // Don't reset the properties if Vivian is currently being used
        Player *playerPtr = marioGetPtr();
        if (playerPtr->currentMotionId != MarioMotion::kVivian)
        {
            playerPtr->flags1 &= ~0x400;
        }
    }
}

const char *replaceJumpFallAnim(char *jumpFallString)
{
    Cheats *cheatsPtr = gCheats;
    if (strncmp(jumpFallString, "M_J_", 4) == 0)
    {
        const char *mapCheck = "gor_01";
        if (compareStringToNextMap(mapCheck))
        {
            if (cheatsPtr->miscFlagIsSet(CheatsMiscFlag::CHEATS_MISC_FLAG_MARIO_FREEZE))
            {
                // Return an arbitrary string
                return mapCheck;
            }
            else
            {
                const Player *playerPtr = marioGetPtr();

                const int32_t marPosX = floatToInt(playerPtr->playerPosition.x);
                if ((marPosX >= -9) && (marPosX <= 8))
                {
                    const int32_t marPosZ = floatToInt(playerPtr->playerPosition.z);
                    if ((marPosZ >= 245) && (marPosZ <= 265))
                    {
                        const int32_t marPosY = floatToInt(playerPtr->playerPosition.y);
                        if (marPosY >= 59)
                        {
                            cheatsPtr->setMiscFlag(CheatsMiscFlag::CHEATS_MISC_FLAG_MARIO_FREEZE);

                            // Return an arbitrary string
                            return mapCheck;
                        }
                    }
                }
            }
        }
    }
    return jumpFallString;
}

void saveMarioAndPartnerPositions(Cheats *cheatsPtr, Mod *modPtr)
{
    (void)modPtr;

    if (!cheatsPtr->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_SAVE_COORDINATES))
    {
        return;
    }

    if (!cheatsPtr->checkCheatButtonCombo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_SAVE_COORDINATES))
    {
        return;
    }

    Player *playerPtr = marioGetPtr();
    SaveLoadCoordinatesCheats *saveLoadCoordinatesPtr = cheatsPtr->getSaveLoadCoordinatesCheatsPtr();
    float *savedPosPtr = saveLoadCoordinatesPtr->getMarioSavedPosPtr();

    savedPosPtr[0] = playerPtr->playerPosition.x;        // Mario Coordinate X
    savedPosPtr[1] = playerPtr->playerPosition.y;        // Mario Coordinate Y
    savedPosPtr[2] = playerPtr->playerPosition.z;        // Mario Coordinate Z
    savedPosPtr[3] = playerPtr->wPlayerAngleCurrent;     // Mario Angle Current
    savedPosPtr[4] = playerPtr->wPlayerAngle;            // Mario Angle
    savedPosPtr[5] = playerPtr->wPlayerDirectionCurrent; // Mario Direction Current
    savedPosPtr[6] = playerPtr->wPlayerDirection;        // Mario Direction

    PartyEntry *partnerPtr = getPartnerPtr();
    if (!partnerPtr)
    {
        return;
    }

    savedPosPtr = saveLoadCoordinatesPtr->getPartnerSavedPosPtr();

    savedPosPtr[0] = partnerPtr->partnerPosition.x;        // Partner Coordinate X
    savedPosPtr[1] = partnerPtr->partnerPosition.y;        // Partner Coordinate Y
    savedPosPtr[2] = partnerPtr->partnerPosition.z;        // Partner Coordinate Z
    savedPosPtr[3] = partnerPtr->wPartnerAngleCurrent;     // Partner Angle Current
    savedPosPtr[4] = partnerPtr->wPartnerAngle;            // Partner Angle
    savedPosPtr[5] = partnerPtr->wPartnerDirectionCurrent; // Partner Direction Current
    savedPosPtr[6] = partnerPtr->wPartnerDirection;        // Partner Direction
}

void loadMarioAndPartnerPositions(Cheats *cheatsPtr, Mod *modPtr)
{
    (void)modPtr;

    if (!cheatsPtr->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_LOAD_COORDINATES))
    {
        return;
    }

    if (!cheatsPtr->checkCheatButtonCombo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_LOAD_COORDINATES))
    {
        return;
    }

    Player *playerPtr = marioGetPtr();
    SaveLoadCoordinatesCheats *saveLoadCoordinatesPtr = cheatsPtr->getSaveLoadCoordinatesCheatsPtr();
    float *savedPosPtr = saveLoadCoordinatesPtr->getMarioSavedPosPtr();

    playerPtr->playerPosition.x = savedPosPtr[0];        // Mario Coordinate X
    playerPtr->playerPosition.y = savedPosPtr[1];        // Mario Coordinate Y
    playerPtr->playerPosition.z = savedPosPtr[2];        // Mario Coordinate Z
    playerPtr->wPlayerAngleCurrent = savedPosPtr[3];     // Mario Angle Current
    playerPtr->wPlayerAngle = savedPosPtr[4];            // Mario Angle
    playerPtr->wPlayerDirectionCurrent = savedPosPtr[5]; // Mario Direction Current
    playerPtr->wPlayerDirection = savedPosPtr[6];        // Mario Direction

    PartyEntry *partnerPtr = getPartnerPtr();
    if (!partnerPtr)
    {
        return;
    }

    savedPosPtr = saveLoadCoordinatesPtr->getPartnerSavedPosPtr();

    partnerPtr->partnerPosition.x = savedPosPtr[0];        // Partner Coordinate X
    partnerPtr->partnerPosition.y = savedPosPtr[1];        // Partner Coordinate Y
    partnerPtr->partnerPosition.z = savedPosPtr[2];        // Partner Coordinate Z
    partnerPtr->wPartnerAngleCurrent = savedPosPtr[3];     // Partner Angle Current
    partnerPtr->wPartnerAngle = savedPosPtr[4];            // Partner Angle
    partnerPtr->wPartnerDirectionCurrent = savedPosPtr[5]; // Partner Direction Current
    partnerPtr->wPartnerDirection = savedPosPtr[6];        // Partner Direction
}

// Custom script for handling saving
// clang-format off
EVT_BEGIN(memcardSaveScript)

    // Set the System Level
    USER_FUNC(lect_set_systemlevel, 1)

    // Take away control from the player
    USER_FUNC(evt_mario_key_onoff, 0)

    // Call evt_memcard_save synchronously, which handles the main saving code
    // LF(10) and LW(0) need to be set to 0 before evt_memcard_save can be called
    SET(LF(10), 0)
    SET(LW(0), 0)
    RUN_CHILD_EVT(evt_memcard_save)

    // Give back control to the player
    USER_FUNC(evt_mario_key_onoff, 1)

    // Clear the System Level
    USER_FUNC(lect_set_systemlevel, 0)

    RETURN()
EVT_END()
// clang-format on

void saveAnywhere(Cheats *cheatsPtr, Mod *modPtr)
{
    SaveAnywhereCheat *saveAnywherePtr = cheatsPtr->getSaveAnywhereCheatPtr();

    // Don't allow starting the save script if it's already running
    if (!cheatsPtr->miscFlagIsSet(CheatsMiscFlag::CHEATS_MISC_FLAG_SAVE_SCRIPT_IS_RUNNING))
    {
        // Save script is not currently running
        if (!cheatsPtr->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_SAVE_ANYWHERE))
        {
            return;
        }

        if (modPtr->changingButtonCombo())
        {
            return;
        }

        if (!cheatsPtr->checkCheatButtonCombo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_SAVE_ANYWHERE))
        {
            return;
        }

        // Make sure a file is currently loaded
        if (!checkIfInGame())
        {
            return;
        }

        // Make sure the System Level is not set
        if (!systemLevelIsZero())
        {
            return;
        }

        // Start the Save script
        const EvtEntry *saveScriptPtr = evtEntryType(memcardSaveScript, 0, 0, 0);
        saveAnywherePtr->setThreadId(saveScriptPtr->threadId);
        cheatsPtr->setMiscFlag(CheatsMiscFlag::CHEATS_MISC_FLAG_SAVE_SCRIPT_IS_RUNNING);
    }
    else if (!evtCheckID(saveAnywherePtr->getThreadId()))
    {
        // Save Script is no longer running
        cheatsPtr->clearMiscFlag(CheatsMiscFlag::CHEATS_MISC_FLAG_SAVE_SCRIPT_IS_RUNNING);
    }
}

void setTextStorage(Cheats *cheatsPtr, Mod *modPtr)
{
    (void)modPtr;

    if (!cheatsPtr->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_TEXT_STORAGE))
    {
        return;
    }

    if (!cheatsPtr->checkCheatButtonCombo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_TEXT_STORAGE))
    {
        return;
    }

    PartyEntry *partnerPtr = getPartnerPtr();
    if (partnerPtr)
    {
        partnerPtr->wActionState = 0;
    }
}

void setTimeStopTextStorage(Cheats *cheatsPtr, Mod *modPtr)
{
    (void)modPtr;

    if (!cheatsPtr->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_TIME_STOP_TEXT_STORAGE))
    {
        return;
    }

    if (!cheatsPtr->checkCheatButtonCombo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_TIME_STOP_TEXT_STORAGE))
    {
        return;
    }

    PartyEntry *partnerPtr = getPartnerPtr();
    if (partnerPtr)
    {
        partnerPtr->wActionState = 21;
    }
}

void speedUpMario(Cheats *cheatsPtr, Mod *modPtr)
{
    Player *playerPtr = marioGetPtr();
    SpeedUpMarioCheat *speedUpMarioPtr = cheatsPtr->getSpeedUpMarioCheatPtr();

    float *marioVarsPtr = speedUpMarioPtr->getMarioVarsPtr();
    const float current_unk_184 = marioVarsPtr[0];
    const float current_unk_188 = marioVarsPtr[1];

    const float new_unk_184 = playerPtr->unk_184;
    const float new_unk_188 = playerPtr->unk_188;

    if (!cheatsPtr->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_SPEED_UP_MARIO) || modPtr->changingButtonCombo())
    {
        // Check if the values should be restored
        if (speedUpMarioPtr->getChangedState() > SpeedUpMarioChangedState::SPEED_UP_MARIO_CHANGED_STATE_NO_CHANGES)
        {
            speedUpMarioPtr->setChangedState(SpeedUpMarioChangedState::SPEED_UP_MARIO_CHANGED_STATE_NO_CHANGES);
            playerPtr->unk_184 = current_unk_184;
            playerPtr->unk_188 = current_unk_188;
            return;
        }
    }

    constexpr float speedValueToSet = 16.f;

    // Check to see if the current values have changed
    if ((current_unk_184 != new_unk_184) || (current_unk_188 != new_unk_188))
    {
        // Don't update if the new value is what was just set by this function
        if (new_unk_188 != speedValueToSet)
        {
            // Set the current values to the new values
            marioVarsPtr[0] = new_unk_184;
            marioVarsPtr[1] = new_unk_188;
        }
    }

    if (cheatsPtr->checkCheatButtonComboEveryFrame(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_SPEED_UP_MARIO))
    {
        speedUpMarioPtr->setChangedState(SpeedUpMarioChangedState::SPEED_UP_MARIO_CHANGED_STATE_VALUES_MODIFIED);
        playerPtr->wPlayerBaseSpeed = speedValueToSet;
        playerPtr->unk_184 = speedValueToSet;
        playerPtr->unk_188 = speedValueToSet;
    }
    else if (speedUpMarioPtr->getChangedState() == SpeedUpMarioChangedState::SPEED_UP_MARIO_CHANGED_STATE_VALUES_MODIFIED)
    {
        speedUpMarioPtr->setChangedState(SpeedUpMarioChangedState::SPEED_UP_MARIO_CHANGED_STATE_VALUES_SHOULD_BE_RESTORED);
    }

    // Check if the values should be restored
    if (speedUpMarioPtr->getChangedState() == SpeedUpMarioChangedState::SPEED_UP_MARIO_CHANGED_STATE_VALUES_SHOULD_BE_RESTORED)
    {
        speedUpMarioPtr->setChangedState(SpeedUpMarioChangedState::SPEED_UP_MARIO_CHANGED_STATE_NO_CHANGES);
        playerPtr->unk_184 = marioVarsPtr[0];
        playerPtr->unk_188 = marioVarsPtr[1];
    }
}

bool disableBattles()
{
    if (gDisplays->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_ENEMY_ENCOUNTER_NOTIFIER))
    {
        // Non-cutscene battles should always be disabled when the Enemy Encounter Notifier display is on
        return true;
    }
    else if (checkButtonComboEveryFrame(OPEN_CLOSE_MENU_BUTTON_COMBO))
    {
        // Prevent entering a non-cutscene battle if opening the menu
        return true;
    }
    else if (!gMod.changingButtonCombo())
    {
        Cheats *cheatsPtr = gCheats;

        // Prevent entering a non-cutscene battle if reloading the room
        if (cheatsPtr->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_RELOAD_ROOM) &&
            cheatsPtr->checkCheatButtonComboEveryFrame(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_RELOAD_ROOM))
        {
            return true;
        }
        else if (cheatsPtr->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_DISABLE_BATTLES) &&
                 cheatsPtr->checkCheatButtonComboEveryFrame(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_DISABLE_BATTLES))
        {
            return true;
        }
    }

    return false;
}

bool infiniteItemUsage(ItemId item, uint32_t index)
{
    Cheats *cheatsPtr = gCheats;

    if (!cheatsPtr->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_INFINITE_ITEM_USAGE))
    {
        // Call the original function
        return g_pouchRemoveItemIndex_trampoline(item, index);
    }

    if (gMod.changingButtonCombo())
    {
        // Call the original function
        return g_pouchRemoveItemIndex_trampoline(item, index);
    }

    if (!cheatsPtr->checkCheatButtonComboEveryFrame(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_INFINITE_ITEM_USAGE))
    {
        // Call the original function
        return g_pouchRemoveItemIndex_trampoline(item, index);
    }

    // Prevent the original function from running
    return false;
}

void reloadRoomMain()
{
    // Only run while a file is loaded
    const SeqIndex nextSeq = seqGetNextSeq();
    if ((nextSeq < SeqIndex::kGame) || (nextSeq > SeqIndex::kMapChange))
    {
        return;
    }

    if (seqGetSeq() == SeqIndex::kBattle)
    {
        // Reloading the room with the Seq set to Battle will cause the game to crash, so don't allow it
        return;
    }

    // Resolve any necessary fades, as they can sometimes cause issues if not resolved, such as black screens
    // Only resolve the fades if not currently in a screen transition
    if (checkForSpecificSeq(SeqIndex::kGame))
    {
        constexpr uint32_t totalFadeEntries = sizeof(FadeWork::entry) / sizeof(FadeEntry);
        for (uint32_t i = 0; i < totalFadeEntries; i++)
        {
            resolveFade(i);
        }
    }

    // A separate address for _next_bero is needed, as the original value will be cleared during the reloading process
    // The game will crash if _next_map is used directly in seqSetSeq, so a separate address must be used instead
    // newBero and newMap need to be global variables
    Cheats *cheatsPtr = gCheats;
    ReloadRoomCheat *reloadCheatPtr = cheatsPtr->getReloadRoomCheatPtr();

    char *newBeroPtr = reloadCheatPtr->getNewBeroPtr();
    const uint32_t newBeroSize = reloadCheatPtr->getNewBeroSize() - 1;
    strncpy(newBeroPtr, _next_bero, newBeroSize);
    newBeroPtr[newBeroSize] = '\0';

    char *newMapPtr = reloadCheatPtr->getNewMapPtr();
    const uint32_t newMapSize = reloadCheatPtr->getNewMapSize() - 1;
    strncpy(newMapPtr, _next_map, newMapSize);
    newMapPtr[newMapSize] = '\0';

    setSeqMapChange(newMapPtr, newBeroPtr);

    // Set a flag to prevent battles from starting
    // This needs to be done after setSeqMapChange is called
    cheatsPtr->setMiscFlag(CheatsMiscFlag::CHEATS_MISC_FLAG_MANUALLY_RELOADING_ROOM);

    // Make sure the System Level can be set again
    gMod.clearSystemLevel();

    // Reset the camera - mainly for the black bars at the top and bottom of the screen
    CameraEntry *cameraPtr = camGetPtr(CameraId::k2d);
    cameraPtr->flags &= ~0x300;

    if (systemLevelIsZero())
    {
        return;
    }

    // Only run the following if the System Level is not 0
    // Re-enable the 3D camera if necessary
    L_camDispOn(CameraId::k3d);

    // Enable sound effects, set the default camera id for Mario, and give back control to the player
    psndClearFlag(0x80);
    marioSetCamId(CameraId::k3d);
    marioStSystemLevel(0);
    cheatsPtr->setMiscFlag(CheatsMiscFlag::CHEATS_MISC_FLAG_SYSTEM_LEVEL_SHOULD_BE_LOWERED);
}

void reloadRoom(Cheats *cheatsPtr, Mod *modPtr)
{
    if (!modPtr->changingButtonCombo() &&
        cheatsPtr->checkCheatButtonCombo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_RELOAD_ROOM))
    {
        // Prevent being able to reload the room if the menu is open, if currently in the spawn item menu, or if the memory
        // editor is open
        if (cheatsPtr->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_RELOAD_ROOM) && !gMenu &&
            !cheatsPtr->getSpawnItemCheatPtr()->getValueEditorPtr() && !memoryEditorIsOpen())
        {
            reloadRoomMain();
        }
    }

    // Manually reset the System Level and the camera if currently getting an item while the room is trying to load
    Player *playerPtr = marioGetPtr();
    if (playerPtr->currentMotionId != MarioMotion::kGetItem)
    {
        return;
    }

    if (seqGetNextSeq() != SeqIndex::kMapChange)
    {
        return;
    }

    // Reset the System Level
    cheatsPtr->setMiscFlag(CheatsMiscFlag::CHEATS_MISC_FLAG_SYSTEM_LEVEL_SHOULD_BE_LOWERED);

    // Reset the camera - mainly for the black bars at the top and bottom of the screen
    CameraEntry *cameraPtr = camGetPtr(CameraId::k2d);
    cameraPtr->flags &= ~0x300;
}

void checkIfSystemLevelShouldBeLowered(Cheats *cheatsPtr, Mod *modPtr)
{
    (void)modPtr;

    // Only run when the player has either reloaded the room or warped
    if (!cheatsPtr->miscFlagIsSet(CheatsMiscFlag::CHEATS_MISC_FLAG_SYSTEM_LEVEL_SHOULD_BE_LOWERED))
    {
        return;
    }

    // Keep resetting the System Level until the seq changes to Game
    if (!checkForSpecificSeq(SeqIndex::kGame))
    {
        // Only reset the System Level if it's not currently at 0
        if (!systemLevelIsZero())
        {
            marioStSystemLevel(0);
        }
    }
    else
    {
        cheatsPtr->clearMiscFlag(CheatsMiscFlag::CHEATS_MISC_FLAG_SYSTEM_LEVEL_SHOULD_BE_LOWERED);
    }
}

void disableBattlesOnReload(SeqIndex seq, const char *map, const char *bero)
{
    Cheats *cheatsPtr = gCheats;

    if (cheatsPtr->miscFlagIsSet(CheatsMiscFlag::CHEATS_MISC_FLAG_MANUALLY_RELOADING_ROOM))
    {
        // Don't set the seq if trying to start a battle while reloading the room
        if (seq == SeqIndex::kBattle)
        {
            return;
        }

        // Any other seq should be fine, so clear the flag
        cheatsPtr->clearMiscFlag(CheatsMiscFlag::CHEATS_MISC_FLAG_MANUALLY_RELOADING_ROOM);
    }

    // Call the original function
    return g_seqSetSeq_trampoline(seq, map, bero);
}

void levitate(Cheats *cheatsPtr, Mod *modPtr)
{
    (void)modPtr;

    if (!cheatsPtr->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_LEVITATE))
    {
        return;
    }

    if (!cheatsPtr->checkCheatButtonComboEveryFrame(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_LEVITATE))
    {
        return;
    }

    marioGetPtr()->wJumpVelocityY = 4.540000915527344f;
}

uint32_t autoMashText(PadId controllerId)
{
    Cheats *cheatsPtr = gCheats;

    if (!cheatsPtr->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_AUTO_MASH_TEXT))
    {
        // Return the intended value
        return keyGetButtonTrg(controllerId);
    }

    // Don't auto mash if the Palace Skip display is currently on
    const Displays *displaysPtr = gDisplays;
    if (displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_PALACE_SKIP) ||
        displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_PALACE_SKIP_MINIMAL))
    {
        // Return the intended value
        return keyGetButtonTrg(controllerId);
    }

    // Don't auto mash if currently changing button combos
    if (gMod.changingButtonCombo())
    {
        // Return the intended value
        return keyGetButtonTrg(controllerId);
    }

    if (!cheatsPtr->checkCheatButtonComboEveryFrame(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_AUTO_MASH_TEXT))
    {
        // Return the intended value
        return keyGetButtonTrg(controllerId);
    }

    // Return the value for B to make sure the text is being mashed through as fast as possible
    return PadInput::PAD_B;
}

void setOSTime(int64_t time)
{
    // Interrupts should be disabled for safety
    const bool enable = OSDisableInterrupts();

    // Set the new time
    asmSetTime(time);

    // Reset EXI stuff
    EXIProbeReset();

    // Restore interrupts
    OSRestoreInterrupts(enable);
}

void frameAdvance()
{
    Cheats *cheatsPtr = gCheats;

    if (!cheatsPtr->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_FRAME_ADVANCE))
    {
        // Call the original function
        return g_DEMOPadRead_trampoline();
    }

    // If currently changing button combos, then do not run
    if (gMod.changingButtonCombo())
    {
        // Call the original function
        return g_DEMOPadRead_trampoline();
    }

    // Make sure memory card stuff isn't currently happening
    if (cardIsExec())
    {
        // Call the original function
        return g_DEMOPadRead_trampoline();
    }

    // Use various locals to keep the next section cleaner
    constexpr uint32_t padStatusSize = sizeof(PadStatus) * 4;         // One for each controller port
    constexpr uint32_t demoPadStatusSize = sizeof(DEMOPadStatus) * 4; // One for each controller port

    FrameAdvancePrevInputs *prevInputsPtr = cheatsPtr->getFrameAdvanceCheatPtr()->allocMemoryForPrevInputs();
    DEMOPadStatus *demoPadPtr = &DemoPad[0];
    PadStatus *padPtr = &Pad[0];

    // Keep track of whether the player is in the process of advancing frames
    const bool advancingFrame = cheatsPtr->miscFlagIsSet(CheatsMiscFlag::CHEATS_MISC_FLAG_FRAME_ADVANCE_ADVANCE_FRAME);
    if (!advancingFrame)
    {
        // Get the button inputs for this frame
        g_DEMOPadRead_trampoline();

        // Back up the button inputs that were just generated
        memcpy(prevInputsPtr->getPadPrevFramePtr(), padPtr, padStatusSize);
        memcpy(prevInputsPtr->getDemoPadPrevFramePtr(), demoPadPtr, demoPadStatusSize);
    }

    if (!advancingFrame &&
        !cheatsPtr->checkCheatButtonComboDemo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_FRAME_ADVANCE_PAUSE_RESUME))
    {
        // Button inputs for this frame should have already been retrieved, so can just return normally
        return;
    }

    if (advancingFrame)
    {
        // Restore the button inputs from the previous frame
        // Copy both sets at the same time
        memcpy(prevInputsPtr->getPadPrevFramePtr(), prevInputsPtr->getPadCurrentFramePtr(), padStatusSize + demoPadStatusSize);
    }

    // The game should currently be paused and not currently frame advancing
    // FRAME_ADVANCE_GAME_IS_PAUSED needs to be set now to prevent viPostCallback from running
    // FRAME_ADVANCE_ADVANCE_FRAME isn't currently used anywhere else, but might be in the future, so adjust it now
    cheatsPtr->clearMiscFlag(CheatsMiscFlag::CHEATS_MISC_FLAG_FRAME_ADVANCE_ADVANCE_FRAME);
    cheatsPtr->setMiscFlag(CheatsMiscFlag::CHEATS_MISC_FLAG_FRAME_ADVANCE_GAME_IS_PAUSED);

    // Back up the current OSTime
    const int64_t currentTime = OSGetTime();

    // Go to the next frame to avoid button combos from the previous frame from activating
    VIWaitForRetrace();

    // Recheck the button inputs
    g_DEMOPadRead_trampoline();

    // Loop indefinitely until a button combo is pressed
    while (1)
    {
        // Check for advancing to the next frame or resuming the game
        // Check for the pause combo first to prevent duplicate buttons causing softlocks
        if (cheatsPtr->checkCheatButtonComboDemo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_FRAME_ADVANCE_PAUSE_RESUME))
        {
            // Restore the backed up OSTime
            setOSTime(currentTime);

            // Resume gameplay
            cheatsPtr->clearMiscFlag(CheatsMiscFlag::CHEATS_MISC_FLAG_FRAME_ADVANCE_ADVANCE_FRAME);
            cheatsPtr->clearMiscFlag(CheatsMiscFlag::CHEATS_MISC_FLAG_FRAME_ADVANCE_GAME_IS_PAUSED);
            return;
        }
        else if (cheatsPtr->checkCheatButtonComboDemo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_FRAME_ADVANCE_ADVANCE_FRAME))
        {
            // Back up the button inputs for this frame
            memcpy(prevInputsPtr->getPadCurrentFramePtr(), padPtr, padStatusSize);
            memcpy(prevInputsPtr->getDemoPadCurrentFramePtr(), demoPadPtr, demoPadStatusSize);

            // Restore the button inputs from the previous frame
            memcpy(padPtr, prevInputsPtr->getPadPrevFramePtr(), padStatusSize);
            memcpy(demoPadPtr, prevInputsPtr->getDemoPadPrevFramePtr(), demoPadStatusSize);

            // Restore the backed up OSTime
            setOSTime(currentTime);

            // Advance one frame
            cheatsPtr->setMiscFlag(CheatsMiscFlag::CHEATS_MISC_FLAG_FRAME_ADVANCE_ADVANCE_FRAME);
            cheatsPtr->clearMiscFlag(CheatsMiscFlag::CHEATS_MISC_FLAG_FRAME_ADVANCE_GAME_IS_PAUSED);
            return;
        }

        // Go to the next frame
        VIWaitForRetrace();

        // Recheck the button inputs
        g_DEMOPadRead_trampoline();
    }
}

void preventViPostCallBackOnFramePause(uint32_t retraceCount)
{
    Cheats *cheatsPtr = gCheats;
    if (!cheatsPtr->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_FRAME_ADVANCE))
    {
        // Call the original function
        return g_viPostCallback_trampoline(retraceCount);
    }

    // If the game is paused, then don't call the original function
    if (cheatsPtr->miscFlagIsSet(CheatsMiscFlag::CHEATS_MISC_FLAG_FRAME_ADVANCE_GAME_IS_PAUSED))
    {
        // Call psndMainInt to prevent the audio from looping
        // viPostCallback normally does this immediately before returning
        return psndMainInt();
    }

    // Call the original function
    return g_viPostCallback_trampoline(retraceCount);
}

int32_t disableMenuSounds(int32_t soundId, uint8_t a2, uint8_t vol, uint8_t pan)
{
    if (gCheats->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_DISABLE_SOUNDS_PAUSE_MENU_Z_MENU))
    {
        constexpr int32_t openMenuId = 1169;
        constexpr int32_t closeMenuId = 1168;

        if ((soundId == openMenuId) || (soundId == closeMenuId))
        {
            // Prevent the sound from playing
            return -1;
        }
    }

    // Call the original function
    return g_SoundEfxPlayEx_trampoline(soundId, a2, vol, pan);
}

bool disableBgm(int32_t flags, const char *bgmName, uint16_t wFadeTime, uint16_t wFadeTime2, bool unused)
{
    if (gCheats->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_DISABLE_SOUNDS_BACKGROUND_MUSIC))
    {
        // Prevent the music from playing
        return false;
    }

    // Call the original function
    return g_psndBGMOn_f_d_trampoline(flags, bgmName, wFadeTime, wFadeTime2, unused);
}

bool disableEnvSounds(int32_t flags, const char *envName, int32_t wFadeTime, bool unused)
{
    if (gCheats->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_DISABLE_SOUNDS_ENVIRONMENT_SOUNDS))
    {
        // Prevent the sound from playing
        return false;
    }

    // Call the original function
    return g_psndENVOn_f_d_trampoline(flags, envName, wFadeTime, unused);
}

void generateLagSpike(Cheats *cheatsPtr, Mod *modPtr)
{
    (void)modPtr;

    if (!cheatsPtr->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_GENERATE_LAG_SPIKE))
    {
        return;
    }

    if (!cheatsPtr->checkCheatButtonCombo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_GENERATE_LAG_SPIKE))
    {
        return;
    }

    // Make sure the desired duration does not exceed 10 seconds
    GenerateLagSpikeCheat *generateLagSpikeCheatPtr = cheatsPtr->getGenerateLagSpikeCheatPtr();
    uint32_t duration = generateLagSpikeCheatPtr->getDuration();
    if (duration > LAG_SPIKE_MAX_DURATION)
    {
        // Reset the duration
        duration = LAG_SPIKE_DEFAULT_DURATION;
        generateLagSpikeCheatPtr->setDuration(duration);
    }

    const uint32_t startTick = OSGetTick();
    const uint32_t durationTick = duration * ((OSBusClock / 4) / 1000);

    while (1)
    {
        if ((OSGetTick() - startTick) >= durationTick)
        {
            break;
        }
    }
}

void lockMarioHpToMax(Cheats *cheatsPtr, Mod *modPtr)
{
    (void)modPtr;

    if (!cheatsPtr->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_LOCK_MARIO_HP_TO_MAX))
    {
        return;
    }

    // Set Mario's HP outside of battles
    PouchData *pouchPtr = pouchGetPtr();
    pouchPtr->currentHp = pouchPtr->maxHp;

    // Set Mario's HP inside of battles
    BattleWorkUnit *marioBattlePtr = getMarioBattlePtr();
    if (marioBattlePtr)
    {
        marioBattlePtr->current_hp = marioBattlePtr->max_hp;
    }
}

void bobberyEarly(Cheats *cheatsPtr, Mod *modPtr)
{
    (void)modPtr;

    if (checkForSpecificSeq(SeqIndex::kMapChange))
    {
        cheatsPtr->clearMiscFlag(CheatsMiscFlag::CHEATS_MISC_FLAG_MARIO_FREEZE);
    }

    if (cheatsPtr->miscFlagIsSet(CheatsMiscFlag::CHEATS_MISC_FLAG_MARIO_FREEZE))
    {
        const MapObject *rootMapObj = mapGetWork()->entries[0].rootMapObj;
        MapObject *mapObj = nullptr;

        if (rootMapObj)
        {
            mapObj = rootMapObj->child;
            if (mapObj)
            {
                mapObj = mapObj->next;
                if (mapObj)
                {
                    mapObj = mapObj->child;
                    if (mapObj)
                    {
                        mapObj = mapObj->child;
                    }
                }
            }
        }

        if (mapObj)
        {
            Player *playerPtr = marioGetPtr();
            playerPtr->flags1 |= 0x2;
            playerPtr->wJumpVelocityY = 0.f;
            playerPtr->wJumpAccelerationY = 0.f;
            playerPtr->unk_84 = 0.f;
            playerPtr->playerPosition.x = -1.f;
            playerPtr->playerPosition.y = 59.f;
            playerPtr->playerPosition.z = mapObj->modelWorldMtx[2][3] + 10.f;
            playerPtr->wPlayerAngleCurrent = 180.f;
            playerPtr->wPlayerAngle = 180.f;
            playerPtr->wPlayerDirectionCurrent = 180.f;
            playerPtr->wPlayerDirection = 180.f;
        }
    }

    if (!cheatsPtr->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_BOBBERY_EARLY))
    {
        return;
    }

    // Clear the flag for defeating the embers circling the tree that Bobbery is in
    swClear(3137);

    if (!compareStringToNextMap("muj_05"))
    {
        return;
    }

    // Allow the Ember to be refought
    NpcEntry *npcPtr = &npcWork[0].entries[5]; // NPC 6
    npcPtr->wJumpFlags &= 0xFFFFFF00;          // Clear byte 0x1D7 in npcPtr
}

void cancelSpawnSelectedItem()
{
    gCheats->getSpawnItemCheatPtr()->freeValueEditor();

    // Enable the pause menu and reset the System Level
    winOpenEnable();
    setSystemLevel(0);
}

void spawnSelectedItem(const ValueType *valuePtr)
{
    // Spawn the selected item
    Cheats *cheatsPtr = gCheats;
    SpawnItemCheat *spawnItemCheatPtr = cheatsPtr->getSpawnItemCheatPtr();

    const int32_t item = valuePtr->s32;
    spawnItemCheatPtr->setPrevItem(item);

    const uint32_t itemCounter = spawnItemCheatPtr->getItemCounter() + 1; // Start the string count at 1
    spawnItemCheatPtr->setItemCounter(itemCounter);

    char buf[32];
    snprintf(buf, sizeof(buf), "C_Item_%" PRIu32, itemCounter);

    Player *playerPtr = marioGetPtr();
    float coordinateXAdjustment = 30.f; // Mario is facing right

    if (playerPtr->wPlayerDirection <= 0.f)
    {
        // Mario is facing left
        coordinateXAdjustment = -coordinateXAdjustment;
    }

    itemEntry(buf,
              item,
              16,
              -1,
              nullptr,
              playerPtr->playerPosition.x + coordinateXAdjustment,
              playerPtr->playerPosition.y,
              playerPtr->playerPosition.z);

    // Close the value editor
    cancelSpawnSelectedItem();
}

void drawSpawnItemValueEditor(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // This function should only be called if the pointer for the value editor is set
    gCheats->getSpawnItemCheatPtr()->getValueEditorPtr()->draw();
}

void spawnItem(Cheats *cheatsPtr, Mod *modPtr)
{
    SpawnItemCheat *spawnItemCheatPtr = cheatsPtr->getSpawnItemCheatPtr();
    ValueEditor *valueEditorPtr = spawnItemCheatPtr->getValueEditorPtr();

    // Check if transitioning between rooms
    if (checkForSpecificSeq(SeqIndex::kMapChange))
    {
        // Reset the item counter to be safe
        spawnItemCheatPtr->setItemCounter(0);

        // Make sure the value editor isn't open, just in case the player tries to reload the room with it open or something
        valueEditorPtr = spawnItemCheatPtr->freeValueEditor();
    }

    if (cheatsPtr->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_SPAWN_ITEM) && !gMenu &&
        !modPtr->changingButtonCombo() && !memoryEditorIsOpen())
    {
        // Make sure a file is currently loaded
        if (!checkIfInGame())
        {
            return;
        }

        // Do not run if the pause menu is open
        if ((marioStGetSystemLevel() & 15) == 15)
        {
            return;
        }

        // Check to see if the value editor is open or not
        if (!valueEditorPtr)
        {
            // Check if the value editor should be opened
            if (cheatsPtr->checkCheatButtonCombo(CheatsWithButtonCombo::CHEATS_BUTTON_COMBO_SPAWN_ITEM))
            {
                // Disable the pause menu, set the System Level, and open the value editor
                winOpenDisable();
                setSystemLevel(1);

                valueEditorPtr = spawnItemCheatPtr->allocMemoryForValueEditor();

                uint32_t flags = 0;
                flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);
                flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Y_SET_MAX);
                flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Z_SET_MIN);
                flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_ITEM_ICON_AND_TEXT);

                const Window *rootWindowPtr = gRootWindow;
                const int32_t minValue = static_cast<int32_t>(ItemId::ITEM_STRANGE_SACK);
                const int32_t maxValue = static_cast<int32_t>(ItemId::ITEM_SUPER_CHARGE_P);

                valueEditorPtr->init(spawnItemCheatPtr->getPrevItemPtr(),
                                     &minValue,
                                     &maxValue,
                                     rootWindowPtr,
                                     flags,
                                     VariableType::s16,
                                     rootWindowPtr->getAlpha());

                valueEditorPtr->startDrawing(spawnSelectedItem, cancelSpawnSelectedItem);

                // Draw the value editor, but don't check for inputs on the frame that it was opened
                drawOnDebugLayer(drawSpawnItemValueEditor, DRAW_ORDER_CHEAT_SPAWN_ITEM);
            }
        }
        else
        {
            // Handle the value editor controls
            valueEditorPtr->controls(getMenuButtonInput(true));

            // Draw the value editor

            // The value editor may have been closed via its controls, in which the pointer for it will have been cleared, so
            // have to recheck it
            valueEditorPtr = spawnItemCheatPtr->getValueEditorPtr();
            if (valueEditorPtr)
            {
                drawOnDebugLayer(drawSpawnItemValueEditor, DRAW_ORDER_CHEAT_SPAWN_ITEM);
            }
        }
    }
    else if (valueEditorPtr)
    {
        // Close the value editor
        cancelSpawnSelectedItem();
    }
}

void lockFlags(Cheats *cheatsPtr, Mod *modPtr)
{
    (void)modPtr;

    LockFlagsCheat *lockFlagsPtr = cheatsPtr->getLockFlagsCheatPtr();

    // Loop through each region and lock flags for them
    for (uint32_t i = 0; i < TOTAL_LOCK_FLAGS_OPTIONS; i++)
    {
        if (!cheatsPtr->miscFlagIsSet(CheatsMiscFlag::CHEATS_MISC_FLAG_LOCK_GSW + i))
        {
            continue;
        }

        // Make sure the pointer for the current set of flags is set
        const uint8_t *prevMemory = lockFlagsPtr->getPrevMemoryPtr(i);
        if (!prevMemory)
        {
            continue;
        }

        // Restore the memory
        // Do other stuff depending on the index
        switch (i)
        {
            case LockFlagsOptions::LOCK_FLAGS_GSW:
            {
                // Restore the sequence position
                setSequencePosition(lockFlagsPtr->getSequencePosition());
                break;
            }
            case LockFlagsOptions::LOCK_FLAGS_LSW:
            case LockFlagsOptions::LOCK_FLAGS_LSWF:
            {
                // Only restore if currently in the original area where the flag was set
                const char *area;
                if (i == LockFlagsOptions::LOCK_FLAGS_LSW)
                {
                    area = lockFlagsPtr->getLswAreaLocked();
                }
                else
                {
                    area = lockFlagsPtr->getLswfAreaLocked();
                }

                if (strcmp(area, _next_area) != 0)
                {
                    continue;
                }
                break;
            }
            default:
            {
                break;
            }
        }

        // Restore the standard flags
        void *sourceMemory;
        uint32_t sourceSize;
        getLockFlagsRegionPtrAndSize(i, &sourceMemory, &sourceSize);
        memcpy(sourceMemory, prevMemory, sourceSize);
    }
}

void checkIfAreaFlagsShouldBeCleared(Cheats *cheatsPtr, Mod *modPtr)
{
    (void)modPtr;

    // Only run when the player has chosen to clear some flags
    if (!cheatsPtr->miscFlagIsSet(CheatsMiscFlag::CHEATS_MISC_FLAG_AREA_FLAGS_SHOULD_BE_CLEARED))
    {
        return;
    }

    // Check to see if currently in the process of clearing flags or not
    if (!cheatsPtr->miscFlagIsSet(CheatsMiscFlag::CHEATS_MISC_FLAG_AREA_FLAGS_START_CLEARING_FLAGS))
    {
        // Only run if currently in a screen transition
        if (!checkForSpecificSeq(SeqIndex::kMapChange))
        {
            return;
        }

        // Set up the flags to be cleared
        cheatsPtr->setMiscFlag(CheatsMiscFlag::CHEATS_MISC_FLAG_AREA_FLAGS_START_CLEARING_FLAGS);
    }
    else
    {
        // Keep resetting the flags until the seq changes to Game
        if (!checkForSpecificSeq(SeqIndex::kGame))
        {
            // Clear the currently-selected flags
            clearAreaFlags(cheatsPtr->getClearAreaFlagsCheat()->getSelectedArea());
        }
        else
        {
            cheatsPtr->clearMiscFlag(CheatsMiscFlag::CHEATS_MISC_FLAG_AREA_FLAGS_SHOULD_BE_CLEARED);
            cheatsPtr->clearMiscFlag(CheatsMiscFlag::CHEATS_MISC_FLAG_AREA_FLAGS_START_CLEARING_FLAGS);
        }
    }
}

// Handle setting up/calling cheats
const CheatsArrayFunc gCheatsWithButtonCombos[] = {
    saveMarioAndPartnerPositions,
    loadMarioAndPartnerPositions,
    setTextStorage,
    setTimeStopTextStorage,
    lockMarioHpToMax,
    levitate,
    generateLagSpike,
};

const CheatsArrayFunc gCheatsNoButtonCombos[] = {
    walkThroughMostObjects, // Needs to always run due to a variable constantly being checked
    saveAnywhere,           // Needs to always run due to the script taking more than one frame
    spawnItem,              // Needs to always run due to the value editor sometimes being open
    speedUpMario,           // Needs to always run due to Mario's base speed constantly being set
    reloadRoom,             // Needs to always run due to the extra code that always does some failsafe checking
    bobberyEarly,
    checkIfAreaFlagsShouldBeCleared,
    checkIfSystemLevelShouldBeLowered,
    lockFlags,
};

void runCheatFuncsEveryFrame()
{
    Cheats *cheatsPtr = gCheats;
    Mod *modPtr = &gMod;

    // Run each cheat function that is button-based
    // Only run button-based codes if currently not changing button combos
    if (!modPtr->changingButtonCombo())
    {
        constexpr uint32_t loopCount = sizeof(gCheatsWithButtonCombos) / sizeof(CheatsArrayFunc);
        const CheatsArrayFunc *cheatsWithButtonCombosPtr = gCheatsWithButtonCombos;

        for (uint32_t i = 0; i < loopCount; i++)
        {
            const CheatsArrayFunc func = cheatsWithButtonCombosPtr[i];
            if (func)
            {
                func(cheatsPtr, modPtr);
            }
        }
    }

    // Run each cheat function that isn't button-based
    constexpr uint32_t loopCount = sizeof(gCheatsNoButtonCombos) / sizeof(CheatsArrayFunc);
    const CheatsArrayFunc *cheatsNoButtonCombosPtr = gCheatsNoButtonCombos;

    for (uint32_t i = 0; i < loopCount; i++)
    {
        const CheatsArrayFunc func = cheatsNoButtonCombosPtr[i];
        if (func)
        {
            func(cheatsPtr, modPtr);
        }
    }
}
