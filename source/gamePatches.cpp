#include "mod.h"
#include "gamePatches.h"
#include "assembly.h"
#include "patch.h"
#include "menuUtils.h"
#include "cheats.h"
#include "displays.h"
#include "memoryEditor.h"
#include "customState.h"
#include "gc/types.h"
#include "gc/pad.h"
#include "gc/OSCache.h"
#include "misc/utils.h"
#include "misc/functionHooks.h"
#include "ttyd/mariost.h"
#include "ttyd/mario_pouch.h"
#include "ttyd/memory.h"
#include "ttyd/party.h"
#include "ttyd/seq_mapchange.h"
#include "ttyd/seqdrv.h"
#include "ttyd/fontmgr.h"
#include "ttyd/mario.h"
#include "ttyd/seq_title.h"
#include "ttyd/npcdrv.h"
#include "ttyd/swdrv.h"
#include "menus/cheatsMenu.h"
#include "menus/warpsMenu.h"
#include "menus/battlesMenu.h"

#ifdef TTYD_EU
#include "gc/OSReset.h"
#endif

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cinttypes>

uint32_t cFixBlooperCrash1(uint32_t unkValue, void *battleUnitPtr)
{
    if (ptrIsValid(battleUnitPtr))
    {
#ifdef TTYD_US
        uint32_t offset = 0x218;
#elif defined TTYD_JP
        uint32_t offset = 0x214;
#elif defined TTYD_EU
        uint32_t offset = 0x218;
#endif
        *reinterpret_cast<uint32_t *>(reinterpret_cast<uint32_t>(battleUnitPtr) + offset) = unkValue;
    }

    // Restore the value that was in r3
    return 2;
}

PartyEntry *cFixEvtMapSetFlagCrash(PartyEntry *partyPtr, bool shouldSpawnPartner)
{
    // Bring out a partner/follower if no partner/follower is currently out
    if (partyPtr)
    {
        return partyPtr;
    }

    return partyGetPtr(spawnFailsafePartnerOrFollower(shouldSpawnPartner));
}

void cPreventDiaryTextboxOptionSelection(const char *currentText, int32_t *storeAddress, int32_t selectedOption)
{
    enum DiaryTextboxOption
    {
        FirstOption = 0,
        SecondOption,
        ThirdOption,
    };

    int32_t newOption = selectedOption;

    // Only need to check if trying to read the diary
    if (strcmp(currentText, "stg6_rsh_diary_01_yn") == 0)
    {
        // Prevent the first option from being selected, so that the game does not crash when reading the diary
        // Only needs to run when not on the train
        if (strcmp(_next_area, "rsh") != 0)
        {
            if (selectedOption == DiaryTextboxOption::ThirdOption)
            {
                seqSetSeq(SeqIndex::kGameOver, nullptr, nullptr);
            }

            newOption = DiaryTextboxOption::SecondOption;
        }
    }

    // Restore the overwritten instruction
    *storeAddress = newOption;
}

bool cDisplayMegaBadgesInBattleMenu(bool checkJumpBadge, uint32_t bitfield)
{
    if (checkJumpBadge)
    {
        if (checkIfBadgeEquipped(ItemId::ITEM_MEGA_JUMP))
        {
            // Return false to make the branch be taken
            return false;
        }

        // Restore the original instruction
        return bitfield & 0x200;
    }
    else
    {
        if (checkIfBadgeEquipped(ItemId::ITEM_MEGA_SMASH) || checkIfBadgeEquipped(ItemId::ITEM_MEGA_QUAKE))
        {
            // Return false to make the branch be taken
            return false;
        }

        // Restore the original instruction
        return bitfield & 0x400;
    }
}

bool cFontDrawMessageMtxHandleCommand(const char *command, const char *commandValueString)
{
    if (strcmp(command, "col") == 0)
    {
        uint32_t color;
        sscanf(commandValueString, "%" PRIx32, &color);
        FontDrawColor(reinterpret_cast<uint8_t *>(&color));
    }
    else if (strcmp(command, "/col") == 0)
    {
        FontDrawColorIDX(0);
    }
    else
    {
        // Command not found
        return false;
    }

    // Command found
    return true;
}

#ifdef TTYD_JP
int32_t cCrashScreenDecrementYPos()
{
    static int32_t decrementCount = 0;
    int32_t tempDecrementCount = decrementCount;

    constexpr int32_t kPosYIncrementAmount = 50;
    int32_t textPosY = tempDecrementCount + kPosYIncrementAmount;

    if (--tempDecrementCount < -960)
    {
        // Simulate incrementing exactly once to match the US/PAL code
        decrementCount = -1;
        return kPosYIncrementAmount;
    }
    else
    {
        decrementCount = tempDecrementCount;
        return textPosY;
    }
}
#endif

