#include "patch.h"
#include "misc/utils.h"

#include <cstdint>

static void writeBranchMain(void *ptr, void *destination, uint32_t branch)
{
    uint32_t delta = reinterpret_cast<uint32_t>(destination) - reinterpret_cast<uint32_t>(ptr);

    branch |= (delta & 0x03FFFFFC);

    uint32_t *p = reinterpret_cast<uint32_t *>(ptr);
    *p = branch;

    clear_DC_IC_Cache(ptr, sizeof(uint32_t));
}

void writeBranch(void *ptr, void *destination)
{
    uint32_t branch = 0x48000000; // b
    writeBranchMain(ptr, destination, branch);
}

void writeBranchBL(void *ptr, void *destination)
{
    uint32_t branch = 0x48000001; // bl
    writeBranchMain(ptr, destination, branch);
}

void writeStandardBranches(void *ptr, void *funcStart, void *funcEnd)
{
    // Write the main branch
    writeBranch(ptr, funcStart);

    // Write the returning branch
    writeBranch(funcEnd, reinterpret_cast<void *>(reinterpret_cast<uint32_t>(ptr) + 0x4));
}

void applyAssemblyPatch(void *address, uint32_t value)
{
    *reinterpret_cast<uint32_t *>(address) = value;
    clear_DC_IC_Cache(address, sizeof(uint32_t));
}
