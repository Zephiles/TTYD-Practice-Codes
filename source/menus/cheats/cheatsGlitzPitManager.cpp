#include "menuUtils.h"
#include "drawText.h"
#include "mod.h"
#include "classes/glitzPitOpponentSelector.h"
#include "menus/cheatsMenu.h"
#include "menus/rootMenu.h"
#include "misc/utils.h"
#include "ttyd/rel/tou.h"
#include "ttyd/camdrv.h"
#include "ttyd/swdrv.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

static void controls(Menu *menuPtr, MenuButtonInput button);
static void draw(CameraId cameraId, void *user);
static void selectedOptionChangeMarioRank(Menu *menuPtr);
static void selectedOptionChangeCurrentCondition(Menu *menuPtr);
static void selectedOptionChangeCurrentOpponent(Menu *menuPtr);
static void selectedOptionToggleMatchReserved(Menu *menuPtr);
static void selectedOptionToggleWonPreviousMatch(Menu *menuPtr);

static const MenuOption gOptions[] = {
    "Change Mario's Rank",
    selectedOptionChangeMarioRank,

    "Change Current Condition",
    selectedOptionChangeCurrentCondition,

    "Change Current Opponent",
    selectedOptionChangeCurrentOpponent,

    "Toggle Match Reserved",
    selectedOptionToggleMatchReserved,

    "Toggle Won Previous Match",
    selectedOptionToggleWonPreviousMatch,

};

static const MenuFunctions gFuncs = {
    gOptions,
    controls,
    draw,
    nullptr, // Exit function not needed
};

void cheatsMenuGlitzPitManagerInit(Menu *menuPtr)
{
    (void)menuPtr;

    constexpr uint32_t totalOptions = sizeof(gOptions) / sizeof(MenuOption);
    enterNextMenu(&gFuncs, totalOptions);
}

static void drawGlitzPitManagerInfo()
{
    // Get the text position for the top-left of the window two lines under the main text
    float tempPosX;
    float tempPosY;
    constexpr float scale = MENU_SCALE;
    const uint32_t totalOptions = gMenu->getTotalOptions();
    gRootWindow->getTextPosXYUnderMainText(nullptr, WindowAlignment::TOP_LEFT, totalOptions, 2, scale, &tempPosX, &tempPosY);

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    const float posX = tempPosX;
    float posY = tempPosY;

    // Draw the header for Mario's current rank
    drawText("Mario's Rank", posX, posY, scale, getColorWhite(0xFF));

    // Draw Mario's current rank
    char buf[64];
    constexpr uint32_t bufSize = sizeof(buf);

    GlitzPitVariables glitzPitVariables;
    const uint32_t marioCurrentRank = glitzPitVariables.getMarioRank();
    const char *marioCurrentRankText;

    if (marioCurrentRank == 0)
    {
        // Assume that Mario is currently the champion
        marioCurrentRankText = "Champion";
    }
    else
    {
        snprintf(buf, bufSize, "%" PRIu32, marioCurrentRank);
        marioCurrentRankText = buf;
    }

    // Get the text position a bit to the right of the longest text excluding the current condition
    const float width = getTextWidth("Won Previous Match", scale);
    const float widthAdjustment = width + (20.f * scale);

    drawText(marioCurrentRankText, posX + widthAdjustment, posY, scale, getColorWhite(0xFF));
    posY -= lineDecrement;

    // Draw the header text for the current opponent
    drawText("Current Opponent", posX, posY, scale, getColorWhite(0xFF));

    // Draw the text for the current opponent
    int32_t currentOpponentIndex = GlitzPitOpponentSelector::glitzPitOpponentToIndex();
    const char *currentOpponentText;

    // Make sure `currentOpponentIndex` is valid
    if (currentOpponentIndex >= 0)
    {
        currentOpponentText = GlitzPitOpponentSelector::gGlitzPitOpponents[currentOpponentIndex];
    }
    else
    {
        // `currentOpponentIndex` is invalid somehow
        currentOpponentText = "Invalid Opponent";
    }

    drawText(currentOpponentText, posX + widthAdjustment, posY, scale, getColorWhite(0xFF));
    posY -= lineDecrement;

    // Draw the header text for whether or not a match is currently reserved
    drawText("Match Reserved", posX, posY, scale, getColorWhite(0xFF));

    // Draw the flag for whether or not a match is currently reserved
    const char *string;
    uint32_t color;

    getYesNoTextAndColor(swGet(2388), &string, &color, 0xFF);
    drawText(string, posX + widthAdjustment, posY, scale, color);
    posY -= lineDecrement;

    // Draw the header text for whether or not the player won the previous match
    drawText("Won Previous Match", posX, posY, scale, getColorWhite(0xFF));

    // Draw the flag for whether or not the player won the previous match
    getYesNoTextAndColor(swGet(2443), &string, &color, 0xFF);
    drawText(string, posX + widthAdjustment, posY, scale, color);
    posY -= (lineDecrement * 2.f);

    // Draw the header text for the current condition
    drawText("Current Condition", posX, posY, scale, getColorWhite(0xFF));
    posY -= lineDecrement;

    // Draw the current condition
    GlitzPitConditionSelector::getAdjustedConditionText(buf, bufSize);
    drawText(buf, posX, posY, scale, getColorWhite(0xFF));
}

