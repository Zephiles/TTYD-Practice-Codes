#include "menuUtils.h"
#include "drawText.h"
#include "cheats.h"
#include "mod.h"
#include "cxx.h"
#include "classes/window.h"
#include "menus/warpsMenu.h"
#include "menus/rootMenu.h"
#include "menus/cheatsMenu.h"
#include "ttyd/camdrv.h"
#include "ttyd/item_data.h"
#include "ttyd/mario_pouch.h"
#include "ttyd/swdrv.h"
#include "ttyd/event.h"
#include "ttyd/mario_motion.h"
#include "ttyd/mario_party.h"
#include "ttyd/mario.h"
#include "ttyd/evt_pouch.h"
#include "ttyd/party.h"

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cinttypes>

static void draw(CameraId cameraId, void *user);

static void selectedOptionSelectEvent(Menu *menuPtr);
static void selectedOptionKeepInventory(Menu *menuPtr);
static void selectedOptionEquipBadges(Menu *menuPtr);
static void selectedOptionSetFlags(Menu *menuPtr);
static void selectedOptionWarp(Menu *menuPtr);

static uint32_t getTotalStageEvents();
static bool indexToStageAndEvent(uint32_t index, int32_t stageEventIdsOut[2]);
static bool checkForValidStageAndEvent(int32_t stageId, int32_t eventId);
static uint32_t getSequenceForEvent(int32_t stageId, int32_t eventId);
static bool getEventDetails(uint32_t index, WarpByEventDetails *warpByEventDetailsPtr);

static const MenuOption gOptions[] {
    "Select Event",
    selectedOptionSelectEvent,

    "Keep Inventory",
    selectedOptionKeepInventory,

    "Equip Badges",
    selectedOptionEquipBadges,

    "Set Flags",
    selectedOptionSetFlags,

    "Warp",
    selectedOptionWarp,
};

static const MenuFunctions gFuncs = {
    gOptions,
    warpsMenuEventIndexControls,
    draw,
    nullptr, // Exit function not needed
};

void warpsMenuEventInit(Menu *menuPtr)
{
    (void)menuPtr;

    constexpr uint32_t totalOptions = sizeof(gOptions) / sizeof(MenuOption);
    enterNextMenu(&gFuncs, totalOptions);
}

static void drawSelectEventWarpInfo(float offsetY)
{
    // Get the text position for the top-left of the window
    const Window *rootWindowPtr = gRootWindow;
    constexpr float scale = MENU_SCALE;

    float tempPosX;
    float tempPosY;
    rootWindowPtr->getTextPosXY(nullptr, WindowAlignment::TOP_LEFT, scale, &tempPosX, &tempPosY);

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    float posX = tempPosX;
    float posY = tempPosY;

    // Draw the help text
    const char *helpText = "Note: Warping via this menu will clear all game states.";
    drawText(helpText, posX, posY, scale, getColorWhite(0xFF));

    // Get the width that is being used by the main text options
    const float textWidth = getTextWidth("Keep Inventory", scale);

    // Set the text to be a bit to the right of the main text
    posX += textWidth + (30.f * scale);

    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    posY -= offsetY + lineDecrement;

    // Draw the flag for whether the inventory should be kept or not
    Mod *modPtr = gMod;
    const char *yesNoText;
    uint32_t color;

    const bool shouldKeepInventory = modPtr->flagIsSet(ModFlag::MOD_FLAG_WARP_BY_EVENT_KEEP_INVENTORY);
    getYesNoTextAndColor(shouldKeepInventory, &yesNoText, &color, 0xFF);
    drawText(yesNoText, posX, posY, scale, color);
    posY -= lineDecrement;

    // Draw the flag for whether the equiped badges should be restored or not
    const bool shouldEquipBadges = modPtr->flagIsSet(ModFlag::MOD_FLAG_WARP_BY_EVENT_EQUIP_BADGES);
    getYesNoTextAndColor(shouldEquipBadges, &yesNoText, &color, 0xFF);
    drawText(yesNoText, posX, posY, scale, color);
    posY -= lineDecrement;

    // Draw the flag for whether specific flags should be set or not when warping to the event
    const bool shouldSetFlags = modPtr->flagIsSet(ModFlag::MOD_FLAG_WARP_BY_EVENT_SET_FLAGS);
    getYesNoTextAndColor(shouldSetFlags, &yesNoText, &color, 0xFF);
    drawText(yesNoText, posX, posY, scale, color);

    // Get the text position for the top-left of the window two lines under the main text
    const uint32_t totalOptions = gMenu->getTotalOptions();
    rootWindowPtr->getTextPosXYUnderMainText(nullptr, WindowAlignment::TOP_LEFT, totalOptions, 4, scale, &tempPosX, &tempPosY);

    // Draw the details for the current event
    drawEventDetails(modPtr->getWarpByEventPtr()->getIndex(), tempPosX, tempPosY);
}

