#include "mod.h"
#include "gamePatches.h"
#include "assembly.h"
#include "patch.h"
#include "gc/types.h"
#include "gc/pad.h"
#include "gc/OSCache.h"
#include "ttyd/mario_pouch.h"
#include "ttyd/memory.h"
#include "ttyd/seq_mapchange.h"
#include "ttyd/seqdrv.h"
#include "ttyd/fontmgr.h"
#include "ttyd/mario.h"
#include "ttyd/seq_title.h"
#include "misc/utils.h"
#include "misc/functionHooks.h"

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cinttypes>

void *cFixPouchInitMemoryLeak(OSHeapHandle heapHandle, uint32_t size)
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
        return __memAlloc(heapHandle, size);
    }
}

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

void *cFixEvtMapBlendSetFlagPartnerCrash(void *partnerPtr)
{
    // Bring out a partner if no partner is currently out
    if (partnerPtr)
    {
        return partnerPtr;
    }

    spawnFailsafePartnerOrFollower(true);
    return getPartnerPtr();
}

void *cFixEvtMapBlendSetFlagFollowerCrash(void *followerPtr)
{
    // Bring out a follower if no follower is currently out
    if (followerPtr)
    {
        return followerPtr;
    }

    spawnFailsafePartnerOrFollower(false);
    return getFollowerPtr();
}

void preventDiaryTextboxOptionSelection(const char *currentText, int32_t *storeAddress, int32_t selectedOption)
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
        if (strcmp(NextArea, "rsh") != 0)
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