uint16_t cArbitraryMemoryWriteDisableRandomFail(uint16_t flags)
{
    if (!gMod->flagIsSet(ModFlag::MOD_FLAG_PERFORMING_AMW_GLITCH))
    {
        return flags;
    }

    // Return 0 to force the branch to always be taken, thus disabling the random chance for the glitch to fail
    return 0;
}

uint32_t cArbitraryMemoryWriteGetProperPointer(uint32_t pointerRaw)
{
    if (!gMod->flagIsSet(ModFlag::MOD_FLAG_PERFORMING_AMW_GLITCH))
    {
        return pointerRaw;
    }

    // Return the pointer value that would be used under vanilla scenarios
#ifdef TTYD_JP
    constexpr uint32_t vanillaPointerRaw = 0x806E0640;
#elif defined TTYD_US
    constexpr uint32_t vanillaPointerRaw = 0x806EED40;
#elif defined TTYD_EU
    // One of two pointer values is used depending on different circumstances
    constexpr uint32_t firstPointerValue = 0x8072FC60;
    uint32_t vanillaPointerRaw;

    // BUG - If the player resets before selecting a Hz setting, then `firstPointerValue` will still be used even if
    // OSGetResetCode does not return 0. Need to look into how to get around this.
    if (OSGetResetCode() == 0) // First boot
    {
        // First boot should always use the same pointer value
        vanillaPointerRaw = firstPointerValue;
    }
    else if (_globalWorkPtr->framerate == 60)
    {
        // A different pointer value is used when the game was reset at least once and 60Hz was chosen
        vanillaPointerRaw = 0x806FB860;
    }
    else
    {
        // Assume 50Hz is being used, in which the first pointer value will still be used
        vanillaPointerRaw = firstPointerValue;
    }
#endif

    return vanillaPointerRaw;
}

static void *fixPouchInitMemoryLeak(int32_t heap, uint32_t size)
{
    // Check if the memory has already been allocated or not
    PouchData *pouchPtr = pouchGetPtr();
    if (pouchPtr)
    {
        // The memory has already been allocated
        return pouchPtr;
    }
    else
    {
        // The memory has not been allocated, so allocate it
        return __memAlloc(heap, size);
    }
}

const char *getCustomMessage(const char *key)
{
    const char *nextAreaPtr = _next_area;

    if (strcmp(key, "stg6_rsh_diary_01") == 0)
    {
        // Change the text asking if you want to read the diary
        // Only needs to change when not on the train
        if (strcmp(nextAreaPtr, "rsh") != 0)
        {
            return "<system>\nThis is error text.\n<o>";
        }
    }
    else if (strcmp(key, "stg6_rsh_diary_01_yn") == 0)
    {
        // Change the yes/no text answers for the diary
        // Only needs to change when not on the train
        if (strcmp(nextAreaPtr, "rsh") != 0)
        {
#ifdef TTYD_JP
            const char *message = "<select 0 0 300 40>Yes\nNo\nChoose this option";
#else
            const char *message = "<select 0 0 0 40>Yes\nNo\nChoose this option";
#endif
            return message;
        }
    }

    // Call the original function
    return g_msgSearch_trampoline(key);
}

int32_t pauseMenuPreventUnpause(void *pauseMenuPtr)
{
    // Prevent unpausing if L + Start is being held
    if (checkButtonComboEveryFrame(OPEN_CLOSE_MENU_BUTTON_COMBO))
    {
        // Prevent being able to close the menu if L + Start is being held
        return 0;
    }

    // Call the original function
    return g_winRootMain_trampoline(pauseMenuPtr);
}

int32_t fixMarioKeyOn()
{
    Player *playerPtr = marioGetPtr();
    const int32_t key = playerPtr->wKey;

    // Make sure the value is greater than 0
    if (key < 1)
    {
        playerPtr->wKey = 1;
    }

    // Call the original function
    return g_marioKeyOn_trampoline();
}

