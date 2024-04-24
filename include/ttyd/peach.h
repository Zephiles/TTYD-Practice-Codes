#ifndef TTYD_PEACH_H
#define TTYD_PEACH_H

#include <cstdint>

extern "C"
{
    // peach_talk
    // peach_dash
    // peach_walk
    // peach_stay
    // peachMain
    // peachPreInit
    void peachTransformOff();
    void peachTransformOn();
    // peachChgPose
    // peachSetStsCarry
    // peachSetStsOrdinary
    // peachSetStsAngry
    // peachSetStsUneasy
    // peachSetStsHappy
    // peachSetStsInvisible
    // peachSetStsAftershower
    // peachSetStsNormal
}

#endif