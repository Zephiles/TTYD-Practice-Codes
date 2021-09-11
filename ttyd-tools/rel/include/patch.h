#pragma once

#include <cstdint>

namespace mod::patch {

void clear_DC_IC_Cache(void *ptr, uint32_t size);
void writeBranch(void *ptr, void *destination);
void writeBranchBL(void *ptr, void *destination);
void writeBranchMain(void *ptr, void *destination, uint32_t branch);

template<typename Func>
void writeBranch_Template(void *ptr, Func destination)
{
    uint32_t branch = 0x48000000; // b
    writeBranchMain(ptr, reinterpret_cast<void *>(destination), branch);
}

template<typename Func>
void writeBranchBL_Template(void *ptr, Func destination)
{
    uint32_t branch = 0x48000001; // bl
    writeBranchMain(ptr, reinterpret_cast<void *>(destination), branch);
}

template<typename FuncStart, typename FuncBranchBack>
void writeStandardBranches(void *address, FuncStart funcStart, FuncBranchBack funcBranchBack)
{
    void *BranchBackAddress = reinterpret_cast<void *>(
        reinterpret_cast<uint32_t>(address) + 0x4);
    
    writeBranch(address, reinterpret_cast<void *>(funcStart));
    writeBranch(reinterpret_cast<void *>(funcBranchBack), BranchBackAddress);
}

template<typename Func, typename Dest>
Func hookFunction(Func function, Dest destination)
{
    uint32_t *instructions = reinterpret_cast<uint32_t *>(function);
    
    uint32_t *trampoline = new uint32_t[2];
    
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
    clear_DC_IC_Cache(instructions, sizeof(uint32_t));
    
    // Free the memory used by the trampoline
    delete[] (instructions);
    return nullptr;
}

}