static void draw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    // Help text will be drawn at the top-left of the window, so draw the main text two lines under it
    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * MENU_SCALE;
    constexpr float offsetY = lineDecrement * 2.f;

    basicMenuLayoutDraw(cameraId, user, LINE_HEIGHT_FLOAT, 0.f, offsetY);

    // Draw the info for selecting a warp
    drawSelectEventWarpInfo(offsetY);

    // Draw the value editor if applicable
    WarpsMenu *warpsMenuPtr = gWarpsMenu;
    ValueEditor *valueEditorPtr = warpsMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->draw();
    }

    // Draw an error message if applicable
    ErrorWindow *errorWindowPtr = warpsMenuPtr->getErrorWindowPtr();
    if (errorWindowPtr->shouldDraw())
    {
        errorWindowPtr->draw();
    }
}

static void selectEvent(const ValueType *valuePtr)
{
    gMod->getWarpByEventPtr()->setIndex(valuePtr->u32);

    // Close the value editor
    warpsMenuCloseValueEditor();
}

static void selectedOptionSelectEvent(Menu *menuPtr)
{
    (void)menuPtr;

    // Initialize the value editor
    WarpsMenu *warpsMenuPtr = gWarpsMenu;
    ValueEditor *valueEditorPtr = warpsMenuPtr->getValueEditorPtr();

    uint32_t flags = 0;
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Y_SET_MAX);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Z_SET_MIN);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_WARP_BY_EVENT_DETAILS);

    const uint32_t currentValue = gMod->getWarpByEventPtr()->getIndex();
    const uint32_t maxValue = getTotalStageEvents() - 1;
    constexpr uint32_t minValue = 0;
    const Window *rootWindowPtr = gRootWindow;

    valueEditorPtr
        ->init(&currentValue, &minValue, &maxValue, rootWindowPtr, flags, VariableType::u16, rootWindowPtr->getAlpha());

    valueEditorPtr->startDrawing(selectEvent, warpsMenuCloseValueEditor);
}

static void selectedOptionKeepInventory(Menu *menuPtr)
{
    (void)menuPtr;

    Mod *modPtr = gMod;
    if (!modPtr->toggleFlag(ModFlag::MOD_FLAG_WARP_BY_EVENT_KEEP_INVENTORY))
    {
        // MOD_FLAG_WARP_BY_EVENT_KEEP_INVENTORY is now off, so turn off MOD_FLAG_WARP_BY_EVENT_EQUIP_BADGES
        modPtr->clearFlag(ModFlag::MOD_FLAG_WARP_BY_EVENT_EQUIP_BADGES);
    }
}

static void selectedOptionEquipBadges(Menu *menuPtr)
{
    (void)menuPtr;

    // If MOD_FLAG_WARP_BY_EVENT_KEEP_INVENTORY is not set, then do nothing
    Mod *modPtr = gMod;
    if (!modPtr->flagIsSet(ModFlag::MOD_FLAG_WARP_BY_EVENT_KEEP_INVENTORY))
    {
        return;
    }

    modPtr->toggleFlag(ModFlag::MOD_FLAG_WARP_BY_EVENT_EQUIP_BADGES);
}

static void selectedOptionSetFlags(Menu *menuPtr)
{
    (void)menuPtr;

    gMod->toggleFlag(ModFlag::MOD_FLAG_WARP_BY_EVENT_SET_FLAGS);
}

