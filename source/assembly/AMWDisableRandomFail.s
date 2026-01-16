.global asmArbitraryMemoryWriteDisableRandomFail

asmArbitraryMemoryWriteDisableRandomFail:
stwu %sp,-0x10(%sp)
mflr %r3
stw %r3,0x14(%sp)

# Backup important register values
stw %r4,0x8(%sp)

mr %r3,%r0 # flags
bl cArbitraryMemoryWriteDisableRandomFail

# Restore the overwritten instruction
rlwinm. %r0,%r3,0,31,31

# Restore important register values
lwz %r4,0x8(%sp)

lwz %r3,0x14(%sp)
mtlr %r3
addi %sp,%sp,0x10
blr
