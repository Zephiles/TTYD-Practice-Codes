#ifndef TTYD_SEQ_MAPCHANGE_H
#define TTYD_SEQ_MAPCHANGE_H

#include <cstdint>

enum RelId : uint32_t
{
    AAA = 1, // Mario's house
    AJI,     // X-Naut Fortress
    BOM,     // Fahr Outpost

#ifndef TTYD_US
    DIG,
#endif

    DMO, // Intro sequence
    DOU, // Pirate's Grotto
    EKI, // Riverside Station
    END, // Ending credits
    GON, // Hooktail Castle
    GOR, // Rogueport (excluding sewers)
    GRA, // Twilight Trail
    HEI, // Petal Meadows
    HOM, // Excess Express traveling cutscenes
    JIN, // Creepy Steeple
    JON, // Pit of 100 Trials
    KPA, // Bowser stages
    LAS, // Palace of Shadow
    MOO, // Moon
    MRI, // The Great Tree
    MUJ, // Keelhaul Key
    NOK, // Petalburg
    PIK, // Poshley Heights

#ifndef TTYD_US
    QIZ,
#endif

    RSH,  // Excess Express
    SYS,  // Chapter start screens/Game Over screen
    TIK,  // Rogueport Sewers
    TOU,  // Glitzville (excluding arena)
    TOU2, // Glitzville arena

#ifdef TTYD_EU
    TST,
#endif

    USU, // Twilight Town
    WIN, // Boggly Woods
    YUU, // Pianta Parlor minigames
};

extern "C"
{
    extern char _next_bero[32];
    extern char _next_map[32];
    extern char NextArea[32];

    // _relUnLoad
    // _load
    void _unload(const char *currentMap, const char *nextMap, const char *nextBero);
    // seq_mapChangeMain
    // seq_mapChangeExit
    // seq_mapChangeInit
}

#endif
