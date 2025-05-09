#include "mod.h"
#include "menuUtils.h"
#include "drawText.h"
#include "drawIcon.h"
#include "classes/window.h"
#include "menus/battlesMenu.h"
#include "menus/rootMenu.h"
#include "misc/utils.h"
#include "ttyd/battle_unit.h"
#include "ttyd/item_data.h"
#include "ttyd/camdrv.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

static void controls(Menu *menuPtr, MenuButtonInput button);
static void draw(CameraId cameraId, void *user);
static void selectedOptionChangeValue(Menu *menuPtr);
static void selectedOptionClearHeldItem(Menu *menuPtr);

static const char *gStatsStrings[] = {
    "HP",
    "Max HP",
    "FP",
    "Max FP",
};

static const MenuOption gOptions[] = {
    "Change HP",
    selectedOptionChangeValue,

    "Change Max HP",
    selectedOptionChangeValue,

    "Change FP",
    selectedOptionChangeValue,

    "Change Max FP",
    selectedOptionChangeValue,

    "Change Held Item",
    selectedOptionChangeValue,

    "Clear Held Item",
    selectedOptionClearHeldItem,

    "Change Statuses",
    battlesMenuStatusesInit,
};

static const MenuFunctions gFuncs = {
    gOptions,
    controls,
    draw,
    nullptr, // Exit function not needed
};

void battlesMenuStatsInit()
{
    constexpr uint32_t totalOptions = sizeof(gOptions) / sizeof(MenuOption);
    enterNextMenu(&gFuncs, totalOptions);
}

static void controls(Menu *menuPtr, MenuButtonInput button)
{
    // Close the Battles menu if not in a battle
    if (!getBattleWorkPtr())
    {
        rootMenuExitBattlesMenu();
        return;
    }

    // If the pointer to the selected actor is invalid, then assume that the actor is no longer in the battle
    BattlesMenu *battlesMenuPtr = gBattlesMenu;
    if (!getActorBattlePtr(battlesMenuPtr->getCurrentActorIndex()))
    {
        battlesMenuReturnToInitMenu();
        return;
    }

    // If the value editor is open, then handle the controls for that
    ValueEditor *valueEditorPtr = battlesMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->controls(button);
        return;
    }

    // Use the default controls
    basicMenuLayoutControls(menuPtr, button);
}

static void changeValue(const ValueType *valuePtr)
{
    // Make sure the current index is valid
    Menu *menuPtr = gMenu;
    const uint32_t index = menuPtr->getCurrentIndex();

    if (index > StatsBattlesStatsOptions::BATTLES_STATS_ACTOR_CHANGE_HELD_ITEM)
    {
        // Failsafe: Reset the current index to 0
        menuPtr->setCurrentIndex(0);
        return;
    }

    const int32_t value = valuePtr->s32;
    BattleWorkUnit *actorPtr = getActorBattlePtr(gBattlesMenu->getCurrentActorIndex());

    // Set the new value
    switch (index)
    {
        case StatsBattlesStatsOptions::BATTLES_STATS_ACTOR_CURRENT_HP:
        {
            actorPtr->current_hp = static_cast<int16_t>(value);
            break;
        }
        case StatsBattlesStatsOptions::BATTLES_STATS_ACTOR_MAX_HP:
        {
            actorPtr->max_hp = static_cast<int16_t>(value);
            break;
        }
        case StatsBattlesStatsOptions::BATTLES_STATS_ACTOR_CURRENT_FP:
        {
            actorPtr->current_fp = static_cast<int16_t>(value);
            break;
        }
        case StatsBattlesStatsOptions::BATTLES_STATS_ACTOR_MAX_FP:
        {
            actorPtr->max_fp = static_cast<int16_t>(value);
            break;
        }
        case StatsBattlesStatsOptions::BATTLES_STATS_ACTOR_CHANGE_HELD_ITEM:
        {
            actorPtr->held_item = value;

            // If the new item is a badge, then unequip all current badges and equip the new badge
            if (value < static_cast<int32_t>(ItemId::ITEM_POWER_JUMP))
            {
                break;
            }

            // Do not modify for Mario nor the partners
            const BattleUnitType type = actorPtr->current_kind;
            if ((type >= BattleUnitType::kUnitMario) && (type <= BattleUnitType::kUnitMsMowz))
            {
                break;
            }

            // Clear all of the currently equipped badges
            clearMemory(&actorPtr->badges_equipped, sizeof(actorPtr->badges_equipped));

            // Equip the new badge
            _EquipItem(actorPtr, 1, static_cast<ItemId>(value));
            break;
        }
        default:
        {
            break;
        }
    }

    // Close the value editor
    battlesMenuCancelChangeValue();
}

