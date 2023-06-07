#ifndef TTYD_BATTLE_UNIT_H
#define TTYD_BATTLE_UNIT_H

#include "gc/types.h"
#include "ttyd/battle_database_common.h"

#include <cstdint>

#define TOTAL_BATTLE_UNIT_TYPES 230

// There may be more values for ActorGroupBelong
enum class ActorGroupBelong : uint8_t
{
    kParty = 0,
    kEnemy,
    kExtra,
};

// Taken from NWPlayer123's decomp:
// https://github.com/NWPlayer123/PaperMario2/blob/master/include/battle/battle_unit.h
enum class BattleUnitType : int32_t
{
    kNullUnitKind,
    kUnitGoomba,
    kUnitParagoomba,
    kUnitSpikyGoomba,
    kUnitSpinia,
    kUnitSpania,
    kUnitLordCrumpPrologue,
    kUnitGus,
    kUnitBlooper,
    kUnitBlooperLeftTentacle,
    kUnitBlooperRightTentacle,
    kUnitKoopatrol,
    kUnitMagikoopa,
    kUnitMagikoopaClone,
    kUnitKoopaTroopa,
    kUnitParatroopa,
    kUnitFuzzy,
    kUnitDullBones,
    kUnitBaldCleft,
    kUnitBristle,
    kUnitGoldFuzzy,
    kUnitFuzzyHorde,
    kUnitRedBones,
    kUnitHooktail,
    kUnitDarkPuff,
    kUnitPalePiranha,
    kUnitCleft,
    kUnitPider,
    kUnitXNaut,
    kUnitYux,
    kUnitMiniYux,
    kUnitBeldamChapter2,
    kUnitMarilynChapter2,
    kUnitVivianChapter2,
    kUnitMagnus,
    kUnitXFist,
    kUnitGoombaGlitzville,
    kUnitKpKoopa,
    kUnitKpParatroopa,
    kUnitPokey,
    kUnitLakitu,
    kUnitSpiny,
    kUnitHyperBaldCleft,
    kUnitBobOmb,
    kUnitBandit,
    kUnitBigBandit,
    kUnitRedSpikyBuzzy,
    kUnitShadyKoopa,
    kUnitShadyParatroopa,
    kUnitRedMagikoopa,
    kUnitRedMagikoopaClone,
    kUnitWhiteMagikoopa,
    kUnitWhiteMagikoopaClone,
    kUnitGreenMagikoopa,
    kUnitGreenMagikoopaClone,
    kUnitDarkCraw,
    kUnitHammerBro,
    kUnitBoomerangBro,
    kUnitFireBro,
    kUnitRedChomp,
    kUnitDarkKoopatrol,
    kUnitIronCleftRed,
    kUnitIronCleftGreen,
    kUnitBowserChapter3,
    kUnitRawkHawk,
    kUnitMachoGrubba,
    kUnitHyperGoomba,
    kUnitHyperParagoomba,
    kUnitHyperSpikyGoomba,
    kUnitCrazeeDayzee,
    kUnitAmazyDayzee,
    kUnitHyperCleft,
    kUnitBuzzyBeetle,
    kUnitSpikeTop,
    kUnitSwooper,
    kUnitBoo,
    kUnitAtomicBoo,
    kUnitDooplissChapter4Fight1,
    kUnitDooplissChapter4Invincible,
    kUnitDooplissChapter4Fight2,
    kUnitGoombellaChapter4,
    kUnitKoopsChapter4,
    kUnitYoshiChapter4,
    kUnitFlurrieChapter4,
    kUnitEmber,
    kUnitLavaBubble,
    kUnitGreenFuzzy,
    kUnitFlowerFuzzy,
    kUnitPutridPiranha,
    kUnitParabuzzy,
    kUnitBillBlaster,
    kUnitBulletBill,
    kUnitBulkyBobOmb,
    kUnitCortez,
    kUnitCortezBonePile,
    kUnitCortezSword,
    kUnitCortezHook,
    kUnitCortezRapier,
    kUnitCortezSaber,
    kUnitLordCrumpChapter5,
    kUnitXNautsCrumpFormation1,
    kUnitXNautsCrumpFormation2,
    kUnitXNautsCrumpFormation3,
    kUnitRuffPuff,
    kUnitPoisonPokey,
    kUnitSpikyParabuzzy,
    kUnitDarkBoo,
    kUnitSmorg,
    kUnitSmorgMiasmaTentacleA,
    kUnitSmorgMiasmaTentacleB,
    kUnitSmorgMiasmaTentacleC,
    kUnitSmorgMiasmaClaw,
    kUnitIcePuff,
    kUnitFrostPiranha,
    kUnitMoonCleft,
    kUnitZYux,
    kUnitMiniZYux,
    kUnitXYux,
    kUnitMIniXYux,
    kUnitXNautPhD,
    kUnitEliteXNaut,
    kUnitMagnus20,
    kUnitXPunch,
    kUnitSwoopula,
    kUnitPhantomEmber,
    kUnitBombshellBillBlaster,
    kUnitBombshellBill,
    kUnitChainChomp,
    kUnitDarkWizzerd,
    kUnitDarkWizzerdClone,
    kUnitDryBones,
    kUnitDarkBones,
    kUnitGloomtail,
    kUnitBeldamChapter8,
    kUnitMarilynChapter8,
    kUnitDooplissChapter8,
    kUnitDooplissChapter8Mario,
    kUnitDooplissChapter8Goombella,
    kUnitDooplissChapter8Koops,
    kUnitDooplissChapter8Yoshi,
    kUnitDooplissChapter8Flurrie,
    kUnitDooplissChapter8Vivian,
    kUnitDooplissChapter8Bobbery,
    kUnitDooplissChapter8MsMowz,
    kUnitBowserChapter8,
    kUnitKammyKoopa,
    kUnitGrodus,
    kUnitGrodusX,
    kUnitShadowQueenPeach,
    kUnitShadowQueenInvincible,
    kUnitShadowQueenDemon,
    kUnitShadowQueenLeftRightHand,
    kUnitShadowQueenDeadHands,
    kUnitGloomba,
    kUnitParagloomba,
    kUnitSpikyGloomba,
    kUnitDarkKoopa,
    kUnitDarkParatroopa,
    kUnitBadgeBandit,
    kUnitDarkLakitu,
    kUnitSkyBlueSpiny,
    kUnitWizzerd,
    kUnitPiranhaPlant,
    kUnitSpunia,
    kUnitArantula,
    kUnitDarkBristle,
    kUnitPoisonPuff,
    kUnitSwampire,
    kUnitBobUlk,
    kUnitEliteWizzerd,
    kUnitEliteWizzerdClone,
    kUnitBonetail,
    kUnitUnusedRedBuzzy,
    kUnitUnusedRedParabuzzy,
    kUnitUnusedRedSpikyParabuzzy,
    kUnitUnusedHyperBobOmb,
    kUnitUnusedUltraBobOmb,
    kUnitTutorialGoombella,
    kUnitTutorialFranklyB2,
    kUnitTutorialFranklyB3,
    kUnitTutorialFranklyB4,
    kUnitEpilogueDooplissMario,
    kUnitEpilogueFlurrie,
    kUnitEpilogueBoo,
    kUnitEpilogueAtomicBoo,
    kUnitEpilogueMaleToad,
    kUnitEpilogueFemaleToad,
    kUnitUnusedTest,
    kUnitUnusedCrump2,
    kUnitUnusedBeldam2,
    kUnitUnusedMarilyn2,
    kUnitUnusedVivian2,
    kUnitUnusedBeldam3,
    kUnitUnusedMarilyn3,
    kUnitUnusedMechaKuri,
    kUnitUnusedMechaKame,
    kUnitUnusedOkorl,
    kUnitUnusedYowarl,
    kUnitUnusedTuyonarl,
    kUnitUnusedWanawana,
    kUnitUnusedMinaraiKamec,
    kUnitUnusedShyGuy,
    kUnitUnusedGrooveGuy,
    kUnitUnusedPyroGuy,
    kUnitUnusedSpyGuy,
    kUnitUnusedAntiGuy,
    kUnitUnusedBzzap,
    kUnitUnusedMiniBzzap,
    kUnitUnusedUfo,
    kUnitUnusedPennington,
    kUnitUnusedFighter,
    kUnitUnusedZessT,
    kUnitUnusedMaster,
    kUnitUnusedReporter,
    kUnitUnusedHotdogMaster,
    kUnitUnusedFlavio,
    kUnitUnusedTree,
    kUnitUnusedSwitch,
    kUnitUnusedTestnpc,
    kUnitBombSquadBomb,
    kUnitSystem,
    kUnitPrologueGoombella,
    UNIT_MARIO,
    kUnitShellShield,
    kUnitGoombella,
    kUnitKoops,
    kUnitYoshi,
    kUnitFlurrie,
    kUnitVivian,
    kUnitBobbery,
    kUnitMsMowz,
};

