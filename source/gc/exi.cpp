#include "gc/exi.h "
#include "gc/os.h"

#include <cstdint>

void EXIProbeReset()
{
    int32_t *exiProbeStartTimePtr = &EXIProbeStartTime[0];
    exiProbeStartTimePtr[0] = 0;
    exiProbeStartTimePtr[1] = 0;

    EXIControl *ecbPtr = &Ecb[0];
    ecbPtr[0].idTime = 0;
    ecbPtr[1].idTime = 0;

    __EXIProbe(0);
    __EXIProbe(1);
}
