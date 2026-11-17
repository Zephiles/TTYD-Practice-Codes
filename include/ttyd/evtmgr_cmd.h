#ifndef TTYD_EVTMGR_CMD_H
#define TTYD_EVTMGR_CMD_H

#include "ttyd/evtmgr.h"

#include <cstdint>

extern "C"
{
    int32_t evtmgrCmd(EvtEntry *entry);
    int32_t evtGetValue(EvtEntry *entry, int32_t index);
    int32_t evtGetNumber(EvtEntry *entry, int32_t index);
    int32_t evtSetValue(EvtEntry *entry, int32_t index, int32_t value);
    float evtGetFloat(EvtEntry *entry, int32_t index);
    float evtSetFloat(EvtEntry *entry, int32_t index, float value);
    // evt_debug_put_reg
    // evt_brother_evt_id
    // evt_brother_evt
    // evt_inline_evt_id
    // evt_inline_evt
    // evt_run_evt_id
    // evt_run_evt
    // evt_case_end
    // evt_case_and
    // evt_case_or
    // evt_case_flag
    // evt_case_between
    // evt_case_large_equal
    // evt_case_large
    // evt_case_small_equal
    // evt_case_small
    // evt_case_not_equal
    // evt_case_equal
    // evt_if_not_flag
    // evt_if_flag
    // evt_if_large_equal
    // evt_if_small_equal
    // evt_if_large
    // evt_if_small
    // evt_if_not_equal
    // evt_if_equal
    // evt_iff_large_equal
    // evt_iff_small_equal
    // evt_iff_large
    // evt_iff_small
    // evt_iff_not_equal
    // evt_iff_equal
    // evt_if_str_large_equal
    // evt_if_str_small_equal
    // evt_if_str_large
    // evt_if_str_small
    // evt_if_str_not_equal
    // evt_if_str_equal
    // evt_wait_msec
    // evt_while
}

#endif
