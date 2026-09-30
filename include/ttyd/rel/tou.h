#ifndef TTYD_REL_TOU_H
#define TTYD_REL_TOU_H

#include <cstdint>

// Partially taken from the TTYDAP project:
// https://github.com/jamesbrq/TTYDAP/blob/main/rel/include/ttyd/tou.h
enum GlitzPitFighterId
{
    GLITZ_PIT_FIGHTER_ID_RAWK_HAWK = 0,
    GLITZ_PIT_FIGHTER_ID_KOOPINATOR,
    GLITZ_PIT_FIGHTER_ID_CHOMP_COUNTRY,
    GLITZ_PIT_FIGHTER_ID_HAMMA_BAMMA_FLARE,
    GLITZ_PIT_FIGHTER_ID_CRAW_DADDY,
    GLITZ_PIT_FIGHTER_ID_MAGIKOOPA_MASTERS,
    GLITZ_PIT_FIGHTER_ID_FUZZ,
    GLITZ_PIT_FIGHTER_ID_SHELLSHOCKERS,
    GLITZ_PIT_FIGHTER_ID_POKER_FACES,
    GLITZ_PIT_FIGHTER_ID_TINY_SPINIES,
    GLITZ_PIT_FIGHTER_ID_ARMORED_HARRIERS,
    GLITZ_PIT_FIGHTER_ID_HAND_IT_OVERS,
    GLITZ_PIT_FIGHTER_ID_BOB_OMB_SQUAD,
    GLITZ_PIT_FIGHTER_ID_PUNK_ROCKS,
    GLITZ_PIT_FIGHTER_ID_MIND_BOGGLERS,
    GLITZ_PIT_FIGHTER_ID_SPIKE_STORM,
    GLITZ_PIT_FIGHTER_ID_DEAD_BONES,
    GLITZ_PIT_FIGHTER_ID_POKEY_TRIPLETS,
    GLITZ_PIT_FIGHTER_ID_KP_KOOPAS,
    GLITZ_PIT_FIGHTER_ID_GOOMBA_BROS,
    GLITZ_PIT_FIGHTER_ID_MARIO,
    GLITZ_PIT_FIGHTER_ID_WINGS_OF_NIGHT,
    GLITZ_PIT_FIGHTER_ID_DESTRUCTORS,
};

// Only including the valid conditions for now
enum GlitzPitCondition
{
    GLITZ_PIT_CONDITION_DONT_USE_JUMP = 1,
    GLITZ_PIT_CONDITION_DONT_USE_HAMMER = 3,
    GLITZ_PIT_CONDITION_DONT_USE_SPECIAL_MOVES = 5,
    GLITZ_PIT_CONDITION_USE_SPECIAL_MOVE_ONCE = 6,
    GLITZ_PIT_CONDITION_WIN_BEFORE_TAKING_20_DAMAGE = 11,
    GLITZ_PIT_CONDITION_TAKE_DAMAGE_X_TIMES = 18,
    GLITZ_PIT_CONDITION_DONT_USE_ITEMS = 26,
    GLITZ_PIT_CONDITION_DONT_SWITCH_PARTNERS = 32,
    GLITZ_PIT_CONDITION_APPEAL_X_TIMES = 43,
    GLITZ_PIT_CONDITION_DONT_USE_FP = 48,
    GLITZ_PIT_CONDITION_WIN_IN_5_TURNS_OR_LESS = 59,
    GLITZ_PIT_CONDITION_DONT_ATTACK_WITH_MARIO = 63,
    GLITZ_PIT_CONDITION_DONT_ATTACK_WITH_PARTNERS = 64,
    GLITZ_PIT_CONDITION_DONT_ATTACK_FOR_FIRST_3_TURNS = 65,
};

enum GlitzPitFlag
{
    GLITZ_PIT_FLAG_WIN = 1,      // Bit 0: Win flag
    GLITZ_PIT_FLAG_INACTIVE = 2, // Bit 1: Inactive/skip entry
    GLITZ_PIT_FLAG_STOP = 4,     // Bit 2: Stop counting flag
};

struct RankingEntry
{
    uint16_t flags;    // +0x00: Flags
    uint16_t pad1;     // +0x02: Padding
    int32_t fighterId; // +0x04: Fighter ID
    int16_t score;     // +0x08: Score value
    uint16_t pad2;     // +0x0A: Padding
} __attribute__((__packed__));

struct RankingData
{
    int32_t count;         // Number of entries
    RankingEntry *entries; // Array of ranking entries
} __attribute__((__packed__));

static_assert(sizeof(RankingEntry) == 0xC);
static_assert(sizeof(RankingData) == 0x8);

extern "C"
{
    extern RankingData *tou_rank_wp;

    void tou_rankingControll();
}

#endif