static void selectedOptionChangeValue(Menu *menuPtr)
{
    // Make sure the current index is valid
    const uint32_t index = menuPtr->getCurrentIndex();

    if (index > StatsBattlesStatsOptions::BATTLES_STATS_ACTOR_CHANGE_HELD_ITEM)
    {
        // Failsafe: Reset the current index to 0
        menuPtr->setCurrentIndex(0);
        return;
    }

    BattlesMenu *battlesMenuPtr = gBattlesMenu;
    const BattleWorkUnit *actorPtr = getActorBattlePtr(battlesMenuPtr->getCurrentActorIndex());

    // Get the current, min, and max values
    int32_t currentValue = 0;
    int32_t minValue = 0;
    int32_t maxValue = 999;

    switch (index)
    {
        case StatsBattlesStatsOptions::BATTLES_STATS_ACTOR_CURRENT_HP:
        {
            currentValue = actorPtr->current_hp;
            maxValue = actorPtr->max_hp;
            break;
        }
        case StatsBattlesStatsOptions::BATTLES_STATS_ACTOR_MAX_HP:
        {
            currentValue = actorPtr->max_hp;
            break;
        }
        case StatsBattlesStatsOptions::BATTLES_STATS_ACTOR_CURRENT_FP:
        {
            currentValue = actorPtr->current_fp;
            maxValue = actorPtr->max_fp;
            break;
        }
        case StatsBattlesStatsOptions::BATTLES_STATS_ACTOR_MAX_FP:
        {
            currentValue = actorPtr->max_fp;
            break;
        }
        case StatsBattlesStatsOptions::BATTLES_STATS_ACTOR_CHANGE_HELD_ITEM:
        {
            const int32_t currentHeldItem = actorPtr->held_item;
            constexpr int32_t thunderBoltValue = static_cast<int32_t>(ItemId::ITEM_THUNDER_BOLT);

            if (itemIsValid(static_cast<ItemId>(currentHeldItem)))
            {
                currentValue = currentHeldItem;
            }
            else
            {
                currentValue = thunderBoltValue;
            }

            minValue = thunderBoltValue;
            maxValue = static_cast<int32_t>(ItemId::ITEM_SUPER_CHARGE_P);
            break;
        }
        default:
        {
            break;
        }
    }

    // Initialize the value editor
    ValueEditor *valueEditorPtr = battlesMenuPtr->getValueEditorPtr();
    VariableType type = VariableType::s16;

    uint32_t flags = 0;
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Y_SET_MAX);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Z_SET_MIN);

    if (index == StatsBattlesStatsOptions::BATTLES_STATS_ACTOR_CHANGE_HELD_ITEM)
    {
        type = VariableType::s32;
        flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::BATTLES_CHANGE_HELD_ITEM);
        flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_ITEM_ICON_AND_TEXT);
    }

    const Window *rootWindowPtr = gRootWindow;
    valueEditorPtr->init(&currentValue, &minValue, &maxValue, rootWindowPtr, flags, type, rootWindowPtr->getAlpha());
    valueEditorPtr->startDrawing(changeValue, battlesMenuCancelChangeValue);
}

static void selectedOptionClearHeldItem(Menu *menuPtr)
{
    (void)menuPtr;

    BattleWorkUnit *actorPtr = getActorBattlePtr(gBattlesMenu->getCurrentActorIndex());
    actorPtr->held_item = static_cast<int32_t>(ItemId::ITEM_NONE);

    // If the actor is neither Mario nor any partners, then clear all equipped badges
    const BattleUnitType type = actorPtr->current_kind;
    if ((type >= BattleUnitType::kUnitMario) && (type <= BattleUnitType::kUnitMsMowz))
    {
        return;
    }

    clearMemory(&actorPtr->badges_equipped, sizeof(actorPtr->badges_equipped));
}

