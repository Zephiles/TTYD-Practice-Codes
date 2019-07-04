#pragma once

#include <cstdint>

namespace ttyd::evtmgr {

struct EvtWork
{
	uint32_t maxEventCount;
	uint32_t gwData[32];
	uint32_t gfData[3]; // 96 flags total
	void *pContextBuffer; // EvtContext *pContextBuffer;
	uint32_t unk_94;
	uint64_t currentEventTime;
} __attribute__((__packed__));

extern "C" {

// evtGetPtrID
// evtGetPtr
// evtStartOther
// evtStopOther
// evtStartAll
// evtStopAll
// evtStartID
// evtStopID
// evtStart
// evtStop
// evtSetType
// evtSetSpeed
// evtSetPri
bool evtCheckID(uint32_t threadID);
// evtDeleteID
// evtDelete
// evtmgrMain
// evtRestart
// evtBrotherEntry
// evtChildEntry
void *evtEntryType(void *event, uint8_t priority, uint32_t flags, uint32_t type);
void *evtEntry(void *event, uint8_t priority, uint32_t flags);
// evtEntryRunCheck
// evtmgrReInit
// evtmgrInit
// make_pri_table
EvtWork *evtGetWork();

}

}