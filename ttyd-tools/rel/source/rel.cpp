#include "rel_shrink.h"

#include <gc/OSModule.h>

extern "C" {

typedef void (*PFN_voidfunc)();
__attribute__((section (".ctors"))) extern PFN_voidfunc _ctors_start[];
__attribute__((section (".ctors"))) extern PFN_voidfunc _ctors_end[];
__attribute__((section (".dtors"))) extern PFN_voidfunc _dtors_start[];
__attribute__((section (".dtors"))) extern PFN_voidfunc _dtors_end[];

void _prolog();
void _epilog();
void _unresolved();

}

namespace mod {

extern void main();

}

void _prolog()
{
    // Free the memory used by the relocation data of this project's REL file, as this REL file shouldn't ever be unlinked
    gc::OSModule::OSModuleInfo *rel = *reinterpret_cast<gc::OSModule::OSModuleInfo **>(0x80004150); // Stored by the REL Loader code
    MakeRelFixed(0, rel);
    
    // Run global constructors
    for (PFN_voidfunc *ctor = _ctors_start; ctor != _ctors_end && *ctor; ++ctor)
    {
        (*ctor)();
    }
    
    // Run mod main
    mod::main();
}

void _epilog()
{
    // In the unlikely event we ever get here, run the global destructors
    for (PFN_voidfunc *dtor = _dtors_start; dtor != _dtors_end && *dtor; ++dtor)
    {
        (*dtor)();
    }
}

void _unresolved(void)
{
    
}