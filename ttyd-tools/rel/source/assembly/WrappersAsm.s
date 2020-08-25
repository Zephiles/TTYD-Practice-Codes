.global __wrap_sprintf
.global __wrap_strcmp
.global __wrap_strncmp
.global __wrap_strcpy
.global __wrap_strncpy
.global __wrap_strcat
.global __wrap_strlen
.global __wrap_memcpy
.global __wrap_memset
.global __wrap_sysMsec2Frame
.global __wrap_keyGetButton
.global __wrap_keyGetButtonTrg
.global __wrap_seqGetSeq
.global __wrap_seqGetNextSeq
.global __wrap_swByteGet
.global __wrap_swByteSet
.global __wrap_swClear
.global __wrap_swSet
.global __wrap_marioStGetSystemLevel
.global __wrap_marioStSystemLevel
.global __wrap_marioGetPartyId
.global __wrap_marioGetExtraPartyId
.global __wrap_marioGetPtr
.global __wrap_partyGetPtr
.global __wrap_pouchGetPtr
.global __wrap_btlGetScreenPoint
.global __wrap_evtGetWork
.global __wrap_eventStgNum
.global __wrap_eventStgDtPtr
.global __wrap_winGetPtr
.global __wrap_winOpenDisable
.global __wrap_winOpenEnable
.global __wrap_DCFlushRange
.global __wrap_ICInvalidateRange
.global __wrap___udivdi3
.global __wrap___umoddi3
.global __wrap_dispEntry
.global __wrap_FontDrawStart_alpha
.global __wrap_FontDrawEdge

__wrap_sprintf:
b __real_sprintf

__wrap_strcmp:
b __real_strcmp

__wrap_strncmp:
b __real_strncmp

__wrap_strcpy:
b __real_strcpy

__wrap_strncpy:
b __real_strncpy

__wrap_strcat:
b __real_strcat

__wrap_strlen:
b __real_strlen

__wrap_memcpy:
b __real_memcpy

__wrap_memset:
b __real_memset

__wrap_sysMsec2Frame:
b __real_sysMsec2Frame

__wrap_keyGetButton:
b __real_keyGetButton

__wrap_keyGetButtonTrg:
b __real_keyGetButtonTrg

__wrap_seqGetSeq:
b __real_seqGetSeq

__wrap_seqGetNextSeq:
b __real_seqGetNextSeq

__wrap_swByteGet:
b __real_swByteGet

__wrap_swByteSet:
b __real_swByteSet

__wrap_swClear:
b __real_swClear

__wrap_swSet:
b __real_swSet

__wrap_marioStGetSystemLevel:
b __real_marioStGetSystemLevel

__wrap_marioStSystemLevel:
b __real_marioStSystemLevel

__wrap_marioGetPartyId:
b __real_marioGetPartyId

__wrap_marioGetExtraPartyId:
b __real_marioGetExtraPartyId

__wrap_marioGetPtr:
b __real_marioGetPtr

__wrap_partyGetPtr:
b __real_partyGetPtr

__wrap_pouchGetPtr:
b __real_pouchGetPtr

__wrap_btlGetScreenPoint:
b __real_btlGetScreenPoint

__wrap_evtGetWork:
b __real_evtGetWork

__wrap_eventStgNum:
b __real_eventStgNum

__wrap_eventStgDtPtr:
b __real_eventStgDtPtr

__wrap_winGetPtr:
b __real_winGetPtr

__wrap_winOpenDisable:
b __real_winOpenDisable

__wrap_winOpenEnable:
b __real_winOpenEnable

__wrap_DCFlushRange:
b __real_DCFlushRange

__wrap_ICInvalidateRange:
b __real_ICInvalidateRange

__wrap___udivdi3:
b __real___udivdi3

__wrap___umoddi3:
b __real___umoddi3

__wrap_dispEntry:
b __real_dispEntry

__wrap_FontDrawStart_alpha:
b __real_FontDrawStart_alpha

__wrap_FontDrawEdge:
b __real_FontDrawEdge