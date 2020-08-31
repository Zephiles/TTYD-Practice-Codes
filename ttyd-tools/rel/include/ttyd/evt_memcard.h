#pragma once

#include <cstdint>

#include <ttyd/evtmgr.h>

namespace ttyd::evt_memcard {

extern "C" {

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

extern const int32_t evt_memcard_save[]; // Script

}

}