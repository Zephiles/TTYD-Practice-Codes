#ifndef TTYD_MSGDRV_H
#define TTYD_MSGDRV_H

#include <cstdint>

extern "C"
{
    // selectWindow_Disp
    // selectWindow_Main
    // msgWindow_Disp
    // msgWindow_Clear_Main
    // msgWindow_Main
    // msgWindow_ForceClose
    // msgWindow_Repeat
    // msgWindow_Continue
    // msgWindow_Add
    // msgWindow_Delete
    // msgWindow_Entry
    // msgIconStr2ID
    // msgGetCommand
    bool _ismbblead(const char chr); // Checks if chr is the start of a Japanese character
    const char *msgSearch(const char *key);
    // msg_compare
    // L_msgGetWork
    // msgAnalize
    // msgDispKeyWait_render
    // msgDisp
    // msgMain
    // msgLoad
    // msgInit
}

#endif