bool displayMegaBadgesInBattleMenu(bool checkJumpBadge, uint32_t bitfield)
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
int32_t crashScreenDecrementYPos()
{
    static int32_t decrementCount = 0;
    int32_t tempDecrementCount = decrementCount;

    constexpr int32_t kPosYIncrementAmount = 50;
    int32_t textPosY = tempDecrementCount + kPosYIncrementAmount;
    tempDecrementCount--;

    if (tempDecrementCount < -960)
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

const char *getCustomMessage(const char *key)
{
    const char *nextAreaPtr = NextArea;

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
    constexpr uint32_t openMenuCombo = PadInput::PAD_L | PadInput::PAD_START;

    if (checkButtonComboEveryFrame(openMenuCombo))
    {
        // Prevent being able to close the menu if L + Start is being held
        return 0;
    }

    // Call the original function
    return g_winRootMain_trampoline(pauseMenuPtr);
}

int32_t fixMarioKeyOn()
{
    Player *player = marioGetPtr();
    const int32_t key = player->wKey;

    // Make sure the value is greater than 0
    if (key < 1)
    {
        player->wKey = 1;
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
    if (modPtr->shouldClearMarioStatsCache())
    {
        modPtr->resetMarioStatsCacheBool();

        const uint32_t size = (&pouchPtr->starPoints - &pouchPtr->currentHp) * sizeof(int16_t);
        DCFlushRange(&pouchPtr->currentHp, size + sizeof(int16_t));
    }

    // Clear the cache for the partners' stats if they were changed manually
    if (modPtr->shouldClearPartnerStatsCache())
    {
        modPtr->resetPartnerStatsCacheBool();
        DCFlushRange(pouchPtr->partyData, sizeof(pouchPtr->partyData));
    }

    // Call the original function
    return g_battle_init_trampoline();
}

void applyGameFixes()
{
    // Fix a memory leak from occuring when starting a new file. The leak is that memory for the inventory is re-allocated
    // without checking if it's already allocated.
#ifdef TTYD_US
    uint32_t pouchInitMemoryLeakAddress = 0x800D59DC;
#elif defined TTYD_JP
    uint32_t pouchInitMemoryLeakAddress = 0x800D17A8;
#elif defined TTYD_EU
    uint32_t pouchInitMemoryLeakAddress = 0x800D67E4;
#endif

    writeBranchBL(pouchInitMemoryLeakAddress, cFixPouchInitMemoryLeak);

    // Fix the game not properly allocating enough memory for text in textboxes. The vanilla code does not allocate memory for
    // the null terminator.
#ifdef TTYD_US
    uint32_t msgWindowMrAddress = 0x800816F4;
#elif defined TTYD_JP
    uint32_t msgWindowMrAddress = 0x80080B6C;
#elif defined TTYD_EU
    uint32_t msgWindowMrAddress = 0x800829B0;
#endif

    applyAssemblyPatch(msgWindowMrAddress, 0x38830001); // addi r4,r3,1

    // Fix a crash that can occur when defeating Blooper with an attack that causes both the body and the left tentacle to be
    // defeated at roughly the same time
#ifdef TTYD_US
    uint32_t blooperCrash1Address = 0x8010F810;
    uint32_t blooperCrash2Address = 0x8010F87C;
#elif defined TTYD_JP
    uint32_t blooperCrash1Address = 0x8010A724;
    uint32_t blooperCrash2Address = 0x8010A790;
#elif defined TTYD_EU
    uint32_t blooperCrash1Address = 0x801106E8;
    uint32_t blooperCrash2Address = 0x80110754;
#endif

    writeBranchBL(blooperCrash1Address, asmFixBlooperCrash1);
    writeBranchBL(blooperCrash2Address, asmFixBlooperCrash2);

    // Fix crashes that can occur when evt_map_blend_set_flag is trying to apply effects to partners/followers when one is not
    // out
#ifdef TTYD_US
    uint32_t evtMapBlendSetFlagPartnerCrashAddress = 0x800389C4;
    uint32_t evtMapBlendSetFlagFollowerCrashAddress = 0x80038A0C;
#elif defined TTYD_JP
    uint32_t evtMapBlendSetFlagPartnerCrashAddress = 0x80038328;
    uint32_t evtMapBlendSetFlagFollowerCrashAddress = 0x80038370;
#elif defined TTYD_EU
    uint32_t evtMapBlendSetFlagPartnerCrashAddress = 0x80038AAC;
    uint32_t evtMapBlendSetFlagFollowerCrashAddress = 0x80038AF4;
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
    uint32_t debugModeInitialzeAddress = 0x80009B2C;
#elif defined TTYD_JP
    uint32_t debugModeInitialzeAddress = 0x8000999C;
#elif defined TTYD_EU
    uint32_t debugModeInitialzeAddress = 0x80009CF0;
#endif

    applyAssemblyPatch(debugModeInitialzeAddress, 0x3800FFFF); // li r0,-1

    // Always show the build date on the title screen
#ifdef TTYD_US
    uint32_t debugModeShowBuildDateAddress = 0x80008FE8;
#elif defined TTYD_JP
    uint32_t debugModeShowBuildDateAddress = 0x80008EB8;
#elif defined TTYD_EU
    uint32_t debugModeShowBuildDateAddress = 0x800091B4;
#endif

    applyAssemblyPatch(debugModeShowBuildDateAddress, 0x60000000); // nop

    // Adjust the size of the text on the crash screen
#ifdef TTYD_US
    float *crasheScreenTextSizeAddress = reinterpret_cast<float *>(0x80428BC0);
#elif defined TTYD_JP
    float *crasheScreenTextSizeAddress = reinterpret_cast<float *>(0x80422618);
#elif defined TTYD_EU
    float *crasheScreenTextSizeAddress = reinterpret_cast<float *>(0x804356C8);
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
    uint32_t crashScreenPPCHaltBranchAddress = 0x8025E4A4;
    uint32_t crashScreenEndBranchAddress = 0x8025E4A8;
#elif defined TTYD_JP
    uint32_t crashScreenPosYValueAddress = 0x802582F8;
#elif defined TTYD_EU
    uint32_t crashScreenPPCHaltBranchAddress = 0x8026207C;
    uint32_t crashScreenEndBranchAddress = 0x80262080;
#endif

#ifdef TTYD_JP
    writeStandardBranches(crashScreenPosYValueAddress, asmCrashScreenDecrementYPosStart, asmCrashScreenDecrementYPosBranchBack);
#else
    applyAssemblyPatch(crashScreenPPCHaltBranchAddress, 0x3B400000); // li r26,0
    applyAssemblyPatch(crashScreenEndBranchAddress, 0x4BFFFDD4);     // b -0x22C
#endif

    // Prevent upgrade item cutscenes from occuring, and instead just give them like standard items. This is needed because the
    // upgrades could potentially be obtained in maps other than where they are intended to be obtained, which will cause a
    // crash since the data for their cutscenes will not be available.
#ifdef TTYD_US
    uint32_t preventUpgradeItemCutscenesAddress = 0x800ABD04;
#elif defined TTYD_JP
    uint32_t preventUpgradeItemCutscenesAddress = 0x800AA048;
#elif defined TTYD_EU
    uint32_t preventUpgradeItemCutscenesAddress = 0x800AD0D4;
#endif

    applyAssemblyPatch(preventUpgradeItemCutscenesAddress, 0x60000000); // nop

    // Prevent the game from crashing if the player tries to read the diary while not on the Excess Express
#ifdef TTYD_US
    uint32_t PreventDiaryTextboxSelectionAddress = 0x800D214C;
#elif defined TTYD_JP
    uint32_t PreventDiaryTextboxSelectionAddress = 0x800CE01C;
#elif defined TTYD_EU
    uint32_t PreventDiaryTextboxSelectionAddress = 0x800D2F44;
#endif

    writeBranchBL(PreventDiaryTextboxSelectionAddress, asmPreventDiaryTextboxSelection);

    // Always show the Mega Jump and Mega Hammer options in the battle menu if those badges are equipped, as normally they are
    // only present if the player has the Ultra Boots or Ultra Hammer respectively
#ifdef TTYD_US
    uint32_t BattleMenuJumpAddress = 0x80122BA4;
    uint32_t BattleMenuHammerAddress = 0x80122BB8;
#elif defined TTYD_JP
    uint32_t BattleMenuJumpAddress = 0x8011D6DC;
    uint32_t BattleMenuHammerAddress = 0x8011D6F0;
#elif defined TTYD_EU
    uint32_t BattleMenuJumpAddress = 0x80123AE4;
    uint32_t BattleMenuHammerAddress = 0x80123AF8;
#endif

    writeBranchBL(BattleMenuJumpAddress, asmDisplayMegaJumpBadgeInBattleMenu);
    writeBranchBL(BattleMenuHammerAddress, asmDisplayMegaHammerBadgesInBattleMenu);

    // Always show the partner menu, even if the sequence is below 7 and the player has no partners
    // Always show the badge menu, even if the sequence is below 20 and the player has no badges
#ifdef TTYD_US
    uint32_t PauseMenuPartnerMenuAddress = 0x801649A0;
    uint32_t PauseMenuBadgeMenuAddress = 0x80164A44;
#elif defined TTYD_JP
    uint32_t PauseMenuPartnerMenuAddress = 0x8015EFBC;
    uint32_t PauseMenuBadgeMenuAddress = 0x8015F060;
#elif defined TTYD_EU
    uint32_t PauseMenuPartnerMenuAddress = 0x80166490;
    uint32_t PauseMenuBadgeMenuAddress = 0x80166534;
#endif

    applyAssemblyPatch(PauseMenuPartnerMenuAddress, 0x60000000); // nop
    applyAssemblyPatch(PauseMenuBadgeMenuAddress, 0x60000000);   // nop

    // Allow FontDrawMessageMtx to handle more custom commands like FontDrawMessage can
#ifdef TTYD_US
    uint32_t FontDrawMessageMtxHandleCommandAddress = 0x800759E0;
#elif defined TTYD_JP
    uint32_t FontDrawMessageMtxHandleCommandAddress = 0x8007471C;
#elif defined TTYD_EU
    uint32_t FontDrawMessageMtxHandleCommandAddress = 0x80076C9C;
#endif

    writeStandardBranches(FontDrawMessageMtxHandleCommandAddress,
                          asmFontDrawMessageMtxHandleCommandStart,
                          asmFontDrawMessageMtxHandleCommandBranchBack);
}
