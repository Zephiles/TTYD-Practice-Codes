#ifndef TTYD_WIN_ITEM_H
#define TTYD_WIN_ITEM_H

#include "winmgr.h"

#include <cstdint>

extern "C"
{
    extern WinMgrDesc win_item_window_desc[2];

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
