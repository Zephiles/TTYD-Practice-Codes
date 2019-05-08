.global StartDisableDPadOptionsDisplay
.global BranchBackDisableDPadOptionsDisplay

StartDisableDPadOptionsDisplay:
stwu %sp,-0x10(%sp)
stmw %r30,0x8(%sp)
mr %r30,%r3
mr %r31,%r4

mr %r3,%r0 # unkVar
bl disableDPadOptionsDisplay

# Check the returned bool
cmpwi %r3,0 # False

mr %r3,%r30
mr %r4,%r31
lmw %r30,0x8(%sp)
addi %sp,%sp,0x10

BranchBackDisableDPadOptionsDisplay:
b 0