static void controls(Menu *menuPtr, MenuButtonInput button)
{
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;

    // If the value editor is open, then handle the controls for that
    ValueEditor *valueEditorPtr = cheatsMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->controls(button);
        return;
    }

    // If the Glitz Pit condition selector is open, then handle the controls for that
    GlitzPitConditionSelector *glitzPitConditionSelectorPtr = cheatsMenuPtr->getGlitzPitConditionSelectorPtr();
    if (glitzPitConditionSelectorPtr->shouldDraw())
    {
        glitzPitConditionSelectorPtr->controls(button);
        return;
    }

    // If the Glitz Pit opponent selector is open, then handle the controls for that
    GlitzPitOpponentSelector *glitzPitOpponentSelectorPtr = cheatsMenuPtr->getGlitzPitOpponentSelectorPtr();
    if (glitzPitOpponentSelectorPtr->shouldDraw())
    {
        glitzPitOpponentSelectorPtr->controls(button);
        return;
    }

    // Use the default controls
    basicMenuLayoutControls(menuPtr, button);
}

static void draw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDraw(cameraId, user);

    // Draw the info for the Glitz Pit Manager cheat
    drawGlitzPitManagerInfo();

    // Draw the value editor if applicable
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;

    ValueEditor *valueEditorPtr = cheatsMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->draw();
    }

    // Draw the Glitz Pit condition selector if applicable
    GlitzPitConditionSelector *glitzPitConditionSelectorPtr = cheatsMenuPtr->getGlitzPitConditionSelectorPtr();
    if (glitzPitConditionSelectorPtr->shouldDraw())
    {
        glitzPitConditionSelectorPtr->draw();
    }

    // Draw the Glitz Pit opponent selector if applicable
    GlitzPitOpponentSelector *glitzPitOpponentSelectorPtr = cheatsMenuPtr->getGlitzPitOpponentSelectorPtr();
    if (glitzPitOpponentSelectorPtr->shouldDraw())
    {
        glitzPitOpponentSelectorPtr->draw();
    }
}

