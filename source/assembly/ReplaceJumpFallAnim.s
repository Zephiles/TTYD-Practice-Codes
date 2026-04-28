.global asmReplaceJumpFallAnim

asmReplaceJumpFallAnim:
stwu %sp,-0x10(%sp)
mflr %r3
stw %r3,0x14(%sp)

# Backup important register values
stw %r0,0x8(%sp)

mr %r3,%r25 # jumpFallString
bl cReplaceJumpFallAnim
mr %r4,%r3

# Restore important register values
lwz %r0,0x8(%sp)

lwz %r3,0x14(%sp)
mtlr %r3
addi %sp,%sp,0x10
blr
