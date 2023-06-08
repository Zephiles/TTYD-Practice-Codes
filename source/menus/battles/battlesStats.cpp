#include "menuUtils.h"
#include "drawText.h"
#include "drawIcon.h"
#include "classes/window.h"
#include "menus/battles.h"
#include "menus/root.h"
#include "misc/utils.h"
#include "ttyd/battle_unit.h"
#include "ttyd/item_data.h"
#include "ttyd/dispdrv.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

const char *gBattlesActorStatsInitialStrings[] = {
    "HP",
    "Max HP",
    "FP",
    "Max FP",
};

const MenuOption gBattlesMenuStatsOptions[] = {
    "Change HP",
    nullptr,

    "Change Max HP",
    nullptr,

    "Change FP",
    nullptr,

    "Change Max FP",
    nullptr,

    "Change Held Item",
    nullptr,

    "Clear Held Item",
    selectedOptionBattlesClearHeldItem,

    "Change Statuses",
    nullptr,
};

const MenuFunctions gBattlesMenuStatsFuncs = {
    battlesMenuStatsControls,
    battlesMenuStatsDraw,
    nullptr, // Exit function not needed
};

void battlesMenuStatsInit()
{
    constexpr uint32_t totalOptions = sizeof(gBattlesMenuStatsOptions) / sizeof(MenuOption);
    enterNextMenu(gBattlesMenuStatsOptions, &gBattlesMenuStatsFuncs, totalOptions);
}

void battlesMenuStatsControls(Menu *menuPtr, MenuButtonInput button)
{
    // Close the Battles menu if not in a battle
    if (!getBattleWorkPtr())
    {
        rootMenuExitBattleMenu();
        return;
    }

    // If the pointer to the selected actor is invalid, then assume that the actor is no longer in the battle
    Battles *battlesPtr = gBattles;
    if (!getActorBattlePtr(battlesPtr->getCurrentActorIndex()))
    {
        battlesMenuReturnToSelectActorMenu();
        return;
    }

    // If the value editor is open, then handle the controls for that
    if (menuPtr->flagIsSet(BattlesStatsFlag::BATTLES_FLAG_STATS_CURRENTLY_SELECTING_ID))
    {
        battlesPtr->getValueEditor()->controls(button);
        return;
    }

    // Use the default controls
    controlsBasicMenuLayout(menuPtr, button);
}

void selectedOptionBattlesClearHeldItem(Menu *menuPtr)
{
    (void)menuPtr;

    getActorBattlePtr(gBattles->getCurrentActorIndex())->held_item = static_cast<int32_t>(ItemId::ITEM_NONE);
}

void Battles::drawBattleActorStats() const
{
    // Get the pointer to the selected actor
    const BattleWorkUnit *actorPtr = getActorBattlePtr(this->currentActorIndex);

    // Failsafe: Make sure current_kind is valid for the selected actor
    const BattleUnitType type = actorPtr->current_kind;
    if (!battleUnitTypeIsValid(type))
    {
        return;
    }

    // Get the width that is being used by the main text options
    const char *longestText = "Change Held Item";
    const float scale = this->scale;
    float textWidth;
    getTextWidthHeight(longestText, scale, &textWidth, nullptr);

    // Get the text position for the top-left of the window
    float tempPosX;
    float tempPosY;
    const Window *rootWindowPtr = gRootWindow;
    rootWindowPtr->getTextPosXY(longestText, WindowAlignment::TOP_LEFT, scale, &tempPosX, &tempPosY);

    // Draw the selected actor text two lines under the options
    constexpr uint32_t totalOptions = sizeof(gBattlesMenuStatsOptions) / sizeof(MenuOption);
    const float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    const float selectedActorTextPosY = tempPosY - (intToFloat(totalOptions + 1) * lineDecrement);

    char *displayBufferPtr = gDisplayBuffer;
    const uint32_t textColor = getColorWhite(0xFF);
    snprintf(displayBufferPtr, DISPLAY_BUFFER_SIZE, "Selected Actor\n%s", gBattleActors[static_cast<int32_t>(type) - 1]);
    drawText(displayBufferPtr, tempPosX, selectedActorTextPosY, scale, textColor);

    // Set the text to be a bit to the right of the main text
    // Retrieve posXBase and posYBase as a separate variables to avoid repeatedly loading them from the stack when using them
    const float textIncrement = lineDecrement + (lineDecrement / 2.f) - LINE_HEIGHT_ADJUSTMENT_5(scale);
    const float posXBase = tempPosX + textWidth + textIncrement;
    const float posYBase = tempPosY;

    float posX = posXBase;
    float posY = posYBase;

    // Draw the option descriptions
    constexpr uint32_t totalStats = 4;
    const char **battlesActorStatsInitialStringsPtr = gBattlesActorStatsInitialStrings;

    for (uint32_t i = 0; i < totalStats; i++)
    {
        drawText(battlesActorStatsInitialStringsPtr[i], posX, posY, scale, textColor);
        posY -= lineDecrement;
    }

    // Set the values text to be a bit to the right of the previous text
    getTextWidthHeight("Max HP", scale, &textWidth, nullptr);
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
        snprintf(displayBufferPtr, DISPLAY_BUFFER_SIZE, "%" PRId32, actorStats[i]);
        drawText(displayBufferPtr, posX, posY, scale, textColor);
        posY -= lineDecrement;
    }

    // Draw the text for showing what the current item is two lines below the previous text
    posX = posXBase;
    posY -= lineDecrement;
    drawText("Current Held Item", posX, posY, scale, textColor);

    const ItemId heldItem = static_cast<ItemId>(actorPtr->held_item);
    if (itemIsValid(heldItem))
    {
        const float iconSizeAdjustment = ICON_SIZE_FLOAT * scale;
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

void battlesMenuStatsDraw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    drawBasicMenuLayout(cameraId, user);

    // If the pointer to the selected actor is invalid, then assume that the actor is no longer in the battle
    Battles *battlesPtr = gBattles;
    if (!getActorBattlePtr(battlesPtr->getCurrentActorIndex()))
    {
        battlesMenuReturnToSelectActorMenu();
        return;
    }

    // Draw the stats for the selected actor
    battlesPtr->drawBattleActorStats();

    // Draw the value editor if applicable
    ValueEditor *valueEditorPtr = battlesPtr->getValueEditor();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->draw();
    }
}