bool applyRelPatches(OSModuleInfo *module, void *bss)
{
    // Call the original function immediately, as the rel file should be linked before applying patches
    const bool ret = g_OSLink_trampoline(module, bss);

    // Make sure the linking process succeeded
    if (!ret)
    {
        return ret;
    }

#ifdef TTYD_US
    // bom
    constexpr uint32_t minnnanokoe_evt_bom_Offset = 0xD2FC;
    constexpr uint32_t minnnanokoe_evt_bom_WaitMS_Offset = 0x674;

    // las
    constexpr uint32_t last_evt_3_2_Offset = 0x36FBC;
    constexpr uint32_t last_evt_3_2_Loop_Offset = 0x64;
    constexpr uint32_t last_evt_3_2_WaitMS_Offset = 0x114;

    // mri
    constexpr uint32_t minnnanokoe_evt_mri_Offset = 0x2BE80;
    constexpr uint32_t minnnanokoe_evt_mri_WaitMS_Offset = 0x968;
#elif defined TTYD_JP
    // bom
    constexpr uint32_t minnnanokoe_evt_bom_Offset = 0xD2C4;
    constexpr uint32_t minnnanokoe_evt_bom_WaitMS_Offset = 0x634;

    // las
    constexpr uint32_t last_evt_3_2_Offset = 0x36FC0;
    constexpr uint32_t last_evt_3_2_Loop_Offset = 0x58;
    constexpr uint32_t last_evt_3_2_WaitMS_Offset = 0xD4;

    // mri
    constexpr uint32_t minnnanokoe_evt_mri_Offset = 0x2BA98;
    constexpr uint32_t minnnanokoe_evt_mri_WaitMS_Offset = 0x968;
#elif defined TTYD_EU
    // bom
    constexpr uint32_t minnnanokoe_evt_bom_Offset = 0xD35C;
    constexpr uint32_t minnnanokoe_evt_bom_WaitMS_Offset = 0x634;

    // las
    constexpr uint32_t last_evt_3_2_Offset = 0x36FBC;
    constexpr uint32_t last_evt_3_2_Loop_Offset = 0x64;
    constexpr uint32_t last_evt_3_2_WaitMS_Offset = 0x114;

    // mri
    constexpr uint32_t minnnanokoe_evt_mri_Offset = 0x2BE80;
    constexpr uint32_t minnnanokoe_evt_mri_WaitMS_Offset = 0x968;
#endif

    const uint32_t relPtrRaw = reinterpret_cast<uint32_t>(module);
    switch (module->id)
    {
        case RelId::BOM:
        {
            // Shadow Queen cutscene fix, just before 2nd fight
            // Make changes to the script that spawns the textboxes, to prevent the standard heap from being corrupted
            uint32_t waitMsAddress = relPtrRaw + minnnanokoe_evt_bom_Offset + minnnanokoe_evt_bom_WaitMS_Offset;

            // Wait 400ms instead of 300ms at the end of the loop
            // Use applyAssemblyPatch to apply the change and clear the cache, all in a single function call
            applyAssemblyPatch(waitMsAddress, 400);
            break;
        }
        case RelId::LAS:
        {
            // Shadow Queen cutscene fix, just before 2nd fight
            // Make changes to the script that spawns the textboxes, to prevent the standard heap from being corrupted
            uint32_t loopCountAddress = relPtrRaw + last_evt_3_2_Offset + last_evt_3_2_Loop_Offset;
            uint32_t waitMsAddress = relPtrRaw + last_evt_3_2_Offset + last_evt_3_2_WaitMS_Offset;

            // Change the loop count from 10 to 5
            // Use applyAssemblyPatch to apply the change and clear the cache, all in a single function call
            applyAssemblyPatch(loopCountAddress, 5);

            // Wait 400ms instead of 200ms at the end of the loop
            // Use applyAssemblyPatch to apply the change and clear the cache, all in a single function call
            applyAssemblyPatch(waitMsAddress, 400);
            break;
        }
        case RelId::MRI:
        {
            // Shadow Queen cutscene fix, just before 2nd fight
            // Make changes to the script that spawns the textboxes, to prevent the standard heap from being corrupted
            uint32_t waitMsAddress = relPtrRaw + minnnanokoe_evt_mri_Offset + minnnanokoe_evt_mri_WaitMS_Offset;

            // Wait 400ms instead of 300ms at the end of the loop
            // Use applyAssemblyPatch to apply the changes and clear the cache, all in a single function call
            applyAssemblyPatch(waitMsAddress, 400);
            break;
        }
        default:
        {
            break;
        }
    }

    return ret;
}

bool performPreBattleActions()
{
    // Make sure the Jump and Hammer upgrades have been properly checked
    recheckJumpAndHammerLevels();

    PouchData *pouchPtr = pouchGetPtr();
    Mod *modPtr = gMod;

    // Clear the cache for Mario's stats if they were changed manually
    if (modPtr->flagIsSet(ModFlag::MOD_FLAG_CLEAR_MARIO_STATS))
    {
        modPtr->clearFlag(ModFlag::MOD_FLAG_CLEAR_MARIO_STATS);

        const uint32_t size = (&pouchPtr->starPoints - &pouchPtr->currentHp) * sizeof(int16_t);
        DCFlushRange(&pouchPtr->currentHp, size + sizeof(int16_t));
    }

    // Clear the cache for the partners' stats if they were changed manually
    if (modPtr->flagIsSet(ModFlag::MOD_FLAG_CLEAR_PARTNER_STATS))
    {
        modPtr->clearFlag(ModFlag::MOD_FLAG_CLEAR_PARTNER_STATS);
        DCFlushRange(pouchPtr->partyData, sizeof(pouchPtr->partyData));
    }

    // Call the original function
    return g_battle_init_trampoline();
}

