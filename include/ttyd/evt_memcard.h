#ifndef TTYD_EVT_MEMCARD_H
#define TTYD_EVT_MEMCARD_H

#include "ttyd/evtmgr.h"

#include <cstdint>

extern "C"
{
    // memcard_write
    // memcard_file_existance
    // memcard_ipl
    // memcard_header_write
    // memcard_copy
    // memcard_delete
    // memcard_create
    // memcard_format
    // memcard_load
    // memcard_code

    EVT_DECLARE(evt_memcard_save);
}

#endif
