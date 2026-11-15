#ifndef TTYD_FILEMGR_H
#define TTYD_FILEMGR_H

#include <cstdint>

extern "C"
{
    // fileSetCurrentArchiveType
    // fileAsync
    // fileAsyncf
    // dvdReadDoneCallBack
    // fileFree
    void *_fileAlloc(const char *fileName, int32_t type, void (*callback)(void *));
    // fileAlloc
    // fileAllocf
    // _fileGarbage
    // fileGarbageMoveMem
    // fileGarbageDataAdrSet
    // fileGarbageDataAdrClear
    // fileInit
}

#endif
