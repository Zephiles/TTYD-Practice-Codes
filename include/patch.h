#ifndef PATCH_H
#define PATCH_H

#include "cxx.h"
#include "misc/utils.h"

#include <cstdint>

void writeBranch(void *ptr, void *destination);
void writeBranchBL(void *ptr, void *destination);
void writeBranchMain(void *ptr, void *destination, uint32_t branch);
void writeStandardBranches(void *ptr, void *funcStart, void *funcEnd);
void applyAssemblyPatch(void *address, uint32_t value);

template<typename Func, typename Dest>
void writeBranchBL(Func func, Dest dst)
{
    writeBranchBL(reinterpret_cast<void *>(func), reinterpret_cast<void *>(dst));
}

template<typename Ptr, typename FuncStart, typename FuncEnd>
void writeStandardBranches(Ptr ptr, FuncStart funcStart, FuncEnd funcEnd)
{
    writeStandardBranches(reinterpret_cast<void *>(ptr),
                          reinterpret_cast<void *>(funcStart),
                          reinterpret_cast<void *>(funcEnd));
}

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

template<typename Ptr>
void applyAssemblyPatch(Ptr address, uint32_t value)
{
    applyAssemblyPatch(reinterpret_cast<void *>(address), value);
}

#endif
