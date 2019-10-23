#include <gc/card.h>
#include <ttyd/seqdrv.h>
#include <ttyd/party.h>
#include <ttyd/mario.h>
#include <ttyd/evtmgr.h>
#include <ttyd/event.h>

#include <cstdint>
#include <cstdio>
#include <cstdarg>
#include <cstring>

extern "C"
{
	// Real functions
	int32_t __real_strcmp(const char *lhs, const char *rhs);
	int32_t __real_strncmp(const char *lhs, const char *rhs, size_t count);
	char *__real_strcpy(char *dest, const char *src);
	char *__real_strncpy(char *dest, const char *src, std::size_t count);
	char *__real_strcat(char *dest, const char *src);
	std::size_t __real_strlen(const char *str);
	void *__real_memcpy(char *dest, const void *src, std::size_t count);
	void *__real_memset(char *dest, int32_t ch, std::size_t count);
	uint32_t __real_sysMsec2Frame(uint32_t milliseconds);
	uint32_t __real_keyGetButton(uint32_t padId);
	uint32_t __real_keyGetButtonTrg(uint32_t padId);
	ttyd::seqdrv::SeqIndex __real_seqGetSeq();
	ttyd::seqdrv::SeqIndex __real_seqGetNextSeq();
	uint32_t __real_swByteGet(uint32_t gsw);
	void __real_swByteSet(uint32_t gsw, uint32_t value);
	void __real_swClear(uint32_t gswf);
	void __real_swSet(uint32_t gswf);
	uint32_t __real_marioStGetSystemLevel();
	void __real_marioStSystemLevel(uint32_t level);
	ttyd::party::PartySlotId __real_marioGetPartyId();
	ttyd::party::PartySlotId __real_marioGetExtraPartyId();
	ttyd::mario::Player *__real_marioGetPtr();
	void *__real_partyGetPtr(ttyd::party::PartySlotId partySlotId);
	void *__real_pouchGetPtr();
	void __real_btlGetScreenPoint(float pos[3], float posOut[3]);
	ttyd::evtmgr::EvtWork *__real_evtGetWork();
	int32_t __real_eventStgNum();
	ttyd::event::EventStageDescription *__real_eventStgDtPtr(int32_t stageId);
	void *__real_winGetPtr();
	void __real_winOpenEnable();
	int32_t __real_CARDClose(gc::card::CARDFileInfo *fileInfo);
	void __real_DCFlushRange(void *startAddr, uint32_t nBytes);
	void __real_ICInvalidateRange(void *startAddr, uint32_t nBytes);
	uint64_t __real___udivdi3(uint64_t a, uint64_t b);
	uint64_t __real___umoddi3(uint64_t a, uint64_t b);
	
	// Wrap functions
	// sprintf
	__attribute__((noinline)) int32_t __wrap_sprintf(char *buffer, const char *format, ...)
	{
		va_list Args;
		va_start(Args, format);
		int32_t Length = vsprintf(buffer, format, Args);
		va_end(Args);
		return Length;
	}
	
	// strcmp
	__attribute__((noinline)) int32_t __wrap_strcmp(const char *lhs, const char *rhs)
	{
		return __real_strcmp(lhs, rhs);
	}
	
	// strncmp
	__attribute__((noinline)) int32_t __wrap_strncmp(const char *lhs, const char *rhs, size_t count)
	{
		return __real_strncmp(lhs, rhs, count);
	}
	
	// strcpy
	__attribute__((noinline)) char *__wrap_strcpy(char *dest, const char *src)
	{
		return __real_strcpy(dest, src);
	}
	
	// strncpy
	__attribute__((noinline)) char *__wrap_strncpy(char *dest, const char *src, std::size_t count)
	{
		return __real_strncpy(dest, src, count);
	}
	
	// strcat
	__attribute__((noinline)) char *__wrap_strcat(char *dest, const char *src)
	{
		return __real_strcat(dest, src);
	}
	
	// strlen
	__attribute__((noinline)) std::size_t __wrap_strlen(const char *str)
	{
		return __real_strlen(str);
	}
	
	// memcpy
	__attribute__((noinline)) void *__wrap_memcpy(char *dest, const void *src, std::size_t count)
	{
		return __real_memcpy(dest, src, count);
	}
	
	// memset
	__attribute__((noinline)) void *__wrap_memset(char *dest, int32_t ch, std::size_t count)
	{
		return __real_memset(dest, ch, count);
	}
	
	// sysMsec2Frame
	__attribute__((noinline)) uint32_t __wrap_sysMsec2Frame(uint32_t milliseconds)
	{
		return __real_sysMsec2Frame(milliseconds);
	}
	
	// keyGetButton
	__attribute__((noinline)) uint32_t __wrap_keyGetButton(uint32_t padId)
	{
		return __real_keyGetButton(padId);
	}
	
	// keyGetButtonTrg
	__attribute__((noinline)) uint32_t __wrap_keyGetButtonTrg(uint32_t padId)
	{
		return __real_keyGetButtonTrg(padId);
	}
	
	// seqGetSeq
	__attribute__((noinline)) ttyd::seqdrv::SeqIndex __wrap_seqGetSeq()
	{
		return __real_seqGetSeq();
	}
	
	// seqGetNextSeq
	__attribute__((noinline)) ttyd::seqdrv::SeqIndex __wrap_seqGetNextSeq()
	{
		return __real_seqGetNextSeq();
	}
	
	// swByteGet
	__attribute__((noinline)) uint32_t __wrap_swByteGet(uint32_t gsw)
	{
		return __real_swByteGet(gsw);
	}
	
	// swByteSet
	__attribute__((noinline)) void __wrap_swByteSet(uint32_t gsw, uint32_t value)
	{
		__real_swByteSet(gsw, value);
	}
	
	// swClear
	__attribute__((noinline)) void __wrap_swClear(uint32_t gswf)
	{
		__real_swClear(gswf);
	}
	
	// swSet
	__attribute__((noinline)) void __wrap_swSet(uint32_t gswf)
	{
		__real_swSet(gswf);
	}
	
	// marioStGetSystemLevel
	__attribute__((noinline)) uint32_t __wrap_marioStGetSystemLevel()
	{
		return __real_marioStGetSystemLevel();
	}
	
	// marioStSystemLevel
	__attribute__((noinline)) void __wrap_marioStSystemLevel(uint32_t level)
	{
		__real_marioStSystemLevel(level);
	}
	
	// marioGetPartyId
	__attribute__((noinline)) ttyd::party::PartySlotId __wrap_marioGetPartyId()
	{
		return __real_marioGetPartyId();
	}
	
	// marioGetExtraPartyId
	__attribute__((noinline)) ttyd::party::PartySlotId __wrap_marioGetExtraPartyId()
	{
		return __real_marioGetExtraPartyId();
	}
	
	// marioGetPtr
	__attribute__((noinline)) ttyd::mario::Player *__wrap_marioGetPtr()
	{
		return __real_marioGetPtr();
	}
	
	// partyGetPtr
	__attribute__((noinline)) void *__wrap_partyGetPtr(ttyd::party::PartySlotId partySlotId)
	{
		return __real_partyGetPtr(partySlotId);
	}
	
	// pouchGetPtr
	__attribute__((noinline)) void *__wrap_pouchGetPtr()
	{
		return __real_pouchGetPtr();
	}
	
	// btlGetScreenPoint
	__attribute__((noinline)) void __wrap_btlGetScreenPoint(float pos[3], float posOut[3])
	{
		__real_btlGetScreenPoint(pos, posOut);
	}
	
	// evtGetWork
	__attribute__((noinline)) ttyd::evtmgr::EvtWork *__wrap_evtGetWork()
	{
		return __real_evtGetWork();
	}
	
	// eventStgNum
	__attribute__((noinline)) int32_t __wrap_eventStgNum()
	{
		return __real_eventStgNum();
	}
	
	// eventStgDtPtr
	ttyd::event::EventStageDescription *__wrap_eventStgDtPtr(int32_t stageId)
	{
		return __real_eventStgDtPtr(stageId);
	}
	
	// winGetPtr
	__attribute__((noinline)) void *__wrap_winGetPtr()
	{
		return __real_winGetPtr();
	}
	
	// winOpenEnable
	__attribute__((noinline)) void __wrap_winOpenEnable()
	{
		__real_winOpenEnable();
	}
	
	// CARDClose
	__attribute__((noinline)) int32_t __wrap_CARDClose(gc::card::CARDFileInfo *fileInfo)
	{
		return __real_CARDClose(fileInfo);
	}
	
	// DCFlushRange
	__attribute__((noinline)) void __wrap_DCFlushRange(void *startAddr, uint32_t nBytes)
	{
		__real_DCFlushRange(startAddr, nBytes);
	}
	
	// ICInvalidateRange
	__attribute__((noinline)) void __wrap_ICInvalidateRange(void *startAddr, uint32_t nBytes)
	{
		__real_ICInvalidateRange(startAddr, nBytes);
	}
	
	// __udivdi3
	__attribute__((noinline)) uint64_t __wrap___udivdi3(uint64_t a, uint64_t b)
	{
		return __real___udivdi3(a, b);
	}
	
	// __umoddi3
	__attribute__((noinline)) uint64_t __wrap___umoddi3(uint64_t a, uint64_t b)
	{
		return __real___umoddi3(a, b);
	}
}
