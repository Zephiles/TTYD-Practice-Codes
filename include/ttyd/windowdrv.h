#ifndef TTYD_WINDOWDRV_H
#define TTYD_WINDOWDRV_H

#include <cstdint>

extern "C"
{
    // getWakuTexObj
    // windowGetPointer
    // windowCheckID
    // windowDispGX2_Waku_col

    void windowDispGX_Waku_col(uint16_t GXTexMapID,
                               const uint8_t color[4],
                               float posX,
                               float posY,
                               float width,
                               float height,
                               float cornerRadius);

    // windowDispGX_Message
    // _windowDispGX_Message
    // windowDispGX_System
    // windowDispGX_Kanban
    // windowMain
    // windowDeleteID
    // windowDelete
    // windowEntry
    // windowReInit
    void windowTexSetup();
    // windowInit
}

#endif