void unequipJumpmanHammermanActionCommandsTutorial()
{
    if (strcmp(_next_map, "gor_02") != 0)
    {
        return;
    }

    // Check if the tutorial version of Goombella is in the battle
    BattleWorkUnit *actorPtr = getActorBattlePtr(2);
    if (!actorPtr || (actorPtr->current_kind != BattleUnitType::kUnitTutorialGoombella))
    {
        return;
    }

    // Clear the values for Jumpman and Hammerman
    BattleWorkUnit *marioBattlePtr = getMarioBattlePtr();
    if (marioBattlePtr)
    {
        BadgesEquipped *equippedBadgesPtr = &marioBattlePtr->badges_equipped;
        equippedBadgesPtr->jumpman = 0;
        equippedBadgesPtr->hammerman = 0;
    }
}

void performBattleChecks()
{
    // Handle the Auto Action Commands cheat
    handleAutoActionCommands();

    // Prevent Jumpman and Hammerman from being equipped when in the Action Commands tutorial, as this leads to a softlock
    unequipJumpmanHammermanActionCommandsTutorial();

    // Prevent all buttons from being pressed when either the menu is open or the memory editor is open, except for R and X
    if (gMenu || memoryEditorIsOpen())
    {
        if (!checkButtonsEveryFrame(PadInput::PAD_R | PadInput::PAD_X))
        {
            // The menu is open and neither R nor X are being pressed, so prevent the original function from running
            return;
        }
    }

    // Call the original function
    return g_BattlePadManager_trampoline();
}

NpcEntry *fbatHitCheck_Work(uint32_t flags, void *unk)
{
    // Call the original function immediately
    NpcEntry *ret = g_fbatHitCheck_trampoline(flags, unk);

    // Check for enemy encounters
    checkForEnemyEncounters(ret);

    // Check if battles should be disabled
    if (disableBattles())
    {
        return nullptr;
    }
    else
    {
        return ret;
    }
}

static void fixMapProblems() // Gets called in initStageEvents
{
    const uint32_t sequencePosition = getSequencePosition();

    if (compareStringToNextMap("nok_00"))
    {
        // Prevent the game from crashing if the player enters the intro cutscene after interacting with an NPC that is past
        // slot 10
        // Check if the cutscene is going to play
        if (sequencePosition < 26)
        {
            // Clear the pointer used to check which animation Mario should use when greeting the Koopa
            fbatGetPointer()->wHitNpc = nullptr; // Mario will do no animation when the pointer is not set
        }
    }
    else if (compareStringToNextMap("rsh_05_a"))
    {
        // Prevent the game from crashing if the player enters rsh_05_a with the sequence past 338
        if (sequencePosition > 338)
        {
            // Set the sequence to 338 to prevent the crash
            setSequencePosition(338);
        }
    }
    else if (compareStringToNextMap("aji_13"))
    {
        // Prevent the game from crashing if the conveyor belt has not been activated
        // Set GW(11) to 0 upon entering the room to prevent the crash
        setGW(11, 0);
    }
    else if (compareStringToNextMap("las_08"))
    {
        // Prevent the game from crashing if the player entered las_08 with the sequence at 385 and GSW(1121) at 7
        if (sequencePosition == 385)
        {
            // Check if GSW(1121) is currently higher than 6
            if (swByteGet(1121) > 6)
            {
                // Lower the value to 6 to prevent the game from crashing
                swByteSet(1121, 6);
            }
        }
    }
}

static GlobalWork *initStageEvents()
{
    CustomState *customStatePtr = &gCustomState;
    Mod *modPtr = gMod;

    // TODO: Add if check for custom states
    if (customStatePtr->stateWasSelected())
    {
        customStatePtr->clearInit();
        modPtr->clearFlag(ModFlag::MOD_FLAG_WARP_BY_EVENT_INIT); // Failsafe
        customStatePtr->loadState(customStatePtr->getSelectedState());
    }
    else if (modPtr->flagIsSet(ModFlag::MOD_FLAG_WARP_BY_EVENT_INIT))
    {
        modPtr->clearFlag(ModFlag::MOD_FLAG_WARP_BY_EVENT_INIT);
        customStatePtr->clearInit(); // Failsafe
        handleWarpByEvent();
    }

    // Override any flags set in this function if the previous flags were locked
    lockFlags(gCheats, modPtr);

    // Check to see if any fixes need to be applied to specific maps
    fixMapProblems();

    // The overwritten instruction sets r3 to the global work pointer, so return that
    return _globalWorkPtr;
}

