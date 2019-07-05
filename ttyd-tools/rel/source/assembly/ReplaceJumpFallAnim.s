.global StartReplaceJumpFallAnim

StartReplaceJumpFallAnim:
stwu %sp,-0x10(%sp)
mflr %r3
stw %r3,0x14(%sp)
stw %r31,0xC(%sp)
mr %r31,%r0

mr %r3,%r25 # jumpFallString
bl replaceJumpFallAnim
mr %r4,%r3

mr %r0,%r31
lwz %r31,0xC(%sp)
lwz %r3,0x14(%sp)
mtlr %r3
addi %sp,%sp,0x10
blr