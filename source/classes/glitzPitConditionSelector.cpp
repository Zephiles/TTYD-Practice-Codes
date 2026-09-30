#include "mod.h"
#include "classes/glitzPitConditionSelector.h"
#include "classes/window.h"
#include "classes/menu.h"
#include "misc/utils.h"
#include "ttyd/rel/tou.h"
#include "ttyd/swdrv.h"

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cinttypes>

const char *GlitzPitConditionSelector::gGlitzPitConditions[TOTAL_GLITZ_PIT_CONDITIONS_MAJOR_LEAGUE] = {
    // The following conditions are used in both the Minor League and the Major League
    "Don't use your Jump",
    "Don't yse your Hammer",
    "Don't use Special Moves",
    "Use a Special Move at least one",
    "Take damage X times",
    "Don't use Items",
    "Don't switch Partners",
    "Appeal at least X times",
    "Don't use FP",
    "Win in 5 Turns or Less",

    // The following conditions are only available when using the Major League set of conditions
    "Don't Attack with Mario",
    "Don't Attack with Partners",
    "Don't Attack for the first 3 turns",
    "Win before taking 20 points of Damage",
};

static const uint8_t gGlitzPitConditionsValues[TOTAL_GLITZ_PIT_CONDITIONS_MAJOR_LEAGUE] = {
    // The following conditions are used in both the Minor League and the Major League
    GlitzPitCondition::GLITZ_PIT_CONDITION_DONT_USE_JUMP,
    GlitzPitCondition::GLITZ_PIT_CONDITION_DONT_USE_HAMMER,
    GlitzPitCondition::GLITZ_PIT_CONDITION_DONT_USE_SPECIAL_MOVES,
    GlitzPitCondition::GLITZ_PIT_CONDITION_USE_SPECIAL_MOVE_ONCE,
    GlitzPitCondition::GLITZ_PIT_CONDITION_TAKE_DAMAGE_X_TIMES,
    GlitzPitCondition::GLITZ_PIT_CONDITION_DONT_USE_ITEMS,
    GlitzPitCondition::GLITZ_PIT_CONDITION_DONT_SWITCH_PARTNERS,
    GlitzPitCondition::GLITZ_PIT_CONDITION_APPEAL_X_TIMES,
    GlitzPitCondition::GLITZ_PIT_CONDITION_DONT_USE_FP,
    GlitzPitCondition::GLITZ_PIT_CONDITION_WIN_IN_5_TURNS_OR_LESS,

    // The following conditions are only available when using the Major League set of conditions
    GlitzPitCondition::GLITZ_PIT_CONDITION_DONT_ATTACK_WITH_MARIO,
    GlitzPitCondition::GLITZ_PIT_CONDITION_DONT_ATTACK_WITH_PARTNERS,
    GlitzPitCondition::GLITZ_PIT_CONDITION_DONT_ATTACK_FOR_FIRST_3_TURNS,
    GlitzPitCondition::GLITZ_PIT_CONDITION_WIN_BEFORE_TAKING_20_DAMAGE,
};

void GlitzPitConditionSelector::init(const Window *parentWindow)
{
    this->init(parentWindow, 0xFF);
}

static bool usingMajorLeagueConditions()
{
    // If in a post-chapter 3 sequence, then the game will always use the Major League conditions
    if (getSequencePosition() < 172)
    {
        // Have not beaten chapter 3 yet
        GlitzPitVariables glitzPitVariables;

        if (glitzPitVariables.getMarioRank() > 10)
        {
            // Using Minor League conditions
            return false;
        }
        else
        {
            // Using Major League conditions
            return true;
        }
    }
    else
    {
        // In a post-chapter 3 sequence, so use the Major League conditions
        return true;
    }
}

static uint32_t getTotalConditions()
{
    if (usingMajorLeagueConditions())
    {
        return TOTAL_GLITZ_PIT_CONDITIONS_MAJOR_LEAGUE;
    }
    else
    {
        return TOTAL_GLITZ_PIT_CONDITIONS_MINOR_LEAGUE;
    }
}

void GlitzPitConditionSelector::init(const Window *parentWindow, uint8_t windowAlpha)
{
    this->OptionSelector::init(gHelpTextAConfirmBCancel,
                               this->gGlitzPitConditions,
                               getTotalConditions(),
                               1,
                               parentWindow,
                               windowAlpha,
                               0.f);
}

int32_t GlitzPitConditionSelector::glitzPitConditionToIndex()
{
    // Find the current condition in `gGlitzPitConditionsValues`
    const uint32_t currentCondition = swByteGet(502);
    const int32_t totalConditions = static_cast<int32_t>(getTotalConditions());
    const uint8_t *glitzPitConditionsPtr = &gGlitzPitConditionsValues[0];

    for (int32_t i = 0; i < totalConditions; i++)
    {
        if (currentCondition == glitzPitConditionsPtr[i])
        {
            return i;
        }
    }

    // Didn't find the condition
    return -1;
}

int32_t GlitzPitConditionSelector::indexToGlitzPitCondition(int32_t index)
{
    // Make sure the index is valid
    if ((index < 0) || (index >= static_cast<int32_t>(getTotalConditions())))
    {
        return -1;
    }

    return static_cast<int32_t>(gGlitzPitConditionsValues[index]);
}

void GlitzPitConditionSelector::getAdjustedConditionText(char *bufferOut, uint32_t bufferSize)
{
    // Make sure the current condition is valid
    const int32_t conditionIndex = GlitzPitConditionSelector::glitzPitConditionToIndex();
    if (conditionIndex < 0)
    {
        // The current condition is somehow invalid
        copyStringAndNullTerminate(bufferOut, bufferSize, "Invalid Condition");
        return;
    }

    // `Take damage X times` and `Appeal at least X times` must be adjusted based on what the game is currently using
    if (conditionIndex == GlitzPitConditionSelectorIndex::GLITZ_PIT_CONDITION_SELECTOR_INDEX_TAKE_DAMAGE_X_TIMES)
    {
        uint32_t count;

        if (usingMajorLeagueConditions())
        {
            count = 5;
        }
        else
        {
            count = 3;
        }

        snprintf(bufferOut, bufferSize, "Take damage %" PRIu32 " times", count);
    }
    else if (conditionIndex == GlitzPitConditionSelectorIndex::GLITZ_PIT_CONDITION_SELECTOR_INDEX_APPEAL_X_TIMES)
    {
        const char *appealText;

        if (usingMajorLeagueConditions())
        {
            appealText = "Appeal at least 3 times";
        }
        else
        {
            appealText = "Appeal at least once";
        }

        copyStringAndNullTerminate(bufferOut, bufferSize, appealText);
    }
    else
    {
        copyStringAndNullTerminate(bufferOut, bufferSize, gGlitzPitConditions[conditionIndex]);
    }
}
