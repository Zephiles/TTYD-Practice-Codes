#pragma once

#include <cstdint>

namespace ttyd::cardmgr {

extern "C" {

// memcard_open
// statusCallback
// readCallback
// writeCallback
// createCallback
// formatCallback
// checkCallback
// mountAttachCallback
// mountDetachCallback
// cardFormat
// create_main
// cardCreate
// read_all_main
// cardReadAll
// read_main
// write_header_main
// cardWriteHeader
// write_main
// cardWrite
// cardCopy
// cardErase
// cardCopy2Main
// cardMain
// cardGetCode
// cardIsExec
// cardInit
// cardBufReset
// cardGetFilePtr

extern void *cardMgrWorkPointer;

}

}