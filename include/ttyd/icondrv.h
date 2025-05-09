#ifndef TTYD_ICONDRV_H
#define TTYD_ICONDRV_H

#include "gc/types.h"

#include <cstdint>

enum class IconId : int16_t
{
    ICON_INVALID_ITEM_ZAP_TAP = 0,
    ICON_MONEY_MONEY,
    ICON_W_EMBLEM,
    ICON_L_EMBLEM,
    ICON_FLOWER_FINDER,
    ICON_FP_PLUS,
    ICON_FP_DRAIN,
    ICON_FEELING_FINE,
    ICON_HEART_FINDER,
    ICON_HP_PLUS,
    ICON_HP_DRAIN_BADGE,
    ICON_QUAKE_HAMMER,
    ICON_MEGA_QUAKE,
    ICON_PIERCING_BLOW,
    ICON_HAMMER_THROW,
    ICON_FIRE_DRIVE,
    ICON_HEAD_RATTLE,
    ICON_HAPPY_FLOWER,
    ICON_ITEM_HOG,
    ICON_ICE_POWER,
    ICON_DOUBLE_PAIN,
    ICON_ATTACK_FX_G,
    ICON_ATTACK_FX_B,
    ICON_ATTACK_FX_R,
    ICON_ATTACK_FX_P,
    ICON_ATTACK_FX_Y,
    ICON_CHILL_OUT,
    ICON_PRETTY_LUCKY,
    ICON_SPIKE_SHIELD,
    ICON_FIRST_ATTACK,
    ICON_BUMP_ATTACK,
    ICON_MULTIBOUNCE,
    ICON_POWER_BOUNCE,
    ICON_TORNADO_JUMP,
    ICON_SHRINK_STOMP,
    ICON_SLEEPY_STOMP,
    ICON_SOFT_STOMP,
    ICON_CHARGE,
    ICON_SUPER_CHARGE,
    ICON_DOUBLE_DIP,
    ICOn_TRIPLE_DIP,
    ICON_QUICK_CHANGE,
    ICON_JUMPMAN,
    ICON_HAMMERMAN,
    ICON_UNKNOWN_BADGE_DAMAGE_DODGE,
    ICON_HAPPY_HEART,
    ICON_FLOWER_SAVER,
    ICON_POWER_PLUS,
    ICON_DEFEND_PLUS,
    ICON_DAMAGE_DODGE,
    ICON_PEEKABOO,
    ICON_TIMING_TUTOR,
    ICON_P_DOWN_D_UP,
    ICON_P_UP_D_DOWN,
    ICON_ALL_OR_NOTHING,
    ICON_MEGA_RUSH,
    ICON_LAST_STAND,

#ifdef TTYD_JP
    ICON_POWER_RUSH,
#endif

    ICON_CLOSE_CALL,
    ICON_LUCKY_DAY,
    ICON_PITY_FLOWER,
    ICON_UNUSED_BADGE_ICE,
    ICON_REFUND,
    ICON_LUCKY_START,
    ICON_RETURN_POSTAGE,
    ICON_SLOW_GO,
    ICON_SIMPLIFIER,
    ICON_UNSIMPLIFIER,
    ICON_MEGA_JUMP,
    ICON_POWER_JUMP,
    ICON_POWER_SMASH,
    ICON_MEGA_SMASH,
    ICON_SUPER_APPEAL,
    ICON_ICE_SMASH,

#ifndef TTYD_JP
    ICON_POWER_RUSH, // Changes depending on the current language being used
    ICON_POWER_RUSH_D,
    ICON_POWER_RUSH_K,
    ICON_POWER_RUSH_A,
    ICON_POWER_RUSH_C,
#endif

    ICON_P_UP_D_DOWN_P,
    ICON_P_DOWN_D_UP_P,
    ICON_FEELING_FINE_P,
    ICON_LUCKY_DAY_P,
    ICON_PRETTY_LUCKY_P,

#ifdef TTYD_JP
    ICON_POWER_RUSH_P,
#endif

