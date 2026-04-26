.global asmClearDCICCache
.set HID0,1008

asmClearDCICCache:
stwu %sp,-0x10(%sp)
mflr %r0
stw %r0,0x14(%sp)
stw %r3,0x8(%sp)
stw %r4,0xC(%sp)

# Use DCFlushRangeNoSync to avoid using the system call interrupt
bl DCFlushRangeNoSync

# Implement the assembly used for the system call interrupt
mfspr %r3,HID0
ori %r4,%r3,0x8
mtspr HID0,%r4
isync
sync
mtspr HID0,%r3

lwz %r3,0x8(%sp)
lwz %r4,0xC(%sp)
lwz %r0,0x14(%sp)
mtlr %r0
addi %sp,%sp,0x10
b ICInvalidateRange
