.global asmOSAllocFromHeapGetHeapArrayPtrStart
.global asmOSAllocFromHeapGetHeapArrayPtrBranchBack
.global asmOSFreeToHeapGetHeapArrayPtr

asmOSAllocFromHeapGetHeapArrayPtrStart:
stwu %sp,-0x10(%sp)
mflr %r6
stw %r6,0x14(%sp)

# Backup important register values
stw %r4,0xC(%sp)

# r3 already contains heapArrayPtr
mr %r4,%r0 # handle
bl OS_getHeapArrayPtr
mr %r5,%r3 # New heapArrayPtr

# Restore important register values
lwz %r4,0xC(%sp)

lwz %r6,0x14(%sp)
mtlr %r6
addi %sp,%sp,0x10

asmOSAllocFromHeapGetHeapArrayPtrBranchBack:
b 0

asmOSFreeToHeapGetHeapArrayPtr:
stwu %sp,-0x10(%sp)
mflr %r5
stw %r5,0x14(%sp)

# Backup important register values
stw %r3,0xC(%sp)

mr %r3,%r4 # heapArrayPtr
mr %r4,%r0 # handle
bl OS_getHeapArrayPtr
mr %r31,%r3 # New heapArrayPtr

# Restore important register values
lwz %r3,0xC(%sp)

lwz %r5,0x14(%sp)
mtlr %r5
addi %sp,%sp,0x10
blr