    ICON_CLOSE_CALL_P,
    ICON_LAST_STAND_P,
    ICON_FP_DRAIN_P,
    ICON_DEFEND_PLUS_P,
    ICON_UNKNOWN_BADGE_DAMAGE_DODGE_P,
    ICON_HP_PLUS_P,
    ICON_DAMAGE_DODGE_P,
    ICON_HP_DRAIN_P,
    ICON_HAPPY_HEART_P,
    ICON_FLOWER_SAVER_P,
    ICON_POWER_PLUS_P,
    ICON_HAPPY_FLOWER_P,
    ICON_CHARGE_P,
    ICON_SUPER_CHARGE_P,
    ICON_SUPER_APPEAL_P,
    ICON_MEGA_RUSH_P,
    ICON_ALL_OR_NOTHING_P,
    ICON_DOUBLE_DIP_P,
    ICOn_TRIPLE_DIP_P,
    ICON_PITY_FLOWER_P,

#ifndef TTYD_JP
    ICON_POWER_RUSH_P, // Changes depending on the current language being used
    ICON_POWER_RUSH_P_D,
    ICON_POWER_RUSH_P_K,
    ICON_POWER_RUSH_P_A,
    ICON_POWER_RUSH_P_C,
#endif

    ICON_BUTTON_A_UNPRESSED,
    ICON_BUTTON_A_PRESSED,
    ICON_BUTTON_B_UNPRESSED,
    ICON_BUTTON_B_PRESSED,
    ICON_BUTTON_X_UNPRESSED,
    ICON_BUTTON_X_PRESSED,
    ICON_BUTTON_Y_UNPRESSED,
    ICON_BUTTON_Y_PRESSED,
    ICON_STICK_C_HELD_DOWN_LEFT,
    ICON_STICK_C_HELD_DOWN,
    ICON_STICK_C_HELD_DOWN_RIGHT,
    ICON_STICK_C_HELD_LEFT,
    ICON_STICK_C_UNHELD,
    ICON_STICK_C_HELD_RIGHT,
    ICON_STICK_C_HELD_UP_LEFT,
    ICON_STICK_C_HELD_UP,
    ICON_STICK_C_HELD_UP_RIGHT,
    ICON_STICK_ANALOG_HELD_DOWN_LEFT,
    ICON_STICK_ANALOG_HELD_DOWN,
    ICON_STICK_ANALOG_HELD_DOWN_RIGHT,
    ICON_STICK_ANALOG_HELD_LEFT,
    ICON_STICK_ANALOG_UNHELD,
    ICON_STICK_ANALOG_HELD_RIGHT,
    ICON_STICK_ANALOG_HELD_UP_LEFT,
    ICON_STICK_ANALOG_HELD_UP,
    ICON_STICK_ANALOG_HELD_UP_RIGHT,
    ICON_BUTTON_L_UNPRESSED,
    ICON_BUTTON_L_PRESSED,
    ICON_BUTTON_R_UNPRESSED,
    ICON_BUTTON_R_PRESSED,
    ICON_BUTTON_Z_UNPRESSED,
    ICON_BUTTON_Z_PRESSED,
    ICON_BUTTON_START_UNPRESSED,
    ICON_BUTTON_START_PRESSED,
    ICON_BUTTON_D_PAD_UP_UNPRESSED,
    ICON_BUTTON_D_PAD_UP_PRESSED,
    ICON_BUTTON_D_PAD_DOWN_UNPRESSED,
    ICON_BUTTON_D_PAD_DOWN_PRESSED,
    ICON_BUTTON_D_PAD_UP_DOWN_UNPRESSED,
    ICON_BUTTON_D_PAD_UP_DOWN_PRESSED,
    ICON_UNKNOWN_BLUE_BAR,
    ICON_UNKNOWN_GREEN_BAR,
    ICON_UNKNOWN_BROWN_SQUARE,
    ICON_UNKNOWN_BROWN_HALF_CIRCLE_LEFT,
    ICON_UNKNOWN_BROWN_HALF_CIRCLE_RIGHT,
    ICON_BATTLE_BLUE_CIRCLE,
    ICON_ACTION_COMMAND_GREEN_CIRCLE,
    ICON_ACTION_COMMAND_YELLOW_CIRCLE,
    ICON_ACTION_COMMAND_ORANGE_CIRCLE,
    ICON_ACTION_COMMAND_RED_CIRCLE_STAR,
    ICON_BATTLE_RUN_METER,
    ICON_REGEN_HP,
    ICON_REGEN_FP,
    ICON_MARIO_UNDER_GROUND_VEIL,
    ICON_UNKNOWN_BATTLE_EXCLAMATION_MARK,
    ICON_STRANGE_SACK,
    ICON_NORMAL_HANMER,
    ICON_SUPER_HANMER,
    ICON_ULTRA_HANMER,
    ICON_NORMAL_BOOTS,
    ICON_SUPER_BOOTS,
    ICON_ULTRA_BOOTS,
    ICON_UNUSED_GREEN_BOOTS,
    ICON_PLANE_MODE_CURSE,
    ICON_BOAT_MODE_CURSE,
    ICON_PAPER_MODE_CURSE,
    ICON_TUBE_MODE_CURSE,
    ICON_COCONUT_BOMB,
    ICON_DRIED_BOUQUET,
    ICON_ZESS_FRAPPE,
    ICON_ICICLE_POP,
    ICON_SHROOM_FRY,
    ICON_SHROOM_ROAST,
    ICON_SHROOM_STEAK,
    ICON_ZESS_DELUXE,
    ICON_UNUSED_COCONUT_DISH,
    ICON_MISTAKE,
    ICON_SPICY_SOUP,
    ICON_SPACE_FOOD,
    ICON_SNOW_BUNNY,
    ICON_UNUSED_SNOW_BUNNY,
    ICON_HONEY_SHROOM,
    ICON_MAPLE_SHROOM,
    ICON_JELLY_SHROOM,
    ICON_HONEY_SUPER,
    ICON_MAPLE_SUPER,
    ICON_JELLY_SUPER,
    ICON_HONEY_ULTRA,
    ICON_MAPLE_ULTRA,
    ICON_JELLY_ULTRA,
    ICON_ZESS_TEA,
    ICON_ZESS_DINNER,
    ICON_ZESS_SPECIAL,
    ICON_ZESS_DYNAMITE,
    ICON_COURAGE_MEAL,
    ICON_SPITE_POUCH,
    ICON_MYSTIC_EGG,
    ICON_TURTLEY_LEAF,
    ICON_KEEL_MANGO,
    ICON_FRESH_PASTA,
    ICON_GOLDEN_LEAF,
    ICON_COCO_CANDY,
    ICON_HONEY_CANDY,
    ICON_JELLY_CANDY,
    ICON_HEALTHY_SALAD,
    ICON_METEOR_MEAL,
    ICON_INK_PASTA,
    ICON_KOOPASTA,
    ICON_SHROOM_BROTH,
    ICON_HEARTFUL_CAKE,
    ICON_OMELETTE_MEAL,
    ICON_PEACH_TART,
    ICON_TRIAL_STEW,
    ICON_INKY_SAUCE,
    ICON_SHROOM_CREPE,
    ICON_POISON_SHROOM,
    ICON_FRESH_JUICE,
    ICON_KOOPA_TEA,
    ICON_HOT_SAUCE,
    ICON_SHROOM_CAKE,
    ICON_MANGO_DELIGHT,
    ICON_FRIED_EGG,
    ICON_CAKE_MIX,
    ICON_MOUSSE_CAKE,
    ICON_PEACHY_PEACH,
    ICON_FRUIT_PARFAIT,
    ICON_SPAGHETTI,
    ICON_CHOCO_CAKE,
    ICON_EGG_BOMB,
    ICON_HORSETAIL,
    ICON_KOOPA_BUN,
    ICON_ZESS_COOKIE,
    ICON_ELECTRO_POP,
    ICON_FIRE_POP,
    ICON_SPICY_PASTA,
    ICON_LOVE_PUDDING,
    ICON_COUPLES_CAKE,
    ICON_BLUE_KEY,
    ICON_RED_KEY,
    ICON_GROTTO_KEY,
    ICON_STATION_KEY_1,
    ICON_STATION_KEY_2,
    ICON_CASTLE_KEY,
    ICON_HOUSE_KEY,
    ICON_STORAGE_KEY,
    ICON_ELEVATOR_KEY_RIVERSIDE,
    ICON_STEEPLE_KEY_PURPLE,
    ICON_STEEPLE_KEY_WHITE,
    ICON_ELEVATOR_KEY_BLUE,
    ICON_ELEVATOR_KEY_RED,
    ICON_CARD_KEY_RED,
    ICON_CARD_KEY_YELLOW,
    ICON_CARD_KEY_GREEN,
    ICON_CARD_KEY_BLUE,
    ICON_PALACE_KEY_WHITE,
    ICON_PALACE_KEY_BLUE,
    ICON_CAKE,
    ICON_POISONED_CAKE,
    ICON_SHINE_SPRITE,
    ICON_CONTACT_LENS,
    ICON_SKULL_GEM,
    ICON_OLD_LETTER,
    ICON_RAGGED_DIARY,
    ICON_COG,
    ICON_HOT_DOG,
    ICON_SUPERBOMBOMB,
    ICON_BRIEFCASE,
    ICON_MAILBOX_SP,
    ICON_BLANKET,

#ifdef TTYD_JP
    ICON_THE_LETTER_P,
#endif

