#include "mod.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

Mod *gMod = nullptr;

const char *gHelpTextAConfirmBCancel = "Press A to confirm\nPress B to cancel";
const char *gHelpTextButtonCombo = "Button Combo (Can be used in any order)";
const char *gTimeStringFormat = "%02" PRIu32 ":%02" PRIu32 ":%02" PRIu32 ".%02" PRIu32;

bool _flagIsSet(const uint32_t *flagsPtr, uint32_t flag, uint32_t maxFlags)
{
    // Make sure the flag is valid
    if (flag >= maxFlags)
    {
        return false;
    }

    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
    return (flagsPtr[flag / bitsPerWord] >> (flag % bitsPerWord)) & 1U;
}

void _setFlag(uint32_t *flagsPtr, uint32_t flag, uint32_t maxFlags)
{
    // Make sure the flag is valid
    if (flag >= maxFlags)
    {
        return;
    }

    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
    flagsPtr[flag / bitsPerWord] |= (1UL << (flag % bitsPerWord));
}

void _clearFlag(uint32_t *flagsPtr, uint32_t flag, uint32_t maxFlags)
{
    // Make sure the flag is valid
    if (flag >= maxFlags)
    {
        return;
    }

    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
    flagsPtr[flag / bitsPerWord] &= ~(1UL << (flag % bitsPerWord));
}

bool _toggleFlag(uint32_t *flagsPtr, uint32_t flag, uint32_t maxFlags)
{
    // Make sure the flag is valid
    if (flag >= maxFlags)
    {
        return false;
    }

    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
    flagsPtr[flag / bitsPerWord] ^= (1UL << (flag % bitsPerWord));

    return _flagIsSet(flagsPtr, flag, maxFlags);
}

bool Mod::flagIsSet(uint32_t flag) const
{
    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
    constexpr uint32_t maxFlags = MOD_FLAGS_ARRAY_SIZE * bitsPerWord;

    return _flagIsSet(this->flags, flag, maxFlags);
}

void Mod::setFlag(uint32_t flag)
{
    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
    constexpr uint32_t maxFlags = MOD_FLAGS_ARRAY_SIZE * bitsPerWord;

    _setFlag(this->flags, flag, maxFlags);
}

void Mod::clearFlag(uint32_t flag)
{
    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
    constexpr uint32_t maxFlags = MOD_FLAGS_ARRAY_SIZE * bitsPerWord;

    return _clearFlag(this->flags, flag, maxFlags);
}

bool Mod::toggleFlag(uint32_t flag)
{
    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
    constexpr uint32_t maxFlags = MOD_FLAGS_ARRAY_SIZE * bitsPerWord;

    return _toggleFlag(this->flags, flag, maxFlags);
}

bool Mod::saveFlagIsSet(uint32_t flag) const
{
    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
    constexpr uint32_t maxFlags = MOD_SAVE_FLAGS_ARRAY_SIZE * bitsPerWord;

    return _flagIsSet(this->saveFlags, flag, maxFlags);
}

void Mod::setSaveFlag(uint32_t flag)
{
    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
    constexpr uint32_t maxFlags = MOD_SAVE_FLAGS_ARRAY_SIZE * bitsPerWord;

    _setFlag(this->saveFlags, flag, maxFlags);
}

void Mod::clearSaveFlag(uint32_t flag)
{
    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
    constexpr uint32_t maxFlags = MOD_SAVE_FLAGS_ARRAY_SIZE * bitsPerWord;

    _clearFlag(this->saveFlags, flag, maxFlags);
}

bool Mod::toggleSaveFlag(uint32_t flag)
{
    constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
    constexpr uint32_t maxFlags = MOD_SAVE_FLAGS_ARRAY_SIZE * bitsPerWord;

    return _toggleFlag(this->saveFlags, flag, maxFlags);
}
