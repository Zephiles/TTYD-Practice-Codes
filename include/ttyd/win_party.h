#ifndef TTYD_WIN_PARTY_H
#define TTYD_WIN_PARTY_H

#include <cstdint>

extern "C"
{
    // winPartyDisp
    // winPartyMain2
    // winPartyMain
    void winPartyExit(void *pauseMenuPtr);
    // winPartyInit2
    void winPartyInit(void *pauseMenuPtr);
}

#endif