    ICON_GALLEY_POT,
    ICON_NECKLACE,
    ICON_VITAL_PAPER,
    ICON_SHELL_EARRINGS,
    ICON_PUNI_ORB,
    ICON_GOLD_RING,
    ICON_WEDDING_RING_LARGE,
    ICON_WEDDING_RING_SMALL,
    ICON_SUPER_LUIGI_1,
    ICON_SUPER_LUIGI_2,
    ICON_SUPER_LUIGI_3,
    ICON_SUPER_LUIGI_4,
    ICON_SUPER_LUIGI_5,
    ICON_AUTOGRAPH,
    ICON_GATE_HANDLE,
    ICON_INN_COUPON,
    ICON_BLIMP_TICKET,
    ICON_TRAIN_TICKET,
    ICON_ULTRA_STONE,
    ICON_CHUCKOLA_COLA,
    ICON_COCONUT,
    ICON_CHAMPS_BELT,
    ICON_MOON_STONE,
    ICON_SUN_STONE,
    ICON_GOLDBOB_GUIDE,
    ICON_UNUSED_GOLDBOB_GUIDE_RED,
    ICON_UNUSED_GOLDBOB_GUIDE_BLUE,
    ICON_BOWSER_UPGRADE_MEAT,
    ICON_COOKBOOK,

#ifndef TTYD_JP
    ICON_THE_LETTER_P,
#endif

