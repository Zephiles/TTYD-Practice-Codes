#ifndef PATCH_H
#define PATCH_H

#include "cxx.h"
#include "misc/utils.h"

#include <cstdint>

/**
 * Writes a `b` branch at a function/address that goes to a function/address.
 *
 * @param ptr The pointer to the function/address that the `b` branch will go to.
 * @param destination The function/address where the `b` branch will be placed.
 */
void writeBranch(void *ptr, void *destination);

/**
 * Writes a `bl` branch at a function/address that goes to a function/address.
 *
 * @param ptr The pointer to the function/address that the `bl` branch will go to.
 * @param destination The function/address where the `bl` branch will be placed.
 */
void writeBranchBL(void *ptr, void *destination);

/**
 * Writes a `bl` branch at an address that goes to a function.
 *
 * @tparam func The function that the `bl` branch will go to.
 * @tparam dst The address where the `bl` branch will be placed.
 *
 * @overload
 */
template<typename Func, typename Dest>
void writeBranchBL(Func func, Dest dst)
{
    writeBranchBL(reinterpret_cast<void *>(func), reinterpret_cast<void *>(dst));
}

/**
 * Writes a `b` branch at a function/address that goes to a function, and then branches back with a `b` branch at the end of the
 * function to just after the original function/address.
 *
 * @param ptr The pointer for where the first `b` branch will be placed.
 * @param funcStart The function that the first `b` branch will go to.
 * @param funcEnd The end of the function that branches back via a `b` branch to just after the original function/address.
 *
 * @warning `funcEnd` must be at the end of the starting function specified by `funcStart`.
 */
void writeStandardBranches(void *ptr, void *funcStart, void *funcEnd);

/**
 * Writes a `b` branch at an address that goes to a function, and then branches back with a `b` branch at the end of the
 * function to just after the address.
 *
 * @tparam ptr The pointer for where the first `b` branch will be placed.
 * @tparam funcStart The function that the first `b` branch will go to.
 * @tparam funcEnd The end of the function that branches back via a `b` branch to just after the address.
 *
 * @warning `funcEnd` must be at the end of the starting function specified by `funcStart`.
 *
 * @overload
 */
template<typename Ptr, typename FuncStart, typename FuncEnd>
void writeStandardBranches(Ptr ptr, FuncStart funcStart, FuncEnd funcEnd)
{
    writeStandardBranches(reinterpret_cast<void *>(ptr),
                          reinterpret_cast<void *>(funcStart),
                          reinterpret_cast<void *>(funcEnd));
}

/**
 * Writes a `b` branch at the start of a function that goes to a different function.
 *
 * @tparam function The function that the `b` branch will go to.
 * @tparam destination The function where the `b` branch will be placed, in which the first instruction will be overwritten.
 *
 * @returns A pointer to the dynamic memory that contains the backed-up original instruction and the `b` branch that goes to
 * just after the start of the original function.
 *
 * @note This function effectively allows one function to be overwritten with another.
 *
 * @note The first instruction of the original function will be stored in dynamic memory, and a `b` branch that goes to just
 * after the start of the original function will be placed in dynamic memory directly after the backed-up first instruction.
 *
 * @note The returned pointer is intended to be used as a function pointer, which allows for calling the original function.
 */
template<typename Func, typename Dest>
Func hookFunction(Func function, Dest destination)
{
    uint32_t *instructions = reinterpret_cast<uint32_t *>(function);
    uint32_t *trampoline = new (true) uint32_t[2];

    // Original instruction
    trampoline[0] = instructions[0];
    clear_DC_IC_Cache(&trampoline[0], sizeof(uint32_t));

    // Branch to original function past hook
    writeBranch(&trampoline[1], &instructions[1]);

    // Write actual hook
    writeBranch(&instructions[0], reinterpret_cast<void *>(static_cast<Func>(destination)));

    return reinterpret_cast<Func>(trampoline);
}

/**
 * Undos the work done via `hookFunction`: Restores the first instruction from the dynamic memory, and then frees the dynamic
 * memory.
 *
 * @tparam trampoline The pointer to the dynamic memory that contains the backed-up original instruction and the `b` branch that
 * goes to just after the start of the original function.
 *
 * @returns `nullptr`.
 */
template<typename Func>
Func unhookFunction(Func trampoline)
{
    if (!trampoline)
    {
        return nullptr;
    }

    uint32_t *instructions = reinterpret_cast<uint32_t *>(trampoline);

    // Restore the original instruction
    int32_t branchLength = instructions[1] & 0x03FFFFFC;

    // Check if this is a negative branch
    if (branchLength > 0x01FFFFFC)
    {
        const int32_t Width = 26;
        const int32_t Mask = (1 << (Width - 1));
        branchLength = (branchLength ^ Mask) - Mask - 0x4;
    }

    uint32_t instructionAddress = reinterpret_cast<uint32_t>(&instructions[1]);
    uint32_t *address = reinterpret_cast<uint32_t *>(instructionAddress + branchLength);
    *address = instructions[0];

    // Clear the cache for both the address and where the instructions were stored
    clear_DC_IC_Cache(address, sizeof(uint32_t));
    clear_DC_IC_Cache(instructions, sizeof(uint32_t) * 2);

    // Free the memory used by the trampoline
    delete[] instructions;
    return nullptr;
}

/**
 * Writes a `uint32_t` value to an address, and then clears the data cache and instruction cache for the address.
 *
 * @param address The address where the value will be written.
 * @param value The value to write.
 */
void applyAssemblyPatch(void *address, uint32_t value);

/**
 * Writes a `uint32_t` value to an address, and then clears the data cache and instruction cache for the address.
 *
 * @tparam address The address where the value will be written.
 * @param value The value to write.
 *
 * @overload
 */
template<typename Ptr>
void applyAssemblyPatch(Ptr address, uint32_t value)
{
    applyAssemblyPatch(reinterpret_cast<void *>(address), value);
}

#endif