static uint32_t warpToMapByEvent(uint32_t index)
{
    // Make sure a file is currently loaded and the player is not currently transitioning screens nor in a battle
    if (!checkIfInGame())
    {
        return WarpsMenuWarpToMapReturnValue::WARPS_MENU_WARP_TO_MAP_NOT_IN_GAME;
    }

    // Make sure the index is valid
    if (index >= getTotalStageEvents())
    {
        return WarpsMenuWarpToMapReturnValue::WARPS_MENU_WARP_TO_MAP_INVALID_INDEX;
    }

    WarpByEventDetails warpByEventDetails;
    if (!getEventDetails(index, &warpByEventDetails))
    {
        return WarpsMenuWarpToMapReturnValue::WARPS_MENU_WARP_TO_MAP_FATAL_ERROR;
    }

    // Initialize warping by event
    gMod->setFlag(ModFlag::MOD_FLAG_WARP_BY_EVENT_INIT);

    // Set the new map and bero
    // Make sure the new map and bero are valid
    const char *newMap = warpByEventDetails.mapPtr;
    if (newMap && (newMap[0] != '\0'))
    {
        setNextMap(newMap);
    }

    const char *newBero = warpByEventDetails.beroPtr;
    if (newBero)
    {
        setNextBero(newBero);
    }
    else
    {
        setNextBero("");
    }

    // Warp to the event
    reloadRoomMain();

    return WarpsMenuWarpToMapReturnValue::WARPS_MENU_WARP_TO_MAP_SUCCESS;
}

static void selectedOptionWarp(Menu *menuPtr)
{
    (void)menuPtr;

    switch (warpToMapByEvent(gMod->getWarpByEventPtr()->getIndex()))
    {
        case WarpsMenuWarpToMapReturnValue::WARPS_MENU_WARP_TO_MAP_SUCCESS:
        {
            // Warped successfully, so close the menu
            return closeAllMenus();
        }
        case WarpsMenuWarpToMapReturnValue::WARPS_MENU_WARP_TO_MAP_NOT_IN_GAME:
        {
            gWarpsMenu->initErrorWindow(gWarpsMenuCannotWarpText);
            break;
        }
        default:
        {
            break;
        }
    }
}