// Based on the TTYDAP project:
// https://github.com/jamesbrq/TTYDAP/blob/f5523a3375e8ed34d291ad6d9beccb0d15e66753/rel/subrels/tou/source/main.cpp#L213-L258
static void setMarioRankInsideGlitzville(const ValueType *valuePtr, const GlitzPitVariables *glitzPitVarsPtr)
{
    // Loop through all of the ranks and set/clear the Win flag based on what rank Mario should be
    RankingData *rankingDataPtr = glitzPitVarsPtr->getRankWorkPtr();
    RankingEntry *entriesPtr = rankingDataPtr->entries;
    const uint32_t totalEntries = static_cast<uint32_t>(rankingDataPtr->count);

    const uint32_t newRank = valuePtr->u32;
    uint32_t rankIndex = 0;

    for (uint32_t i = 0; i < totalEntries; i++)
    {
        RankingEntry *currentEntry = &entriesPtr[i];

        // Ignore entries that are either inactive or are Mario
        if (currentEntry->flags & (GlitzPitFlag::GLITZ_PIT_FLAG_INACTIVE | GlitzPitFlag::GLITZ_PIT_FLAG_STOP))
        {
            continue;
        }

        if (rankIndex < newRank)
        {
            // Have not reached the new rank yet, so the current entry should not be beaten, as these would be for ranks that
            // are above Mario
            currentEntry->flags &= ~GlitzPitFlag::GLITZ_PIT_FLAG_WIN;
        }
        else
        {
            // The new rank has been reached, so every remaining entry should be marked as beaten, as these would be for ranks
            // that are below Mario
            currentEntry->flags |= GlitzPitFlag::GLITZ_PIT_FLAG_WIN;
        }

        rankIndex++;
    }

    // Call `rankingControll` to properly update the entry positions and GSW(580)

    // When going from the Minor League to the Major League (which is going from rank 11 to rank 10) or vice versa, the fighter
    // that Mario defeats/loses to will always be placed directly beneath Mario regardless of what rank he is placed at. This
    // works fine when moving one rank at a time, but breaks when moving by more than one. To avoid this, GSWF(2532) needs to be
    // cleared before calling `rankingControll`, as `rankingControll` checks for this and if it's not set then it will not
    // reposition the fighter as described. An example of this behavior is that if GSWF(2532) was set and Mario was repositioned
    // from rank 3 to rank 17, then the Armored Harriers would be placed at rank 16.
    swClear(2532);
    glitzPitVarsPtr->getRankingControllFuncPtr()();
}

