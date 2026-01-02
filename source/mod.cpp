#include "mod.h"
#include "gc/OSCache.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

Mod *gMod = nullptr;

const char *gHelpTextAConfirmBCancel = "Press A to confirm\nPress B to cancel";
const char *gHelpTextButtonCombo = "Button Combo (Can be used in any order)";
const char *gTimeStringFormat = "%02" PRIu32 ":%02" PRIu32 ":%02" PRIu32 ".%02" PRIu32;

bool Mod::flagIsSet(uint32_t flag) const
{
    // Make sure the flag is valid
    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
    constexpr uint32_t maxFlags = MOD_FLAGS_ARRAY_SIZE * bitsPerWord;

    if (flag >= maxFlags)
    {
        return false;
    }

    // Make sure the flag does not exceed TOTAL_MOD_FLAGS
    if (flag >= TOTAL_MOD_FLAGS)
    {
        return false;
    }

    return (this->flags[flag / bitsPerWord] >> (flag % bitsPerWord)) & 1U;
}

void Mod::setFlag(uint32_t flag)
{
    // Make sure the flag is valid
    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
    constexpr uint32_t maxFlags = MOD_FLAGS_ARRAY_SIZE * bitsPerWord;

    if (flag >= maxFlags)
    {
        return;
    }

    // Make sure the flag does not exceed TOTAL_MOD_FLAGS
    if (flag >= TOTAL_MOD_FLAGS)
    {
        return;
    }

    this->flags[flag / bitsPerWord] |= (1UL << (flag % bitsPerWord));
}

void Mod::clearFlag(uint32_t flag)
{
    // Make sure the flag is valid
    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
    constexpr uint32_t maxFlags = MOD_FLAGS_ARRAY_SIZE * bitsPerWord;

    if (flag >= maxFlags)
    {
        return;
    }

    // Make sure the flag does not exceed TOTAL_MOD_FLAGS
    if (flag >= TOTAL_MOD_FLAGS)
    {
        return;
    }

    this->flags[flag / bitsPerWord] &= ~(1UL << (flag % bitsPerWord));
}

bool Mod::toggleFlag(uint32_t flag)
{
    // Make sure the flag is valid
    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
    constexpr uint32_t maxFlags = MOD_FLAGS_ARRAY_SIZE * bitsPerWord;

    if (flag >= maxFlags)
    {
        return false;
    }

    // Make sure the flag does not exceed TOTAL_MOD_FLAGS
    if (flag >= TOTAL_MOD_FLAGS)
    {
        return false;
    }

    this->flags[flag / bitsPerWord] ^= (1UL << (flag % bitsPerWord));
    return this->flagIsSet(flag);
}

#ifdef TTYD_JP
void adjustMarioZeroHitboxGlitchFloats(bool enabled)
{
    float *motSlitValues = reinterpret_cast<float *>(0x8041AED8);
    if (enabled)
    {
        // Cheat should be enabled, so clear the values
        motSlitValues[0] = 0.f;
        motSlitValues[1] = 0.f;
    }
    else
    {
        // Cheat should be disabled, so re-set the proper values
        motSlitValues[0] = 11.f;
        motSlitValues[1] = 20.f;
    }

    // Clear the cache for the addresses since this is modifying cached memory
    DCFlushRange(motSlitValues, sizeof(float) * 2);
}
#endif
