.global StartAllowRunningFromBattles

StartAllowRunningFromBattles:
stwu %sp,-0x8(%sp)
mflr %r0
stw %r0,0xC(%sp)

# r3 already contains ptr
bl allowRunningFromBattles
mr %r0,%r3

lwz %r3,0xC(%sp)
mtlr %r3
addi %sp,%sp,0x8
blr