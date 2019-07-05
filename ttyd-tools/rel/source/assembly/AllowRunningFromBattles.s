.global StartAllowRunningFromBattles

StartAllowRunningFromBattles:
stwu %sp,-0x10(%sp)
mflr %r0
stw %r0,0x14(%sp)

# r3 already contains ptr
bl allowRunningFromBattles
mr %r0,%r3

lwz %r3,0x14(%sp)
mtlr %r3
addi %sp,%sp,0x10
blr