// Taken from Jdaster64's Infinite Pit project:
// https://github.com/jdaster64/ttyd-infinite-pit/blob/master/ttyd-tools/rel/include/ttyd/battle_unit.h

// All of the following structs are based around the US version of the game, and there may be differences for other versions
struct MovementSoundControl
{
    const char *sound_name_left;
    const char *sound_name_right;
    int16_t unk_08;
    int16_t unk_0a;
    int16_t initial_wait_timer;
    int16_t repeat_wait_timer_left;
    int16_t repeat_wait_timer_right;
    int16_t current_timer;
    int16_t sound_to_play_next;
    int16_t pad_16;
    int32_t psnd_sfx_idx;
} __attribute__((__packed__));

struct MovementSoundData
{
    const char *jump_sound_left;
    const char *jump_sound_right;
    const char *run_sound_left;
    const char *run_sound_right;
    int16_t run_initial_wait_timer;
    int16_t run_repeat_wait_timer_left;
    int16_t run_repeat_wait_timer_right;
    int16_t pad_16;
    const char *walk_sound_left;
    const char *walk_sound_right;
    int16_t walk_initial_wait_timer;
    int16_t walk_repeat_wait_timer_left;
    int16_t walk_repeat_wait_timer_right;
    int16_t pad_26;
    const char *dive_sound_left;
    const char *dive_sound_right;
    int16_t dive_initial_wait_timer;
    int16_t dive_repeat_wait_timer_left;
    int16_t dive_repeat_wait_timer_right;
    int16_t pad_36;
} __attribute__((__packed__));