void handleWarpByEvent()
{
    // Make sure Mario's and the partners' stats are properly updated when entering a battle
    Mod *modPtr = gMod;
    modPtr->setFlag(ModFlag::MOD_FLAG_CLEAR_MARIO_STATS);
    modPtr->setFlag(ModFlag::MOD_FLAG_CLEAR_PARTNER_STATS);

    const WarpByEvent *warpByEventPtr = modPtr->getWarpByEventPtr();
    const uint32_t currentIndex = warpByEventPtr->getIndex();
    int32_t stageEventIds[2];

    if (!indexToStageAndEvent(currentIndex, stageEventIds))
    {
        return;
    }

    const int32_t stageId = stageEventIds[0];
    const int32_t eventId = stageEventIds[1];

    if (!checkForValidStageAndEvent(stageId, eventId))
    {
        return;
    }

    // Back up the inventory if desired
    WarpByEventInventory *warpByEventInventoryPtr = nullptr;
    ItemId *standardInventoryPtr = nullptr;
    ItemId *badgesInventoryPtr = nullptr;
    ItemId *equippedBadgesInventoryPtr = nullptr;

    constexpr uint32_t standardInventorySize = sizeof(PouchData::items) / sizeof(ItemId);
    constexpr uint32_t badgesInventorySize = sizeof(PouchData::badges) / sizeof(ItemId);

    const bool shouldKeepInventory = modPtr->flagIsSet(ModFlag::MOD_FLAG_WARP_BY_EVENT_KEEP_INVENTORY);
    const bool shouldEquipBadges = modPtr->flagIsSet(ModFlag::MOD_FLAG_WARP_BY_EVENT_EQUIP_BADGES);
    if (shouldKeepInventory)
    {
        warpByEventInventoryPtr = new WarpByEventInventory;
        PouchData *pouchPtr = pouchGetPtr();

        standardInventoryPtr = pouchPtr->items;
        badgesInventoryPtr = pouchPtr->badges;
        equippedBadgesInventoryPtr = pouchPtr->equippedBadges;

        // Back up the standard inventory
        memcpy(warpByEventInventoryPtr->items, standardInventoryPtr, sizeof(warpByEventInventoryPtr->items));

        // Back up the badges
        memcpy(warpByEventInventoryPtr->badges, badgesInventoryPtr, sizeof(warpByEventInventoryPtr->badges));

        if (shouldEquipBadges)
        {
            // Back up the equipped badges
            memcpy(warpByEventInventoryPtr->equippedBadges,
                   equippedBadgesInventoryPtr,
                   sizeof(warpByEventInventoryPtr->equippedBadges));
        }
    }

    // Clear all current states
    swInit();

    // Run the init functions for each event, up to and including the current one
    for (int32_t i = 0; i <= stageId; i++)
    {
        const EventStageDescription *stage = eventStgDtPtr(i);
        const EventStageEventDescription *targetEvent = &stage->pEvents[0];
        const int32_t lastEvent = (i == stageId ? eventId : (stage->eventCount - 1));

        for (int32_t e = 0; e <= lastEvent; e++)
        {
            void (*initFunction)() = targetEvent->pfnInit;
            if (initFunction)
            {
                initFunction();
            }

            targetEvent++;
        }
    }

    // Restore the standard inventory if desired
    if (shouldKeepInventory)
    {
        auto addItemToInventory = [](ItemId *inventoryPtr, uint32_t totalSlots, ItemId item)
        {
            for (uint32_t i = 0; i < totalSlots; i++)
            {
                if (inventoryPtr[i] == ItemId::ITEM_NONE)
                {
                    inventoryPtr[i] = item;
                    break;
                }
            }
        };

        // Restore the standard inventory
        memcpy(standardInventoryPtr, warpByEventInventoryPtr->items, sizeof(warpByEventInventoryPtr->items));

        // Restore the badges
        memcpy(badgesInventoryPtr, warpByEventInventoryPtr->badges, sizeof(warpByEventInventoryPtr->badges));

        if (shouldEquipBadges)
        {
            // Restore the equipped badges
            memcpy(equippedBadgesInventoryPtr,
                   warpByEventInventoryPtr->equippedBadges,
                   sizeof(warpByEventInventoryPtr->equippedBadges));
        }

        delete warpByEventInventoryPtr;
        warpByEventInventoryPtr = nullptr;

        // Add any items or badges that the player should have gotten

        // If the player warped to an event that is 58 or higher, then they should be givenAttack FX R, Power Bounce,
        // Multibounce, and Power Smash. Attack FX R should also be equipped if going to fight Hooktail.
        if (currentIndex >= 58)
        {
            // Set up the badges to give
            static constexpr const ItemId badgesToGive[] = {
                ItemId::ITEM_ATTACK_FX_R,
                ItemId::ITEM_POWER_BOUNCE,
                ItemId::ITEM_MULTIBOUNCE,
                ItemId::ITEM_POWER_SMASH,
            };

            // Give the badges
            // Only give the badges if the player does not have them already
            constexpr uint32_t loopCount = sizeof(badgesToGive) / sizeof(badgesToGive[0]);
            for (uint32_t i = 0; i < loopCount; i++)
            {
                const ItemId currentBadge = badgesToGive[i];
                if (pouchCheckItem(currentBadge) == 0)
                {
                    addItemToInventory(badgesInventoryPtr, badgesInventorySize, currentBadge);
                }
            }

            // Unequip Attack FX R if it's equipped, as the new badge order may cause it to break
            N_pouchUnEquipBadgeID(ItemId::ITEM_ATTACK_FX_R);

            // Re-equip Attack FX R if currently going to fight Hooktail
            if (currentIndex == 58)
            {
                pouchEquipBadgeID(ItemId::ITEM_ATTACK_FX_R);
            }
        }

        // If the player warped to the event with the index of 278, then they should be given a Coconut, as it is used to get
        // the Chuckola Cola from Flavio
        if (currentIndex == 278)
        {
            // Only give the Coconut if the player doesn't have one already
            if (pouchCheckItem(ItemId::ITEM_COCONUT) == 0)
            {
                uint32_t totalSlots = standardInventorySize / 2;
                if (pouchCheckItem(ItemId::ITEM_STRANGE_SACK) > 0)
                {
                    totalSlots = standardInventorySize;
                }

                addItemToInventory(standardInventoryPtr, totalSlots, ItemId::ITEM_COCONUT);
            }
        };
    }

    // Set specific flags if desired
    if (modPtr->flagIsSet(ModFlag::MOD_FLAG_WARP_BY_EVENT_SET_FLAGS))
    {
        // Set up the flags to set
        static constexpr const uint16_t flagsToSet[] = {
            0,   // Shop tutorial
            37,  // Got first email
            38,  // Read any email
            233, // Save block tutorial
            234, // Recovery block tutorial
            235, // Item tutorial
        };

        // Set the flags
        constexpr uint32_t loopCount = sizeof(flagsToSet) / sizeof(flagsToSet[0]);
        for (uint32_t i = 0; i < loopCount; i++)
        {
            swSet(flagsToSet[i]);
        }
    }

    // Set the Sequence to the value for the current event
    setSequencePosition(getSequenceForEvent(stageId, eventId));

    // Reset Mario's motion
    marioChgMot(MarioMotion::kStay);

    // Set Mario's motion
    const EventStageEventDescription *targetEvent = &eventStgDtPtr(stageId)->pEvents[eventId];
    switch (targetEvent->entryMotionType)
    {
        case 0:
        {
            marioChgStayMotion();
            break;
        }
        case 1:
        {
            marioChgShipMotion();
            break;
        }
        default:
        {
            break;
        }
    }

    // Remove any partners and followers that are currently out from the overworld
    marioPartyKill();

    // Reset the previous partner and follower ids
    Player *playerPtr = marioGetPtr();
    playerPtr->prevPartnerId = PartyMembers::kNone;
    playerPtr->prevFollowerId = PartyMembers::kNone;

    // Spawn the partner for the current event
    const PartyMembers partnerId = targetEvent->partnerId;
    if (partnerId != PartyMembers::kNone)
    {
        spawnPartnerOrFollower(partnerId);
    }

    // Spawn the follower for the current event
    const PartyMembers followerId = targetEvent->followerId;
    if (followerId != PartyMembers::kNone)
    {
        spawnPartnerOrFollower(followerId);
    }

    // Perform a full recovery for Mario and the partners
    pouchReviseMarioParam();
    evt_pouch_mario_recovery(nullptr, false); // Parameters are unused for this function

    // Must call pouchRevisePartyParam to properly set each partner's stats, otherwise they will each have a maximum of 10 HP
    pouchRevisePartyParam();
    evt_pouch_all_party_recovery(nullptr, false); // Parameters are unused for this function
}

