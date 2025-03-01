#include "mod.h"
#include "classes/marioCharacterSelector.h"
#include "classes/window.h"
#include "classes/menu.h"
#include "ttyd/mario.h"

#include <cstdint>

const char *gMarioCharactersStrings[TOTAL_MARIO_CHARACTERS] = {
    "Mario",
    "Bowser",
    "Peach",
    "X-Naut",
};

int32_t marioCharacterIdToIndex()
{
    const Player *marioPtr = marioGetPtr();

    switch (marioPtr->characterId)
    {
        case MarioCharacters::kMario:
        {
            return MarioCharacterSelectorIndex::MARIO_CHARACTER_SELECTOR_INDEX_MARIO;
        }
        case MarioCharacters::kPeach:
        {
            if (!peachInXNautForm(marioPtr))
            {
                // Currently Peach
                return MarioCharacterSelectorIndex::MARIO_CHARACTER_SELECTOR_INDEX_PEACH;
            }
            else
            {
                // Currently X-Naut
                return MarioCharacterSelectorIndex::MARIO_CHARACTER_SELECTOR_INDEX_X_NAUT;
            }
        }
        case MarioCharacters::kBowser:
        {
            return MarioCharacterSelectorIndex::MARIO_CHARACTER_SELECTOR_INDEX_BOWSER;
        }
        default:
        {
            return -1;
        }
    }
}

void MarioCharacterSelector::init(const Window *parentWindow)
{
    this->init(parentWindow, 0xFF);
}

void MarioCharacterSelector::init(const Window *parentWindow, uint8_t windowAlpha)
{
    this->OptionSelector::init(gHelpTextAConfirmBCancel,
                               gMarioCharactersStrings,
                               TOTAL_MARIO_CHARACTERS,
                               1,
                               parentWindow,
                               windowAlpha,
                               0.f);
}
