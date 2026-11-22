#ifndef TTYD_EVT_DOOR_H
#define TTYD_EVT_DOOR_H

#include "ttyd/evtmgr.h"

#include <cstdint>

extern "C"
{
    // evt_door_end_wait
    // evt_door_set_param
    // evt_door_data_copy
    // npc_hide_onoff
    // evt_door_load_mapflag
    // evt_door_save_mapflag

    // `evt_door_param` doesn't always use all of the params that it can take
    EVT_DEFINE_USER_FUNC(evt_door_param);

    // door_dark_flag
    // door_dark_flag_sub
    // door_position_sub
    // get_noclip_map
    // npc_light
    // npc_dark
    // animation
    // init_inside_group
    // inside_group
    // door_group
    // outside_group
    // in_pos
    // out_pos
    // inout_hit
    // door_entry
    // snd_door_out
    // snd_door_in
}

#endif