struct MovementParams
{
    Vec3 move_start_position;
    Vec3 move_current_position;
    Vec3 move_target_position;
    int32_t move_frame_count;
    float move_speed_xz;
    float fall_accel;
    float move_speed_y;
    float move_angle_xz;
    float move_dist_xz;
    float jump_offset_height;
    int8_t move_direction;
    int8_t face_direction;
    int8_t pad_42[2];
    MovementSoundControl sound_control_work;

    // Jump parameters used in btlevtcmd_MarioJumpPosition.
    Vec3 jump_move_per_frame;
    float jump_angle_perturbance_angle;
    float jump_angle_perturbance_angle_delta;
    float jump_current_position_y; // ?
    int8_t unk_78[4];
    MovementSoundData sound_data_table;
} __attribute__((__packed__));

struct BadgesEquipped
{
    int8_t close_call;
    int8_t pretty_lucky;
    int8_t lucky_day;
    int8_t unk_03; // Probably unused
    int8_t power_plus;
    int8_t p_up_d_down;
    int8_t all_or_nothing;
    int8_t mega_rush;

    int8_t power_rush;
    int8_t p_down_d_up;
    int8_t double_pain;
    int8_t last_stand;
    int8_t defend_plus;
    int8_t damage_dodge;
    int8_t happy_heart;
    int8_t happy_flower;

    int8_t return_postage;
    int8_t hp_plus;
    int8_t fp_plus;
    int8_t double_dip;
    int8_t triple_dip;
    int8_t flower_saver;
    int8_t feeling_fine;
    int8_t zap_tap;

    int8_t pity_flower;
    int8_t hp_drain;
    int8_t fp_drain;
    int8_t refund;
    int8_t charge;
    int8_t super_charge;
    int8_t unused_defend_command_badge;
    int8_t jumpman;

    int8_t hammerman;
    int8_t ice_power;
    int8_t spike_shield;
    int8_t super_appeal;
    int8_t lucky_start;
    int8_t simplifier;
    int8_t unsimplifier;
    int8_t auto_command_badge;
} __attribute__((__packed__));

struct BattleWorkUnit;

struct BattleWorkUnitPart
{
    BattleWorkUnitPart *next_part;
    BattleUnitKindPart *kind_part_params;
    const char *part_name;
    Vec3 home_position;
    Vec3 position;
    Vec3 position_offset;
    Vec3 display_offset;
    Vec3 base_rotation;
    Vec3 rotation;
    Vec3 rotation_offset;
    Vec3 scale;
    Vec3 base_scale;
    float unk_078; // some sort of scale

