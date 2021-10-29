#include <gc/exi.h>
#include <gc/os.h>

namespace gc::exi {

void EXIProbeReset()
{
    int32_t *EXIProbeStartTimePtr = &gc::os::EXIProbeStartTime[0];
    EXIProbeStartTimePtr[0] = 0;
    EXIProbeStartTimePtr[1] = 0;
    
    EXIControl *EcbPtr = &Ecb[0];
    EcbPtr[0].idTime = 0;
    EcbPtr[1].idTime = 0;
    
    __EXIProbe(0);
    __EXIProbe(1);
}

}