void psndSFXOff_Work(int32_t flags)
{
    // Check if the AMW glitch is being performed
    // If the last byte of `flags` is 0xD8, then assume the glitch is being performed
    if ((flags & 0xFF) != 0xD8)
    {
        // Call the original function
        return g_psndSFXOff_trampoline(flags);
    }

#ifdef TTYD_JP
    Displays *displaysPtr = gDisplays;
    if (displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_AMW_SPIN_JUMP))
    {
        // Successfully performed the trick, so set the misc flag for it
        displaysPtr->setMiscFlag(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_AMW_SPIN_JUMP_SUCCESSFULLY_PERFORMED_TRICK);
    }
#endif

    // Set the flag to indicate that the glitch is being performed
    Mod *modPtr = gMod;
    modPtr->setFlag(ModFlag::MOD_FLAG_PERFORMING_AMW_GLITCH);

    // Call the original function
    g_psndSFXOff_trampoline(flags);

    // Clear the flag now that the glitch has been performed
    modPtr->clearFlag(ModFlag::MOD_FLAG_PERFORMING_AMW_GLITCH);
}

void applyGameFixes()
{
    // Fix a memory leak from occuring when starting a new file. The leak is that memory for the inventory is re-allocated
    // without checking if it's already allocated.
#ifdef TTYD_US
    constexpr uint32_t pouchInitMemoryLeakAddress = 0x800D59DC;
#elif defined TTYD_JP
    constexpr uint32_t pouchInitMemoryLeakAddress = 0x800D17A8;
#elif defined TTYD_EU
    constexpr uint32_t pouchInitMemoryLeakAddress = 0x800D67E4;
#endif

    writeBranchBL(pouchInitMemoryLeakAddress, fixPouchInitMemoryLeak);

    // Fix the game not properly allocating enough memory for text in textboxes. The vanilla code does not allocate memory for
    // the null terminator.
#ifdef TTYD_US
    constexpr uint32_t msgWindowMrAddress = 0x800816F4;
#elif defined TTYD_JP
    constexpr uint32_t msgWindowMrAddress = 0x80080B6C;
#elif defined TTYD_EU
    constexpr uint32_t msgWindowMrAddress = 0x800829B0;
#endif

    applyAssemblyPatch(msgWindowMrAddress, 0x38830001); // addi r4,r3,1

    // Fix a crash that can occur when defeating Blooper with an attack that causes both the body and the left tentacle to be
    // defeated at roughly the same time
#ifdef TTYD_US
    constexpr uint32_t blooperCrash1Address = 0x8010F810;
    constexpr uint32_t blooperCrash2Address = 0x8010F87C;
#elif defined TTYD_JP
    constexpr uint32_t blooperCrash1Address = 0x8010A724;
    constexpr uint32_t blooperCrash2Address = 0x8010A790;
#elif defined TTYD_EU
    constexpr uint32_t blooperCrash1Address = 0x801106E8;
    constexpr uint32_t blooperCrash2Address = 0x80110754;
#endif

    writeBranchBL(blooperCrash1Address, asmFixBlooperCrash1);
    writeBranchBL(blooperCrash2Address, asmFixBlooperCrash2);

    // Fix crashes that can occur when evt_map_blend_set_flag is trying to apply effects to partners/followers when one is not
    // out
#ifdef TTYD_US
    constexpr uint32_t evtMapBlendSetFlagPartnerCrashAddress = 0x800389C4;
    constexpr uint32_t evtMapBlendSetFlagFollowerCrashAddress = 0x80038A0C;
#elif defined TTYD_JP
    constexpr uint32_t evtMapBlendSetFlagPartnerCrashAddress = 0x80038328;
    constexpr uint32_t evtMapBlendSetFlagFollowerCrashAddress = 0x80038370;
#elif defined TTYD_EU
    constexpr uint32_t evtMapBlendSetFlagPartnerCrashAddress = 0x80038AAC;
    constexpr uint32_t evtMapBlendSetFlagFollowerCrashAddress = 0x80038AF4;
#endif

    writeStandardBranches(evtMapBlendSetFlagPartnerCrashAddress,
                          asmFixEvtMapBlendSetFlagPartnerCrashStart,
                          asmFixEvtMapBlendSetFlagPartnerCrashBranchBack);

    writeStandardBranches(evtMapBlendSetFlagFollowerCrashAddress,
                          asmFixEvtMapBlendSetFlagFollowerCrashStart,
                          asmFixEvtMapBlendSetFlagFollowerCrashBranchBack);
}

