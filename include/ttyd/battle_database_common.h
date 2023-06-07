#ifndef TTYD_BATTLE_DATABASE_COMMON_H
#define TTYD_BATTLE_DATABASE_COMMON_H

#include "gc/types.h"
#include "ttyd/icondrv.h"

#include <cstdint>

// Taken from Jdaster64's Infinite Pit project:
// https://github.com/jdaster64/ttyd-infinite-pit/blob/master/ttyd-tools/rel/include/ttyd/battle_database_common.h

// All of the following structs are based around the US version of the game, and there may be differences for other versions
struct BattleWeapon
{
    const char *name;
    IconId icon;
    int8_t unk_06[2];
    int32_t item_id;
    const char *description;
    int8_t base_accuracy;
    int8_t base_fp_cost;
    int8_t base_sp_cost;
    int8_t superguards_allowed; // Not sure of difference between 1 and 2
    float unk_14;
    int8_t stylish_multiplier;
    int8_t unk_19;
    int8_t bingo_card_chance;
    int8_t unk_1b;
    void *damage_function;
    uint32_t damage_function_params[8];
    void *fp_damage_function;
    uint32_t fp_damage_function_params[8];
    uint32_t target_class_flags;
    uint32_t target_property_flags;
    int8_t element;
    int8_t damage_pattern; // Used for hit effects (spinning, crushing...)
    int8_t weapon_ac_level;
    int8_t unk_6f;
    const char *ac_help_msg;
    uint32_t special_property_flags;
    uint32_t counter_resistance_flags;
    uint32_t target_weighting_flags;

    int8_t sleep_chance;
    int8_t sleep_time;
    int8_t stop_chance;
    int8_t stop_time;
    int8_t dizzy_chance;
    int8_t dizzy_time;
    int8_t poison_chance;
    int8_t poison_time;

    int8_t poison_strength;
    int8_t confuse_chance;
    int8_t confuse_time;
    int8_t electric_chance;
    int8_t electric_time;
    int8_t dodgy_chance;
    int8_t dodgy_time;
    int8_t burn_chance;

    int8_t burn_time;
    int8_t freeze_chance;
    int8_t freeze_time;
    int8_t size_change_change;
    int8_t size_change_time;
    int8_t size_change_strength;
    int8_t atk_change_chance;
    int8_t atk_change_time;

    int8_t atk_change_strength;
    int8_t def_change_chance;
    int8_t def_change_time;
    int8_t def_change_strength;
    int8_t allergic_chance;
    int8_t allergic_time;
    int8_t ohko_chance;
    int8_t charge_strength;

    int8_t fast_chance;
    int8_t fast_time;
    int8_t slow_chance;
    int8_t slow_time;
    int8_t fright_chance;
    int8_t gale_force_chance;
    int8_t payback_time;
    int8_t hold_fast_time;

    int8_t invisible_chance;
    int8_t invisible_time;
    int8_t hp_regen_time;
    int8_t hp_regen_strength;
    int8_t fp_regen_time;
    int8_t fp_regen_strength;
    int8_t pad_ae[2];

    void *attack_evt_code;
    int8_t bg_a1_a2_fall_weight;
    int8_t bg_a1_fall_weight;
    int8_t bg_a2_fall_weight;
    int8_t bg_no_a_fall_weight;
    int8_t bg_b_fall_weight;
    int8_t nozzle_turn_chance;
    int8_t nozzle_fire_chance;
    int8_t ceiling_fall_chance;
    int8_t object_fall_chance;
    int8_t unused_stage_hazard_chance;
    int8_t pad_be[2];
} __attribute__((__packed__));

struct StatusVulnerability
{
    uint8_t sleep;
    uint8_t stop;
    uint8_t dizzy;
    uint8_t poison;
    uint8_t confuse;
    uint8_t electric;
    uint8_t burn;
    uint8_t freeze;
    uint8_t huge;
    uint8_t tiny;
    uint8_t attack_up;
    uint8_t attack_down;
    uint8_t defense_up;
    uint8_t defense_down;
    uint8_t allergic;
    uint8_t fright;
    uint8_t gale_force;
    uint8_t fast;
    uint8_t slow;
    uint8_t dodgy;
    uint8_t invisible;
    uint8_t ohko;
} __attribute__((__packed__));

struct ItemDropData
{
    int32_t item_id;
    int16_t hold_weight;
    int16_t drop_weight;
} __attribute__((__packed__));

struct BattleUnitKindPart
{
    int32_t index; // One-indexed
    const char *name;
    const char *model_name;
    Vec3 part_offset_pos;
    Vec3 part_hit_base_offset;
    Vec3 part_hit_cursor_base_offset;
    int16_t unk_30;
    int16_t unk_32;
    int16_t base_alpha;
    int16_t pad_36;

    // Default status when spawning a unit of this kind.
    int8_t *defense;
    int8_t *defense_attr;
    uint32_t attribute_flags;
    uint32_t counter_attribute_flags;
    void *pose_table;
} __attribute__((__packed__));

struct BattleUnitKind
{
    int32_t unit_type;
    const char *unit_name;
    int16_t max_hp;
    int16_t max_fp;
    int8_t danger_hp;
    int8_t peril_hp;
    int8_t level;
    int8_t bonus_exp;
    int8_t bonus_coin;
    int8_t bonus_coin_rate;
    int8_t base_coin;
    int8_t run_rate;
    int16_t pb_soft_cap;
    int16_t width;
    int16_t height;
    int16_t hit_offset[2];
    int8_t pad_1e[2];
    Vec3 center_offset;
    int16_t hp_gauge_offset[2];
    Vec3 talk_toge_base_offset;
    Vec3 held_item_base_offset;
    Vec3 burn_flame_offset;
    float unk_54;
    float unk_58;
    Vec3 binta_hit_offset; // Love Slap, other grounded moves
    Vec3 kiss_hit_offset;  // Lip Lock
    Vec3 cut_base_offset;  // Art Attack
    float cut_width;
    float cut_height;
    int8_t turn_order;
    int8_t turn_order_variance;
    int8_t swallow_chance;
    int8_t swallow_attributes;
    int8_t hammer_knockback_chance;
    int8_t itemsteal_param;
    int8_t pad_8e[2];
    Vec3 star_point_disp_offset;
    const char *damage_sfx_name;
    const char *fire_damage_sfx_name;
    const char *ice_damage_sfx_name;
    const char *explosion_damage_sfx_name;

    // Default attribute_flags & status chances.
    uint32_t attribute_flags;
    StatusVulnerability *status_vulnerability;
    int8_t num_parts;
    int8_t pad_b5[3];
    BattleUnitKindPart *parts;
    void *init_evt_code;
    void *data_table;
} __attribute__((__packed__));

static_assert(sizeof(BattleWeapon) == 0xC0);
static_assert(sizeof(StatusVulnerability) == 0x16);
static_assert(sizeof(ItemDropData) == 0x8);
static_assert(sizeof(BattleUnitKindPart) == 0x4C);
static_assert(sizeof(BattleUnitKind) == 0xC4);

#endif
