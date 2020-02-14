#include "patch.h"

#include <gc/OSCache.h>

#include <cstdint>

namespace mod::patch {

void clear_DC_IC_Cache(void *ptr, uint32_t size)
{
	gc::OSCache::DCFlushRange(ptr, size);
	gc::OSCache::ICInvalidateRange(ptr, size);
}

void writeStandardBranches(void *address, void functionStart(), void functionBranchBack())
{
	void *BranchBackAddress = reinterpret_cast<void *>(
		reinterpret_cast<uint32_t>(address) + 0x4);
	
	writeBranch(address, reinterpret_cast<void *>(functionStart));
	writeBranch(reinterpret_cast<void *>(functionBranchBack), BranchBackAddress);
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

void writeBranchMain(void *ptr, void *destination, uint32_t branch)
{
	uint32_t delta = reinterpret_cast<uint32_t>(destination) - reinterpret_cast<uint32_t>(ptr);
	
	branch |= (delta & 0x03FFFFFC);
	
	uint32_t *p = reinterpret_cast<uint32_t *>(ptr);
	*p = branch;
	
	clear_DC_IC_Cache(ptr, sizeof(uint32_t));
}

}