void applyVariousGamePatches()
{
    // Enable debug mode by default
    // Use applyAssemblyPatch to apply the change and clear the cache, all in a single function call
    applyAssemblyPatch(&seqTitleWorkPtr2->debug, static_cast<uint32_t>(-1));

    // The debug mode is reset whenever the title screen is reached, so adjust it to always enable debug mode instead
#ifdef TTYD_US
    constexpr uint32_t debugModeInitializeAddress = 0x80009B2C;
#elif defined TTYD_JP
    constexpr uint32_t debugModeInitializeAddress = 0x8000999C;
#elif defined TTYD_EU
    constexpr uint32_t debugModeInitializeAddress = 0x80009CF0;
#endif

    applyAssemblyPatch(debugModeInitializeAddress, 0x3800FFFF); // li r0,-1

    // Always show the build date on the title screen
#ifdef TTYD_US
    constexpr uint32_t debugModeShowBuildDateAddress = 0x80008FE8;
#elif defined TTYD_JP
    constexpr uint32_t debugModeShowBuildDateAddress = 0x80008EB8;
#elif defined TTYD_EU
    constexpr uint32_t debugModeShowBuildDateAddress = 0x800091B4;
#endif

    applyAssemblyPatch(debugModeShowBuildDateAddress, 0x60000000); // nop

    // Adjust the size of the text on the crash screen
#ifdef TTYD_US
    constexpr uint32_t crasheScreenTextSizeAddress = 0x80428BC0;
#elif defined TTYD_JP
    constexpr uint32_t crasheScreenTextSizeAddress = 0x80422618;
#elif defined TTYD_EU
    constexpr uint32_t crasheScreenTextSizeAddress = 0x804356C8;
#endif

    // Use applyAssemblyPatch to apply the change and clear the cache, all in a single function call
    // The float value will be initialized via lis and ori in the assembly when handled like this
#ifdef TTYD_JP
    constexpr float newTextSize = 0.7f;
#else
    constexpr float newTextSize = 0.66f;
#endif
    const ValueType *newTextSizePtr = reinterpret_cast<const ValueType *>(&newTextSize);
    applyAssemblyPatch(crasheScreenTextSizeAddress, newTextSizePtr->u32);

    // Make the crash screen scroll and loop back around once it has gone offscreen
#ifdef TTYD_US
    constexpr uint32_t crashScreenPPCHaltBranchAddress = 0x8025E4A4;
    constexpr uint32_t crashScreenEndBranchAddress = 0x8025E4A8;
#elif defined TTYD_JP
    constexpr uint32_t crashScreenPosYValueAddress = 0x802582F8;
#elif defined TTYD_EU
    constexpr uint32_t crashScreenPPCHaltBranchAddress = 0x8026207C;
    constexpr uint32_t crashScreenEndBranchAddress = 0x80262080;
#endif

#ifdef TTYD_JP
    writeStandardBranches(crashScreenPosYValueAddress, asmCrashScreenDecrementYPosStart, asmCrashScreenDecrementYPosBranchBack);
#else
    applyAssemblyPatch(crashScreenPPCHaltBranchAddress, 0x3B400000); // li r26,0
    applyAssemblyPatch(crashScreenEndBranchAddress, 0x4BFFFDD4);     // b -0x22C
#endif

    // Allow for running various init code that requires the game's heaps to already be created
#ifdef TTYD_US
    constexpr uint32_t marioStInitAddress = 0x8006FE38;
#elif defined TTYD_JP
    constexpr uint32_t marioStInitAddress = 0x8006EBD8;
#elif defined TTYD_EU
    constexpr uint32_t marioStInitAddress = 0x800710F4;
#endif

    writeBranchBL(marioStInitAddress, initAfterHeapsCreated);

    // Prevent upgrade item cutscenes from occuring, and instead just give them like standard items. This is needed because the
    // upgrades could potentially be obtained in maps other than where they are intended to be obtained, which will cause a
    // crash since the data for their cutscenes will not be available.
#ifdef TTYD_US
    constexpr uint32_t preventUpgradeItemCutscenesAddress = 0x800ABD04;
#elif defined TTYD_JP
    constexpr uint32_t preventUpgradeItemCutscenesAddress = 0x800AA048;
#elif defined TTYD_EU
    constexpr uint32_t preventUpgradeItemCutscenesAddress = 0x800AD0D4;
#endif

    applyAssemblyPatch(preventUpgradeItemCutscenesAddress, 0x60000000); // nop

    // Prevent the game from crashing if the player tries to read the diary while not on the Excess Express
#ifdef TTYD_US
    constexpr uint32_t preventDiaryTextboxSelectionAddress = 0x800D214C;
#elif defined TTYD_JP
    constexpr uint32_t preventDiaryTextboxSelectionAddress = 0x800CE01C;
#elif defined TTYD_EU
    constexpr uint32_t preventDiaryTextboxSelectionAddress = 0x800D2F44;
#endif

    writeBranchBL(preventDiaryTextboxSelectionAddress, asmPreventDiaryTextboxSelection);

    // Always show the Mega Jump and Mega Hammer options in the battle menu if those badges are equipped, as normally they are
    // only present if the player has the Ultra Boots or Ultra Hammer respectively
#ifdef TTYD_US
    constexpr uint32_t battleMenuJumpAddress = 0x80122BA4;
    constexpr uint32_t battleMenuHammerAddress = 0x80122BB8;
#elif defined TTYD_JP
    constexpr uint32_t battleMenuJumpAddress = 0x8011D6DC;
    constexpr uint32_t battleMenuHammerAddress = 0x8011D6F0;
#elif defined TTYD_EU
    constexpr uint32_t battleMenuJumpAddress = 0x80123AE4;
    constexpr uint32_t battleMenuHammerAddress = 0x80123AF8;
#endif

    writeBranchBL(battleMenuJumpAddress, asmDisplayMegaJumpBadgeInBattleMenu);
    writeBranchBL(battleMenuHammerAddress, asmDisplayMegaHammerBadgesInBattleMenu);

    // Always show the partner menu, even if the sequence is below 7 and the player has no partners
    // Always show the badge menu, even if the sequence is below 20 and the player has no badges
#ifdef TTYD_US
    constexpr uint32_t pauseMenuPartnerMenuAddress = 0x801649A0;
    constexpr uint32_t pauseMenuBadgeMenuAddress = 0x80164A44;
#elif defined TTYD_JP
    constexpr uint32_t pauseMenuPartnerMenuAddress = 0x8015EFBC;
    constexpr uint32_t pauseMenuBadgeMenuAddress = 0x8015F060;
#elif defined TTYD_EU
    constexpr uint32_t pauseMenuPartnerMenuAddress = 0x80166490;
    constexpr uint32_t pauseMenuBadgeMenuAddress = 0x80166534;
#endif

    applyAssemblyPatch(pauseMenuPartnerMenuAddress, 0x60000000); // nop
    applyAssemblyPatch(pauseMenuBadgeMenuAddress, 0x60000000);   // nop

    // Allow FontDrawMessageMtx to handle more custom commands like FontDrawMessage can
#ifdef TTYD_US
    constexpr uint32_t fontDrawMessageMtxHandleCommandAddress = 0x800759E0;
#elif defined TTYD_JP
    constexpr uint32_t fontDrawMessageMtxHandleCommandAddress = 0x8007471C;
#elif defined TTYD_EU
    constexpr uint32_t fontDrawMessageMtxHandleCommandAddress = 0x80076C9C;
#endif

    writeStandardBranches(fontDrawMessageMtxHandleCommandAddress,
                          asmFontDrawMessageMtxHandleCommandStart,
                          asmFontDrawMessageMtxHandleCommandBranchBack);

    // In order for custom states and warping by event to work properly, they need to handle their main code at a specific point
    // during the screen transition, so hook a specific address for that. A function for checking for and fixing problems in
    // specific rooms will also run at this address, as that needs to run at this time as well.
#ifdef TTYD_US
    constexpr uint32_t initStageEventsAddress = 0x800080E4;
#elif defined TTYD_JP
    constexpr uint32_t initStageEventsAddress = 0x80008054;
#elif defined TTYD_EU
    constexpr uint32_t initStageEventsAddress = 0x800082BC;
#endif

    writeBranchBL(initStageEventsAddress, initStageEvents);

    // Allow disabling the random fail chance for the Arbitrary Memory Write glitch
#ifdef TTYD_US
    constexpr uint32_t psndSFXOffAddress = 0x800D963C;
#elif defined TTYD_JP
    constexpr uint32_t psndSFXOffAddress = 0x800D5240;
#elif defined TTYD_EU
    constexpr uint32_t psndSFXOffAddress = 0x800DA438;
#endif

    writeBranchBL(psndSFXOffAddress, asmArbitraryMemoryWriteDisableRandomFail);

    // Get the proper pointer value that the vanilla game would use for the Arbitrary Memory Write glitch
#ifdef TTYD_US
    constexpr uint32_t SoundEfxStopAddress = 0x800E0C58;
#elif defined TTYD_JP
    constexpr uint32_t SoundEfxStopAddress = 0x800DC560;
#elif defined TTYD_EU
    constexpr uint32_t SoundEfxStopAddress = 0x800E1A54;
#endif

    writeBranchBL(SoundEfxStopAddress, asmArbitraryMemoryWriteGetProperPointer);
}