static void drawBattleActorStats(BattleWorkUnit *actorPtr)
{
    // Failsafe: Make sure current_kind is valid for the selected actor
    const BattleUnitType type = actorPtr->current_kind;
    if (!battleUnitTypeIsValid(type))
    {
        return;
    }

    // Get the text position for the top-left of the window
    float tempPosX;
    float tempPosY;
    constexpr float scale = MENU_SCALE;
    gRootWindow->getTextPosXY(nullptr, WindowAlignment::TOP_LEFT, scale, &tempPosX, &tempPosY);

    // Draw the selected actor text two lines under the options
    constexpr uint32_t totalOptions = sizeof(gOptions) / sizeof(MenuOption);
    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    const float selectedActorTextPosY = tempPosY - (intToFloat(totalOptions + 1) * lineDecrement);

    char buf[96];
    constexpr uint32_t bufSize = sizeof(buf);
    const uint32_t textColor = getColorWhite(0xFF);

    snprintf(buf, bufSize, "Selected Actor\n%s", gBattleActors[static_cast<int32_t>(type) - 1]);
    drawText(buf, tempPosX, selectedActorTextPosY, scale, textColor);

    // Get the width that is being used by the main text options
    float textWidth = getTextWidth("Change Held Item", scale);

    // Set the text to be a bit to the right of the main text
    constexpr float textIncrement = lineDecrement + (lineDecrement / 2.f) - LINE_HEIGHT_ADJUSTMENT_5(scale);
    const float posXBase = tempPosX + textWidth + textIncrement;
    const float posYBase = tempPosY;

    float posX = posXBase;
    float posY = posYBase;

    // Draw the option descriptions
    constexpr uint32_t totalStats = 4;
    const char **statsStringsPtr = gStatsStrings;

    for (uint32_t i = 0; i < totalStats; i++)
    {
        drawText(statsStringsPtr[i], posX, posY, scale, textColor);
        posY -= lineDecrement;
    }

    // Set the values text to be a bit to the right of the previous text
    textWidth = getTextWidth("Max HP", scale);
    posX = posXBase + textWidth + textIncrement;
    posY = posYBase;

    // Put each stat in an array
    int32_t actorStats[totalStats];
    actorStats[0] = actorPtr->current_hp;
    actorStats[1] = actorPtr->max_hp;
    actorStats[2] = actorPtr->current_fp;
    actorStats[3] = actorPtr->max_fp;

    // Draw each stat
    for (uint32_t i = 0; i < totalStats; i++)
    {
        snprintf(buf, bufSize, "%" PRId32, actorStats[i]);
        drawText(buf, posX, posY, scale, textColor);
        posY -= lineDecrement;
    }

    // Draw the text for showing what the current item is two lines below the previous text
    posX = posXBase;
    posY -= lineDecrement;
    drawText("Current Held Item", posX, posY, scale, textColor);

    const ItemId heldItem = static_cast<ItemId>(actorPtr->held_item);
    if (itemIsValid(heldItem))
    {
        constexpr float iconSizeAdjustment = ICON_SIZE_FLOAT * scale;
        posX += (iconSizeAdjustment / 2.f) - (scale * 2.f);
        posY -= lineDecrement + iconSizeAdjustment + (3.f * scale);

        drawItemIconWithText(posX, posY, scale, 0.f, heldItem, textColor);
    }
    else
    {
        // Assume no item is held
        posY -= lineDecrement;

        drawText("None", posX, posY, scale, textColor);
    }
}

static void draw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDraw(cameraId, user);

    // If the pointer to the selected actor is invalid, then assume that the actor is no longer in the battle
    BattlesMenu *battlesMenuPtr = gBattlesMenu;
    BattleWorkUnit *actorPtr = getActorBattlePtr(battlesMenuPtr->getCurrentActorIndex());

    if (!actorPtr)
    {
        battlesMenuReturnToInitMenu();
        return;
    }

    // Draw the stats for the selected actor
    drawBattleActorStats(actorPtr);

    // Draw the value editor if applicable
    ValueEditor *valueEditorPtr = battlesMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->draw();
    }
}
