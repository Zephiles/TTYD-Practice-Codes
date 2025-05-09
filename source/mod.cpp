#include "mod.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

Mod *gMod = nullptr;

const char *gHelpTextAConfirmBCancel = "Press A to confirm\nPress B to cancel";
const char *gHelpTextButtonCombo = "Button Combo (Can be used in any order)";
const char *gTimeStringFormat = "%02" PRIu32 ":%02" PRIu32 ":%02" PRIu32 ".%02" PRIu32;