    ICON_BOX,
    ICON_BATTLE_TRUNKS,
    ICON_ROUTING_SLIP,
    ICON_WRESTLING_MAG,
    ICON_PRESENT,
    ICON_BLACK_KEY,
    ICON_MAGICAL_MAP_SMALL,
    ICON_MAGICAL_MAP_LARGE,
    ICON_SHADOW_SIRENS_MARIO_POSTER,
    ICON_UNKNOWN_PAPERS,
    ICON_DUBIOUS_PAPER,
    ICON_BLUE_POTION,
    ICON_RED_POTION,
    ICON_ORANGE_POTION,
    ICON_GREEN_POTION,
    ICON_STAR_KEY,
    ICON_SPECIAL_CARD,
    ICON_PLATINUM_CARD,
    ICON_GOLD_CARD,
    ICON_SILVER_CARD,
    ICON_PIANTA,
    ICON_LOTTERY_PICK,
    ICON_GOLD_BAR,
    ICON_GOLD_BAR_X3,
    ICON_BATTLE_POISON_MUSHROOM,
    ICON_UP_ARROW,
    ICON_WHACKA_BUMP,
    ICON_BATTLE_NORMAL_BOOT,
    ICON_BATTLE_SUPER_BOOT,
    ICON_BATTLE_ULTRA_BOOT,
    ICON_BATTLE_NORMAL_HAMMER,
    ICON_BATTLE_SUPER_HAMMER,
    ICON_BATTLE_ULTRA_HAMMER,
    ICON_BATTLE_FIRE_FLOWER_ITEMS,
    ICON_BATTLE_STAR_SPECIAL_MOVES,
    ICON_BATTLE_RED_FLAG_TACTICS,
    ICON_BATTLE_SHINE_SPRITE,
    ICON_SPECIAL_MOVES_CIRCLE_ORANGE,
    ICON_SPECIAL_MOVES_CIRCLE_BLUE,
    ICON_SPECIAL_MOVES_CIRCLE_GREEN,
    ICON_SPECIAL_MOVES_CIRCLE_RED,
    ICON_UNUSED_PICTURE_F,
    ICON_BATTLE_TACTICS_GOOMBELLA,
    ICON_BATTLE_TACTICS_KOOPS,
    ICON_BATTLE_TACTICS_BOBBERY,
    ICON_BATTLE_TACTICS_YOSHI_GREEN,
    ICON_BATTLE_TACTICS_YOSHI_RED,
    ICON_BATTLE_TACTICS_YOSHI_BLUE,
    ICON_BATTLE_TACTICS_YOSHI_ORANGE,
    ICON_BATTLE_TACTICS_YOSHI_PINK,
    ICON_BATTLE_TACTICS_YOSHI_BLACK,
    ICON_BATTLE_TACTICS_YOSHI_WHITE,
    ICON_BATTLE_TACTICS_FLURRIE,
    ICON_BATTLE_TACTICS_VIVIAN,
    ICON_BATTLE_TACTICS_MS_MOWZ,
    ICON_UNKNOWN_WHITE_LARGE_X,
    ICON_BATTLE_ARROW_RUN_AWAY,
    ICON_BATTLE_APPEAL,
    ICON_UNKNOWN_BOX_BLUE_TRIANGLES,
    ICON_BATTLE_CHARGE,
    ICON_BATTLE_SUPER_CHARGE,
    ICON_BATTLE_AUDIENCE_CAN,
    ICON_BATTLE_AUDIENCE_ROCK,
    ICON_BATTLE_AUDIENCE_BONE,
    ICON_BATTLE_AUDIENCE_HAMMER,
    ICON_COURAGE_SHELL,
    ICON_TRADE_OFF,
    ICON_UNUSED_FIRE_FLOWER_ROD,
    ICON_POWER_PUNCH,
    ICON_THUNDER_RAGE,
    ICON_FIRE_FLOWER,
    ICON_THUNDER_BOLT,
    ICON_DIZZY_DIAL,
    ICON_HONEY_SYRUP,
    ICON_POINT_SWAP,
    ICON_MUSHROOM,
    ICON_SUPER_SHROOM,
    ICON_ULTRA_SHROOM,
    ICON_LIFE_SHROOM,
    ICON_SLOW_SHROOM,
    ICON_DRIED_SHROOM,
    ICON_VOLT_SHROOM,
    ICON_GRADUAL_SYRUP,
    ICON_KOOPA_CURSE,
    ICON_SHOOTING_STAR,
    ICON_ICE_STORM,
    ICON_UNKNOWN_PILLOW,
    ICON_MAPLE_SYRUP,
    ICON_MINI_MR_MINI,
    ICON_MYSTERY,
    ICON_SLEEPY_SHEEP,
    ICON_STOPWATCH,
    ICON_HP_DRAIN_ITEM,
    ICON_EARTH_QUAKE,
    ICON_BATTLE_DROP_HEART,
    ICON_BATTLE_DROP_FLOWER,
    ICON_BATTLE_DROP_COIN,
    ICON_BATTLE_STAR_POINTS_FROM_ENEMIES_COUNT,
    ICON_STAR_PIECE,
    ICON_POW_BLOCK,
    ICON_MR_SOFTENER,
    ICON_BOOS_SHEET,
    ICON_RUIN_POWDER,
    ICON_JAMMIN_JELLY,
    ICON_TASTY_TONIC,
    ICON_REPEL_CAPE,
    ICON_DATA_DISK,
    ICON_FRIGHT_MASK,
    ICON_DIAMOND_STAR,
    ICON_EMERALD_STAR,
    ICON_GARNET_STAR,
    ICON_GOLD_STAR,
    ICON_CRYSTAL_STAR,
    ICON_RUBY_STAR,
    ICON_SAPPHIRE_STAR,
    ICON_PAUSE_MENU_MARIO_HEAD,
    ICON_PAUSE_MENU_HEART,
    ICON_PAUSE_MENU_FLOWER,
    ICON_PAUSE_MENU_BADGE_POINTS,
    ICON_PAUSE_MENU_CLOCK,
    ICON_HUD_MARIO_HEALTH,
    ICON_HUD_GOOMBELLA_HEALTH,
    ICON_HUD_KOOPS_HEALTH,
    ICON_HUD_BOBBERY_HEALTH,
    ICON_HUD_YOSHI_GREEN_HEALTH,
    ICON_HUD_YOSHI_RED_HEALTH,
    ICON_HUD_YOSHI_BLUE_HEALTH,
    ICON_HUD_YOSHI_ORANGE_HEALTH,
    ICON_HUD_YOSHI_PINK_HEALTH,
    ICON_HUD_YOSHI_BLACK_HEALTH,
    ICON_HUD_YOSHI_WHITE_HEALTH,
    ICON_HUD_FLURRIE_HEALTH,
    ICON_HUD_VIVIAN_HEALTH,
    ICON_HUD_MS_MOWZ_HEALTH,
    ICON_HUD_FLOWER,
    ICON_HUD_STAR_POWER_STAR,
    ICON_HUD_STAR_POINTS,
    ICON_HUD_COINS,
    ICON_UNKNOWN_RED_TRIANGLE_UP,
    ICON_UNKNOWN_RED_TRIANGLE_DOWN,
    ICON_HUD_SP_BAR_YELLOW,
    ICON_HUD_SP_BAR_ORANGE,
    ICON_HUD_SP_BAR_RED,
    ICON_HUD_SP_BAR_PINK,
    ICON_HUD_SP_BAR_PURPLE,
    ICON_HUD_SP_BAR_BLUE,
    ICON_HUD_SP_BAR_CYAN,
    ICON_HUD_SP_BAR_GREEN,
    ICON_UNKNOWN_GRAY_CIRCLE,
    ICON_HUD_SP_BAR_FILLED_1_17,
    ICON_HUD_SP_BAR_FILLED_2_17,
    ICON_HUD_SP_BAR_FILLED_3_17,
    ICON_HUD_SP_BAR_FILLED_4_17,
    ICON_HUD_SP_BAR_FILLED_5_17,
    ICON_HUD_SP_BAR_FILLED_6_17,
    ICON_HUD_SP_BAR_FILLED_7_17,
    ICON_HUD_SP_BAR_FILLED_8_17,
    ICON_HUD_SP_BAR_FILLED_9_17,
    ICON_HUD_SP_BAR_FILLED_10_17,
    ICON_HUD_SP_BAR_FILLED_11_17,
    ICON_HUD_SP_BAR_FILLED_12_17,
    ICON_HUD_SP_BAR_FILLED_13_17,
    ICON_HUD_SP_BAR_FILLED_14_17,
    ICON_HUD_SP_BAR_FILLED_15_17,
    ICON_HUD_SP_BAR_FILLED_16_17,
    ICON_HUD_RED_BAR_HP,
    ICON_HUD_YELLOW_BAR_FP,
    ICON_HUD_BLUE_BAR_SP,
    ICON_HUD_GREEN_BAR_STAR_POINTS_COINS,
    ICON_UNKNOWN_WHITE_BOX,
    ICON_UNKNOWN_WHITE_SMALL_X,
    ICON_UNKNOWN_WHITE_TINY_X,
    ICON_UNKNOWN_WHITE_SIDEWAYS_LINE,
    ICON_UNKNOWN_WHITE_SIDEWAYS_SMALLER_LINE,
    ICON_WHITE_NUMBER_0,
    ICON_WHITE_NUMBER_SMALL_0,
    ICON_WHITE_NUMBER_1,
    ICON_WHITE_NUMBER_SMALL_1,
    ICON_WHITE_NUMBER_2,
    ICON_WHITE_NUMBER_SMALL_2,
    ICON_WHITE_NUMBER_3,
    ICON_WHITE_NUMBER_SMALL_3,
    ICON_WHITE_NUMBER_4,
    ICON_WHITE_NUMBER_SMALL_4,
    ICON_WHITE_NUMBER_5,
    ICON_WHITE_NUMBER_SMALL_5,
    ICON_WHITE_NUMBER_6,
    ICON_WHITE_NUMBER_SMALL_6,
    ICON_WHITE_NUMBER_7,
    ICON_WHITE_NUMBER_SMALL_7,
    ICON_WHITE_NUMBER_8,
    ICON_WHITE_NUMBER_SMALL_8,
    ICON_WHITE_NUMBER_9,
    ICON_WHITE_NUMBER_SMALL_9,
    ICON_WHITE_SMALL_PLUS,
    ICON_WHITE_SMALL_MINUS,
    ICON_ANIMATED_WHITE_HAND_DOWN,
    ICON_ANIMATED_WHITE_HAND_LEFT,

#ifdef TTYD_JP
    ICON_JAPANESE_TEXT_POINTS,
#endif

