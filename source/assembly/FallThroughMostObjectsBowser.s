.global asmFallThroughMostObjectsBowser

asmFallThroughMostObjectsBowser:
stwu %sp,-0x10(%sp)
mflr %r0
stw %r0,0x14(%sp)

# r3 already contains ptr
bl fallThroughMostObjects

# Restore the overwritten instruction
mr. %r24,%r3

lwz %r0,0x14(%sp)
mtlr %r0
addi %sp,%sp,0x10
blr