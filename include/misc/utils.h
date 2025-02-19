#ifndef MISC_MISC_H
#define MISC_MISC_H

#include "assembly.h"
#include "gc/types.h"
#include "ttyd/seqdrv.h"
#include "ttyd/party.h"
#include "ttyd/item_data.h"
#include "ttyd/dispdrv.h"
#include "ttyd/mario_party.h"
#include "ttyd/mario_pouch.h"

#include <cstdint>

// Used to specifiy what region of GameCube memory a pointer points to.
enum PointerVerificationType
{
    PTR_INVALID = 0, // Does not point to valid GameCube memory
    PTR_CACHED,      // 0x80000000 through 0x817FFFFF
    PTR_UNCACHED,    // 0xC0000000 through 0xC17FFFFF
};

/**
 * Gets the ceiling of a value and aligns it to a specified alignment. As an example, `intCeil(12, 4)` returns `3`, and
 * `intCeil(13, 4)` returns `4`.
 *
 * @param value The value to get the ceiling of.
 * @param alignment The value to align the ceiling to.
 *
 * @returns The ceiling of the value aligned to the specified alignment.
 */
#define intCeil(value, alignment) (((value) + ((alignment) - 1)) / (alignment))

/**
 * Rounds a size up to a specified alignment.
 *
 * @param size The value to round up.
 * @param alignment The value to round the size up to.
 *
 * @returns The size rounded up to the specified alignment.
 */
#define roundUp(size, alignment) (((size) + ((alignment) - 1)) & ~((alignment) - 1))

/**
 * Rounds a size down to a specified alignment.
 *
 * @param size The value to round down.
 * @param alignment The value to round the size down to.
 *
 * @returns The size rounded down to the specified alignment.
 */
#define roundDown(size, alignment) (((size) / (alignment)) * (alignment))

/**
 * Checks if a button combination was pressed this frame.
 *
 * @param combo The button combination to check for.
 *
 * @returns `true` if the button combination was pressed this frame, otherwise `false`.
 */
bool checkButtonCombo(uint32_t combo);

/**
 * Checks if any one of some specified buttons are held.
 *
 * @param buttons The buttons to check for.
 *
 * @returns `true` if any one of the buttons are held, otherwise `false`.
 */
bool checkButtonsEveryFrame(uint32_t buttons);

/**
 * Checks if a button combination is held.
 *
 * @param buttons The button combination to check for.
 *
 * @returns `true` if the button combination is held, otherwise `false`.
 */
bool checkButtonComboEveryFrame(uint32_t combo);

// Gets the current sequence position.
uint32_t getSequencePosition();

/**
 * Sets the sequence position to a new value.
 *
 * @param position The value to set the sequence position to.
 */
void setSequencePosition(uint32_t position);

/**
 * Compares a string to `_next_map` via `strcmp`.
 *
 * @param str The string to compare against `_next_map`.
 *
 * @returns `true` if the strings are the same, otherwise `false`.
 */
bool compareStringToNextMap(const char *str);

/**
 * Initializes the sequence for transitioning to a new map.
 *
 * @param map The map to transition to.
 * @param bero The loading zone to come out of in the new map. Can be `nullptr`, which will cause Mario to spawn at the center
 * of the map.
 */
void setSeqMapChange(const char *map, const char *bero);

/**
 * Checks if the current seq specified by `seqGetSeq` and the next seq specified by `seqGetNextSeq` match a specified seq.
 *
 * @param wantedSeq The seq to check for.
 *
 * @returns `true` if the specified seq matches the current seq specified by `seqGetSeq` and the next seq specified by
 * `seqGetNextSeq`, otherwise `false`.
 */
bool checkForSpecificSeq(SeqIndex wantedSeq);

/**
 * Checks if the player has a file loaded and is not in a battle nor a screen transition.
 *
 * @returns `true` if the player has a file loaded and is not in a battle nor a screen transition, otherwise `false`.
 */
bool checkIfInGame();

/**
 * Checks if the game's internal system levels are 0 or not.
 *
 * @returns `true` if the game's internal system levels are 0, otherwise `false`.
 */
bool systemLevelIsZero();

/**
 * Sets the game's internal system levels to a new value.
 *
 * @param level The value to set the system level to.
 *
 * @note The global variable `_mariostSystemLevel` will be set to `level`, and `GlobalWork::systemLevelFlags` will be adjusted
 * via the function `marioStSystemLevel`.
 */
void setSystemLevel(int32_t level);

/**
 * Converts an `int32_t` value to a `float` value.
 *
 * @param value The value to convert to a `float` value.
 *
 * @returns The value converted to a `float` value.
 */
float intToFloat(int32_t value);

