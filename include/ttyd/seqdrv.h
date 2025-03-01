#ifndef TTYD_SEQDRV_H
#define TTYD_SEQDRV_H

#include <cstdint>

enum class SeqIndex : int32_t
{
    kBoot = -1,
    kLogo,
    kTitle,
    kGame,
    kMapChange,
    kBattle,
    kGameOver,
    kLoad,
    kE3,
};

struct SeqWork
{
    SeqIndex seq;
    int32_t state;
    const char *mapName;
    const char *beroName;
    uint32_t counter;
    uint32_t unk_14;
    uint32_t unk_18;
    uint32_t unk_1c;
} __attribute__((__packed__));

static_assert(sizeof(SeqWork) == 0x20);

extern "C"
{
    extern SeqWork seqWork;

    void seqInit_MARIOSTORY();
    void seqMain();
    void seqSetSeq(SeqIndex seq, const char *map, const char *bero);
    SeqIndex seqGetSeq();
    SeqIndex seqGetPrevSeq();
    SeqIndex seqGetNextSeq();
    bool seqCheckSeq();
}

#endif