#include "menus/cheatsMenu.h"
#include "ttyd/swdrv.h"

#include <cstdint>

void clearGSWFsRange(uint32_t lowerBound, uint32_t upperBound)
{
    for (uint32_t i = lowerBound; i <= upperBound; i++)
    {
        swClear(i);
    }
}

void clearAreaFlags(ClearAreaFlagsIndex index)
{
    switch (index)
    {
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_GOR:
        {
            // Clear the regular flags
            clearGSWFsRange(1175, 1250);

            // Clear the Shine Sprites
            clearGSWFsRange(5525, 5529);

            // Clear the Star Pieces
            clearGSWFsRange(5567, 5584);

            // Clear misc flags not part of the standard gor memory area
            static constexpr const uint8_t flagsToClear[] = {
                9,  // Post-Chapter 3 Koopie Koo Email
                10, // Post-Chapter 4 Petuni Email
                11, // Email from Boo after Chapter 5 Intermission
                14, // Real Zip Toad Email after Chapter 6
                20, // RDM #4 Email after Chapter 5
                23, // Koopook Email after Chapter 2
                24, // Email from Pine T. Jr. after Chapter 3
                25, // Email from Jolene after Zess Trouble
                26, // Email from Bub-ulber after Mowz Trouble
                27, // Eve Email after Chapter 6
            };

            constexpr uint32_t loopCount = sizeof(flagsToClear) / sizeof(flagsToClear[0]);
            for (uint32_t i = 0; i < loopCount; i++)
            {
                swClear(flagsToClear[i]);
            }
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_TIK:
        {
            // Clear the regular flags
            clearGSWFsRange(1325, 1371);

            // Clear the Shine Sprites
            clearGSWFsRange(5530, 5537);

            // Clear the Star Pieces
            clearGSWFsRange(5585, 5597);

            // Clear misc flags not part of the standard tik memory area
            static constexpr const uint8_t flagsToClear[] = {
                16, // RDM #7 Email after Chapter 7
            };

            constexpr uint32_t loopCount = sizeof(flagsToClear) / sizeof(flagsToClear[0]);
            for (uint32_t i = 0; i < loopCount; i++)
            {
                swClear(flagsToClear[i]);
            }
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_HEI:
        {
            // Clear the regular flags
            clearGSWFsRange(1774, 1806);

            // Clear the Star Pieces
            clearGSWFsRange(5598, 5600);
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_NOK:
        {
            // Clear the regular flags
            clearGSWFsRange(1624, 1629);

            // Clear the Star Pieces
            clearGSWFsRange(5601, 5602);

            // Clear misc flags not part of the standard nok memory area
            static constexpr const uint8_t flagsToClear[] = {
                7, // Frankly Email at right side of town
            };

            constexpr uint32_t loopCount = sizeof(flagsToClear) / sizeof(flagsToClear[0]);
            for (uint32_t i = 0; i < loopCount; i++)
            {
                swClear(flagsToClear[i]);
            }
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_GON:
        {
            // Clear the regular flags
            clearGSWFsRange(1476, 1511);

            // Clear the Shine Sprites
            clearGSWFsRange(5538, 5540);

            // Clear the Star Pieces
            clearGSWFsRange(5603, 5607);
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_WIN:
        {
            // Clear the regular flags
            clearGSWFsRange(2675, 2687);

            // Clear the Shine Sprites
            clearGSWFsRange(5541, 5541);

            // Clear the Star Pieces
            clearGSWFsRange(5608, 5611);

            // Clear misc flags not part of the standard win memory area
            static constexpr const uint8_t flagsToClear[] = {
                17, // RDM #1 Email after getting Flurrie
            };

            constexpr uint32_t loopCount = sizeof(flagsToClear) / sizeof(flagsToClear[0]);
            for (uint32_t i = 0; i < loopCount; i++)
            {
                swClear(flagsToClear[i]);
            }
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_MRI:
        {
            // Clear the regular flags
            clearGSWFsRange(2825, 2885);

            // Clear the Shine Sprites
            clearGSWFsRange(5542, 5545);

            // Clear the Star Pieces
            clearGSWFsRange(5612, 5617);
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_TOU:
        {
            // Clear the regular flags
            clearGSWFsRange(2374, 2533);

            // Clear the Shine Sprites
            clearGSWFsRange(5546, 5547);

            // Clear the Star Pieces
            clearGSWFsRange(5618, 5627);

            // Clear misc flags not part of the standard tou memory area
            static constexpr const uint8_t flagsToClear[] = {
                18,  // RDM #2 Email after getting Egg
                250, // RDM #2 Email after getting Egg
                251, // RDM #4 Email after Chapter 5
            };

            constexpr uint32_t loopCount = sizeof(flagsToClear) / sizeof(flagsToClear[0]);
            for (uint32_t i = 0; i < loopCount; i++)
            {
                swClear(flagsToClear[i]);
            }
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_USU:
        {
            // Clear the regular flags
            clearGSWFsRange(1925, 1939);

            // Clear the Star Pieces
            clearGSWFsRange(5628, 5630);
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_GRA:
        {
            // Clear the regular flags
            clearGSWFsRange(2075, 2091);

            // Clear the Shine Sprites
            clearGSWFsRange(5548, 5548);

            // Clear the Star Pieces
            clearGSWFsRange(5631, 5633);
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_JIN:
        {
            // Clear the regular flags
            clearGSWFsRange(2226, 2241);

            // Clear the Shine Sprites
            clearGSWFsRange(5549, 5551);

            // Clear the Star Pieces
            clearGSWFsRange(5634, 5637);
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_MUJ:
        {
            // Clear the regular flags
            clearGSWFsRange(3126, 3158);

            // Clear the Shine Sprites
            clearGSWFsRange(5552, 5553);

            // Clear the Star Pieces
            clearGSWFsRange(5638, 5643);
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_DOU:
        {
            // Clear the regular flags
            clearGSWFsRange(2975, 2994);

            // Clear the Shine Sprites
            clearGSWFsRange(5554, 5558);

            // Clear the Star Pieces
            clearGSWFsRange(5644, 5647);
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_HOM:
        {
            // Clear the regular flags
            clearGSWFsRange(3574, 3575);

            // Clear misc flags not part of the standard hom memory area
            static constexpr const uint8_t flagsToClear[] = {
                13, // King K. Email after fixing drawbridge
            };

            constexpr uint32_t loopCount = sizeof(flagsToClear) / sizeof(flagsToClear[0]);
            for (uint32_t i = 0; i < loopCount; i++)
            {
                swClear(flagsToClear[i]);
            }
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_RSH:
        {
            // Clear the regular flags
            clearGSWFsRange(3425, 3462);

            // Clear the Shine Sprites
            clearGSWFsRange(5559, 5560);

            // Clear the Star Pieces
            clearGSWFsRange(5648, 5652);

            // Clear misc flags not part of the standard rsh memory area
            static constexpr const uint8_t flagsToClear[] = {
                12, // Email from Pa-Patch on Day 3
                21, // RDM #5 Email on second day
            };

            constexpr uint32_t loopCount = sizeof(flagsToClear) / sizeof(flagsToClear[0]);
            for (uint32_t i = 0; i < loopCount; i++)
            {
                swClear(flagsToClear[i]);
            }
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_EKI:
        {
            // Clear the regular flags
            clearGSWFsRange(3725, 3754);

            // Clear the Shine Sprites
            clearGSWFsRange(5561, 5562);

            // Clear the Star Pieces
            clearGSWFsRange(5653, 5653);
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_PIK:
        {
            // Clear the regular flags
            clearGSWFsRange(3276, 3279);

            // Clear the Shine Sprites
            clearGSWFsRange(5563, 5564);

            // Clear the Star Pieces
            clearGSWFsRange(5654, 5657);

            // Clear misc flags not part of the standard pik memory area
            static constexpr const uint8_t flagsToClear[] = {
                22,  // RDM #6 Email after Goldbob cannon
                252, // RDM #6 Email after Goldbob cannon
            };

            constexpr uint32_t loopCount = sizeof(flagsToClear) / sizeof(flagsToClear[0]);
            for (uint32_t i = 0; i < loopCount; i++)
            {
                swClear(flagsToClear[i]);
            }
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_BOM:
        {
            // Clear the regular flags
            clearGSWFsRange(3874, 3892);

            // Clear the Shine Sprites
            clearGSWFsRange(5565, 5566);

            // Clear the Star Pieces
            clearGSWFsRange(5658, 5663);

            // Clear misc flags not part of the standard bom memory area
            static constexpr const uint8_t flagsToClear[] = {
                15, // Rawk Hawk Email after General White quest
            };

            constexpr uint32_t loopCount = sizeof(flagsToClear) / sizeof(flagsToClear[0]);
            for (uint32_t i = 0; i < loopCount; i++)
            {
                swClear(flagsToClear[i]);
            }
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_MOO:
        {
            // Clear the regular flags
            clearGSWFsRange(4025, 4039);

            // Clear the Star Pieces
            clearGSWFsRange(5664, 5664);
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_AJI:
        {
            // Clear the regular flags
            clearGSWFsRange(4175, 4217);

            // Clear the Star Pieces
            clearGSWFsRange(5665, 5666);
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_LAS:
        {
            // Clear the regular flags
            clearGSWFsRange(4326, 4394);
            break;
        }
        case ClearAreaFlagsIndex::CLEAR_AREA_FLAGS_JON:
        {
            // Clear the regular flags
            clearGSWFsRange(5075, 5085);
            break;
        }
        default:
        {
            break;
        }
    }
}
