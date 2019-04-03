#pragma once

#include <cstdint>

namespace ttyd::win_item {

extern "C" {

// itemUseDisp2
// itemUseDisp
// item_disp
// winItemDisp
// winItemMain2
// winItemMain
void winItemExit(void *pauseMenuPointer);
// winItemInit2
void winItemInit(void *pauseMenuPointer);
// winMakeSkipList

}

}