    MovementParams movement_params;
    uint32_t parts_work[16];       // length unknown; at least 3
    Vec3 hit_base_position;        // ?
    Vec3 hit_offset;
    Vec3 hit_cursor_base_position; // ?
    Vec3 hit_cursor_offset;
    int16_t addl_target_offset_x;
    int16_t unk_1a2;
    int16_t unk_1a4;
    int8_t unk_1a6[6];

    uint32_t part_attribute_flags;
    uint32_t counter_attribute_flags;
    int8_t *defense;
    int8_t *defense_attr;

    void *pose_table;
    int32_t anim_pose_type;
    char anim_pose_name[64]; // length unknown
    uint32_t unk_204;        // anim-related flags
    float unk_208;
    float anim_motion_speed;
    void *unk_210;  // some sort of anim-related callback fn
    int8_t unk_214;
    int8_t unk_215; // flags related to anim
    int8_t pad_216[2];
    uint32_t base_blur_color;
    uint32_t blur_flags;
    uint32_t blur_colors[2];
    int8_t blur_params[0x44 * 10];
    int8_t pose_sound_params[0x18];

    int32_t unk_4e8;
    BattleWorkUnit *unit_owner;
    uint32_t color;
    uint32_t blended_color; // with base alpha, invis status, etc.
    float unk_4f8;          // some sort of z-offset?  Used in btlDispMain
    int8_t unk_4fc;         // axis order to apply rotations?
    int8_t unk_4fd[3];
} __attribute__((__packed__));

struct BattleWorkUnit
{
    int32_t unit_id;
    BattleUnitType true_kind;
    BattleUnitType current_kind;
    int8_t alliance;
    int8_t level;
    int8_t group_index;
    int8_t pad_00f;
    BattleUnitKind *unit_kind_params;
    BattleWorkUnitPart *parts;
    void *data_table;
    uint32_t unit_flags;
    int8_t move_state; // ?
    int8_t max_move_count;
    int8_t moves_remaining;
    int8_t max_moves_this_turn;
    int8_t active_turns; // ? Used to make Slow status skip turns
    int8_t swallow_chance;
    int8_t swallow_attributes;
    int8_t pad_027;
    int32_t move_priority;
    int32_t attack_phase;

    Vec3 home_position;
    Vec3 position;
    Vec3 offset_position;
    Vec3 display_offset;
    Vec3 base_rotation;
    Vec3 rotation;
    Vec3 rotation_offset;
    Vec3 center_offset;
    Vec3 scale;
    Vec3 base_scale;
    Vec3 toge_offset; // Used for speech bubble spike?
    Vec3 held_item_base_offset;
    Vec3 possession_item_offset;
    int16_t width;
    int16_t height;
    int16_t status_icon_offset[2];
    int16_t hp_gauge_offset[2];
    Vec3 cut_base_offset;
    float cut_width;
    float cut_height;
    Vec3 binta_hit_offset;
    Vec3 kiss_hit_offset;

    uint32_t attribute_flags;
    int16_t max_hp;
    int16_t base_max_hp; // No badges
    int16_t current_hp;
    int16_t max_fp;
    int16_t base_max_fp; // No badges
    int16_t current_fp;
    float unk_scale;

    int8_t sleep_turns;
    int8_t unk_119; // Something to do with waking up from sleep?
    int8_t stop_turns;
    int8_t dizzy_turns;
    int8_t poison_turns;
    int8_t poison_strength;
    int8_t confusion_turns;
    int8_t electric_turns;

    int8_t dodgy_turns;
    int8_t burn_turns;
    int8_t freeze_turns;
    int8_t size_change_turns;
    int8_t size_change_strength;
    int8_t attack_change_turns;
    int8_t attack_change_strength;
    int8_t defense_change_turns;

    int8_t defense_change_strength;
    int8_t charge_strength;
    int8_t allergic_turns;
    int8_t flipped_turns; // For shelled enemies, etc.
    int8_t invisible_turns;
    int8_t payback_turns;
    int8_t hold_fast_turns;
    int8_t fast_turns;

    int8_t slow_turns;
    int8_t hp_regen_turns;
    int8_t hp_regen_strength;
    int8_t fp_regen_turns;
    int8_t fp_regen_strength;
    int8_t knocked_out; // OHKO strength?
    int8_t unk_136[2];
    uint32_t status_flags;

#ifndef TTYD_JP
    int8_t unk_13c[4];
#endif