/**
 * Converts a `float` value to an `int32_t` value.
 *
 * @param value The value to convert to an `int32_t` value.
 *
 * @returns The value converted to an `int32_t` value.
 *
 * @note Any decimals contained by the `float` value will be discarded upon being converted. As an example, `3.2` will be
 * converted to `3`, and `-3.2` will be converted to `-3`.
 */
int32_t floatToInt(float value);

/**
 * Gets the pointer to the current partner out.
 *
 * @returns The pointer to the current partner out. Will be `nullptr` if no partner is currently out.
 */
inline PartyEntry *getPartnerPtr()
{
    return partyGetPtr(marioGetPartyId());
}

/**
 * Gets the pointer to the current follower out.
 *
 * @returns The pointer to the current follower out. Will be `nullptr` if no follower is currently out.
 */
inline PartyEntry *getFollowerPtr()
{
    return partyGetPtr(marioGetExtraPartyId());
}

/**
 * Get the id for the current partner or follower out.
 *
 * @param getPartner If `true`, then gets the id for the current partner out. Otherwise, gets the id for the current follower
 * out.
 *
 * @returns The id for the current partner or follower out.
 */
PartyMembers getCurrentPartnerOrFollowerOut(bool getPartner);

/**
 * Removes the current partner from the overworld, and frees the memory used by it.
 *
 * @note If no partner is currently out, then nothing will happen.
 */
inline void removePartnerFromOverworld()
{
    partyKill2(marioGetPartyId());
}

/**
 * Removes the current follower from the overworld, and frees the memory used by it.
 *
 * @note If no follower is currently out, then nothing will happen.
 */
inline void removeFollowerFromOverworld()
{
    partyKill2(marioGetExtraPartyId());
}

/**
 * Spawns a partner or follower.
 *
 * @param id Id of the partner or follower to spawn.
 *
 * @returns The id for the slot that the partner or follower was spawned in.
 */
PartySlotId spawnPartnerOrFollower(PartyMembers id);

/**
 * Spawns a partner or follower in the event that one is currently not out.
 *
 * @param shouldSpawnPartner If `true`, then a partner will be spawned. Otherwise, a follower will be spawned.
 *
 * @returns The id for the slot that the partner or follower was spawned in.
 *
 * @note If a partner or follower was previously out as indicated by `Player::prevPartnerId` or `Player::prevFollowerId`, then
 * that partner or follower will be brought out. Otherwise, the function will bring out either `Goombella` or `Frankly`.
 */
PartySlotId spawnFailsafePartnerOrFollower(bool shouldSpawnPartner);

// Rechecks Mario's jump and hammer levels.
inline void recheckJumpAndHammerLevels()
{
    pouchGetJumpLv();
    pouchGetHammerLv();
}

/**
 * Rechecks Mario's jump and hammer levels depending on which item is specified.
 *
 * @param item The item to check.
 *
 * @note Mario's jump and hammer levels will only be rechecked if `checkForSpecificSeq(SeqIndex::kBattle)` returns `true`, and
 * if the item is a boot or hammer upgrade (including the standard boot/hammer).
 */
void recheckBattleUpgrades(ItemId item);

/**
 * Checks if at least one of a badge is currently equipped.
 *
 * @param badge The badge to check for.
 *
 * @returns `true` if at least one of the badge is currently equipped, otherwise `false`.
 */
inline bool checkIfBadgeEquipped(ItemId badge)
{
    return pouchEquipCheckBadge(badge) > 0;
}

/**
 * Reinitializes the item menu in the pause menu.
 *
 * @note The item menu will only be reinitialized if the pause menu is currently open.
 */
void resetPauseMenuItemsMenu();

/**
 * Reinitializes the partners menu in the pause menu.
 *
 * @note The partners menu will only be reinitialized if the pause menu is currently open.
 */
void resetPauseMenuPartnersMenu();

/**
 * Calls `dispEntry` to add a function to the array of `dispdrv` drawing functions that run each frame, specifically on the
 * `CameraId::kDebug3d` layer.
 *
 * @param func The function to add.
 * @param order The order for the function. A higher/lower order will cause things drawn in this function to be drawn on
 * top/bottom of things drawn in other functions on the `CameraId::kDebug3d` layer respectively.
 *
 * @note The render mode will be set to `2`, and the user will be set to `nullptr`.
 */
void drawOnDebugLayer(DispCallback func, float order);

/**
 * Calls `dispEntry` to add a function to the array of `dispdrv` drawing functions that run each frame, specifically on the
 * `CameraId::k2d` layer.
 *
 * @param func The function to add.
 * @param order The order for the function. A higher/lower order will cause things drawn in this function to be drawn on
 * top/bottom of things drawn in other functions on the `CameraId::k2d` layer respectively.
 *
 * @note The render mode will be set to `2`, and the user will be set to `nullptr`.
 */
void drawOn2DLayer(DispCallback func, float order);