static void setMarioRankOutsideGlitzville(const ValueType *valuePtr)
{
    // The ranks are stored as flags starting at GSWF(2465) and are ordered based on the fighter ids. So we must loop through
    // all of them and set/clear them based on what rank Mario should be.
    const uint32_t newRank = valuePtr->u32;
    const bool sequenceBelow172 = getSequencePosition() < 172;
    uint32_t rankIndex = 0;

    for (uint32_t i = 0; i < 23; i++)
    {
        // Don't adjust the flag for Mario
        if (i == GlitzPitFighterId::GLITZ_PIT_FIGHTER_ID_MARIO)
        {
            continue;
        }

        // Certain fighters are ignored based on the sequence and various flags
        if (sequenceBelow172)
        {
            // If the KP Koopas have been removed, then don't adjust their flag
            if ((i == GlitzPitFighterId::GLITZ_PIT_FIGHTER_ID_KP_KOOPAS) && (swGet(2412)))
            {
                continue;
            }

            // If Wings Of Night have been added, then make sure their flag is adjusted
            if ((i == GlitzPitFighterId::GLITZ_PIT_FIGHTER_ID_WINGS_OF_NIGHT) && (!swGet(2413)))
            {
                continue;
            }

            if (((i == GlitzPitFighterId::GLITZ_PIT_FIGHTER_ID_HAND_IT_OVERS)) ||
                (i == GlitzPitFighterId::GLITZ_PIT_FIGHTER_ID_DESTRUCTORS))
            {
                // If the The Hand-It-Overs were removed and replaced by The Destructors, then make sure the appropriate flags
                // are adjusted
                if (swGet(2421))
                {
                    if (i == GlitzPitFighterId::GLITZ_PIT_FIGHTER_ID_HAND_IT_OVERS)
                    {
                        // // The Hand-It-Overs were removed and replaced by this point, so don't adjust their flag
                        continue;
                    }
                    else if (i == GlitzPitFighterId::GLITZ_PIT_FIGHTER_ID_DESTRUCTORS)
                    {
                        // The Hand-It-Overs were removed and replaced by The Destructors, so the flag for The Destructors
                        // should be adjusted, so don't need to do anything here
                    }
                }
                else if (i == GlitzPitFighterId::GLITZ_PIT_FIGHTER_ID_HAND_IT_OVERS)
                {
                    // The Hand-It-Overs have not been removed and replaced yet, so their flag should be adjusted, so don't need
                    // to do anything here
                }
                else if (i == GlitzPitFighterId::GLITZ_PIT_FIGHTER_ID_DESTRUCTORS)
                {
                    // The Hand-It-Overs have not been removed and replaced yet, so The Destructors are still hidden, so don't
                    // adjust their flag
                    continue;
                }
            }
        }
        else if (((i == GlitzPitFighterId::GLITZ_PIT_FIGHTER_ID_WINGS_OF_NIGHT)) ||
                 (i == GlitzPitFighterId::GLITZ_PIT_FIGHTER_ID_DESTRUCTORS))
        {
            // Wings Of Night and The Destructors are always excluded in a post-chapter 3 state
            continue;
        }

        const uint32_t currentFlag = 2465 + i;
        if (rankIndex < newRank)
        {
            // Mario has not beaten this fighter yet
            swClear(currentFlag);
        }
        else
        {
            // Mario has beaten this fighter
            swSet(currentFlag);
        }

        rankIndex++;
    }

    // When going from the Minor League to the Major League (which is going from rank 11 to rank 10) or vice versa, the fighter
    // that Mario defeats/loses to will always be placed directly beneath Mario regardless of what rank he is placed at. This
    // works fine when moving one rank at a time, but breaks when moving by more than one. To avoid this, GSWF(2532) needs to be
    // cleared before calling entering Glitzville, as `rankingControll` will end up being called automatically. This function
    // checks for this flag, and if it's not set then it will not reposition the fighter as described. An example of this
    // behavior is that if GSWF(2532) was set and Mario was repositioned from rank 3 to rank 17, then the Armored Harriers would
    // be placed at rank 16.
    swClear(2532);

    // Update GSW(580) with Mario's new rank
    swByteSet(580, newRank);
}

static void setMarioRank(const ValueType *valuePtr)
{
    // The method for setting Mario's rank depends on whether or not the player is currently in Glitzville
    GlitzPitVariables glitzPitVars;

    if (glitzPitVars.playerIsInGlitzville())
    {
        setMarioRankInsideGlitzville(valuePtr, &glitzPitVars);
    }
    else
    {
        setMarioRankOutsideGlitzville(valuePtr);
    }

    // Close the value editor
    cheatsMenuValueEditorCancelSetValue();
}

static void selectedOptionChangeMarioRank(Menu *menuPtr)
{
    (void)menuPtr;

    // Initialize the value editor
    uint32_t totalActiveFightersMaxIndex = 20; // The Destructors and Wings of Night are not included by default

    // The amount of total fighters differs based on various things
    if (getSequencePosition() < 172)
    {
        // Have not beaten the chapter yet, so certain flags determine fighters leaving/joining
        // Check if the KP Koopas have been removed
        if (swGet(2412))
        {
            totalActiveFightersMaxIndex--;
        }

        // Check if Wings of Night have been added
        if (swGet(2413))
        {
            totalActiveFightersMaxIndex++;
        }

        // The Hand-It-Overs end up getting replaced by The Destructors, so the actual amount of active fighters does not
        // change, so nothing needs to be done for them
    }

    GlitzPitVariables glitzPitVariables;
    const uint32_t currentRank = glitzPitVariables.getMarioRank();

    uint32_t flags = 0;
    flags = ValueEditor::setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);
    flags = ValueEditor::setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Y_SET_MAX);
    flags = ValueEditor::setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Z_SET_MIN);

    cheatsMenuInitValueEditor(currentRank, 0, totalActiveFightersMaxIndex, flags, VariableType::u8, true, setMarioRank);
}

