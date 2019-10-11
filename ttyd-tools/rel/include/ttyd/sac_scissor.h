#pragma once

#include <ttyd/dispdrv.h>

#include <cstdint>

namespace ttyd::sac_scissor {

extern "C" {

// wait_star_standby
// star_stone_appear
// scissor_end
// scissor_init
// scissor_disp_region
// scissor_offscreen
void scissor_disp_control(ttyd::dispdrv::CameraId cameraId);
// scissor_capture
// scissor_ofscreen_ok
// scissor_damage_sub2
// scissor_damage_sub
// scissor_damage
// scissor_control
uint32_t scissor_timer_main();
// scissor_timer_get
// star_disp2D_sub
// star_disp2D
// star_control
// weaponGetPower_Scissor
// tess
// scissor_tess
// scissor_cross
// scissor_intersection4
// scissor_intersection3
// scissor_intersection2
// scissor_intersection
void *GetScissorPtr();

}

}