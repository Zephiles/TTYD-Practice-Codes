.global StartDisableBattles

StartDisableBattles:
stwu %sp,-0x10(%sp)
mflr %r0
stw %r0,0x14(%sp)

mr %r3,%r30 # ptr
bl disableBattles
mr %r21,%r3

lwz %r0,0x14(%sp)
mtlr %r0
addi %sp,%sp,0x10

# Restore the overwritten CR value(s)
rlwinm. %r26,%r19,0,27,27
blr