static uint32_t getTotalStageEvents()
{
    const int32_t totalStages = eventStgNum();
    int32_t totalEvents = 0;

    for (int32_t i = 0; i < totalStages; i++)
    {
        const EventStageDescription *stage = eventStgDtPtr(i);
        totalEvents += stage->eventCount;
    }

    // Failsafe: Make sure the total does not exceed the size of the JP string array
    if (totalEvents > WARPS_TOTAL_EVENT_NAMES)
    {
        totalEvents = WARPS_TOTAL_EVENT_NAMES;
    }

    return static_cast<uint32_t>(totalEvents);
}

static bool indexToStageAndEvent(uint32_t index, int32_t stageEventIdsOut[2])
{
    if (index >= getTotalStageEvents())
    {
        return false;
    }

    const int32_t totalStages = eventStgNum();
    int32_t stageStartIndex = 0;

    for (int32_t i = 0; i < totalStages; i++)
    {
        const EventStageDescription *targetStage = eventStgDtPtr(i);
        const int32_t stageEventCount = targetStage->eventCount;

        if ((static_cast<int32_t>(index) >= stageStartIndex) &&
            (static_cast<int32_t>(index) < (stageStartIndex + stageEventCount)))
        {
            stageEventIdsOut[0] = i;                       // Stage Id
            stageEventIdsOut[1] = index - stageStartIndex; // Event Id
            return true;
        }

        stageStartIndex += stageEventCount;
    }

    return false;
}

static bool checkForValidStageAndEvent(int32_t stageId, int32_t eventId)
{
    if ((stageId < 0) || (stageId >= eventStgNum()))
    {
        return false;
    }

    const EventStageDescription *targetStage = eventStgDtPtr(stageId);
    if ((eventId < 0) || (eventId >= targetStage->eventCount))
    {
        return false;
    }

    return true;
}

