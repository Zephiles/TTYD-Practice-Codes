#include "mod.h"
#include "classes/glitzPitOpponentSelector.h"
#include "classes/window.h"
#include "classes/menu.h"
#include "misc/utils.h"
#include "ttyd/rel/tou.h"
#include "ttyd/swdrv.h"

#include <cstdint>

const char *GlitzPitOpponentSelector::gGlitzPitOpponents[TOTAL_GLITZ_PIT_OPPONENTS] = {
    "Rawk Hawk",
    "The Koopinator",
    "Chomp Country",
    "Hamma, Bamma, and Flare",
    "Craw-Daddy",
    "The Magikoopa Masters",
    "The Fuzz",
    "The Shellshockers",
    "The Poker Faces",
    "The Tiny Spinies",
    "The Armored Harriers",
    "The Hand-It-Overs",
    "The Bob-omb Squad",
    "The Punk Rocks",
    "The Mind-Bogglers",
    "Spike Storm",
    "The Dead Bones",
    "The Pokey Triplets",
    "The KP Koopas",
    "The Goomba Bros.",
    "Wings of Night",
    "The Destructors",
};

void GlitzPitOpponentSelector::init(const Window *parentWindow)
{
    this->init(parentWindow, 0xFF);
}

void GlitzPitOpponentSelector::init(const Window *parentWindow, uint8_t windowAlpha)
{
    const uint32_t totalOpponents = this->getGlitzPitTotalFighters() - 1; // Subtract one to exclude Mario

    this->OptionSelector::init(gHelpTextAConfirmBCancel,
                               this->gGlitzPitOpponents,
                               totalOpponents,
                               2,
                               parentWindow,
                               windowAlpha,
                               0.f);
}

uint32_t GlitzPitOpponentSelector::getGlitzPitTotalFighters()
{
    // The Destructors and Wings of Night are only added if the sequence is less than 172
    if (getSequencePosition() < 172)
    {
        return 23;
    }
    else
    {
        return 21;
    }
}

int32_t GlitzPitOpponentSelector::glitzPitOpponentToIndex()
{
    // Make sure the fighter id is valid
    int32_t highestFighterId;

    if (getSequencePosition() < 172)
    {
        // The Destructors and Wings of Night are included
        highestFighterId = GlitzPitFighterId::GLITZ_PIT_FIGHTER_ID_DESTRUCTORS;
    }
    else
    {
        highestFighterId = GlitzPitFighterId::GLITZ_PIT_FIGHTER_ID_GOOMBA_BROS;
    }

    int32_t fighterId = static_cast<int32_t>(swByteGet(501));
    if ((fighterId < 0) || (fighterId > highestFighterId))
    {
        return -1;
    }

    if (fighterId > GlitzPitFighterId::GLITZ_PIT_FIGHTER_ID_MARIO)
    {
        // Subtract one to account for Mario not being included
        fighterId -= 1;
    }

    return fighterId;
}

int32_t GlitzPitOpponentSelector::indexToGlitzPitOpponent(int32_t index)
{
    // Make sure the index is valid
    const int32_t totalOpponents = GlitzPitOpponentSelector::getGlitzPitTotalFighters() - 1; // Subtract one to exclude Mario

    if ((index < 0) || (index >= totalOpponents))
    {
        return -1;
    }

    if (index >= GlitzPitOpponentSelectorIndex::GLITZ_PIT_OPPONENT_SELECTOR_INDEX_WINGS_OF_NIGHT)
    {
        // Add one to account for Mario not being included
        index += 1;
    }

    return index;
}