    uint32_t protect_unit_idx; // Related to Shell Shield?
    StatusVulnerability *status_vulnerability;
    MovementParams movement_params;
    int8_t hp_gauge_params[0x1C];
    uint32_t unit_work[16];

    // Parameters used during a single "act" (attack).
    BattleWorkUnitPart *currently_targeted_part;
    uint32_t unk_25c;               // Unreferenced?
    int16_t hp_damaging_hits_dealt; // Used for Power Bounce decay, etc.
    int16_t pad_262;
    int32_t total_hp_damage_taken;
    int32_t total_fp_damage_taken;
    int32_t total_fp_lost;
    int8_t hp_damage_taken;
    int8_t fp_damage_taken;
    int8_t fp_lost;
    int8_t hits_taken;
    uint32_t damage_pattern;
    uint32_t damage_code;
    uint32_t token_flags;
    int32_t hits_dealt_this_attack;         // Used for Fire Drive decay, etc.
    int32_t total_damage_dealt_this_attack; // Used for Drain badges

    int32_t init_evt_tid;
    void *wait_evt_code;
    int32_t wait_evt_tid;
    void *unison_phase_evt_code;
    void *phase_evt_code;
    int32_t phase_evt_tid;
    void *attack_evt_code;
    void *confuse_evt_code;
    int32_t attack_evt_tid;
    uint32_t unk_2ac; // Battle menu state?
    void *damage_evt_code;
    int32_t damage_evt_tid;
    void *entry_evt_code;
    int32_t entry_evt_tid;
    void *ceiling_fall_evt_code;
    int32_t ceiling_fall_evt_tid;
    void *unknown_evt_code;
    int32_t unknown_evt_tid;

#ifdef TTYD_EU
    int8_t unk_2d0[4];
#endif

    void *hit_evt_code; // ? BattleRunHitEventDirect
    uint32_t talk_body_part_id;
    const char *talk_pose_name;
    const char *stay_pose_name;

    BadgesEquipped badges_equipped;
    int32_t held_item;
    ItemDropData *held_item_table;

    int8_t misc_310[0x824];
} __attribute__((__packed__));

static_assert(sizeof(MovementSoundControl) == 0x1C);
static_assert(sizeof(MovementSoundData) == 0x38);
static_assert(sizeof(MovementParams) == 0xB4);
static_assert(sizeof(BadgesEquipped) == 0x28);
static_assert(sizeof(BattleWorkUnitPart) == 0x500);

#ifdef TTYD_JP
static_assert(sizeof(BattleWorkUnit) == 0xB30);
#elif defined TTYD_US
static_assert(sizeof(BattleWorkUnit) == 0xB34);
#elif defined TTYD_EU
static_assert(sizeof(BattleWorkUnit) == 0xB38);
#endif

inline bool battleUnitTypeIsValid(BattleUnitType type)
{
    return (type > BattleUnitType::kNullUnitKind) && (type <= BattleUnitType::kUnitMsMowz);
}

