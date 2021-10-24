#include <gc/exi.h>

namespace gc::exi {

void EXIProbeReset()
{
    uint32_t *EXIVarsPtr = reinterpret_cast<uint32_t *>(0x800030C0);
    EXIVarsPtr[0] = 0;
    EXIVarsPtr[1] = 0;
    
    ECB *EcbPtr = &Ecb;
    EcbPtr->unk_20 = 0;
    EcbPtr->unk_60 = 0;
    
    __EXIProbe(0);
    __EXIProbe(1);
}

}