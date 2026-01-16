.global asmArbitraryMemoryWriteGetProperPointer

asmArbitraryMemoryWriteGetProperPointer:
stwu %sp,-0x10(%sp)
mflr %r4
stw %r4,0x14(%sp)

# Backup important register values
stw %r0,0x8(%sp)

# r3 already contains pointerRaw
bl cArbitraryMemoryWriteGetProperPointer

# Restore important register values
lwz %r0,0x8(%sp)

# Restore the overwritten instruction
add %r31,%r3,%r0

lwz %r0,0x14(%sp)
mtlr %r0
addi %sp,%sp,0x10
blr