    ICON_UNKNOWN_WHITE_CIRCLE_DIALOG_BUBBLE,
    ICON_UNKNOWN_BUTTON_A_PRESSED,
    ICON_UNKNOWN_BUTTON_B_PRESSED,
    ICON_UNKNOWN_BUTTON_X_PRESSED,
    ICON_UNKNOWN_BLACK_BOX_GRAY_CIRCLE,
    ICON_TIMER_0,
    ICON_TIMER_1,
    ICON_TIMER_2,
    ICON_TIMER_3,
    ICON_TIMER_4,
    ICON_TIMER_5,
    ICON_TIMER_6,
    ICON_TIMER_7,
    ICON_TIMER_8,
    ICON_TIMER_9,
    ICON_TIMER_COLON,
    ICON_TIMER_PERIOD,
    ICON_PUNI_BAR_WHITE,
    ICON_PUNI_WHITE,
    ICON_UNKNOWN_SMALL_RED_ARROW_DOWN,

#ifndef TTYD_JP
    ICON_BATTLE_AUDIENCE_COUNT,
    ICON_ANIMATED_BUTTON_A,
    ICON_ANIMATED_ANALOG_STICK_RIGHT,
    ICON_ANIMATED_BUTTON_START,
    ICON_INFINITE_SYMBOL,
    ICON_UNKNOWN_YELLOW_STAR,
    ICON_PAUSE_MENU_WHITE_CIRCLE,
    ICON_BUTTON_D_PAD_ALL_DIRECTIONS,
    ICON_JAPANESE_TEXT_PARTNER_MENU,
    ICON_JAPANESE_TEXT_INVENTORY_MENU,
    ICON_JAPANESE_TEXT_BADGE_MENU,
    ICON_JAPANESE_TEXT_JOURNAL_MENU,
    ICON_UNKNOWN_BUTTON_Z,
#endif