/**
 * Converts an `OSTime` variable to a string.
 *
 * @param[out] stringOut The pointer to the buffer to place the string.
 * @param[in] stringSize The total size of `stringOut`, including the null terminator.
 * @param[in] time The `OSTime` variable to convert.
 */
void getTimeString(char *stringOut, uint32_t stringSize, OSTime time);

/**
 * Gets the current angle of the analog stick.
 *
 * @param[out] stickXYOut The pointer to the buffer to store the X and Y values of the analog stick. Can be set to `nullptr` if
 * they are not needed.
 * @param[out] stickAngleOut The pointer to the buffer to store the angle of the analog stick.
 *
 * @returns `true` if the angle is not at neutral (the X and Y values of the analog stick are both 0), otherwise `false`.
 */
bool getStickAngle(int32_t stickXYOut[2], double *stickAngleOut);

/**
 * Gets the current angle of the analog stick via the function `getStickAngle`, and converts it to a string.
 *
 * @param[out] stringOut The pointer to the buffer to place the string.
 * @param[in] stringSize The total size of `stringOut`, including the null terminator.
 *
 * @note If the angle is at neutral (the X and Y values of the analog stick are both 0), then `Neutral` will be written to
 * `stringOut`.
 */
void getStickAngleString(char *stringOut, uint32_t stringSize);

// Functions for calling fpclassify on floats/doubles and checking if they are either FP_ZERO or FP_NORMAL

/**
 * Checks if a `float` value is one that can be easily worked with.
 *
 * @param value The value to be checked.
 *
 * @returns `true` if the value can be easily worked with, otherwise `false`.
 *
 * @note The value is checked via the function `std::fpclassify`, in which the returned value must be either `FP_ZERO` or
 * `FP_NORMAL` for this function to return `true`.
 */
bool floatCanBeWorkedWith(float value);

/**
 * Checks if a `double` value is one that can be easily worked with.
 *
 * @param value The value to be checked.
 *
 * @returns `true` if the value can be easily worked with, otherwise `false`.
 *
 * @note The value is checked via the function `std::fpclassify`, in which the returned value must be either `FP_ZERO` or
 * `FP_NORMAL` for this function to return `true`.
 */
bool doubleCanBeWorkedWith(double value);

/**
 * Checks if a pointer is within a specified range of addresses that are considered valid for GameCube memory addresses.
 *
 * @param ptr The pointer that is being checked.
 *
 * @returns A value from the `PointerVerificationType` enum.
 */
uint32_t ptrIsValid(void *ptr);

/**
 * Checks if a pointer is within a specified range of addresses that are considered valid for GameCube memory addresses.
 *
 * @tparam ptr The pointer that is being checked.
 *
 * @returns A value from the `PointerVerificationType` enum.
 *
 * @warning This function will not work correctly if a `const` pointer is passed in, so `const_cast` must be used if a `const`
 * pointer needs to be checked.
 *
 * @overload
 */
template<typename Ptr>
uint32_t ptrIsValid(Ptr ptr)
{
    return ptrIsValid(reinterpret_cast<void *>(ptr));
}

/**
 * Sets all of the memory starting at the specified address to 0.
 *
 * @param ptr The pointer to the memory that is being set to 0.
 * @param size The amount of bytes that are being set to 0.
 *
 * @returns `ptr`.
 */
void *clearMemory(void *ptr, uint32_t size);

/**
 * Sets all of the memory starting at a specified address to 0.
 *
 * @tparam ptr The pointer to the memory that is being set to 0.
 * @param size The amount of bytes that are being set to 0.
 *
 * @returns `ptr`.
 *
 * @overload
 */
template<typename Ptr>
Ptr clearMemory(Ptr ptr, uint32_t size)
{
    return reinterpret_cast<Ptr>(clearMemory(reinterpret_cast<void *>(ptr), size));
}

/**
 * Clears the data cache and instruction cache of memory starting at a specified address.
 *
 * @param ptr The pointer to the memory that will have its cache cleared.
 * @param size The amount of bytes that will have their cache cleared.
 */
inline void clear_DC_IC_Cache(void *ptr, uint32_t size)
{
    return asmClearDCICCache(ptr, size);
}

/**
 * Clears the data cache and instruction cache of memory starting at a specified address.
 *
 * @tparam ptr The pointer to the memory that will have its cache cleared.
 * @param size The amount of bytes that will have their cache cleared.
 *
 * @warning This function will not work correctly if a `const` pointer is passed in, so `const_cast` must be used if a `const`
 * pointer to some memory needs to have its cache cleared.
 *
 * @overload
 */
template<typename Ptr>
void clear_DC_IC_Cache(Ptr ptr, uint32_t size)
{
    return clear_DC_IC_Cache(reinterpret_cast<void *>(ptr), size);
}

#endif