static void cancelChangeCurrentCondition()
{
    gCheatsMenu->getGlitzPitConditionSelectorPtr()->stopDrawing();
}

static void changeCurrentCondition(uint32_t index)
{
    // Convert the index to a condition
    int32_t conditionId = GlitzPitConditionSelector::indexToGlitzPitCondition(static_cast<int32_t>(index));

    // Make sure the condition is valid
    if (conditionId < 0)
    {
        // Condition is invalid somehow, so default to `Win in 5 Turns or Less`
        conditionId = GlitzPitCondition::GLITZ_PIT_CONDITION_WIN_IN_5_TURNS_OR_LESS;
    }

    // Set the new condition
    swByteSet(502, static_cast<uint32_t>(conditionId));

    // Close the Glitz Pit condition selector
    cancelChangeCurrentCondition();
}

static void selectedOptionChangeCurrentCondition(Menu *menuPtr)
{
    (void)menuPtr;

    // Initialize the Glitz Pit condition selector
    GlitzPitConditionSelector *glitzPitConditionSelectorPtr = gCheatsMenu->getGlitzPitConditionSelectorPtr();
    const Window *rootWindowPtr = gRootWindow;

    glitzPitConditionSelectorPtr->init(rootWindowPtr, rootWindowPtr->getAlpha());

    // Get the index for the current condition
    int32_t conditionIndex = GlitzPitConditionSelector::glitzPitConditionToIndex();

    // Make sure the index is valid
    if (conditionIndex < 0)
    {
        // Index is invalid somehow
        conditionIndex = 0;
    }

    glitzPitConditionSelectorPtr->setCurrentIndex(static_cast<uint32_t>(conditionIndex));
    glitzPitConditionSelectorPtr->startDrawing(changeCurrentCondition, cancelChangeCurrentCondition);
}

static void cancelChangeCurrentOpponent()
{
    gCheatsMenu->getGlitzPitOpponentSelectorPtr()->stopDrawing();
}

static void changeCurrentOpponent(uint32_t index)
{
    // Convert the index to a fighter id
    int32_t opponentId = GlitzPitOpponentSelector::indexToGlitzPitOpponent(static_cast<int32_t>(index));

    // Make sure the opponent is valid
    if (opponentId < 0)
    {
        // Opponent is invalid somehow, so default to The Goomba Bros.
        opponentId = GlitzPitFighterId::GLITZ_PIT_FIGHTER_ID_GOOMBA_BROS;
    }

    // Set the new opponent
    swByteSet(501, static_cast<uint32_t>(opponentId));

    // Close the Glitz Pit opponent selector
    cancelChangeCurrentOpponent();
}

static void selectedOptionChangeCurrentOpponent(Menu *menuPtr)
{
    (void)menuPtr;

    // Initialize the Glitz Pit opponent selector
    GlitzPitOpponentSelector *glitzPitOpponentSelectorPtr = gCheatsMenu->getGlitzPitOpponentSelectorPtr();
    const Window *rootWindowPtr = gRootWindow;

    glitzPitOpponentSelectorPtr->init(rootWindowPtr, rootWindowPtr->getAlpha());

    // Get the index for the current opponent
    int32_t opponentIndex = GlitzPitOpponentSelector::glitzPitOpponentToIndex();

    // Make sure the index is valid
    if (opponentIndex < 0)
    {
        // Index is invalid somehow
        opponentIndex = 0;
    }

    glitzPitOpponentSelectorPtr->setCurrentIndex(static_cast<uint32_t>(opponentIndex));
    glitzPitOpponentSelectorPtr->startDrawing(changeCurrentOpponent, cancelChangeCurrentOpponent);
}

static void selectedOptionToggleMatchReserved(Menu *menuPtr)
{
    (void)menuPtr;

    toggleGSWF(2388);
}

static void selectedOptionToggleWonPreviousMatch(Menu *menuPtr)
{
    (void)menuPtr;

    toggleGSWF(2443);
}