static uint32_t getSequenceForEvent(int32_t stageId, int32_t eventId)
{
    if ((stageId <= 0) && (eventId <= 0))
    {
        return 0;
    }

    if (!checkForValidStageAndEvent(stageId, eventId))
    {
        return 0;
    }

    const EventStageDescription *currentStage = eventStgDtPtr(stageId);
    const uint32_t currentSequencePosition = currentStage->pEvents[eventId].sequencePosition;
    uint32_t newSequencePosition;

    while (1)
    {
        if (eventId <= 0)
        {
            const EventStageDescription *newStage = eventStgDtPtr(--stageId);

            eventId = newStage->eventCount - 1;
            newSequencePosition = newStage->pEvents[eventId].sequencePosition;
        }
        else
        {
            const EventStageDescription *newStage = eventStgDtPtr(stageId);
            newSequencePosition = newStage->pEvents[--eventId].sequencePosition;
        }

        if (newSequencePosition < currentSequencePosition)
        {
            return newSequencePosition;
        }

        if ((stageId <= 0) && (eventId <= 0))
        {
            return 0;
        }
    }
}

static const char *getPartyName(PartyMembers id)
{
    switch (id)
    {
        case PartyMembers::kNone:
        {
            return "None";
        }
        case PartyMembers::kGoombella:
        {
            return "Goombella";
        }
        case PartyMembers::kKoops:
        {
            return "Koops";
        }
        case PartyMembers::kBobbery:
        {
            return "Bobbery";
        }
        case PartyMembers::kYoshi:
        {
            return "Yoshi";
        }
        case PartyMembers::kFlurrie:
        {
            return "Flurrie";
        }
        case PartyMembers::kVivian:
        {
            return "Vivian";
        }
        case PartyMembers::kMsMowz:
        {
            return "Ms. Mowz";
        }
        case PartyMembers::kEgg:
        {
            return "Egg";
        }
        case PartyMembers::kFlavio:
        {
            return "Flavio";
        }
        case PartyMembers::kPunio:
        {
            return "Punio";
        }
        case PartyMembers::kFrankly:
        {
            return "Frankly";
        }
        case PartyMembers::kCraw:
        {
            return "Craw";
        }
        case PartyMembers::kGoombellaFollower:
        {
            return "Goombella (Follower)";
        }
        case PartyMembers::kKoopsFollower:
        {
            return "Koops (Follower)";
        }
        case PartyMembers::kBobberyFollower:
        {
            return "Bobbery (Follower)";
        }
        case PartyMembers::kYoshiFollower:
        {
            return "Yoshi (Follower)";
        }
        case PartyMembers::kFlurrieFollower:
        {
            return "Flurrie (Follower)";
        }
        case PartyMembers::kVivianFollower:
        {
            return "Vivian (Follower)";
        }
        case PartyMembers::kMsMowzFollower:
        {
            return "Ms. Mowz (Follower)";
        }
        default:
        {
            return "Invalid";
        }
    }
}

void getMapAndBeroTextAndColor(const char *mapTextPtr, const char *beroTextPtr, MapAndBeroDetails *mapAndBeroDetailsPtr)
{
    // Get the map values
    if (mapTextPtr && (mapTextPtr[0] != '\0'))
    {
        mapAndBeroDetailsPtr->mapTextPtr = mapTextPtr;
        mapAndBeroDetailsPtr->mapColor = getColorWhite(0xFF);
    }
    else
    {
        mapAndBeroDetailsPtr->mapTextPtr = "None";
        mapAndBeroDetailsPtr->mapColor = getColorGrayedOut(0xFF);
    }

    // Get the bero values
    if (beroTextPtr && (beroTextPtr[0] != '\0'))
    {
        mapAndBeroDetailsPtr->beroTextPtr = beroTextPtr;
        mapAndBeroDetailsPtr->beroColor = getColorWhite(0xFF);
    }
    else
    {
        mapAndBeroDetailsPtr->beroTextPtr = "None";
        mapAndBeroDetailsPtr->beroColor = getColorGrayedOut(0xFF);
    }
}

