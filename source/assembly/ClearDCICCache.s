.global asmClearDCICCache

asmClearDCICCache:
stwu %sp,-0x10(%sp)
mflr %r0
stw %r0,0x14(%sp)
stw %r3,0x8(%sp)
stw %r4,0xC(%sp)

# Use DCFlushRangeNoSync to avoid using the system call interrupt
bl DCFlushRangeNoSync
sync
isync # Shouldn't be needed but the system call interrupt would do so anyway

lwz %r3,0x8(%sp)
lwz %r4,0xC(%sp)
lwz %r0,0x14(%sp)
mtlr %r0
addi %sp,%sp,0x10
b ICInvalidateRange
