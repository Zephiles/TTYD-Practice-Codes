.global asmJumpOnWater

asmJumpOnWater:
stwu %sp,-0x10(%sp)
mflr %r0
stw %r0,0x14(%sp)

# r3 already contains ptr
bl jumpOnWater

lwz %r0,0x14(%sp)
mtlr %r0
addi %sp,%sp,0x10

# Restore the overwritten instruction
cmplwi %r3,0
blr