    ICON_TEXT_KOOPA,

#ifndef TTYD_JP
    ICON_TEXT_BOWSER,
#endif

    ICON_TEXT_TIME,

#ifndef TTYD_JP
    ICON_TEXT_ZEIT,
    ICON_TEXT_TEMPS,
    ICON_TEXT_TIEMPO,
    ICON_TEXT_TEMPO,
#endif

    ICON_TEXT_WORLD,

#ifdef TTYD_JP
    ICON_BATTLE_AUDIENCE_COUNT,
#else
    ICON_TEXT_WELT,
    ICON_TEXT_MONDE,
    ICON_TEXT_MUNDO,
    ICON_TEXT_MONDO,
    ICON_JAPANESE_TEXT_POINTS,
#endif

    ICON_TEXT_POINTS,

#ifdef TTYD_JP
    ICON_ANIMATED_BUTTON_A,
    ICON_ANIMATED_ANALOG_STICK_RIGHT,
    ICON_ANIMATED_BUTTON_START,
    ICON_INFINITE_SYMBOL,
    ICON_UNKNOWN_YELLOW_STAR,
    ICON_TEXT_BOWSER,
    ICON_PAUSE_MENU_WHITE_CIRCLE,
    ICON_BUTTON_D_PAD_ALL_DIRECTIONS,
    ICON_JAPANESE_TEXT_PARTNER_MENU,
    ICON_JAPANESE_TEXT_INVENTORY_MENU,
    ICON_JAPANESE_TEXT_BADGE_MENU,
    ICON_JAPANESE_TEXT_JOURNAL_MENU,
    ICON_UNKNOWN_BUTTON_Z,
#else
    ICON_TEXT_PUNKTE,
    ICON_TEXT_PUNTOS,
    ICON_TEXT_PUNTI,
    ICON_TEXT_RED_PARTY,
    ICON_TEXT_RED_TEAM,
    ICON_TEXT_RED_GROUPE,
    ICON_TEXT_RED_EQUIPO,
    ICON_TEXT_RED_COMPAGNIA,
    ICON_TEXT_YELLOW_GEAR,
    ICON_TEXT_YELLOW_ITEMS,
    ICON_TEXT_YELLOW_OBJETS,
    ICON_TEXT_YELLOW_OBJETOS,
    ICON_TEXT_YELLOW_OGGETTI,
    ICON_GREEN_TEXT_BADGES,
    ICON_GREEN_TEXT_ORDEN,
    ICON_GREEN_TEXT_MEDALLAS,
    ICON_GREEN_TEXT_TESSERE,
    ICON_BLUE_TEXT_JOURNAL,
    ICON_BLUE_TEXT_DIARIO,
    ICON_BLUE_TEXT_CRONACA,
#endif