static bool getEventDetails(uint32_t index, WarpByEventDetails *warpByEventDetailsPtr)
{
    int32_t stageEventIds[2];
    if (!indexToStageAndEvent(index, stageEventIds))
    {
        return false;
    }

    const int32_t stageId = stageEventIds[0];
    const int32_t eventId = stageEventIds[1];
    if (!checkForValidStageAndEvent(stageId, eventId))
    {
        return false;
    }

    const uint32_t eventSequencePosition = getSequenceForEvent(stageId, eventId);
    const EventStageDescription *targetStage = eventStgDtPtr(stageId);
    const EventStageEventDescription *targetEvent = &targetStage->pEvents[eventId];

#ifdef TTYD_JP
    char *stageNameBuffer = warpByEventDetailsPtr->stage;
    if (!getStageString(stageNameBuffer, sizeof(warpByEventDetailsPtr->stage), eventSequencePosition))
    {
        stageNameBuffer[0] = '\0';
    }

    if (index < WARPS_TOTAL_EVENT_NAMES)
    {
        warpByEventDetailsPtr->eventPtr = gWarpsEventNames[index];
    }
    else
    {
        warpByEventDetailsPtr->eventPtr = "";
    }
#else
    warpByEventDetailsPtr->stagePtr = targetStage->nameEn;
    warpByEventDetailsPtr->eventPtr = targetEvent->nameEn;
#endif

    warpByEventDetailsPtr->mapPtr = targetEvent->map;
    warpByEventDetailsPtr->beroPtr = targetEvent->bero;
    warpByEventDetailsPtr->sequencePosition = eventSequencePosition;
    warpByEventDetailsPtr->partnerPtr = getPartyName(targetEvent->partnerId);
    warpByEventDetailsPtr->followerPtr = getPartyName(targetEvent->followerId);

    return true;
}

// This function assumes that text drawing has been initialized already
void drawEventDetails(uint32_t index, float posX, float posY)
{
    // Draw the labels for the details for the current event
    const char *labels = "Stage\nEvent\nSequence\nPartner\nFollower\nMap\nLZ";
    constexpr float scale = MENU_SCALE;
    drawText(labels, posX, posY, scale, getColorWhite(0xFF));

    // Get the values for the details for the current event
    WarpByEventDetails warpByEventDetails;
    if (!getEventDetails(index, &warpByEventDetails))
    {
        return;
    }

    // Get the text and color for the partner
    const char *partnerString = warpByEventDetails.partnerPtr;
    uint32_t partnerColor;

    if (strcmp(partnerString, "None") != 0)
    {
        partnerColor = getColorWhite(0xFF);
    }
    else
    {
        partnerColor = getColorGrayedOut(0xFF);
    }

    // Get the text and color for the follower
    const char *followerString = warpByEventDetails.followerPtr;
    uint32_t followerColor;

    if (strcmp(followerString, "None") != 0)
    {
        followerColor = getColorWhite(0xFF);
    }
    else
    {
        followerColor = getColorGrayedOut(0xFF);
    }

    // Get the text and color for the map and bero
    MapAndBeroDetails mapAndBeroDetails;
    getMapAndBeroTextAndColor(warpByEventDetails.mapPtr, warpByEventDetails.beroPtr, &mapAndBeroDetails);

    // Set up the string for the values for the details for the current event
    char buf[160];
    snprintf(buf,
             sizeof(buf),
             "%s\n%s\n%" PRIu32 "\n<col %" PRIx32 ">%s\n<col %" PRIx32 ">%s\n<col %" PRIx32 ">%s\n<col %" PRIx32 ">%s",
             warpByEventDetails.getStagePtr(),
             warpByEventDetails.eventPtr,
             warpByEventDetails.sequencePosition,
             partnerColor,
             partnerString,
             followerColor,
             followerString,
             mapAndBeroDetails.mapColor,
             mapAndBeroDetails.mapTextPtr,
             mapAndBeroDetails.beroColor,
             mapAndBeroDetails.beroTextPtr);

    // Get the width that is being used by the labels
    const float textWidth = getTextWidth("Sequence", scale);

    // Set the text to be a bit to the right of the labels
    posX += textWidth + (30.f * scale);

    // Draw the values for the details for the current event
    drawText(buf, posX, posY, scale, getColorWhite(0xFF));
}
