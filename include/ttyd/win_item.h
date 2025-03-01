#ifndef TTYD_WIN_ITEM_H
#define TTYD_WIN_ITEM_H

#include <cstdint>

extern "C"
{
    // itemUseDisp2
    // itemUseDisp
    // item_disp
    // winItemDisp
    // winItemMain2
    // winItemMain
    void winItemExit(void *pauseMenuPtr);
    // winItemInit2
    void winItemInit(void *pauseMenuPtr);
    // winMakeSkipList
}

#endif