extern "C"
{
    // BtlUnit_CheckShadowGuard
    // BtlUnit_EnemyItemCanUseCheck
    // BtlUnit_HpGaugeMain
    // BtlUnit_HpGaugeInit
    // BtlUnit_snd_se_pos
    // BtlUnit_snd_se
    // BtlUnit_ControlPoseSoundMain
    // BtlUnit_PoseSoundInit
    // BtlUnit_SetCommandAnimPose
    // BtlUnit_SetSeMode
    // BtlUnit_LoadSeMode
    // BtlUnit_ResetMoveStatus
    // BtlUnit_GetGuardKouraPtr
    // BtlUnit_PayWeaponCost
    // BtlUnit_CheckWeaponCost
    // BtlUnit_GetWeaponCost
    // BtlUnit_SetMaxFp
    // BtlUnit_GetMaxFp
    // BtlUnit_RecoverFp
    // BtlUnit_RecoverHp
    // BtlUnit_SetFp
    // BtlUnit_GetFp
    // BtlUnit_GetCoin
    // BtlUnit_GetExp
    // BtlUnit_CheckPinchStatus
    // BtlUnit_SetParamToPouch
    // BtlUnit_ReviseHpFp
    // BtlUnit_SetParamFromPouch
    // BtlUnit_CanActStatus
    // BtlUnit_CanGuardStatus
    // BtlUnit_CheckData
    // BtlUnit_GetData
    // BtlUnit_GetACPossibility
    // BtlUnit_SetTotalHitDamage
    // BtlUnit_GetTotalHitDamage
    // BtlUnit_GetHitDamage
    // BtlUnit_GetEnemyBelong
    // BtlUnit_GetTalkTogePos
    // BtlUnit_ChangeStayAnim
    // BtlUnit_ChangeTalkAnim
    // BtlUnit_SetBodyAnim
    // BtlUnit_SetBodyAnimType
    // BtlUnit_SetAnim
    // BtlUnit_SetAnimType
    // BtlUnit_GetPoseNameFromType
    // BtlUnit_OffUnitFlag
    // BtlUnit_OnUnitFlag
    // BtlUnit_CheckUnitFlag
    // BtlUnit_OffStatusFlag
    // BtlUnit_OnStatusFlag
    // BtlUnit_CheckStatusFlag
    bool BtlUnit_CheckStatus(BattleWorkUnit *battleUnitPtr, uint32_t status);
    // BtlUnit_CheckRecoveryStatus
    // BtlUnit_ClearStatus
    // BtlUnit_SetStatus
    // BtlUnit_GetStatus
    // _CheckMoveCount
    ActorGroupBelong BtlUnit_GetBelong(BattleWorkUnit *battleUnitPtr);
    // BtlUnit_SetJumpSpeed
    // BtlUnit_SetPartsMoveSpeed
    // BtlUnit_SetMoveSpeed
    // BtlUnit_SetPartsFallAccel
    // BtlUnit_SetFallAccel
    // BtlUnit_SetPartsMoveTargetPos
    // BtlUnit_SetMoveTargetPos
    // BtlUnit_SetPartsMoveCurrentPos
    // BtlUnit_SetMoveCurrentPos
    // BtlUnit_SetPartsMoveStartPos
    // BtlUnit_SetMoveStartPos
    // BtlUnit_AddPartsDispOffset
    // BtlUnit_SetPartsDispOffset
    // BtlUnit_SetDispOffset
    // BtlUnit_AddPartsOffsetPos
    // BtlUnit_SetPartsOffsetPos
    // BtlUnit_GetPartsOffsetPos
    // BtlUnit_SetOffsetPos
    // BtlUnit_SetHeight
    // BtlUnit_GetHeight
    // BtlUnit_GetWidth
    // BtlUnit_AddPartsScale
    // BtlUnit_SetPartsScale
    // BtlUnit_SetPartsBaseScale
    // BtlUnit_AddScale
    // BtlUnit_SetScale
    // BtlUnit_GetScale
    // BtlUnit_SetBaseScale
    // BtlUnit_AddPartsRotateOffset
    // BtlUnit_SetPartsRotateOffset
    // BtlUnit_SetRotateOffset
    // BtlUnit_GetPartsBaseRotate
    // BtlUnit_SetPartsBaseRotate
    // BtlUnit_GetBaseRotate
    // BtlUnit_SetBaseRotate
    // BtlUnit_AddPartsRotate
    // BtlUnit_GetPartsRotate
    // BtlUnit_SetPartsRotate
    // BtlUnit_AddRotate
    // BtlUnit_GetRotate
    // BtlUnit_SetRotate
    // BtlUnit_SetPartsHomePos
    // BtlUnit_AddHomePos
    // BtlUnit_SetHomePos
    // BtlUnit_GetHomePos
    // BtlUnit_SetHitCursorOffset
    // BtlUnit_SetHitOffset
    // BtlUnit_GetHitPos
    // BtlUnit_GetPartsWorldPos
    // BtlUnit_AddPartsPos
    // BtlUnit_SetPartsPos
    // BtlUnit_GetPartsPos
    // BtlUnit_AddPos
    // BtlUnit_SetPos
    void BtlUnit_GetPos(BattleWorkUnit *battleUnitPtr, float *posXOut, float *posYOut, float *posZOut);
    int32_t BtlUnit_GetBodyPartsId(BattleWorkUnit *battleUnitPtr);
    BattleWorkUnitPart *BtlUnit_GetPartsPtr(BattleWorkUnit *battleUnitPtr, int32_t bodyPartsId);
    // BtlUnit_GetUnitId
    // BtlUnit_Spawn
    // BtlUnit_Delete
    // BtlUnit_Entry
    // BtlUnit_Init
}

#endif
