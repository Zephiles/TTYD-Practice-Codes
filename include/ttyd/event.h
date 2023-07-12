#ifndef TTYD_EVENT_H
#define TTYD_EVENT_H

#include "ttyd/party.h"

#include <cstdint>

typedef void (*EventStageEventDescriptionInitFunc)();

struct EventStageEventDescription
{
    uint8_t entryMotionType;
    PartyMembers partnerId;
    PartyMembers followerId;
    uint8_t pad_3;
    uint16_t gsw0; // Sequence Position
    uint16_t pad_6;
    char *textId;
    char *nameJp;

#ifndef TTYD_JP
    char *nameEn;
#endif

    char *map;
    char *bero;
    EventStageEventDescriptionInitFunc pfnInit;
} __attribute__((__packed__));

struct EventStageDescription
{
    char *nameJp;

#ifndef TTYD_JP
    char *nameEn;
#endif

    EventStageEventDescription *pEvents;
    int32_t eventCount;
} __attribute__((__packed__));

#ifdef TTYD_JP
static_assert(sizeof(EventStageEventDescription) == 0x1C);
static_assert(sizeof(EventStageDescription) == 0xC);
#else
static_assert(sizeof(EventStageEventDescription) == 0x20);
static_assert(sizeof(EventStageDescription) == 0x10);
#endif

extern "C"
{
    int32_t eventStgNum();
    EventStageDescription *eventStgDtPtr(int32_t stageId);
    // N_stg9_02_init
    // stg8_15_init
    // stg8_14_init
    // stg8_13_init
    // stg8_11_init
    // stg8_10_init
    // stg8_06_init
    // stg8_05_init
    // stg8_02_init
    // stg8_01_02_init
    // stg7_52_init
    // stg7_51_init
    // stg7_50_init
    // stg7_25_00_init
    // stg7_20_init
    // stg7_19_init
    // stg7_17_init
    // stg7_16_init
    // stg7_15_room_C_init
    // stg7_15_room_B_init
    // stg7_15_room_A_init
    // stg7_15_init
    // stg7_11_init
    // stg7_10_init
    // stg7_06_init
    // stg7_04_init
    // stg7_03_white_5
    // stg7_03_white_4
    // stg7_03_white_3
    // stg7_03_white_2
    // stg7_03_white_1
    // stg7_03_white_0
    // stg7_01_init
    // stg6_end_init02
    // stg6_end_init01
    // stg6_72_init
    // stg6_71_01_init
    // stg6_70_init
    // stg6_56_init
    // stg6_54_init
    // stg6_47_00_init
    // stg6_44_init
    // stg6_39_init
    // stg6_38_init
    // stg6_37_init
    // stg6_34_init
    // stg6_33_init
    // stg6_32_init
    // stg6_30_init
    // stg6_29_init
    // stg6_27_init
    // stg6_24_init
    // stg6_22_init
    // stg6_20_init
    // stg6_19_init
    // stg6_18_init
    // stg6_13_init
    // stg6_12_init
    // stg6_06_init
    // stg6_00_init
    // stg5_66_init
    // stg5_65_init
    // stg5_64_03_init
    // stg5_64_02_init
    // stg5_64_01_init
    // stg5_64_init
    // stg5_63_init
    // stg5_62_init
    // stg5_60_init
    // stg5_59_01_init
    // stg5_58_init
    // stg5_47_init
    // stg5_43_init
    // stg5_30_init02
    // stg5_30_init01
    // stg5_29_init
    // stg5_22_init01
    // stg5_22_init00
    // stg5_21_01_init00
    // stg5_21_init03
    // stg5_21_init01
    // stg5_21_init00
    // stg5_19_01_init
    // stg5_19_init02
    // stg5_19_init01
    // stg5_19_init00
    // stg5_18_init_02
    // stg5_18_init
    // stg5_17_init
    // stg5_15_init02
    // stg5_15_init01
    // stg5_12_init01
    // stg4_end_init
    // stg4_37_init
    // stg4_36_init
    // stg4_35_init
    // stg4_30_init
    // stg4_16_init
    // stg4_15_03_init
    // stg4_15_init
    // stg4_14_init
    // stg4_13_03_init
    // stg4_13_02_init
    // stg4_13_init
    // stg4_11_init
    // stage4_10_init
    // stage4_09_05_init
    // stage4_05_init
    // stage4_04_01_init
    // stage4_04_init
    // stage4_00_init
    // stg3_end_init
    // stg3_52_init
    // stg3_50_init
    // stg3_35_init
    // stg3_34_init
    // stg3_33_04_init
    // stg3_33_03_init
    // stg3_33_init
    // stg3_32_init
    // stg3_31_init
    // stg3_31_00_init
    // stg3_30_init
    // stg3_21_init
    // stg3_20_15_init
    // stg3_20_14_init
    // stg3_20_13_init
    // stg3_20_init_1
    // stg3_20_init_2
    // stg3_20_init_3
    // stg3_20_12_init
    // stg3_20_09_init
    // stg3_20_08_init
    // stg3_20_07_00_init
    // stg3_20_06_init
    // stg3_20_init_4
    // stg3_20_init_5
    // stg3_20_init_6
    // stg3_20_init_7
    // stg3_20_02_init
    // stg3_20_01_init
    // stg3_20_init_8
    // stg3_20_init_9
    // stg3_20_init_10
    // stg3_12_2_init
    // stg3_12_init__1
    // stg3_12_init__2
    // stg3_12_init__3
    // stg3_12_init__4
    // stg3_10_init__5
    // stg3_07_init__6
    // stg3_07_init__7
    // stg3_07_init__8
    // stg3_07_init__9
    // stg3_06_init
    // stg2_end_init
    // stg2_47_init
    // stg2_44_init
    // stg2_40_init
    // stg2_26_init
    // stg2_25_02_init
    // stg2_25_init
    // stg2_23_01_init
    // stg2_23_init
    // stg2_21_06_init01
    // stg2_21_06_init00
    // stg2_21_04_init
    // stg2_21_03_init
    // stg2_21_02_init
    // stg2_21_01_init
    // stg2_21_init02
    // stg2_21_init00
    // stg2_20_init
    // stg2_19_init
    // stg2_18_init02
    // stg2_18_init01
    // stg2_16_init01
    // stg2_16_init00
    // stg2_15_init
    // stg2_14_init
    // stg2_12_01_init
    // stg2_11_init
    // stg2_10_init
    // stg2_09_init
    // stg2_03_03_init
    // stg2_00_init
    // stg1_35_init01
    // stg1_30_init01
    // stg1_30_init
    // stg1_20_init
    // stg1_19_init
    // stg1_17_init
    // stg1_16_init01
    // stg1_16_init00
    // stg1_15_04_init
    // stg1_15_03_init
    // stg1_15_02_init
    // stg1_14_init
    // stg1_13_03_init
    // stg1_13_02_init
    // stg1_13_01_init
    // stg1_13_init
    // stg1_12_02_init
    // stg1_10_init
    // stg1_09_02_init
    // stg1_09_init
    // stg1_08_init
    // stg1_06_01_init
    // stg1_03_init
    // stg1_01a_init
    // stg0_16_init
    // stg0_13_init00
    // stg0_12_02_init
    // stg0_11_init
    // stg0_08_init
    // stg0_05_init
    // stg0_00_init
}

#endif
