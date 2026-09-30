#ifndef TTYD_REL_TOU2_H
#define TTYD_REL_TOU2_H

#include "ttyd/rel/tou.h"

#include <cstdint>

extern "C"
{
    extern RankingData *tou2_rank_wp;

    void tou2_rankingControll();
}

#endif