    ICON_MAX_VALUE, // Should never use this value nor higher
};

inline bool iconIsValid(IconId icon)
{
    return (icon >= IconId::ICON_INVALID_ITEM_ZAP_TAP) && (icon < IconId::ICON_MAX_VALUE);
}

extern "C"
{
    // iconNumberDispGx3D
    void iconNumberDispGx(mtx34 mtx, int32_t number, bool isTextSmall, const uint8_t color[4]);
    // iconSetAlpha
    // iconSetScale
    // iconFlagOff
    // iconFlagOn
    // iconSetPos
    // iconNameToPtr
    // iconGX
    void iconGetWidthHight(uint16_t *widthOut, uint16_t *heightOut, IconId icon);
    // iconGetTexObj
    void iconDispGxCol(mtx34 mtx, uint16_t flags, IconId icon, const uint8_t color[4]);
    // iconDispGx2
    void iconDispGx(Vec3 *pos, uint16_t flags, IconId icon, float scale);
    void iconDispGxAlpha(Vec3 *pos, uint16_t flags, IconId icon, uint8_t alpha, float scale);
    // iconDisp
    // iconChange
    // iconDelete
    // iconEntry2D
    // iconEntry
    // iconMain
    // iconReInit
    // iconTexSetup
    // iconInit
    // _callback_bin
    // _callback_tpl
}

#endif
