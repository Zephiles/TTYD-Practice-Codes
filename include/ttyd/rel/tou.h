#ifndef TTYD_REL_TOU_H
#define TTYD_REL_TOU_H

#include <cstdint>

// Partially taken from the TTYDAP project:
// https://github.com/jamesbrq/TTYDAP/blob/main/rel/include/ttyd/tou.h
enum GlitzPitFighterId
{
    GLITZ_PIT_FIGHTER_ID_RAWK_HAWK = 0,
    GLITZ_PIT_FIGHTER_ID_KOOPINATOR,
    GLITZ_PIT_FIGHTER_ID_CHOMP_COUNTRY,
    GLITZ_PIT_FIGHTER_ID_HAMMA_BAMMA_FLARE,
    GLITZ_PIT_FIGHTER_ID_CRAW_DADDY,
    GLITZ_PIT_FIGHTER_ID_MAGIKOOPA_MASTERS,
    GLITZ_PIT_FIGHTER_ID_FUZZ,
    GLITZ_PIT_FIGHTER_ID_SHELLSHOCKERS,
    GLITZ_PIT_FIGHTER_ID_POKER_FACES,
    GLITZ_PIT_FIGHTER_ID_TINY_SPINIES,
    GLITZ_PIT_FIGHTER_ID_ARMORED_HARRIERS,
    GLITZ_PIT_FIGHTER_ID_HAND_IT_OVERS,
    GLITZ_PIT_FIGHTER_ID_BOB_OMB_SQUAD,
    GLITZ_PIT_FIGHTER_ID_PUNK_ROCKS,
    GLITZ_PIT_FIGHTER_ID_MIND_BOGGLERS,
    GLITZ_PIT_FIGHTER_ID_SPIKE_STORM,
    GLITZ_PIT_FIGHTER_ID_DEAD_BONES,
    GLITZ_PIT_FIGHTER_ID_POKEY_TRIPLETS,
    GLITZ_PIT_FIGHTER_ID_KP_KOOPAS,
    GLITZ_PIT_FIGHTER_ID_GOOMBA_BROS,
    GLITZ_PIT_FIGHTER_ID_MARIO,
    GLITZ_PIT_FIGHTER_ID_WINGS_OF_NIGHT,
    GLITZ_PIT_FIGHTER_ID_DESTRUCTORS,
};

// Only including the valid conditions for now
enum GlitzPitCondition
{
    GLITZ_PIT_CONDITION_DONT_USE_JUMP = 1,
    GLITZ_PIT_CONDITION_DONT_USE_HAMMER = 3,
    GLITZ_PIT_CONDITION_DONT_USE_SPECIAL_MOVES = 5,
    GLITZ_PIT_CONDITION_USE_SPECIAL_MOVE_ONCE = 6,
    GLITZ_PIT_CONDITION_WIN_BEFORE_TAKING_20_DAMAGE = 11,
    GLITZ_PIT_CONDITION_TAKE_DAMAGE_X_TIMES = 18,
    GLITZ_PIT_CONDITION_DONT_USE_ITEMS = 26,
    GLITZ_PIT_CONDITION_DONT_SWITCH_PARTNERS = 32,
    GLITZ_PIT_CONDITION_APPEAL_X_TIMES = 43,
    GLITZ_PIT_CONDITION_DONT_USE_FP = 48,
    GLITZ_PIT_CONDITION_WIN_IN_5_TURNS_OR_LESS = 59,
    GLITZ_PIT_CONDITION_DONT_ATTACK_WITH_MARIO = 63,
    GLITZ_PIT_CONDITION_DONT_ATTACK_WITH_PARTNERS = 64,
    GLITZ_PIT_CONDITION_DONT_ATTACK_FOR_FIRST_3_TURNS = 65,
};

enum GlitzPitFlag
{
    GLITZ_PIT_FLAG_WIN = 1,      // Bit 0: Win flag
    GLITZ_PIT_FLAG_INACTIVE = 2, // Bit 1: Inactive/skip entry
    GLITZ_PIT_FLAG_STOP = 4,     // Bit 2: Stop counting flag
};

struct RankingEntry
{
    uint16_t flags;    // +0x00: Flags
    uint16_t pad1;     // +0x02: Padding
    int32_t fighterId; // +0x04: Fighter ID
    int16_t score;     // +0x08: Score value
    uint16_t pad2;     // +0x0A: Padding
} __attribute__((__packed__));

