#pragma once

#include <cstdint>

#include <ttyd/evtmgr.h>

namespace ttyd::evt_lecture {

extern "C" {

// lect_juyoitem_rel_get
// lect_juyoitem_rel_set
// lect_cam_load
// lect_cam_save
// lect_test_systemlevel

// lect_set_systemlevel(int level)
EVT_DECLARE_USER_FUNC(lect_set_systemlevel, 1)

}

}