void applyCheatAndDisplayInjects()
{
    // Allow running from all battles
#ifdef TTYD_US
    constexpr uint32_t allowRunningFromBattlesAddress = 0x80123CA4;
#elif defined TTYD_JP
    constexpr uint32_t allowRunningFromBattlesAddress = 0x8011E7DC;
#elif defined TTYD_EU
    constexpr uint32_t allowRunningFromBattlesAddress = 0x80124BE4;
#endif

    writeStandardBranches(allowRunningFromBattlesAddress,
                          asmAllowRunningFromBattlesStart,
                          asmAllowRunningFromBattlesBranchBack);

    // Force NPC Item Drop
#ifdef TTYD_US
    constexpr uint32_t forceNpcItemDropAddress = 0x8004EC10;
#elif defined TTYD_JP
    constexpr uint32_t forceNpcItemDropAddress = 0x8004DFB0;
#elif defined TTYD_EU
    constexpr uint32_t forceNpcItemDropAddress = 0x8004ECDC;
#endif

    writeBranchBL(forceNpcItemDropAddress, asmForceNpcItemDrop);

#ifdef TTYD_US
    constexpr uint32_t replaceJumpAnimAddress = 0x800411D0;
#elif defined TTYD_JP
    constexpr uint32_t replaceJumpAnimAddress = 0x80040B34;
#elif defined TTYD_EU
    constexpr uint32_t replaceJumpAnimAddress = 0x800412B8;
#endif

    writeBranchBL(replaceJumpAnimAddress, asmReplaceJumpFallAnim);

    // Fall Through Most Objects
#ifdef TTYD_US
    constexpr uint32_t fallThroughMostObjectsStandAddress = 0x8008E9DC;
    constexpr uint32_t fallThroughMostObjectsTubeAddress = 0x8008E1E8;
    constexpr uint32_t fallThroughMostObjectsBowserAddress = 0x8021A30C;
#elif defined TTYD_JP
    constexpr uint32_t fallThroughMostObjectsStandAddress = 0x8008D428;
    constexpr uint32_t fallThroughMostObjectsTubeAddress = 0x8008CC4C;
    constexpr uint32_t fallThroughMostObjectsBowserAddress = 0x80215668;
#elif defined TTYD_EU
    constexpr uint32_t fallThroughMostObjectsStandAddress = 0x8008FD38;
    constexpr uint32_t fallThroughMostObjectsTubeAddress = 0x8008F544;
    constexpr uint32_t fallThroughMostObjectsBowserAddress = 0x8021DD9C;
#endif

    writeBranchBL(fallThroughMostObjectsStandAddress, asmFallThroughMostObjectsStandard);
    writeBranchBL(fallThroughMostObjectsTubeAddress, asmFallThroughMostObjectsStandard);
    writeBranchBL(fallThroughMostObjectsBowserAddress, asmFallThroughMostObjectsBowser);

    // Auto Mash Through Text
#ifdef TTYD_US
    constexpr uint32_t autoMashThroughTextAddress1 = 0x80080FCC;
    constexpr uint32_t autoMashThroughTextAddress2 = 0x80080FF0;
    constexpr uint32_t autoMashThroughTextAddress3 = 0x80084268;
#elif defined TTYD_JP
    constexpr uint32_t autoMashThroughTextAddress1 = 0x8008047C;
    constexpr uint32_t autoMashThroughTextAddress2 = 0x800804A0;
    constexpr uint32_t autoMashThroughTextAddress3 = 0x80083390;
#elif defined TTYD_EU
    constexpr uint32_t autoMashThroughTextAddress1 = 0x80082288;
    constexpr uint32_t autoMashThroughTextAddress2 = 0x800822AC;
    constexpr uint32_t autoMashThroughTextAddress3 = 0x800855BC;
#endif

    writeBranchBL(autoMashThroughTextAddress1, autoMashText);
    writeBranchBL(autoMashThroughTextAddress2, autoMashText);
    writeBranchBL(autoMashThroughTextAddress3, autoMashText);

    // D-Pad Options GUI
#ifdef TTYD_US
    constexpr uint32_t disableDPadOptionsAddress = 0x8013D148;
#elif defined TTYD_JP
    constexpr uint32_t disableDPadOptionsAddress = 0x80137C1C;
#elif defined TTYD_EU
    constexpr uint32_t disableDPadOptionsAddress = 0x8013EC30;
#endif

    writeBranchBL(disableDPadOptionsAddress, asmDisableDPadOptionsDisplay);

#ifdef TTYD_EU
    // Allow jumping on water when the Bridge Skip display is enabled
    constexpr uint32_t jumpOnWaterAddress = 0x80093CF0;
    writeStandardBranches(jumpOnWaterAddress, asmJumpOnWaterStart, asmJumpOnWaterBranchBack);
#endif
}