struct RankingData
{
    int32_t count;         // Number of entries
    RankingEntry *entries; // Array of ranking entries
} __attribute__((__packed__));

static_assert(sizeof(RankingEntry) == 0xC);
static_assert(sizeof(RankingData) == 0x8);

extern "C"
{
    extern RankingData *tou_rank_wp;

    // tou_tou_champion_belt_onoff
    // tou_tou_champion_belt
    // tou_camGetRy
    // tou_fight_money
    // tou_octo_start
    // tou_octo_stop
    // tou_chk
    // tou_getYoshiColor
    // tou_onoff
    // tou_monitor_tev
    // tou_make_monitor
    // tou_gans_tex
    // tou_evt_tou_print_ranking
    // tou_disp_proc
    // tou_printRanking
    // tou_evt_tou_set_rule_result
    // tou_evt_tou_set_ranking_win_lose
    // tou_evt_tou_get_rule_msg2
    // tou_evt_tou_get_rule_info
    // tou_evt_tou_get_fighter_battle_msg
    // tou_evt_tou_get_fighter_rank
    // tou_evt_tou_get_fighter_name2
    // tou_evt_tou_get_fighter_info
    // tou_evt_tou_get_next_match
    // tou_evt_tou_get_ranking
    void tou_rankingControll();
    // tou_insert
    // tou_evt_rankingReInit
    // tou_rankingExit
    // tou_rankingInit
    // tou__unresolved
    // tou__epilog
    // tou__prolog
    // tou_kemuri_move
    // tou_kpa_hikousen_yure
    // tou_npc_set_pos
    // tou_time_shift
    // tou_starstone_set_ry
    // tou_starstone_set_pos
    // tou_starstone_entry
    // tou_yami_oowareru_check
    // tou_delete_npc_regl_evt
    // tou_anim_tevcallback_mobj
    // tou_anim_tevcallback
    // tou_yami_view
    // tou_mario_chk
    // tou_eff_sleep_off
    // tou_eff_sleep_on
    // tou_proj_mtx
    // tou_make_shadow
    // tou_make_shadow_disp
    // tou_proj_on
    // tou_proj_off
    // tou_evt_end_count
    // tou_check_hammer
    // tou_ext_fan_dispent
    // tou_ext_fan_main
    // tou_fan_disp
    // tou_ext_fan_init
    // tou_randf
    // tou_pose_change
    // tou_pose_main
    // tou_pose_init
    // tou_kemuri_stop
    // tou_make_list
    // tou_memoryRY
    // tou_gans_msg_callback
    // tou_iri_13_make_name
    // tou_free_tbl
    // tou_get_pos
    // tou_make_tbl
    // tou_mario_recovery
    // tou_paper_off
    // tou_paper_on
    // tou_get_yoshi_color
    // tou_icon_off
    // tou_getYoshiColor
    // tou_setYoshiColor
    // tou_name_check
    // tou_bgm_start_wait
    // tou_set_dir
    // tou_lect_push_b_check
    // tou_lect_push_a_check
    // tou_lect_mobj_set_nosysblend
    // tou_juyoitem_goato
    // tou_juyoitem_gogo
    // tou_juyoitem_end
    // tou_juyoitem_get_no
    // tou_lect_party_ido_sum
    // tou_lect_party_ido_sub
    // tou_efffukidashichange_cam
    // tou_lect_npc_balloon_on_off
    // tou_lect_itemget_kesan
    // tou_lect_mobj_hit_nasi
    // tou_effitemget_busaiku
    // tou_effitemget_open_ptr
    // tou_effitemget_open
    // tou_effitemgetchange_cam_ptr
    // tou_effitemgetchange_cam
    // tou_lect_map_cam_chg
    // tou_lect_map_cam_chg_sub
    // tou_lecture_button_set_pos
    // tou_lecture_button_seq_wait
    // tou_lecture_button_get_icon_id
    // tou_lecture_button_set_seq
    // tou_lecture_button_flag_on_off
    // tou_lecture_button_set_alpha
    // tou_lecture_button_set_icon_id
    // tou_lecture_button_rtn
    // tou_lecture_button_disp
    // tou_x_flag_wait
    // tou_x_flag_get
    // tou_x_flag_add
    // tou_x_flag_set
    // tou_lect_status_on
    // tou_lect_status_off
    // tou_lect_mario_ido_sum
    // tou_lect_mario_ido_sub
}

#endif
